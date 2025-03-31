// Copyright 2024 Buvi Games. All Rights Reserved.

#include "FigmaRectangle.h"

FVector2D FFigmaRectangle::GetPosition(const float Rotation) const
{
	if (Rotation == 0.0f)
	{
		return FVector2D(X, Y);
	}
		
	const FVector2D Center = GetCenter();
	const FVector2D Size = GetSize(-Rotation);

	const FVector2D UnRotatedTopLeft = Center - (Size * 0.5f);
	return UnRotatedTopLeft;
}

FVector2D FFigmaRectangle::GetSize(float Rotation) const
{
	const float Radians = FMath::DegreesToRadians(Rotation);
	const float CosAngle = FMath::Abs(FMath::Cos(Radians));
	const float SinAngle = FMath::Abs(FMath::Sin(Radians));

	const float RotatedWidth = Width * CosAngle + Height * SinAngle;
	const float RotatedHeight = Width * SinAngle + Height * CosAngle;

	return FVector2D(RotatedWidth, RotatedHeight);
}

FVector2D FFigmaRectangle::GetCenter() const
{
	const FVector2D RotatedTopLeft(X, Y);
	const FVector2D RotatedSize(Width, Height);
	const FVector2D Center = RotatedTopLeft + (RotatedSize * 0.5f);
	return Center;
}