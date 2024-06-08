// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/Vectors/FigmaVectorNode.h"

#include "Builder/Asset/Texture2DBuilder.h"
#include "Builder/Widget/ImageWidgetBuilder.h"
#include "Components/Image.h"
#include "Factory/RawTexture2DFactory.h"
#include "REST/FigmaImporter.h"

void UFigmaVectorNode::PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj)
{
	Super::PostSerialize(InParent, JsonObj);

	PostSerializeProperty(JsonObj, "fills", Fills);
	PostSerializeProperty(JsonObj, "strokes", Strokes);
}

FVector2D UFigmaVectorNode::GetAbsolutePosition() const
{
	return AbsoluteBoundingBox.GetPosition();
}

FVector2D UFigmaVectorNode::GetSize() const
{
	return AbsoluteBoundingBox.GetSize();
}

TScriptInterface<IAssetBuilder> UFigmaVectorNode::CreateAssetBuilder(const FString& InFileKey)
{
	AssetBuilder = NewObject<UTexture2DBuilder>();
	AssetBuilder->SetNode(InFileKey, this);
	return AssetBuilder;
}

FString UFigmaVectorNode::GetPackageName() const
{
	return GetPackagePath();
}

TScriptInterface<IWidgetBuilder> UFigmaVectorNode::CreateWidgetBuilders() const
{
	UImageWidgetBuilder* ImageWidgetBuilder = NewObject<UImageWidgetBuilder>();
	ImageWidgetBuilder->SetNode(this);
	ImageWidgetBuilder->SetTexture2DBuilder(AssetBuilder);

	return ImageWidgetBuilder;
}

void UFigmaVectorNode::AddImageRequest(FString FileKey, FImageRequests& ImageRequests)
{
	ImageRequests.AddRequest(FileKey, GetNodeName(), GetId(), OnRawImageReceivedCB);
}

void UFigmaVectorNode::OnRawImageReceived(const TArray<uint8>& RawData)
{
	URawTexture2DFactory* Factory = NewObject<URawTexture2DFactory>(URawTexture2DFactory::StaticClass());
	Factory->DownloadSubFolder = GetFigmaFile()->GetFileName() + TEXT("/Images");
	Factory->RawData = RawData;
	GetOrCreateTexture2D(Factory);
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

void UFigmaVectorNode::LoadOrCreateAssets()
{
}

void UFigmaVectorNode::LoadAssets()
{
	LoadAsset<UTexture2D>();
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
		UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		UClass* ClassOverride = Importer ? Importer->GetOverrideClassForNode<UImage>(GetUniqueName()) : nullptr;
		if (ClassOverride && Builder.Image->GetClass() != ClassOverride)
		{
			Builder.Image = IWidgetOwner::NewWidget<UImage>(ParentNode->GetAssetOuter(), *GetUniqueName(), ClassOverride);
		}
		IWidgetOwner::TryRenameWidget(GetUniqueName(), Builder.Image);
	}
	else
	{
		Builder.Image = IWidgetOwner::NewWidget<UImage>(ParentNode->GetAssetOuter(), *GetUniqueName());
	}

	UTexture2D* Texture = GetAsset<UTexture2D>();
	if (Texture)
	{
		Builder.Image->SetBrushFromTexture(GetAsset<UTexture2D>(), true);
	}
	else
	{
		FSlateBrush Brush = Builder.Image->GetBrush();
		const FLinearColor Color(0.0f, 0.0f, 0.0f, 0.0f);
		Brush.TintColor = Color;
		Builder.Image->SetBrush(Brush);
	}
	return Builder.Image;
}

void UFigmaVectorNode::SetupWidget(TObjectPtr<UWidget> Widget)
{
	if (Widget)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[SetupWidget] UFigmaVectorNode %s received a UWidget %s of type %s."), *GetNodeName(), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
	}

	Builder.SetupWidget(Widget);
}

void UFigmaVectorNode::PostInsert() const
{
	TObjectPtr<UWidget> TopWidget = GetTopWidget();
	if (!TopWidget)
		return;

	IWidgetOwner::PostInsert();

	SetSize(TopWidget, AbsoluteBoundingBox.GetSize());
}

void UFigmaVectorNode::Reset()
{
	Builder.Reset();
	ResetAsset();
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

void UFigmaVectorNode::PatchBinds(TObjectPtr<UWidgetBlueprint> WidgetBp) const
{
	if (WidgetBp == nullptr)
		return;

	ProcessComponentPropertyReferences(WidgetBp, Builder.Image);
}
