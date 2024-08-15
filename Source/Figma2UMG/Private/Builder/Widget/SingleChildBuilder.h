// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "SingleChildBuilder.generated.h"


UCLASS(Abstract)
class FIGMA2UMG_API USingleChildBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

	void SetChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder);

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch) override PURE_VIRTUAL(USingleChildBuilder::PatchAndInsertWidget());
	virtual void PostInsertWidgets(TObjectPtr<UWidgetBlueprint> WidgetBlueprint) override;
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) override;
	virtual void PatchWidgetBinds(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint) override;
	virtual void PatchWidgetProperties() override;

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override PURE_VIRTUAL(UMultiChildBuilder::SetWidget;);
	virtual TObjectPtr<UWidget> GetWidget() const override;
	virtual TObjectPtr<UWidget> FindWidgetRecursive(const FString& WidgetName) const override;
	virtual void ResetWidget() override;
protected:
	virtual TObjectPtr<UContentWidget> GetContentWidget() const PURE_VIRTUAL(USingleChildBuilder::GetContentWidget(), return nullptr;);
	virtual void PatchAndInsertChild(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UContentWidget>& ParentWidget);
	void SetChildWidget(TObjectPtr<UContentWidget> ParentWidget);

	UPROPERTY()
	TScriptInterface<IWidgetBuilder> ChildWidgetBuilder = nullptr;
};
