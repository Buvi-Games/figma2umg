// Copyright 2024 Buvi Games. All Rights Reserved.

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
	UObject* RefAsset = nullptr;
	UObject* InstanceAsset = nullptr;
};
