// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Interfaces/FlowTransition.h"

void IFlowTransition::GetAllTransitionNodeID(TArray<FString>& TransitionNodeIDs) const
{
	FString TransitionId = GetTransitionNodeID("OnButtonClicked");
	if(!TransitionId.IsEmpty())
	{
		TransitionNodeIDs.Add(TransitionId);
	}
}
