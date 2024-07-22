// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GFontFamilyInfo.generated.h"

USTRUCT()
struct FGFontFamilyInfo
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString Family;

	UPROPERTY()
	FString URL;

	UPROPERTY()
	TArray<FString> Variants;

	void Download();
};
