// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"

#include "FigmaColor.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaColor
{
public:
	GENERATED_BODY()

	UPROPERTY()
	float R = 1.0f;

	UPROPERTY()
	float G = 1.0f;

	UPROPERTY()
	float B = 1.0f;

	UPROPERTY()
	float A = 1.0f;
};
