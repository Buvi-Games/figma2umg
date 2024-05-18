// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/FigmaFile.h"

#include "Figma2UMGModule.h"
#include "Async/Async.h"
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

FString UFigmaFile::FindComponentSetName(const FString& ComponentSetId)
{
	if (ComponentSets.Contains(ComponentSetId))
	{
		const FFigmaComponentSetRef& ComponentSet = ComponentSets[ComponentSetId];
		return ComponentSet.Name;
	}

	return FString();
}

FFigmaComponentSetRef* UFigmaFile::FindComponentSetRef(const FString& ComponentSetId)
{
	if (ComponentSets.Contains(ComponentSetId))
	{
		FFigmaComponentSetRef& ComponentSet = ComponentSets[ComponentSetId];
		return &ComponentSet;
	}

	return nullptr;
}

TObjectPtr<UFigmaComponentSet> UFigmaFile::FindComponentSetByKey(const FString& Key)
{
	for (const TPair<FString, FFigmaComponentSetRef>& Element : ComponentSets)
	{
		if (Element.Value.Remote)
			continue;

		if (Element.Value.Key == Key)
		{
			return FindByID<UFigmaComponentSet>(Element.Key);
		}
	}

	return nullptr;
}

void UFigmaFile::PrepareForFlow()
{
	if (!Document)
		return;

	Document->PrepareForFlow();
}

void UFigmaFile::FixComponentSetRef()
{
	for (TPair<FString, FFigmaComponentRef>& ComponentPair : Components)
	{
		if (ComponentPair.Value.ComponentSetId.IsEmpty())
			continue;

		if (ComponentSets.Contains(ComponentPair.Value.ComponentSetId))
		{
			ComponentPair.Value.SetComponentSet(&ComponentSets[ComponentPair.Value.ComponentSetId]);
		}
		else
		{
			UE_LOG_Figma2UMG(Error, TEXT("File %s's Component %s is part of a ComponentSet %i not found."), *Name, *ComponentPair.Value.Name, *ComponentPair.Value.ComponentSetId);
		}
	}
}

void UFigmaFile::FixRemoteReferences(const TMap<FString, TObjectPtr<UFigmaFile>>& LibraryFiles)
{
	FixRemoteComponentSetReferences(LibraryFiles);
	FixRemoteComponentReferences(LibraryFiles);
}

void UFigmaFile::FixRemoteComponentReferences(const TMap<FString, TObjectPtr<UFigmaFile>>& LibraryFiles)
{
	TMap<FString, FFigmaComponentRef> PendingComponents;
	for (TPair<FString, FFigmaComponentRef>& ComponentPair : Components)
	{
		if (!ComponentPair.Value.Remote)
			continue;

		if (ComponentPair.Value.GetComponent() != nullptr)
			continue;

		for (const TPair<FString, TObjectPtr<UFigmaFile>> LibraryFile : LibraryFiles)
		{
			TObjectPtr<UFigmaComponent> Component = LibraryFile.Value->FindComponentByKey(ComponentPair.Value.Key);
			if (Component != nullptr)
			{
				AddRemoteComponent(ComponentPair.Value, LibraryFile, Component, PendingComponents);
				break;
			}
		}
	}

	if (PendingComponents.Num() > 0)
	{
		for (TPair<FString, FFigmaComponentRef>& Element : PendingComponents)
		{
			Element.Value.Remote = true;
			Components.Add(Element);
		}

		FixRemoteComponentReferences(LibraryFiles);
	}
}

void UFigmaFile::FixRemoteComponentSetReferences(const TMap<FString, TObjectPtr<UFigmaFile>>& LibraryFiles)
{
	TMap<FString, FFigmaComponentRef> PendingComponents;
	TMap<FString, FFigmaComponentSetRef> PendingComponentSets;
	for (TPair<FString, FFigmaComponentSetRef>& ComponentSetPair : ComponentSets)
	{
		if (!ComponentSetPair.Value.Remote)
			continue;

		if (ComponentSetPair.Value.GetComponentSet() != nullptr)
			continue;

		for (const TPair<FString, TObjectPtr<UFigmaFile>> LibraryFile : LibraryFiles)
		{
			TObjectPtr<UFigmaComponentSet> Component = LibraryFile.Value->FindComponentSetByKey(ComponentSetPair.Value.Key);
			if (Component != nullptr)
			{
				AddRemoteComponentSet(ComponentSetPair.Value, LibraryFile, Component, PendingComponents, PendingComponentSets);
				break;
			}
		}
	}

	if (PendingComponents.Num() > 0)
	{
		for (TPair<FString, FFigmaComponentRef>& Element : PendingComponents)
		{
			Element.Value.Remote = true;
			Components.Add(Element);
		}
	}

	if (PendingComponentSets.Num() > 0)
	{
		for (TPair<FString, FFigmaComponentSetRef>& Element : PendingComponentSets)
		{
			Element.Value.Remote = true;
			ComponentSets.Add(Element);
		}

		FixRemoteComponentSetReferences(LibraryFiles);
	}
}

void UFigmaFile::LoadOrCreateAssets(const FProcessFinishedDelegate& ProcessDelegate)
{
	CurrentProcessDelegate = ProcessDelegate;

	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			for (TPair<FString, FFigmaComponentSetRef>& ComponentSetPair : ComponentSets)
			{
				if (!ComponentSetPair.Value.Remote)
					continue;

				TObjectPtr<UFigmaComponentSet> ComponentSet = ComponentSetPair.Value.GetComponentSet();
				if (ComponentSet)
				{
					ComponentSet->LoadOrCreateAssets(this);
					UWidgetBlueprint* Asset = ComponentSet->GetAsset<UWidgetBlueprint>();
					TObjectPtr<UFigmaFile> OriginalFile = ComponentSet->GetFigmaFile();
					if (OriginalFile && OriginalFile->Components.Contains(ComponentSetPair.Key))
					{
						OriginalFile->ComponentSets[ComponentSetPair.Key].SetComponentSet(ComponentSet);
					}
				}
			}

			for (TPair<FString, FFigmaComponentRef>& ComponentPair : Components)
			{
				if (!ComponentPair.Value.Remote)
					continue;

				TObjectPtr<UFigmaComponent> Component = ComponentPair.Value.GetComponent();
				if (Component)
				{
					Component->LoadOrCreateAssets(this);
					UWidgetBlueprint* Asset = Component->GetAsset<UWidgetBlueprint>();
					TObjectPtr<UFigmaFile> OriginalFile = Component->GetFigmaFile();
					if (OriginalFile && OriginalFile->Components.Contains(ComponentPair.Key))
					{
						OriginalFile->Components[ComponentPair.Key].SetComponent(Component);
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
	for (TPair<FString, FFigmaComponentRef>& ComponentPair : Components)
	{
		if (!ComponentPair.Value.Remote)
			continue;

		if (ComponentPair.Value.RemoteFileKey.IsEmpty())
			continue;

		if (TObjectPtr<UFigmaComponent> Component = ComponentPair.Value.GetComponent())
		{
			TArray<IFigmaImageRequester*> ComponentImageRequests;
			Component->GetAllChildrenByType(ComponentImageRequests);
			for (IFigmaImageRequester* FileNode : ComponentImageRequests)
			{
				FileNode->AddImageRequest(ComponentPair.Value.RemoteFileKey, ImageRequests);
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

void UFigmaFile::Patch(const FProcessFinishedDelegate& ProcessDelegate, FScopedSlowTask* Progress)
{
	CurrentProcessDelegate = ProcessDelegate;
	AsyncTask(ENamedThreads::GameThread, [this, Progress]()
		{
			FGCScopeGuard GCScopeGuard;

			Progress->EnterProgressFrame(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PatchPreInsertWidget", "Patch PreInsert Widgets"));
			PatchPreInsertWidget();

			Progress->EnterProgressFrame(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PatchPreInsertWidget", "Patch PostInsert Widgets"));
			if (PatchPostInsertWidget())
			{
				Progress->EnterProgressFrame(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PatchPreInsertWidget", "Compiling BluePrints"));
				CompileBPs();

				Progress->EnterProgressFrame(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PatchPreInsertWidget", "Reloading compiled BluePrints"));
				ReloadBPAssets();

				Progress->EnterProgressFrame(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PatchPreInsertWidget", "Patching Widget Binds"));
				PatchWidgetBinds();

				Progress->EnterProgressFrame(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PatchPreInsertWidget", "Patching Widget Properties"));
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
			for (TPair<FString, FFigmaComponentSetRef>& ComponentSetPair : ComponentSets)
			{
				if (!ComponentSetPair.Value.Remote)
					continue;

				if (TObjectPtr<UFigmaComponentSet> ComponentSet = ComponentSetPair.Value.GetComponentSet())
				{
					ComponentSet->PostPatchWidget();
				}
			}

			for (TPair<FString, FFigmaComponentRef>& ComponentPair : Components)
			{
				if (!ComponentPair.Value.Remote)
					continue;

				if (TObjectPtr<UFigmaComponent> Component = ComponentPair.Value.GetComponent())
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

void UFigmaFile::AddRemoteComponent(FFigmaComponentRef& ComponentRef, const TPair<FString, TObjectPtr<UFigmaFile>>& LibraryFile, TObjectPtr<UFigmaComponent> Component, TMap<FString, FFigmaComponentRef>& PendingComponents)
{
	UE_LOG_Figma2UMG(Display, TEXT("[Component] Adding remote Component %s key:%s"), *ComponentRef.Name, *ComponentRef.Key);

	ComponentRef.RemoteFileKey = LibraryFile.Key;
	ComponentRef.SetComponent(Component);
	if (!ComponentRef.ComponentSetId.IsEmpty())
	{
		if (FFigmaComponentSetRef* RemoteComponentSet = LibraryFile.Value->FindComponentSetRef(ComponentRef.ComponentSetId))
		{
			ComponentRef.SetComponentSet(RemoteComponentSet);
		}
		else if (FFigmaComponentSetRef* ComponentSet = this->FindComponentSetRef(ComponentRef.ComponentSetId))
		{
			ComponentRef.SetComponentSet(ComponentSet);
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[Component] Can't find ComponentSet id %s"), *ComponentRef.ComponentSetId);
		}
	}

	TArray<UFigmaInstance*> SubInstances;
	Component->GetAllChildrenByType(SubInstances);

	for (UFigmaInstance* SubInstance : SubInstances)
	{
		if (PendingComponents.Contains(SubInstance->GetComponentId()))
			continue;

		if (LibraryFile.Value->Components.Contains(SubInstance->GetComponentId()))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[Component] Adding dependency to Component %s id %s"), *SubInstance->GetNodeName(), *SubInstance->GetComponentId());
			FFigmaComponentRef& RemoteCommponentRef = LibraryFile.Value->Components[SubInstance->GetComponentId()];
			if (!RemoteCommponentRef.Remote)
			{
				RemoteCommponentRef.RemoteFileKey = LibraryFile.Key;
			}
			FFigmaComponentRef& SubComponentRef = PendingComponents.Add(SubInstance->GetComponentId(), RemoteCommponentRef);
		}
	}
}

void UFigmaFile::AddRemoteComponentSet(FFigmaComponentSetRef& ComponentSetRef, const TPair<FString, TObjectPtr<UFigmaFile>>& LibraryFile, TObjectPtr<UFigmaComponentSet> ComponentSet, TMap<FString, FFigmaComponentRef>& PendingComponents, TMap<FString, FFigmaComponentSetRef>& PendingComponentSets)
{
	UE_LOG_Figma2UMG(Display, TEXT("[ComponentSet] Adding remote ComponentSet %s key:%s"), *ComponentSetRef.Name, *ComponentSetRef.Key);

	ComponentSetRef.RemoteFileKey = LibraryFile.Key;
	ComponentSetRef.SetComponentSet(ComponentSet);

	TArray<UFigmaComponent*> SubComponents;
	ComponentSet->GetAllChildrenByType(SubComponents);

	for (const UFigmaComponent* SubComponent : SubComponents)
	{
		if (PendingComponents.Contains(SubComponent->GetId()))
			continue;

		if (LibraryFile.Value->Components.Contains(SubComponent->GetId()))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[ComponentSet] Adding dependency to Component %s id %s"), *SubComponent->GetNodeName(), *SubComponent->GetId());
			FFigmaComponentRef& RemoteCommponentRef = LibraryFile.Value->Components[SubComponent->GetId()];
			if (!RemoteCommponentRef.Remote)
			{
				RemoteCommponentRef.RemoteFileKey = LibraryFile.Key;
			}
			FFigmaComponentRef& SubComponentRef = PendingComponents.Add(SubComponent->GetId(), RemoteCommponentRef);
		}
	}


	TArray<UFigmaComponentSet*> SubComponentSets;
	ComponentSet->GetAllChildrenByType(SubComponentSets);
	for (const UFigmaComponentSet* SubComponentSet : SubComponentSets)
	{
		if (PendingComponentSets.Contains(SubComponentSet->GetId()))
			continue;

		if (LibraryFile.Value->ComponentSets.Contains(SubComponentSet->GetId()))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[ComponentSet] Adding dependency to ComponentSet %s id %s"), *SubComponentSet->GetNodeName(), *SubComponentSet->GetId());
			FFigmaComponentSetRef& RemoteComponentSetRef = LibraryFile.Value->ComponentSets[SubComponentSet->GetId()];
			if (!RemoteComponentSetRef.Remote)
			{
				RemoteComponentSetRef.RemoteFileKey = LibraryFile.Key;
			}
			FFigmaComponentSetRef& SubComponentRef = PendingComponentSets.Add(SubComponentSet->GetId(), RemoteComponentSetRef);
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

void UFigmaFile::PatchPreInsertWidget()
{
	for (TPair<FString, FFigmaComponentSetRef>& ComponentSetPair : ComponentSets)
	{
		if (!ComponentSetPair.Value.Remote)
			continue;

		if (const TObjectPtr<UFigmaComponentSet> ComponentSet = ComponentSetPair.Value.GetComponentSet())
		{
			ComponentSet->PatchPreInsertWidget(nullptr);
		}
	}

	for (TPair<FString, FFigmaComponentRef>& ComponentPair : Components)
	{
		if (!ComponentPair.Value.Remote)
			continue;

		if (const TObjectPtr<UFigmaComponent> Component = ComponentPair.Value.GetComponent())
		{
			Component->PatchPreInsertWidget(nullptr);
		}
	}

	if (Document)
	{
		Document->PatchPreInsertWidget(nullptr);
	}
}

bool UFigmaFile::PatchPostInsertWidget()
{
	for (TPair<FString, FFigmaComponentSetRef>& ComponentSetPair : ComponentSets)
	{
		if (!ComponentSetPair.Value.Remote)
			continue;

		if (const TObjectPtr<UFigmaComponentSet> ComponentSet = ComponentSetPair.Value.GetComponentSet())
		{
			ComponentSet->PatchPostInsertWidget();
		}
	}

	for (TPair<FString, FFigmaComponentRef>& ComponentPair : Components)
	{
		if (!ComponentPair.Value.Remote)
			continue;

		if (const TObjectPtr<UFigmaComponent> Component = ComponentPair.Value.GetComponent())
		{
			Component->PatchPostInsertWidget();
		}
	}

	if (Document)
	{
		Document->PatchPostInsertWidget();
		return true;
	}
	return false;
}

void UFigmaFile::PatchWidgetBinds()
{
	for (TPair<FString, FFigmaComponentSetRef>& ComponentSetPair : ComponentSets)
	{
		TObjectPtr<UFigmaComponentSet> ComponentSet = ComponentSetPair.Value.GetComponentSet();
		TObjectPtr<UWidgetBlueprint> WidgetBP = ComponentSetPair.Value.GetAsset();
		if (!ComponentSet || !WidgetBP)
			continue;

		ComponentSet->PatchBluePrintBinds();
	}

	for (TPair<FString, FFigmaComponentRef>& ComponentPair : Components)
	{
		TObjectPtr<UFigmaComponent> Component = ComponentPair.Value.GetComponent();
		TObjectPtr<UWidgetBlueprint> WidgetBP = ComponentPair.Value.GetAsset();
		if (!Component || !WidgetBP)
			continue;

		Component->PatchBluePrintBinds();
	}

	TArray<UFigmaFrame*> AllFrames;
	if (Document)
	{
		Document->GetAllChildrenByType(AllFrames);
	}
	for (UFigmaFrame* FigmaFrame : AllFrames)
	{
		if(FigmaFrame->IsA<UFigmaComponent>() || FigmaFrame->IsA<UFigmaComponentSet>())
			continue;

		FigmaFrame->PatchBluePrintBinds();
	}
}

void UFigmaFile::PatchWidgetProperties() const
{
	TArray<UFigmaInstance*> AllInstances;
	for (const TPair<FString, FFigmaComponentSetRef>& ComponentSetPair : ComponentSets)
	{
		TObjectPtr<UFigmaComponentSet> ComponentSet = ComponentSetPair.Value.GetComponentSet();
		if (!ComponentSet || !ComponentSetPair.Value.Remote)
			continue;

		ComponentSet->GetAllChildrenByType(AllInstances);
	}

	for (const TPair<FString, FFigmaComponentRef>& ComponentPair : Components)
	{
		TObjectPtr<UFigmaComponent> Component = ComponentPair.Value.GetComponent();
		if (!Component || !ComponentPair.Value.Remote)
			continue;

		Component->GetAllChildrenByType(AllInstances);
	}

	if (Document)
	{
		Document->GetAllChildrenByType(AllInstances);
	}

	for (UFigmaInstance* FigmaInstance : AllInstances)
	{
		FigmaInstance->PatchComponentProperty();
	}
}

void UFigmaFile::CompileBPs()
{
	for (TPair<FString, FFigmaComponentRef>& ComponentRef : Components)
	{
		if (!ComponentRef.Value.Remote)
			continue;

		if (TObjectPtr<UFigmaComponent> Component = ComponentRef.Value.GetComponent())
		{
			Component->CompileBP(Component->GetNodeName());
		}
	}

	for (TPair<FString, FFigmaComponentSetRef>& ComponentSetRef : ComponentSets)
	{
		if (!ComponentSetRef.Value.Remote)
			continue;

		if (TObjectPtr<UFigmaComponentSet> ComponentSet = ComponentSetRef.Value.GetComponentSet())
		{
			ComponentSet->CompileBP(ComponentSet->GetNodeName());
		}
	}

	TArray<IFigmaFileHandle*> AllUAssets;
	if (Document)
	{
		AllUAssets.Add(Document);
		Document->GetAllChildrenByType(AllUAssets);
	}

	for (IFigmaFileHandle* FigmaInstance : AllUAssets)
	{
		FigmaInstance->CompileBP(Cast<UFigmaNode>(FigmaInstance)->GetNodeName());
	}
}

void UFigmaFile::ReloadBPAssets()
{
	ResetWidgets();
	LoadAssets();
	FindWidgets();
}

void UFigmaFile::ResetWidgets()
{
	TArray<IWidgetOwner*> AllWidgets;
	if (Document)
	{
		Document->GetAllChildrenByType(AllWidgets);
	}

	for (IWidgetOwner* FigmaInstance : AllWidgets)
	{
		FigmaInstance->Reset();
	}

	for (TPair<FString, FFigmaComponentRef>& ComponentRef : Components)
	{
		if (TObjectPtr<UFigmaComponent> Component = ComponentRef.Value.GetComponent())
		{
			Component->Reset();
		}
	}

	for (TPair<FString, FFigmaComponentSetRef>& ComponentSetRef : ComponentSets)
	{
		if (TObjectPtr<UFigmaComponentSet> ComponentSet = ComponentSetRef.Value.GetComponentSet())
		{
			ComponentSet->Reset();
		}
	}
}

void UFigmaFile::LoadAssets()
{
	TArray<IFigmaFileHandle*> AllUAssets;
	if (Document)
	{
		AllUAssets.Add(Document);
		Document->GetAllChildrenByType(AllUAssets);
	}

	for (IFigmaFileHandle* FigmaInstance : AllUAssets)
	{
		FigmaInstance->LoadAssets();
	}

	for (TPair<FString, FFigmaComponentRef>& ComponentRef : Components)
	{
		if (!ComponentRef.Value.Remote)
			continue;

		if (const TObjectPtr<UFigmaComponent> Component = ComponentRef.Value.GetComponent())
		{
			Component->LoadAssets();
		}
	}

	for (TPair<FString, FFigmaComponentSetRef>& ComponentSetRef : ComponentSets)
	{
		if (!ComponentSetRef.Value.Remote)
			continue;

		if (const TObjectPtr<UFigmaComponentSet> ComponentSet = ComponentSetRef.Value.GetComponentSet())
		{
			ComponentSet->LoadAssets();
		}
	}
}

void UFigmaFile::FindWidgets()
{
	for (TPair<FString, FFigmaComponentSetRef>& ComponentSetPair : ComponentSets)
	{
		if (!ComponentSetPair.Value.Remote)
			continue;

		if (const TObjectPtr<UFigmaComponentSet> ComponentSet = ComponentSetPair.Value.GetComponentSet())
		{
			ComponentSet->SetWidget(nullptr);
		}
	}

	for (TPair<FString, FFigmaComponentRef>& ComponentPair : Components)
	{
		if (!ComponentPair.Value.Remote)
			continue;

		if (const TObjectPtr<UFigmaComponent> Component = ComponentPair.Value.GetComponent())
		{
			Component->SetWidget(nullptr);
		}
	}

	if (Document)
	{
		Document->SetWidget(nullptr);
	}
}
