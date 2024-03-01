// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/AssetFileHandler.h"

#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "Blueprint/WidgetTree.h"
#include "Factory/RawTexture2DFactory.h"
#include "Kismet2/BlueprintEditorUtils.h"

template <>
UWidgetBlueprint* IFigmaFileHandle::GetOrCreateAsset<UWidgetBlueprint, UWidgetBlueprintFactory>(UWidgetBlueprintFactory* Factory)
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
			if (Factory == nullptr)
			{
				Factory = NewObject<UWidgetBlueprintFactory>(UWidgetBlueprintFactory::StaticClass());
			}
			UE_LOG_Figma2UMG(Display, TEXT("Create UAsset %s of type %s"), *PackagePath, *AssetClass);
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


template <>
UTexture2D* IFigmaFileHandle::GetOrCreateAsset<UTexture2D, URawTexture2DFactory>(URawTexture2DFactory* Factory)
{
	UTexture2D* TextureAsset = Cast<UTexture2D>(Asset);
	if (TextureAsset == nullptr)
	{
		const FString PackagePath = UPackageTools::SanitizePackageName(GetPackagePath());
		const FString AssetName = ObjectTools::SanitizeInvalidChars(GetAssetName(), INVALID_OBJECTNAME_CHARACTERS);
		const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

		UClass* AssetClass = UTexture2D::StaticClass();
		if(Factory == nullptr)
		{
			Factory = NewObject<URawTexture2DFactory>(URawTexture2DFactory::StaticClass());
		}

		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
		TextureAsset = Cast<UTexture2D>(AssetData.FastGetAsset(true));

		if (TextureAsset == nullptr)
		{
			static const FName NAME_AssetTools = "AssetTools";
			IAssetTools* AssetTools = &FModuleManager::GetModuleChecked<FAssetToolsModule>(NAME_AssetTools).Get();
			UE_LOG_Figma2UMG(Display, TEXT("Create UAsset %s of type %s"), *PackagePath, *AssetClass);
			TextureAsset = Cast<UTexture2D>(AssetTools->CreateAsset(AssetName, PackagePath, AssetClass, Factory, FName("Figma2UMG")));
		}
		else
		{

			UPackage* Pkg = CreatePackage(*PackagePath);
			const EObjectFlags Flags = RF_Public | RF_Standalone | RF_Transactional;
			UE_LOG_Figma2UMG(Display, TEXT("Reimport UAsset %s of type %s"), *PackagePath, *AssetClass);
			TextureAsset = Cast<UTexture2D>(Factory->FactoryCreateNew(AssetClass, Pkg, *AssetName, Flags, nullptr, GWarn));
			if (TextureAsset)
			{
				FAssetRegistryModule::AssetCreated(TextureAsset);
			}
		}

		Asset = TextureAsset;
		AssetOuter = TextureAsset;
	}

	if (TextureAsset)
	{
		TextureAsset->SetFlags(RF_Transactional);
		TextureAsset->Modify();
	}

	return TextureAsset;
}

void IFigmaFileHandle::Reset()
{
	Asset = nullptr;
	AssetOuter = nullptr;
}
