// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PanelWidgetBuilder.h"

#include "WBoxBuilder.generated.h"

UCLASS()
class UWBoxBuilder : public UPanelWidgetBuilder
{
public:
	GENERATED_BODY()

protected:
	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;

	UPROPERTY()
	TObjectPtr<UWrapBox> Box = nullptr;
};
