// MIT License
// Copyright (c) 2024 Buvi Games


#include "CanvasBuilder.h"

#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Parser/Nodes/FigmaDocument.h"

void UCanvasBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch)
{
    TObjectPtr<UWidget> MyWidgetToPatch = WidgetToPatch;
    if(MyWidgetToPatch && MyWidgetToPatch->IsA<UWidgetSwitcher>() && Node->GetParentNode() && Node->GetParentNode()->IsA<UFigmaDocument>())
    {
        MyWidgetToPatch = FindNodeWidgetInParent(Cast<UWidgetSwitcher>(WidgetToPatch));
    }

    CanvasPanel = Patch<UCanvasPanel>(WidgetBlueprint->WidgetTree, MyWidgetToPatch);

    Insert(WidgetBlueprint->WidgetTree, WidgetToPatch, CanvasPanel);
    PatchAndInsertChildren(WidgetBlueprint, CanvasPanel);
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
