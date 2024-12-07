// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FigmaEnums.h"

#include "FigmaTransition.generated.h"

UCLASS()
class FIGMA2UMG_API UFigmaTransition : public UObject
{
public:
	GENERATED_BODY()

	UPROPERTY()
	EFigmaTransitionType Type;
};

UCLASS()
class FIGMA2UMG_API UFigmaSimpleTransition : public UFigmaTransition
{
public:
	GENERATED_BODY()

	UPROPERTY()
	float Duration;

	UPROPERTY()
	EFigmaEasingType Easing;
};

UCLASS()
class FIGMA2UMG_API UFigmaDirectionalTransition : public UFigmaTransition
{
public:
	GENERATED_BODY()

	UPROPERTY()
	float Delay;

	UPROPERTY()
	bool DeprecatedVersion;
};