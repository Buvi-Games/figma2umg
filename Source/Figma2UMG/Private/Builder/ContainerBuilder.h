// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Properties/FigmaEnums.h"

#include "ContainerBuilder.generated.h"

struct FFigmaPaint;
class UWidget;

USTRUCT()
struct FIGMA2UMG_API FContainerBuilder
{
public:
	GENERATED_BODY()

	virtual void ForEach(const IWidgetOwner::FOnEachFunction& Function);

	virtual TObjectPtr<UWidget> Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName);
	virtual void SetupWidget(TObjectPtr<UWidget> Widget);
	virtual void Reset();

	void SetLayout(EFigmaLayoutMode InLayoutMode, EFigmaLayoutWrap InLayoutWrap);

	virtual TObjectPtr<UWidget> GetTopWidget() const;
	virtual TObjectPtr<UPanelWidget> GetContainerWidget() const;

private:
	template<class WidgetType>
	TObjectPtr<WidgetType> Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName);

	UPROPERTY()
	TObjectPtr<UPanelWidget> Conainter = nullptr;

	EFigmaLayoutMode LayoutMode = EFigmaLayoutMode::NONE;
	EFigmaLayoutWrap LayoutWrap = EFigmaLayoutWrap::NO_WRAP;
};

template <class WidgetType>
TObjectPtr<WidgetType> FContainerBuilder::Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName)
{
	TObjectPtr<WidgetType> PatchedWidget = nullptr;

	PatchedWidget = Cast<WidgetType>(WidgetToPatch);
	if (!PatchedWidget)
	{
		if(const TObjectPtr<UBorder> BorderWrapperOld = Cast<UBorder>(WidgetToPatch))
		{
			PatchedWidget = Cast<WidgetType>(BorderWrapperOld->GetContent());
		}

		if (WidgetToPatch)
		{
			if (WidgetToPatch->GetName() == WidgetName)
			{
				FString OldName = (WidgetName + "_OLD");
				WidgetToPatch->Rename(*OldName);
			}
		}

		if (!PatchedWidget)
		{
			PatchedWidget = WidgetName.IsEmpty() ? NewObject<WidgetType>(AssetOuter) : NewObject<WidgetType>(AssetOuter, *WidgetName);
		}
		else if (PatchedWidget->GetName() != WidgetName)
		{
			PatchedWidget->Rename(*WidgetName);
		}
	}

	Conainter = PatchedWidget;
	return PatchedWidget;
}
