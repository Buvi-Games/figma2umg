// Copyright 2024 Buvi Games. All Rights Reserved.


#include "WBoxBuilder.h"

#include "Figma2UMGModule.h"

void UWBoxBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
    Box = Patch<UWrapBox>(WidgetTree, WidgetToPatch);

    PatchAndInsertChildren(WidgetTree, Box);

    Insert(WidgetTree, WidgetToPatch, Box);
}

void UWBoxBuilder::SetWidget(const TObjectPtr<UWidget>& InWidget)
{
    Super::SetWidget(InWidget);
    Box = Cast<UWrapBox>(Widget);
}

void UWBoxBuilder::ResetWidget()
{
	Super::ResetWidget();
    Box = nullptr;
}

void UWBoxBuilder::Setup() const
{
    UE_LOG_Figma2UMG(Warning, TEXT("[UWBoxBuilder::Setup] TODO."));
}
