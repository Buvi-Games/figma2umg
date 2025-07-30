// MIT License
// Copyright (c) 2024 Buvi Games

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

USTRUCT()
struct FIGMA2UMG_API FImagesRefMeta
{
	GENERATED_BODY()
public:

	UPROPERTY()
	TMap<FString, FString> Images;
};

USTRUCT()
struct FIGMA2UMG_API FImagesRefRequestResult
{
	GENERATED_BODY()
public:

	UPROPERTY()
	FString Err;

	UPROPERTY()
	FImagesRefMeta Meta;

	UPROPERTY()
	int Status;

};