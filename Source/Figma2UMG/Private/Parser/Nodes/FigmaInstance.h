// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "Interfaces/FigmaRefHandle.h"
#include "Parser/Properties/FigmaOverrides.h"

#include "FigmaInstance.generated.h"

struct FFigmaComponentProperty;

UCLASS()
class UFigmaInstance : public UFigmaFrame, public IFigmaRefHandle
{
public:
	GENERATED_BODY()

	// IWidgetOwner
	void ForEach(const IWidgetOwner::FOnEachFunction& Function) override;
	virtual TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch) override;
	virtual TObjectPtr<UWidget> GetTopWidget() const override;
protected:
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
