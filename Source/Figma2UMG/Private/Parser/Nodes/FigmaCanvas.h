// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/FigmaContainer.h"
#include "Interfaces/FlowTransition.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Properties/FigmaColor.h"
#include "Parser/Properties/FigmaExportSetting.h"
#include "Parser/Properties/FigmaFlowStartingPoint.h"
#include "Parser/Properties/FigmaPrototypeDevice.h"

#include "FigmaCanvas.generated.h"

class UCanvasPanel;

UCLASS()
class FIGMA2UMG_API UFigmaCanvas : public UFigmaNode, public IFigmaContainer, public IFlowTransition
{
public:
	GENERATED_BODY()
	// UFigmaNode
	virtual FVector2D GetAbsolutePosition() const override { return FVector2D::ZeroVector; }
	virtual TScriptInterface<IWidgetBuilder> CreateWidgetBuilders(bool IsRoot = false, bool AllowFrameButton = true) const override;

	// IFigmaContainer
	virtual FString GetJsonArrayName() const override { return FString("Children"); };
	virtual TArray<UFigmaNode*>& GetChildren() override { return Children; }
	virtual const TArray<UFigmaNode*>& GetChildrenConst() const override { return Children; }

	// FlowTransition
	virtual const bool HasAction(const EFigmaActionType ActionType, const EFigmaActionNodeNavigation Navigation) const override;
	virtual const FFigmaInteraction& GetInteractionFromTrigger(const EFigmaTriggerType TriggerType) const override;
	virtual const FFigmaInteraction& GetInteractionFromAction(const EFigmaActionType ActionType, const EFigmaActionNodeNavigation Navigation) const override;
	virtual void GetAllDestinationId(TArray<FString>& TransitionNodeIDs) const override;
	virtual const float GetTransitionDuration() const override;
	virtual const EFigmaEasingType GetTransitionEasing() const override;
protected:

	UPROPERTY()
	TArray<UFigmaNode*> Children;

	UPROPERTY()
	FFigmaColor BackgroundColor;

	UPROPERTY()
	FString PrototypeStartNodeID;

	UPROPERTY()
	TArray<FFigmaFlowStartingPoint> FlowStartingPoints;

	UPROPERTY()
	FFigmaPrototypeDevice PrototypeDevice;

	UPROPERTY()
	TArray<FFigmaExportSetting> ExportSettings;

	UPROPERTY()
	UCanvasPanel* Canvas;
};
