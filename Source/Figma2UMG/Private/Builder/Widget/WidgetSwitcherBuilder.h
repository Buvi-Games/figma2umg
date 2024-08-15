// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MultiChildBuilder.h"
#include "WidgetSwitcherBuilder.generated.h"

class UWidgetSwitcher;

UCLASS()
class FIGMA2UMG_API UWidgetSwitcherBuilder : public UMultiChildBuilder
{
public:
	GENERATED_BODY()
	virtual TObjectPtr<UWidget> FindNodeWidgetInParent(const TObjectPtr<UPanelWidget>& ParentWidget) const override;
	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch) override;

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override;
	virtual void ResetWidget() override;
protected:
	virtual TObjectPtr<UPanelWidget> GetPanelWidget() const override;

	UPROPERTY()
	TObjectPtr<UWidgetSwitcher> Widget = nullptr;
};
