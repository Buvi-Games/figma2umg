// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FigmaRectangle.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaRectangle
{
public:
	GENERATED_BODY()

	FVector2D GetPosition() const
	{
		return FVector2D(X, Y);
	}
	FVector2D GetSize() const
	{
		return FVector2D(Width, Height);
	}

	UPROPERTY()
	float X;

	UPROPERTY()
	float Y;

	UPROPERTY()
	float Width;

	UPROPERTY()
	float Height;
};
