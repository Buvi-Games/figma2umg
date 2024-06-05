// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "SizeBoxWidgetBuilder.generated.h"

class USizeBox;

UCLASS()
class USizeBoxWidgetBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

	void SetChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder);

protected:
	UPROPERTY()
	TObjectPtr<USizeBox> Widget = nullptr;

	UPROPERTY()
	TScriptInterface<IWidgetBuilder> SubWidgetBuilder;
};