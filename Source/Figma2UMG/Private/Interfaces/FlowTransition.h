// Copyright 2024 Buvi Games. All Rights Reserved.

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
	virtual const bool HasTransition() const { return !GetTransitionNodeID("OnButtonClicked").IsEmpty(); }

	UFUNCTION()
	virtual const FString& GetTransitionNodeID(const FName EventName) const = 0;

	UFUNCTION()
	virtual void GetAllTransitionNodeID(TArray<FString>& TransitionNodeIDs) const;

	UFUNCTION()
	virtual const float GetTransitionDuration() const = 0;

	UFUNCTION()
	virtual const EFigmaEasingType GetTransitionEasing() const = 0;
protected:
};
