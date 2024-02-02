// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/BordedCanvasContent.h"
#include "REST/Nodes/FigmaNode.h"
#include "REST/Properties/FigmaPaint.h"
#include "REST/Properties/FigmaRectangle.h"

#include "FigmaSection.generated.h"

UCLASS()
class UFigmaSection : public  UFigmaNode, public IBordedCanvasContent
{
public:
	GENERATED_BODY()

	// IBordedCanvasContent
	virtual FVector2D GetPosition() const override;
	virtual FVector2D GetSize() const override;

	// UFigmaNode
	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;

	virtual void PostInsert(UWidget* Widget) const override;

	virtual FVector2D GetAbsolutePosition() const override;

	virtual FString GetCurrentPackagePath() const override;
protected:
	virtual TObjectPtr<UWidget> AddOrPathToWidgetImp(TObjectPtr<UWidget> WidgetToPatch) override;

	UPROPERTY()
	bool SectionContentsHidden = false;

	UPROPERTY()
	FString DevStatus;

	UPROPERTY()
	TArray<FFigmaPaint> Fills;

	UPROPERTY()
	TArray<FFigmaPaint> Strokes;

	UPROPERTY()
	float StrokeWeight;

	UPROPERTY()
	FString StrokeAlign;

	UPROPERTY()
	TArray<UFigmaNode*> Children;

	UPROPERTY()
	FFigmaRectangle AbsoluteBoundingBox;

	UPROPERTY()
	FFigmaRectangle AbsoluteRenderBounds;
};
