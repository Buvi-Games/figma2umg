// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BorderBuilder.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Properties/FigmaEnums.h"

#include "SizeBoxBuilder.generated.h"

class USizeBox;
struct FFigmaPaint;

USTRUCT()
struct FIGMA2UMG_API FSizeBoxBuilder
{
public:
	GENERATED_BODY()
	virtual ~FSizeBoxBuilder() = default;

	virtual void ForEach(const IWidgetOwner::FOnEachFunction& Function);

	virtual TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName);
	virtual void SetupWidget(TObjectPtr<UWidget> Widget);
	virtual void Reset();

	void SetupBorder(const TArray<FFigmaPaint>& Fills, const TArray<FFigmaPaint>& Strokes, float InStrokeWeight, const EFigmaStrokeAlign& InStrokeAlign, const FVector4& InCornerRadii, const float InCornerSmoothing);
	void SetupBrush(FSlateBrush& Brush, const TArray<FFigmaPaint>& Fills, const TArray<FFigmaPaint>& Strokes, float InStrokeWeight, EFigmaStrokeAlign InStrokeAlign, const FVector4& InCornerRadii, float InCornerSmoothing) const;
	void SetLayout(EFigmaLayoutMode InLayoutMode, EFigmaLayoutWrap InLayoutWrap);
	void SetLayoutSize(EFigmaLayoutSizing InLayoutSizingHorizontal, float InWidth, EFigmaLayoutSizing InLayoutSizingVertical, float InHeight);

	virtual TObjectPtr<UWidget> GetTopWidget() const;
	virtual TObjectPtr<UPanelWidget> GetContainerWidget() const;

private:
	UPROPERTY()
	TObjectPtr<USizeBox> SizeBox = nullptr;

	EFigmaLayoutSizing LayoutSizingHorizontal = EFigmaLayoutSizing::HUG;
	EFigmaLayoutSizing LayoutSizingVertical = EFigmaLayoutSizing::HUG;
	float FixedWidth = 0.0f;
	float FixedHeight = 0.0f;

	FBorderBuilder BorderBuilder;
};
