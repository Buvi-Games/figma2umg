// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaCanvas.h"

#include "Builder/Widget/Panels/CanvasBuilder.h"
#include "Builder/Widget/PanelWidgetBuilder.h"

const bool UFigmaCanvas::HasAction(const EFigmaActionType ActionType, const EFigmaActionNodeNavigation Navigation) const
{
	if (Navigation == EFigmaActionNodeNavigation::NAVIGATE)
	{
		return !PrototypeStartNodeID.IsEmpty();
	}
	return false;
}

const FFigmaInteraction& UFigmaCanvas::GetInteractionFromTrigger(const EFigmaTriggerType TriggerType) const
{
	return FFigmaInteraction::Invalid;
}

const FFigmaInteraction& UFigmaCanvas::GetInteractionFromAction(const EFigmaActionType ActionType,const EFigmaActionNodeNavigation Navigation) const
{
	return FFigmaInteraction::Invalid;
}

void UFigmaCanvas::GetAllDestinationId(TArray<FString>& TransitionNodeIDs) const
{
	if (!PrototypeStartNodeID.IsEmpty() && TransitionNodeIDs.Contains(PrototypeStartNodeID))
	{
		TransitionNodeIDs.Add(PrototypeStartNodeID);
	}
}

const float UFigmaCanvas::GetTransitionDuration() const
{
	return 0.0f;
}

const EFigmaEasingType UFigmaCanvas::GetTransitionEasing() const
{
	return EFigmaEasingType::LINEAR;
}

TScriptInterface<IWidgetBuilder> UFigmaCanvas::CreateWidgetBuilders(bool IsRoot /*= false*/, bool AllowFrameButton/*= true*/) const
{
	UCanvasBuilder* Builder = NewObject<UCanvasBuilder>();
	Builder->SetNode(this);

	for (const UFigmaNode* Child : Children)
	{
		if (TScriptInterface<IWidgetBuilder> SubBuilder = Child->CreateWidgetBuilders())
		{
			Builder->AddChild(SubBuilder);
		}
	}

	return Builder;
}