// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaGroup.h"
#include "REST/Properties/FigmaLayoutGrid.h"

#include "FigmaFrame.generated.h"

UCLASS()
class UFigmaFrame : public  UFigmaGroup
{
public:
	GENERATED_BODY()

	virtual void PostSerialize(const TSharedRef<FJsonObject> JsonObj) override;

protected:
	UPROPERTY()
	TArray<FFigmaLayoutGrid> LayoutGrids;
};
