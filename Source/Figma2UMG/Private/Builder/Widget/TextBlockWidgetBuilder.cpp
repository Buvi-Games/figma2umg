// Copyright 2024 Buvi Games. All Rights Reserved.


#include "TextBlockWidgetBuilder.h"

#include "Figma2UMGModule.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Parser/Nodes/FigmaNode.h"

void UTextBlockWidgetBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	UE_LOG_Figma2UMG(Warning, TEXT("[UTextBlockWidgetBuilder::PatchAndInsertWidget] TODO."));
	Insert(WidgetTree, WidgetToPatch, WidgetToPatch);
}

bool UTextBlockWidgetBuilder::TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget)
{
	UE_LOG_Figma2UMG(Warning, TEXT("[UTextBlockWidgetBuilder::TryInsertOrReplace] Node %s is an TextBlock and can't insert widgets."), *Node->GetNodeName());
	return false;
}

TObjectPtr<UWidget> UTextBlockWidgetBuilder::GetWidget() const
{
	return Widget;
}
