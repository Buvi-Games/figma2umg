// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "REST/ImageRequest.h"

#include "FigmaImageRequester.generated.h"

struct FImageRequests;

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UFigmaImageRequester : public UInterface
{
	GENERATED_BODY()
	
};

class FIGMA2UMG_API IFigmaImageRequester
{
	GENERATED_BODY()
public:
	IFigmaImageRequester();

	virtual void AddImageRequest(FString FileKey, FImageRequests& ImageRequests) = 0;
	virtual void OnRawImageReceived(const TArray<uint8>& RawData) = 0;

protected:

	FOnRawImageReceiveDelegate OnRawImageReceivedCB;

private:
	void OnRawImageReceivedBase(const TArray<uint8>& RawData);
};
