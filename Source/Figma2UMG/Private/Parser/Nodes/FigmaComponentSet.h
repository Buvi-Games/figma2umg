// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "Parser/Properties/FigmaComponentPropertyDefinition.h"

#include "FigmaComponentSet.generated.h"

UCLASS()
class UFigmaComponentSet : public  UFigmaFrame
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;
	virtual TScriptInterface<IWidgetBuilder> CreateWidgetBuilders(bool IsRoot = false, bool AllowFrameButton = true) const override;
	virtual FString GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const override;

	UPROPERTY()
	TMap<FString, FFigmaComponentPropertyDefinition> ComponentPropertyDefinitions;

protected:
	bool IsDoingInPlace = false;

	UPROPERTY()
	TArray<UFigmaNode*> ButtonSubNodes;
};
