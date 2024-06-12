// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FigmaVector.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaVector
{
public:
	GENERATED_BODY()

	UPROPERTY()
	double X;

	UPROPERTY()
	double Y;
};
