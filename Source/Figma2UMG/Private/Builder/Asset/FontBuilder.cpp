// Copyright 2024 Buvi Games. All Rights Reserved.


#include "FontBuilder.h"

#include "AssetToolsModule.h"
#include "Figma2UMGModule.h"
#include "FigmaImportSubsystem.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Font.h"
#include "Factories/FontFactory.h"
#include "Parser/Nodes/FigmaNode.h"

void UFontBuilder::LoadOrCreateAssets()
{
	LoadAssets();
	if (Asset == nullptr)
	{
		UFontFactory* Factory = NewObject<UFontFactory>(UFontFactory::StaticClass());
		UFont* FontAsset = nullptr;
		const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this));
		const FString AssetName = ObjectTools::SanitizeInvalidChars(Node->GetUAssetName(), INVALID_OBJECTNAME_CHARACTERS);
		const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

		UClass* AssetClass = UFont::StaticClass();

		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
		FontAsset = Cast<UFont>(AssetData.FastGetAsset(true));

		if (FontAsset == nullptr)
		{
			static const FName NAME_AssetTools = "AssetTools";
			IAssetTools* AssetTools = &FModuleManager::GetModuleChecked<FAssetToolsModule>(NAME_AssetTools).Get();
			UE_LOG_Figma2UMG(Display, TEXT("Create UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
			FontAsset = Cast<UFont>(AssetTools->CreateAsset(AssetName, PackagePath, AssetClass, Factory, FName("Figma2UMG")));
		}
		else
		{
			UPackage* Pkg = CreatePackage(*PackagePath);
			const EObjectFlags Flags = RF_Public | RF_Standalone | RF_Transactional;
			UE_LOG_Figma2UMG(Display, TEXT("Reimport UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
			FontAsset = Cast<UFont>(Factory->FactoryCreateNew(AssetClass, Pkg, *AssetName, Flags, nullptr, GWarn));
			if (FontAsset)
			{
				FAssetRegistryModule::AssetCreated(FontAsset);
			}
		}

		Asset = FontAsset;
		if (FontAsset)
		{
			FontAsset->SetFlags(RF_Transactional);
			FontAsset->Modify();
			UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
			if (Importer)
			{
				Importer->AddNewFont(Asset);
			}
		}
	}
}

void UFontBuilder::LoadAssets()
{
	const UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
	Asset = Importer ? Importer->FindFontAssetFromFamily(FontFamily) : nullptr;
}

void UFontBuilder::Reset()
{
	Asset = nullptr;
	Faces.Reset();
	if (OnRawFontReceivedCB.IsBound())
	{
		OnRawFontReceivedCB.Unbind();
	}
}

const TObjectPtr<UFont>& UFontBuilder::GetAsset() const
{
	return Asset;
}

UPackage* UFontBuilder::GetAssetPackage() const
{
	return Asset ? Asset->GetPackage() : nullptr;
}

void UFontBuilder::AddPackages(TArray<UPackage*>& Packages) const
{
	IAssetBuilder::AddPackages(Packages);
	for(UFontFace* FontFace : Faces)
	{
		if (FontFace)
		{
			Packages.AddUnique(FontFace->GetPackage());
		}
	}
}

void UFontBuilder::SetFontFamily(const FString& InFontFamily)
{
	FontFamily = InFontFamily;
}

void UFontBuilder::AddFontRequest(FFontRequests& FontRequests)
{
	LoadAssets();
	if(Asset == nullptr)
	{
		OnRawFontReceivedCB.BindUObject(this, &UFontBuilder::OnRawFontFileReceived);
		FontRequests.AddRequest(FontFamily, OnRawFontReceivedCB);
	}
}

void UFontBuilder::OnRawFontFileReceived(const FString& Variant, const TArray<uint8>& InRawData)
{
	const FString FullFilename = FPaths::ProjectContentDir() + TEXT("../Downloads/Fonts/") + FontFamily + TEXT("/") + Variant + TEXT(".ttf");
	FFileHelper::SaveArrayToFile(InRawData, *FullFilename);

	FacesRawData.Add(Variant, InRawData);
}
