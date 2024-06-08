// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Builder/Widget/PanelWidgetBuilder.h"

#include "HBoxBuilder.generated.h"

UCLASS()
class UHBoxBuilder : public UPanelWidgetBuilder
{
public:
	GENERATED_BODY()

protected:
	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;
	virtual void Setup() const override;

	UPROPERTY()
	TObjectPtr<UHorizontalBox> Box = nullptr;
};
