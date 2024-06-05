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

protected:
	UPROPERTY()
	TObjectPtr<UPanelWidget> Widget = nullptr;
};
