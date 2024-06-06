// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Builder/Widget/CanvasBuilder.h"

#include "Components/CanvasPanel.h"

TObjectPtr<UWidget> UCanvasBuilder::PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
    CanvasPanel = Patch<UCanvasPanel>(WidgetTree, WidgetToPatch);
    return CanvasPanel;
}
