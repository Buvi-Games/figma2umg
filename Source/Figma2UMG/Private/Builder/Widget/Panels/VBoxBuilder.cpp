// Copyright 2024 Buvi Games. All Rights Reserved.


#include "VBoxBuilder.h"

#include "Figma2UMGModule.h"

void UVBoxBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch)
{
    Box = Patch<UVerticalBox>(WidgetBlueprint->WidgetTree, WidgetToPatch);

    Insert(WidgetBlueprint->WidgetTree, WidgetToPatch, Box);

    PatchAndInsertChildren(WidgetBlueprint, Box);
}

void UVBoxBuilder::SetWidget(const TObjectPtr<UWidget>& InWidget)
{
    Super::SetWidget(InWidget);
    Box = Cast<UVerticalBox>(Widget);
}

void UVBoxBuilder::ResetWidget()
{
	Super::ResetWidget();
    Box = nullptr;
}

void UVBoxBuilder::Setup() const
{
    UE_LOG_Figma2UMG(Warning, TEXT("[UVBoxBuilder::Setup] TODO."));
}
