// Copyright 2024 Buvi Games. All Rights Reserved.


#include "VBoxBuilder.h"

#include "Figma2UMGModule.h"

void UVBoxBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
    Box = Patch<UVerticalBox>(WidgetTree, WidgetToPatch);

    Insert(WidgetTree, WidgetToPatch, Box);

    PatchAndInsertChildren(WidgetTree, Box);
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
