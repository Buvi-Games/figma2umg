// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/FigmaFrame.h"

#include "WidgetBlueprintFactory.h"

void UFigmaFrame::SetGenerateFile()
{
	GenerateFile = true;
}

FString UFigmaFrame::GetPackagePath() const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "Menu";
}

FString UFigmaFrame::GetAssetName() const
{
	return GetUniqueName();
}

void UFigmaFrame::LoadOrCreateAssets(UFigmaFile* FigmaFile)
{
	if (GenerateFile)
	{
		GetOrCreateAsset<UWidgetBlueprint, UWidgetBlueprintFactory>();
	}
}

void UFigmaFrame::LoadAssets()
{
	if (GenerateFile)
	{
		LoadAsset<UWidgetBlueprint>();
	}
}

UObject* UFigmaFrame::GetAssetOuter() const
{
	if (GenerateFile)
	{
		return Super::GetAssetOuter();
	}
	else if (ParentNode)
	{
		return ParentNode->GetAssetOuter();
	}

	return nullptr;
}
