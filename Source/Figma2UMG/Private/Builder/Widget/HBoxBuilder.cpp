// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Builder/Widget/HBoxBuilder.h"

TObjectPtr<UWidget> UHBoxBuilder::PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
    Box = Patch<UHorizontalBox>(WidgetTree, WidgetToPatch);

    PatchPreInsertChildren(WidgetTree, Box);

    return Box;
}
