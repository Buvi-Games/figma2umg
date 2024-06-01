// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetBuilder.h"
#include "Texture2DBuilder.generated.h"

class UBorder;

UCLASS()
class UTexture2DBuilder : public UObject, public IAssetBuilder
{
	GENERATED_BODY()
public:
	virtual void LoadOrCreateAssets() override;
	virtual void LoadAssets() override;

protected:
	UPROPERTY()
	TObjectPtr<UTexture2D> Asset = nullptr;
};
