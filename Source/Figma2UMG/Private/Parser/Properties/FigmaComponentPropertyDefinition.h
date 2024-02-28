// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaEnums.h"
#include "FigmaInstanceSwapPreferredValue.h"

#include "FigmaComponentPropertyDefinition.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaComponentPropertyDefinition
{
public:
	GENERATED_BODY()

	UPROPERTY()
	EFigmaComponentPropertyType Type;

	UPROPERTY()
	FString DefaultValue;

	UPROPERTY()
	TArray<FString> VariantOptions;

	UPROPERTY()
	TArray<FFigmaInstanceSwapPreferredValue> PreferredValues;
};
