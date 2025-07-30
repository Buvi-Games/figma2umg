// MIT License
// Copyright (c) 2024 Buvi Games

#include "Builder/Asset/AssetBuilder.h"

void IAssetBuilder::SetNode(const FString& InFileKey, const UFigmaNode* InNode)
{
	FileKey = InFileKey;
	Node = InNode;
}

const UFigmaNode* IAssetBuilder::GetNode() const
{
	return Node;
}

void IAssetBuilder::AddPackages(TArray<UPackage*>& Packages) const
{
	if (UPackage* Package = GetAssetPackage())
	{
		Packages.AddUnique(Package);
	}
}
