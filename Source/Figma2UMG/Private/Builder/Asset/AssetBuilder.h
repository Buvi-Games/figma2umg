// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetBuilder.generated.h"

class UFigmaFile;
class UFigmaNode;

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UAssetBuilder : public UInterface
{
	GENERATED_BODY()
};

class FIGMA2UMG_API IAssetBuilder
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual void SetNode(const FString& InFileKey, const UFigmaNode* InNode);

	UFUNCTION()
	virtual void LoadOrCreateAssets() = 0;

	UFUNCTION()
	virtual void LoadAssets() = 0;

	UFUNCTION()
	virtual UPackage* GetPackage() const = 0;

	UFUNCTION()
	virtual void Reset() = 0;

protected:
	FString FileKey;
	const UFigmaNode* Node = nullptr;
};
