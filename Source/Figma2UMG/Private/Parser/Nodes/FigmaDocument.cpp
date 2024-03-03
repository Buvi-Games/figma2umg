// Fill out your copyright notice in the Description page of Project Settings.


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
	GetOrCreateAsset<UWidgetBlueprint, UWidgetBlueprintFactory>();
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
	GetOrCreateAsset<UWidgetBlueprint, UWidgetBlueprintFactory>();

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
			UWidget** FoundChild = Cast<UPanelWidget>(WidgetToPatch)->GetAllChildren().FindByPredicate([CanvasName](const UWidget* Child)
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
				WidgetToPatch = MainWidget = NewObject<UWidgetSwitcher>(GetAssetOuter(), *GetUniqueName());
				MainWidget->AddChild(OldRootWidget);
			}
			else if (MainWidget->GetName() != GetUniqueName())
			{
				MainWidget->Rename(*GetUniqueName());
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
			WidgetToPatch = MainWidget = NewObject<UWidgetSwitcher>(GetAssetOuter(), *GetUniqueName());
		}
		else if (MainWidget->GetName() != GetUniqueName())
		{
			MainWidget->Rename(*GetUniqueName());
		}
	}

	Widget->WidgetTree->RootWidget = WidgetToPatch;
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Widget);

	return WidgetToPatch;
}
