// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Builder/ImageBuilder.h"
#include "Interfaces/AssetFileHandler.h"
#include "Interfaces/FigmaImageRequester.h"
#include "Interfaces/FlowTransition.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Properties/FigmaBlendMode.h"
#include "Parser/Properties/FigmaEasingType.h"
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

UCLASS()
class FIGMA2UMG_API UFigmaVectorNode : public UFigmaNode, public IFigmaImageRequester, public IFigmaFileHandle, public IWidgetOwner, public IFlowTransition
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;
	virtual FVector2D GetAbsolutePosition() const override;
	FVector2D GetSize() const;
	virtual TScriptInterface<IAssetBuilder> CreateAssetBuilder(const FString& InFileKey) override;
	virtual FString GetPackageName() const override;

	// IFigmaImageRequester
	virtual void AddImageRequest(FString FileKey, FImageRequests& ImageRequests) override;
	virtual void OnRawImageReceived(const TArray<uint8>& RawData) override;

	// IFigmaFileHandle
	virtual FString GetPackagePath() const override;
	virtual FString GetAssetName() const override;
	virtual void LoadOrCreateAssets() override;
	virtual void LoadAssets() override;

	// IWidgetOwner
	virtual void ForEach(const IWidgetOwner::FOnEachFunction& Function) override;

	virtual TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch) override;
	virtual void SetupWidget(TObjectPtr<UWidget> Widget) override;
	virtual void PostInsert() const override;
	virtual void Reset() override;

	virtual TObjectPtr<UWidget> GetTopWidget() const override;
	virtual FVector2D GetTopWidgetPosition() const override;

	virtual TObjectPtr<UPanelWidget> GetContainerWidget() const override;
	virtual void PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const override;

	// FlowTransition
	virtual const FString& GetTransitionNodeID() const override { return TransitionNodeID; }
	virtual const float GetTransitionDuration() const override { return TransitionDuration; };
	virtual const EFigmaEasingType GetTransitionEasing() const override { return TransitionEasing; };
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

	UPROPERTY()
	FImageBuilder Builder;
};
