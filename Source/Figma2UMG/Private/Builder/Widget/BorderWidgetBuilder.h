// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "BorderWidgetBuilder.generated.h"

struct FFigmaPaint;
class UBorder;

UCLASS()
class UBorderWidgetBuilder : public USingleChildBuilder
{
public:
	GENERATED_BODY()
	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;

protected:
	void Setup() const;

	void SetFill(const TArray<FFigmaPaint>& Fills) const;
	void SetStroke(const TArray<FFigmaPaint>& Strokes, const float& StrokeWeight) const;
	void SetCorner(const FVector4& CornerRadii) const;

	UPROPERTY()
	TObjectPtr<UBorder> Widget = nullptr;
};
