// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"

#include "FigmaColor.h"
#include "FigmaColorStop.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaColorStop
{
public:
	GENERATED_BODY()

	UPROPERTY()
	FFigmaColor Color;

	UPROPERTY()
	double Position = 0.0f;
};
