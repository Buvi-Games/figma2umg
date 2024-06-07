// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "Parser/Properties/FigmaEnums.h"
#include "SizeBoxWidgetBuilder.generated.h"

class USizeBox;

UCLASS()
class USizeBoxWidgetBuilder : public USingleChildBuilder
{
public:
	GENERATED_BODY()

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;

protected:
	virtual TObjectPtr<UContentWidget> GetContentWidget() override;

	void Setup() const;
	void GetValues(EFigmaLayoutSizing& LayoutSizingHorizontal, EFigmaLayoutSizing& LayoutSizingVertical, float& FixedWidth, float& FixedHeight) const;

	UPROPERTY()
	TObjectPtr<USizeBox> Widget = nullptr;
};