// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Figma2UMGModule.h"
#include "PackageTools.h"
#include "ObjectTools.h"

#include "AssetFileHandler.generated.h"

class UWidgetBlueprint;

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UFigmaFileHandle : public UInterface
{
	GENERATED_BODY()
};

class FIGMA2UMG_API IFigmaFileHandle
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual FString GetPackagePath() const = 0;

	UFUNCTION()
	virtual FString GetAssetName() const = 0;

	UFUNCTION()
	virtual void LoadOrCreateAssets(UFigmaFile* FigmaFile) = 0;

	UObject* GetAsset() const { return Asset; }
	template<class Type>
	Type* GetAsset() const { return Cast<Type>(Asset); }
	UObject* GetOuter() const { return AssetOuter; }

	void Reset();
protected:
	template<class AssetType, class FactoryType>
	AssetType* GetOrCreateAsset(FactoryType* Factory = nullptr);

	UObject* Asset = nullptr;
	UObject* AssetOuter = nullptr;
};

template <class AssetType, class FactoryType>
AssetType* IFigmaFileHandle::GetOrCreateAsset(FactoryType* Factory)
{
	AssetType* TypedAsset = Cast<AssetType>(Asset);
	if (TypedAsset == nullptr)
	{
		const FString PackagePath = UPackageTools::SanitizePackageName(GetPackagePath());
		const FString AssetName = ObjectTools::SanitizeInvalidChars(GetAssetName(), INVALID_OBJECTNAME_CHARACTERS);
		const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
		TypedAsset = Cast<AssetType>(AssetData.FastGetAsset(true));

		if (TypedAsset == nullptr)
		{
			static const FName NAME_AssetTools = "AssetTools";
			IAssetTools* AssetTools = &FModuleManager::GetModuleChecked<FAssetToolsModule>(NAME_AssetTools).Get();
			UClass* AssetClass = AssetType::StaticClass();
			if (Factory == nullptr)
			{
				Factory = NewObject<FactoryType>(FactoryType::StaticClass());
			}

			UE_LOG_Figma2UMG(Display, TEXT("Create UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
			TypedAsset = Cast<AssetType>(AssetTools->CreateAsset(AssetName, PackagePath, AssetClass, Factory, FName("Figma2UMG")));
		}

		Asset = TypedAsset;
		AssetOuter = TypedAsset;
	}

	TypedAsset->SetFlags(RF_Transactional);
	TypedAsset->Modify();

	return TypedAsset;
}

