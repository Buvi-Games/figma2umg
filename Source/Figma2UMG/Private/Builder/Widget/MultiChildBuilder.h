// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "MultiChildBuilder.generated.h"

UCLASS(Abstract)
class FIGMA2UMG_API UMultiChildBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

	void AddChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder);
	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch) override PURE_VIRTUAL(UMultiChildBuilder::PatchAndInsertWidget());
	virtual void PostInsertWidgets(TObjectPtr<UWidgetBlueprint> WidgetBlueprint) override;
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) override;
	virtual void PatchWidgetBinds(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint) override;
	virtual void PatchWidgetProperties() override;

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override PURE_VIRTUAL(UMultiChildBuilder::SetWidget;);
	virtual TObjectPtr<UWidget> GetWidget() const override;
	virtual TObjectPtr<UWidget> FindWidgetRecursive(const FString& WidgetName) const override;
	virtual void ResetWidget() override;
protected:
	virtual TObjectPtr<UPanelWidget> GetPanelWidget() const PURE_VIRTUAL(UMultiChildBuilder::GetPanelWidget(), return nullptr;);
	virtual void PatchAndInsertChildren(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UPanelWidget>& ParentWidget);
	void SetChildrenWidget(TObjectPtr<UPanelWidget> ParentWidget);

	void FixSpacers(const TObjectPtr<UPanelWidget>& PanelWidget) const;
	UPROPERTY()
	TArray<TScriptInterface<IWidgetBuilder>> ChildWidgetBuilders;
};
