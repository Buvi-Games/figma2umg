// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Builder/Widget/PanelWidgetBuilder.h"

#include "CanvasBuilder.generated.h"

class UCanvasPanel;

UCLASS()
class UCanvasBuilder : public UPanelWidgetBuilder
{
public:
	GENERATED_BODY()

protected:
	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;
	virtual void Setup() const override;

	UPROPERTY()
	TObjectPtr<UCanvasPanel> CanvasPanel = nullptr;
};
