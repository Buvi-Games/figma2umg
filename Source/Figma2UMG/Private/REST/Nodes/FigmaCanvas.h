// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/FigmaContainer.h"
#include "REST/Nodes/FigmaNode.h"
#include "REST/Properties/FigmaColor.h"
#include "REST/Properties/FigmaExportSetting.h"
#include "REST/Properties/FigmaFlowStartingPoint.h"
#include "REST/Properties/FigmaPrototypeDevice.h"

#include "FigmaCanvas.generated.h"

UCLASS()
class UFigmaCanvas : public UFigmaNode, public IFigmaContainer
{
public:
	GENERATED_BODY()
	// UFigmaNode
	virtual FVector2D GetAbsolutePosition() const override { return FVector2D(); }
	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch) override;

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
};
