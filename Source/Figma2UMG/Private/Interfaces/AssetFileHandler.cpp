// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/AssetFileHandler.h"

#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "Blueprint/WidgetTree.h"
#include "Factories/Texture2dFactoryNew.h"
#include "Factories/TextureFactory.h"
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

		UPackage* Pkg = CreatePackage(*PackagePath);
		const EObjectFlags Flags = RF_Public | RF_Standalone | RF_Transactional;
		TextureAsset = Cast<UTexture2D>(Factory->FactoryCreateNew(AssetClass, Pkg, *AssetName, Flags, nullptr, GWarn));
		if (TextureAsset)
		{
			Pkg->SetIsExternallyReferenceable(true);
			FAssetRegistryModule::AssetCreated(TextureAsset);
			Pkg->MarkPackageDirty();
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