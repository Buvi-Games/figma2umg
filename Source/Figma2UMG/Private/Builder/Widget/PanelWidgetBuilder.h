// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBuilder.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Interfaces/WidgetOwner.h"
#include "Parser/Nodes/FigmaNode.h"

#include "PanelWidgetBuilder.generated.h"

class UPanelWidget;

UCLASS(Abstract)
class UPanelWidgetBuilder : public UObject, public IWidgetBuilder
{
public:
	GENERATED_BODY()

	void AddChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder);

	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch) override PURE_VIRTUAL(UFigmaNode::GetAbsolutePosition(), return WidgetToPatch;);

protected:
	template<class WidgetType>
	TObjectPtr<WidgetType> Patch(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch);

	UPROPERTY()
	TObjectPtr<UPanelWidget> Widget = nullptr;

	UPROPERTY()
	TArray<TScriptInterface<IWidgetBuilder>> ChildWidgetBuilders;
};


template <class WidgetType>
TObjectPtr<WidgetType> UPanelWidgetBuilder::Patch(TObjectPtr<UWidgetTree> WidgetTree, const TObjectPtr<UWidget>& WidgetToPatch)
{
	TObjectPtr<WidgetType> PatchedWidget = nullptr;
	FString WidgetName = Node->GetUniqueName();

	PatchedWidget = Cast<WidgetType>(WidgetToPatch);
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
			PatchedWidget = WidgetName.IsEmpty() ? NewObject<WidgetType>(WidgetTree) : IWidgetOwner::NewWidget<WidgetType>(WidgetTree, *WidgetName);
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
			WidgetType* NewWidget = IWidgetOwner::NewWidget<WidgetType>(WidgetTree, *WidgetName, ClassOverride);
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