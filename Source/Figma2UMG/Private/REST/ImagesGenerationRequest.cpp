// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/ImagesGenerationRequest.h"

#include "Figma2UMGModule.h"


void UImagesGenerationRequest::Setup(const FString& InAccessToken, const FString& InURL, const FOnImageGenerationRequestCompleteDelegate& Delegate)
{
	AccessToken = InAccessToken;
	URL = InURL;
	OnImageRequestCompleteDelegate = Delegate;
}

void UImagesGenerationRequest::HandleFigmaDownload(const TArray<uint8>& RawData)
{
	FImagesRequestResult result;
	if (Status == eRequestStatus::Succeeded)
	{
		// TODO: Parse
		OnImageRequestCompleteDelegate.ExecuteIfBound(true, result);
	}
	else
	{
		UE_LOG_Figma2UMG(Warning, TEXT("Failed to download image at %s."), *URL);
		OnImageRequestCompleteDelegate.ExecuteIfBound(false, result);
	}
}