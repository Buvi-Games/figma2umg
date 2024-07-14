// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FigmaReference.h"

#include "FigmaStyleRef.generated.h"

UENUM()
enum class EFigmaStyleType
{
	FILL,
	FILLS,
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
