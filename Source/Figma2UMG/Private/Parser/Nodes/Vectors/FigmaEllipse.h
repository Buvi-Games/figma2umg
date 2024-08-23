// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Parser/Nodes/Vectors/FigmaVectorNode.h"
#include "Parser/Properties/FigmaArcData.h"

#include "FigmaEllipse.generated.h"

UCLASS()
class FIGMA2UMG_API UFigmaEllipse : public UFigmaVectorNode
{
public:
	GENERATED_BODY()

	virtual bool DoesSupportImageRef() const override;

	UPROPERTY()
	FFigmaArcData ArcData;
};
