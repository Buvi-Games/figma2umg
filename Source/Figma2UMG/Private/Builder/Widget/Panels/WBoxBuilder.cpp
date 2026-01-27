// MIT License
// Copyright (c) 2024 Buvi Games


#include "Builder/Widget/Panels/WBoxBuilder.h"

#include "Figma2UMGModule.h"

void UWBoxBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch, TMap<FString, int32>& NameTracker)
{
    Box = Patch<UWrapBox>(WidgetBlueprint->WidgetTree, WidgetToPatch, NameTracker);

    PatchAndInsertChildren(WidgetBlueprint, Box, NameTracker);

    Insert(WidgetBlueprint->WidgetTree, WidgetToPatch, Box);
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
