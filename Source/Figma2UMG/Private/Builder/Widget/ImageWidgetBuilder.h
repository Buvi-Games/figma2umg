// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "ImageWidgetBuilder.generated.h"

class UTexture2DBuilder;
class UWidget;
class UImage;

UCLASS()
class FIGMA2UMG_API UImageWidgetBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()
	void SetTexture2DBuilder(const TObjectPtr<UTexture2DBuilder>& InTexture2DBuilder);
	void SetMaterial(const TObjectPtr<UMaterialInterface>& InMaterial, const FLinearColor& InColor);
	void SetColor(const FLinearColor& InColor);

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch) override;
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) override;

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override;
	virtual TObjectPtr<UWidget> GetWidget() const override;
	virtual void ResetWidget() override;
protected:
	void Setup() const;
	void SetupFill() const;
	FLinearColor GetTintColor() const;
	TEnumAsByte<enum ESlateBrushDrawType::Type > GetDrawAs(TEnumAsByte<ESlateBrushDrawType::Type> DefaultReturn = ESlateBrushDrawType::Box) const;

	UPROPERTY()
	TObjectPtr<UTexture2DBuilder> Texture2DBuilder = nullptr;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> Material = nullptr;

	bool HasSolidColor = false;
	FLinearColor SolidColor = FLinearColor::White;

	UPROPERTY()
	TObjectPtr<UImage> Widget = nullptr;
};
