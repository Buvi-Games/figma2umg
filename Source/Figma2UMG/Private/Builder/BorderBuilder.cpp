// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/BorderBuilder.h"

#include "Components/Border.h"
#include "Parser/Properties/FigmaPaint.h"

void FBorderBuilder::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	if (!Function.IsBound())
		return;

	if (Border)
	{
		Function.ExecuteIfBound(*Border);
	}
}

void FBorderBuilder::SetupBorder(const TArray<FFigmaPaint>& Fills, const TArray<FFigmaPaint>& Strokes, float InStrokeWeight, const EFigmaStrokeAlign& InStrokeAlign, const FVector4& InCornerRadii, const float InCornerSmoothing)
{
	if (!Fills.IsEmpty())
	{
		Fill = &Fills[0];
	}

	if (!Strokes.IsEmpty())
	{
		Stroke = &Strokes[0];
	}
	StrokeWeight = InStrokeWeight;

	CornerRadii = InCornerRadii;
	CornerSmoothing = InCornerSmoothing;
}

TObjectPtr<UWidget> FBorderBuilder::GetTopWidget() const
{
	return Border;
}

TObjectPtr<UPanelWidget> FBorderBuilder::GetContainerWidget() const
{
	return nullptr;
}

TObjectPtr<UWidget> FBorderBuilder::Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName)
{
	Border = Cast<UBorder>(WidgetToPatch);
	if (Border)
	{
		if (Border->GetName() != WidgetName)
		{
			Border->Rename(*WidgetName);
		}
	}
	else
	{
		Border = NewObject<UBorder>(AssetOuter, *WidgetName);
		if (WidgetToPatch)
		{
			Border->SetContent(WidgetToPatch);
		}
	}

	SetFill();
	SetStroke();
	SetCorner();

	return Border;
}

void FBorderBuilder::SetFill() const
{
	if (Border)
	{
		if (Fill)
		{
			Border->SetBrushColor(Fill->Color.GetLinearColor());
		}
		else
		{
			Border->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
		}
	}
}

void FBorderBuilder::SetStroke() const
{
	if (Border)
	{
		if(Stroke)
		{
			FSlateBrush Brush = Border->Background;

			Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
			Brush.OutlineSettings.Color = Stroke->Color.GetLinearColor();
			Brush.OutlineSettings.Width = StrokeWeight;

			Border->SetBrush(Brush);
		}
		else
		{
			FSlateBrush Brush = Border->Background;
			Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
			Border->SetBrush(Brush);
			
		}
	}
}

void FBorderBuilder::SetCorner() const
{
	if (Border && Border->Background.GetDrawType() == ESlateBrushDrawType::RoundedBox)
	{
		FSlateBrush Brush = Border->Background;

		Brush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		Brush.OutlineSettings.CornerRadii = CornerRadii;
		
		Border->SetBrush(Brush);
	}
}

void FBorderBuilder::Reset()
{
	Border = nullptr;
}
