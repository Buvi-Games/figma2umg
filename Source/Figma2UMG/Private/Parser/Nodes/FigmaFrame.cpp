// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaFrame.h"

#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/Asset/WidgetBlueprintBuilder.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Templates/WidgetTemplateBlueprintClass.h"

void UFigmaFrame::SetGenerateFile()
{
	GenerateFile = true;
}

TObjectPtr<UWidget> UFigmaFrame::PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch)
{
	if(!GenerateFile)
	{
		return Super::PatchPreInsertWidget(WidgetToPatch);
	}

	UE_LOG_Figma2UMG(Display, TEXT("PatchPreInsertWidget [%s]"), *GetUniqueName());

	UWidgetBlueprint* Widget = GetAsset<UWidgetBlueprint>();
	WidgetToPatch = Widget->WidgetTree->RootWidget;
	Widget->WidgetTree->RootWidget = Patch(WidgetToPatch);

	Widget->WidgetTree->SetFlags(RF_Transactional);
	Widget->WidgetTree->Modify();

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Widget);

	TObjectPtr<UWidget> WidgetInstance = nullptr;
	if (ParentNode)
	{
		UE_LOG_Figma2UMG(Display, TEXT("Adding in-place Instance for Component %s. This should be a template."), *GetUniqueName());

		InstanceAsset = WidgetInstance = CreateInstance(ParentNode->GetAssetOuter());
	}

	TObjectPtr<UPanelWidget> PanelWidget = GetContainerWidget();
	FString NodeName = GetNodeName();
	IFigmaContainer::ForEach(IFigmaContainer::FOnEachFunction::CreateLambda([NodeName, PanelWidget](UFigmaNode& ChildNode, const int Index)
		{
			TObjectPtr<UWidget> OldWidget = ChildNode.FindWidgetForNode(PanelWidget);
			TObjectPtr<UWidget> NewWidget = ChildNode.PatchPreInsertWidget(OldWidget);
			if (NewWidget)
			{
				if (NewWidget != OldWidget)
				{
					PanelWidget->SetFlags(RF_Transactional);
					PanelWidget->Modify();

					UE_LOG_Figma2UMG(Display, TEXT("[Widget Insert] Parent [%s] Child [%s]."), *NodeName, *ChildNode.GetNodeName());
					if (Index < PanelWidget->GetChildrenCount())
					{
						PanelWidget->ReplaceChildAt(Index, NewWidget);
					}
					else
					{
						PanelWidget->AddChild(NewWidget);
					}
				}
			}
		}));

	if (PanelWidget)
	{
		FixSpacers(PanelWidget);
	}

	return WidgetInstance;
}

void UFigmaFrame::SetWidget(TObjectPtr<UWidget> Widget)
{
	if (!GenerateFile)
	{
		Super::SetWidget(Widget);
		return;
	}

	if (Widget)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[SetWidget] %s %s received a UWidget %s of type %s."), *GetClass()->GetName(), *GetNodeName(), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
	}

	UWidgetBlueprint* WidgetBP = GetAsset<UWidgetBlueprint>();
	Super::SetWidget(WidgetBP->WidgetTree->RootWidget);

	if (ParentNode)
	{
		InstanceAsset = Widget;
	}

	TObjectPtr<UPanelWidget> PanelWidget = GetContainerWidget();
	IFigmaContainer::ForEach(IFigmaContainer::FOnEachFunction::CreateLambda([PanelWidget](UFigmaNode& ChildNode, const int Index)
		{
			TObjectPtr<UWidget> Widget = ChildNode.FindWidgetForNode(PanelWidget);
			ChildNode.SetWidget(Widget);
		}));
}

IAssetBuilder* UFigmaFrame::CreateAssetBuilder(const FString& InFileKey)
{
	if (GenerateFile)
	{
		UWidgetBlueprintBuilder* AssetBuilder = NewObject<UWidgetBlueprintBuilder>();
		AssetBuilder->SetNode(InFileKey, this);
		return AssetBuilder;
	}

	return nullptr;
}

FString UFigmaFrame::GetPackageName() const
{
	return GetPackagePath();
}

FString UFigmaFrame::GetPackagePath() const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "Menu";
}

FString UFigmaFrame::GetAssetName() const
{
	return GetUniqueName();
}

void UFigmaFrame::LoadOrCreateAssets()
{
	if (GenerateFile)
	{
		GetOrCreateWidgetBlueprint();
	}
}

void UFigmaFrame::LoadAssets()
{
	if (GenerateFile)
	{
		LoadAsset<UWidgetBlueprint>();
	}
}

void UFigmaFrame::PostInsert() const
{
	Super::PostInsert();

	if (GenerateFile)
	{
		if (TObjectPtr<UWidget> WidgetInstance = Cast<UWidget>(InstanceAsset))
		{
			SetPosition(WidgetInstance, GetPosition());
			SetSize(WidgetInstance, AbsoluteBoundingBox.GetSize(), true);
		}
	}
}

void UFigmaFrame::PatchBluePrintBinds() const
{
	if (!GenerateFile)
		return;

	TObjectPtr<UWidgetBlueprint> WidgetBp = GetAsset<UWidgetBlueprint>();
	if (!WidgetBp)
		return;

	PatchBinds(WidgetBp);
}

UObject* UFigmaFrame::GetAssetOuter() const
{
	if (GenerateFile)
	{
		return Super::GetAssetOuter();
	}
	else if (ParentNode)
	{
		return ParentNode->GetAssetOuter();
	}

	return nullptr;
}

UWidget* UFigmaFrame::CreateInstance(UObject* InAssetOuter) const
{
	if (!GenerateFile)
	{
		return nullptr;
	}

	TObjectPtr<UWidgetTree> OwningObject = Cast<UWidgetTree>(InAssetOuter);
	if (!OwningObject)
	{
		if (InAssetOuter)
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[CreateInstance] AssetOuter %s is of type %s but we requite a UWidgetTree"), *InAssetOuter->GetName(), *InAssetOuter->GetClass()->GetDisplayNameText().ToString());
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[CreateInstance] AssetOuter is nullptr"));
		}

		return nullptr;
	}
	UWidgetBlueprint* Widget = GetAsset<UWidgetBlueprint>();
	if (!Widget)
		return nullptr;

	TSubclassOf<UUserWidget> UserWidgetClass = Widget->GetBlueprintClass();

	TSharedPtr<FWidgetTemplateBlueprintClass> Template = MakeShared<FWidgetTemplateBlueprintClass>(FAssetData(Widget), UserWidgetClass);
	UWidget* NewWidget = Template->Create(OwningObject);

	if (NewWidget)
	{
		NewWidget->CreatedFromPalette();
	}

	TryRenameWidget(GetUniqueName(), NewWidget);
	return NewWidget;
}
