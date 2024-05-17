// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FigmaImageFilters.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaImageFilters
{
public:
	GENERATED_BODY()

	UPROPERTY()
	float Exposure = 0.0f;

	UPROPERTY()
	float Contrast = 0.0f;

	UPROPERTY()
	float Saturation = 0.0f;

	UPROPERTY()
	float Temperature = 0.0f;

	UPROPERTY()
	float Tint = 0.0f;

	UPROPERTY()
	float Highlights = 0.0f;

	UPROPERTY()
	float Shadows = 0.0f;
};
