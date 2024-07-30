// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
class UFigmaText : public UFigmaNode
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;
	virtual FVector2D GetAbsolutePosition() const override;
	virtual FVector2D GetAbsoluteSize() const override;
	virtual bool CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders) override;
	virtual FString GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const override;
	virtual TScriptInterface<IWidgetBuilder> CreateWidgetBuilders(bool IsRoot = false, bool AllowFrameButton = true) const override;

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
	float Opacity = 1.0f;

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
	EFigmaLayoutSizing LayoutSizingHorizontal;

	UPROPERTY()
	EFigmaLayoutSizing LayoutSizingVertical;

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

protected:
	virtual void ProcessComponentPropertyReference(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, const TPair<FString, FString>& PropertyReference) const override;
};
