// Fill out your copyright notice in the Description page of Project Settings.

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
