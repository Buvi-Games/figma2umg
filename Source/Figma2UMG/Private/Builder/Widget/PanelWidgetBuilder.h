// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "PanelWidgetBuilder.generated.h"

class UPanelWidget;

UCLASS()
class UPanelWidgetBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

	void AddChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder);

protected:
	UPROPERTY()
	TObjectPtr<UPanelWidget> Widget = nullptr;

	UPROPERTY()
	TArray<TScriptInterface<IWidgetBuilder>> ChildWidgetBuilders;
};