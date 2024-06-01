// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "BorderWidgetBuilder.generated.h"

class UBorder;

UCLASS()
class UBorderWidgetBuilder : public UWidgetBuilder
{
public:
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UBorder> Widget = nullptr;
};
