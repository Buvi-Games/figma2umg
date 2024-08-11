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
	void SetMaterial(const TObjectPtr<UMaterialInterface>& InMaterial);
	void SetColor(const FLinearColor& InColor);

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch) override;
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) override;

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override;
	virtual TObjectPtr<UWidget> GetWidget() const override;
	virtual void ResetWidget() override;
protected:
	UPROPERTY()
	TObjectPtr<UTexture2DBuilder> Texture2DBuilder = nullptr;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> Material = nullptr;

	bool HasSolidColor = false;
	FLinearColor SolidColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);

	UPROPERTY()
	TObjectPtr<UImage> Widget = nullptr;
};
