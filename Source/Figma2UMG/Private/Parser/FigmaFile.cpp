// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/FigmaFile.h"

#include "Interfaces/FigmaImageRequester.h"
#include "Nodes/FigmaDocument.h"
#include "Nodes/FigmaInstance.h"
#include "Properties/FigmaComponentRef.h"


void UFigmaFile::PostSerialize(const FString& InPackagePath, const TSharedRef<FJsonObject> fileJsonObject)
{
	PackagePath = InPackagePath;
	if(Document)
	{
		Document->SetFigmaFile(this);
		Document->PostSerialize(nullptr, fileJsonObject->GetObjectField("Document").ToSharedRef());
	}
}

FString UFigmaFile::FindComponentName(const FString& ComponentId)
{
	if (Components.Contains(ComponentId))
	{
		const FFigmaComponentRef& Component = Components[ComponentId];
		return Component.Name;
	}

	return FString();
}

FFigmaComponentRef* UFigmaFile::FindComponentRef(const FString& ComponentId)
{
	if (Components.Contains(ComponentId))
	{
		FFigmaComponentRef& Component = Components[ComponentId];
		return &Component;
	}

	return nullptr;
}

TObjectPtr<UFigmaComponent> UFigmaFile::FindComponentByKey(const FString& Key)
{
	for (const TPair<FString, FFigmaComponentRef>& Element : Components)
	{
		if (Element.Value.Remote)
			continue;

		if (Element.Value.Key == Key)
		{
			return FindByID<UFigmaComponent>(Element.Key);
		}
	}

	return nullptr;
}

void UFigmaFile::FixRemoteReferences(const TMap<FString, TObjectPtr<UFigmaFile>>& LibraryFiles)
{
	TMap<FString, FFigmaComponentRef> PendingComponents;
	for (TPair<FString, FFigmaComponentRef>& Element : Components)
	{
		if (!Element.Value.Remote)
			continue;

		
		if(Element.Value.GetComponent() != nullptr)
			continue;

		for (const TPair<FString, TObjectPtr<UFigmaFile>> LibraryFile : LibraryFiles)
		{
			TObjectPtr<UFigmaComponent> Component = LibraryFile.Value->FindComponentByKey(Element.Value.Key);
			if (Component != nullptr)
			{
				AddRemoteComponent(Element.Value, LibraryFile, Component, PendingComponents);
				break;
			}
		}
	}

	if(PendingComponents.Num() > 0)
	{
		for (TPair<FString, FFigmaComponentRef>& Element : PendingComponents)
		{
			Element.Value.Remote = true;
			Components.Add(Element);
		}

		FixRemoteReferences(LibraryFiles);
	}
}

void UFigmaFile::LoadOrCreateAssets(const FProcessFinishedDelegate& ProcessDelegate)
{
	CurrentProcessDelegate = ProcessDelegate;

	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			for (TPair<FString, FFigmaComponentRef>& Element : Components)
			{
				if (!Element.Value.Remote)
					continue;

				TObjectPtr<UFigmaComponent> Component = Element.Value.GetComponent();
				if (Component)
				{
					Component->LoadOrCreateAssets(this);
					UWidgetBlueprint* Asset = Component->GetAsset<UWidgetBlueprint>();
					Element.Value.SetAsset(Asset);
					TObjectPtr<UFigmaFile> OriginalFile = Component->GetFigmaFile();
					if (OriginalFile && OriginalFile->Components.Contains(Element.Key))
					{
						OriginalFile->Components[Element.Key].SetAsset(Asset);
					}
				}
			}

			if (Document)
			{
						TArray<IFigmaFileHandle*> AllFiles;
						AllFiles.Add(Document);
						Document->GetAllChildrenByType(AllFiles);

						FGCScopeGuard GCScopeGuard;
						for (IFigmaFileHandle* FileNode : AllFiles)
						{
							FileNode->LoadOrCreateAssets(this);
						}

						ExecuteDelegate(true);
			}
			else
			{
				ExecuteDelegate(false);
			}
		});
}

void UFigmaFile::BuildImageDependency(FString FileKey, FImageRequests& ImageRequests)
{
	for (TPair<FString, FFigmaComponentRef>& Element : Components)
	{
		if (!Element.Value.Remote)
			continue;

		if (Element.Value.RemoteFileKey.IsEmpty())
			continue;

		if (TObjectPtr<UFigmaComponent> Component = Element.Value.GetComponent())
		{
			TArray<IFigmaImageRequester*> ComponentImageRequests;
			Component->GetAllChildrenByType(ComponentImageRequests);
			for (IFigmaImageRequester* FileNode : ComponentImageRequests)
			{
				FileNode->AddImageRequest(Element.Value.RemoteFileKey, ImageRequests);
			}
		}
	}

	TArray<IFigmaImageRequester*> AllImageRequests;
	if (Document)
	{
		Document->GetAllChildrenByType(AllImageRequests);
	}

	FGCScopeGuard GCScopeGuard;
	for (IFigmaImageRequester* FileNode : AllImageRequests)
	{
		FileNode->AddImageRequest(FileKey, ImageRequests);
	}
}

void UFigmaFile::Patch(const FProcessFinishedDelegate& ProcessDelegate)
{
	CurrentProcessDelegate = ProcessDelegate;
	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			for (TPair<FString, FFigmaComponentRef>& Element : Components)
			{
				if (!Element.Value.Remote)
					continue;

				if (TObjectPtr<UFigmaComponent> Component = Element.Value.GetComponent())
				{
					Component->PatchPreInsertWidget(nullptr);
					Component->PatchPostInsertWidget();
				}
			}

			if (Document)
			{
				Document->PatchPreInsertWidget(nullptr);
				Document->PatchPostInsertWidget();

				PatchWidgetBinds();
				PatchWidgetProperties();

				ExecuteDelegate(true);
			}
			else
			{
				ExecuteDelegate(false);
			}
		});
}

void UFigmaFile::PostPatch(const FProcessFinishedDelegate& ProcessDelegate)
{
	CurrentProcessDelegate = ProcessDelegate;
	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			for (TPair<FString, FFigmaComponentRef>& Element : Components)
			{
				if (!Element.Value.Remote)
					continue;

				if (TObjectPtr<UFigmaComponent> Component = Element.Value.GetComponent())
				{
					Component->PostPatchWidget();
				}
			}

			if (Document)
			{
				Document->PostPatchWidget();

				ExecuteDelegate(true);
			}
			else
			{
				ExecuteDelegate(false);
			}
		});
}

void UFigmaFile::AddRemoteComponent(FFigmaComponentRef& ComponentRef, const TPair<FString, TObjectPtr<UFigmaFile>> LibraryFile, TObjectPtr<UFigmaComponent> Component, TMap<FString, FFigmaComponentRef>& PendingComponents)
{
	ComponentRef.RemoteFileKey = LibraryFile.Key;
	ComponentRef.SetComponent(Component);

	TArray<UFigmaInstance*> SubInstances;
	Component->GetAllChildrenByType(SubInstances);

	for (UFigmaInstance* SubInstance : SubInstances)
	{
		if (PendingComponents.Contains(SubInstance->GetComponentId()))
			continue;

		if (LibraryFile.Value->Components.Contains(SubInstance->GetComponentId()))
		{
			FFigmaComponentRef& RemoteCommponentRef = LibraryFile.Value->Components[SubInstance->GetComponentId()];
			if (!RemoteCommponentRef.Remote)
			{
				RemoteCommponentRef.RemoteFileKey = LibraryFile.Key;
			}
			FFigmaComponentRef& SubCommponentRef = PendingComponents.Add(SubInstance->GetComponentId(), RemoteCommponentRef);
		}
	}
}

void UFigmaFile::ExecuteDelegate(const bool Succeeded)
{
	if (CurrentProcessDelegate.ExecuteIfBound(Succeeded))
	{
//		CurrentProcessDelegate.Unbind();
	}
}

void UFigmaFile::PatchWidgetBinds()
{
	for (TPair<FString, FFigmaComponentRef>& Element : Components)
	{
		TObjectPtr<UFigmaComponent> Component = Element.Value.GetComponent();
		TObjectPtr<UWidgetBlueprint> WidgetBP = Element.Value.GetAsset();
		if (!Component || !WidgetBP)
			continue;

		Component->PatchBinds();
	}
}


void UFigmaFile::PatchWidgetProperties()
{
	TArray<UFigmaInstance*> AllInstances;
	if (Document)
	{
		Document->GetAllChildrenByType(AllInstances);
	}

	for (UFigmaInstance* FigmaInstance : AllInstances)
	{
		FigmaInstance->PatchComponentProperty();
	}
}