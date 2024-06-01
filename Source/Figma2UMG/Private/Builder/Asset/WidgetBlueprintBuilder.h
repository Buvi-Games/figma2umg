// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetBuilder.h"
#include "WidgetBlueprintBuilder.generated.h"

class UWidgetBlueprint;

UCLASS()
class UWidgetBlueprintBuilder : public UAssetBuilder
{
public:
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UWidgetBlueprint> Widget = nullptr;
};
