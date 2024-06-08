// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SingleChildBuilder.h"
#include "ButtonWidgetBuilder.generated.h"

class UButton;

UCLASS()
class UButtonWidgetBuilder : public USingleChildBuilder
{
public:
	GENERATED_BODY()
	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;

protected:
	virtual TObjectPtr<UContentWidget> GetContentWidget() const override;

	UPROPERTY()
	TObjectPtr<UButton> Widget = nullptr;
};
