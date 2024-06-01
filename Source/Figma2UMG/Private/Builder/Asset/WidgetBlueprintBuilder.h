// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetBuilder.h"
#include "WidgetBlueprintBuilder.generated.h"

class UWidgetBlueprint;

UCLASS()
class UWidgetBlueprintBuilder : public UObject, public IAssetBuilder
{
	GENERATED_BODY()
public:
	virtual void LoadOrCreateAssets() override;
	virtual void LoadAssets() override;

protected:
	UPROPERTY()
	TObjectPtr<UWidgetBlueprint> Asset = nullptr;
};
