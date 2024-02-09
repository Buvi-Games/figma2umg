// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FigmaImageRequester.generated.h"

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UFigmaImageRequester : public UInterface
{
	GENERATED_BODY()
	
};

class FIGMA2UMG_API IFigmaImageRequester
{
	GENERATED_BODY()
public:
	virtual void AddImageIds(TArray<FString> Array) = 0;
};
