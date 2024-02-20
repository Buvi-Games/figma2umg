// Fill out your copyright notice in the Description page of Project Settings.

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