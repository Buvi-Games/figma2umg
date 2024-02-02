// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "REST/FigmaFile.h"
#include "REST/Nodes/FigmaNode.h"

#include "FigmaDocument.generated.h"

class UWidgetBlueprint;

UCLASS()
class UFigmaDocument : public UFigmaNode, public IFigmaFileHandle
{
public:
	GENERATED_BODY()

	virtual FString GetPackagePath() const override;
	virtual FString GetAssetName() const override;

	void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj);

	virtual FVector2D GetAbsolutePosition() const override { return FVector2D(); }

	void SetFileName(FString InFigmaFileName) { FigmaFileName = InFigmaFileName; }
protected:
	virtual TObjectPtr<UWidget> AddOrPathToWidgetImp(TObjectPtr<UWidget> WidgetToPatch) override;

	FString FigmaFileName;

	UPROPERTY()
	TArray<UFigmaNode*> Children;
};
