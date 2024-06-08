// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "SingleChildBuilder.generated.h"


UCLASS(Abstract)
class USingleChildBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

	void SetChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder);

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override PURE_VIRTUAL(USingleChildBuilder::PatchAndInsertWidget());
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) override;
	virtual void PatchWidgetBinds(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint) override;

	virtual TObjectPtr<UWidget> GetWidget() const override;
protected:
	virtual TObjectPtr<UContentWidget> GetContentWidget() const PURE_VIRTUAL(USingleChildBuilder::GetContentWidget(), return nullptr;);
	virtual void PatchAndInsertChild(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UContentWidget>& ParentWidget);

	UPROPERTY()
	TScriptInterface<IWidgetBuilder> ChildWidgetBuilder = nullptr;
};
