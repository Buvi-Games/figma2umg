// MIT License
// Copyright (c) 2024 Buvi Games


#include "Builder/Widget/Panels/VBoxBuilder.h"

#include "Figma2UMGModule.h"
#include "Parser/Nodes/FigmaGroup.h"

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

bool UVBoxBuilder::GetSizeValue(FVector2D& Size, bool& SizeToContent) const
{
    bool IsValid = Super::GetSizeValue(Size, SizeToContent);
    if (IsValid)
    {
        if (const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
        {
            SizeToContent = FigmaGroup->LayoutSizingVertical == EFigmaLayoutSizing::FILL || FigmaGroup->LayoutSizingVertical == EFigmaLayoutSizing::HUG;
        }
    }

    return IsValid;
}
