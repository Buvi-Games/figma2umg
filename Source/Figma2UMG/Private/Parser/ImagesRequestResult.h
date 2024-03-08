// Fill out your copyright notice in the Description page of Project Settings.

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
	int Status = 0;
	
};
