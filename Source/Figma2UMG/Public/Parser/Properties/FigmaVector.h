// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"

#include "FigmaVector.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaVector
{
public:
	GENERATED_BODY()

	FVector2D ToVector2D() const
	{
		return FVector2D(X, Y);
	}

	UPROPERTY()
	double X;

	UPROPERTY()
	double Y;
};

