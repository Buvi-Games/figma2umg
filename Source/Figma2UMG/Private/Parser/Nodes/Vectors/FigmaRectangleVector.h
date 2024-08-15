// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Parser/Nodes/Vectors/FigmaVectorNode.h"

#include "FigmaRectangleVector.generated.h"

UCLASS()
class FIGMA2UMG_API UFigmaRectangleVector : public UFigmaVectorNode
{
public:
	GENERATED_BODY()

	virtual bool CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders) override;
	virtual TScriptInterface<IWidgetBuilder> CreateWidgetBuilders(bool IsRoot = false, bool AllowFrameButton = true) const override;
protected:
};
