// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "WidgetSwitcherBuilder.generated.h"

class UWidgetSwitcher;

UCLASS()
class UWidgetSwitcherBuilder : public UMultiChildBuilder
{
public:
	GENERATED_BODY()

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;

protected:
	virtual TObjectPtr<UPanelWidget> GetPanelWidget() override;

	UPROPERTY()
	TObjectPtr<UWidgetSwitcher> Widget = nullptr;
};
