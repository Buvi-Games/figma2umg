// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FigmaEnums.h"

#include "FigmaLayoutConstraint.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaLayoutConstraint
{
public:
	GENERATED_BODY()

	UPROPERTY()
	EFigmaLayoutConstraintVertical Vertical;

	UPROPERTY()
	EFigmaLayoutConstraintHorizontal Horizontal;
};
