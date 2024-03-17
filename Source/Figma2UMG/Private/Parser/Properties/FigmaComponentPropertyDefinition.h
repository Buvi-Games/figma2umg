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

	bool IsButton() const;
};

static FString Hovered = FString("Hovered");
static FString Pressed = FString("Pressed");
inline bool FFigmaComponentPropertyDefinition::IsButton() const
{
	if (Type != EFigmaComponentPropertyType::VARIANT)
		return false;

	const bool hasHovered = VariantOptions.Find(Hovered) != INDEX_NONE;
	const bool hasPressed = VariantOptions.Find(Pressed) != INDEX_NONE;
	return (hasHovered && hasPressed);
}
