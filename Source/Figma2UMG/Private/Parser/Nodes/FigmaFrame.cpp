// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaFrame.h"

#include "Builder/Asset/MaterialBuilder.h"
#include "Builder/Asset/Texture2DBuilder.h"
#include "Builder/Asset/WidgetBlueprintBuilder.h"
#include "Builder/Widget/UserWidgetBuilder.h"

void UFigmaFrame::SetGenerateFile(bool Value /*= true*/)
{
	GenerateFile = Value;
}

TScriptInterface<IWidgetBuilder> UFigmaFrame::CreateWidgetBuilders(bool IsRoot/*= false*/, bool AllowFrameButton/*= true*/) const
{
	if (!GenerateFile || IsRoot)
	{
		return Super::CreateWidgetBuilders(IsRoot, AllowFrameButton);
	}
	else
	{
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
