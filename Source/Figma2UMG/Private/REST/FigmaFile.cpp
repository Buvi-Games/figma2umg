// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/FigmaFile.h"

#include "AssetToolsModule.h"
#include "FileHelpers.h"
#include "WidgetBlueprintFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor/UMGEditor/Public/WidgetBlueprint.h"
#include "Nodes/FigmaDocument.h"
#include "Properties/FigmaComponentRef.h"
#include "Properties/FigmaComponentSetRef.h"


void UFigmaFile::PostSerialize(const TSharedRef<FJsonObject> fileJsonObject)
{
	if(Document)
	{
		Document->PostSerialize(nullptr, fileJsonObject->GetObjectField("Document").ToSharedRef());
	}

	ImportComponents();

	Convert();
}

void UFigmaFile::ImportComponents()
{
	for (TTuple<FString, FFigmaComponentRef>& Element : Components)
	{
		Element.Value.Import();
	}
	for (TTuple<FString, FFigmaComponentSetRef>& Element : ComponentSets)
	{
		Element.Value.Import();
	}
}

void UFigmaFile::SetRootPath(const FString& InPackagePath)
{
	PackagePath = InPackagePath;
}

void UFigmaFile::Convert()
{
	if (Document)
	{
		Document->SetCurrentPackagePath(PackagePath);
		Document->SetFileName(Name);
		Document->AddOrPathToWidget(nullptr);
	}
}
