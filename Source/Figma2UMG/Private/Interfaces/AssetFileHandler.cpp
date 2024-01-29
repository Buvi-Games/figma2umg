// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/AssetFileHandler.h"

#include "AssetToolsModule.h"
#include "PackageTools.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"

UWidgetBlueprint* IFigmaFileHandle::GetOrCreateAsset()
{
	const FString PackagePath = GetPackagePath();
	const FString AssetName = GetAssetName();
	const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);
	UWidgetBlueprint* Asset = nullptr;

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
	Asset = Cast<UWidgetBlueprint>(AssetData.FastGetAsset(true));

	if (Asset == nullptr)
	{
		static const FName NAME_AssetTools = "AssetTools";
		IAssetTools* AssetTools = &FModuleManager::GetModuleChecked<FAssetToolsModule>(NAME_AssetTools).Get();
		UClass* AssetClass = UWidgetBlueprint::StaticClass();
		UWidgetBlueprintFactory* Factory = NewObject<UWidgetBlueprintFactory>(UWidgetBlueprintFactory::StaticClass());
		Asset = Cast<UWidgetBlueprint>(AssetTools->CreateAsset(AssetName, PackagePath, AssetClass, Factory, FName("Figma2UMG")));
	}

	return Asset;
}
