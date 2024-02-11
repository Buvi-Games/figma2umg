// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Builder/TextBoxBuilder.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Nodes/Vectors/FigmaVectorNode.h"
#include "Parser/Properties/FigmaTypeStyle.h"

#include "FigmaText.generated.h"

class UTextBlock;

UENUM()
enum class EFigmaLineType
{
	ORDERED,// : Text is an ordered list (numbered)
	UNORDERED,// : Text is an unordered list (bulleted)
	NONE,// : Text is plain text and not part of any list
};

UCLASS()
class UFigmaText : public UFigmaNode, public IWidgetOwner
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual FVector2D GetAbsolutePosition() const override;
	FVector2D GetSize() const;

	// IWidgetOwner
	virtual void ForEach(const FOnEachFunction& Function) override;
	virtual TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch) override;
	virtual TObjectPtr<UWidget> GetTopWidget() const override;
	virtual FVector2D GetTopWidgetPosition() const override;
	virtual TObjectPtr<UPanelWidget> GetContainerWidget() const override;

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

	UPROPERTY()
	FString Characters;

	UPROPERTY()
	FFigmaTypeStyle Style;

	UPROPERTY()
	TArray<int> CharacterStyleOverrides;

	UPROPERTY()
	TMap<int, FFigmaTypeStyle> StyleOverrideTable;

	UPROPERTY()
	TArray<EFigmaLineType> LineTypes;

	UPROPERTY()
	TArray<int> LineIndentations;

	UPROPERTY()
	FTextBoxBuilder Builder;
};
