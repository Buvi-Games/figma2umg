// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ImageBuilder.generated.h"

class UWidget;
class UImage;


USTRUCT()
struct FIGMA2UMG_API FImageBuilder
{
public:
	GENERATED_BODY()

	void SetupWidget(TObjectPtr<UWidget> Widget);
	void Reset();

	UPROPERTY()
	TObjectPtr<UImage> Image = nullptr;
};
