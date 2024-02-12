// Fill out your copyright notice in the Description page of Project Settings.


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
#include "Blueprint/WidgetTree.h"
#include "Interfaces/WidgetOwner.h"
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

//void UFigmaNode::PatchPreInsertWidgetChildren(UPanelWidget* ParentWidget, const TArray<UFigmaNode*>& Children) const
//{
//	if (!ParentWidget)
//		return;
//
//	for (int Index = 0; Index < Children.Num(); Index++)
//	{
//		UFigmaNode* Element = Children[Index];
//
//		TObjectPtr<UWidget> OldWidget = ParentWidget->GetChildAt(Index);
//		TObjectPtr<UWidget> NewWidget = Element->PatchPreInsertWidget(OldWidget);
//		if (NewWidget)
//		{
//			if(NewWidget != OldWidget)
//			{
//				ParentWidget->SetFlags(RF_Transactional);
//				ParentWidget->Modify();
//
//				if (Index < ParentWidget->GetChildrenCount())
//				{
//					ParentWidget->ReplaceChildAt(Index, NewWidget);
//				}
//				else
//				{
//					ParentWidget->AddChild(NewWidget);
//				}
//			}
//		}
//	}
//}
