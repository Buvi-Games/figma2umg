// Copyright 2024 Buvi Games. All Rights Reserved.

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

		if (!PatchedWidget)
		{
			PatchedWidget = WidgetName.IsEmpty() ? NewObject<WidgetType>(AssetOuter) : IWidgetOwner::NewWidget<WidgetType>(AssetOuter, *WidgetName);
		}
		else
		{
			IWidgetOwner::TryRenameWidget(WidgetName, PatchedWidget);
		}
	}
	else
	{
		UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<WidgetType>(WidgetName) : nullptr;
		if (ClassOverride && PatchedWidget->GetClass() != ClassOverride)
		{
			WidgetType* NewWidget = IWidgetOwner::NewWidget<WidgetType>(AssetOuter, *WidgetName, ClassOverride);
			while (PatchedWidget->GetChildrenCount() > 0)
			{
				NewWidget->AddChild(PatchedWidget->GetChildAt(0));
			}
			PatchedWidget = NewWidget;
		}
	}

	Conainter = PatchedWidget;
	return PatchedWidget;
}
