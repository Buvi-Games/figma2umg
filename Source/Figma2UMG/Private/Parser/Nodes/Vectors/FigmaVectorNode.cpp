// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/Vectors/FigmaVectorNode.h"

#include "Factories/Texture2dFactoryNew.h"
#include "Factory/RawTexture2DFactory.h"
#include "REST/FigmaImporter.h"

FVector2D UFigmaVectorNode::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

FVector2D UFigmaVectorNode::GetSize() const
{
	return AbsoluteBoundingBox.GetSize();
}

void UFigmaVectorNode::AddImageRequest(FImageRequests& ImageRequests)
{
	ImageRequests.AddRequest(GetNodeName(), GetId(), OnRawImageReceivedCB);
}

void UFigmaVectorNode::OnRawImageReceived(TArray<uint8>& RawData)
{
	URawTexture2DFactory* Factory = NewObject<URawTexture2DFactory>(URawTexture2DFactory::StaticClass());
	Factory->RawData = RawData;
	GetOrCreateAsset<UTexture>(Factory);
}

FString UFigmaVectorNode::GetPackagePath() const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "Textures";
}

FString UFigmaVectorNode::GetAssetName() const
{
	return GetUniqueName();
}

void UFigmaVectorNode::LoadOrCreateAssets()
{
}