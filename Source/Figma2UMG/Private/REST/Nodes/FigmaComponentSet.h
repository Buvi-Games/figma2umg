// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "REST/Properties/FigmaComponentPropertyDefinition.h"

#include "FigmaComponentSet.generated.h"

UCLASS()
class UFigmaComponentSet : public  UFigmaFrame
{
public:
	GENERATED_BODY()

	virtual void PostSerialize(const TSharedRef<FJsonObject> JsonObj) override;

protected:
	UPROPERTY()
	TMap<FString, FFigmaComponentPropertyDefinition> ComponentPropertyDefinitions;
};
