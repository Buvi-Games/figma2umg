// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Interfaces/IHttpRequest.h"

#include "FigmaRequest.generated.h"

UINTERFACE(BlueprintType, Experimental, meta = (CannotImplementInterfaceInBlueprint))
class FIGMA2UMG_API UFigmaRequest : public UInterface
{
	GENERATED_BODY()
};


class FIGMA2UMG_API IFigmaRequest
{
	GENERATED_BODY()
public:
	void StartDownload();

	eRequestStatus GetStatus() const { return Status; }
private:
	void HandleFigmaDownload(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

protected:
	virtual void HandleFigmaDownload(const TArray<uint8>& RawData, const TSharedRef<FJsonObject>& JsonObject) = 0;

	FString AccessToken;
	FString URL;

	eRequestStatus Status = eRequestStatus::NotStarted;
};