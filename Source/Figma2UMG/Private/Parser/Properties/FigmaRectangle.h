// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FigmaRectangle.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaRectangle
{
public:
	GENERATED_BODY()

	FVector2D GetPosition(const float Rotation) const
	{
		const FVector2D Position(X, Y);
	    return Position.GetRotated(-Rotation);
	}

	FVector2D GetSize(float Rotation) const
	{
		const float Radians = FMath::DegreesToRadians(Rotation);
	    const float CosAngle = FMath::Abs(FMath::Cos(Radians));
		const float SinAngle = FMath::Abs(FMath::Sin(Radians));

		const float RotatedWidth = Width * CosAngle + Height * SinAngle;
		const float RotatedHeight = Width * SinAngle + Height * CosAngle;

	    return FVector2D(RotatedWidth, RotatedHeight);
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
