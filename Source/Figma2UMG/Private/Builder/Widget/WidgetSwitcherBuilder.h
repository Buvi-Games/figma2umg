// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "WidgetSwitcherBuilder.generated.h"

class UWidgetSwitcher;

UCLASS()
class UWidgetSwitcherBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UWidgetSwitcher> Widget = nullptr;
};
