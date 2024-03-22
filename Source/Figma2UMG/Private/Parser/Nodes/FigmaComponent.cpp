// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/FigmaComponent.h"

#include "FigmaInstance.h"
#include "WidgetBlueprint.h"
#include "WidgetBlueprintFactory.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Parser/FigmaFile.h"
#include "Parser/Properties/FigmaComponentRef.h"
#include "Templates/WidgetTemplateBlueprintClass.h"

void UFigmaComponent::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	FFigmaComponentRef* ComponentRef = FigmaFile->FindComponentRef(GetId());
	ComponentRef->SetComponent(this);
}

void UFigmaComponent::Reset()
{
	Super::Reset();
	ResetAsset();
	InstanceAsset = nullptr;
}

FString UFigmaComponent::GetPackagePath() const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "Components";
}

FString UFigmaComponent::GetAssetName() const
{
	return GetUniqueName();
}

void UFigmaComponent::LoadOrCreateAssets(UFigmaFile* FigmaFile)
{
	UWidgetBlueprint* WidgetBP = GetOrCreateAsset<UWidgetBlueprint, UWidgetBlueprintFactory>();
	if (PatchPropertiesToWidget(WidgetBP))
	{
		CompileBP(GetNodeName());
	}

	RefAsset = WidgetBP;
}

void UFigmaComponent::LoadAssets()
{
	RefAsset = LoadAsset<UWidgetBlueprint>();
}


void UFigmaComponent::PrePatchWidget()
{
	Super::PrePatchWidget();
}

TObjectPtr<UWidget> UFigmaComponent::PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch)
{
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
			TObjectPtr<UWidget> OldWidget = PanelWidget->GetChildAt(Index);
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

	return WidgetInstance;
}

void UFigmaComponent::SetWidget(TObjectPtr<UWidget> Widget)
{
	if (Widget)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[SetWidget] UFigmaComponent %s received a UWidget %s of type %s."), *GetNodeName(), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
	}

	UWidgetBlueprint* WidgetBP = GetAsset<UWidgetBlueprint>();
	Super::SetWidget(WidgetBP->WidgetTree->RootWidget);

	if (ParentNode)
	{
		InstanceAsset = Widget;
	}

	TObjectPtr<UPanelWidget> PanelWidget = GetContainerWidget();
	IFigmaContainer::ForEach(IFigmaContainer::FOnEachFunction::CreateLambda([ PanelWidget](UFigmaNode& ChildNode, const int Index)
		{
			TObjectPtr<UWidget> Widget = PanelWidget->GetChildAt(Index);
			ChildNode.SetWidget(Widget);
		}));
}

void UFigmaComponent::PostInsert() const
{
	Super::PostInsert();

	if (TObjectPtr<UWidget> WidgetInstance = Cast<UWidget>(InstanceAsset))
	{
		SetPosition(WidgetInstance, GetPosition());
		SetSize(WidgetInstance, AbsoluteBoundingBox.GetSize(), true);
	}
}

void UFigmaComponent::FillType(const FFigmaComponentPropertyDefinition& Def, FEdGraphPinType& MemberType) const
{
	MemberType.ContainerType = EPinContainerType::None;
	switch (Def.Type)
	{
	case EFigmaComponentPropertyType::BOOLEAN:
		MemberType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
		break;
	case EFigmaComponentPropertyType::TEXT:
		MemberType.PinCategory = UEdGraphSchema_K2::PC_String;

		break;
	case EFigmaComponentPropertyType::INSTANCE_SWAP:
		MemberType.PinCategory = UEdGraphSchema_K2::PC_Object;
		// MemberType.PinSubCategory = ?
		// MemberType.PinSubCategoryObject = ?
		break;
	case EFigmaComponentPropertyType::VARIANT:
		//TODO:
		break;
	}
}

bool UFigmaComponent::PatchPropertiesToWidget(UWidgetBlueprint* Widget) const
{
	bool AddedMemberVariable = false;
	for(const TPair<FString, FFigmaComponentPropertyDefinition> Property : ComponentPropertyDefinitions)
	{
		FEdGraphPinType MemberType;
		FillType(Property.Value, MemberType);
		FString PropertyName = Property.Key;//TODO: Remove '#id'
		if (FBlueprintEditorUtils::AddMemberVariable(Widget, *PropertyName, MemberType, Property.Value.DefaultValue))
		{
			FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(Widget, *PropertyName, false);
			AddedMemberVariable = true;
		}
	}

	return AddedMemberVariable;
}

void UFigmaComponent::PatchBinds()
{
	TObjectPtr<UWidgetBlueprint> WidgetBp = GetAsset<UWidgetBlueprint>();
	if(!WidgetBp)
		return;

	Super::PatchBinds(WidgetBp);
}

UWidget* UFigmaComponent::CreateInstance(UObject* InAssetOuter) const
{
	TObjectPtr<UWidgetTree> OwningObject = Cast<UWidgetTree>(InAssetOuter);
	if (!OwningObject)
	{
		if(InAssetOuter)
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[UFigmaComponent::CreateInstance] AssetOuter %s is of type %s but we requite a UWidgetTree"), *InAssetOuter->GetName(), *InAssetOuter->GetClass()->GetDisplayNameText().ToString());
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[UFigmaComponent::CreateInstance] AssetOuter is nullptr"));
		}

		return nullptr;
	}
	UWidgetBlueprint* Widget = GetAsset<UWidgetBlueprint>();
	TSubclassOf<UUserWidget> UserWidgetClass = Widget->GetBlueprintClass();

	TSharedPtr<FWidgetTemplateBlueprintClass> Template = MakeShared<FWidgetTemplateBlueprintClass>(FAssetData(Widget), UserWidgetClass);
	UWidget* NewWidget = Template->Create(OwningObject);

	if (NewWidget)
	{
		NewWidget->CreatedFromPalette();
	}

	return NewWidget;
}

void UFigmaComponent::TryAddComponentPropertyDefinition(FString PropertyId, FFigmaComponentPropertyDefinition Definition)
{
	if (ComponentPropertyDefinitions.Contains(PropertyId))
		return;

	ComponentPropertyDefinitions.Add(PropertyId, Definition);
}

void UFigmaComponent::PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const
{
}
