// Fill out your copyright notice in the Description page of Project Settings.

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
