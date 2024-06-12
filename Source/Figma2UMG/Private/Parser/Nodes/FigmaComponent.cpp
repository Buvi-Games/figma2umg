// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaComponent.h"

#include "FigmaInstance.h"
#include "Parser/FigmaFile.h"
#include "Parser/Properties/FigmaComponentRef.h"

void UFigmaComponent::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);
	GenerateFile = true;

	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	FFigmaComponentRef* ComponentRef = FigmaFile->FindComponentRef(GetId());
	ComponentRef->SetComponent(this);
}

FString UFigmaComponent::GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "Components";
}

void UFigmaComponent::TryAddComponentPropertyDefinition(FString PropertyId, FFigmaComponentPropertyDefinition Definition)
{
	if (ComponentPropertyDefinitions.Contains(PropertyId))
		return;

	ComponentPropertyDefinitions.Add(PropertyId, Definition);
}
