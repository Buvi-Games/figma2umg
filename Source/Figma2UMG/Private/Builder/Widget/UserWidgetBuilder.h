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

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) override;

	virtual TObjectPtr<UWidget> GetWidget() const override;
protected:
	virtual void GetPaddingValue(FMargin& Padding) const override;
	virtual bool GetAlignmentValues(EHorizontalAlignment& HorizontalAlignment, EVerticalAlignment& VerticalAlignment) const;

	UPROPERTY()
	TObjectPtr<UWidgetBlueprintBuilder> WidgetBlueprintBuilder = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> Widget = nullptr;
};
