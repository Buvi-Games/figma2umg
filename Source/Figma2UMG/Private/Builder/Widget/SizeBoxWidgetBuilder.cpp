// Copyright 2024 Buvi Games. All Rights Reserved.


#include "SizeBoxWidgetBuilder.h"

#include "FigmaImportSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Nodes/FigmaGroup.h"
#include "Parser/Nodes/FigmaInstance.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Nodes/Vectors/FigmaText.h"


void USizeBoxWidgetBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	Widget = Cast<USizeBox>(WidgetToPatch);
	const FString NodeName = Node->GetNodeName();
	const FString WidgetName = "SizeBox-" + Node->GetUniqueName();
	if (Widget)
	{
		UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<USizeBox>(NodeName) : nullptr;
		if (ClassOverride && Widget->GetClass() != ClassOverride)
		{
			USizeBox* NewSizeBox = UFigmaImportSubsystem::NewWidget<USizeBox>(WidgetTree, NodeName, WidgetName, ClassOverride);
			NewSizeBox->SetContent(Widget->GetContent());
			Widget = NewSizeBox;
		}
		UFigmaImportSubsystem::TryRenameWidget(WidgetName, Widget);
	}
	else
	{
		Widget = UFigmaImportSubsystem::NewWidget<USizeBox>(WidgetTree, NodeName, WidgetName);

		if (WidgetToPatch)
		{
			Widget->SetContent(WidgetToPatch);
		}
	}

	Insert(WidgetTree, WidgetToPatch, Widget);
	Setup();
	PatchAndInsertChild(WidgetTree, Widget);
}

TObjectPtr<UContentWidget> USizeBoxWidgetBuilder::GetContentWidget() const
{
	return Widget;
}

void USizeBoxWidgetBuilder::GetPaddingValue(FMargin& Padding) const
{
	Padding.Left = 0.0f;
	Padding.Right = 0.0f;
	Padding.Top = 0.0f;
	Padding.Bottom = 0.0f;
}

void USizeBoxWidgetBuilder::Setup() const
{
	EFigmaLayoutSizing LayoutSizingHorizontal = EFigmaLayoutSizing::FILL;
	EFigmaLayoutSizing LayoutSizingVertical = EFigmaLayoutSizing::FILL;
	float FixedWidth;
	float FixedHeight;
	GetValues(LayoutSizingHorizontal, LayoutSizingVertical, FixedWidth, FixedHeight);

	if (LayoutSizingHorizontal == EFigmaLayoutSizing::FIXED)
	{
		Widget->SetWidthOverride(FixedWidth);
	}

	if (LayoutSizingVertical == EFigmaLayoutSizing::FIXED)
	{
		Widget->SetHeightOverride(FixedHeight);
	}
}

void USizeBoxWidgetBuilder::GetValues(EFigmaLayoutSizing& LayoutSizingHorizontal, EFigmaLayoutSizing& LayoutSizingVertical, float& FixedWidth, float& FixedHeight) const
{
	if (const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
	{
		LayoutSizingHorizontal = FigmaGroup->LayoutSizingHorizontal;
		LayoutSizingVertical = FigmaGroup->LayoutSizingVertical;
		FixedWidth = FigmaGroup->AbsoluteBoundingBox.Width;
		FixedHeight = FigmaGroup->AbsoluteBoundingBox.Height;
	}
	else if (const UFigmaInstance* FigmaInstance = Cast<UFigmaInstance>(Node))
	{
		LayoutSizingHorizontal = FigmaInstance->LayoutSizingHorizontal;
		LayoutSizingVertical = FigmaInstance->LayoutSizingVertical;
		FixedWidth = FigmaInstance->AbsoluteBoundingBox.Width;
		FixedHeight = FigmaInstance->AbsoluteBoundingBox.Height;
	}
	else if (const UFigmaText* FigmaText = Cast<UFigmaText>(Node))
	{
		LayoutSizingHorizontal = FigmaText->LayoutSizingHorizontal;
		LayoutSizingVertical = FigmaText->LayoutSizingVertical;
		FixedWidth = FigmaText->AbsoluteBoundingBox.Width;
		FixedHeight = FigmaText->AbsoluteBoundingBox.Height;
	}
}
