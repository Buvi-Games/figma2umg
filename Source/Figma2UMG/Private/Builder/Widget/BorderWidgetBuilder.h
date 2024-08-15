// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SingleChildBuilder.h"
#include "BorderWidgetBuilder.generated.h"

struct FFigmaPaint;
class UBorder;

UCLASS()
class FIGMA2UMG_API UBorderWidgetBuilder : public USingleChildBuilder
{
public:
	GENERATED_BODY()
	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch) override;

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override;
	virtual void ResetWidget() override;
protected:
	virtual TObjectPtr<UContentWidget> GetContentWidget() const override;
	virtual void GetPaddingValue(FMargin& Padding) const override;
	virtual bool GetAlignmentValues(EHorizontalAlignment& HorizontalAlignment, EVerticalAlignment& VerticalAlignment) const override;

	void Setup() const;

	void SetFill(const TArray<FFigmaPaint>& Fills) const;
	void SetStroke(const TArray<FFigmaPaint>& Strokes, const float& StrokeWeight) const;
	void SetCorner(const FVector4& CornerRadii) const;

	UPROPERTY()
	TObjectPtr<UBorder> Widget = nullptr;
};
