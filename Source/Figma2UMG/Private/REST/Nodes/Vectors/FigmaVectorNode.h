// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "REST/Nodes/FigmaNode.h"
#include "REST/Properties/FigmaBlendMode.h"
#include "REST/Properties/FigmaEasingType.h"
#include "REST/Properties/FigmaEffect.h"
#include "REST/Properties/FigmaExportSetting.h"
#include "REST/Properties/FigmaLayoutConstraint.h"
#include "REST/Properties/FigmaPaint.h"
#include "REST/Properties/FigmaPaintOverride.h"
#include "REST/Properties/FigmaPath.h"
#include "REST/Properties/FigmaRectangle.h"
#include "REST/Properties/FigmaStrokeWeights.h"
#include "REST/Properties/FigmaStyleRef.h"
#include "REST/Properties/FigmaTransform.h"
#include "REST/Properties/FigmaVector.h"

#include "FigmaVectorNode.generated.h"

UENUM()
enum class EFigmaLayoutAlign
{
	INHERIT,
	STRETCH,
	MIN,
	CENTER,
	MAX,
};

UENUM()
enum class EFigmaStrokeCap
{
	NONE,
	ROUND,
	SQUARE,
	LINE_ARROW,
	TRIANGLE_ARROW,
	DIAMOND_FILLED,
	CIRCLE_FILLED,
	TRIANGLE_FILLED,
	WASHI_TAPE_1,
	WASHI_TAPE_2,
	WASHI_TAPE_3,
	WASHI_TAPE_4,
	WASHI_TAPE_5,
	WASHI_TAPE_6,
};

UENUM()
enum class EFigmaStrokeJoin
{
	MITER,
	BEVEL,
	ROUND,
};

UENUM()
enum class EFigmaStrokeAlign
{
	INSIDE, // stroke drawn inside the shape boundary,
	OUTSIDE, // stroke drawn outside the shape boundary
	CENTER, // stroke drawn centered along the shape boundary
};



UCLASS()
class UFigmaVectorNode : public UFigmaNode
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual FVector2D GetAbsolutePosition() const override;
	FVector2D GetSize() const;

protected:
	UPROPERTY()
	bool Locked = false;

	UPROPERTY()
	TArray<FFigmaExportSetting> ExportSettings;

	UPROPERTY()
	EFigmaBlendMode BlendMode;

	UPROPERTY()
	bool PreserveRatio = false;

	UPROPERTY()
	bool LayoutAlign = false;

	UPROPERTY()
	float LayoutGrow = 0.0f;

	UPROPERTY()
	FFigmaLayoutConstraint Constraints;

	UPROPERTY()
	FString TransitionNodeID;

	UPROPERTY()
	float TransitionDuration;

	UPROPERTY()
	EFigmaEasingType TransitionEasing;

	UPROPERTY()
	float Opacity;

	UPROPERTY()
	FFigmaRectangle AbsoluteBoundingBox;

	UPROPERTY()
	FFigmaRectangle AbsoluteRenderBounds;

	UPROPERTY()
	TArray<FFigmaEffect> Effects;

	UPROPERTY()
	FFigmaVector Size;

	UPROPERTY()
	FFigmaTransform relativeTransform;

	UPROPERTY()
	bool IsMask = false;

	UPROPERTY()
	TArray<FFigmaPaint> Fills;

	UPROPERTY()
	TArray<FFigmaPath> FillGeometry;

	UPROPERTY()
	TMap<int, FFigmaPaintOverride> FillOverrideTable;

	UPROPERTY()
	TArray<FFigmaPaint> Strokes;

	UPROPERTY()
	float StrokesWeight;

	UPROPERTY()
	FFigmaStrokeWeights IndividualStrokeWeights;

	UPROPERTY()
	EFigmaStrokeCap StrokeCap = EFigmaStrokeCap::NONE;

	UPROPERTY()
	EFigmaStrokeJoin StrokeJoin = EFigmaStrokeJoin::MITER;;

	UPROPERTY()
	TArray<float> StrokeDashes;

	UPROPERTY()
	float StrokeMiterAngle = 28.96f;

	UPROPERTY()
	TArray<FFigmaPath> StrokeGeometry;

	UPROPERTY()
	EFigmaStrokeAlign StrokeAlign = EFigmaStrokeAlign::INSIDE;

	UPROPERTY()
	TMap<EFigmaStyleType, FString> styles;

	//UPROPERTY()
	//FFigmaAnnotation annotation
};
