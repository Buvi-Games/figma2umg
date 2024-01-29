// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AssetFileHandler.generated.h"

class UWidgetBlueprint;

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class UFigmaFileHandle : public UInterface
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

	UWidgetBlueprint* GetOrCreateAsset();
};