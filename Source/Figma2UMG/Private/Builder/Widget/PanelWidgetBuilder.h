// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FigmaImportSubsystem.h"
#include "MultiChildBuilder.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Parser/Nodes/FigmaNode.h"

#include "PanelWidgetBuilder.generated.h"

class UPanelWidget;

UCLASS(Abstract)
class UPanelWidgetBuilder : public UMultiChildBuilder
{
public:
	GENERATED_BODY()

	virtual void PatchAndInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override PURE_VIRTUAL(UPanelWidgetBuilder::PatchAndInsertWidget());

	virtual void SetWidget(const TObjectPtr<UWidget>& InWidget) override;
	virtual void ResetWidget() override;
protected:
	virtual TObjectPtr<UPanelWidget> GetPanelWidget() const override;

	template<class WidgetType>
	TObjectPtr<WidgetType> Patch(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch);
	virtual void Setup() const PURE_VIRTUAL(UPanelWidgetBuilder::Setup());

	UPROPERTY()
	TObjectPtr<UPanelWidget> Widget = nullptr;
};


template <class WidgetType>
TObjectPtr<WidgetType> UPanelWidgetBuilder::Patch(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	TObjectPtr<WidgetType> PatchedWidget = nullptr;
	const FString NodeName = Node->GetNodeName();
	FString WidgetName = Node->GetUniqueName();

	if (const USizeBox* SizeBoxWrapper = Cast<USizeBox>(WidgetToPatch))
	{
		UWidget* SizeBoxContent = SizeBoxWrapper->GetContent();
		if (const UBorder* BorderWrapper = Cast<UBorder>(SizeBoxContent))
		{
			PatchedWidget = Cast<WidgetType>(BorderWrapper->GetContent());
		}
		else
		{
			PatchedWidget = Cast<WidgetType>(SizeBoxWrapper->GetContent());
		}
	}
	else if (const UBorder* BorderWrapper = Cast<UBorder>(WidgetToPatch))
	{
		PatchedWidget = Cast<WidgetType>(BorderWrapper->GetContent());
	}
	else
	{
		PatchedWidget = Cast<WidgetType>(WidgetToPatch);
	}

	if (!PatchedWidget)
	{
		if (const TObjectPtr<UBorder> BorderWrapperOld = Cast<UBorder>(WidgetToPatch))
		{
			PatchedWidget = Cast<WidgetType>(BorderWrapperOld->GetContent());
		}
		if (const TObjectPtr<USizeBox> SizeBoxOld = Cast<USizeBox>(WidgetToPatch))
		{
			PatchedWidget = Cast<WidgetType>(SizeBoxOld->GetContent());
		}

		if (!PatchedWidget)
		{
			PatchedWidget = UFigmaImportSubsystem::NewWidget<WidgetType>(WidgetTree, NodeName, WidgetName);
		}
		else
		{
			UFigmaImportSubsystem::TryRenameWidget(WidgetName, PatchedWidget);
		}
	}
	else
	{
		UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<WidgetType>(NodeName) : nullptr;
		if (ClassOverride && PatchedWidget->GetClass() != ClassOverride)
		{
			WidgetType* NewWidget = UFigmaImportSubsystem::NewWidget<WidgetType>(WidgetTree, NodeName, WidgetName, ClassOverride);
			while (PatchedWidget->GetChildrenCount() > 0)
			{
				NewWidget->AddChild(PatchedWidget->GetChildAt(0));
			}
			PatchedWidget = NewWidget;
		}
	}

	Widget = PatchedWidget;
	return PatchedWidget;
}