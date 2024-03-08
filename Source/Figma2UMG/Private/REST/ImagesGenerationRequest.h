// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Interfaces/IHttpRequest.h"
#include "Parser/ImagesRequestResult.h"
#include "ImagesGenerationRequest.generated.h"


DECLARE_DELEGATE_TwoParams(FOnImageGenerationRequestCompleteDelegate, bool, const FImagesRequestResult&);

USTRUCT()
struct FImagesGenerationRequest
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString ImageName;

	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString URL;

	void StartDownload(const FOnImageGenerationRequestCompleteDelegate& Delegate);

	eRequestStatus GetStatus() const { return Status; }
private:
	void HandleFigmaDownload(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FOnImageGenerationRequestCompleteDelegate OnImageRequestCompleteDelegate;

	eRequestStatus Status = eRequestStatus::NotStarted;
};

