// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaNode.h"

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
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/WidgetBlueprintBuilder.h"
#include "Interfaces/WidgetOwner.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Parser/FigmaFile.h"
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

FString UFigmaNode::GetIdForName() const
{
	FString IdForName = Id.Replace(TEXT(":"), TEXT("-"), ESearchCase::CaseSensitive);
	return IdForName;
}

FString UFigmaNode::GetNodeName() const
{
	return Name;
}

FString UFigmaNode::GetUniqueName() const
{
	return Name + "--" + GetIdForName();
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
	if (const IFigmaFileHandle* FileHandle = Cast<IFigmaFileHandle>(this))
	{
		UObject* Outer = FileHandle->GetOuter();
		if(Outer == nullptr && ParentNode && ParentNode->IsA<UFigmaComponentSet>())
		{
			return ParentNode->GetAssetOuter();
		}
		return Outer;
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

void UFigmaNode::PrepareForFlow()
{
	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	if(!FigmaFile)
	{
		UE_LOG_Figma2UMG(Error, TEXT("[PrepareForFlow] Node %s don't have a File."), *Name);
		return;
	}

	if (IFlowTransition* FlowTransition = Cast<IFlowTransition>(this))
	{
		if (FlowTransition->HasTransition())
		{
			const FString& NodeID = FlowTransition->GetTransitionNodeID();
			TObjectPtr<UFigmaFrame> Frame = FigmaFile->FindByID<UFigmaFrame>(NodeID);
			if (Frame)
			{
				Frame->SetGenerateFile();
			}
			else if (TObjectPtr<UFigmaNode> Node = FigmaFile->FindByID<UFigmaNode>(NodeID))
			{
				UE_LOG_Figma2UMG(Error, TEXT("[PrepareForFlow] File %s's contain Node with ID %s and type %s. Expecting a UFigmaFrame type"), *Name, *NodeID, *Node->GetClass()->GetName());
			}
			else
			{
				UE_LOG_Figma2UMG(Error, TEXT("[PrepareForFlow] File %s's doesn't contain Node with ID %s."), *Name, *NodeID);
			}
		}
	}

	if (IFigmaContainer* FigmaContainer = Cast<IFigmaContainer>(this))
	{
		FigmaContainer->ForEach(IFigmaContainer::FOnEachFunction::CreateLambda([](UFigmaNode& Node, const int Index)
			{
				Node.PrepareForFlow();
			}));
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
	UE_LOG_Figma2UMG(Display, TEXT("PatchPreInsertWidget [%s]"), *GetUniqueName());

	UPanelWidget* ParentWidget = Cast<UPanelWidget>(WidgetToPatch);
	if(IWidgetOwner* WidgetOwner = Cast<IWidgetOwner>(this))
	{
		WidgetToPatch = WidgetOwner->Patch(WidgetToPatch);
		ParentWidget = WidgetOwner->GetContainerWidget();
	}

	IFigmaContainer* FigmaContainer = Cast<IFigmaContainer>(this);
	if (FigmaContainer && ParentWidget)
	{
		FString NodeName = GetNodeName();
		FigmaContainer->ForEach(IFigmaContainer::FOnEachFunction::CreateLambda([NodeName, ParentWidget](UFigmaNode& ChildNode, const int Index)
			{
				TObjectPtr<UWidget> OldWidget = ParentWidget->GetChildAt(Index);
				if (OldWidget == nullptr)
				{
					TArray<UWidget*> AllChildren = ParentWidget->GetAllChildren();
					for (TObjectPtr<UWidget> Widget : AllChildren)
					{
						if (Widget == nullptr)
							continue;

						if (Widget->GetName().Contains(ChildNode.GetIdForName(), ESearchCase::IgnoreCase))
						{
							OldWidget = Widget;
							break;
						}
					}
				}
				TObjectPtr<UWidget> NewWidget = ChildNode.PatchPreInsertWidget(OldWidget);
				if (NewWidget)
				{
					if (NewWidget != OldWidget)
					{
						ParentWidget->SetFlags(RF_Transactional);
						ParentWidget->Modify();

						UE_LOG_Figma2UMG(Display, TEXT("[Widget Insert] Parent [%s] Child [%s]."), *NodeName, *ChildNode.GetNodeName());
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

void UFigmaNode::SetWidget(TObjectPtr<UWidget> Widget)
{
	if (Widget)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[SetWidget] UFigmaNode %s received a UWidget %s of type %s."), *GetNodeName(), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
	}
	UPanelWidget* ParentWidget = Cast<UPanelWidget>(Widget);
	if (IWidgetOwner* WidgetOwner = Cast<IWidgetOwner>(this))
	{
		WidgetOwner->SetupWidget(Widget);
		ParentWidget = WidgetOwner->GetContainerWidget();
	}

	IFigmaContainer* FigmaContainer = Cast<IFigmaContainer>(this);
	if (FigmaContainer && ParentWidget)
	{
		FString NodeName = GetNodeName();
		FigmaContainer->ForEach(IFigmaContainer::FOnEachFunction::CreateLambda([NodeName, ParentWidget](UFigmaNode& ChildNode, const int Index)
			{
				TObjectPtr<UWidget> ChildWidget = ParentWidget->GetChildAt(Index);
				ChildNode.SetWidget(ChildWidget);
			}));
	}
}

void UFigmaNode::InsertSubWidgets()
{
}

void UFigmaNode::PatchPostInsertWidget()
{
	UE_LOG_Figma2UMG(Display, TEXT("PatchPostInsertWidget [%s]"), *GetUniqueName());

	if (IWidgetOwner* WidgetOwner = Cast<IWidgetOwner>(this))
	{
		if (TObjectPtr<UWidget> Widget = WidgetOwner->GetTopWidget())
		{
			Widget->SetVisibility(GetVisibility());
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
#if (ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3)
			FEditorFileUtils::FPromptForCheckoutAndSaveParams Params;
			FEditorFileUtils::PromptForCheckoutAndSave({ Texture->GetPackage() }, Params);
#else
			FEditorFileUtils::PromptForCheckoutAndSave({ Texture->GetPackage() }, true, false);
#endif
		}

		FileHandle->ResetAsset();
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
			return VariableDescription.VarName.ToString() == PropertyReference.Value;
		});


	if (VariableDescription != nullptr)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[ProcessComponentPropertyReference] Variable '%s' found in UWidgetBlueprint %s."), *PropertyReference.Value, *WidgetBP->GetName());

		if (PropertyReference.Key == VisibleStr)
		{
			WidgetBlueprintBuilder::PatchVisibilityBind(WidgetBP, Widget, *PropertyReference.Value);
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[ProcessComponentPropertyReference] Unknown property '%s'."), *PropertyReference.Key);
		}

		return;
	}
	else
	{
		UClass* WidgetClass = Widget->GetClass();
		FProperty* Property = WidgetClass ? FindFProperty<FProperty>(WidgetClass, *PropertyReference.Value) : nullptr;
		if (Property)
		{
			static FString True("True");
			const FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property);
			void* Value = BoolProperty->ContainerPtrToValuePtr<uint8>(Widget);
			BoolProperty->SetPropertyValue(Value, PropertyReference.Value.Compare(True, ESearchCase::IgnoreCase) == 0);

			UE_LOG_Figma2UMG(Display, TEXT("[ProcessComponentPropertyReference] Variable '%s' found in UWidget %s."), *PropertyReference.Key, *Widget->GetName());
		}
	}

	UE_LOG_Figma2UMG(Error, TEXT("[ProcessComponentPropertyReference] Variable '%s' not found in UWidgetBlueprint %s or UWidget %s."), *PropertyReference.Value, *WidgetBP->GetName(), *Widget->GetName());

}