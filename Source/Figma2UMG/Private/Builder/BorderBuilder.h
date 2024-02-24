// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Properties/FigmaEnums.h"

#include "BorderBuilder.generated.h"

struct FFigmaPaint;
class UBorder;

USTRUCT()
struct FIGMA2UMG_API FBorderBuilder
{
public:
	GENERATED_BODY()
	virtual ~FBorderBuilder() = default;

	virtual void ForEach(const IWidgetOwner::FOnEachFunction& Function);

	virtual TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName);
	virtual void Reset();

	void SetupBorder(const TArray<FFigmaPaint>& Fills, const TArray<FFigmaPaint>& Strokes, float InStrokeWeight, const EFigmaStrokeAlign& InStrokeAlign, const FVector4& InCornerRadii, const float InCornerSmoothing);

	virtual TObjectPtr<UWidget> GetTopWidget() const;
	virtual TObjectPtr<UPanelWidget> GetContainerWidget() const;

	TObjectPtr<UBorder> GetBorder() const { return Border; }
private:
	void SetFill() const;
	void SetStroke() const;
	void SetCorner() const;

	UPROPERTY()
	TObjectPtr<UBorder> Border = nullptr;

	const FFigmaPaint* Fill = nullptr;
	const FFigmaPaint* Stroke = nullptr;
	float StrokeWeight = 1.0f;
	EFigmaStrokeAlign StrokeAlign = EFigmaStrokeAlign::OUTSIDE;
	FVector4 CornerRadii = FVector4::Zero();
	float CornerSmoothing = 0.0f;
};
