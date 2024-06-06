// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Builder/Widget/WBoxBuilder.h"

TObjectPtr<UWidget> UWBoxBuilder::PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
    Box = Patch<UWrapBox>(WidgetTree, WidgetToPatch);
    return Box;
}
