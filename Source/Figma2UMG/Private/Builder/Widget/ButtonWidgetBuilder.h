// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "ButtonWidgetBuilder.generated.h"

class UButton;

UCLASS()
class UButtonWidgetBuilder : public UWidgetBuilder
{
public:
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UButton> Widget = nullptr;
};
