// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/Nodes/FigmaInstance.h"

#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Builder/WidgetBlueprintBuilder.h"
#include "Components/Image.h"
#include "Factory/RawTexture2DFactory.h"
#include "Parser/FigmaFile.h"
#include "Parser/Properties/FigmaComponentRef.h"
#include "REST/ImageRequest.h"
#include "Templates/WidgetTemplateBlueprintClass.h"

FVector2D UFigmaInstance::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

void UFigmaInstance::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	SerializeArray(Children, JsonObj,"Children");
}

void UFigmaInstance::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	if (TObjectPtr<UWidget> Widget = Cast<UWidget>(InstanceAsset))
	{
		Function.ExecuteIfBound(*Widget);
	}

	if (BuilderFallback.Image)
	{
		Function.ExecuteIfBound(*BuilderFallback.Image);
	}
}

TObjectPtr<UWidget> UFigmaInstance::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	FFigmaComponentRef* ComponentRef = FigmaFile->FindComponentRef(ComponentId);
	UWidgetBlueprint* ComponentAsset = ComponentRef ? ComponentRef->GetAsset() : nullptr;
	if (ComponentAsset)
	{
		if (WidgetToPatch && WidgetToPatch.GetClass()->ClassGeneratedBy == ComponentAsset)
		{
			TryRenameWidget(GetUniqueName(), WidgetToPatch);
			InstanceAsset = WidgetToPatch;
			return WidgetToPatch;
		}
		else if (ParentNode)
		{
			TObjectPtr<UWidgetTree> OwningObject = Cast<UWidgetTree>(ParentNode->GetAssetOuter());

			TSubclassOf<UUserWidget> UserWidgetClass = ComponentAsset->GetBlueprintClass();
			TSharedPtr<FWidgetTemplateBlueprintClass> Template = MakeShared<FWidgetTemplateBlueprintClass>(FAssetData(ComponentAsset), UserWidgetClass);
			WidgetToPatch = Template->Create(OwningObject);
			if (WidgetToPatch)
			{
				TryRenameWidget(GetUniqueName(), WidgetToPatch);
				WidgetToPatch->CreatedFromPalette();
			}

			InstanceAsset = WidgetToPatch;
			return WidgetToPatch;
		}
	}
	else if (MissingComponentTexture)
	{
		BuilderFallback.Image = Cast<UImage>(WidgetToPatch);
		if (BuilderFallback.Image)
		{
			if (BuilderFallback.Image->GetName() != GetUniqueName())
			{
				IWidgetOwner::TryRenameWidget(GetUniqueName(), BuilderFallback.Image);
			}
		}
		else
		{
			BuilderFallback.Image = IWidgetOwner::NewWidget<UImage>(ParentNode->GetAssetOuter(), *GetUniqueName());
		}

		BuilderFallback.Image->SetBrushFromTexture(GetAsset<UTexture2D>(), true);

		InstanceAsset = BuilderFallback.Image;
		return BuilderFallback.Image;
	}
	else
	{
		UE_LOG_Figma2UMG(Error, TEXT("[Patch] UFigmaInstance %s failed to patch."), *GetNodeName());
	}
	return WidgetToPatch;
}

void UFigmaInstance::SetupWidget(TObjectPtr<UWidget> Widget)
{
	if (Widget)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[SetupWidget] UFigmaInstance %s received a UWidget %s of type %s."), *GetNodeName(), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
	}

	if (!IsMissingComponent)
	{
		InstanceAsset = Widget;
		if (!InstanceAsset)
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[SetupWidget] UFigmaInstance %s received a null UWidget."), *GetNodeName());
		}
	}
	else
	{
		BuilderFallback.SetupWidget(Widget);
		InstanceAsset = BuilderFallback.Image;
	}
}

void UFigmaInstance::PostInsert() const
{
	TObjectPtr<UWidget> TopWidget = GetTopWidget();
	if (!TopWidget)
		return;

	TopWidget->bIsVariable = true;

	IWidgetOwner::PostInsert();

	SetSize(TopWidget, AbsoluteBoundingBox.GetSize(), true);
	SetPadding(GetContainerWidget(), PaddingLeft, PaddingRight, PaddingTop, PaddingBottom);
}

void UFigmaInstance::Reset()
{
	InstanceAsset = nullptr;
	BuilderFallback.Reset();
	ResetAsset();
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
	return nullptr;
}

void UFigmaInstance::AddImageRequest(FString FileKey, FImageRequests& ImageRequests)
{
	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	FFigmaComponentRef* ComponentRef = FigmaFile->FindComponentRef(ComponentId);
	UWidgetBlueprint* ComponentAsset = ComponentRef ? ComponentRef->GetAsset() : nullptr;
	IsMissingComponent = ComponentAsset == nullptr;
	if (IsMissingComponent)
	{
		//We don't have the Component Asset, import as a Texture as a PlaceHolder
		UE_LOG_Figma2UMG(Warning, TEXT("[Instance] Can't find Component %s for instance %s, import as a Texture as a PlaceHolder"), *ComponentId, *GetNodeName());
		ImageRequests.AddRequest(FileKey, GetNodeName(), GetId(), OnRawImageReceivedCB);
	}
}

void UFigmaInstance::OnRawImageReceived(TArray<uint8>& RawData)
{
	URawTexture2DFactory* Factory = NewObject<URawTexture2DFactory>(URawTexture2DFactory::StaticClass());
	Factory->DownloadSubFolder = GetFigmaFile()->GetFileName() + TEXT("/MissingComponents");
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

	return TopParentNode->GetCurrentPackagePath() + TEXT("/InstanceTextures");
}

FString UFigmaInstance::GetAssetName() const
{
	return GetUniqueName();
}

void UFigmaInstance::LoadOrCreateAssets(UFigmaFile* FigmaFile)
{
	// Don't do anything here. Need to wait for the Image stage, in case the Component is missing.
}

void UFigmaInstance::LoadAssets()
{
	if (IsMissingComponent)
	{
		MissingComponentTexture = LoadAsset<UTexture2D>();
	}
}

void UFigmaInstance::PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const
{
	if (WidgetBp == nullptr)
		return;

	if (MissingComponentTexture != nullptr)
		return;

	TObjectPtr<UWidget> Widget = Cast<UWidget>(InstanceAsset);
	ProcessComponentPropertyReferences(WidgetBp, Widget);
}

void UFigmaInstance::PatchComponentProperty() const
{
	if (MissingComponentTexture != nullptr)
		return;

	TObjectPtr<UUserWidget> Widget = Cast<UUserWidget>(InstanceAsset);
	if (Widget == nullptr)
		return;

	for (const TPair<FString, FFigmaComponentProperty>& ComponentProperty : ComponentProperties)
	{
		WidgetBlueprintBuilder::SetPropertyValue(Widget, *ComponentProperty.Key, ComponentProperty.Value);
	}
}