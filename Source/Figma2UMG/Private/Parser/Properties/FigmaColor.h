// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FigmaColor.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaColor
{
public:
	GENERATED_BODY()

	UPROPERTY()
	float R = 0.0f;

	UPROPERTY()
	float G = 0.0f;

	UPROPERTY()
	float B = 0.0f;

	UPROPERTY()
	float A = 0.0f;
};
