// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Parser/Properties/FigmaEasingType.h"

#include "FlowTransition.generated.h"

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UFlowTransition : public UInterface
{
	GENERATED_BODY()
};

class FIGMA2UMG_API IFlowTransition
{
	GENERATED_BODY()
public:

	UFUNCTION()
	virtual const bool HasTransition() const { return !GetTransitionNodeID().IsEmpty(); }

	UFUNCTION()
	virtual const FString& GetTransitionNodeID() const = 0;

	UFUNCTION()
	virtual const float GetTransitionDuration() const = 0;

	UFUNCTION()
	virtual const EFigmaEasingType GetTransitionEasing() const = 0;
protected:
};
