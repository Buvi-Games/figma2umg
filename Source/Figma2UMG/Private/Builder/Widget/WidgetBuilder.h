// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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

	virtual TObjectPtr<UWidget> GetWidget() = 0;
protected:
	bool Insert(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget);

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

	virtual TObjectPtr<UWidget> GetWidget() override;
protected:
	virtual TObjectPtr<UContentWidget> GetContentWidget() PURE_VIRTUAL(USingleChildBuilder::PatchAndInsertWidget(), return nullptr;);
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

	virtual TObjectPtr<UWidget> GetWidget() override;
protected:
	virtual TObjectPtr<UPanelWidget> GetPanelWidget() PURE_VIRTUAL(UMultiChildBuilder::GetPanelWidget(), return nullptr;);
	virtual void PatchAndInsertChildren(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UPanelWidget>& ParentWidget);

	UPROPERTY()
	TArray<TScriptInterface<IWidgetBuilder>> ChildWidgetBuilders;
};
