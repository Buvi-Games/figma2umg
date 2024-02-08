// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaBlendMode.h"
#include "FigmaColor.h"
#include "FigmaColorStop.h"
#include "FigmaImageFilters.h"
#include "FigmaTransform.h"
#include "FigmaVariableAlias.h"
#include "FigmaVector.h"

#include "FigmaPaint.generated.h"

UENUM()
enum class EPaintTypes
{
	SOLID,
	GRADIENT_LINEAR,
	GRADIENT_RADIAL,
	GRADIENT_ANGULAR,
	GRADIENT_DIAMOND,
	IMAGE,
	EMOJI,
	VIDEO,
};

UENUM()
enum class EScaleMode
{
	FILL,
	FIT,
	TILE,
	STRETCH,
};

USTRUCT()
struct FIGMA2UMG_API FFigmaPaint
{
public:
	GENERATED_BODY()

	UPROPERTY()
	EPaintTypes Type;

	UPROPERTY()
	bool Visible;

	UPROPERTY()
	float Opacity;

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
