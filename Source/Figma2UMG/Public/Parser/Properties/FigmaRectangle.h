// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"

#include "FigmaRectangle.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaRectangle
{
public:
	GENERATED_BODY()

	FVector2D GetPosition(const float Rotation) const;
	FVector2D GetSize(float Rotation) const;
	FVector2D GetCenter() const;

	UPROPERTY()
	float X;

	UPROPERTY()
	float Y;

	UPROPERTY()
	float Width;

	UPROPERTY()
	float Height;
};
