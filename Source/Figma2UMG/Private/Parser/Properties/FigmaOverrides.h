// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"

#include "FigmaOverrides.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaOverrides
{
public:
	GENERATED_BODY()

	UPROPERTY()
	FString Id;

	UPROPERTY()
	TArray<FString> OverriddenFields;
};
