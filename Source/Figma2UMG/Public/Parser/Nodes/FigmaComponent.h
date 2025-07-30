// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "Parser/Properties/FigmaComponentPropertyDefinition.h"

#include "FigmaComponent.generated.h"

class UFigmaInstance;
struct FEdGraphPinType;

UCLASS()
class FIGMA2UMG_API UFigmaComponent : public  UFigmaFrame
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;
	virtual FString GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const override;

	void TryAddComponentPropertyDefinition(FString PropertyId, FFigmaComponentPropertyDefinition Definition);
	TObjectPtr<UFigmaInstance> InstanciateFigmaComponent(const FString& InstanceID);

	UPROPERTY()
	TMap<FString, FFigmaComponentPropertyDefinition> ComponentPropertyDefinitions;
};
