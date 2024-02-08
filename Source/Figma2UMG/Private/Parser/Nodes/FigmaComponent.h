// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FigmaFrame.h"
#include "Interfaces/AssetFileHandler.h"
#include "Interfaces/FigmaRefHandle.h"
#include "Parser/Properties/FigmaComponentPropertyDefinition.h"

#include "FigmaComponent.generated.h"

UCLASS()
class UFigmaComponent : public  UFigmaFrame, public IFigmaFileHandle, public IFigmaRefHandle
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual void PrePatchWidget() override;
	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch) override;
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;

	// IFigmaFileHandle
	virtual FString GetPackagePath() const override;
	virtual FString GetAssetName() const override;

	// IWidgetOwner
	virtual void PostInsert() const override;

protected:
	UPROPERTY()
	TMap<FString, FFigmaComponentPropertyDefinition> ComponentPropertyDefinitions;
};
