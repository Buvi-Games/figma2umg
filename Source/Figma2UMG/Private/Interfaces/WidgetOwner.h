// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Parser/Properties/FigmaEnums.h"

#include "WidgetOwner.generated.h"

class UWidgetBlueprint;
class UPanelWidget;
class UWidget;
class UFigmaNode;
struct FFigmaLayoutConstraint;

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UWidgetOwner : public UInterface
{
	GENERATED_BODY()
};

class FIGMA2UMG_API IWidgetOwner
{
	GENERATED_BODY()
public:
	DECLARE_DELEGATE_OneParam(FOnEachFunction, UWidget&)
	virtual void ForEach(const FOnEachFunction& Function) = 0;

	virtual TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch) = 0;
	virtual void SetupWidget(TObjectPtr<UWidget> Widget) = 0;
	virtual void PostInsert() const;
	virtual void PostInsertWidgets(TObjectPtr<UWidget> TopWidget, TObjectPtr<UPanelWidget> ContentWidget) const;
	virtual void Reset() = 0;

	virtual TObjectPtr<UWidget> GetTopWidget() const = 0;
	virtual FVector2D GetTopWidgetPosition() const = 0;

	virtual TObjectPtr<UPanelWidget> GetContainerWidget() const = 0;

	virtual void PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const = 0;

	void SetPosition(TObjectPtr<UWidget> Widget, const FVector2D& Size) const;
	void SetSize(TObjectPtr<UWidget> Widget, const FVector2D& Size, const bool SizeToContent = false) const;
	void SetPadding(TObjectPtr<UWidget> Widget, const float PaddingLeft, const float PaddingRight, const float PaddingTop, const float PaddingBottom) const;

	void SetConstraints(const TObjectPtr<UWidget>& Widget, EFigmaPrimaryAxisAlignItems PrimaryAlign, EFigmaCounterAxisAlignItems SecondaryAlign) const;
	void SetConstraints(TObjectPtr<UWidget> Widget, const FFigmaLayoutConstraint& InConstraints) const;
	void SetAlign(TObjectPtr<UWidget> Widget, EFigmaTextAlignHorizontal TextAlignHorizontal, EFigmaTextAlignVertical TextAlignVertical) const;

	static void TryRenameWidget(const FString& InName, TObjectPtr<UWidget> Widget);

	template<class Type>
	static Type* NewWidget(UObject* TreeViewOuter, const FString& InName);
protected:
	EHorizontalAlignment Convert(EFigmaTextAlignHorizontal TextAlignHorizontal) const;
	EHorizontalAlignment Convert(EFigmaLayoutConstraintHorizontal LayoutConstraint) const;
	EHorizontalAlignment Convert(EFigmaPrimaryAxisAlignItems  LayoutConstraint) const;
	EVerticalAlignment Convert(EFigmaTextAlignVertical TextAlignVertical) const;
	EVerticalAlignment Convert(EFigmaLayoutConstraintVertical LayoutConstraint) const;
	EVerticalAlignment Convert(EFigmaCounterAxisAlignItems LayoutConstraint) const;
};

template <class Type>
Type* IWidgetOwner::NewWidget(UObject* TreeViewOuter, const FString& InName)
{
	const FString UniqueName = MakeUniqueObjectName(TreeViewOuter, Type::StaticClass(), *InName).ToString();
	return NewObject<Type>(TreeViewOuter, *UniqueName);
}
