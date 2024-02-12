// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/FigmaGroup.h"

#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"


FVector2D UFigmaGroup::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

void UFigmaGroup::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	Builder.ForEach(Function);
}

TObjectPtr<UWidget> UFigmaGroup::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	Builder.Border = Cast<UBorder>(WidgetToPatch);
	Builder.Canvas = nullptr;
	if (Builder.Border)
	{
		if (Builder.Border->GetName() != GetUniqueName())
		{
			Builder.Border->Rename(*GetUniqueName());
		}
		Builder.Canvas = Cast<UCanvasPanel>(Builder.Border->GetContent());
	}
	else
	{
		Builder.Border = NewObject<UBorder>(GetAssetOuter(), *GetUniqueName());
	}

	if (!Builder.Canvas)
	{
		Builder.Canvas = NewObject<UCanvasPanel>(GetAssetOuter());
		Builder.Border->SetContent(Builder.Canvas);
	}

	if (Fills.Num() > 0 && Fills[0].Visible)
	{
		Builder.SetFill(Fills[0]);
	}
	else
	{
		Builder.Border->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
	}

	if (Strokes.Num() > 0 && Strokes[0].Visible)
	{
		Builder.SetStroke(Strokes[0], StrokeWeight, StrokeAlign);
	}

	if (RectangleCornerRadii.Num() == 4)
	{
		Builder.SetCorner(RectangleCornerRadii[0], RectangleCornerRadii[1], RectangleCornerRadii[2], RectangleCornerRadii[3], CornerSmoothing);
	}
	else
	{
		Builder.SetCorner(CornerRadius, CornerRadius, CornerRadius, CornerRadius, CornerSmoothing);
	}


	return Builder.Border;
}

void UFigmaGroup::PostInsert() const
{
	TObjectPtr<UWidget> TopWidget = GetTopWidget();
	if (!TopWidget)
		return;

	IWidgetOwner::PostInsert();

	if (UCanvasPanelSlot* CanvasSlot = TopWidget->Slot ? Cast<UCanvasPanelSlot>(TopWidget->Slot) : nullptr)
	{
		CanvasSlot->SetSize(AbsoluteBoundingBox.GetSize());
	}
}

void UFigmaGroup::Reset()
{
	Builder.Reset();
}

TObjectPtr<UWidget> UFigmaGroup::GetTopWidget() const
{
	return Builder.Border;
}

FVector2D UFigmaGroup::GetTopWidgetPosition() const
{
	return GetPosition();
}

TObjectPtr<UPanelWidget> UFigmaGroup::GetContainerWidget() const
{
	return Builder.Canvas;
}
