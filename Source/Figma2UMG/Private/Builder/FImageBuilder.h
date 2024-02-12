// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FImageBuilder.generated.h"

class UImage;


USTRUCT()
struct FIGMA2UMG_API FImageBuilder
{
public:
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UImage> Image = nullptr;
};
