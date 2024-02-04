// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FigmaRefHandle.generated.h"

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UFigmaRefHandle : public UInterface
{
	GENERATED_BODY()
};

class FIGMA2UMG_API IFigmaRefHandle
{
	GENERATED_BODY()
public:

protected:
	UObject* Asset = nullptr;
};
