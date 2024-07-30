// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBlueprint.h"
#include "Parser/Properties/FigmaEnums.h"
#include "WidgetBuilder.generated.h"

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

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) = 0;
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) = 0;
	virtual void PatchWidgetBinds(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint);
	virtual void PatchWidgetProperties() {}

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) = 0;
	virtual TObjectPtr<UWidget> GetWidget() const = 0;
	virtual TObjectPtr<UWidget> FindWidgetRecursive(const FString& WidgetName) const;
	virtual void ResetWidget() = 0;

protected:
	bool Insert(const TObjectPtr<UWidgetTree>& WidgetTree, const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) const;
	void OnInsert() const;

	bool IsTopWidgetForNode() const;

	void SetPosition() const;
	void SetSize() const;
	void SetPadding() const;
	void SetOpacity() const;
	void SetConstraintsAndAlign() const;

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

private:
	TScriptInterface<IWidgetBuilder> Parent = nullptr;
};
