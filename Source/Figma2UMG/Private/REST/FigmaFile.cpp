// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/FigmaFile.h"

#include "AssetToolsModule.h"
#include "FileHelpers.h"
#include "WidgetBlueprintFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor/UMGEditor/Public/WidgetBlueprint.h"
#include "Nodes/FigmaDocument.h"

void UFigmaFile::PostSerialize(const TSharedRef<FJsonObject> fileJsonObject)
{
	if(Document)
	{
		Document->PostSerialize(nullptr, fileJsonObject->GetObjectField("Document").ToSharedRef());
	}
}

void UFigmaFile::CreateOrUpdateAsset(const FString& PackagePath)
{
	const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + Name);
	UWidgetBlueprint* Asset = nullptr;

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *Name, FString()));
	Asset = Cast<UWidgetBlueprint>(AssetData.FastGetAsset(true));

	if (Asset == nullptr)
	{
		static const FName NAME_AssetTools = "AssetTools";
		IAssetTools* AssetTools = &FModuleManager::GetModuleChecked<FAssetToolsModule>(NAME_AssetTools).Get();
		UClass* AssetClass = UWidgetBlueprint::StaticClass();
		UWidgetBlueprintFactory* Factory = NewObject<UWidgetBlueprintFactory>(UWidgetBlueprintFactory::StaticClass());
		Asset = Cast<UWidgetBlueprint>(AssetTools->CreateAsset(Name, PackagePath, AssetClass, Factory, FName("Figma2UMG")));

		if (Asset == nullptr)
		{
			return;
		}
	}

	if (Document)
	{
		Document->AddToAsset(Asset);
	}
}
