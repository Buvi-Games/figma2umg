// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "Builder/FImageBuilder.h"
#include "Interfaces/AssetFileHandler.h"
#include "Interfaces/FigmaImageRequester.h"
#include "Interfaces/FigmaRefHandle.h"
#include "Parser/Properties/FigmaComponentProperty.h"
#include "Parser/Properties/FigmaOverrides.h"

#include "FigmaInstance.generated.h"

UCLASS()
class UFigmaInstance : public UFigmaNode, public IWidgetOwner, public IFigmaRefHandle, public IFigmaImageRequester, public IFigmaFileHandle
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual FVector2D GetAbsolutePosition() const override;

	// IWidgetOwner
	virtual void ForEach(const IWidgetOwner::FOnEachFunction& Function) override;

	virtual TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch) override;
	virtual void PostInsert() const override;
	virtual void Reset() override;

	virtual TObjectPtr<UWidget> GetTopWidget() const override;
	virtual FVector2D GetTopWidgetPosition() const override;

	virtual TObjectPtr<UPanelWidget> GetContainerWidget() const override;

	// IFigmaImageRequester
	virtual void AddImageRequest(FString FileKey, FImageRequests& ImageRequests) override;
	virtual void OnRawImageReceived(TArray<uint8>& RawData) override;

	// IFigmaFileHandle
	virtual FString GetPackagePath() const override;
	virtual FString GetAssetName() const override;
	virtual void LoadOrCreateAssets(UFigmaFile* FigmaFile) override;

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
	TMap<EFigmaStyleType, FString> Styles;

	UPROPERTY()
	TArray<FFigmaLayoutGrid> LayoutGrids;

	UPROPERTY()
	FString ComponentId;

	UPROPERTY()
	bool IsExposedInstance = false;

	UPROPERTY()
	TArray<FString> ExposedInstances;

	UPROPERTY()
	TMap<FString, FFigmaComponentProperty> ComponentProperties;

	UPROPERTY()
	TArray<FFigmaOverrides> Overrides;

	UPROPERTY()
	FImageBuilder BuilderFallback;

	UPROPERTY()
	TObjectPtr<UTexture> MissingComponentTexture = nullptr;
};
