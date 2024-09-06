// Copyright 2024 Buvi Games. All Rights Reserved.


#include "HBoxBuilder.h"

#include "Figma2UMGModule.h"
#include "Parser/Nodes/FigmaGroup.h"

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

bool UHBoxBuilder::GetSizeValue(FVector2D& Size, bool& SizeToContent) const
{
    bool IsValid = Super::GetSizeValue(Size, SizeToContent);
    if (IsValid)
    {
        if (const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
        {
            SizeToContent = FigmaGroup->LayoutSizingHorizontal == EFigmaLayoutSizing::FILL || FigmaGroup->LayoutSizingHorizontal == EFigmaLayoutSizing::HUG;
        }
    }

    return IsValid;
}
