// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBlueprint.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Properties/FigmaEnums.h"
#include "Parser/Properties/FigmaPaint.h"
#include "WidgetBuilder.generated.h"

class UBorder;
class UImage;
class UWidgetTree;
class UWidget;
class UContentWidget;
class UPanelWidget;
class UFigmaNode;


UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UWidgetBuilder : public UInterface
{
	GENERATED_BODY()
};

class FIGMA2UMG_API IWidgetBuilder
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual void SetNode(const UFigmaNode* InNode);

	UFUNCTION()
	virtual void SetParent(TScriptInterface<IWidgetBuilder> InParent);

	virtual TObjectPtr<UWidget> FindNodeWidgetInParent(const TObjectPtr<UPanelWidget>& ParentWidget) const;

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch) = 0;
	virtual void PostInsertWidgets(TObjectPtr<UWidgetBlueprint> WidgetBlueprint) {}
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) = 0;
	virtual void PatchWidgetBinds(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint);
	virtual void PatchWidgetProperties() {}

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) = 0;
	virtual TObjectPtr<UWidget> GetWidget() const = 0;
	virtual TObjectPtr<UWidget> FindWidgetRecursive(const FString& WidgetName) const;
	virtual void ResetWidget() = 0;

	bool IsInsideComponentPackage(FString PackagePath) const;
protected:
	bool Insert(const TObjectPtr<UWidgetTree>& WidgetTree, const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) const;
	void OnInsert() const;

	bool IsTopWidgetForNode() const;

	void SetPosition() const;
	void SetRotation() const;
	void SetSize() const;
	void SetPadding() const;
	void SetOpacity() const;
	void SetConstraintsAndAlign() const;
	void SetClipsContent() const;

	void SetFill(const TArray<FFigmaPaint>& Fills) const;
	template<class WidgetT>
	void SetStroke(TObjectPtr<WidgetT> Widget, const TArray<FFigmaPaint>& Strokes, const float& StrokeWeight) const;
	template<class WidgetT>
	void SetCorner(TObjectPtr<WidgetT> Widget, const FVector4& CornerRadii, bool bForceRoundCorner = false) const;

	virtual bool GetSizeValue(FVector2D& Size, bool& SizeToContent) const;
	virtual void GetPaddingValue(FMargin& Padding) const;
	virtual bool GetAlignmentValues(EHorizontalAlignment& HorizontalAlignment, EVerticalAlignment& VerticalAlignment) const;

	EHorizontalAlignment Convert(EFigmaTextAlignHorizontal TextAlignHorizontal) const;
	EHorizontalAlignment Convert(EFigmaLayoutConstraintHorizontal LayoutConstraint) const;
	EHorizontalAlignment Convert(EFigmaPrimaryAxisAlignItems  LayoutConstraint) const;
	EVerticalAlignment Convert(EFigmaTextAlignVertical TextAlignVertical) const;
	EVerticalAlignment Convert(EFigmaLayoutConstraintVertical LayoutConstraint) const;
	EVerticalAlignment Convert(EFigmaCounterAxisAlignItems LayoutConstraint) const;

	void ProcessComponentPropertyReference(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint, const TObjectPtr<UWidget>& Widget, const TPair<FString, FString>& PropertyReference) const;

	const UFigmaNode* Node = nullptr;

protected:
	FSlateBrush GetBrush(TObjectPtr<UBorder> Widget) const;
	FSlateBrush GetBrush(TObjectPtr<UImage> Widget) const;
	void SetBrush(TObjectPtr<UBorder> Widget, FSlateBrush& Brush) const;
	void SetBrush(TObjectPtr<UImage> Widget, FSlateBrush& Brush) const;

	void SetColorAndOpacity(TObjectPtr<UBorder> Widget, const FLinearColor& Color) const;
	void SetColorAndOpacity(TObjectPtr<UImage> Widget, const FLinearColor& Color) const;

private:
	TScriptInterface<IWidgetBuilder> Parent = nullptr;
};

template <class WidgetT>
void IWidgetBuilder::SetStroke(TObjectPtr<WidgetT> Widget, const TArray<FFigmaPaint>& Strokes, const float& StrokeWeight) const
{
	if (!Widget)
		return;

	FSlateBrush Brush = GetBrush(Widget);
	if (Strokes.Num() > 0 && Strokes[0].Visible)
	{
		for (const FFigmaPaint& Paint : Strokes)
		{
			if (const TObjectPtr<UMaterialInterface> Material = Paint.GetMaterial())
			{
				Widget->SetBrushFromMaterial(Material);
				Brush.OutlineSettings.Color = Strokes[0].GetLinearColor();
				Brush.OutlineSettings.Width = StrokeWeight;
				Brush.TintColor = FLinearColor::White;
				Brush.DrawAs = ESlateBrushDrawType::Box;
				Brush.Margin.Top = 0.5f;
				Brush.Margin.Bottom = 0.5f;
				Brush.Margin.Left = 0.5f;
				Brush.Margin.Right = 0.5f;
				Widget->SetBrush(Brush);
				SetColorAndOpacity(Widget, Paint.GetLinearColor());
				return;
			}
		}

		Brush.OutlineSettings.Color = Strokes[0].GetLinearColor();
		Brush.OutlineSettings.Width = StrokeWeight;
		SetBrush(Widget, Brush);
	}
	else
	{
		Brush.OutlineSettings.Color = FLinearColor(1.0f, 1.0f, 1.0f, 0.0f);
		Brush.OutlineSettings.Width = 0.0f;
		SetBrush(Widget, Brush);
	}
}

template <class WidgetT>
void IWidgetBuilder::SetCorner(TObjectPtr<WidgetT> Widget, const FVector4& CornerRadii, bool bForceRoundCorner /*= false*/) const
{
	if (!Widget)
		return;

	FSlateBrush Brush = GetBrush(Widget);
	if (Brush.GetDrawType() == ESlateBrushDrawType::RoundedBox || bForceRoundCorner)
	{
		Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
		Brush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		Brush.OutlineSettings.CornerRadii = CornerRadii;
		FVector2D AbsoluteSize = Node->GetAbsoluteSize();
		AbsoluteSize.X -= Brush.OutlineSettings.Width * 2.0f;
		AbsoluteSize.Y -= Brush.OutlineSettings.Width * 2.0f;

		const float fMaxHalfRadius = FMath::Min(AbsoluteSize.X, AbsoluteSize.Y) * 0.5f;
		if ((Brush.OutlineSettings.CornerRadii.X >= fMaxHalfRadius)
			&& (Brush.OutlineSettings.CornerRadii.X >= fMaxHalfRadius)
			&& (Brush.OutlineSettings.CornerRadii.X >= fMaxHalfRadius)
			&& (Brush.OutlineSettings.CornerRadii.X >= fMaxHalfRadius))
		{
			Brush.OutlineSettings.RoundingType = ESlateBrushRoundingType::HalfHeightRadius;
		}

		Brush.OutlineSettings.CornerRadii.X = FMath::Min(Brush.OutlineSettings.CornerRadii.X, FMath::Min(AbsoluteSize.X, AbsoluteSize.Y) * 0.5f);
		Brush.OutlineSettings.CornerRadii.Y = FMath::Min(Brush.OutlineSettings.CornerRadii.Y, FMath::Min(AbsoluteSize.X, AbsoluteSize.Y) * 0.5f);
		Brush.OutlineSettings.CornerRadii.Z = FMath::Min(Brush.OutlineSettings.CornerRadii.Z, FMath::Min(AbsoluteSize.X, AbsoluteSize.Y) * 0.5f);
		Brush.OutlineSettings.CornerRadii.W = FMath::Min(Brush.OutlineSettings.CornerRadii.W, FMath::Min(AbsoluteSize.X, AbsoluteSize.Y) * 0.5f);

		SetBrush(Widget, Brush);
	}
}
