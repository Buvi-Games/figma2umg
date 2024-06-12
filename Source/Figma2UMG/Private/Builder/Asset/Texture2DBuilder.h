// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetBuilder.h"
#include "REST/ImageRequest.h"
#include "Texture2DBuilder.generated.h"

UCLASS()
class UTexture2DBuilder : public UObject, public IAssetBuilder
{
	GENERATED_BODY()
public:
	virtual void LoadOrCreateAssets() override;
	virtual void LoadAssets() override;

	void AddImageRequest(FImageRequests& ImageRequests);
	void OnRawImageReceived(const TArray<uint8>& InRawData);

	const TObjectPtr<UTexture2D>& GetAsset() const;

	virtual UPackage* GetAssetPackage() const override;
protected:
	UPROPERTY()
	TObjectPtr<UTexture2D> Asset = nullptr;

	TArray<uint8> RawData;
	FOnRawImageReceiveDelegate OnRawImageReceivedCB;
};
