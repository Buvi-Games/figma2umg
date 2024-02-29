// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/WidgetBlueprintBuilder.h"

#include "K2Node_VariableGet.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_SwitchString.h"
#include "WidgetBlueprint.h"
#include "Blueprint/UserWidget.h"
#include "Kismet2/BlueprintEditorUtils.h"

static const FVector2D BaseSize = FVector2D(300.0f, 150.0f);
static const FVector2D Pan = FVector2D(20.0f, 20.0f);

void WidgetBlueprintBuilder::PatchVisibilityBind(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, const FBPVariableDescription& VariableDescription, const FName& VariableName)
{
	FString FunctionName = "Get_" + Widget.GetName() + "_Visibility";
	TObjectPtr<UEdGraph>* Graph = WidgetBP->FunctionGraphs.FindByPredicate([FunctionName](const TObjectPtr<UEdGraph> Graph)
		{
			return Graph.GetName() == FunctionName;
		});

	UEdGraph* FunctionGraph = Graph ? *Graph : nullptr;

	if (!FunctionGraph)
	{
		FunctionGraph = FBlueprintEditorUtils::CreateNewGraph(WidgetBP, FBlueprintEditorUtils::FindUniqueKismetName(WidgetBP, FunctionName), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());

		UFunction* BindableSignature = Widget->GetClass()->FindFunctionByName("GetSlateVisibility__DelegateSignature");
		FBlueprintEditorUtils::AddFunctionGraph(WidgetBP, FunctionGraph, true, BindableSignature);
	}

	AddBindingFunction(WidgetBP, Widget, FunctionGraph, "Visibility");

	TObjectPtr<class UEdGraphNode>* FoundEntryNode = FunctionGraph->Nodes.FindByPredicate([](const TObjectPtr<class UEdGraphNode> Node)
		{
			return Node && Node->IsA<UK2Node_FunctionEntry>();
		});

	TObjectPtr<UK2Node_FunctionEntry> FunctionEntry = FoundEntryNode ? Cast<UK2Node_FunctionEntry>(*FoundEntryNode) : nullptr;

	const FVector2D StartPos = FunctionEntry ? FVector2D(FunctionEntry->NodePosX, FunctionEntry->NodePosY) : FVector2D(0.0f, 0.0f);

	const FVector2D GetGraphPosition = StartPos + FVector2D(0.0f, BaseSize.Y + Pan.Y);
	const UK2Node_VariableGet* VariableGetNode = PatchVariableGetNode(WidgetBP, FunctionGraph, VariableName, GetGraphPosition);

	const FVector2D VisibleResultPosition = StartPos + FVector2D((BaseSize.X + Pan.X) * 2.0f, StartPos.Y);
	const UK2Node_FunctionResult* VisibleResult = PatchFunctionResult(FunctionGraph, VisibleResultPosition, "Visible");

	const FVector2D CollapsedResultPosition = VisibleResultPosition + FVector2D(0, BaseSize.Y + Pan.Y);
	const UK2Node_FunctionResult* CollapsedResult = PatchFunctionResult(FunctionGraph, CollapsedResultPosition, "Collapsed");

	const FVector2D IfThenElseGraphPosition = StartPos + FVector2D(BaseSize.X + Pan.X, StartPos.Y);
	PatchIfThenElseNode(FunctionGraph, IfThenElseGraphPosition, FunctionEntry ? FunctionEntry->GetThenPin() : nullptr, VariableGetNode->GetValuePin(), VisibleResult->GetExecPin(), CollapsedResult->GetExecPin());
}

void WidgetBlueprintBuilder::PatchTextBind(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UTextBlock> TextBlock, const FName& VariableName)
{
	AddBindingProperty(WidgetBP, TextBlock, "Text", VariableName);
}

void WidgetBlueprintBuilder::CreateSwitchFunction(TObjectPtr<UWidgetBlueprint> WidgetBP, const FString& PropertyName, const TArray<FString>& PinNames)
{
	if (!WidgetBP)
		return;

	FString FunctionName = "Set" + PropertyName;
	TObjectPtr<UEdGraph>* Graph = WidgetBP->FunctionGraphs.FindByPredicate([FunctionName](const TObjectPtr<UEdGraph> Graph)
		{
			return Graph.GetName() == FunctionName;
		});
	UEdGraph* FunctionGraph = Graph ? *Graph : nullptr;
	if (!FunctionGraph)
	{
		FunctionGraph = FBlueprintEditorUtils::CreateNewGraph(WidgetBP, FBlueprintEditorUtils::FindUniqueKismetName(WidgetBP, FunctionName), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());

		FBlueprintEditorUtils::AddFunctionGraph<UClass>(WidgetBP, FunctionGraph, true, nullptr);
	}

	const UK2Node_FunctionEntry* FunctionEntry = PatchFunctionEntry(FunctionGraph, PropertyName, UEdGraphSchema_K2::PC_String, EPinContainerType::None);
	const FVector2D StartPos = FVector2D(FunctionEntry->NodePosX, FunctionEntry->NodePosY);
	
	const FVector2D SwitchPosition = StartPos + FVector2D(BaseSize.X + Pan.X, 0.0f);
	PatchSwitchStringNode(FunctionGraph, SwitchPosition, FunctionEntry->GetThenPin(), PinNames);
}

void WidgetBlueprintBuilder::PatchSwitchFunction(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidgetSwitcher> WidgetSwitcher, const FString& PropertyName, TArray<FString> Values)
{
	if (!WidgetBP || !WidgetSwitcher)
		return;

	FString FunctionName = "Set" + PropertyName;
	TObjectPtr<UEdGraph>* Graph = WidgetBP->FunctionGraphs.FindByPredicate([FunctionName](const TObjectPtr<UEdGraph> Graph)
		{
			return Graph.GetName() == FunctionName;
		});
	UEdGraph* FunctionGraph = Graph ? *Graph : nullptr;
	if (!FunctionGraph)
		return;

	const UK2Node_SwitchString* SwitchNode = nullptr;
	TArray<UK2Node_SwitchString*> ExistingNodes;
	FunctionGraph->GetNodesOfClass<UK2Node_SwitchString>(ExistingNodes);
	if (ExistingNodes.Num() == 1)
	{
		SwitchNode = ExistingNodes[0];
	}

	if (!SwitchNode)
		return;

	
	const FVector2D StartPos = FVector2D(SwitchNode->NodePosX, SwitchNode->NodePosY);

	const FVector2D GetPosition = StartPos + FVector2D(0.0f, -BaseSize.Y - Pan.Y);
	UK2Node_VariableGet* Target = PatchVariableGetNode(WidgetBP, FunctionGraph, *PropertyName, GetPosition);

	for (FString Value : Values)
	{
	}
}

void WidgetBlueprintBuilder::SetPropertyValue(TObjectPtr<UUserWidget> Widget, const FName& VariableName, const FFigmaComponentProperty& ComponentProperty)
{
	if (!Widget)
		return;

	UClass* WidgetClass = Widget->GetClass();
	FProperty* Property = WidgetClass ? FindFProperty<FProperty>(WidgetClass, VariableName) : nullptr;
	if(Property)
	{
		switch (ComponentProperty.Type)
		{
			case EFigmaComponentPropertyType::BOOLEAN:
			{
				static FString True("True");
				const FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property);
				void* Value = BoolProperty->ContainerPtrToValuePtr<uint8>(Widget);
				BoolProperty->SetPropertyValue(Value, ComponentProperty.Value.Compare(True, ESearchCase::IgnoreCase) == 0);
			}
			break;
			case EFigmaComponentPropertyType::TEXT:
			{
				const FStrProperty* StringProperty = CastField<FStrProperty>(Property);
				void* Value = StringProperty->ContainerPtrToValuePtr<uint8>(Widget);
				StringProperty->SetPropertyValue(Value, ComponentProperty.Value);
			}
			break;
			case EFigmaComponentPropertyType::INSTANCE_SWAP:
				//TODO:
			break;
			case EFigmaComponentPropertyType::VARIANT:
				//TODO:
			break;
		}
	}
}

void WidgetBlueprintBuilder::AddBindingFunction(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, UEdGraph* FunctionGraph, const FName& PropertyName)
{
	UFunction* Function = WidgetBP->SkeletonGeneratedClass->FindFunctionByName(FunctionGraph->GetFName());

	FDelegateEditorBinding Binding;
	Binding.ObjectName = Widget->GetName();
	Binding.PropertyName = PropertyName;
	Binding.SourcePath = FEditorPropertyPath({ Function });

	if (Function != nullptr)
	{
		Binding.FunctionName = Function->GetFName();

		UBlueprint::GetGuidFromClassByFieldName<UFunction>(
			Function->GetOwnerClass(),
			Function->GetFName(),
			Binding.MemberGuid);

		Binding.Kind = EBindingKind::Function;
	}

	WidgetBP->Bindings.Remove(Binding);
	WidgetBP->Bindings.AddUnique(Binding);

	const UEdGraphSchema_K2* Schema_K2 = Cast<UEdGraphSchema_K2>(FunctionGraph->GetSchema());
	Schema_K2->AddExtraFunctionFlags(FunctionGraph, FUNC_BlueprintPure);
}

void WidgetBlueprintBuilder::AddBindingProperty(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, const FName& PropertyName, const FName& MemberPropertyName)
{
	FProperty* Property = WidgetBP->SkeletonGeneratedClass->FindPropertyByName(MemberPropertyName);

	FDelegateEditorBinding Binding;
	Binding.ObjectName = Widget->GetName();
	Binding.PropertyName = PropertyName;
	Binding.SourcePath = FEditorPropertyPath({ Property });
	Binding.SourceProperty = MemberPropertyName;
	Binding.Kind = EBindingKind::Property;

	UBlueprint::GetGuidFromClassByFieldName<FProperty>(WidgetBP->SkeletonGeneratedClass, MemberPropertyName, Binding.MemberGuid);

	WidgetBP->Bindings.Remove(Binding);
	WidgetBP->Bindings.AddUnique(Binding);
}

UK2Node_FunctionEntry* WidgetBlueprintBuilder::PatchFunctionEntry(UEdGraph* Graph, const FString VarName, FName VarType, EPinContainerType VarContainerType)
{
	TObjectPtr<class UEdGraphNode>* FoundNode = Graph->Nodes.FindByPredicate([](const TObjectPtr<class UEdGraphNode> Node) { return Node && Node->IsA<UK2Node_FunctionEntry>(); });
	UK2Node_FunctionEntry* FunctionEntry = FoundNode ? Cast<UK2Node_FunctionEntry>(*FoundNode) : nullptr;
	if (!FunctionEntry)
	{
		FunctionEntry = NewObject<UK2Node_FunctionEntry>(Graph, UK2Node_FunctionEntry::StaticClass());
		FunctionEntry->CreateNewGuid();
		FunctionEntry->SetFlags(RF_Transactional);
		FunctionEntry->AllocateDefaultPins();
		FunctionEntry->PostPlacedNewNode();

		Graph->Modify();
		// the FBlueprintMenuActionItem should do the selecting
		Graph->AddNode(FunctionEntry, /*bFromUI =*/false, /*bSelectNewNode =*/false);
	}

	TSharedPtr<FUserPinInfo>* InputPtr = FunctionEntry->UserDefinedPins.FindByPredicate([VarName](const TSharedPtr<FUserPinInfo> Pin) { return Pin->PinName == VarName; });
	TSharedPtr<FUserPinInfo> Input = InputPtr ? *InputPtr : nullptr;
	if(Input)
	{
		Input->PinType.PinCategory = VarType;
		Input->PinType.ContainerType = VarContainerType;
		Input->DesiredPinDirection = EGPD_Output;
	}
	else
	{
		FEdGraphPinType PinType;
		PinType.PinCategory = VarType;
		PinType.ContainerType = VarContainerType;

		FunctionEntry->CreateUserDefinedPin(*VarName, PinType, EGPD_Output);
	}

	return FunctionEntry;
}

UK2Node_VariableGet* WidgetBlueprintBuilder::PatchVariableGetNode(TObjectPtr<UWidgetBlueprint> WidgetBP, UEdGraph* Graph, FName VariableName, FVector2D NodeLocation)
{
	TObjectPtr<class UEdGraphNode>* FoundNode = Graph->Nodes.FindByPredicate([VariableName](const TObjectPtr<class UEdGraphNode> Node)
		{
			if (!Node || !Node->IsA<UK2Node_VariableGet>())
				return false;
	
			const UK2Node_VariableGet* NodeVarGet = Cast<UK2Node_VariableGet>(Node);
			return NodeVarGet->GetVarName() == VariableName;
		});
	
	if (FoundNode)
	{
		TObjectPtr<UK2Node_VariableGet> VariableGetNode = Cast<UK2Node_VariableGet>(*FoundNode);
		VariableGetNode->NodePosX = static_cast<int32>(NodeLocation.X);
		VariableGetNode->NodePosY = static_cast<int32>(NodeLocation.Y);
		return VariableGetNode;
	}
	
	const UEdGraphSchema_K2* K2_Schema = Cast<const UEdGraphSchema_K2>(Graph->GetSchema());
	if (K2_Schema)
	{
		return K2_Schema->SpawnVariableGetNode(NodeLocation, Graph, VariableName, WidgetBP->SkeletonGeneratedClass);
	}
	
	return nullptr;
}

UK2Node_IfThenElse* WidgetBlueprintBuilder::PatchIfThenElseNode(UEdGraph* Graph, FVector2D NodeLocation, UEdGraphPin* ExecPin, UEdGraphPin* ConditionValuePin, UEdGraphPin* ThenReturnPin, UEdGraphPin* ElseReturnPin)
{
	UK2Node_IfThenElse* IfThenElseNode = nullptr;
	TArray<UK2Node_IfThenElse*> ExistingNodes;
	Graph->GetNodesOfClass<UK2Node_IfThenElse>(ExistingNodes);
	if (ExistingNodes.IsEmpty())
	{
		IfThenElseNode = NewObject<UK2Node_IfThenElse>(Graph, UK2Node_IfThenElse::StaticClass());
		IfThenElseNode->CreateNewGuid();
		IfThenElseNode->SetFlags(RF_Transactional);
		IfThenElseNode->AllocateDefaultPins();
		IfThenElseNode->PostPlacedNewNode();

		Graph->Modify();
		// the FBlueprintMenuActionItem should do the selecting
		Graph->AddNode(IfThenElseNode, /*bFromUI =*/false, /*bSelectNewNode =*/false);
	}
	else if (ExistingNodes.Num() > 1)
	{
		IfThenElseNode = ExistingNodes[0];
		for (int i = 1; i < ExistingNodes.Num(); i++)
		{
			Graph->RemoveNode(ExistingNodes[i]);
		}
	}
	else
	{
		IfThenElseNode = ExistingNodes[0];
	}

	IfThenElseNode->NodePosX = static_cast<int32>(NodeLocation.X);
	IfThenElseNode->NodePosY = static_cast<int32>(NodeLocation.Y);

	UEdGraphPin* ExecutePin = IfThenElseNode->GetExecPin();
	if (ExecPin && ExecutePin)
	{
		ExecPin->BreakAllPinLinks();
		ExecPin->MakeLinkTo(ExecutePin);
	}

	UEdGraphPin* ConditionPin = IfThenElseNode->GetConditionPin();
	if (ConditionValuePin && ConditionPin)
	{
		ConditionPin->BreakAllPinLinks();
		ConditionValuePin->MakeLinkTo(ConditionPin);
	}

	UEdGraphPin* ThenPin = IfThenElseNode->GetThenPin();
	if (ThenReturnPin && ThenPin)
	{
		ThenPin->BreakAllPinLinks();
		ThenPin->MakeLinkTo(ThenReturnPin);
	}
	UEdGraphPin* ElsePin = IfThenElseNode->GetElsePin();
	if (ElseReturnPin && ElsePin)
	{
		ElsePin->BreakAllPinLinks();
		ElsePin->MakeLinkTo(ElseReturnPin);
	}
	return IfThenElseNode;
}

UK2Node_SwitchString* WidgetBlueprintBuilder::PatchSwitchStringNode(UEdGraph* Graph, FVector2D NodeLocation, UEdGraphPin* ExecPin, const TArray<FString>& PinNames)
{
	UK2Node_SwitchString* SwitchNode = nullptr;
	TArray<UK2Node_SwitchString*> ExistingNodes;
	Graph->GetNodesOfClass<UK2Node_SwitchString>(ExistingNodes);
	if (ExistingNodes.IsEmpty())
	{
		SwitchNode = NewObject<UK2Node_SwitchString>(Graph, UK2Node_SwitchString::StaticClass());
		SwitchNode->CreateNewGuid();
		SwitchNode->SetFlags(RF_Transactional);
		SwitchNode->AllocateDefaultPins();
		SwitchNode->PostPlacedNewNode();

		Graph->Modify();
		// the FBlueprintMenuActionItem should do the selecting
		Graph->AddNode(SwitchNode, /*bFromUI =*/false, /*bSelectNewNode =*/false);
	}
	else if (ExistingNodes.Num() > 1)
	{
		SwitchNode = ExistingNodes[0];
		for (int i = 1; i < ExistingNodes.Num(); i++)
		{
			Graph->RemoveNode(ExistingNodes[i]);
		}
	}
	else
	{
		SwitchNode = ExistingNodes[0];
	}

	SwitchNode->NodePosX = static_cast<int32>(NodeLocation.X);
	SwitchNode->NodePosY = static_cast<int32>(NodeLocation.Y);

	UEdGraphPin* ExecutePin = SwitchNode->GetExecPin();
	if (ExecPin && ExecutePin)
	{
		ExecPin->BreakAllPinLinks();
		ExecPin->MakeLinkTo(ExecutePin);
	}

	static const FName DefaultPinName("Default");
	for (int i = 0; i < PinNames.Num(); i++)
	{
		const FName PinName = *PinNames[i];

		UEdGraphPin* ThenExecPin = nullptr;
		if (PinName == DefaultPinName)
		{
			ThenExecPin = SwitchNode->FindPin(DefaultPinName, EGPD_Output);
		}
		else
		{
			if(SwitchNode->PinNames.Num() <= i)
			{
				SwitchNode->PinNames.Add(PinName);
				ThenExecPin = SwitchNode->CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinName);
				ThenExecPin->bAllowFriendlyName = false;
			}
			else if(SwitchNode->PinNames[i] != PinName)
			{
				SwitchNode->PinNames[i] = PinName;
				ThenExecPin = SwitchNode->FindPin(PinName, EGPD_Output);
				if(ThenExecPin)
				{
					ThenExecPin->PinName = PinName;
				}
				else
				{
					ThenExecPin = SwitchNode->CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinName);
					ThenExecPin->bAllowFriendlyName = false;
				}
			}
		}


	return SwitchNode;
}

UK2Node_FunctionResult* WidgetBlueprintBuilder::PatchFunctionResult(UEdGraph* Graph, FVector2D NodeLocation, const FString& ReturnValue)
{
	UK2Node_FunctionResult* FunctionResult = nullptr;
	TObjectPtr<class UEdGraphNode>* FoundNode = Graph->Nodes.FindByPredicate([ReturnValue](const TObjectPtr<class UEdGraphNode> Node)
		{
			if (!Node || !Node->IsA<UK2Node_FunctionResult>())
				return false;
	
			const UK2Node_FunctionResult* NodeVarGet = Cast<UK2Node_FunctionResult>(Node);
			UEdGraphPin* ReturnPin = NodeVarGet->FindPin(UEdGraphSchema_K2::PN_ReturnValue);
			return ReturnPin && ReturnPin->DefaultValue == ReturnValue;
		});
	
	if (FoundNode)
	{
		FunctionResult = Cast<UK2Node_FunctionResult>(*FoundNode);
	}
	else
	{
		FunctionResult = NewObject<UK2Node_FunctionResult>(Graph, UK2Node_FunctionResult::StaticClass());
		FunctionResult->CreateNewGuid();
		FunctionResult->SetFlags(RF_Transactional);
		FunctionResult->AllocateDefaultPins();
		FunctionResult->PostPlacedNewNode();
	
		Graph->Modify();
		// the FBlueprintMenuActionItem should do the selecting
		Graph->AddNode(FunctionResult, /*bFromUI =*/false, /*bSelectNewNode =*/false);
	}
	
	FunctionResult->NodePosX = static_cast<int32>(NodeLocation.X);
	FunctionResult->NodePosY = static_cast<int32>(NodeLocation.Y);
	
	UEdGraphPin* ReturnPin = FunctionResult->FindPin(UEdGraphSchema_K2::PN_ReturnValue);
	ReturnPin->DefaultValue = ReturnValue;
	
	return FunctionResult;
}
