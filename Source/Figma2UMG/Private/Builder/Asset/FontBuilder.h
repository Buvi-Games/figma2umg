// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetBuilder.h"
#include "REST/Gfonts/GFontRequest.h"
#include "FontBuilder.generated.h"

struct FFontRequests;
class UFont;
class UFontFace;

UCLASS()
class UFontBuilder : public UObject, public IAssetBuilder
{
	GENERATED_BODY()
public:
	virtual void LoadOrCreateAssets() override;
	virtual void LoadAssets() override;
	virtual void Reset() override;

	const TObjectPtr<UFont>& GetAsset() const;

	virtual UPackage* GetAssetPackage() const override;

	void SetFontFamily(const FString& InFontFamily);

	void AddFontRequest(FFontRequests& FontRequests);
	void OnRawFontFileReceived(const TArray<uint8>& InRawData);

protected:
	UPROPERTY()
	FString FontFamily;

	UPROPERTY()
	TObjectPtr<UFont> Asset = nullptr;

	UPROPERTY()
	TArray<UFontFace*> Faces;

	TArray<uint8> RawData;
	FOnRawFontFileReceive::FDelegate OnRawFontReceivedCB;
};