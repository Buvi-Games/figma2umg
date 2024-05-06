// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FigmaColor.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaColor
{
public:
	GENERATED_BODY()

	UPROPERTY()
	float R = 0.0f;

	UPROPERTY()
	float G = 0.0f;

	UPROPERTY()
	float B = 0.0f;

	UPROPERTY()
	float A = 0.0f;
};
