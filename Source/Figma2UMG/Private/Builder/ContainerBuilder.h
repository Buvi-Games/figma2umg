// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BorderBuilder.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Properties/FigmaEnums.h"

#include "ContainerBuilder.generated.h"

struct FFigmaPaint;

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
	template<class WidgetType>
	TObjectPtr<WidgetType> Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName);

	UPROPERTY()
	TObjectPtr<UPanelWidget> Canvas = nullptr;

	EFigmaLayoutMode LayoutMode = EFigmaLayoutMode::NONE;
	EFigmaLayoutWrap LayoutWrap = EFigmaLayoutWrap::NO_WRAP;
};

template <class WidgetType>
TObjectPtr<WidgetType> FContainerBuilder::Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName)
{
	TObjectPtr<WidgetType> PatchedWidget = nullptr;
	if (const TObjectPtr<UBorder> BorderWrapper = GetBorder())
	{
		PatchedWidget = Cast<WidgetType>(BorderWrapper->GetContent());
		if (!PatchedWidget)
		{
			PatchedWidget = NewObject<WidgetType>(AssetOuter);
			BorderWrapper->SetContent(PatchedWidget);
		}
	}
	else
	{
		PatchedWidget = Cast<WidgetType>(WidgetToPatch);
		if (!PatchedWidget)
		{
			PatchedWidget = NewObject<WidgetType>(AssetOuter, *WidgetName);
		}
	}

	Canvas = PatchedWidget;
	return PatchedWidget;
}
