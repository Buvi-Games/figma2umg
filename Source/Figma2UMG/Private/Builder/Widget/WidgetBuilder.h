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

	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) = 0;

protected:
	TScriptInterface<IWidgetBuilder> Parent = nullptr;
	const UFigmaNode* Node = nullptr;
};

UCLASS(Abstract)
class USingleChildBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

	void SetChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder);

	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) PURE_VIRTUAL(USingleChildBuilder::PatchPreInsertWidget(), return nullptr;);

protected:
	virtual void PatchPreInsertChild(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UContentWidget>& ParentWidget);

	UPROPERTY()
	TScriptInterface<IWidgetBuilder> ChildWidgetBuilder = nullptr;
};


UCLASS(Abstract)
class UMultiChildBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

	void AddChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder);

	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) PURE_VIRTUAL(UMultiChildBuilder::PatchPreInsertWidget(), return nullptr;);

protected:
	virtual void PatchPreInsertChildren(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UPanelWidget>& ParentWidget);

	UPROPERTY()
	TArray<TScriptInterface<IWidgetBuilder>> ChildWidgetBuilders;
};
