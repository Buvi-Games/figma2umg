// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/FigmaNode.h"

#include "BlueprintNodeSpawner.h"
#include "FigmaCanvas.h"
#include "FigmaComponent.h"
#include "FigmaComponentSet.h"
#include "FigmaConnector.h"
#include "FigmaDocument.h"
#include "FigmaFrame.h"
#include "FigmaInstance.h"
#include "FigmaSection.h"
#include "FigmaShapeWithText.h"
#include "FigmaSlice.h"
#include "FigmaSticky.h"
#include "FileHelpers.h"
#include "JsonObjectConverter.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_VariableGet.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Interfaces/WidgetOwner.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Table/FigmaTable.h"
#include "Table/FigmaTableCell.h"
#include "Vectors/FigmaBooleanOp.h"
#include "Vectors/FigmaEllipse.h"
#include "Vectors/FigmaLine.h"
#include "Vectors/FigmaPoly.h"
#include "Vectors/FigmaRectangleVector.h"
#include "Vectors/FigmaStar.h"
#include "Vectors/FigmaText.h"
#include "Vectors/FigmaVectorNode.h"
#include "Vectors/FigmaWashiTape.h"

FString UFigmaNode::GetNodeName() const
{
	return Name;
}

FString UFigmaNode::GetUniqueName() const
{
	FString IdReplace = Id.Replace(TEXT(":"), TEXT("-"), ESearchCase::CaseSensitive);
	return Name + "--" + IdReplace;
}

ESlateVisibility UFigmaNode::GetVisibility() const
{
	return Visible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
}

FVector2D UFigmaNode::GetPosition() const
{
	FVector2D Pos = GetAbsolutePosition();
	if (ParentNode)
	{
		const FVector2D ParentPos = ParentNode->GetAbsolutePosition();
		Pos = (Pos - ParentPos);
	}
	return Pos;
}

void UFigmaNode::SetCurrentPackagePath(const FString& InPackagePath)
{
	PackagePath = InPackagePath;
}

FString UFigmaNode::GetCurrentPackagePath() const
{
	if (PackagePath.IsEmpty() && ParentNode != nullptr)
	{
		return ParentNode->GetCurrentPackagePath();
	}
	return PackagePath;
}

TObjectPtr<UFigmaFile> UFigmaNode::GetFigmaFile() const
{
	if(ParentNode)
	{
		return ParentNode->GetFigmaFile();
	}

	return nullptr;
}

UObject* UFigmaNode::GetAssetOuter() const
{
	if(const IFigmaFileHandle* FileHandle = Cast<IFigmaFileHandle>(this))
	{
		return FileHandle->GetOuter();
	}
	else if (ParentNode)
	{
		return ParentNode->GetAssetOuter();
	}

	return nullptr;
}

TObjectPtr<UFigmaNode> UFigmaNode::FindTypeByID(const UClass* Class, const FString& ID)
{
	if (this->IsA(Class) && Id == ID)
		return this;

	if (IFigmaContainer* FigmaContainer = Cast<IFigmaContainer>(this))
	{
		TArray<UFigmaNode*>& Children = FigmaContainer->GetChildren();
		for (TObjectPtr<UFigmaNode> Child : Children)
		{
			TObjectPtr<UFigmaNode> Found = Child->FindTypeByID(Class, ID);
			if (Found)
				return Found;
		}
	}

	return nullptr;
}

void UFigmaNode::SerializeArray(TArray<UFigmaNode*>& Array, const TSharedRef<FJsonObject> JsonObj, const FString& ArrayName)
{
	Array.Reset();
	if (JsonObj->HasTypedField<EJson::Array>(ArrayName))
	{
		const TArray<TSharedPtr<FJsonValue>>& ArrayJson = JsonObj->GetArrayField(ArrayName);
		for (const TSharedPtr<FJsonValue>& Item : ArrayJson)
		{
			if (Item.IsValid() && Item->Type == EJson::Object)
			{
				const TSharedPtr<FJsonObject>& ItemObject = Item->AsObject();
				UFigmaNode* Node = CreateNode(ItemObject);
				if (Node != nullptr)
				{
					Array.Add(Node);
				}
			}
		}
	}
}

void UFigmaNode::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	ParentNode = InParent;

	if (IFigmaContainer* FigmaContainer = Cast<IFigmaContainer>(this))
	{
		SerializeArray(FigmaContainer->GetChildren(), JsonObj, FigmaContainer->GetJsonArrayName());
	}
}

void UFigmaNode::PrePatchWidget()
{
	if (IFigmaContainer* FigmaContainer = Cast<IFigmaContainer>(this))
	{
		FigmaContainer->ForEach(IFigmaContainer::FOnEachFunction::CreateLambda([](UFigmaNode& Node, const int Index)
			{
				Node.PrePatchWidget();
			}));
	}
	
}

TObjectPtr<UWidget> UFigmaNode::PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch)
{
	UPanelWidget* ParentWidget = Cast<UPanelWidget>(WidgetToPatch);
	if(IWidgetOwner* WidgetOwner = Cast<IWidgetOwner>(this))
	{
		WidgetToPatch = WidgetOwner->Patch(WidgetToPatch);
		ParentWidget = WidgetOwner->GetContainerWidget();
	}

	IFigmaContainer* FigmaContainer = Cast<IFigmaContainer>(this);
	if (FigmaContainer && ParentWidget)
	{
		FigmaContainer->ForEach(IFigmaContainer::FOnEachFunction::CreateLambda([ParentWidget](UFigmaNode& ChildNode, const int Index)
			{
				TObjectPtr<UWidget> OldWidget = ParentWidget->GetChildAt(Index);
				TObjectPtr<UWidget> NewWidget = ChildNode.PatchPreInsertWidget(OldWidget);
				if (NewWidget)
				{
					if (NewWidget != OldWidget)
					{
						ParentWidget->SetFlags(RF_Transactional);
						ParentWidget->Modify();

						if (Index < ParentWidget->GetChildrenCount())
						{
							ParentWidget->ReplaceChildAt(Index, NewWidget);
						}
						else
						{
							ParentWidget->AddChild(NewWidget);
						}
					}
				}
			}));
	}

	return WidgetToPatch;
}

void UFigmaNode::PatchPostInsertWidget()
{
	if (IWidgetOwner* WidgetOwner = Cast<IWidgetOwner>(this))
	{
		if (TObjectPtr<UWidget> Widget = WidgetOwner->GetTopWidget())
		{
			Widget->SetVisibility(GetVisibility());

			//ProcessPropertyReferences(Widget);
		}

		WidgetOwner->PostInsert();
	}

	if (IFigmaContainer* FigmaContainer = Cast<IFigmaContainer>(this))
	{
		FigmaContainer->ForEach(IFigmaContainer::FOnEachFunction::CreateLambda([](UFigmaNode& Node, const int Index)
			{
				Node.PatchPostInsertWidget();
			}));
	}
}

void UFigmaNode::PostPatchWidget()
{
	if (IFigmaFileHandle* FileHandle = Cast<IFigmaFileHandle>(this))
	{
		UObject* Asset = FileHandle->GetAsset();
		UObject* AssetOuter = FileHandle->GetAsset();
		if (Asset)
		{
			Asset->Modify();
		}
		if (AssetOuter && AssetOuter != Asset)
		{
			AssetOuter->Modify();
		}

		if (UBlueprint * BlueprintObj = Cast<UBlueprint>(Asset))
		{
			FCompilerResultsLog LogResults;
			LogResults.SetSourcePath(BlueprintObj->GetPathName());
			LogResults.BeginEvent(TEXT("Compile"));
			LogResults.bLogDetailedResults = true;

			FKismetEditorUtilities::CompileBlueprint(BlueprintObj, EBlueprintCompileOptions::None, &LogResults);

			LogResults.EndEvent();
		}
		else if (UTexture2D* Texture = Cast<UTexture2D>(Asset))
		{
			FEditorFileUtils::FPromptForCheckoutAndSaveParams Params;
			FEditorFileUtils::PromptForCheckoutAndSave({ Texture->GetPackage() }, Params);
		}

		FileHandle->Reset();
	}

	if (IWidgetOwner* WidgetOwner = Cast<IWidgetOwner>(this))
	{
		WidgetOwner->Reset();
	}

	if (IFigmaContainer* FigmaContainer = Cast<IFigmaContainer>(this))
	{
		FigmaContainer->ForEach(IFigmaContainer::FOnEachFunction::CreateLambda([](UFigmaNode& Node, const int Index)
			{
				Node.PostPatchWidget();
			}));
	}
}

UFigmaNode* UFigmaNode::CreateNode(const TSharedPtr<FJsonObject>& JsonObj)
{
	if (!JsonObj->HasTypedField<EJson::String>("type"))
		return nullptr;

	const FString NodeTypeStr = JsonObj->GetStringField("type");
	
	static const FString EnumPath = "/Script/Figma2UMG.ENodeTypes";
	static UEnum* EnumDef = FindObject<UEnum>(nullptr, *EnumPath, true);
	if (!EnumDef)
		return nullptr;

	UFigmaNode* FigmaNode = nullptr;
	switch (const ENodeTypes NodeType = static_cast<ENodeTypes>(EnumDef->GetValueByName(*NodeTypeStr)))
	{
	case ENodeTypes::DOCUMENT:
		FigmaNode = NewObject<UFigmaDocument>();
		break;
	case ENodeTypes::CANVAS:
		FigmaNode = NewObject<UFigmaCanvas>();
		break;
	case ENodeTypes::FRAME:
		FigmaNode = NewObject<UFigmaFrame>();
		break;
	case ENodeTypes::GROUP:
		FigmaNode = NewObject<UFigmaGroup>();
		break;
	case ENodeTypes::SECTION:
		FigmaNode = NewObject<UFigmaSection>();
		break;
	case ENodeTypes::VECTOR:
		FigmaNode = NewObject<UFigmaVectorNode>();
		break;
	case ENodeTypes::BOOLEAN_OPERATION:
		FigmaNode = NewObject<UFigmaBooleanOp>();
		break;
	case ENodeTypes::STAR:
		FigmaNode = NewObject<UFigmaStar>();
		break;
	case ENodeTypes::LINE:
		FigmaNode = NewObject<UFigmaLine>();
		break;
	case ENodeTypes::ELLIPSE:
		FigmaNode = NewObject<UFigmaEllipse>();
		break;
	case ENodeTypes::REGULAR_POLYGON:
		FigmaNode = NewObject<UFigmaPoly>();
		break;
	case ENodeTypes::RECTANGLE:
		FigmaNode = NewObject<UFigmaRectangleVector>();
		break;
	case ENodeTypes::TABLE:
		FigmaNode = NewObject<UFigmaTable>();
		break;
	case ENodeTypes::TABLE_CELL:
		FigmaNode = NewObject<UFigmaTableCell>();
		break;
	case ENodeTypes::TEXT:
		FigmaNode = NewObject<UFigmaText>();
		break;
	case ENodeTypes::SLICE:
		FigmaNode = NewObject<UFigmaSlice>();
		break;
	case ENodeTypes::COMPONENT:
		FigmaNode = NewObject<UFigmaComponent>();
		break;
	case ENodeTypes::COMPONENT_SET:
		FigmaNode = NewObject<UFigmaComponentSet>();
		break;
	case ENodeTypes::INSTANCE:
		FigmaNode = NewObject<UFigmaInstance>();
		break;
	case ENodeTypes::STICKY:
		FigmaNode = NewObject<UFigmaSticky>();
		break;
	case ENodeTypes::SHAPE_WITH_TEXT:
		FigmaNode = NewObject<UFigmaShapeWithText>();
		break;
	case ENodeTypes::CONNECTOR:
		FigmaNode = NewObject<UFigmaConnector>();
		break;
	case ENodeTypes::WASHI_TAPE:
		FigmaNode = NewObject<UFigmaWashiTape>();
		break;
	}

	if (FigmaNode != nullptr && FJsonObjectConverter::JsonObjectToUStruct(JsonObj.ToSharedRef(), FigmaNode->GetClass(), FigmaNode))
	{
		FigmaNode->PostSerialize(this, JsonObj.ToSharedRef());
	}
	
	return FigmaNode;
}

void UFigmaNode::ProcessComponentPropertyReferences(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget) const
{
	if (WidgetBP == nullptr || Widget == nullptr)
		return;

	for (const TPair<FString, FString>& ComponentPropertyReference : ComponentPropertyReferences)
	{
		ProcessComponentPropertyReference(WidgetBP, Widget, ComponentPropertyReference);
	}
}

void UFigmaNode::ProcessComponentPropertyReference(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, const TPair<FString, FString>& PropertyReference) const
{
	static const FString VisibleStr("visible");
	const FBPVariableDescription* VariableDescription = WidgetBP->NewVariables.FindByPredicate([PropertyReference](const FBPVariableDescription& VariableDescription)
		{
			return VariableDescription.VarName == PropertyReference.Value;
		});

	if(VariableDescription == nullptr)
		return;

	if (PropertyReference.Key == VisibleStr)
	{
		PatchVisibilityBind(WidgetBP, Widget, *VariableDescription, *PropertyReference.Value);
	}
}

void UFigmaNode::AddBinding(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, UEdGraph* FunctionGraph, const FName& PropertyName) const
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

void UFigmaNode::PatchVisibilityBind(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, const FBPVariableDescription& VariableDescription, const FName& VariableName) const
{
	FString FunctionName = "Get_" + Widget.GetName() + "_Visibility";
	TObjectPtr<UEdGraph>* Graph = WidgetBP->FunctionGraphs.FindByPredicate([FunctionName](const TObjectPtr<UEdGraph> Graph)
		{
			return Graph.GetName() == FunctionName;
		});

	UEdGraph* FunctionGraph = Graph ? *Graph : nullptr;

	if (!FunctionGraph)
	{
		FunctionGraph = FBlueprintEditorUtils::CreateNewGraph(
		   WidgetBP,
		   FBlueprintEditorUtils::FindUniqueKismetName(WidgetBP, FunctionName),
		   UEdGraph::StaticClass(),
		   UEdGraphSchema_K2::StaticClass());

		UFunction* BindableSignature = Widget->GetClass()->FindFunctionByName("GetSlateVisibility__DelegateSignature");
		FBlueprintEditorUtils::AddFunctionGraph(WidgetBP, FunctionGraph, true, BindableSignature);
	}

	AddBinding(WidgetBP, Widget, FunctionGraph, "Visibility");

	TObjectPtr<class UEdGraphNode>* FoundEntryNode = FunctionGraph->Nodes.FindByPredicate([](const TObjectPtr<class UEdGraphNode> Node)
		{
			return Node && Node->IsA<UK2Node_FunctionEntry>();
		});

	TObjectPtr<UK2Node_FunctionEntry> FunctionEntry = FoundEntryNode ? Cast<UK2Node_FunctionEntry>(*FoundEntryNode) : nullptr;

	FVector2D StartPos = FunctionEntry ? FVector2D(FunctionEntry->NodePosX, FunctionEntry->NodePosY) : FVector2D(0.0f, 0.0f);
	FVector2D BaseSize = FVector2D(300.0f, 150.0f);
	FVector2D Pan = FVector2D(20.0f, 20.0f);

	FVector2D GetGraphPosition = StartPos + FVector2D(0.0f, BaseSize.Y + Pan.Y);
	UK2Node_VariableGet* VariableGetNode = PatchVariableGetNode(WidgetBP, FunctionGraph, VariableName, GetGraphPosition);

	FVector2D VisibleResultPosition = StartPos + FVector2D((BaseSize.X + Pan.X) * 2.0f, StartPos.Y);
	UK2Node_FunctionResult* VisibleResult = PatchFunctionResult(FunctionGraph, VisibleResultPosition, "Visible");

	FVector2D CollapsedResultPosition = VisibleResultPosition + FVector2D(0, BaseSize.Y + Pan.Y);
	UK2Node_FunctionResult* CollapsedResult = PatchFunctionResult(FunctionGraph, CollapsedResultPosition, "Collapsed");

	FVector2D IfThenElseGraphPosition = StartPos + FVector2D(BaseSize.X + Pan.X, StartPos.Y);
	PatchIfThenElseNode(FunctionGraph, IfThenElseGraphPosition, FunctionEntry ? FunctionEntry->GetThenPin() : nullptr, VariableGetNode->GetValuePin(), VisibleResult->GetExecPin(), CollapsedResult->GetExecPin());
}

UK2Node_VariableGet* UFigmaNode::PatchVariableGetNode(TObjectPtr<UWidgetBlueprint> WidgetBP, UEdGraph* Graph, FName VariableName, FVector2D NodeLocation) const
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

UK2Node_IfThenElse* UFigmaNode::PatchIfThenElseNode(UEdGraph* Graph, FVector2D NodeLocation, UEdGraphPin* ExecPin, UEdGraphPin* ConditionValuePin, UEdGraphPin* ThenReturnPin, UEdGraphPin* ElseReturnPin) const
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
	if(ConditionValuePin && ConditionPin)
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

UK2Node_FunctionResult* UFigmaNode::PatchFunctionResult(UEdGraph* Graph, FVector2D NodeLocation, const FString& ReturnValue) const
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
