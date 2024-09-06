// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Builder/Widget/PanelWidgetBuilder.h"

#include "VBoxBuilder.generated.h"

UCLASS()
class FIGMA2UMG_API UVBoxBuilder : public UPanelWidgetBuilder
{
public:
	GENERATED_BODY()

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override;
	virtual void ResetWidget() override;
protected:
	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch) override;
	virtual void Setup() const override;

	virtual bool GetSizeValue(FVector2D& Size, bool& SizeToContent) const override;

	UPROPERTY()
	TObjectPtr<UVerticalBox> Box = nullptr;
};
