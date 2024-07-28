// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Parser/Properties/FigmaEnums.h"

#include "FigmaInstanceSwapPreferredValue.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaInstanceSwapPreferredValue
{
public:
	GENERATED_BODY()

	UPROPERTY()
	ENodeTypes Type;

	UPROPERTY()
	FString Key;
};
