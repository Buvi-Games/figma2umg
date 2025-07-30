// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "Parser/Properties/FigmaTypeStyle.h"
#include "TextBlockWidgetBuilder.generated.h"

class UObjectLibrary;
class UTextBlock;

UCLASS()
class FIGMA2UMG_API UTextBlockWidgetBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch) override;
	virtual bool TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget) override;

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override;
	virtual TObjectPtr<UWidget> GetWidget() const override;
	virtual void ResetWidget() override;
protected:
	void Setup() const;
	void SetStyle(const FFigmaTypeStyle& Style) const;
	float ConvertFontSizeFromDisplayToNative(float DisplayFontSize) const;

	UPROPERTY()
	TObjectPtr<UTextBlock> Widget = nullptr;
};
