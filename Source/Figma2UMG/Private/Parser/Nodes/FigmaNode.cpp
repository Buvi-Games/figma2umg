// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaNode.h"

#include "Figma2UMGModule.h"
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
#include "JsonObjectConverter.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/WidgetBlueprintHelper.h"
#include "Builder/Asset/MaterialBuilder.h"
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

FString UFigmaNode::GetUAssetName() const
{
	return GetUniqueName();
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

TObjectPtr<UWidget> UFigmaNode::FindWidgetForNode(const TObjectPtr<UPanelWidget>& ParentWidget) const
{
	if (!ParentWidget)
		return nullptr;

	TArray<UWidget*> AllChildren = ParentWidget->GetAllChildren();
	for (TObjectPtr<UWidget> Widget : AllChildren)
	{
		if (Widget == nullptr)
			continue;

		if (Widget->GetName().Contains(GetIdForName(), ESearchCase::IgnoreCase))
		{
			return Widget;
		}
	}

	return nullptr;
}

void UFigmaNode::CreatePaintAssetBuilderIfNeeded(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders, TArray<FFigmaPaint>& InFills) const
{
	for (FFigmaPaint& Paint : InFills)
	{
		Paint.CreateAssetBuilder(InFileKey, this, AssetBuilders);		
	}
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

UFigmaNode* UFigmaNode::CreateNode(const TSharedPtr<FJsonObject>& JsonObj)
{
	static FString TypeStr("type");
	if (!JsonObj->HasTypedField<EJson::String>(TypeStr))
		return nullptr;

	const FString NodeTypeStr = JsonObj->GetStringField(TypeStr);
	
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
#if (ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3)
			return VariableDescription.VarName == PropertyReference.Value;
#else
			return VariableDescription.VarName.ToString() == PropertyReference.Value;
#endif
		});


	if (VariableDescription != nullptr)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[ProcessComponentPropertyReference] Variable '%s' found in UWidgetBlueprint %s."), *PropertyReference.Value, *WidgetBP->GetName());

		if (PropertyReference.Key == VisibleStr)
		{
			WidgetBlueprintHelper::PatchVisibilityBind(WidgetBP, Widget, *PropertyReference.Value);
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