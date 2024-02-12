// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/BorderCanvasBuilder.h"

#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Parser/Properties/FigmaPaint.h"

void FBorderCanvasBuilder::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	if (!Function.IsBound())
		return;

	if (Border)
	{
		Function.ExecuteIfBound(*Border);
	}

	if (Canvas)
	{
		Function.ExecuteIfBound(*Canvas);
	}
}

void FBorderCanvasBuilder::SetFill(const FFigmaPaint& Fill)
{
	if (Border)
	{
		Border->SetBrushColor(Fill.Color.GetLinearColor());
	}
}

void FBorderCanvasBuilder::SetStroke(const FFigmaPaint& Stroke, float StrokeWeight, FString StrokeAlign)
{
	if (Border)
	{
		FSlateBrush Brush = Border->Background;

		Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
		Brush.OutlineSettings.Color = Stroke.Color.GetLinearColor();
		Brush.OutlineSettings.Width = StrokeWeight;

		Border->SetBrush(Brush);
	}
}

void FBorderCanvasBuilder::SetCorner(const float TopLeftRadius, const float TopRightRadius, const float BottomRightRadius, const float BottomLeftRadius, const float CornerSmoothing)
{
	if (Border)
	{
		FSlateBrush Brush = Border->Background;
		
		Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
		Brush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		Brush.OutlineSettings.CornerRadii = FVector4(TopLeftRadius, TopRightRadius, BottomRightRadius, BottomLeftRadius);
		
		Border->SetBrush(Brush);
	}
}

void FBorderCanvasBuilder::Reset()
{
	Border = nullptr;
	Canvas = nullptr;
}
