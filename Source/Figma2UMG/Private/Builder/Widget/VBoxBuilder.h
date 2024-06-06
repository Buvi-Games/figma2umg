// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PanelWidgetBuilder.h"

#include "VBoxBuilder.generated.h"

UCLASS()
class UVBoxBuilder : public UPanelWidgetBuilder
{
public:
	GENERATED_BODY()

protected:
	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;

	UPROPERTY()
	TObjectPtr<UVerticalBox> Box = nullptr;
};
