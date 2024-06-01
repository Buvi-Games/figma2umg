// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "SizeBoxWidgetBuilder.generated.h"

class USizeBox;

UCLASS()
class USizeBoxWidgetBuilder : public UWidgetBuilder
{
public:
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<USizeBox> Widget = nullptr;
};
