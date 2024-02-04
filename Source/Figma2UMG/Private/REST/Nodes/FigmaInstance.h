// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "Interfaces/FigmaRefHandle.h"
#include "REST/Properties/FigmaOverrides.h"

#include "FigmaInstance.generated.h"

struct FFigmaComponentProperty;

UCLASS()
class UFigmaInstance : public UFigmaFrame, public IFigmaRefHandle
{
public:
	GENERATED_BODY()

protected:
	virtual TObjectPtr<UWidget> PatchWidgetImp(TObjectPtr<UWidget> WidgetToPatch) override;

	UPROPERTY()
	FString ComponentId;

	UPROPERTY()
	bool IsExposedInstance = false;

	UPROPERTY()
	TArray<FString> ExposedInstances;

	UPROPERTY()
	TMap<FString, FFigmaComponentProperty> ComponentProperties;

	UPROPERTY()
	TArray<FFigmaOverrides> Overrides;
};
