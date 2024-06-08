// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaDocument.h"

#include "Blueprint/WidgetTree.h"
#include <Components/WidgetSwitcher.h>

#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "Builder/Asset/WidgetBlueprintBuilder.h"
#include "Builder/Widget/PanelWidgetBuilder.h"
#include "Builder/Widget/WidgetSwitcherBuilder.h"
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

void UFigmaDocument::LoadOrCreateAssets()
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
	UE_LOG_Figma2UMG(Display, TEXT("PatchAndInsertWidget [%s]"), *GetUniqueName());

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
			else
			{
				UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
				UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<UWidgetSwitcher>(GetUniqueName()) : nullptr;
				if (ClassOverride && MainWidget->GetClass() != ClassOverride)
				{
					UWidgetSwitcher* NewSwitcher = IWidgetOwner::NewWidget<UWidgetSwitcher>(GetAssetOuter(), *GetUniqueName(), ClassOverride);
					while (MainWidget->GetChildrenCount() > 0)
					{
						NewSwitcher->AddChild(MainWidget->GetChildAt(0));
					}
					WidgetToPatch = MainWidget = NewSwitcher;
				}
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
		else
		{
			UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
			UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<UWidgetSwitcher>(GetUniqueName()) : nullptr;
			if (ClassOverride && MainWidget->GetClass() != ClassOverride)
			{
				UWidgetSwitcher* NewSwitcher = IWidgetOwner::NewWidget<UWidgetSwitcher>(GetAssetOuter(), *GetUniqueName(), ClassOverride);
				while (MainWidget->GetChildrenCount() > 0)
				{
					NewSwitcher->AddChild(MainWidget->GetChildAt(0));
				}
				WidgetToPatch = MainWidget = NewSwitcher;
			}
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

TScriptInterface<IAssetBuilder> UFigmaDocument::CreateAssetBuilder(const FString& InFileKey)
{
	UWidgetBlueprintBuilder* AssetBuilder = NewObject<UWidgetBlueprintBuilder>();
	AssetBuilder->SetNode(InFileKey, this);
	return AssetBuilder;
}

FString UFigmaDocument::GetPackageName() const
{
	return GetPackagePath();
}

FString UFigmaDocument::GetUAssetName() const
{
	return FigmaFile ? FigmaFile->GetFileName() : FString();
}

TScriptInterface<IWidgetBuilder> UFigmaDocument::CreateWidgetBuilders(bool IsRoot /*= false*/) const
{
	if (Children.Num() > 1)
	{
		UWidgetSwitcherBuilder* Builder = NewObject<UWidgetSwitcherBuilder>();
		Builder->SetNode(this);

		for (UFigmaNode* Child : Children)
		{
			if(!Child)
				continue;

			TScriptInterface<IWidgetBuilder> SubBuilder = Child->CreateWidgetBuilders();
			if (SubBuilder)
			{
				Builder->AddChild(SubBuilder);
			}

		}

		return Builder;
	}
	else if (Children.Num() == 1)
	{
		TScriptInterface<IWidgetBuilder> SubBuilder = Children[0] ? Children[0]->CreateWidgetBuilders() : nullptr;
		return SubBuilder;
	}

	return nullptr;
}
