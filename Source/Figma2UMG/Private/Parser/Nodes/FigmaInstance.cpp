// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaInstance.h"

#include "Figma2UMGModule.h"
#include "WidgetBlueprint.h"
#include "Builder/WidgetBlueprintHelper.h"
#include "Builder/Asset/Texture2DBuilder.h"
#include "Builder/Widget/ImageWidgetBuilder.h"
#include "Builder/Widget/UserWidgetBuilder.h"
#include "Builder/Widget/WidgetSwitcherBuilder.h"
#include "Parser/FigmaFile.h"
#include "Parser/Properties/FigmaComponentRef.h"

void UFigmaInstance::PrepareForFlow()
{
	Super::PrepareForFlow();

	for (UFigmaNode* Child : Children)
	{
		Child->PrepareForFlow();
	}
}

FVector2D UFigmaInstance::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

FVector2D UFigmaInstance::GetAbsoluteSize() const
{
	return AbsoluteBoundingBox.GetSize();
}

void UFigmaInstance::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	PostSerializeProperty(JsonObj, "fills", Fills);
	PostSerializeProperty(JsonObj, "strokes", Strokes);

	SerializeArray(Children, JsonObj,"Children");
}

bool UFigmaInstance::CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders)
{
	const TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	const FFigmaComponentRef* ComponentRef = FigmaFile->FindComponentRef(ComponentId);
	const TObjectPtr<UFigmaComponent> FigmaComponent = ComponentRef ? ComponentRef->GetComponent() : nullptr;
	IsMissingComponent = (FigmaComponent == nullptr);
	if (IsMissingComponent)
	{
		//We don't have the Component Asset, import as a Texture as a PlaceHolder
		UE_LOG_Figma2UMG(Warning, TEXT("[Instance] Can't find Component %s for instance %s, import as a Texture as a PlaceHolder"), *ComponentId, *GetNodeName());

		Texture2DBuilder = NewObject<UTexture2DBuilder>();
		Texture2DBuilder->SetNode(InFileKey, this);
		AssetBuilders.Add(Texture2DBuilder);
	}

	return Texture2DBuilder != nullptr;
}

FString UFigmaInstance::GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/InstanceTextures");
}

TScriptInterface<IWidgetBuilder> UFigmaInstance::CreateWidgetBuilders(bool IsRoot /*= false*/, bool AllowFrameButton/*= true*/) const
{
	if (IsMissingComponent)
	{
		UImageWidgetBuilder* ImageWidgetBuilder = NewObject<UImageWidgetBuilder>();
		ImageWidgetBuilder->SetNode(this);
		ImageWidgetBuilder->SetTexture2DBuilder(Texture2DBuilder);
		return ImageWidgetBuilder;
	}
	else
	{
		UWidgetSwitcherBuilder* InstanceSwapBuilder = nullptr;
		if (const FFigmaComponentPropertyDefinition* PropertyDefinition = IsInstanceSwap())
		{
			InstanceSwapBuilder = NewObject<UWidgetSwitcherBuilder>();
			InstanceSwapBuilder->SetNode(this);

			for(const FFigmaInstanceSwapPreferredValue& PreferredValue : PropertyDefinition->PreferredValues)
			{
				TObjectPtr<UWidgetBlueprintBuilder> Builder = nullptr;
				TObjectPtr<UFigmaInstance> NewInstance = nullptr;
				const TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
				if (PreferredValue.Type == ENodeTypes::COMPONENT)
				{
					if(const FFigmaComponentRef* ComponentRef = FigmaFile->FindComponentRefByKey(PreferredValue.Key))
					{
						Builder = ComponentRef->GetAssetBuilder();
						TObjectPtr<UFigmaComponent> Component = ComponentRef->GetComponent();
						NewInstance = Component->InstanciateFigmaComponent(GetIdForName());
						
					}
				}
				else if (PreferredValue.Type == ENodeTypes::COMPONENT_SET)
				{
					if (const FFigmaComponentSetRef* ComponentSetRef = FigmaFile->FindComponentSetRefByKey(PreferredValue.Key))
					{
						Builder = ComponentSetRef->GetAssetBuilder();
						TObjectPtr<UFigmaComponentSet> Component = ComponentSetRef->GetComponentSet();
						NewInstance = Component->InstanciateFigmaComponent(GetIdForName());
					}
				}
				if (NewInstance)
				{
					NewInstance->ParentNode = ParentNode;
					NewInstance->ComponentProperties = ComponentProperties;
					NewInstance->Overrides = Overrides;
					InstanceSwapValues.Add(NewInstance);
				}

				if (Builder && NewInstance)
				{
					UUserWidgetBuilder* UserWidgetBuilder = NewObject<UUserWidgetBuilder>();
					UserWidgetBuilder->SetNode(NewInstance);
					UserWidgetBuilder->SetWidgetBlueprintBuilder(Builder);
					InstanceSwapBuilder->AddChild(UserWidgetBuilder);
				}
				else
				{
					UE_LOG_Figma2UMG(Warning, TEXT("[Instance] Can't find Asset Builder for PreferredValues %s in InstanceSwap Node %s"), *PreferredValue.Key, *GetNodeName());
				}
			}
			return InstanceSwapBuilder;
		}
		else
		{
			const TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
			const FFigmaComponentRef* ComponentRef = FigmaFile->FindComponentRef(ComponentId);
			TObjectPtr<UWidgetBlueprintBuilder> Builder = ComponentRef ? ComponentRef->GetAssetBuilder() : nullptr;

			UUserWidgetBuilder* UserWidgetBuilder = NewObject<UUserWidgetBuilder>();
			UserWidgetBuilder->SetNode(this);
			UserWidgetBuilder->SetWidgetBlueprintBuilder(Builder);

			return UserWidgetBuilder;
		}

		if(InstanceSwapBuilder)
		{
		}
		else
		{
		}
	}
}

void UFigmaInstance::ProcessChildrenComponentPropertyReferences(TObjectPtr<UWidgetBlueprint> WidgetBp, TObjectPtr<UWidget> Widget, const TArray<UFigmaNode*>& CurrentChildren) const
{
	for (UFigmaNode* Child : CurrentChildren)
	{
		if (Child->IsA<UFigmaInstance>())
			continue;

		Child->ProcessComponentPropertyReferences(WidgetBp, Widget);

		IFigmaContainer* ContainerChild = Cast<IFigmaContainer>(Child);
		if (ContainerChild)
		{
			TArray<UFigmaNode*>& SubChildren = ContainerChild->GetChildren();
			ProcessChildrenComponentPropertyReferences(WidgetBp, Widget, SubChildren);
		}
	}
}

const FFigmaComponentPropertyDefinition* UFigmaInstance::IsInstanceSwap() const
{
	if (!ParentNode)
		return nullptr;

	const FString MainComponentStr("mainComponent");
	if (!ComponentPropertyReferences.Contains(MainComponentStr))
		return nullptr;

	const FString MainComponent = ComponentPropertyReferences[MainComponentStr];
	const FFigmaComponentPropertyDefinition* PropertyDefinition = nullptr;
	if(UFigmaComponent* FigmaComponent =  Cast<UFigmaComponent>(ParentNode))
	{
		PropertyDefinition = FigmaComponent->ComponentPropertyDefinitions.Find(MainComponent);
	}
	else if (UFigmaComponentSet* FigmaComponentSet = Cast<UFigmaComponentSet>(ParentNode))
	{
		PropertyDefinition = FigmaComponentSet->ComponentPropertyDefinitions.Find(MainComponent);
	}

	if (PropertyDefinition && PropertyDefinition->Type == EFigmaComponentPropertyType::INSTANCE_SWAP)
	{
		return PropertyDefinition;
	}

	return nullptr;
}
