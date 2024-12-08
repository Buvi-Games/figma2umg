// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Parser/Properties/FigmaEnums.h"
#include "Parser/Properties/FigmaInteraction.h"

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
	virtual const bool HasTrigger(const EFigmaTriggerType TriggerType) const { return GetInteractionFromTrigger(TriggerType).IsValid(); }

	UFUNCTION()
	virtual const bool HasAction(const EFigmaActionType ActionType, const EFigmaActionNodeNavigation Navigation) const { return GetInteractionFromAction(ActionType, Navigation).IsValid(); }

	UFUNCTION()
	virtual const FFigmaInteraction& GetInteractionFromTrigger(const EFigmaTriggerType TriggerType) const = 0;

	UFUNCTION()
	virtual const FFigmaInteraction& GetInteractionFromAction(const EFigmaActionType ActionType, const EFigmaActionNodeNavigation Navigation) const = 0;

	UFUNCTION()
	virtual const FString& GetDestinationIdFromEvent(const FName& EventName) const = 0;

	UFUNCTION()
	virtual void GetAllDestinationId(TArray<FString>& TransitionNodeIDs) const;

	UFUNCTION()
	virtual const float GetTransitionDuration() const = 0;

	UFUNCTION()
	virtual const EFigmaEasingType GetTransitionEasing() const = 0;
protected:
};
