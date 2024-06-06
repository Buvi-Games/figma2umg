// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Builder/Widget/CanvasBuilder.h"

#include "Components/CanvasPanel.h"
#include "Parser/Nodes/FigmaDocument.h"

TObjectPtr<UWidget> UCanvasBuilder::PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
    TObjectPtr<UWidget> MyWidgetToPatch = WidgetToPatch;
    if(MyWidgetToPatch && MyWidgetToPatch->IsA<UWidgetSwitcher>() && Node->GetParentNode() && Node->GetParentNode()->IsA<UFigmaDocument>())
    {
        MyWidgetToPatch = FindNodeWidgetInParent(Cast<UWidgetSwitcher>(WidgetToPatch));
    }

    CanvasPanel = Patch<UCanvasPanel>(WidgetTree, MyWidgetToPatch);

    PatchPreInsertChildren(WidgetTree, CanvasPanel);

    return CanvasPanel;
}
