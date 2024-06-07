// Copyright 2024 Buvi Games. All Rights Reserved.


#include "ImageWidgetBuilder.h"

#include "Figma2UMGModule.h"
#include "Components/Image.h"
#include "Components/Widget.h"
#include "Parser/Nodes/FigmaNode.h"


void UImageWidgetBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	UE_LOG_Figma2UMG(Warning, TEXT("[UImageWidgetBuilder::PatchAndInsertWidget] TODO."));
	Insert(WidgetTree, WidgetToPatch, WidgetToPatch);
}

bool UImageWidgetBuilder::TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget)
{
	UE_LOG_Figma2UMG(Warning, TEXT("[UImageWidgetBuilder::TryInsertOrReplace] Node %s is an Image and can't insert widgets."), *Node->GetNodeName());
	return false;
}

TObjectPtr<UWidget> UImageWidgetBuilder::GetWidget()
{
	return Widget;
};
