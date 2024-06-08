// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "ImageWidgetBuilder.generated.h"

class UTexture2DBuilder;
class UWidget;
class UImage;

UCLASS()
class UImageWidgetBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()
	void SetTexture2DBuilder(const TObjectPtr<UTexture2DBuilder>& InTexture2DBuilder);

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) override;

	virtual TObjectPtr<UWidget> GetWidget() const override;
protected:
	UPROPERTY()
	TObjectPtr<UTexture2DBuilder> Texture2DBuilder = nullptr;

	UPROPERTY()
	TObjectPtr<UImage> Widget = nullptr;
};
