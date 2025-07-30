// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"
#include "SingleChildBuilder.h"
#include "Parser/Properties/FigmaEnums.h"
#include "SizeBoxWidgetBuilder.generated.h"

class USizeBox;

UCLASS()
class FIGMA2UMG_API USizeBoxWidgetBuilder : public USingleChildBuilder
{
public:
	GENERATED_BODY()

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch) override;

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override;
	virtual void ResetWidget() override;
protected:
	virtual TObjectPtr<UContentWidget> GetContentWidget() const override;
	virtual void GetPaddingValue(FMargin& Padding) const override;

	void Setup() const;
	void GetValues(EFigmaLayoutSizing& LayoutSizingHorizontal, EFigmaLayoutSizing& LayoutSizingVertical, float& FixedWidth, float& FixedHeight) const;

	UPROPERTY()
	TObjectPtr<USizeBox> Widget = nullptr;
};