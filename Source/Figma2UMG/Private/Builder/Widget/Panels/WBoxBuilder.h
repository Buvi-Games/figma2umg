// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Builder/Widget/PanelWidgetBuilder.h"

#include "WBoxBuilder.generated.h"

UCLASS()
class UWBoxBuilder : public UPanelWidgetBuilder
{
public:
	GENERATED_BODY()

protected:
	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;
	virtual void Setup() const override;

	UPROPERTY()
	TObjectPtr<UWrapBox> Box = nullptr;
};
