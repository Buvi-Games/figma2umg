// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/WidgetOwner.h"

#include "BorderCanvasBuilder.generated.h"

struct FFigmaPaint;
class UCanvasPanel;
class UBorder;

USTRUCT()
struct FIGMA2UMG_API FBorderCanvasBuilder
{
public:
	GENERATED_BODY()

	void ForEach(const IWidgetOwner::FOnEachFunction& Function);

	void SetFill (const FFigmaPaint& Fill);
	void SetStroke(const FFigmaPaint& Stroke, float StrokeWeight, FString StrokeAlign);
	void SetCorner(const float TopLeftRadius, const float TopRightRadius, const float BottomRightRadius, const float BottomLeftRadius, const float CornerSmoothing);

	UPROPERTY()
	TObjectPtr<UBorder> Border = nullptr;

	UPROPERTY()
	TObjectPtr<UCanvasPanel> Canvas = nullptr;
};
