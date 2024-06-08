// Copyright 2024 Buvi Games. All Rights Reserved.


#include "HBoxBuilder.h"

#include "Figma2UMGModule.h"

void UHBoxBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
    Box = Patch<UHorizontalBox>(WidgetTree, WidgetToPatch);

    Insert(WidgetTree, WidgetToPatch, Box);
    Setup();
    PatchAndInsertChildren(WidgetTree, Box);
}

void UHBoxBuilder::Setup() const
{
    UE_LOG_Figma2UMG(Warning, TEXT("[UHBoxBuilder::Setup] TODO."));
}
