// Copyright 2024 Buvi Games. All Rights Reserved.


#include "BorderWidgetBuilder.h"

#include "FigmaImportSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Nodes/FigmaGroup.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Nodes/FigmaSection.h"


void UBorderWidgetBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
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
	const FString WidgetName = Node->GetUniqueName();
	if (Widget)
	{
		UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<UBorder>(NodeName) : nullptr;
		if (ClassOverride && Widget->GetClass() != ClassOverride)
		{
			UBorder* NewBorder = UFigmaImportSubsystem::NewWidget<UBorder>(WidgetTree, NodeName, WidgetName, ClassOverride);
			NewBorder->SetContent(Widget->GetContent());
			Widget = NewBorder;
		}
		UFigmaImportSubsystem::TryRenameWidget(WidgetName, Widget);
	}
	else
	{
		Widget = UFigmaImportSubsystem::NewWidget<UBorder>(WidgetTree, NodeName, WidgetName);

		if (WidgetToPatch)
		{
			Widget->SetContent(WidgetToPatch);
		}
	}

	if (Widget)
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
		Widget->SetBrush(Brush);
	}

	Insert(WidgetTree, WidgetToPatch, Widget);

	Setup();

	PatchAndInsertChild(WidgetTree, Widget);
}

TObjectPtr<UContentWidget> UBorderWidgetBuilder::GetContentWidget()
{
	return Widget;
}

void UBorderWidgetBuilder::Setup() const
{
	if (const UFigmaSection* FigmaSection = Cast<UFigmaSection>(Node))
	{
		SetFill(FigmaSection->Fills);
		SetStroke(FigmaSection->Strokes, FigmaSection->StrokeWeight);
	}
	else if(const UFigmaGroup* FigmaGroup = Cast<UFigmaGroup>(Node))
	{
		SetFill(FigmaGroup->Fills);
		SetStroke(FigmaGroup->Strokes, FigmaGroup->StrokeWeight);

		const FVector4 Corners = FigmaGroup->RectangleCornerRadii.Num() == 4 ? FVector4(FigmaGroup->RectangleCornerRadii[0], FigmaGroup->RectangleCornerRadii[1], FigmaGroup->RectangleCornerRadii[2], FigmaGroup->RectangleCornerRadii[3])
																			 : FVector4(FigmaGroup->CornerRadius, FigmaGroup->CornerRadius, FigmaGroup->CornerRadius, FigmaGroup->CornerRadius);
		SetCorner(Corners);
	}
}

void UBorderWidgetBuilder::SetFill(const TArray<FFigmaPaint>& Fills) const
{
	if (Fills.Num() > 0 && Fills[0].Visible)
	{
		Widget->SetBrushColor(Fills[0].GetLinearColor());
	}
	else
	{
		Widget->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
	}
}

void UBorderWidgetBuilder::SetStroke(const TArray<FFigmaPaint>& Strokes, const float& StrokeWeight) const
{
	if (Strokes.Num() > 0 && Strokes[0].Visible)
	{
		FSlateBrush Brush = Widget->Background;
		Brush.OutlineSettings.Color = Strokes[0].GetLinearColor();
		Brush.OutlineSettings.Width = StrokeWeight;
		Widget->SetBrush(Brush);
	}
	else
	{
		FSlateBrush Brush = Widget->Background;
		Brush.OutlineSettings.Color = FLinearColor(1.0f, 1.0f, 1.0f, 0.0f);
		Brush.OutlineSettings.Width = 0.0f;
		Widget->SetBrush(Brush);
	}
}

void UBorderWidgetBuilder::SetCorner(const FVector4& CornerRadii) const
{
	if (Widget && Widget->Background.GetDrawType() == ESlateBrushDrawType::RoundedBox)
	{
		FSlateBrush Brush = Widget->Background;
		Brush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		Brush.OutlineSettings.CornerRadii = CornerRadii;
		Widget->SetBrush(Brush);
	}
}
