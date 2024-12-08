// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/Vectors/FigmaVectorNode.h"

#include "Builder/Asset/MaterialBuilder.h"
#include "Builder/Asset/Texture2DBuilder.h"
#include "Builder/Widget/ImageWidgetBuilder.h"
#include "Components/Image.h"
#include "Factory/RawTexture2DFactory.h"
#include "Parser/Properties/FigmaAction.h"
#include "Parser/Properties/FigmaTrigger.h"
#include "REST/FigmaImporter.h"

void UFigmaVectorNode::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	PostSerializeProperty(JsonObj, "fills", Fills);
	PostSerializeProperty(JsonObj, "strokes", Strokes);
}

FVector2D UFigmaVectorNode::GetAbsolutePosition() const
{
	return AbsoluteRenderBounds.GetPosition();
}

FVector2D UFigmaVectorNode::GetAbsoluteSize() const
{
	return AbsoluteRenderBounds.GetSize();
}

bool UFigmaVectorNode::CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders)
{
	AssetBuilder = NewObject<UTexture2DBuilder>();
	AssetBuilder->SetNode(InFileKey, this);
	AssetBuilders.Add(AssetBuilder);

	if (DoesSupportImageRef())
	{
		CreatePaintAssetBuilderIfNeeded(InFileKey, AssetBuilders, Fills, Strokes);
	}

	return true;
}

FString UFigmaVectorNode::GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	FString Suffix = "Textures";
	if (Cast<UMaterialBuilder>(InAssetBuilder.GetObject()))
	{
		Suffix = "Material";
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + Suffix;
}

TScriptInterface<IWidgetBuilder> UFigmaVectorNode::CreateWidgetBuilders(bool IsRoot/*= false*/, bool AllowFrameButton/*= true*/) const
{
	UImageWidgetBuilder* ImageWidgetBuilder = NewObject<UImageWidgetBuilder>();
	ImageWidgetBuilder->SetNode(this);
	ImageWidgetBuilder->SetTexture2DBuilder(AssetBuilder);

	return ImageWidgetBuilder;
}

const FFigmaInteraction& UFigmaVectorNode::GetInteractionFromTrigger(const EFigmaTriggerType TriggerType) const
{
	return UFigmaNode::GetInteractionFromTrigger(Interactions, TriggerType);
}

const FFigmaInteraction& UFigmaVectorNode::GetInteractionFromAction(const EFigmaActionType ActionType, const EFigmaActionNodeNavigation Navigation) const
{
	return UFigmaNode::GetInteractionFromAction(Interactions, ActionType, Navigation);
}

const FString& UFigmaVectorNode::GetDestinationIdFromEvent(const FName& EventName) const
{
	const FFigmaInteraction& Interaction = GetInteractionFromAction(EFigmaActionType::NODE, EFigmaActionNodeNavigation::NAVIGATE);
	if (!Interaction.Trigger || !Interaction.Trigger->MatchEvent(EventName.ToString()))
		return TransitionNodeID;

	const UFigmaNodeAction* Action = Interaction.FindActionNode(EFigmaActionNodeNavigation::NAVIGATE);
	if (!Action || Action->DestinationId.IsEmpty())
		return TransitionNodeID;

	return Action->DestinationId;
}

bool UFigmaVectorNode::DoesSupportImageRef() const
{
	return false;
}
