// Copyright 2024 Buvi Games. All Rights Reserved.

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
