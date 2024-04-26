// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaGroup.h"
#include "Interfaces/AssetFileHandler.h"
#include "Parser/Properties/FigmaLayoutGrid.h"

#include "FigmaFrame.generated.h"

UCLASS()
class UFigmaFrame : public  UFigmaGroup, public IFigmaFileHandle
{
public:
	GENERATED_BODY()

	void SetGenerateFile();

	// IFigmaFileHandle
	virtual FString GetPackagePath() const override;
	virtual FString GetAssetName() const override;
	virtual void LoadOrCreateAssets(UFigmaFile* FigmaFile) override;
	virtual void LoadAssets() override;

protected:
	UPROPERTY()
	TArray<FFigmaLayoutGrid> LayoutGrids;

	bool GenerateFile = false;
};
