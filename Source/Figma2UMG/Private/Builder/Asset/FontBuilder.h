// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetBuilder.h"
#include "FontBuilder.generated.h"

class UFont;
class UFontFace;

UCLASS()
class UFontBuilder : public UObject, public IAssetBuilder
{
	GENERATED_BODY()
public:
	virtual void LoadOrCreateAssets() override;
	virtual void LoadAssets() override;
	virtual void Reset() override;

	const TObjectPtr<UFont>& GetAsset() const;

	virtual UPackage* GetAssetPackage() const override;
protected:
	UPROPERTY()
	TObjectPtr<UFont> Asset = nullptr;

	UPROPERTY()
	TArray<UFontFace*> Faces;
};
