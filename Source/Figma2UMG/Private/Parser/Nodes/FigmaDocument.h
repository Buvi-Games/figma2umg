// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/FigmaContainer.h"
#include "Parser/FigmaFile.h"
#include "Parser/Nodes/FigmaNode.h"

#include "FigmaDocument.generated.h"

class UWidgetBlueprint;

UCLASS()
class UFigmaDocument : public UFigmaNode, public IFigmaFileHandle, public IFigmaContainer
{
public:
	GENERATED_BODY()

	// UFigmaNode
	virtual FVector2D GetAbsolutePosition() const override { return FVector2D::ZeroVector; }
	virtual TObjectPtr<UFigmaFile> GetFigmaFile() const override { return FigmaFile; }
	virtual void PrePatchWidget() override;
	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch) override;

	// IFigmaFileHandle
	virtual FString GetPackagePath() const override;
	virtual FString GetAssetName() const override;

	// IFigmaContainer
	virtual FString GetJsonArrayName() const override { return FString("Children"); };
	virtual TArray<UFigmaNode*>& GetChildren() override { return Children; }

	void SetFigmaFile(UFigmaFile* InFigmaFile);

protected:

	UFigmaFile* FigmaFile = nullptr;

	UPROPERTY()
	TArray<UFigmaNode*> Children;
};
