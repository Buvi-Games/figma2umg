// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/FigmaFile.h"

#include "AssetToolsModule.h"
#include "FileHelpers.h"
#include "WidgetBlueprintFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor/UMGEditor/Public/WidgetBlueprint.h"
#include "Nodes/FigmaDocument.h"

FString UFigmaFile::GetPackagePath() const
{
	return PackagePath;
}

FString UFigmaFile::GetAssetName() const
{
	return Name;
}

void UFigmaFile::PostSerialize(const TSharedRef<FJsonObject> fileJsonObject)
{
	if(Document)
	{
		Document->PostSerialize(nullptr, fileJsonObject->GetObjectField("Document").ToSharedRef());
	}
}

void UFigmaFile::CreateOrUpdateAsset(const FString& ContentRootFolder)
{
	PackagePath = ContentRootFolder;
	UWidgetBlueprint* Asset = GetOrCreateAsset();

	if (Asset == nullptr)
	{
		return;
	}

	if (Document)
	{
		Document->AddToAsset(Asset);
	}
}