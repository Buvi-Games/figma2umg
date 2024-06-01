// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "ImageWidgetBuilder.generated.h"

class UImage;

UCLASS()
class UImageWidgetBuilder : public UWidgetBuilder
{
public:
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UImage> Widget = nullptr;
};
