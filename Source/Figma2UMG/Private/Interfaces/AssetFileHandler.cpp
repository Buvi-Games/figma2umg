// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/AssetFileHandler.h"

#include "AssetToolsModule.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet2/BlueprintEditorUtils.h"

template <>
UWidgetBlueprint* IFigmaFileHandle::GetOrCreateAsset<UWidgetBlueprint>()
{
	UWidgetBlueprint* WidgetAsset = Cast<UWidgetBlueprint>(Asset);
	if (WidgetAsset == nullptr)
	{
		const FString PackagePath = UPackageTools::SanitizePackageName(GetPackagePath());
		const FString AssetName = ObjectTools::SanitizeInvalidChars(GetAssetName(), INVALID_OBJECTNAME_CHARACTERS);
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
			WidgetAsset = Cast<UWidgetBlueprint>(AssetTools->CreateAsset(AssetName, PackagePath, AssetClass, Factory, FName("Figma2UMG")));
		}

		Asset = WidgetAsset;
		AssetOuter = WidgetAsset->WidgetTree;
	}


	WidgetAsset->WidgetTree->SetFlags(RF_Transactional);
	WidgetAsset->WidgetTree->Modify();

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetAsset);

	return WidgetAsset;
}
