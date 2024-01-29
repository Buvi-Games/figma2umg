// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaDocument.h"

#include "Blueprint/WidgetTree.h"
#include <Components/WidgetSwitcher.h>
#include "WidgetBlueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"

void UFigmaDocument::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	SerializeArray(Children, JsonObj, "Children");
}

void UFigmaDocument::AddToAsset(UWidgetBlueprint* Widget) const
{

	if(Widget->WidgetTree->RootWidget && Cast<UPanelWidget>(Widget->WidgetTree->RootWidget)->GetChildrenCount() != Children.Num())
	{
		if (Children.Num() == 1)
		{
			FString CanvasName = Children[0]->GetUniqueName();
			UWidget** FoundChild = Cast<UPanelWidget>(Widget->WidgetTree->RootWidget)->GetAllChildren().FindByPredicate([CanvasName](const UWidget* Child)
				{
					return Child->GetName() == CanvasName;
				});

			Widget->WidgetTree->RootWidget = Children[0]->AddOrPathToWidget(Widget->WidgetTree, FoundChild  ? *FoundChild : nullptr);
			Children[0]->PostInsert(Widget->WidgetTree->RootWidget);
		}
		else
		{
			UPanelWidget* MainWidget = Widget->WidgetTree->RootWidget ? Cast<UWidgetSwitcher>(Widget->WidgetTree->RootWidget) : nullptr;
			if (MainWidget == nullptr)
			{
				UPanelWidget* OldRootWidget = Cast<UPanelWidget>(Widget->WidgetTree->RootWidget);
				Widget->WidgetTree->RootWidget = MainWidget = NewObject<UWidgetSwitcher>(Widget->WidgetTree, *GetUniqueName());
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
		Widget->WidgetTree->RootWidget = Children[0]->AddOrPathToWidget(Widget->WidgetTree, Widget->WidgetTree->RootWidget);
		Children[0]->PostInsert(Widget->WidgetTree->RootWidget);
	}
	else
	{		
		UPanelWidget* MainWidget = Widget->WidgetTree->RootWidget ? Cast<UWidgetSwitcher>(Widget->WidgetTree->RootWidget) : nullptr;
		if (MainWidget == nullptr)
		{
			Widget->WidgetTree->RootWidget = MainWidget = NewObject<UWidgetSwitcher>(Widget->WidgetTree, *GetUniqueName());
		}
		else if (MainWidget->GetName() != GetUniqueName())
		{
			MainWidget->Rename(*GetUniqueName());
		}

		Widget->WidgetTree->SetFlags(RF_Transactional);
		Widget->WidgetTree->Modify();
		AddOrPathChildren(MainWidget, Children);

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Widget);

	}
}
