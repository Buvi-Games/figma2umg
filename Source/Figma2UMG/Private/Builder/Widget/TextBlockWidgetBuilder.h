// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "TextBlockWidgetBuilder.generated.h"

class UTextBlock;

UCLASS()
class UTextBlockWidgetBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) override;

	virtual TObjectPtr<UWidget> GetWidget() const override;
protected:
	UPROPERTY()
	TObjectPtr<UTextBlock> Widget = nullptr;
};
