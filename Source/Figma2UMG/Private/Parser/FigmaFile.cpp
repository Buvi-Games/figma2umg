// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/FigmaFile.h"

#include "Figma2UMGModule.h"
#include "Async/Async.h"
#include "Builder/Asset/WidgetBlueprintBuilder.h"
#include "Nodes/FigmaDocument.h"
#include "Nodes/FigmaInstance.h"
#include "Properties/FigmaComponentRef.h"


void UFigmaFile::PostSerialize(const FString& InFileKey, const FString& InPackagePath, const TSharedRef<FJsonObject> fileJsonObject)
{
	static FString DocumentStr("Document");
	FileKey = InFileKey;
	PackagePath = InPackagePath;
	if(Document)
	{
		Document->SetFigmaFile(this);
		Document->PostSerialize(nullptr, fileJsonObject->GetObjectField(DocumentStr).ToSharedRef());
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

	for (TPair<FString, FFigmaComponentSetRef>& ComponentSetPair : ComponentSets)
	{
		if (!ComponentSetPair.Value.Remote)
			continue;

		TObjectPtr<UFigmaComponentSet> ComponentSet = ComponentSetPair.Value.GetComponentSet();
		if (ComponentSet)
		{
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
			TObjectPtr<UFigmaFile> OriginalFile = Component->GetFigmaFile();
			if (OriginalFile && OriginalFile->Components.Contains(ComponentPair.Key))
			{
				OriginalFile->Components[ComponentPair.Key].SetComponent(Component);
			}
		}
	}
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

void UFigmaFile::CreateAssetBuilders(const FProcessFinishedDelegate& ProcessDelegate, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders)
{
	CurrentProcessDelegate = ProcessDelegate;
	AsyncTask(ENamedThreads::GameThread, [&]()
		{
			FGCScopeGuard GCScopeGuard;
			for (TPair<FString, FFigmaComponentSetRef>& ComponentSetPair : ComponentSets)
			{
				if (!ComponentSetPair.Value.Remote)
					continue;

				TObjectPtr<UFigmaComponentSet> ComponentSet = ComponentSetPair.Value.GetComponentSet();
				if (ComponentSet)
				{
					TScriptInterface<IAssetBuilder> AssetBuilder = ComponentSet->CreateAssetBuilder(ComponentSetPair.Value.RemoteFileKey);
					if (AssetBuilder != nullptr)
					{
						AssetBuilders.Add(AssetBuilder);
					}
					else
					{
						UE_LOG_Figma2UMG(Warning, TEXT("[Asset] ComponentSet %s Id %s failed to return a AssetBuilder"), *ComponentSet->GetNodeName(), *ComponentSet->GetIdForName());
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
					TScriptInterface<IAssetBuilder> AssetBuilder = Component->CreateAssetBuilder(ComponentPair.Value.RemoteFileKey);
					if (AssetBuilder != nullptr)
					{
						AssetBuilders.Add(AssetBuilder);
					}
					else
					{
						UE_LOG_Figma2UMG(Warning, TEXT("[Asset] Component %s Id %s failed to return a AssetBuilder"), *Component->GetNodeName(), *Component->GetIdForName());
					}
				}
			}

			if (Document)
			{
				CreateAssetBuilder(*Document, AssetBuilders);

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
		if (PendingComponents.Contains(SubInstance->ComponentId))
			continue;

		if (LibraryFile.Value->Components.Contains(SubInstance->ComponentId))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[Component] Adding dependency to Component %s id %s"), *SubInstance->GetNodeName(), *SubInstance->ComponentId);
			FFigmaComponentRef& RemoteCommponentRef = LibraryFile.Value->Components[SubInstance->ComponentId];
			if (!RemoteCommponentRef.Remote)
			{
				RemoteCommponentRef.RemoteFileKey = LibraryFile.Key;
			}
			FFigmaComponentRef& SubComponentRef = PendingComponents.Add(SubInstance->ComponentId, RemoteCommponentRef);
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

void UFigmaFile::CreateAssetBuilder(UFigmaNode& Node, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders)
{
	TScriptInterface<IAssetBuilder> AssetBuilder = Node.CreateAssetBuilder(FileKey);
	if (AssetBuilder != nullptr)
	{
		AssetBuilders.Add(AssetBuilder);
	}

	if (IFigmaContainer* FigmaContainer = Cast<IFigmaContainer>(&Node))
	{
		FigmaContainer->ForEach(IFigmaContainer::FOnEachFunction::CreateLambda([&](UFigmaNode& ChildNode, const int Index)
			{
				CreateAssetBuilder(ChildNode, AssetBuilders);
			}));
	}
}

