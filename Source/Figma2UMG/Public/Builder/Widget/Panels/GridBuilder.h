// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"
#include "Builder/Widget/PanelWidgetBuilder.h"
#include "Components/GridPanel.h"

#include "GridBuilder.generated.h"

UCLASS()
class FIGMA2UMG_API UGridBuilder : public UPanelWidgetBuilder
{
public:
	GENERATED_BODY()

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override;
	virtual void ResetWidget() override;

protected:
	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch, TMap<FString, int32>& NameTracker) override;
	virtual void Setup() const override;
	virtual bool GetSizeValue(FVector2D& Size, bool& SizeToContent) const override;

	void ConfigureGridFillRules() const;
	void ConfigureChildGridPositions() const;
	void CalculateChildGridPosition(int32 ChildIndex, int32& OutRow, int32& OutColumn) const;

	UPROPERTY()
	TObjectPtr<UGridPanel> GridPanel = nullptr;
};
