// Copyright 2024 Buvi Games. All Rights Reserved.


#include "UserWidgetBuilder.h"

#include "EdGraphSchema_K2_Actions.h"
#include "Figma2UMGModule.h"
#include "FigmaImportSubsystem.h"
#include "K2Node_ComponentBoundEvent.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/WidgetBlueprintHelper.h"
#include "Builder/Asset/WidgetBlueprintBuilder.h"
#include "Components/Widget.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Parser/Nodes/FigmaComponent.h"
#include "Parser/Nodes/FigmaComponentSet.h"
#include "Parser/Nodes/FigmaInstance.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Templates/WidgetTemplateBlueprintClass.h"


void UUserWidgetBuilder::SetWidgetBlueprintBuilder(const TObjectPtr<UWidgetBlueprintBuilder>& InWidgetBlueprintBuilder)
{
	WidgetBlueprintBuilder = InWidgetBlueprintBuilder;
	if (!InWidgetBlueprintBuilder)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[UUserWidgetBuilder::SetWidgetBlueprintBuilder] Node %s is receiving <null> WidgetBlueprintBuilder."), *Node->GetNodeName());
	}
}

void UUserWidgetBuilder::PatchAndInsertWidget(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UWidget>& WidgetToPatch)
{
	Widget = Cast<UUserWidget>(WidgetToPatch);

	const FString NodeName = Node->GetNodeName();
	const FString WidgetName = Node->GetUniqueName();
	if (UWidgetBlueprint* ComponentAsset = WidgetBlueprintBuilder ? WidgetBlueprintBuilder->GetAsset() : nullptr)
	{
		if (Widget && Widget.GetClass()->ClassGeneratedBy == ComponentAsset)
		{
			UFigmaImportSubsystem::TryRenameWidget(WidgetName, WidgetToPatch);
			Widget->bIsVariable = true;
		}
		else
		{
			TSubclassOf<UUserWidget> UserWidgetClass = ComponentAsset->GetBlueprintClass();
			TSharedPtr<FWidgetTemplateBlueprintClass> Template = MakeShared<FWidgetTemplateBlueprintClass>(FAssetData(ComponentAsset), UserWidgetClass);
			Widget = Cast<UUserWidget>(Template->Create(WidgetBlueprint->WidgetTree));
			if (Widget)
			{
				UFigmaImportSubsystem::TryRenameWidget(WidgetName, Widget);
				Widget->CreatedFromPalette();
				Widget->bIsVariable = true;
			}
		}
	}

	Insert(WidgetBlueprint->WidgetTree, WidgetToPatch, Widget);
}

bool UUserWidgetBuilder::TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget)
{
	UE_LOG_Figma2UMG(Warning, TEXT("[UUserWidgetBuilder::TryInsertOrReplace] Node %s is an UUserWidget and can't insert widgets."), *Node->GetNodeName());
	return false;
}

void UUserWidgetBuilder::PatchWidgetProperties()
{
	const UFigmaInstance* FigmaInstance = Cast<UFigmaInstance>(Node);
	if (!FigmaInstance)
	{
		if (!Node->IsA<UFigmaComponent>() && !Node->IsA<UFigmaComponentSet>())
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[UUserWidgetBuilder::PatchWidgetProperties] Node %s is of type %s but UFigmaInstance was expected."), *Node->GetNodeName(), *Node->GetClass()->GetName());
		}
		return;
	}

	if (Widget == nullptr)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[UUserWidgetBuilder::PatchWidgetProperties] Missing Widget for node %s."), *Node->GetNodeName());
		return;
	}

	if(FigmaInstance->HasTransition())
	{
		SetupTransition();
	}

	for (const TPair<FString, FFigmaComponentProperty>& ComponentProperty : FigmaInstance->ComponentProperties)
	{
		WidgetBlueprintHelper::SetPropertyValue(Widget, *ComponentProperty.Key, ComponentProperty.Value);
	}
}

void UUserWidgetBuilder::SetWidget(const TObjectPtr<UWidget>& InWidget)
{
	Widget = Cast<UUserWidget>(InWidget);
}

TObjectPtr<UWidget> UUserWidgetBuilder::GetWidget() const
{
	return Widget;
}

void UUserWidgetBuilder::ResetWidget()
{
	Widget = nullptr;
}

void UUserWidgetBuilder::GetPaddingValue(FMargin& Padding) const
{
	Padding.Left = 0.0f;
	Padding.Right = 0.0f;
	Padding.Top = 0.0f;
	Padding.Bottom = 0.0f;
}

bool UUserWidgetBuilder::GetAlignmentValues(EHorizontalAlignment& HorizontalAlignment, EVerticalAlignment& VerticalAlignment) const
{
	HorizontalAlignment = HAlign_Fill;
	VerticalAlignment = VAlign_Fill;
	return true;
}

void UUserWidgetBuilder::SetupTransition() const
{
	UWidgetTree* ParentTree = Widget ? Cast<UWidgetTree>(Widget->GetOuter()) : nullptr;
	TObjectPtr<UWidgetBlueprint> WidgetBlueprint = ParentTree ? Cast<UWidgetBlueprint>(ParentTree->GetOuter()) : nullptr;
	if(!WidgetBlueprint)
		return;
	
	FObjectProperty* VariableProperty = FindFProperty<FObjectProperty>(WidgetBlueprint->SkeletonGeneratedClass, *Widget->GetName());
	const FName EventName("OnButtonClicked");
	const UK2Node_ComponentBoundEvent* ExistingNode = FKismetEditorUtilities::FindBoundEventForComponent(WidgetBlueprint, EventName, VariableProperty->GetFName());
	if (ExistingNode == nullptr)
	{
		FMulticastDelegateProperty* DelegateProperty = FindFProperty<FMulticastDelegateProperty>(Widget->GetClass(), EventName);
		if (DelegateProperty != nullptr)
		{
			UEdGraph* TargetGraph = WidgetBlueprint->GetLastEditedUberGraph();
			if (TargetGraph != nullptr)
			{
				const FVector2D NewNodePos = TargetGraph->GetGoodPlaceForNewNode();
				UK2Node_ComponentBoundEvent* EventNode = FEdGraphSchemaAction_K2NewNode::SpawnNode<UK2Node_ComponentBoundEvent>(TargetGraph, NewNodePos, EK2NewNodeFlags::SelectNewNode);
				EventNode->InitializeComponentBoundEventParams(VariableProperty, DelegateProperty);
				ExistingNode = EventNode;
			}
		}
	}
};
