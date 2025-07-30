// MIT License
// Copyright (c) 2024 Buvi Games


#include "Builder/Asset/WidgetBlueprintBuilder.h"

#include "AssetToolsModule.h"
#include "Figma2UMGModule.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/WidgetBlueprintHelper.h"
#include "Builder/Widget/WidgetBuilder.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Parser/FigmaFile.h"
#include "Parser/Nodes/FigmaComponent.h"
#include "Parser/Nodes/FigmaComponentSet.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Properties/FigmaComponentRef.h"

void UWidgetBlueprintBuilder::LoadOrCreateAssets()
{
	UWidgetBlueprint* WidgetAsset = Cast<UWidgetBlueprint>(Asset);
	if (WidgetAsset == nullptr)
	{
		const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this));
		const FString AssetName = ObjectTools::SanitizeInvalidChars(Node->GetUAssetName(), INVALID_OBJECTNAME_CHARACTERS);
		const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
		WidgetAsset = Cast<UWidgetBlueprint>(AssetData.FastGetAsset(true));

		if (WidgetAsset == nullptr)
		{
			static const FName NAME_AssetTools = "AssetTools";
			IAssetTools* AssetTools = &FModuleManager::GetModuleChecked<FAssetToolsModule>(NAME_AssetTools).Get();
			UClass* AssetClass = UWidgetBlueprint::StaticClass();
			UWidgetBlueprintFactory* Factory = NewObject<UWidgetBlueprintFactory>(UWidgetBlueprintFactory::StaticClass());
			UE_LOG_Figma2UMG(Display, TEXT("Create UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
			WidgetAsset = Cast<UWidgetBlueprint>(AssetTools->CreateAsset(AssetName, PackagePath, AssetClass, Factory, FName("Figma2UMG")));
		}
		else
		{
			UE_LOG_Figma2UMG(Display, TEXT("Loading UAsset %s/%s of type %s"), *PackagePath, *AssetName, *UWidgetBlueprint::StaticClass()->GetDisplayNameText().ToString());
		}

		Asset = WidgetAsset;
	}


	WidgetAsset->WidgetTree->SetFlags(RF_Transactional);
	WidgetAsset->WidgetTree->Modify();

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetAsset);
}

void UWidgetBlueprintBuilder::LoadAssets()
{
	const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this));
	const FString AssetName = ObjectTools::SanitizeInvalidChars(Node->GetUAssetName(), INVALID_OBJECTNAME_CHARACTERS);
	const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
	Asset = Cast<UWidgetBlueprint>(AssetData.FastGetAsset(true));

}

void UWidgetBlueprintBuilder::Reset()
{
	Asset = nullptr;
	if (RootWidgetBuilder)
	{
		RootWidgetBuilder->ResetWidget();
	}
}

void UWidgetBlueprintBuilder::ResetWidgets()
{
	if (Asset && RootWidgetBuilder)
	{
		RootWidgetBuilder->SetWidget(Asset->WidgetTree->RootWidget);
	}
}

void UWidgetBlueprintBuilder::CompileBP(EBlueprintCompileOptions CompileFlags)
{
	if (!Asset)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("Trying to compile %s but there is no UAsset."), *Node->GetNodeName());
		return;
	}

	UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(Asset);
	if (!WidgetBP)
	{
		//Should be fine, this is not a BP
		return;
	}

	Asset = nullptr;
	if (RootWidgetBuilder)
	{
		RootWidgetBuilder->ResetWidget();
	}

	FCompilerResultsLog LogResults;
	LogResults.SetSourcePath(WidgetBP->GetPathName());
	LogResults.BeginEvent(TEXT("Compile"));
	LogResults.bLogDetailedResults = true;

	UE_LOG_Figma2UMG(Display, TEXT("Compilint blueprint %s."), *WidgetBP->GetName());
	FKismetEditorUtilities::CompileBlueprint(WidgetBP, CompileFlags, &LogResults);

	LoadAssets();
}

void UWidgetBlueprintBuilder::CreateWidgetBuilders()
{
	if(!Asset)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[CreateWidgetBuilders] Missing Blueprint for node %s."), *Node->GetNodeName());
		return;
	}
	UE_LOG_Figma2UMG(Display, TEXT("[CreateWidgetBuilders] Generating Tree for %s."), *Asset->GetName());
	RootWidgetBuilder = Node->CreateWidgetBuilders(true);
}

void UWidgetBlueprintBuilder::PatchAndInsertWidgets()
{
	if (!Asset)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[PatchAndInsertWidget] Missing Blueprint for node %s."), *Node->GetNodeName());
		return;
	}

	if (!RootWidgetBuilder)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[PatchAndInsertWidget] Missing Builder for node %s."), *Node->GetNodeName());
		return;
	}

	UE_LOG_Figma2UMG(Display, TEXT("[PatchAndInsertWidget] Bluepring %s."), *Asset->GetName());
	RootWidgetBuilder->PatchAndInsertWidget(Asset, Asset->WidgetTree->RootWidget);
	RootWidgetBuilder->PostInsertWidgets(Asset);
	if (Asset->WidgetTree->RootWidget == nullptr)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[PatchAndInsertWidget] Node %s failed to insert RootWidget."), *Node->GetNodeName());
	}

	Asset->WidgetTree->SetFlags(RF_Transactional);
	Asset->WidgetTree->Modify();

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Asset);

	if (const UFigmaComponent* ComponentNode = Cast<UFigmaComponent>(Node))
	{
		PatchPropertyDefinitions(ComponentNode->ComponentPropertyDefinitions);
	}
	else if (const UFigmaComponentSet* ComponentSetNode = Cast<UFigmaComponentSet>(Node))
	{
		PatchPropertyDefinitions(ComponentSetNode->ComponentPropertyDefinitions);
	}
}

void UWidgetBlueprintBuilder::PatchWidgetBinds()
{
	if (!Asset)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[PatchWidgetBinds] Missing Blueprint for node %s."), *Node->GetNodeName());
		return;
	}
	if (!RootWidgetBuilder)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[PatchWidgetBinds] Missing WidgetRoot for node %s."), *Node->GetNodeName());
		return;
	}
	UE_LOG_Figma2UMG(Display, TEXT("[PatchWidgetBinds] Bluepring %s."), *Asset->GetName());
	RootWidgetBuilder->PatchWidgetBinds(Asset);
}

void UWidgetBlueprintBuilder::PatchWidgetProperties()
{
	UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(Asset);
	if (!WidgetBP)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[PatchWidgetProperties] Missing Blueprint for node %s."), *Node->GetNodeName());
		return;
	}
	if (!RootWidgetBuilder)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[PatchWidgetProperties] Missing WidgetRoot for node %s."), *Node->GetNodeName());
		return;
	}
	UE_LOG_Figma2UMG(Display, TEXT("[PatchWidgetProperties] Bluepring %s."), *WidgetBP->GetName());
	RootWidgetBuilder->PatchWidgetProperties();
}

TObjectPtr<UWidgetBlueprint> UWidgetBlueprintBuilder::GetAsset() const
{
	return Asset;
}

UPackage* UWidgetBlueprintBuilder::GetAssetPackage() const
{
	return Asset ? Asset->GetPackage() : nullptr;
}

void UWidgetBlueprintBuilder::FillType(const FFigmaComponentPropertyDefinition& Def, FEdGraphPinType& MemberType) const
{
	MemberType.ContainerType = EPinContainerType::None;
	switch (Def.Type)
	{
	case EFigmaComponentPropertyType::BOOLEAN:
		MemberType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
		break;
	case EFigmaComponentPropertyType::TEXT:
		MemberType.PinCategory = UEdGraphSchema_K2::PC_String;
		break;
	case EFigmaComponentPropertyType::INSTANCE_SWAP:
		MemberType.PinCategory = UEdGraphSchema_K2::PC_String;
		break;
	case EFigmaComponentPropertyType::VARIANT:
		break;
	}

}

void UWidgetBlueprintBuilder::PatchMemberVariable(UWidgetBlueprint* WidgetBP, const TPair<FString, FFigmaComponentPropertyDefinition> Property) const
{
	FString PropertyName = Property.Key;//TODO: Remove '#id'
	FEdGraphPinType MemberType;
	FillType(Property.Value, MemberType);

	UE_LOG_Figma2UMG(Display, TEXT("Blueprint %s - Adding member %s type %s and defaultValue %s ."), *WidgetBP->GetName(), *PropertyName, *MemberType.PinCategory.ToString(), *Property.Value.DefaultValue);
	if (FBlueprintEditorUtils::AddMemberVariable(WidgetBP, *PropertyName, MemberType, Property.Value.DefaultValue))
	{
		FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(WidgetBP, *PropertyName, false);
	}
	else
	{
		const int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(WidgetBP, *PropertyName);
		if (VarIndex != INDEX_NONE && WidgetBP->NewVariables[VarIndex].VarType != MemberType)
		{
			FBlueprintEditorUtils::RemoveMemberVariable(WidgetBP, *PropertyName);
			if (FBlueprintEditorUtils::AddMemberVariable(WidgetBP, *PropertyName, MemberType, Property.Value.DefaultValue))
			{
				FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(WidgetBP, *PropertyName, false);
			}
			else
			{
				UE_LOG_Figma2UMG(Warning, TEXT("Blueprint %s - Fail to add member %s type %s and defaultValue %s ."), *WidgetBP->GetName(), *PropertyName, *MemberType.PinCategory.ToString(), *Property.Value.DefaultValue);
			}
		}
	}

	const int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(WidgetBP, *PropertyName);
	if (VarIndex != INDEX_NONE && WidgetBP->NewVariables[VarIndex].Category.EqualTo(UEdGraphSchema_K2::VR_DefaultCategory))
	{
		static const FText FigmaCategory(FText::FromString("Figma"));
		WidgetBP->NewVariables[VarIndex].Category = FigmaCategory;
	}
}

void UWidgetBlueprintBuilder::PatchPropertyDefinitions(const TMap<FString, FFigmaComponentPropertyDefinition>& ComponentPropertyDefinitions) const
{
	UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(Asset);
	if (!WidgetBP)
		return;
	
	for (const TPair<FString, FFigmaComponentPropertyDefinition> Property : ComponentPropertyDefinitions)
	{
		if (Property.Value.Type == EFigmaComponentPropertyType::VARIANT)
		{
			FString PropertyName = Property.Key;//TODO: Remove '#id'
			if (!Property.Value.IsButton())
			{
				//TODO: Handle ComponentSet with more than 1 EFigmaComponentPropertyType::VARIANT (SWITCH + AND)
				TObjectPtr<UWidgetSwitcher> WidgetSwitcher = Cast<UWidgetSwitcher>(RootWidgetBuilder->GetWidget());
				if (!WidgetSwitcher)
				{
					UE_LOG_Figma2UMG(Error, TEXT("[PatchPropertyDefinitions] Can't find UWidgetSwitcher to patch property %s at Node %s."), *PropertyName, *Node->GetNodeName());
				}
				WidgetBlueprintHelper::CreateSwitchFunction(WidgetBP, WidgetSwitcher, PropertyName, Property.Value.VariantOptions);
			}

			const int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(WidgetBP, *PropertyName);
			if (VarIndex != INDEX_NONE)
			{
				FBlueprintEditorUtils::RemoveMemberVariable(WidgetBP, *PropertyName);
			}
		}
		else if (Property.Value.Type == EFigmaComponentPropertyType::INSTANCE_SWAP)
		{
			FString PropertyName = Property.Key;
			int Index;
			if (PropertyName.FindChar('#', Index))
			{
				PropertyName.LeftInline(Index);
			}

			FString WidgetName = Property.Key.Replace(TEXT(":"), TEXT("-"), ESearchCase::CaseSensitive);
			TObjectPtr<UWidgetSwitcher> WidgetSwitcher = Cast<UWidgetSwitcher>(RootWidgetBuilder->FindWidgetRecursive(WidgetName));
			if (!WidgetSwitcher)
			{
				UE_LOG_Figma2UMG(Error, TEXT("[PatchPropertyDefinitions] Can't find UWidgetSwitcher to patch property %s at Node %s."), *PropertyName, *Node->GetNodeName());
			}
			TArray<FString> InstancesOptions;
			const TObjectPtr<UFigmaFile> FigmaFile = Node->GetFigmaFile();
			for (const FFigmaInstanceSwapPreferredValue& PreferredValue : Property.Value.PreferredValues)
			{
				TObjectPtr<UWidgetBlueprintBuilder> Builder = nullptr;
				TObjectPtr<UFigmaInstance> NewInstance = nullptr;
				if (PreferredValue.Type == ENodeTypes::COMPONENT)
				{
					if (const TObjectPtr<UFigmaComponent> Component = FigmaFile->FindComponentByKey(PreferredValue.Key))
					{
						InstancesOptions.Add(Component->GetId());

					}
				}
				else if (PreferredValue.Type == ENodeTypes::COMPONENT_SET)
				{
					if (const TObjectPtr<UFigmaComponentSet> ComponentSet = FigmaFile->FindComponentSetByKey(PreferredValue.Key))
					{
						InstancesOptions.Add(ComponentSet->GetId());
					}
				}
			}
			WidgetBlueprintHelper::CreateSwitchFunction(WidgetBP, WidgetSwitcher, PropertyName, InstancesOptions);			

			const int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(WidgetBP, *PropertyName);
			if (VarIndex != INDEX_NONE)
			{
				FBlueprintEditorUtils::RemoveMemberVariable(WidgetBP, *PropertyName);
			}
		}
		else
		{
			PatchMemberVariable(WidgetBP, Property);
		}
	}
}
