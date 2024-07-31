// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/Vectors/FigmaVectorNode.h"

#include "Builder/Asset/MaterialBuilder.h"
#include "Builder/Asset/Texture2DBuilder.h"
#include "Builder/Widget/ImageWidgetBuilder.h"
#include "Components/Image.h"
#include "Factory/RawTexture2DFactory.h"
#include "REST/FigmaImporter.h"

void UFigmaVectorNode::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	PostSerializeProperty(JsonObj, "fills", Fills);
	PostSerializeProperty(JsonObj, "strokes", Strokes);
}

FVector2D UFigmaVectorNode::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

FVector2D UFigmaVectorNode::GetAbsoluteSize() const
{
	return AbsoluteBoundingBox.GetSize();
}

bool UFigmaVectorNode::CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders)
{
	AssetBuilder = NewObject<UTexture2DBuilder>();
	AssetBuilder->SetNode(InFileKey, this);
	AssetBuilders.Add(AssetBuilder);

	CreatePaintAssetBuilderIfNeeded(InFileKey, AssetBuilders, Fills, Strokes);

	return true;
}

FString UFigmaVectorNode::GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	FString Suffix = "Textures";
	if (Cast<UMaterialBuilder>(InAssetBuilder.GetObject()))
	{
		Suffix = "Material";
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + Suffix;
}

TScriptInterface<IWidgetBuilder> UFigmaVectorNode::CreateWidgetBuilders(bool IsRoot/*= false*/, bool AllowFrameButton/*= true*/) const
{
	UImageWidgetBuilder* ImageWidgetBuilder = NewObject<UImageWidgetBuilder>();
	ImageWidgetBuilder->SetNode(this);
	ImageWidgetBuilder->SetTexture2DBuilder(AssetBuilder);

	return ImageWidgetBuilder;
}
