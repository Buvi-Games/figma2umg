// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "UserWidgetBuilder.generated.h"

class UWidgetBlueprintBuilder;
class UTexture2DBuilder;
class UWidget;
class UImage;

UCLASS()
class UUserWidgetBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()
	void SetWidgetBlueprintBuilder(const TObjectPtr<UWidgetBlueprintBuilder>& InWidgetBlueprintBuilder);

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch) override;
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) override;
	virtual void PatchWidgetProperties() override;

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override;
	virtual TObjectPtr<UWidget> GetWidget() const override;
	virtual void ResetWidget() override;
protected:
	virtual void GetPaddingValue(FMargin& Padding) const override;
	virtual bool GetAlignmentValues(EHorizontalAlignment& HorizontalAlignment, EVerticalAlignment& VerticalAlignment) const;

	UPROPERTY()
	TObjectPtr<UWidgetBlueprintBuilder> WidgetBlueprintBuilder = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> Widget = nullptr;
};
