// MIT License
// Copyright (c) 2024 Buvi Games


#include "Builder/Widget/BorderWidgetBuilder.h"

#include "FigmaImportSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Parser/Nodes/FigmaGroup.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Nodes/FigmaSection.h"


void UBorderWidgetBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch)
{
	if (const USizeBox* SizeBoxWrapper = Cast<USizeBox>(WidgetToPatch))
	{
		Widget = Cast<UBorder>(SizeBoxWrapper->GetContent());
	}
	else
	{
		Widget = Cast<UBorder>(WidgetToPatch);
	}	

	const FString NodeName = Node->GetNodeName();
	const FString WidgetName = "Border-" + Node->GetUniqueName();
	if (Widget)
	{
		UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<UBorder>(NodeName) : nullptr;
		if (ClassOverride && Widget->GetClass() != ClassOverride)
		{
			UBorder* NewBorder = UFigmaImportSubsystem::NewWidget<UBorder>(WidgetBlueprint->WidgetTree, NodeName, WidgetName, ClassOverride);
			NewBorder->SetContent(Widget->GetContent());
			Widget = NewBorder;
		}
		UFigmaImportSubsystem::TryRenameWidget(WidgetName, Widget);
	}
	else
	{
		Widget = UFigmaImportSubsystem::NewWidget<UBorder>(WidgetBlueprint->WidgetTree, NodeName, WidgetName);

		if (WidgetToPatch)
		{
			Widget->SetContent(WidgetToPatch);
		}
	}

	Insert(WidgetBlueprint->WidgetTree, WidgetToPatch, Widget);

	Setup();

	PatchAndInsertChild(WidgetBlueprint, Widget);
}

void UBorderWidgetBuilder::SetWidget(const TObjectPtr<UWidget>& InWidget)
{
	Widget = Cast<UBorder>(InWidget);
	SetChildWidget(Widget);
}

void UBorderWidgetBuilder::ResetWidget()
{
	Super::ResetWidget();
	Widget = nullptr;
}

TObjectPtr<UContentWidget> UBorderWidgetBuilder::GetContentWidget() const
{
	return Widget;
}

void UBorderWidgetBuilder::GetPaddingValue(FMargin& Padding) const
{
	Padding.Left = 0.0f;
	Padding.Right = 0.0f;
	Padding.Top = 0.0f;
	Padding.Bottom = 0.0f;
}

bool UBorderWidgetBuilder::GetAlignmentValues(EHorizontalAlignment& HorizontalAlignment, EVerticalAlignment& VerticalAlignment) const
{
	HorizontalAlignment = HAlign_Fill;
	VerticalAlignment = VAlign_Fill;
	return true;
}

void UBorderWidgetBuilder::Setup() const
{
	FSlateBrush Brush = Widget->Background;
	if (Node->IsA<UFigmaSection>())
	{
		Brush.DrawAs = ESlateBrushDrawType::Image;
	}
	else
	{
		Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
	}
	SetBrush(Widget, Brush);

	if (const UFigmaSection* FigmaSection = Cast<UFigmaSection>(Node))
	{
		SetFill(FigmaSection->Fills);
		SetStroke(Widget, FigmaSection->Strokes, FigmaSection->StrokeWeight);
	}
	else if(const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
	{
		SetFill(FigmaGroup->Fills);
		SetStroke(Widget, FigmaGroup->Strokes, FigmaGroup->StrokeWeight);

		const FVector4 Corners = FigmaGroup->RectangleCornerRadii.Num() == 4 ? FVector4(FigmaGroup->RectangleCornerRadii[0], FigmaGroup->RectangleCornerRadii[1], FigmaGroup->RectangleCornerRadii[2], FigmaGroup->RectangleCornerRadii[3])
																			 : FVector4(FigmaGroup->CornerRadius, FigmaGroup->CornerRadius, FigmaGroup->CornerRadius, FigmaGroup->CornerRadius);
		SetCorner(Widget, Corners);
	}
}
