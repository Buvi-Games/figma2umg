// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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

	TObjectPtr<UWidget> FindNodeWidgetInParent(const TObjectPtr<UPanelWidget>& ParentWidget) const;

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) = 0;
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) = 0;

	virtual TObjectPtr<UWidget> GetWidget() const = 0;
protected:
	bool Insert(const TObjectPtr<UWidgetTree>& WidgetTree, const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) const;
	void OnInsert() const;

	virtual void SetPosition() const;
	virtual void SetSize() const;
	virtual void SetPadding() const;
	virtual void SetConstraintsAndAlign() const;

	bool GetSizeValue(FVector2D& Size, bool& SizeToContent) const;
	void GetPaddingValue(FMargin& Padding) const;
	bool GetAlignmentValues(EHorizontalAlignment& HorizontalAlignment, EVerticalAlignment& VerticalAlignment) const;

	EHorizontalAlignment Convert(EFigmaTextAlignHorizontal TextAlignHorizontal) const;
	EHorizontalAlignment Convert(EFigmaLayoutConstraintHorizontal LayoutConstraint) const;
	EHorizontalAlignment Convert(EFigmaPrimaryAxisAlignItems  LayoutConstraint) const;
	EVerticalAlignment Convert(EFigmaTextAlignVertical TextAlignVertical) const;
	EVerticalAlignment Convert(EFigmaLayoutConstraintVertical LayoutConstraint) const;
	EVerticalAlignment Convert(EFigmaCounterAxisAlignItems LayoutConstraint) const;

	const UFigmaNode* Node = nullptr;

private:
	TScriptInterface<IWidgetBuilder> Parent = nullptr;
};

UCLASS(Abstract)
class USingleChildBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

	void SetChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder);

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override PURE_VIRTUAL(USingleChildBuilder::PatchAndInsertWidget());
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) override;

	virtual TObjectPtr<UWidget> GetWidget() const override;
protected:
	virtual TObjectPtr<UContentWidget> GetContentWidget() const PURE_VIRTUAL(USingleChildBuilder::GetContentWidget(), return nullptr;);
	virtual void PatchAndInsertChild(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UContentWidget>& ParentWidget);

	UPROPERTY()
	TScriptInterface<IWidgetBuilder> ChildWidgetBuilder = nullptr;
};


UCLASS(Abstract)
class UMultiChildBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

	void AddChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder);
	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override PURE_VIRTUAL(UMultiChildBuilder::PatchAndInsertWidget());
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) override;

	virtual TObjectPtr<UWidget> GetWidget() const override;
protected:
	virtual TObjectPtr<UPanelWidget> GetPanelWidget() const PURE_VIRTUAL(UMultiChildBuilder::GetPanelWidget(), return nullptr;);
	virtual void PatchAndInsertChildren(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UPanelWidget>& ParentWidget);

	UPROPERTY()
	TArray<TScriptInterface<IWidgetBuilder>> ChildWidgetBuilders;
};
