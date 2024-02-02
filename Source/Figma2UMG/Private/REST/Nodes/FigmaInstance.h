// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "Interfaces/AssetFileHandler.h"

#include "FigmaInstance.generated.h"

UCLASS()
class UFigmaInstance : public UFigmaFrame, public IFigmaFileHandle
{
public:
	GENERATED_BODY()

	// IFigmaFileHandle
	virtual FString GetPackagePath() const override;
	virtual FString GetAssetName() const override;
};
