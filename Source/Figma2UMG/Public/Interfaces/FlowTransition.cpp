// MIT License
// Copyright (c) 2024 Buvi Games


#include "Interfaces/FlowTransition.h"

#include "Parser/Properties/FigmaAction.h"

void IFlowTransition::GetAllDestinationId(TArray<FString>& TransitionNodeIDs) const
{
	const FFigmaInteraction& FigmaInteraction = GetInteractionFromAction(EFigmaActionType::NODE, EFigmaActionNodeNavigation::NAVIGATE);
	if(FigmaInteraction.IsValid())
	{
		const UFigmaNodeAction* FigmaAction = FigmaInteraction.FindActionNode(EFigmaActionNodeNavigation::NAVIGATE);
		if (FigmaAction && !TransitionNodeIDs.Contains(FigmaAction->DestinationId))
		{
			TransitionNodeIDs.Add(FigmaAction->DestinationId);
		}
	}
}
