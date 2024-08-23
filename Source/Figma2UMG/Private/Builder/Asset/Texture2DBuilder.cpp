// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Texture2DBuilder.h"

#include "AssetToolsModule.h"
#include "Figma2UMGModule.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Factory/RawTexture2DFactory.h"
#include "Parser/FigmaFile.h"
#include "Parser/Nodes/FigmaInstance.h"
#include "Parser/Nodes/FigmaNode.h"
#include "Parser/Nodes/FigmaSection.h"
#include "Parser/Nodes/Vectors/FigmaText.h"
#include "Parser/Nodes/Vectors/FigmaVectorNode.h"

void UTexture2DBuilder::LoadOrCreateAssets()
{
	URawTexture2DFactory* Factory = NewObject<URawTexture2DFactory>(URawTexture2DFactory::StaticClass());
	Factory->DownloadSubFolder = Node->GetFigmaFile()->GetFileName() + TEXT("/Images");
	Factory->RawData = RawData;

	UTexture2D* TextureAsset = Cast<UTexture2D>(Asset);
	if (TextureAsset == nullptr)
	{
		const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this));
		const FString AssetName = ObjectTools::SanitizeInvalidChars(Node->GetUAssetName(), INVALID_OBJECTNAME_CHARACTERS);
		const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

		UClass* AssetClass = UTexture2D::StaticClass();
		if (Factory == nullptr)
		{
			Factory = NewObject<URawTexture2DFactory>(URawTexture2DFactory::StaticClass());
		}

		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
		TextureAsset = Cast<UTexture2D>(AssetData.FastGetAsset(true));

		if (TextureAsset == nullptr)
		{
			static const FName NAME_AssetTools = "AssetTools";
			IAssetTools* AssetTools = &FModuleManager::GetModuleChecked<FAssetToolsModule>(NAME_AssetTools).Get();
			UE_LOG_Figma2UMG(Display, TEXT("Create UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
			TextureAsset = Cast<UTexture2D>(AssetTools->CreateAsset(AssetName, PackagePath, AssetClass, Factory, FName("Figma2UMG")));
		}
		else
		{
			UPackage* Pkg = CreatePackage(*PackagePath);
			const EObjectFlags Flags = RF_Public | RF_Standalone | RF_Transactional;
			UE_LOG_Figma2UMG(Display, TEXT("Reimport UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
			TextureAsset = Cast<UTexture2D>(Factory->FactoryCreateNew(AssetClass, Pkg, *AssetName, Flags, nullptr, GWarn));
			if (TextureAsset)
			{
				FAssetRegistryModule::AssetCreated(TextureAsset);
			}
		}

		Asset = TextureAsset;
	}

	if (TextureAsset)
	{
		TextureAsset->SetFlags(RF_Transactional);
		TextureAsset->Modify();
	}
}

void UTexture2DBuilder::LoadAssets()
{
	const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this));
	const FString AssetName = ObjectTools::SanitizeInvalidChars(Node->GetUAssetName(), INVALID_OBJECTNAME_CHARACTERS);
	const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
	Asset = Cast<UTexture2D>(AssetData.FastGetAsset(true));
}

void UTexture2DBuilder::Reset()
{
	Asset = nullptr;
	if (OnRawImageReceivedCB.IsBound())
	{
		OnRawImageReceivedCB.Unbind();
	}
}

void UTexture2DBuilder::AddImageRequest(FImageRequests& ImageRequests)
{
	OnRawImageReceivedCB.BindUObject(this, &UTexture2DBuilder::OnRawImageReceived);
	const TArray<FFigmaPaint>* Fills = nullptr;
	if (const UFigmaGroup* Group = Cast<UFigmaGroup>(Node))
	{
		Fills = &Group->Fills;
	}
	else if (const UFigmaInstance* Instance = Cast<UFigmaInstance>(Node))
	{
		Fills = &Instance->Fills;
	}
	else if (const UFigmaSection* Section = Cast<UFigmaSection>(Node))
	{
		Fills = &Section->Fills;
	}
	else if (const UFigmaVectorNode* Vector = Cast<UFigmaVectorNode>(Node))
	{
		if (Vector->DoesSupportImageRef())
		{
			Fills = &Vector->Fills;
		}
	}
	else if (const UFigmaText* Text = Cast<UFigmaText>(Node))
	{
		Fills = &Text->Fills;
	}

	if(Fills)
	{
		for (const FFigmaPaint& Fill : *Fills)
		{
			if (Fill.Type == EPaintTypes::IMAGE && !Fill.ImageRef.IsEmpty())
			{
				ImageRequests.AddRequest(FileKey, Node->GetNodeName(), Node->GetId(), OnRawImageReceivedCB, Fill.ImageRef);
				return;
			}
		}
	}

	ImageRequests.AddRequest(FileKey, Node->GetNodeName(), Node->GetId(), OnRawImageReceivedCB);
}

void UTexture2DBuilder::OnRawImageReceived(const TArray<uint8>& InRawData)
{
	RawData = InRawData;
}

const TObjectPtr<UTexture2D>& UTexture2DBuilder::GetAsset() const
{
	return Asset;
}

UPackage* UTexture2DBuilder::GetAssetPackage() const
{
	return Asset ? Asset->GetPackage() : nullptr;
}
