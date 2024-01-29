// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Overlay.h"
#include "REST/Nodes/FigmaNode.h"
#include "REST/Properties/FigmaPaint.h"
#include "REST/Properties/FigmaRectangle.h"

#include "FigmaSection.generated.h"

UCLASS()
class UFigmaSection : public  UFigmaNode
{
public:
	GENERATED_BODY()

	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj) override;

	virtual TObjectPtr<UWidget> AddOrPathToWidget(TObjectPtr<UWidgetTree> Outer, TObjectPtr<UWidget> WidgetToPatch) const override;
	virtual void PostInsert(UWidget* Widget) const;

	virtual FVector2D GetAbsolutePosition() const override;
protected:

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
