// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "ButtonWidgetBuilder.generated.h"

class UButton;

UCLASS()
class UButtonWidgetBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

	void SetChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder);

protected:
	UPROPERTY()
	TObjectPtr<UButton> Widget = nullptr;

	UPROPERTY()
	TScriptInterface<IWidgetBuilder> SubWidgetBuilder;
};
