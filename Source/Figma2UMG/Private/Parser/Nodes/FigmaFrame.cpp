// MIT License
// Copyright (c) 2024 Buvi Games


#include "Parser/Nodes/FigmaFrame.h"

#include "Figma2UMGModule.h"
#include "Builder/Asset/MaterialBuilder.h"
#include "Builder/Asset/Texture2DBuilder.h"
#include "Builder/Asset/WidgetBlueprintBuilder.h"
#include "Builder/Widget/UserWidgetBuilder.h"
#include "Parser/FigmaFile.h"

void UFigmaFrame::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	if (UFigmaFile* File = GetFigmaFile())
	{
		if (File->IsNodeSelected(GetId()))
		{
			SetGenerateFile(true);
		}
	}
}

void UFigmaFrame::SetGenerateFile(bool Value /*= true*/)
{
	GenerateFile = Value;
}

TScriptInterface<IWidgetBuilder> UFigmaFrame::CreateWidgetBuilders(bool IsRoot/*= false*/, bool AllowFrameButton/*= true*/) const
{
	UE_LOG_Figma2UMG(Display, TEXT("[FigmaFrame::CreateWidgetBuilders] Node: %s, IsRoot: %d, GenerateFile: %d, AllowFrameButton: %d"),
		*GetNodeName(), IsRoot, GenerateFile, AllowFrameButton);

	if (!GenerateFile || IsRoot)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[FigmaFrame] Calling Super::CreateWidgetBuilders for: %s"), *GetNodeName());
		return Super::CreateWidgetBuilders(IsRoot, AllowFrameButton);
	}
	else
	{
		UE_LOG_Figma2UMG(Display, TEXT("[FigmaFrame] Creating UserWidgetBuilder (placeholder) for: %s - Children will NOT be processed!"), *GetNodeName());
		UUserWidgetBuilder* UserWidgetBuilder = NewObject<UUserWidgetBuilder>();
		UserWidgetBuilder->SetNode(this);
		UserWidgetBuilder->SetWidgetBlueprintBuilder(GetAssetBuilder());
		return UserWidgetBuilder;
	}
}

bool UFigmaFrame::CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders)
{
	if (GenerateFile)
	{
		WidgetBlueprintBuilder = NewObject<UWidgetBlueprintBuilder>();
		WidgetBlueprintBuilder->SetNode(InFileKey, this);
		AssetBuilders.Add(WidgetBlueprintBuilder);
	}

	Super::CreateAssetBuilder(InFileKey, AssetBuilders);

	return WidgetBlueprintBuilder != nullptr;
}

FString UFigmaFrame::GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	FString Suffix = "Menu";
	if (Cast<UMaterialBuilder>(InAssetBuilder.GetObject()))
	{
		Suffix = "Material";
	}
	else if (Cast<UTexture2DBuilder>(InAssetBuilder.GetObject()))
	{
		Suffix = "Textures";
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + Suffix;
}

const TObjectPtr<UWidgetBlueprintBuilder>& UFigmaFrame::GetAssetBuilder() const
{
	return WidgetBlueprintBuilder;
}
