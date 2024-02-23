// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Builder/BorderCanvasBuilder.h"
#include "Interfaces/FigmaContainer.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Properties/FigmaBlendMode.h"
#include "Parser/Properties/FigmaColor.h"
#include "Parser/Properties/FigmaEasingType.h"
#include "Parser/Properties/FigmaEffect.h"
#include "Parser/Properties/FigmaExportSetting.h"
#include "Parser/Properties/FigmaLayoutConstraint.h"
#include "Parser/Properties/FigmaPaint.h"
#include "Parser/Properties/FigmaRectangle.h"
#include "Parser/Properties/FigmaStyleRef.h"
#include "Parser/Properties/FigmaTransform.h"
#include "Parser/Properties/FigmaVector.h"

#include "FigmaGroup.generated.h"

UENUM()
enum class EFigmaLayoutMode
{
	NONE,
	HORIZONTAL,
	VERTICAL,
};

UENUM()
enum class EFigmaLayoutSizing
{
	FIXED,
	HUG,
	FILL,
};

UENUM()
enum class EFigmaLayoutWrap
{
	NO_WRAP,
	WRAP,
};

UENUM()
enum class EFigmaAxisSizingMode
{
	AUTO,
	FIXED,
};

UENUM()
enum class EFigmaPrimaryAxisAlignItems
{
	MIN,
	CENTER,
	MAX,
	SPACE_BETWEEN
};

UENUM()
enum class EFigmaCounterAxisAlignItems
{
	MIN,
	CENTER,
	MAX,
	BASELINE
};

UENUM()
enum class EFigmaCounterAxisAlignContent
{
	AUTO,
	SPACE_BETWEEN
};

UENUM()
enum class EFigmaLayoutPositioning
{
	AUTO,
	ABSOLUTE,
};

UENUM()
enum class EFigmaOverflowDirection
{
	NONE,
	HORIZONTAL_SCROLLING,
	VERTICAL_SCROLLING,
	HORIZONTAL_AND_VERTICAL_SCROLLING,
};

UCLASS()
class UFigmaGroup : public UFigmaNode, public IWidgetOwner, public IFigmaContainer
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual FVector2D GetAbsolutePosition() const override;

	// IFigmaContainer
	virtual FString GetJsonArrayName() const override { return FString("Children"); };
	virtual TArray<UFigmaNode*>& GetChildren() override { return Children; }

	// IWidgetOwner
	virtual void ForEach(const IWidgetOwner::FOnEachFunction& Function) override;

	virtual TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch) override;
	virtual void PostInsert() const override;
	virtual void Reset() override;

	virtual TObjectPtr<UWidget> GetTopWidget() const override;
	virtual FVector2D GetTopWidgetPosition() const override;

	virtual TObjectPtr<UPanelWidget> GetContainerWidget() const override;
	virtual void PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const override;
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
	EFigmaEasingType TransitionEasing;

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
	EFigmaLayoutMode LayoutMode = EFigmaLayoutMode::NONE;

	UPROPERTY()
	EFigmaLayoutSizing LayoutSizingHorizontal;

	UPROPERTY()
	EFigmaLayoutSizing LayoutSizingVertical;

	UPROPERTY()
	EFigmaLayoutWrap LayoutWrap = EFigmaLayoutWrap::NO_WRAP;

	UPROPERTY()
	EFigmaAxisSizingMode PrimaryAxisSizingMode = EFigmaAxisSizingMode::AUTO;

	UPROPERTY()
	EFigmaAxisSizingMode CounterAxisSizingMode = EFigmaAxisSizingMode::AUTO;

	UPROPERTY()
	EFigmaPrimaryAxisAlignItems PrimaryAxisAlignItems = EFigmaPrimaryAxisAlignItems::MIN;

	UPROPERTY()
	EFigmaCounterAxisAlignItems CounterAxisAlignItems = EFigmaCounterAxisAlignItems::MIN;

	UPROPERTY()
	EFigmaCounterAxisAlignContent CounterAxisAlignContent = EFigmaCounterAxisAlignContent::AUTO;

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
	EFigmaLayoutPositioning LayoutPositioning = EFigmaLayoutPositioning::AUTO;

	UPROPERTY()
	bool ItemReverseZIndex = false;

	UPROPERTY()
	bool StrokesIncludedInLayout = false;

	UPROPERTY()
	EFigmaOverflowDirection OverflowDirection = EFigmaOverflowDirection::NONE;

	UPROPERTY()
	TArray<FFigmaEffect> Effects;

	UPROPERTY()
	bool IsMask = false;

	UPROPERTY()
	FString MaskType;

	UPROPERTY()
	TMap<EFigmaStyleType, FString> Styles;

	UPROPERTY()
	FBorderCanvasBuilder Builder;
};
