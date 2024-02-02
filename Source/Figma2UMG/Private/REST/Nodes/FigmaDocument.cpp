// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaDocument.h"

#include "Blueprint/WidgetTree.h"
#include <Components/WidgetSwitcher.h>
#include "WidgetBlueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"

FString UFigmaDocument::GetPackagePath() const
{
	return PackagePath;
}

FString UFigmaDocument::GetAssetName() const
{
	return FigmaFileName;
}

void UFigmaDocument::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	SerializeArray(Children, JsonObj, "Children");
}

TObjectPtr<UWidget> UFigmaDocument::AddOrPathToWidgetImp(TObjectPtr<UWidget> WidgetToPatch)
{
	if (WidgetToPatch && Cast<UPanelWidget>(WidgetToPatch)->GetChildrenCount() != Children.Num())
	{
		if (Children.Num() == 1)
		{
			FString CanvasName = Children[0]->GetUniqueName();
			UWidget** FoundChild = Cast<UPanelWidget>(WidgetToPatch)->GetAllChildren().FindByPredicate([CanvasName](const UWidget* Child)
				{
					return Child->GetName() == CanvasName;
				});

			WidgetToPatch = Children[0]->AddOrPathToWidget(FoundChild ? *FoundChild : nullptr);
			Children[0]->PostInsert(WidgetToPatch);
		}
		else
		{
			UPanelWidget* MainWidget = WidgetToPatch ? Cast<UWidgetSwitcher>(WidgetToPatch) : nullptr;
			if (MainWidget == nullptr)
			{
				UPanelWidget* OldRootWidget = Cast<UPanelWidget>(WidgetToPatch);
				WidgetToPatch = MainWidget = NewObject<UWidgetSwitcher>(GetAssetOuter(), *GetUniqueName());
				MainWidget->AddChild(OldRootWidget);
			}
			else if (MainWidget->GetName() != GetUniqueName())
			{
				MainWidget->Rename(*GetUniqueName());
			}

			AddOrPathChildren(MainWidget, Children);
		}
	}
	else if (Children.Num() == 1)
	{
		WidgetToPatch = Children[0]->AddOrPathToWidget(WidgetToPatch);
		Children[0]->PostInsert(WidgetToPatch);
	}
	else
	{
		UPanelWidget* MainWidget = WidgetToPatch ? Cast<UWidgetSwitcher>(WidgetToPatch) : nullptr;
		if (MainWidget == nullptr)
		{
			WidgetToPatch = MainWidget = NewObject<UWidgetSwitcher>(GetAssetOuter(), *GetUniqueName());
		}
		else if (MainWidget->GetName() != GetUniqueName())
		{
			MainWidget->Rename(*GetUniqueName());
		}
		AddOrPathChildren(MainWidget, Children);
	}

	return WidgetToPatch;
}
