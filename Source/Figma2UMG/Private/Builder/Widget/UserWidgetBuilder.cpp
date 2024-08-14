// Copyright 2024 Buvi Games. All Rights Reserved.


#include "UserWidgetBuilder.h"

#include "BlueprintDelegateNodeSpawner.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2_Actions.h"
#include "Figma2UMGModule.h"
#include "FigmaImportSubsystem.h"
#include "K2Node_CallDelegate.h"
#include "K2Node_CallFunction.h"
#include "K2Node_ComponentBoundEvent.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_VariableGet.h"
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

void UUserWidgetBuilder::PostInsertWidgets(TObjectPtr<UWidgetBlueprint> WidgetBlueprint)
{
	IWidgetBuilder::PostInsertWidgets(WidgetBlueprint);
	
	if (IsInsideComponentPackage(WidgetBlueprint->GetPackage()->GetName()))
	{
		const UWidgetBlueprint* ComponentAsset = WidgetBlueprintBuilder ? WidgetBlueprintBuilder->GetAsset() : nullptr;
		if(!ComponentAsset)
			return;

		bool Modified = false;
		for (TFieldIterator<FMulticastInlineDelegateProperty>It(ComponentAsset->SkeletonGeneratedClass, EFieldIterationFlags::Default); It; ++It)
		{
			FString PropertyName = It->GetFName().ToString();
			if (PropertyName.Equals("OnVisibilityChanged"))
				continue;

			FString EventName = Widget->GetName() + PropertyName;
			SetupEventDispatcher(WidgetBlueprint, *EventName);
			Modified = true;
		}

		if (Modified)
		{
			FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetBlueprint);
		}
	}
}

bool UUserWidgetBuilder::TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget)
{
	UE_LOG_Figma2UMG(Warning, TEXT("[UUserWidgetBuilder::TryInsertOrReplace] Node %s is an UUserWidget and can't insert widgets."), *Node->GetNodeName());
	return false;
}

void UUserWidgetBuilder::PatchWidgetBinds(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint)
{
	if (IsInsideComponentPackage(WidgetBlueprint->GetPackage()->GetName()))
	{
		PatchEvents(WidgetBlueprint);
		PatchButtonsEnabled(WidgetBlueprint);
	}

	IWidgetBuilder::PatchWidgetBinds(WidgetBlueprint);
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
		SetupTransitions(FigmaInstance);
	}

	for (const TPair<FString, FFigmaComponentProperty>& ComponentProperty : FigmaInstance->ComponentProperties)
	{
		WidgetBlueprintHelper::SetPropertyValue(Widget, *ComponentProperty.Key, ComponentProperty.Value);
	}

	PatchInteractiveStateDisabled(FigmaInstance);
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

void UUserWidgetBuilder::SetupTransitions(const IFlowTransition* FlowTransition) const
{
	UWidgetTree* ParentTree = Widget ? Cast<UWidgetTree>(Widget->GetOuter()) : nullptr;
	TObjectPtr<UWidgetBlueprint> WidgetBlueprint = ParentTree ? Cast<UWidgetBlueprint>(ParentTree->GetOuter()) : nullptr;
	if(!WidgetBlueprint)
		return;

	const UWidgetBlueprint* ComponentAsset = WidgetBlueprintBuilder ? WidgetBlueprintBuilder->GetAsset() : nullptr;
	if (!ComponentAsset)
		return;

	FObjectProperty* VariableProperty = FindFProperty<FObjectProperty>(WidgetBlueprint->SkeletonGeneratedClass, *Widget->GetName());
	bool Modified = false;
	for (TFieldIterator<FMulticastInlineDelegateProperty>It(ComponentAsset->SkeletonGeneratedClass, EFieldIterationFlags::Default); It; ++It)
	{
		FString PropertyName = It->GetFName().ToString();
		if (!PropertyName.EndsWith("OnButtonClicked"))
			continue;

		SetupTransition(FlowTransition, WidgetBlueprint, *PropertyName, VariableProperty);
		Modified = true;
	}

	if (Modified)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetBlueprint);
	}
}

void UUserWidgetBuilder::SetupTransition(const IFlowTransition* FlowTransition, TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const FName EventName, FObjectProperty* VariableProperty) const
{
	const FString& TransitionNodeID = FlowTransition->GetTransitionNodeID(EventName);
	if (TransitionNodeID.IsEmpty())
		return;

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
	UK2Node_CallFunction* RemoveFromParentFunction = WidgetBlueprintHelper::AddFunctionAfterNode(WidgetBlueprint, OnButtonClickedNode, RemoveFromParentFunctionName);
	if (RemoveFromParentFunction)
	{
		UClass* FoundClass = FindObject<UClass>(nullptr, TEXT("/Script/UMGEditor.K2Node_CreateWidget"), true);

		UEdGraphNode* UK2Node_CreateWidget = AddNodeAfterNode(RemoveFromParentFunction, FoundClass);
		UEdGraphPin* ClassPin = UK2Node_CreateWidget->FindPin(UEdGraphSchema_K2::PC_Class, EGPD_Input);
		if (ClassPin)
		{
			const TObjectPtr<UFigmaFile> File = Node->GetFigmaFile();
			const UFigmaImporter* Importer = File ? File->GetImporter() : nullptr;
			const TObjectPtr<UWidgetBlueprintBuilder> AssetBuilder = Importer ? Importer->FintAssetBuilderForNode<UWidgetBlueprintBuilder>(TransitionNodeID) : nullptr;
			const TObjectPtr<UWidgetBlueprint> TransitionBP = AssetBuilder ? AssetBuilder->GetAsset() : nullptr;
			if (TransitionBP)
			{
				ClassPin->DefaultObject = TransitionBP->GeneratedClass;
			}
		}
		UEdGraphPin* ReturnValuePin = UK2Node_CreateWidget->FindPin(UEdGraphSchema_K2::PN_ReturnValue, EGPD_Output);

		const FString AddToViewPortFunctionName("AddToViewPort");
		UK2Node_CallFunction* AddToViewPortFunction = WidgetBlueprintHelper::AddFunctionAfterNode(WidgetBlueprint, UK2Node_CreateWidget, AddToViewPortFunctionName);
		if (AddToViewPortFunction && ReturnValuePin)
		{
			UEdGraphPin* TargetPin = AddToViewPortFunction->FindPin(UEdGraphSchema_K2::PN_Self, EGPD_Input);
			if (TargetPin)
			{
				ReturnValuePin->MakeLinkTo(TargetPin);
			}
		}
	}
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
void UUserWidgetBuilder::SetupEventDispatcher(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const FName& EventName) const
{
	if (UObject* ExistingObject = FindObject<UObject>(WidgetBlueprint, *(EventName.ToString())))
	{
		return;
	}

	FEdGraphPinType DelegateType;
	DelegateType.PinCategory = UEdGraphSchema_K2::PC_MCDelegate;
	const bool bVarCreatedSuccess = FBlueprintEditorUtils::AddMemberVariable(WidgetBlueprint, EventName, DelegateType);
	if (!bVarCreatedSuccess)
	{
		//		LogSimpleMessage(LOCTEXT("AddDelegateVariable_Error", "Adding new delegate variable failed."));
		return;
	}

	UEdGraph* const NewGraph = FBlueprintEditorUtils::CreateNewGraph(WidgetBlueprint, EventName, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	if (!NewGraph)
	{
		FBlueprintEditorUtils::RemoveMemberVariable(WidgetBlueprint, EventName);
		//	LogSimpleMessage(LOCTEXT("AddDelegateVariable_Error", "Adding new delegate variable failed."));
		return;
	}

	const int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(WidgetBlueprint, EventName);
	if (VarIndex != INDEX_NONE && WidgetBlueprint->NewVariables[VarIndex].Category.EqualTo(UEdGraphSchema_K2::VR_DefaultCategory))
	{
		static const FText FigmaCategory(FText::FromString("Figma"));
		WidgetBlueprint->NewVariables[VarIndex].Category = FigmaCategory;
	}

	NewGraph->bEditable = false;

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	K2Schema->CreateDefaultNodesForGraph(*NewGraph);
	K2Schema->CreateFunctionGraphTerminators(*NewGraph, (UClass*)nullptr);
	K2Schema->AddExtraFunctionFlags(NewGraph, (FUNC_BlueprintCallable | FUNC_BlueprintEvent | FUNC_Public));
	K2Schema->MarkFunctionEntryAsEditable(NewGraph, true);

	WidgetBlueprint->DelegateSignatureGraphs.Add(NewGraph);
}

void UUserWidgetBuilder::PatchEvents(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint)
{
	FObjectProperty* VariableProperty = FindFProperty<FObjectProperty>(WidgetBlueprint->SkeletonGeneratedClass, *Widget->GetName());

	if (!VariableProperty)
		return;

	const UWidgetBlueprint* ComponentAsset = WidgetBlueprintBuilder ? WidgetBlueprintBuilder->GetAsset() : nullptr;
	if (!ComponentAsset)
		return;

	for (TFieldIterator<FMulticastInlineDelegateProperty>It(ComponentAsset->SkeletonGeneratedClass, EFieldIterationFlags::Default); It; ++It)
	{
		FString EventName = It->GetFName().ToString();
		if (EventName.Equals("OnVisibilityChanged"))
			continue;

		FString EventDispatcherName = Widget->GetName() + EventName;
		PatchEvent(WidgetBlueprint, VariableProperty, *EventName, *EventDispatcherName);
	}
}

void UUserWidgetBuilder::PatchEvent(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint, FObjectProperty* VariableProperty, const FName& EventName, const FName& EventDispatchersName)
{
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

	if(!ExistingNode)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[UUserWidgetBuilder::PatchEvent] Can't create Event %s for Property %s at Bluepriunt %s."), *EventName.ToString(), *VariableProperty->GetName(), *WidgetBlueprint->GetName());
		return;
	}

	FVector2D StartPos(ExistingNode->NodePosX, ExistingNode->NodePosY);
	UEdGraphPin* ThenPin = ExistingNode->FindPin(UEdGraphSchema_K2::PN_Then);
	while (ThenPin && !ThenPin->LinkedTo.IsEmpty())
	{
		UEdGraphNode* ConnectedNode = ThenPin->LinkedTo[0]->GetOwningNode();
		if (!ConnectedNode)
		{
			break;
		}
		else if (ConnectedNode->IsA<UK2Node_CallDelegate>())
		{
			UK2Node_CallDelegate* CallFunctionNode = Cast<UK2Node_CallDelegate>(ConnectedNode);
			if (EventDispatchersName.IsEqual(CallFunctionNode->DelegateReference.GetMemberName()))
			{
				//Nothing to do.
				return;
			}
		}

		ThenPin = ConnectedNode->FindPin(UEdGraphSchema_K2::PN_Then);
		StartPos = FVector2D(ConnectedNode->NodePosX, ConnectedNode->NodePosY);
	}

	FMulticastInlineDelegateProperty* DispatcherProperty = FindFProperty<FMulticastInlineDelegateProperty>(WidgetBlueprint->SkeletonGeneratedClass, EventDispatchersName);
	if (!DispatcherProperty)
	{
		return;
	}

	bool const bIsDelegate = DispatcherProperty->IsA(FMulticastDelegateProperty::StaticClass());
	if (bIsDelegate)
	{
		FMulticastDelegateProperty* DelegateProperty = CastFieldChecked<FMulticastDelegateProperty>(DispatcherProperty);

		UBlueprintNodeSpawner* CallSpawner = UBlueprintDelegateNodeSpawner::Create(UK2Node_CallDelegate::StaticClass(), DelegateProperty);
		TSet<FBindingObject> Bindings;
		Bindings.Add(FBindingObject(WidgetBlueprint));
		const FVector2D Offset = FVector2D(500.0f, 0.0f);
		const FVector2D NodeLocation = StartPos + Offset;
		const UEdGraphNode* GraphNode = CallSpawner->Invoke(ExistingNode->GetGraph(), Bindings, NodeLocation);
		if (GraphNode)
		{
			UEdGraphPin* ExecutePin = GraphNode->FindPin(UEdGraphSchema_K2::PN_Execute);
			if (ThenPin && ExecutePin)
			{
				ThenPin->MakeLinkTo(ExecutePin);
			}
		}
	}
}

void UUserWidgetBuilder::PatchButtonsEnabled(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint)
{
	FObjectProperty* VariableProperty = FindFProperty<FObjectProperty>(WidgetBlueprint->SkeletonGeneratedClass, *Widget->GetName());

	if (!VariableProperty)
		return;

	const UWidgetBlueprint* ComponentAsset = WidgetBlueprintBuilder ? WidgetBlueprintBuilder->GetAsset() : nullptr;
	if (!ComponentAsset)
		return;

	const FString SetEnabledStr("SetEnabled");
	for (TFieldIterator<UFunction>It(ComponentAsset->SkeletonGeneratedClass, EFieldIterationFlags::Default); It; ++It)
	{
		FString FunctionName = It->GetFName().ToString();
		if (!FunctionName.StartsWith(SetEnabledStr))
			continue;

		PatchRelayEnabledFunction(WidgetBlueprint, FunctionName);
	}

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetBlueprint);
}

void UUserWidgetBuilder::PatchRelayEnabledFunction(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint, const FString& FunctionName)
{
	if (!WidgetBlueprint)
		return;

	if (!IsInsideComponentPackage(WidgetBlueprint->GetPackage()->GetName()))
		return;

	if (!Widget)
		return;

	const FString SetEnabledStr("SetEnabled");
	FString RelayFunctionName = SetEnabledStr + Widget->GetName() + FunctionName.Right(FunctionName.Len() - SetEnabledStr.Len());
	TObjectPtr<UEdGraph>* Graph = WidgetBlueprint->FunctionGraphs.FindByPredicate([RelayFunctionName](const TObjectPtr<UEdGraph> Graph)
		{
			return Graph.GetName() == RelayFunctionName;
		});
	UEdGraph* FunctionGraph = Graph ? *Graph : nullptr;
	if (!FunctionGraph)
	{
		FunctionGraph = FBlueprintEditorUtils::CreateNewGraph(WidgetBlueprint, FBlueprintEditorUtils::FindUniqueKismetName(WidgetBlueprint, RelayFunctionName), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());

		FBlueprintEditorUtils::AddFunctionGraph<UClass>(WidgetBlueprint, FunctionGraph, true, nullptr);
	}

	FString EnableStr("Enable");
	const UK2Node_FunctionEntry* FunctionEntry = WidgetBlueprintHelper::PatchFunctionEntry(FunctionGraph, EnableStr, UEdGraphSchema_K2::PC_Boolean, EPinContainerType::None);
	const FVector2D StartPos = FVector2D(FunctionEntry->NodePosX, FunctionEntry->NodePosY);

	const FVector2D GetPosition = StartPos + FVector2D(400.0f, 100.0f);
	UK2Node_VariableGet* ButtonNode = WidgetBlueprintHelper::PatchVariableGetNode(WidgetBlueprint, FunctionGraph, Widget->GetFName(), GetPosition);

	if (ButtonNode)
	{
		UEdGraphPin* ReturnValuePin = ButtonNode->GetValuePin();

		const UFunction* Function = FindUField<UFunction>(Widget->GetClass(), *FunctionName);
		UK2Node_CallFunction* SetIsEnabledFunction = WidgetBlueprintHelper::AddFunctionAfterNode(WidgetBlueprint, FunctionEntry, Function);
		if (SetIsEnabledFunction)
		{
			SetIsEnabledFunction->NodePosY = GetPosition.X + 400.0f;
			SetIsEnabledFunction->NodePosY = StartPos.Y - 10.0f;
			UEdGraphPin* TargetPin = SetIsEnabledFunction->FindPin(UEdGraphSchema_K2::PN_Self, EGPD_Input);
			if (TargetPin && ReturnValuePin)
			{
				ReturnValuePin->MakeLinkTo(TargetPin);
			}

			UEdGraphPin* ThenPin = FunctionEntry->FindPin(UEdGraphSchema_K2::PN_Then, EGPD_Output);
			UEdGraphPin* ExecPin = SetIsEnabledFunction->FindPin(UEdGraphSchema_K2::PN_Execute, EGPD_Input);
			if (ThenPin && ExecPin)
			{
				ThenPin->MakeLinkTo(ExecPin);
			}

			UEdGraphPin* InputValuePin = FunctionEntry->FindPin(EnableStr, EGPD_Output);
			UEdGraphPin* IsEnablePin = SetIsEnabledFunction->FindPin(EnableStr, EGPD_Input);
			if (InputValuePin && IsEnablePin)
			{
				InputValuePin->MakeLinkTo(IsEnablePin);
			}
		}
	}
}

void UUserWidgetBuilder::PatchInteractiveStateDisabled(const UFigmaInstance* FigmaInstance)
{
	if (!FigmaInstance)
		return;

	UWidgetTree* ParentTree = Widget ? Cast<UWidgetTree>(Widget->GetOuter()) : nullptr;
	TObjectPtr<UWidgetBlueprint> WidgetBlueprint = ParentTree ? Cast<UWidgetBlueprint>(ParentTree->GetOuter()) : nullptr;
	if (!WidgetBlueprint)
		return;

	UWidgetBlueprint* ComponentAsset = WidgetBlueprintBuilder ? WidgetBlueprintBuilder->GetAsset() : nullptr;
	if (!ComponentAsset)
		return;

	TObjectPtr<UEdGraph> EventGraph = nullptr;
	TObjectPtr<UK2Node_Event> PreConstructNode = nullptr;
	for (TObjectPtr<UEdGraph> CurrEventGraph : WidgetBlueprint->UbergraphPages)
	{
		TObjectPtr<UEdGraphNode>* FoundNode = CurrEventGraph->Nodes.FindByPredicate([](const TObjectPtr<UEdGraphNode> GraphNode)
			{
				TObjectPtr<UK2Node_Event> EventNode = Cast<UK2Node_Event>(GraphNode);
				return EventNode && EventNode->EventReference.GetMemberName() == "PreConstruct";
			});
		if (FoundNode)
		{
			PreConstructNode = Cast<UK2Node_Event>(*FoundNode);
			EventGraph = CurrEventGraph;
		}
	}

	if (!PreConstructNode)
	{
		return;
	}

	const FString SetEnabledStr("SetEnabled");
	static const FString DisabledStr("Disabled");
	static const FString ComponentPropertiesStr("componentProperties");
	for(const FFigmaOverrides& Override : FigmaInstance->Overrides)
	{
		if (!Override.OverriddenFields.ContainsByPredicate([](const FString& Field) { return Field.Equals(ComponentPropertiesStr, ESearchCase::IgnoreCase);	}))
		{
			continue;
		}

		const UFigmaNode* SubNode = FigmaInstance->FindNodeForOverriden(Override.Id);
		const UFigmaInstance* SubFigmaInstance = Cast<UFigmaInstance>(SubNode);
		if(!SubFigmaInstance)
		{
			continue;
		}

		const FString SetEnabledFunctionStartName = SetEnabledStr + SubFigmaInstance->GetUniqueName(true);
		UFunction* Function = nullptr;
		for (TFieldIterator<UFunction>It(ComponentAsset->SkeletonGeneratedClass, EFieldIterationFlags::Default); It; ++It)
		{
			FString FunctionName = It->GetFName().ToString();
			if (!FunctionName.StartsWith(SetEnabledFunctionStartName))
				continue;

			Function = *It;
			break;
		}

		if(Function)
		{
			for (const TPair<FString, FFigmaComponentProperty>& ComponentProperty : SubFigmaInstance->ComponentProperties)
			{
				if (ComponentProperty.Value.Type == EFigmaComponentPropertyType::VARIANT && ComponentProperty.Value.Value.Equals(DisabledStr))
				{
					UK2Node_CallFunction* SetEnabledButtonFunction = WidgetBlueprintHelper::AddFunctionAfterNode(WidgetBlueprint, PreConstructNode, Function);
					if (SetEnabledButtonFunction)
					{
						const FVector2D GetPosition = FVector2D(SetEnabledButtonFunction->NodePosX - 200.0f, SetEnabledButtonFunction->NodePosY + 200.0f);
						UK2Node_VariableGet* ButtonNode = WidgetBlueprintHelper::PatchVariableGetNode(WidgetBlueprint, EventGraph, Widget->GetFName(), GetPosition);

						UEdGraphPin* ReturnValuePin = ButtonNode ? ButtonNode->GetValuePin() : nullptr;
						UEdGraphPin* TargetPin = SetEnabledButtonFunction->FindPin(UEdGraphSchema_K2::PN_Self, EGPD_Input);
						if (ReturnValuePin && TargetPin)
						{
							ReturnValuePin->MakeLinkTo(TargetPin);
						}
					}
					break;
				}
			}
		}
	}
}
