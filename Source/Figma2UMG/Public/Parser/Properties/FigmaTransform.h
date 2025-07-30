// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"

#include "FigmaTransform.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaTransform
{
public:
	GENERATED_BODY()

	FMatrix Matrix = FMatrix::Identity;
};
