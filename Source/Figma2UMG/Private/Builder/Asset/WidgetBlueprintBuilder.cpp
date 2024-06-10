// Copyright 2024 Buvi Games. All Rights Reserved.


#include "WidgetBlueprintBuilder.h"

#include "AssetToolsModule.h"
#include "Figma2UMGModule.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/Widget/WidgetBuilder.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Parser/Nodes/FigmaComponent.h"
#include "Parser/Nodes/FigmaComponentSet.h"
#include "Parser/Nodes/FigmaNode.h"

void UWidgetBlueprintBuilder::LoadOrCreateAssets()
{
	UWidgetBlueprint* WidgetAsset = Cast<UWidgetBlueprint>(Asset);
	if (WidgetAsset == nullptr)
	{
		const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageName());
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

	if(const UFigmaComponent* ComponentNode = Cast<UFigmaComponent>(Node))
	{
		if (PatchPropertyDefinitions(ComponentNode->ComponentPropertyDefinitions))
		{
			CompileBP(EBlueprintCompileOptions::SkipGarbageCollection | EBlueprintCompileOptions::SaveIntermediateProducts);
		}
	}
	else if (const UFigmaComponentSet* ComponentSetNode = Cast<UFigmaComponentSet>(Node))
	{
		if (PatchPropertyDefinitions(ComponentSetNode->ComponentPropertyDefinitions))
		{
			CompileBP(EBlueprintCompileOptions::SkipGarbageCollection | EBlueprintCompileOptions::SaveIntermediateProducts);
		}
	}
}

void UWidgetBlueprintBuilder::LoadAssets()
{
	const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageName());
	const FString AssetName = ObjectTools::SanitizeInvalidChars(Node->GetUAssetName(), INVALID_OBJECTNAME_CHARACTERS);
	const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
	Asset = Cast<UWidgetBlueprint>(AssetData.FastGetAsset(true));

}

void UWidgetBlueprintBuilder::ResetWidgets()
{
	if (Asset)
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
	RootWidgetBuilder->PatchAndInsertWidget(Asset->WidgetTree, Asset->WidgetTree->RootWidget);
	if (Asset->WidgetTree->RootWidget == nullptr)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[PatchAndInsertWidget] Node %s failed to insert RootWidget."), *Node->GetNodeName());
	}

	Asset->WidgetTree->SetFlags(RF_Transactional);
	Asset->WidgetTree->Modify();

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Asset);
}

void UWidgetBlueprintBuilder::PatchWidgetBinds()
{
	if (!Asset)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[PatchWidgetBinds] Missing Blueprint for node %s."), *Node->GetNodeName());
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
	UE_LOG_Figma2UMG(Display, TEXT("[PatchWidgetProperties] Bluepring %s."), *WidgetBP->GetName());
	RootWidgetBuilder->PatchWidgetProperties();
}

TObjectPtr<UWidgetBlueprint> UWidgetBlueprintBuilder::GetAsset() const
{
	return Asset;
}

bool UWidgetBlueprintBuilder::FillType(const FFigmaComponentPropertyDefinition& Def, FEdGraphPinType& MemberType) const
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
		MemberType.PinCategory = UEdGraphSchema_K2::PC_Object;
		// MemberType.PinSubCategory = ?
		// MemberType.PinSubCategoryObject = ?
		break;
	case EFigmaComponentPropertyType::VARIANT:
		return false;
	}

	return true;
}

bool UWidgetBlueprintBuilder::PatchPropertyDefinitions(const TMap<FString, FFigmaComponentPropertyDefinition>& ComponentPropertyDefinitions) const
{
	UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(Asset);
	if(!WidgetBP)
		return false;
	
	bool AddedMemberVariable = false;
	for (const TPair<FString, FFigmaComponentPropertyDefinition> Property : ComponentPropertyDefinitions)
	{
		FEdGraphPinType MemberType;
		if(!FillType(Property.Value, MemberType))
			continue;

		FString PropertyName = Property.Key;//TODO: Remove '#id'

		UE_LOG_Figma2UMG(Display, TEXT("Blueprint %s - Adding member %s type %s and defaultValue %s ."), *WidgetBP->GetName(), *PropertyName, *MemberType.PinCategory.ToString(), *Property.Value.DefaultValue);
		if (FBlueprintEditorUtils::AddMemberVariable(WidgetBP, *PropertyName, MemberType, Property.Value.DefaultValue))
		{
			FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(WidgetBP, *PropertyName, false);
			AddedMemberVariable = true;
		}
	
		//TODO: This is to initialize the Switchers, would probably need to check if there are Switchers
		if (Node->IsA<UFigmaComponentSet>())
		{
			TSet<FName> CurrentVars;
			FBlueprintEditorUtils::GetClassVariableList(WidgetBP, CurrentVars);
			if (AddedMemberVariable || CurrentVars.Contains(*PropertyName))
			{
				FString FunctionName = "Init" + PropertyName;
				TObjectPtr<UEdGraph>* Graph = WidgetBP->FunctionGraphs.FindByPredicate([FunctionName](const TObjectPtr<UEdGraph> Graph)
					{
						return Graph.GetName() == FunctionName;
					});
	
				UEdGraph* FunctionGraph = Graph ? *Graph : nullptr;
				if (!FunctionGraph)
				{
					FunctionGraph = FBlueprintEditorUtils::CreateNewGraph(WidgetBP, FBlueprintEditorUtils::FindUniqueKismetName(WidgetBP, FunctionName), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	
					FBlueprintEditorUtils::AddFunctionGraph<UClass>(WidgetBP, FunctionGraph, true, nullptr);
					AddedMemberVariable = true;
				}
			}
			else
			{
				UE_LOG_Figma2UMG(Error, TEXT("[UWidgetBlueprintBuilder::PatchPropertiesToWidget] Fail to add member variable %s of type %s."), *PropertyName, *MemberType.PinCategory.ToString());
			}
		}
	}
	
	return AddedMemberVariable;
}
