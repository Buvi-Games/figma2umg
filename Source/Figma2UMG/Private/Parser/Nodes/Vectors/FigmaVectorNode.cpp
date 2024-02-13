// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/Vectors/FigmaVectorNode.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
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

void UFigmaVectorNode::AddImageRequest(FString FileKey, FImageRequests& ImageRequests)
{
	ImageRequests.AddRequest(FileKey, GetNodeName(), GetId(), OnRawImageReceivedCB);
}

void UFigmaVectorNode::OnRawImageReceived(TArray<uint8>& RawData)
{
	URawTexture2DFactory* Factory = NewObject<URawTexture2DFactory>(URawTexture2DFactory::StaticClass());
	Factory->DownloadSubFolder = GetFigmaFile()->GetFileName() + TEXT("/Images");
	Factory->RawData = RawData;
	GetOrCreateAsset<UTexture2D>(Factory);
}

FString UFigmaVectorNode::GetPackagePath() const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/Textures");
}

FString UFigmaVectorNode::GetAssetName() const
{
	return GetUniqueName();
}

void UFigmaVectorNode::LoadOrCreateAssets(UFigmaFile* FigmaFile)
{
}

void UFigmaVectorNode::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	if (Builder.Image)
	{
		Function.ExecuteIfBound(*Builder.Image);
	}
}

TObjectPtr<UWidget> UFigmaVectorNode::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	Builder.Image = Cast<UImage>(WidgetToPatch);
	if (Builder.Image)
	{
		if (Builder.Image->GetName() != GetUniqueName())
		{
			Builder.Image->Rename(*GetUniqueName());
		}
	}
	else
	{
		Builder.Image = NewObject<UImage>(ParentNode->GetAssetOuter(), *GetUniqueName());
	}

	Builder.Image->SetBrushFromTexture(GetAsset<UTexture2D>(), true);
	return Builder.Image;
}

void UFigmaVectorNode::PostInsert() const
{
	TObjectPtr<UWidget> TopWidget = GetTopWidget();
	if (!TopWidget)
		return;

	IWidgetOwner::PostInsert();

	if (UCanvasPanelSlot* CanvasSlot = TopWidget->Slot ? Cast<UCanvasPanelSlot>(TopWidget->Slot) : nullptr)
	{
		CanvasSlot->SetSize(AbsoluteBoundingBox.GetSize());
	}
}

void UFigmaVectorNode::Reset()
{
	Builder.Reset();
}

TObjectPtr<UWidget> UFigmaVectorNode::GetTopWidget() const
{
	return Builder.Image;
}

FVector2D UFigmaVectorNode::GetTopWidgetPosition() const
{
	return GetPosition();
}

TObjectPtr<UPanelWidget> UFigmaVectorNode::GetContainerWidget() const
{
	return nullptr;
}
