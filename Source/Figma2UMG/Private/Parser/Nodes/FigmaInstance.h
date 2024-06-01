// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "Builder/ImageBuilder.h"
#include "Interfaces/AssetFileHandler.h"
#include "Interfaces/FigmaImageRequester.h"
#include "Interfaces/FigmaRefHandle.h"
#include "Parser/Properties/FigmaComponentProperty.h"
#include "Parser/Properties/FigmaOverrides.h"

#include "FigmaInstance.generated.h"

UCLASS()
class UFigmaInstance : public UFigmaNode, public IWidgetOwner, public IFigmaRefHandle, public IFigmaImageRequester, public IFigmaFileHandle, public IFlowTransition
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PrepareForFlow();
	virtual FVector2D GetAbsolutePosition() const override;
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;

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

	// IFigmaImageRequester
	virtual void AddImageRequest(FString FileKey, FImageRequests& ImageRequests) override;
	virtual void OnRawImageReceived(TArray<uint8>& RawData) override;

	// IFigmaFileHandle
	virtual FString GetPackagePath() const override;
	virtual FString GetAssetName() const override;
	virtual void LoadOrCreateAssets() override;
	virtual void LoadAssets() override;

	// FlowTransition
	virtual const FString& GetTransitionNodeID() const override { return TransitionNodeID; }
	virtual const float GetTransitionDuration() const override { return TransitionDuration; };
	virtual const EFigmaEasingType GetTransitionEasing() const override { return TransitionEasing; };

	void PatchComponentProperty() const;
	FString GetComponentId() const { return ComponentId; }
protected:
	void ProcessChildrenComponentPropertyReferences(TObjectPtr<UWidgetBlueprint> WidgetBp, TObjectPtr<UWidget> Widget, const TArray<UFigmaNode*>& CurrentChildren) const;

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
	EFigmaLayoutSizing LayoutSizingHorizontal;

	UPROPERTY()
	EFigmaLayoutSizing LayoutSizingVertical;

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

	bool IsMissingComponent = false;
	UPROPERTY()
	TObjectPtr<UTexture> MissingComponentTexture = nullptr;
};
