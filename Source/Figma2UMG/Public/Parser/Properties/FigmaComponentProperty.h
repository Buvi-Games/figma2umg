// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"

#include "FigmaComponentPropertyDefinition.h"

#include "FigmaComponentProperty.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaComponentProperty
{
public:
	GENERATED_BODY()

	UPROPERTY()
	EFigmaComponentPropertyType Type;

	UPROPERTY()
	FString Value;
};
