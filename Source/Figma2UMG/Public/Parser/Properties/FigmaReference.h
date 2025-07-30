// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"

#include "FigmaReference.generated.h"

USTRUCT()
struct FIGMA2UMG_API FFigmaReference
{
public:
	GENERATED_BODY()

	UPROPERTY()
	FString Key;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString Description;

	UPROPERTY()
	bool Remote;

	FString RemoteFileKey;
};