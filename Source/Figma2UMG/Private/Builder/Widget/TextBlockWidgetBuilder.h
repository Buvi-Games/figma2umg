// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "TextBlockWidgetBuilder.generated.h"

class UTextBlock;

UCLASS()
class UTextBlockWidgetBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UTextBlock> Widget = nullptr;
};
