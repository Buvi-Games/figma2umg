// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "FigmaRequest.h"
#include "Interfaces/IHttpRequest.h"
#include "Parser/ImagesRequestResult.h"
#include "ImagesGenerationRequest.generated.h"


DECLARE_DELEGATE_TwoParams(FOnImageGenerationRequestCompleteDelegate, bool, const FImagesRequestResult&);

UCLASS()
class UImagesGenerationRequest : public UObject, public IFigmaRequest
{
	GENERATED_BODY()
public:
	void Setup(const FString& InAccessToken, const FString& InURL, const FOnImageGenerationRequestCompleteDelegate& Delegate);

protected:
	virtual void HandleFigmaDownload(const TArray<uint8>& RawData) override;

	FOnImageGenerationRequestCompleteDelegate OnImageRequestCompleteDelegate;
};

