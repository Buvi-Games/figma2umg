// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaFrame.h"

#include "Figma2UMGModule.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/Asset/WidgetBlueprintBuilder.h"
#include "Builder/Widget/UserWidgetBuilder.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Templates/WidgetTemplateBlueprintClass.h"

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

TScriptInterface<IAssetBuilder> UFigmaFrame::CreateAssetBuilder(const FString& InFileKey)
{
	if (GenerateFile)
	{
		WidgetBlueprintBuilder = NewObject<UWidgetBlueprintBuilder>();
		WidgetBlueprintBuilder->SetNode(InFileKey, this);
		return WidgetBlueprintBuilder;
	}

	return nullptr;
}

FString UFigmaFrame::GetPackageName() const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "Menu";
}

const TObjectPtr<UWidgetBlueprintBuilder>& UFigmaFrame::GetAssetBuilder() const
{
	return WidgetBlueprintBuilder;
}
