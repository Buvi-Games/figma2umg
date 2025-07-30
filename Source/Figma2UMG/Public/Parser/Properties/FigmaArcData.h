// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"

#include "FigmaArcData.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaArcData
{
public:
	GENERATED_BODY()

	UPROPERTY()
	float StartingAngle = 0.0f;

	UPROPERTY()
	float EndingAngle = 0.0f;

	UPROPERTY()
	float InnerRadius = 0.0f;
};
