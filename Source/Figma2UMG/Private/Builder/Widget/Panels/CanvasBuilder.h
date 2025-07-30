// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"
#include "Builder/Widget/PanelWidgetBuilder.h"

#include "CanvasBuilder.generated.h"

class UCanvasPanel;

UCLASS()
class FIGMA2UMG_API UCanvasBuilder : public UPanelWidgetBuilder
{
public:
	GENERATED_BODY()

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override;
	virtual void ResetWidget() override;
protected:
	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch) override;
	virtual void Setup() const override;

	UPROPERTY()
	TObjectPtr<UCanvasPanel> CanvasPanel = nullptr;
};
