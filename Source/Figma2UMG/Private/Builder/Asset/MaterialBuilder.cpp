// Copyright 2024 Buvi Games. All Rights Reserved.


#include "MaterialBuilder.h"

#include "AssetToolsModule.h"
#include "Figma2UMGModule.h"
#include "MaterialDomain.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor/MaterialEditor/Public/MaterialEditingLibrary.h"
#include "Factories/MaterialFactoryNew.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "MaterialGraph/MaterialGraph.h"
#include "MaterialGraph/MaterialGraphSchema.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Parser/FigmaFile.h"
#include "Parser/Nodes/FigmaNode.h"

void UMaterialBuilder::LoadOrCreateAssets()
{
	UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>(UMaterialFactoryNew::StaticClass());

	UMaterial* MaterialAsset = Cast<UMaterial>(Asset);
	if (MaterialAsset == nullptr)
	{
		const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this));
		const FString AssetName = ObjectTools::SanitizeInvalidChars(Node->GetUAssetName(), INVALID_OBJECTNAME_CHARACTERS);
		const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

		UClass* AssetClass = UMaterial::StaticClass();
		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
		MaterialAsset = Cast<UMaterial>(AssetData.FastGetAsset(true));

		if (MaterialAsset == nullptr)
		{
			static const FName NAME_AssetTools = "AssetTools";
			IAssetTools* AssetTools = &FModuleManager::GetModuleChecked<FAssetToolsModule>(NAME_AssetTools).Get();
			UE_LOG_Figma2UMG(Display, TEXT("Create UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
			MaterialAsset = Cast<UMaterial>(AssetTools->CreateAsset(AssetName, PackagePath, AssetClass, Factory, FName("Figma2UMG")));
		}
		else
		{

			UPackage* Pkg = CreatePackage(*PackagePath);
			const EObjectFlags Flags = RF_Public | RF_Standalone | RF_Transactional;
			UE_LOG_Figma2UMG(Display, TEXT("Reimport UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
			MaterialAsset = Cast<UMaterial>(Factory->FactoryCreateNew(AssetClass, Pkg, *AssetName, Flags, nullptr, GWarn));
			if (MaterialAsset)
			{
				FAssetRegistryModule::AssetCreated(MaterialAsset);
			}
		}

		Asset = MaterialAsset;
	}

	if (MaterialAsset)
	{
		Setup();

		MaterialAsset->SetFlags(RF_Transactional);
		MaterialAsset->Modify();
	}
}

void UMaterialBuilder::LoadAssets()
{
	const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this));
	const FString AssetName = ObjectTools::SanitizeInvalidChars(Node->GetUAssetName(), INVALID_OBJECTNAME_CHARACTERS);
	const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
	Asset = Cast<UMaterial>(AssetData.FastGetAsset(true));
}

const TObjectPtr<UMaterial>& UMaterialBuilder::GetAsset() const
{
	return Asset;
}

void UMaterialBuilder::SetPaint(const FFigmaPaint* InPaint)
{
	Paint = InPaint;
}

void UMaterialBuilder::Setup()
{
	Asset->MaterialDomain = MD_UI;
	if (!Asset->MaterialGraph)
	{
		Asset->MaterialGraph = CastChecked<UMaterialGraph>(FBlueprintEditorUtils::CreateNewGraph(Asset, NAME_None, UMaterialGraph::StaticClass(), UMaterialGraphSchema::StaticClass()));
	}
	Asset->MaterialGraph->Material = Asset;
	UMaterialGraph* ExpressionGraph = ToRawPtr(Asset->MaterialGraph);
	if (ExpressionGraph)
	{
		UMaterialExpression* NewExpression = UMaterialEditingLibrary::CreateMaterialExpressionEx(Asset, nullptr, UMaterialExpressionCustom::StaticClass(), Asset, 0.0f, 0.0f);
		ExpressionGraph->AddExpression(NewExpression, true);
	}
	Asset->MaterialGraph->RebuildGraph();
}

UPackage* UMaterialBuilder::GetAssetPackage() const
{
	return Asset ? Asset->GetPackage() : nullptr;
}
