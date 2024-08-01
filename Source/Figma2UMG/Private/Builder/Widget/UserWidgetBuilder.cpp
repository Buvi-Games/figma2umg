// Copyright 2024 Buvi Games. All Rights Reserved.


#include "UserWidgetBuilder.h"

#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2_Actions.h"
#include "Figma2UMGModule.h"
#include "FigmaImportSubsystem.h"
#include "K2Node_CallFunction.h"
#include "K2Node_ComponentBoundEvent.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/WidgetBlueprintHelper.h"
#include "Builder/Asset/WidgetBlueprintBuilder.h"
#include "Components/Widget.h"
#include "Interfaces/FlowTransition.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Parser/FigmaFile.h"
#include "Parser/Nodes/FigmaComponent.h"
#include "Parser/Nodes/FigmaComponentSet.h"
#include "Parser/Nodes/FigmaInstance.h"
#include "Parser/Nodes/FigmaNode.h"
#include "REST/FigmaImporter.h"
#include "Templates/WidgetTemplateBlueprintClass.h"

static const FVector2D BaseSize = FVector2D(300.0f, 150.0f);
static const FVector2D Pan = FVector2D(20.0f, 20.0f);

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

	if (FigmaInstance->HasTransition())
	{
		SetupTransition(FigmaInstance);
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

void UUserWidgetBuilder::SetupTransition(const IFlowTransition* FlowTransition) const
{
	UWidgetTree* ParentTree = Widget ? Cast<UWidgetTree>(Widget->GetOuter()) : nullptr;
	TObjectPtr<UWidgetBlueprint> WidgetBlueprint = ParentTree ? Cast<UWidgetBlueprint>(ParentTree->GetOuter()) : nullptr;
	if(!WidgetBlueprint)
		return;
	
	FObjectProperty* VariableProperty = FindFProperty<FObjectProperty>(WidgetBlueprint->SkeletonGeneratedClass, *Widget->GetName());
	const FName EventName("OnButtonClicked");
	const UK2Node_ComponentBoundEvent* OnButtonClickedNode = FKismetEditorUtilities::FindBoundEventForComponent(WidgetBlueprint, EventName, VariableProperty->GetFName());
	if (OnButtonClickedNode == nullptr)
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
				OnButtonClickedNode = EventNode;
			}
		}
	}

	const FString RemoveFromParentFunctionName("RemoveFromParent");
	UK2Node_CallFunction* RemoveFromParentFunction = AddFunctionAfterNode(WidgetBlueprint, OnButtonClickedNode, RemoveFromParentFunctionName);
	if(RemoveFromParentFunction)
	{
		UClass* FoundClass = FindObject<UClass>(nullptr, TEXT("/Script/UMGEditor.K2Node_CreateWidget"), true);

		UEdGraphNode* UK2Node_CreateWidget = AddNodeAfterNode(RemoveFromParentFunction, FoundClass);
		UEdGraphPin* ClassPin = UK2Node_CreateWidget->FindPin(UEdGraphSchema_K2::PC_Class, EGPD_Input);
		if (ClassPin)
		{
			const TObjectPtr<UFigmaFile> File = Node->GetFigmaFile();
			const UFigmaImporter* Importer = File ? File->GetImporter() : nullptr;
			const TObjectPtr<UWidgetBlueprintBuilder> AssetBuilder = Importer ? Importer->FintAssetBuilderForNode<UWidgetBlueprintBuilder>(FlowTransition->GetTransitionNodeID()) : nullptr;
			const TObjectPtr<UWidgetBlueprint> TransitionBP = AssetBuilder ? AssetBuilder->GetAsset() : nullptr;
			if(TransitionBP)
			{
				ClassPin->DefaultObject = TransitionBP->GeneratedClass;
			}
		}
		UEdGraphPin* ReturnValuePin = UK2Node_CreateWidget->FindPin(UEdGraphSchema_K2::PN_ReturnValue, EGPD_Output);

		const FString AddToViewPortFunctionName("AddToViewPort");
		UK2Node_CallFunction* AddToViewPortFunction = AddFunctionAfterNode(WidgetBlueprint, UK2Node_CreateWidget, AddToViewPortFunctionName);
		if (AddToViewPortFunction && ReturnValuePin)
		{
			UEdGraphPin* TargetPin = AddToViewPortFunction->FindPin(UEdGraphSchema_K2::PN_Self, EGPD_Input);
			if(TargetPin)
			{
				ReturnValuePin->MakeLinkTo(TargetPin);
			}
		}
	}

	const FString AddToViewPortFunctionName("AddToViewPort");
};

UK2Node_CallFunction* UUserWidgetBuilder::AddFunctionAfterNode(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint, const UEdGraphNode* PreviousNode, const FString& FunctionName) const
{
	FVector2D NodeLocation(PreviousNode->NodePosX, PreviousNode->NodePosY);
	UK2Node_CallFunction* RemoveFromParentFunction = nullptr;

	UEdGraphPin* ThenPin = PreviousNode->FindPin(UEdGraphSchema_K2::PN_Then);
	while (ThenPin && !ThenPin->LinkedTo.IsEmpty())
	{
		UEdGraphNode* ConnectedNode = ThenPin->LinkedTo[0]->GetOwningNode();
		if (!ConnectedNode)
		{
			break;
		}
		else if (ConnectedNode->IsA<UK2Node_CallFunction>())
		{
			UK2Node_CallFunction* CallFunctionNode = Cast<UK2Node_CallFunction>(ConnectedNode);
			if (CallFunctionNode->GetTargetFunction()->GetName().Equals(FunctionName, ESearchCase::IgnoreCase))
			{
				RemoveFromParentFunction = CallFunctionNode;
				break;
			}
		}

		ThenPin = ConnectedNode->FindPin(UEdGraphSchema_K2::PN_Then);
		NodeLocation = FVector2D(ConnectedNode->NodePosX, ConnectedNode->NodePosY);
	}

	if (RemoveFromParentFunction == nullptr)
	{
		const UFunction* Function = FindUField<UFunction>(WidgetBlueprint->GetClass(), *FunctionName);
		if (!Function && WidgetBlueprint->SkeletonGeneratedClass)
		{
			Function = FindUField<UFunction>(Cast<UClass>(WidgetBlueprint->SkeletonGeneratedClass), *FunctionName);
		}

		if (Function)
		{
			const FVector2D CallFunctionPosition = NodeLocation + FVector2D(BaseSize.X + Pan.X, 0.0f);
			WidgetBlueprintHelper::AddCallFunctionOnMemberNode(PreviousNode->GetGraph(), WidgetBlueprint, Function, ThenPin, nullptr, CallFunctionPosition);
		}
	}

	return RemoveFromParentFunction;
}

UEdGraphNode* UUserWidgetBuilder::AddNodeAfterNode(const UK2Node* PreviousNode, TSubclassOf<UEdGraphNode> const NodeClass) const
{
	FVector2D NodeLocation = FVector2D(PreviousNode->NodePosX + BaseSize.X + Pan.X, PreviousNode->NodePosY);
	UEdGraphPin* ThenPin = PreviousNode->FindPin(UEdGraphSchema_K2::PN_Then);
	UEdGraphNode* NewNode = nullptr;
	while (ThenPin && !ThenPin->LinkedTo.IsEmpty())
	{
		UEdGraphNode* ConnectedNode = ThenPin->LinkedTo[0]->GetOwningNode();
		if (!ConnectedNode)
		{
			break;
		}
		else if (ConnectedNode->IsA(NodeClass))
		{
			NewNode = ConnectedNode;
			break;
		}

		ThenPin = ConnectedNode->FindPin(UEdGraphSchema_K2::PN_Then);
		NodeLocation = FVector2D(ConnectedNode->NodePosX, ConnectedNode->NodePosY);
	}

	if(!NewNode)
	{
		UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(NodeClass, PreviousNode->GetGraph());
		TSet<FBindingObject> Bindings;
		NewNode = Spawner->Invoke(PreviousNode->GetGraph(), Bindings, NodeLocation);

		UEdGraphPin* ExecutePin = NewNode->FindPin(UEdGraphSchema_K2::PN_Execute);
		if (ExecutePin && ThenPin)
		{
			ThenPin->MakeLinkTo(ExecutePin);
		}
	}


	return NewNode;
}
