// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/FigmaFile.h"

#include "Interfaces/FigmaImageRequester.h"
#include "Nodes/FigmaDocument.h"
#include "Properties/FigmaComponentRef.h"
#include "REST/FigmaImporter.h"


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
				Element.Value.RemoteFileKey = LibraryFile.Key;
				Element.Value.SetComponent(Component);
				break;
			}
		}
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

void UFigmaFile::ExecuteDelegate(const bool Succeeded)
{
	if (CurrentProcessDelegate.ExecuteIfBound(Succeeded))
	{
//		CurrentProcessDelegate.Unbind();
	}
}
