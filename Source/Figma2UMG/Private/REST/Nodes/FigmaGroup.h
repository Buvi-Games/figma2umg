// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "REST/Nodes/FigmaNode.h"
#include "REST/Properties/FigmaBlendMode.h"
#include "REST/Properties/FigmaColor.h"
#include "REST/Properties/FigmaEasingType.h"
#include "REST/Properties/FigmaEffect.h"
#include "REST/Properties/FigmaExportSetting.h"
#include "REST/Properties/FigmaLayoutConstraint.h"
#include "REST/Properties/FigmaPaint.h"
#include "REST/Properties/FigmaRectangle.h"
#include "REST/Properties/FigmaStyleRef.h"
#include "REST/Properties/FigmaTransform.h"
#include "REST/Properties/FigmaVector.h"

#include "FigmaGroup.generated.h"

UCLASS()
class UFigmaGroup : public UFigmaNode
{
public:
	GENERATED_BODY()

	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;

protected:
	UPROPERTY()
	TArray<UFigmaNode*> Children;

	UPROPERTY()
	bool Locked = false;

	UPROPERTY()
	FFigmaColor BackgroundColor;

	UPROPERTY()
	TArray<FFigmaPaint> Fills;

	UPROPERTY()
	TArray<FFigmaPaint> Strokes;

	UPROPERTY()
	float StrokeWeight;

	UPROPERTY()
	FString StrokeAlign;

	UPROPERTY()
	TArray<float> StrokeDashes;

	UPROPERTY()
	float CornerRadius;

	UPROPERTY()
	TArray<float> RectangleCornerRadii;

	UPROPERTY()
	float CornerSmoothing;

	UPROPERTY()
	TArray<FFigmaExportSetting> ExportSettings;

	UPROPERTY()
	EFigmaBlendMode BlendMode;

	UPROPERTY()
	bool PreserveRatio = false;

	UPROPERTY()
	FFigmaLayoutConstraint Constraints;

	UPROPERTY()
	FString LayoutAlign;

	UPROPERTY()
	FString TransitionNodeID;

	UPROPERTY()
	float TransitionDuration = -1.0f;

	UPROPERTY()
	FFigmaEasingType TransitionEasing;

	UPROPERTY()
	float Opacity = 1.0f;

	UPROPERTY()
	FFigmaRectangle AbsoluteBoundingBox;

	UPROPERTY()
	FFigmaRectangle AbsoluteRenderBounds;

	UPROPERTY()
	FFigmaVector Size;

	UPROPERTY()
	float MinWidth = -1.0f;

	UPROPERTY()
	float MaxWidth = -1.0f;

	UPROPERTY()
	float MinHeight = -1.0f;

	UPROPERTY()
	float MaxHeight = -1.0f;

	UPROPERTY()
	FFigmaTransform RelativeTransform;

	UPROPERTY()
	FString LayoutMode = FString("NONE");

	UPROPERTY()
	FString LayoutSizingHorizontal;

	UPROPERTY()
	FString LayoutSizingVertical;

	UPROPERTY()
	FString LayoutWrap = FString("NO_WRAP");

	UPROPERTY()
	FString PrimaryAxisSizingMode = FString("AUTO");

	UPROPERTY()
	FString CounterAxisSizingMode = FString("AUTO");

	UPROPERTY()
	FString PrimaryAxisAlignItems = FString("MIN");

	UPROPERTY()
	FString CounterAxisAlignItems = FString("MIN");

	UPROPERTY()
	FString CounterAxisAlignContent = FString("AUTO");

	UPROPERTY()
	float PaddingLeft = 0.0f;

	UPROPERTY()
	float PaddingRight = 0.0f;

	UPROPERTY()
	float PaddingTop = 0.0f;

	UPROPERTY()
	float PaddingBottom = 0.0f;

	UPROPERTY()
	float HorizontalPadding = 0.0f;

	UPROPERTY()
	float VerticalPadding = 0.0f;

	UPROPERTY()
	float ItemSpacing = 0.0f;

	UPROPERTY()
	float CounterAxisSpacing = 0.0f;

	UPROPERTY()
	FString LayoutPositioning = FString("AUTO");

	UPROPERTY()
	bool ItemReverseZIndex = false;

	UPROPERTY()
	bool StrokesIncludedInLayout = false;

	UPROPERTY()
	FString OverflowDirection = FString("NONE");

	UPROPERTY()
	TArray<FFigmaEffect> Effects;

	UPROPERTY()
	bool IsMask = false;

	UPROPERTY()
	FString MaskType;

	UPROPERTY()
	TMap<EStyleType, FString> Styles;
};
