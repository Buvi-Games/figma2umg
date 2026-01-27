// MIT License
// Copyright (c) 2024 Buvi Games

#include "Builder/Widget/Panels/GridBuilder.h"

#include "Components/GridSlot.h"
#include "Components/Spacer.h"
#include "Parser/Nodes/FigmaGroup.h"

namespace
{
	EHorizontalAlignment ConvertPrimaryAxisAlign(EFigmaPrimaryAxisAlignItems Align)
	{
		switch (Align)
		{
		case EFigmaPrimaryAxisAlignItems::MIN:
			return EHorizontalAlignment::HAlign_Left;
		case EFigmaPrimaryAxisAlignItems::CENTER:
			return EHorizontalAlignment::HAlign_Center;
		case EFigmaPrimaryAxisAlignItems::MAX:
			return EHorizontalAlignment::HAlign_Right;
		case EFigmaPrimaryAxisAlignItems::SPACE_BETWEEN:
			return EHorizontalAlignment::HAlign_Fill;
		default:
			return EHorizontalAlignment::HAlign_Left;
		}
	}

	EVerticalAlignment ConvertCounterAxisAlign(EFigmaCounterAxisAlignItems Align)
	{
		switch (Align)
		{
		case EFigmaCounterAxisAlignItems::MIN:
			return EVerticalAlignment::VAlign_Top;
		case EFigmaCounterAxisAlignItems::CENTER:
			return EVerticalAlignment::VAlign_Center;
		case EFigmaCounterAxisAlignItems::MAX:
			return EVerticalAlignment::VAlign_Bottom;
		case EFigmaCounterAxisAlignItems::BASELINE:
			return EVerticalAlignment::VAlign_Top; // No baseline support in UE5, default to top
		default:
			return EVerticalAlignment::VAlign_Top;
		}
	}
}

void UGridBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch, TMap<FString, int32>& NameTracker)
{
	GridPanel = Patch<UGridPanel>(WidgetBlueprint->WidgetTree, WidgetToPatch, NameTracker);
	Insert(WidgetBlueprint->WidgetTree, WidgetToPatch, GridPanel);
	Setup();
	PatchAndInsertChildren(WidgetBlueprint, GridPanel, NameTracker);

	// After children are inserted, configure their grid positions
	ConfigureChildGridPositions();
}

void UGridBuilder::SetWidget(const TObjectPtr<UWidget>& InWidget)
{
	Super::SetWidget(InWidget);
	GridPanel = Cast<UGridPanel>(Widget);
}

void UGridBuilder::ResetWidget()
{
	Super::ResetWidget();
	GridPanel = nullptr;
}

void UGridBuilder::Setup() const
{
	if (!GridPanel || !Node)
	{
		return;
	}

	const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node);
	if (!FigmaGroup)
	{
		return;
	}

	ConfigureGridFillRules();
}

void UGridBuilder::ConfigureGridFillRules() const
{
	if (!GridPanel || !Node)
	{
		return;
	}

	const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node);
	if (!FigmaGroup)
	{
		return;
	}

	// Parse and configure column fill rules
	TArray<FFigmaGridTrackSizing> ColumnSizing = FigmaGroup->GetParsedColumnSizing();
	for (int32 i = 0; i < ColumnSizing.Num(); i++)
	{
		if (ColumnSizing[i].Type == EFigmaGridTrackType::Fractional)
		{
			GridPanel->SetColumnFill(i, ColumnSizing[i].Value);
		}
		else if (ColumnSizing[i].Type == EFigmaGridTrackType::Auto)
		{
			// Auto columns don't need explicit fill, they size to content
			GridPanel->SetColumnFill(i, 0.0f);
		}
		else if (ColumnSizing[i].Type == EFigmaGridTrackType::Fixed)
		{
			// Fixed size columns - set to 0 fill (no stretch)
			GridPanel->SetColumnFill(i, 0.0f);
		}
	}

	// Parse and configure row fill rules
	TArray<FFigmaGridTrackSizing> RowSizing = FigmaGroup->GetParsedRowSizing();
	for (int32 i = 0; i < RowSizing.Num(); i++)
	{
		if (RowSizing[i].Type == EFigmaGridTrackType::Fractional)
		{
			GridPanel->SetRowFill(i, RowSizing[i].Value);
		}
		else if (RowSizing[i].Type == EFigmaGridTrackType::Auto)
		{
			GridPanel->SetRowFill(i, 0.0f);
		}
		else if (RowSizing[i].Type == EFigmaGridTrackType::Fixed)
		{
			GridPanel->SetRowFill(i, 0.0f);
		}
	}
}

void UGridBuilder::ConfigureChildGridPositions() const
{
	if (!GridPanel || !Node)
	{
		return;
	}

	const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node);
	if (!FigmaGroup)
	{
		return;
	}

	const int32 ColumnCount = FigmaGroup->GridColumnCount > 0 ? FigmaGroup->GridColumnCount : 1;
	const float ColumnGap = FigmaGroup->GridColumnGap;
	const float RowGap = FigmaGroup->GridRowGap;
	const int32 RowCount = FigmaGroup->GridRowCount > 0 ? FigmaGroup->GridRowCount : 1;

	// Get alignment settings from Figma group
	const EHorizontalAlignment HAlign = ConvertPrimaryAxisAlign(FigmaGroup->PrimaryAxisAlignItems);
	const EVerticalAlignment VAlign = ConvertCounterAxisAlign(FigmaGroup->CounterAxisAlignItems);

	// Iterate through actual children (excluding spacers)
	int32 ChildIndex = 0;
	for (int32 i = 0; i < GridPanel->GetChildrenCount(); i++)
	{
		UWidget* ChildWidget = GridPanel->GetChildAt(i);
		if (!ChildWidget)
		{
			continue;
		}

		// Skip spacers
		if (ChildWidget->IsA<USpacer>())
		{
			continue;
		}

		UGridSlot* GridSlot = Cast<UGridSlot>(ChildWidget->Slot);
		if (!GridSlot)
		{
			continue;
		}

		// Calculate row and column based on child index and column count
		int32 Row, Column;
		CalculateChildGridPosition(ChildIndex, Row, Column);

		GridSlot->SetRow(Row);
		GridSlot->SetColumn(Column);

		// Apply gap as padding to simulate CSS Grid gap
		// Distribute gap evenly between cells
		FMargin GapPadding;
		GapPadding.Left = (Column > 0) ? ColumnGap * 0.5f : 0.0f;
		GapPadding.Right = (Column < ColumnCount - 1) ? ColumnGap * 0.5f : 0.0f;
		GapPadding.Top = (Row > 0) ? RowGap * 0.5f : 0.0f;
		GapPadding.Bottom = (Row < RowCount - 1) ? RowGap * 0.5f : 0.0f;

		GridSlot->SetPadding(GapPadding);

		// Apply alignment from Figma group settings
		GridSlot->SetHorizontalAlignment(HAlign);
		GridSlot->SetVerticalAlignment(VAlign);

		ChildIndex++;
	}
}

void UGridBuilder::CalculateChildGridPosition(int32 ChildIndex, int32& OutRow, int32& OutColumn) const
{
	const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node);
	if (!FigmaGroup)
	{
		OutRow = 0;
		OutColumn = 0;
		return;
	}

	const int32 ColumnCount = FigmaGroup->GridColumnCount > 0 ? FigmaGroup->GridColumnCount : 1;

	// Simple row-major ordering (CSS Grid default flow)
	OutRow = ChildIndex / ColumnCount;
	OutColumn = ChildIndex % ColumnCount;
}

bool UGridBuilder::GetSizeValue(FVector2D& Size, bool& SizeToContent) const
{
	bool IsValid = Super::GetSizeValue(Size, SizeToContent);
	if (IsValid)
	{
		if (const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
		{
			// Grid panels should size to content when using HUG or FILL layout sizing
			SizeToContent = FigmaGroup->LayoutSizingHorizontal == EFigmaLayoutSizing::FILL ||
			                FigmaGroup->LayoutSizingHorizontal == EFigmaLayoutSizing::HUG ||
			                FigmaGroup->LayoutSizingVertical == EFigmaLayoutSizing::FILL ||
			                FigmaGroup->LayoutSizingVertical == EFigmaLayoutSizing::HUG;
		}
	}

	return IsValid;
}
