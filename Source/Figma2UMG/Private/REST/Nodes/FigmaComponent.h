// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "Interfaces/AssetFileHandler.h"
#include "REST/Properties/FigmaComponentPropertyDefinition.h"

#include "FigmaComponent.generated.h"

UCLASS()
class UFigmaComponent : public  UFigmaFrame, public IFigmaFileHandle
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PrePatchWidget() override;
	virtual void PostInsert(UWidget* Widget) const override;
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;

	// IFigmaFileHandle
	virtual FString GetPackagePath() const override;
	virtual FString GetAssetName() const override;

protected:
	UPROPERTY()
	TMap<FString, FFigmaComponentPropertyDefinition> ComponentPropertyDefinitions;
};
