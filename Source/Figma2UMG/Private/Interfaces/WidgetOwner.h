// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "WidgetOwner.generated.h"

class UFigmaNode;

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UWidgetOwner : public UInterface
{
	GENERATED_BODY()
};

class FIGMA2UMG_API IWidgetOwner
{
	GENERATED_BODY()
public:
	DECLARE_DELEGATE_OneParam(FOnEachFunction, UFigmaNode&)
	virtual void ForEach(const FOnEachFunction& Function) = 0;
protected:
};
