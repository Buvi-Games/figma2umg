// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AssetFileHandler.generated.h"

class UWidgetBlueprint;

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UFigmaFileHandle : public UInterface
{
	GENERATED_BODY()
};

class FIGMA2UMG_API IFigmaFileHandle
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual FString GetPackagePath() const = 0;

	UFUNCTION()
	virtual FString GetAssetName() const = 0;

	UFUNCTION()
	virtual void LoadOrCreateAssets() = 0;

	UObject* GetAsset() const { return Asset; }
	template<class Type>
	Type* GetAsset() const { return Cast<Type>(Asset); }
	UObject* GetOuter() const { return AssetOuter; }

protected:
	template<class AssetType>
	AssetType* GetOrCreateAsset();

	UObject* Asset = nullptr;
	UObject* AssetOuter = nullptr;
};

template <class AssetType>
AssetType* IFigmaFileHandle::GetOrCreateAsset()
{
	return nullptr;
}

