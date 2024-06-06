// Copyright 2024 Buvi Games. All Rights Reserved.


#include "SizeBoxWidgetBuilder.h"

#include "FigmaImportSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Nodes/FigmaGroup.h"
#include "Parser/Nodes/FigmaNode.h"


TObjectPtr<UWidget> USizeBoxWidgetBuilder::PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	Widget = Cast<USizeBox>(WidgetToPatch);
	const FString WidgetName = Node->GetUniqueName();
	if (Widget)
	{
		UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<USizeBox>(WidgetName) : nullptr;
		if (ClassOverride && Widget->GetClass() != ClassOverride)
		{
			USizeBox* NewSizeBox = IWidgetOwner::NewWidget<USizeBox>(WidgetTree, *WidgetName, ClassOverride);
			NewSizeBox->SetContent(Widget->GetContent());
			Widget = NewSizeBox;
		}
		IWidgetOwner::TryRenameWidget(WidgetName, Widget);
	}
	else
	{
		Widget = IWidgetOwner::NewWidget<USizeBox>(WidgetTree, *WidgetName);

		if (WidgetToPatch)
		{
			Widget->SetContent(WidgetToPatch);
		}
	}

	Setup();
	PatchPreInsertChild(WidgetTree, Widget);

	return Widget;
}

void USizeBoxWidgetBuilder::Setup()
{
	EFigmaLayoutSizing LayoutSizingHorizontal;
	EFigmaLayoutSizing LayoutSizingVertical;
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
	if(const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
	{
		LayoutSizingHorizontal = FigmaGroup->LayoutSizingHorizontal;
		LayoutSizingVertical = FigmaGroup->LayoutSizingVertical;
		FixedWidth = FigmaGroup->AbsoluteBoundingBox.Width;
		FixedHeight = FigmaGroup->AbsoluteBoundingBox.Height;
	}
}
