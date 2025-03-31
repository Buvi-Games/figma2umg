// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaComponent.h"

#include "FigmaInstance.h"
#include "Parser/FigmaFile.h"
#include "Parser/Properties/FigmaComponentRef.h"
#include "Builder/Asset/MaterialBuilder.h"
#include "Builder/Asset/Texture2DBuilder.h"

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

	FString Suffix = "Components";
	if (Cast<UMaterialBuilder>(InAssetBuilder.GetObject()))
	{
		Suffix = "Material";
	}
	else if (Cast<UTexture2DBuilder>(InAssetBuilder.GetObject()))
	{
		Suffix = "Textures";
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + Suffix;
}

void UFigmaComponent::TryAddComponentPropertyDefinition(FString PropertyId, FFigmaComponentPropertyDefinition Definition)
{
	if (ComponentPropertyDefinitions.Contains(PropertyId))
		return;

	ComponentPropertyDefinitions.Add(PropertyId, Definition);
}

TObjectPtr<UFigmaInstance> UFigmaComponent::InstanciateFigmaComponent(const FString& InstanceID)
{
	TObjectPtr<UFigmaInstance> NewFigmaInstance = NewObject<UFigmaInstance>();
	FString NewID = "I" + InstanceID + ";" + GetId();
	NewFigmaInstance->InitializeFrom(this, NewID);
	NewFigmaInstance->ComponentId = GetId();

	return NewFigmaInstance;
}
