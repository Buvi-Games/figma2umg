// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "REST/Nodes/FigmaNode.h"
#include "REST/Properties/FigmaColor.h"
#include "REST/Properties/FigmaExportSetting.h"
#include "REST/Properties/FigmaFlowStartingPoint.h"
#include "REST/Properties/FigmaPrototypeDevice.h"

#include "FigmaCanvas.generated.h"

UCLASS()
class UFigmaCanvas : public UFigmaNode
{
public:
	GENERATED_BODY()

	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;

	virtual void PostInsert(UWidget* Widget) const override;

	virtual FVector2D GetAbsolutePosition() const override { return FVector2D(); }
protected:
	virtual TObjectPtr<UWidget> AddOrPathToWidgetImp(TObjectPtr<UWidget> WidgetToPatch) override;

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
