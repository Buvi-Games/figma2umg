// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/FigmaContainer.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Properties/FigmaColor.h"
#include "Parser/Properties/FigmaExportSetting.h"
#include "Parser/Properties/FigmaFlowStartingPoint.h"
#include "Parser/Properties/FigmaPrototypeDevice.h"

#include "FigmaCanvas.generated.h"

class UCanvasPanel;

UCLASS()
class UFigmaCanvas : public UFigmaNode, public IWidgetOwner, public IFigmaContainer
{
public:
	GENERATED_BODY()
	// UFigmaNode
	virtual FVector2D GetAbsolutePosition() const override { return FVector2D::ZeroVector; }

	// IWidgetOwner
	virtual void ForEach(const IWidgetOwner::FOnEachFunction& Function) override;
	virtual TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch) override;
	virtual void Reset() override;
	virtual TObjectPtr<UWidget> GetTopWidget() const override;
	virtual FVector2D GetTopWidgetPosition() const override;

	virtual TObjectPtr<UPanelWidget> GetContainerWidget() const override;

	virtual void PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const override;

	// IFigmaContainer
	virtual FString GetJsonArrayName() const override { return FString("Children"); };
	virtual TArray<UFigmaNode*>& GetChildren() override { return Children; }
protected:

	UPROPERTY()
	TArray<UFigmaNode*> Children;

	UPROPERTY()
	FFigmaColor BackgroundColor;

	UPROPERTY()
	TArray<FFigmaFlowStartingPoint> FlowStartingPoints;

	UPROPERTY()
	FFigmaPrototypeDevice PrototypeDevice;

	UPROPERTY()
	TArray<FFigmaExportSetting> ExportSettings;

	UPROPERTY()
	UCanvasPanel* Canvas;
};
