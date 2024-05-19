// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaDocument.h"

#include "Blueprint/WidgetTree.h"
#include <Components/WidgetSwitcher.h>

#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Parser/FigmaFile.h"

FString UFigmaDocument::GetPackagePath() const
{
	return PackagePath;
}

FString UFigmaDocument::GetAssetName() const
{
	return FigmaFile ? FigmaFile->GetFileName() : FString();
}

void UFigmaDocument::LoadOrCreateAssets(UFigmaFile* InFigmaFile)
{
	GetOrCreateWidgetBlueprint();
}

void UFigmaDocument::LoadAssets()
{
	LoadAsset<UWidgetBlueprint>();
}

void UFigmaDocument::SetFigmaFile(UFigmaFile* InFigmaFile)
{
	FigmaFile = InFigmaFile;
	SetCurrentPackagePath(FigmaFile->GetPackagePath());
}

void UFigmaDocument::PrePatchWidget()
{
	GetOrCreateWidgetBlueprint();

	Super::PrePatchWidget();
}

TObjectPtr<UWidget> UFigmaDocument::PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch)
{
	UE_LOG_Figma2UMG(Display, TEXT("PatchPreInsertWidget [%s]"), *GetUniqueName());

	UWidgetBlueprint* Widget = GetAsset<UWidgetBlueprint>();

	WidgetToPatch = Widget->WidgetTree->RootWidget;
	if (WidgetToPatch && Cast<UPanelWidget>(WidgetToPatch)->GetChildrenCount() != Children.Num())
	{
		UE_LOG_Figma2UMG(Warning, TEXT("Mismatching of document pages from previous import. Importing %i and previously was %i"), Children.Num(), Cast<UPanelWidget>(WidgetToPatch)->GetChildrenCount());
		if (Children.Num() == 1)
		{
			FString CanvasName = Children[0]->GetUniqueName();
			TArray<UWidget*> AllChildren = Cast<UPanelWidget>(WidgetToPatch)->GetAllChildren();
			UWidget** FoundChild = AllChildren.FindByPredicate([CanvasName](const UWidget* Child)
				{
					return Child->GetName() == CanvasName;
				});

			WidgetToPatch = Children[0]->PatchPreInsertWidget(FoundChild ? *FoundChild : nullptr);
		}
		else
		{
			UPanelWidget* MainWidget = WidgetToPatch ? Cast<UWidgetSwitcher>(WidgetToPatch) : nullptr;
			if (MainWidget == nullptr)
			{
				UPanelWidget* OldRootWidget = Cast<UPanelWidget>(WidgetToPatch);
				WidgetToPatch = MainWidget = IWidgetOwner::NewWidget<UWidgetSwitcher>(GetAssetOuter(), *GetUniqueName());
				MainWidget->AddChild(OldRootWidget);
			}
			else if (MainWidget->GetName() != GetUniqueName())
			{
				IWidgetOwner::TryRenameWidget(GetUniqueName(), MainWidget);
			}

			for (UFigmaNode* Child : Children)
			{
				FString ChildName = Child->GetUniqueName();
				TArray<UWidget*> AllChildren = MainWidget->GetAllChildren();
				UWidget** FoundChild = AllChildren.FindByPredicate([ChildName](const UWidget* Child)
					{
						return Child->GetName().Contains(ChildName);
					});

				TObjectPtr<UWidget> SubWidget = Child->PatchPreInsertWidget(FoundChild ? *FoundChild : nullptr);
				if(!FoundChild && SubWidget)
				{
					MainWidget->AddChild(SubWidget);
				}
			}
		}
	}
	else if (Children.Num() == 1)
	{
		WidgetToPatch = Children[0]->PatchPreInsertWidget(WidgetToPatch);
	}
	else
	{
		UPanelWidget* MainWidget = WidgetToPatch ? Cast<UWidgetSwitcher>(WidgetToPatch) : nullptr;
		if (MainWidget == nullptr)
		{
			WidgetToPatch = MainWidget = IWidgetOwner::NewWidget<UWidgetSwitcher>(GetAssetOuter(), *GetUniqueName());
		}
		else if (MainWidget->GetName() != GetUniqueName())
		{
			IWidgetOwner::TryRenameWidget(GetUniqueName(), MainWidget);
		}

		for (UFigmaNode* Child : Children)
		{
			FString ChildName = Child->GetUniqueName();
			TArray<UWidget*> AllChildren = MainWidget->GetAllChildren();
			UWidget** FoundChild = AllChildren.FindByPredicate([ChildName](const UWidget* Child)
				{
					return Child->GetName().Contains(ChildName);
				});

			TObjectPtr<UWidget> SubWidget = Child->PatchPreInsertWidget(FoundChild ? *FoundChild : nullptr);
			if (!FoundChild && SubWidget)
			{
				MainWidget->AddChild(SubWidget);
			}
		}
	}

	Widget->WidgetTree->RootWidget = WidgetToPatch;
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Widget);

	return WidgetToPatch;
}

void UFigmaDocument::SetWidget(TObjectPtr<UWidget> Widget)
{
	UWidgetBlueprint* WidgetBP = GetAsset<UWidgetBlueprint>();
	if (Children.Num() == 1)
	{
		Children[0]->SetWidget(WidgetBP->WidgetTree->RootWidget);
	}
	else
	{
		Super::SetWidget(WidgetBP->WidgetTree->RootWidget);
	}
}
