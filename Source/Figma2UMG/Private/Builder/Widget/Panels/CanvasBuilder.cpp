// Copyright 2024 Buvi Games. All Rights Reserved.


#include "CanvasBuilder.h"

#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Parser/Nodes/FigmaDocument.h"

void UCanvasBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
    TObjectPtr<UWidget> MyWidgetToPatch = WidgetToPatch;
    if(MyWidgetToPatch && MyWidgetToPatch->IsA<UWidgetSwitcher>() && Node->GetParentNode() && Node->GetParentNode()->IsA<UFigmaDocument>())
    {
        MyWidgetToPatch = FindNodeWidgetInParent(Cast<UWidgetSwitcher>(WidgetToPatch));
    }

    CanvasPanel = Patch<UCanvasPanel>(WidgetTree, MyWidgetToPatch);

    Insert(WidgetTree, WidgetToPatch, CanvasPanel);
    PatchAndInsertChildren(WidgetTree, CanvasPanel);
}

void UCanvasBuilder::SetWidget(const TObjectPtr<UWidget>& InWidget)
{
    Super::SetWidget(InWidget);
    CanvasPanel = Cast<UCanvasPanel>(Widget);
}

void UCanvasBuilder::ResetWidget()
{
    CanvasPanel = nullptr;
	Super::ResetWidget();
}

void UCanvasBuilder::Setup() const
{
    //Nothing to do
}
