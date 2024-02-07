// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaComponent.h"

#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "REST/FigmaFile.h"
#include "REST/Properties/FigmaComponentRef.h"
#include "Templates/WidgetTemplateBlueprintClass.h"

FString UFigmaComponent::GetPackagePath() const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "COMPONENTS";
}

FString UFigmaComponent::GetAssetName() const
{
	return GetUniqueName();
}


void UFigmaComponent::PrePatchWidget()
{
	Super::PrePatchWidget();

	UWidgetBlueprint* WidgetBB = GetOrCreateAsset<UWidgetBlueprint>();
	RefAsset = WidgetBB;

	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	FFigmaComponentRef* ComponentRef = FigmaFile ? FigmaFile->FindComponentRef(GetId()) : nullptr;
	if(ComponentRef)
	{
		ComponentRef->SetAsset(GetAsset<UWidgetBlueprint>());
	}
	
}

TObjectPtr<UWidget> UFigmaComponent::PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch)
{
	UWidgetBlueprint* Widget = GetAsset<UWidgetBlueprint>();
	WidgetToPatch = Widget->WidgetTree->RootWidget;
	Widget->WidgetTree->RootWidget = Patch(WidgetToPatch);

	Widget->WidgetTree->SetFlags(RF_Transactional);
	Widget->WidgetTree->Modify();

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Widget);

	TObjectPtr<UWidget> WidgetInstance = nullptr;
	if (ParentNode)
	{
		TObjectPtr<UWidgetTree> OwningObject = Cast<UWidgetTree>(ParentNode->GetAssetOuter());
		TSubclassOf<UUserWidget> UserWidgetClass = Widget->GetBlueprintClass();

		TSharedPtr<FWidgetTemplateBlueprintClass> Template = MakeShared<FWidgetTemplateBlueprintClass>(FAssetData(Widget), UserWidgetClass);
		UWidget* NewWidget = Template->Create(OwningObject);

		if (NewWidget)
		{
			NewWidget->CreatedFromPalette();
		}

		WidgetInstance = NewWidget;
		InstanceAsset = WidgetInstance;
	}

	TObjectPtr<UPanelWidget> PanelWidget = GetContainerWidget();
	IFigmaContainer::ForEach(IFigmaContainer::FOnEachFunction::CreateLambda([PanelWidget](UFigmaNode& ChildNode, const int Index)
		{
			TObjectPtr<UWidget> OldWidget = PanelWidget->GetChildAt(Index);
			TObjectPtr<UWidget> NewWidget = ChildNode.PatchPreInsertWidget(OldWidget);
			if (NewWidget)
			{
				if (NewWidget != OldWidget)
				{
					PanelWidget->SetFlags(RF_Transactional);
					PanelWidget->Modify();

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

	return WidgetInstance;
}

void UFigmaComponent::PostInsert() const
{
	Super::PostInsert();

	if (TObjectPtr<UWidget> WidgetInstance = Cast<UWidget>(InstanceAsset))
	{
		UCanvasPanelSlot* CanvasSlot = WidgetInstance->Slot ? Cast<UCanvasPanelSlot>(WidgetInstance->Slot) : nullptr;
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(GetPosition());
			CanvasSlot->SetSize(AbsoluteBoundingBox.GetSize());
		}
	}
}

void UFigmaComponent::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	FFigmaComponentRef* ComponentRef = FigmaFile->FindComponentRef(GetId());
	ComponentRef->SetComponent(this);
}

