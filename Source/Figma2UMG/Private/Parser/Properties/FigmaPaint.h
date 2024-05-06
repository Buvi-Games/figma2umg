// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaBlendMode.h"
#include "FigmaColor.h"
#include "FigmaColorStop.h"
#include "FigmaEnums.h"
#include "FigmaImageFilters.h"
#include "FigmaTransform.h"
#include "FigmaVariableAlias.h"
#include "FigmaVector.h"

#include "FigmaPaint.generated.h"


USTRUCT()
struct FIGMA2UMG_API FFigmaPaint
{
public:
	GENERATED_BODY()

	FLinearColor GetLinearColor() const
	{
		return FLinearColor(Color.R, Color.G, Color.B, Opacity);
	}

	UPROPERTY()
	EPaintTypes Type;

	UPROPERTY()
	bool Visible = true;

	UPROPERTY()
	float Opacity = 1.0f;

	UPROPERTY()
	FFigmaColor Color;

	UPROPERTY()
	EFigmaBlendMode blendMode;

	UPROPERTY()
	FFigmaVector GradientHandlePositions;

	UPROPERTY()
	FFigmaColorStop GradientStops;

	UPROPERTY()
	EScaleMode ScaleMode;

	UPROPERTY()
	FFigmaTransform ImageTransform;

	UPROPERTY()
	float ScalingFactor;

	UPROPERTY()
	float Rotation;

	UPROPERTY()
	FString ImageRef;

	UPROPERTY()
	FFigmaImageFilters Filters;

	UPROPERTY()
	FString GifRef;

	UPROPERTY()
	TMap<FString, FFigmaVariableAlias> BoundVariables;
};
