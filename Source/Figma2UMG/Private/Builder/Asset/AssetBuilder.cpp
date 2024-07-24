// Copyright 2024 Buvi Games. All Rights Reserved.


#include "AssetBuilder.h"

void IAssetBuilder::SetNode(const FString& InFileKey, const UFigmaNode* InNode)
{
	FileKey = InFileKey;
	Node = InNode;
}

void IAssetBuilder::AddPackages(TArray<UPackage*>& Packages) const
{
	if (UPackage* Package = GetAssetPackage())
	{
		Packages.AddUnique(Package);
	}
}
