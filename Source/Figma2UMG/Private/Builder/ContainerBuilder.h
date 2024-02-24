// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BorderBuilder.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Properties/FigmaEnums.h"

#include "ContainerBuilder.generated.h"

struct FFigmaPaint;
class UCanvasPanel;
class UBorder;

USTRUCT()
struct FIGMA2UMG_API FContainerBuilder : public FBorderBuilder
{
public:
	GENERATED_BODY()

	virtual void ForEach(const IWidgetOwner::FOnEachFunction& Function) override;

	virtual TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName) override;
	virtual void Reset() override;

	void SetLayout(EFigmaLayoutMode InLayoutMode, EFigmaLayoutWrap InLayoutWrap);

	virtual TObjectPtr<UWidget> GetTopWidget() const override;
	virtual TObjectPtr<UPanelWidget> GetContainerWidget() const override;
private:
	UPROPERTY()
	TObjectPtr<UCanvasPanel> Canvas = nullptr;

	EFigmaLayoutMode LayoutMode = EFigmaLayoutMode::NONE;
	EFigmaLayoutWrap LayoutWrap = EFigmaLayoutWrap::NO_WRAP;
};
