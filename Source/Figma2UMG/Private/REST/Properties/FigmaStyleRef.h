// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaReference.h"

#include "FigmaStyleRef.generated.h"

UENUM()
enum class EFigmaStyleType
{
	FILL,
	TEXT,
	EFFECT,
	GRID
};

USTRUCT()
struct FIGMA2UMG_API FFigmaStyleRef : public FFigmaReference
{
public:
	GENERATED_BODY()

protected:
	UPROPERTY()
	EFigmaStyleType StyleType = EFigmaStyleType::FILL;
};
