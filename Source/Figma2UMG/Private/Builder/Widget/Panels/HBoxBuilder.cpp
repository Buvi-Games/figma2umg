// Copyright 2024 Buvi Games. All Rights Reserved.


#include "HBoxBuilder.h"

#include "Figma2UMGModule.h"

void UHBoxBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch)
{
    Box = Patch<UHorizontalBox>(WidgetBlueprint->WidgetTree, WidgetToPatch);

    Insert(WidgetBlueprint->WidgetTree, WidgetToPatch, Box);
    Setup();
    PatchAndInsertChildren(WidgetBlueprint, Box);
}

void UHBoxBuilder::SetWidget(const TObjectPtr<UWidget>& InWidget)
{
    Super::SetWidget(InWidget);
    Box = Cast<UHorizontalBox>(Widget);
}

void UHBoxBuilder::ResetWidget()
{
	Super::ResetWidget();
	Box = nullptr;
}

void UHBoxBuilder::Setup() const
{
    UE_LOG_Figma2UMG(Warning, TEXT("[UHBoxBuilder::Setup] TODO."));
}
