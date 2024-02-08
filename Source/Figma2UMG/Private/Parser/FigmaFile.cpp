// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/FigmaFile.h"

#include "Nodes/FigmaDocument.h"
#include "Properties/FigmaComponentRef.h"



void UFigmaFile::PostSerialize(const FString& InPackagePath, const TSharedRef<FJsonObject> fileJsonObject)
{
	PackagePath = InPackagePath;
	if(Document)
	{
		Document->SetFigmaFile(this);
		Document->PostSerialize(nullptr, fileJsonObject->GetObjectField("Document").ToSharedRef());
	}
}

void UFigmaFile::ConvertToAssets()
{
	FGCScopeGuard GCScopeGuard;
	Convert();
}

FString UFigmaFile::FindComponentName(const FString& ComponentId)
{
	if (Components.Contains(ComponentId))
	{
		const FFigmaComponentRef& Component = Components[ComponentId];
		return Component.Name;
	}

	return FString();
}

FFigmaComponentRef* UFigmaFile::FindComponentRef(const FString& ComponentId)
{
	if (Components.Contains(ComponentId))
	{
		FFigmaComponentRef& Component = Components[ComponentId];
		return &Component;
	}

	return nullptr;
}

void UFigmaFile::Convert()
{
	if (Document)
	{
		Document->PrePatchWidget();
		Document->PatchPreInsertWidget(nullptr);
		Document->PatchPostInsertWidget();
		Document->PostPatchWidget();
	}
}
