// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PanelWidgetBuilder.h"

#include "HBoxBuilder.generated.h"

UCLASS()
class UHBoxBuilder : public UPanelWidgetBuilder
{
public:
	GENERATED_BODY()

protected:
	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;

	UPROPERTY()
	TObjectPtr<UHorizontalBox> Box = nullptr;
};
