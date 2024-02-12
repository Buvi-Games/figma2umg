// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/FigmaInstance.h"

#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Factories/Texture2dFactoryNew.h"
#include "Factory/RawTexture2DFactory.h"
#include "Parser/FigmaFile.h"
#include "Parser/Properties/FigmaComponentRef.h"
#include "REST/FigmaImporter.h"
#include "REST/ImageRequest.h"
#include "Templates/WidgetTemplateBlueprintClass.h"

FVector2D UFigmaInstance::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

void UFigmaInstance::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	if (TObjectPtr<UWidget> Widget = GetTopWidget())
	{
		Function.ExecuteIfBound(*Widget);
	}
}

TObjectPtr<UWidget> UFigmaInstance::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	FFigmaComponentRef* ComponentRef = FigmaFile->FindComponentRef(ComponentId);
	UWidgetBlueprint* ComponentAsset = ComponentRef ? ComponentRef->GetAsset() : nullptr;
	if (ComponentAsset)
	{
		if (WidgetToPatch)
		{
			//TODO: Check if it's the correct Template
			return WidgetToPatch;
		}
		else if (ParentNode)
		{
			TObjectPtr<UWidgetTree> OwningObject = Cast<UWidgetTree>(ParentNode->GetAssetOuter());
			TSubclassOf<UUserWidget> UserWidgetClass = ComponentAsset->GetBlueprintClass();

			TSharedPtr<FWidgetTemplateBlueprintClass> Template = MakeShared<FWidgetTemplateBlueprintClass>(FAssetData(ComponentAsset), UserWidgetClass);
			UWidget* NewWidget = Template->Create(OwningObject);

			if (NewWidget)
			{
				//if (NewWidget->GetName() != GetUniqueName())
				//{
				//	NewWidget->Rename(*GetUniqueName());
				//}
				NewWidget->CreatedFromPalette();
			}

			InstanceAsset = NewWidget;
			return NewWidget;
		}
	}
	else if (MissingComponentTexture)
	{
		//Todo Add image
	}
	return WidgetToPatch;
}

void UFigmaInstance::PostInsert() const
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

void UFigmaInstance::Reset()
{
	if (InstanceAsset)
	{
		InstanceAsset = nullptr;
	}
}

TObjectPtr<UWidget> UFigmaInstance::GetTopWidget() const
{
	return Cast<UWidget>(InstanceAsset);
}

FVector2D UFigmaInstance::GetTopWidgetPosition() const
{
	return GetPosition();
}

TObjectPtr<UPanelWidget> UFigmaInstance::GetContainerWidget() const
{
	return Builder.Canvas;
}

void UFigmaInstance::AddImageRequest(FImageRequests& ImageRequests)
{
	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	FFigmaComponentRef* ComponentRef = FigmaFile->FindComponentRef(ComponentId);
	UWidgetBlueprint* ComponentAsset = ComponentRef ? ComponentRef->GetAsset() : nullptr;
	if (ComponentAsset == nullptr)
	{
		//We don't have the Component Asset, import as a Texture as a PlaceHolder
		ImageRequests.AddRequest(GetNodeName(), GetId(), OnRawImageReceivedCB);
	}
}

void UFigmaInstance::OnRawImageReceived(TArray<uint8>& RawData)
{
	URawTexture2DFactory* Factory = NewObject<URawTexture2DFactory>(URawTexture2DFactory::StaticClass());
	Factory->DownloadSubFolder = GetFigmaFile()->GetFileName();
	Factory->RawData = RawData;
	MissingComponentTexture = GetOrCreateAsset<UTexture2D>(Factory);
}

FString UFigmaInstance::GetPackagePath() const
{
	TObjectPtr<UFigmaNode> TopParentNode = ParentNode;
	while (TopParentNode && TopParentNode->GetParentNode())
	{
		TopParentNode = TopParentNode->GetParentNode();
	}

	return TopParentNode->GetCurrentPackagePath() + TEXT("/") + "InstanceTextures";
}

FString UFigmaInstance::GetAssetName() const
{
	return GetUniqueName();
}

void UFigmaInstance::LoadOrCreateAssets()
{
	// Don't do anything here. Need to wait for the Image stage, in case the Component is missing.
}
