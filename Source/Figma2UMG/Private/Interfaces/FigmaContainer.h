// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FigmaContainer.generated.h"

class UFigmaNode;

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UFigmaContainer : public UInterface
{
	GENERATED_BODY()
};

class FIGMA2UMG_API IFigmaContainer
{
	GENERATED_BODY()
public:

	DECLARE_DELEGATE_TwoParams(FOnEachFunction, UFigmaNode&, const int)
	void ForEach(const FOnEachFunction& Function);

	UFUNCTION()
	virtual FString GetJsonArrayName() const = 0;

	UFUNCTION()
	virtual TArray<UFigmaNode*>& GetChildren() = 0;
};
