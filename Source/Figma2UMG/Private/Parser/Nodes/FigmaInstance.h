// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include <Interfaces/FlowTransition.h>

#include "CoreMinimal.h"
#include "FigmaComponent.h"
#include "FigmaNode.h"
#include "Parser/Properties/FigmaComponentProperty.h"
#include "Parser/Properties/FigmaLayoutConstraint.h"
#include "Parser/Properties/FigmaOverrides.h"
#include "Parser/Properties/FigmaPaint.h"
#include "Parser/Properties/FigmaRectangle.h"

#include "FigmaInstance.generated.h"

struct FFigmaLayoutGrid;
enum class EFigmaStyleType;
struct FFigmaEffect;
struct FFigmaExportSetting;
class UTexture2DBuilder;

UCLASS()
class FIGMA2UMG_API UFigmaInstance : public UFigmaNode, public IFlowTransition
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PrepareForFlow() override;
	virtual FVector2D GetAbsolutePosition() const override;
	virtual FVector2D GetAbsoluteSize() const override;
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;
	virtual bool CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders) override;
	virtual FString GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const override;
	virtual TScriptInterface<IWidgetBuilder> CreateWidgetBuilders(bool IsRoot = false, bool AllowFrameButton = true) const override;

	// FlowTransition
	virtual const bool HasTrigger(const EFigmaTriggerType TriggerType) const override;
	virtual const bool HasAction(const EFigmaActionType ActionType, const EFigmaActionNodeNavigation Navigation) const override;
	virtual const FFigmaInteraction& GetInteractionFromTrigger(const EFigmaTriggerType TriggerType) const override;
	virtual const FFigmaInteraction& GetInteractionFromAction(const EFigmaActionType ActionType, const EFigmaActionNodeNavigation Navigation) const override;
	virtual const FString& GetDestinationIdFromEvent(const FName& EventName) const override;
	virtual void GetAllDestinationId(TArray<FString>& TransitionNodeIDs) const override;
	virtual const float GetTransitionDuration() const override { return TransitionDuration; }
	virtual const EFigmaEasingType GetTransitionEasing() const override { return TransitionEasing; };

	const FFigmaComponentPropertyDefinition* IsInstanceSwap() const;

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
	TArray<FFigmaInteraction> Interactions;

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

	UFigmaNode* FindNodeForOverriden(const FString& NodeId) const;
protected:
	void ProcessChildrenComponentPropertyReferences(TObjectPtr<UWidgetBlueprint> WidgetBp, TObjectPtr<UWidget> Widget, const TArray<UFigmaNode*>& CurrentChildren) const;
	UFigmaNode* FindNodeForOverriden(const FString& NodeId, const TArray<UFigmaNode*>& Children) const;

	bool IsMissingComponent = false;

	UPROPERTY()
	TObjectPtr<UTexture2DBuilder> Texture2DBuilder = nullptr;

	UPROPERTY()
	TObjectPtr<UTexture> MissingComponentTexture = nullptr;

	UPROPERTY()
	mutable TArray<UFigmaInstance*> InstanceSwapValues;
};
