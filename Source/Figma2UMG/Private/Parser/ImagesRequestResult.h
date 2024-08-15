// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ImagesRequestResult.generated.h"

USTRUCT()
struct FIGMA2UMG_API FImagesRequestResult
{
	GENERATED_BODY()
public:

	UPROPERTY()
	FString Err;

	UPROPERTY()
	TMap<FString, FString> Images;

	UPROPERTY()
	int Status;
	
};
