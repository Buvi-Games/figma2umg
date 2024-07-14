// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaSection.h"

#include "Figma2UMGModule.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/Asset/MaterialBuilder.h"
#include "Builder/Widget/BorderWidgetBuilder.h"
#include "Builder/Widget/Panels/CanvasBuilder.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UFigmaSection::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	static FString DevStatusStr("devStatus");
	static FString TypeStr("type");
	if (JsonObj->HasTypedField<EJson::Object>(DevStatusStr))
	{
		const TSharedPtr<FJsonObject> DevStatusJson = JsonObj->GetObjectField(DevStatusStr);
		if (DevStatusJson->HasTypedField<EJson::String>(TypeStr))
		{
			DevStatus = JsonObj->GetStringField(TypeStr);
		}
	}

	PostSerializeProperty(JsonObj, "fills", Fills);
	PostSerializeProperty(JsonObj, "strokes", Strokes);
}

FVector2D UFigmaSection::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

FVector2D UFigmaSection::GetAbsoluteSize() const
{
	return AbsoluteBoundingBox.GetSize();
}

FString UFigmaSection::GetCurrentPackagePath() const
{
	FString CurrentPackagePath = Super::GetCurrentPackagePath() + +TEXT("/") + GetNodeName();
	return CurrentPackagePath;
}

bool UFigmaSection::CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders)
{
	CreatePaintAssetBuilderIfNeeded(InFileKey, AssetBuilders, Fills);
	return Super::CreateAssetBuilder(InFileKey, AssetBuilders);
}

FString UFigmaSection::GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const
{
	if (Cast<UMaterialBuilder>(InAssetBuilder.GetObject()))
	{
		TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
		while (TopParentNode && TopParentNode->GetParentNode())
		{
			TopParentNode = TopParentNode->GetParentNode();
		}
		return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "Material";
	}

	return Super::GetPackageNameForBuilder(InAssetBuilder);
}

TScriptInterface<IWidgetBuilder> UFigmaSection::CreateWidgetBuilders(bool IsRoot/*= false*/, bool AllowFrameButton/*= true*/) const
{
	UCanvasBuilder* CanvasBuilder = NewObject<UCanvasBuilder>();
	CanvasBuilder->SetNode(this);

	for (const UFigmaNode* Child : Children)
	{
		if (TScriptInterface<IWidgetBuilder> SubBuilder = Child->CreateWidgetBuilders())
		{
			CanvasBuilder->AddChild(SubBuilder);
		}
	}

	bool RequireBorder = false;
	for (int i = 0; i < Fills.Num() && !RequireBorder; i++)
	{
		if (Fills[i].Visible)
			RequireBorder = true;
	}
	for (int i = 0; i < Strokes.Num() && !RequireBorder; i++)
	{
		if (Strokes[i].Visible)
			RequireBorder = true;
	}

	if (RequireBorder)
	{
		UBorderWidgetBuilder* BorderWidgetBuilder = NewObject<UBorderWidgetBuilder>();
		BorderWidgetBuilder->SetNode(this);
		BorderWidgetBuilder->SetChild(CanvasBuilder);

		return BorderWidgetBuilder;
	}

	return CanvasBuilder;
}
