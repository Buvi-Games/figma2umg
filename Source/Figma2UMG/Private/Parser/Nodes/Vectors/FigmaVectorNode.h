// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/FlowTransition.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Properties/FigmaBlendMode.h"
#include "Parser/Properties/FigmaEffect.h"
#include "Parser/Properties/FigmaEnums.h"
#include "Parser/Properties/FigmaExportSetting.h"
#include "Parser/Properties/FigmaLayoutConstraint.h"
#include "Parser/Properties/FigmaPaint.h"
#include "Parser/Properties/FigmaPaintOverride.h"
#include "Parser/Properties/FigmaPath.h"
#include "Parser/Properties/FigmaRectangle.h"
#include "Parser/Properties/FigmaStrokeWeights.h"
#include "Parser/Properties/FigmaStyleRef.h"
#include "Parser/Properties/FigmaTransform.h"
#include "Parser/Properties/FigmaVector.h"

#include "FigmaVectorNode.generated.h"

class UTexture2DBuilder;

UCLASS()
class FIGMA2UMG_API UFigmaVectorNode : public UFigmaNode, public IFlowTransition
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;
	virtual FVector2D GetAbsolutePosition(const bool IsTopWidgetForNode) const override;
	virtual FVector2D GetAbsoluteSize(const bool IsTopWidgetForNode) const override;
	virtual bool CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders) override;
	virtual FString GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const override;
	virtual TScriptInterface<IWidgetBuilder> CreateWidgetBuilders(bool IsRoot = false, bool AllowFrameButton = true) const override;

	// FlowTransition
	virtual const FFigmaInteraction& GetInteractionFromTrigger(const EFigmaTriggerType TriggerType) const override;
	virtual const FFigmaInteraction& GetInteractionFromAction(const EFigmaActionType ActionType, const EFigmaActionNodeNavigation Navigation) const override;
	virtual const FString& GetDestinationIdFromEvent(const FName& EventName) const override;
	virtual const float GetTransitionDuration() const override { return TransitionDuration; }
	virtual const EFigmaEasingType GetTransitionEasing() const override { return TransitionEasing; };

	virtual bool DoesSupportImageRef() const;

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
	TArray<FFigmaInteraction> Interactions;

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
	TArray<FFigmaPaint> Fills;

	UPROPERTY()
	TArray<FFigmaPath> FillGeometry;

	UPROPERTY()
	TMap<int, FFigmaPaintOverride> FillOverrideTable;

	UPROPERTY()
	TArray<FFigmaPaint> Strokes;

	UPROPERTY()
	float StrokeWeight;

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
	float CornerRadius;

	UPROPERTY()
	TMap<EFigmaStyleType, FString> styles;

	//UPROPERTY()
	//FFigmaAnnotation annotation

protected:
	UPROPERTY()
	TObjectPtr<UTexture2DBuilder> AssetBuilder = nullptr;
};
