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

	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;

protected:
	UPROPERTY()
	TObjectPtr<UWidgetSwitcher> Widget = nullptr;
};
