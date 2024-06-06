// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "SizeBoxWidgetBuilder.generated.h"

class USizeBox;

UCLASS()
class USizeBoxWidgetBuilder : public USingleChildBuilder
{
public:
	GENERATED_BODY()

	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override;

protected:
	UPROPERTY()
	TObjectPtr<USizeBox> Widget = nullptr;
};