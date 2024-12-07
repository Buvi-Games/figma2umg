// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FigmaEnums.h"

#include "FigmaNavigation.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaNavigation
{
public:
	GENERATED_BODY()

	UPROPERTY()
	EFigmaNavigationType Type;
};
