// Copyright 2024 Buvi Games. All Rights Reserved.


#include "FontBuilder.h"

#include "AssetToolsModule.h"
#include "Figma2UMGModule.h"
#include "FigmaImportSubsystem.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Font.h"
#include "Engine/FontFace.h"
#include "Factories/FontFactory.h"
#include "Parser/Nodes/FigmaNode.h"

void UFontBuilder::LoadOrCreateAssets()
{
	LoadAssets();
	if (Asset == nullptr)
	{
		if(FacesRawData.IsEmpty())
		{
			UE_LOG_Figma2UMG(Error, TEXT("Failed to import Font %s"), *FontFamily);
			return;
		}

		UFontFactory* Factory = NewObject<UFontFactory>(UFontFactory::StaticClass());
		const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this) + TEXT("/") + FontFamily);
		const FString FontAssetName = ObjectTools::SanitizeInvalidChars(FontFamily, INVALID_OBJECTNAME_CHARACTERS);
		const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + FontAssetName);

		UClass* AssetClass = UFont::StaticClass();

		static const FName NAME_AssetTools = "AssetTools";
		IAssetTools* AssetTools = &FModuleManager::GetModuleChecked<FAssetToolsModule>(NAME_AssetTools).Get();
		UE_LOG_Figma2UMG(Display, TEXT("Create UAsset %s/%s of type %s"), *PackagePath, *FontAssetName, *AssetClass->GetDisplayNameText().ToString());
		Asset = Cast<UFont>(AssetTools->CreateAsset(FontAssetName, PackagePath, AssetClass, Factory, FName("Figma2UMG")));

		if (Asset)
		{
			for (TPair<FString, TArray<uint8>> FaceRawData: FacesRawData)
			{
				const FString FontFaceAssetName = ObjectTools::SanitizeInvalidChars(FaceRawData.Key, INVALID_OBJECTNAME_CHARACTERS);
				const FString FontFacePackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + FontFaceAssetName);

				UPackage* FontFacePkg = CreatePackage(*FontFacePackageName);
				UFontFace* FontFace = NewObject<UFontFace>(FontFacePkg, UFontFace::StaticClass(), *FontFaceAssetName, RF_Public | RF_Standalone | RF_Transactional);
				if (FontFace)
				{
					FontFace->FontFaceData->SetData(MoveTemp(FaceRawData.Value));
					FontFace->CacheSubFaces();

					FTypefaceEntry& DefaultTypefaceEntry = Asset->CompositeFont.DefaultTypeface.Fonts.Emplace_GetRef();
					DefaultTypefaceEntry.Name = *FaceRawData.Key;
					DefaultTypefaceEntry.Font = FFontData(FontFace);

					FontFace->MarkPackageDirty();
					FontFace->PostEditChange();
					FAssetRegistryModule::AssetCreated(FontFace);

					Faces.Add(FontFace);
				}
			}

			Asset->SetFlags(RF_Transactional);
			Asset->Modify();
			FAssetRegistryModule::AssetCreated(Asset);

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
	FontFamily = UPackageTools::SanitizePackageName(InFontFamily.Replace(TEXT(" "), TEXT("")));
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
	const FString VariantName = GetVariantName(Variant);
	const FString FullFilename = FPaths::ProjectContentDir() + TEXT("../Downloads/Fonts/") + FontFamily + TEXT("/") + VariantName + TEXT(".ttf");
	FFileHelper::SaveArrayToFile(InRawData, *FullFilename);

	FacesRawData.Add(VariantName, InRawData);
}

FString UFontBuilder::GetVariantName(const FString& VariantId)
{
	static const FString Regular = FString("Regular");
	static const FString Italic = FString("Italic");
	static const FString V_100 = FString("100");
	static const FString V_200 = FString("200");
	static const FString V_300 = FString("300");
	static const FString V_400 = FString("400");
	static const FString V_500 = FString("500");
	static const FString V_600 = FString("600");
	static const FString V_700 = FString("700");
	static const FString V_800 = FString("800");
	static const FString V_900 = FString("900");
	static const FString Thin = FString("Thin");
	static const FString ExtraLight = FString("ExtraLight");
	static const FString Light = FString("Light");
	static const FString Medium = FString("Medium");
	static const FString SemiBold = FString("SemiBold");
	static const FString Bold = FString("Bold");
	static const FString ExtraBold = FString("ExtraBold");
	static const FString Black = FString("Black");

	bool Matches = false;
	FString BoldName = VariantId;
	if (VariantId.Contains(V_100))
	{
		BoldName =  Thin;
		Matches = VariantId.Equals(V_100);
	}
	else if (VariantId.Contains(V_200))
	{
		BoldName =  ExtraLight;
		Matches = VariantId.Equals(V_200);
	}
	else if (VariantId.Contains(V_300))
	{
		BoldName =  Light;
		Matches = VariantId.Equals(V_300);
	}
	else if (VariantId.Contains(Regular, ESearchCase::IgnoreCase) || VariantId.Contains(V_400))
	{
		BoldName =  Regular;
		Matches = VariantId.Equals(Regular, ESearchCase::IgnoreCase) || VariantId.Equals(V_400);
	}
	else if (VariantId.Contains(V_500))
	{
		BoldName =  Medium;
		Matches = VariantId.Equals(V_500);
	}
	else if (VariantId.Contains(V_600))
	{
		BoldName =  SemiBold;
		Matches = VariantId.Equals(V_600);
	}
	else if (VariantId.Contains(V_700))
	{
		BoldName =  Bold;
		Matches = VariantId.Equals(V_700);
	}
	else if (VariantId.Contains(V_800))
	{
		BoldName =  ExtraBold;
		Matches = VariantId.Equals(V_800);
	}
	else if (VariantId.Contains(V_900))
	{
		BoldName =  Black;
		Matches = VariantId.Equals(V_900);
	}
	else if (VariantId.Equals(Italic, ESearchCase::IgnoreCase))
	{
		return Italic;
	}
	else
	{
		UE_LOG_Figma2UMG(Display, TEXT("[GetVariantName] Unknown name for Variant %s"), *VariantId);
	}

	const bool IsItalic = VariantId.Contains(Italic, ESearchCase::IgnoreCase);
	if(!IsItalic && !Matches)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[GetVariantName] Mismatching name for Variant %s and %s"), *VariantId, *BoldName);
	}

	return IsItalic ? BoldName+Italic : BoldName;
}
