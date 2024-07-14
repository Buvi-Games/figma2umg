// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ImagesRequestResult.generated.h"

USTRUCT()
struct FImagesRequestResult
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
