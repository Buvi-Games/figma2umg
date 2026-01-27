// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/FigmaContainer.h"
#include "Interfaces/FlowTransition.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Properties/FigmaBlendMode.h"
#include "Parser/Properties/FigmaColor.h"
#include "Parser/Properties/FigmaEffect.h"
#include "Parser/Properties/FigmaEnums.h"
#include "Parser/Properties/FigmaExportSetting.h"
#include "Parser/Properties/FigmaInteraction.h"
#include "Parser/Properties/FigmaLayoutConstraint.h"
#include "Parser/Properties/FigmaPaint.h"
#include "Parser/Properties/FigmaRectangle.h"
#include "Parser/Properties/FigmaStyleRef.h"
#include "Parser/Properties/FigmaTransform.h"
#include "Parser/Properties/FigmaVector.h"

#include "FigmaGroup.generated.h"

class UButtonWidgetBuilder;

// Enum for grid track sizing types (CSS Grid compatible)
UENUM()
enum class EFigmaGridTrackType : uint8
{
	Fixed,       // Fixed pixel size (e.g., "100px" or just "100")
	Fractional,  // Fractional unit (e.g., "1fr", "2fr")
	Auto         // Automatic sizing based on content
};

// Structure representing a single grid track (column or row) sizing
USTRUCT()
struct FIGMA2UMG_API FFigmaGridTrackSizing
{
	GENERATED_BODY()

	UPROPERTY()
	EFigmaGridTrackType Type = EFigmaGridTrackType::Auto;

	UPROPERTY()
	float Value = 1.0f;

	FFigmaGridTrackSizing() = default;
	FFigmaGridTrackSizing(EFigmaGridTrackType InType, float InValue) : Type(InType), Value(InValue) {}
};

UCLASS()
class FIGMA2UMG_API UFigmaGroup : public UFigmaNode, public IFigmaContainer, public IFlowTransition
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;
	virtual bool CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders) override;
	virtual FString GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const override;
	virtual TScriptInterface<IWidgetBuilder> CreateWidgetBuilders(bool IsRoot = false, bool AllowFrameButton = true) const override;
	virtual FVector2D GetAbsolutePosition(const bool IsTopWidgetForNode) const override;
	virtual FVector2D GetAbsoluteSize(const bool IsTopWidgetForNode) const override;
	virtual FVector2D GetAbsoluteCenter() const override;

	// IFigmaContainer
	virtual FString GetJsonArrayName() const override { return FString("Children"); };
	virtual TArray<UFigmaNode*>& GetChildren() override { return Children; }
	virtual const TArray<UFigmaNode*>& GetChildrenConst() const override { return Children; }

	FMargin GetPadding() const;

	// FlowTransition
	virtual const FFigmaInteraction& GetInteractionFromTrigger(const EFigmaTriggerType TriggerType) const override;
	virtual const FFigmaInteraction& GetInteractionFromAction(const EFigmaActionType ActionType, const EFigmaActionNodeNavigation Navigation) const override;
	virtual const FString& GetDestinationIdFromEvent(const FName& EventName) const override;
	virtual const float GetTransitionDuration() const override { return TransitionDuration; };
	virtual const EFigmaEasingType GetTransitionEasing() const override { return TransitionEasing; };

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
	EFigmaStrokeAlign StrokeAlign;

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
	bool ClipsContent = false;

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

	// CSS Grid properties (added for Figma API compatibility)
	UPROPERTY()
	int32 GridColumnCount = 0;

	UPROPERTY()
	int32 GridRowCount = 0;

	UPROPERTY()
	float GridRowGap = 0.0f;

	UPROPERTY()
	float GridColumnGap = 0.0f;

	UPROPERTY()
	FString GridColumnsSizing;

	UPROPERTY()
	FString GridRowsSizing;

	UPROPERTY()
	EFigmaLayoutPositioning LayoutPositioning = EFigmaLayoutPositioning::AUTO;

	UPROPERTY()
	bool ItemReverseZIndex = false;

	UPROPERTY()
	bool StrokesIncludedInLayout = false;

	UPROPERTY()
	EFigmaOverflowDirection OverflowDirection = EFigmaOverflowDirection::NONE;

	UPROPERTY()
	FString ScrollBehavior;

	UPROPERTY()
	TArray<FFigmaPaint> Background;

	UPROPERTY()
	TArray<FFigmaEffect> Effects;

	UPROPERTY()
	bool IsMask = false;

	UPROPERTY()
	FString MaskType;

	UPROPERTY()
	TMap<EFigmaStyleType, FString> Styles;

	// Grid sizing parsing methods
	TArray<FFigmaGridTrackSizing> GetParsedColumnSizing() const;
	TArray<FFigmaGridTrackSizing> GetParsedRowSizing() const;

	bool IsButton() const;

protected:
	TScriptInterface<UButtonWidgetBuilder> CreateButtonBuilder() const;
	TScriptInterface<IWidgetBuilder> CreateContainersBuilder(bool IsRoot = false) const;

	void FixSpacers(const TObjectPtr<UPanelWidget>& PanelWidget) const;
	static FFigmaGridTrackSizing ParseSingleTrackValue(const FString& Value);
	static TArray<FFigmaGridTrackSizing> ParseGridSizingString(const FString& SizingString);
};

