// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "Parser/Properties/FigmaComponentPropertyDefinition.h"

#include "FigmaComponentSet.generated.h"

class UFigmaInstance;

UCLASS()
class FIGMA2UMG_API UFigmaComponentSet : public  UFigmaFrame
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;
	virtual TScriptInterface<IWidgetBuilder> CreateWidgetBuilders(bool IsRoot = false, bool AllowFrameButton = true) const override;
	virtual FString GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const override;

	TObjectPtr<UFigmaInstance> InstanciateFigmaComponent(const FString& InstanceID);

	UPROPERTY()
	TMap<FString, FFigmaComponentPropertyDefinition> ComponentPropertyDefinitions;

protected:
	bool IsDoingInPlace = false;

	UPROPERTY()
	TArray<UFigmaNode*> ButtonSubNodes;
};
