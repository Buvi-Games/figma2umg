// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Builder/Widget/VBoxBuilder.h"

TObjectPtr<UWidget> UVBoxBuilder::PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
    Box = Patch<UVerticalBox>(WidgetTree, WidgetToPatch);
    return Box;
}
