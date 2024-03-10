// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/ImagesGenerationRequest.h"

#include "Figma2UMGModule.h"
#include "JsonObjectConverter.h"


void UImagesGenerationRequest::Setup(const FString& InAccessToken, const FString& InURL, const FOnImageGenerationRequestCompleteDelegate& Delegate)
{
	AccessToken = InAccessToken;
	URL = InURL;
	OnImageRequestCompleteDelegate = Delegate;
}

void UImagesGenerationRequest::HandleFigmaDownload(const TArray<uint8>& RawData, const TSharedRef<FJsonObject>& JsonObject)
{
	FImagesRequestResult result;
	if (Status == eRequestStatus::Succeeded)
	{
		constexpr int64 CheckFlags = 0;
		constexpr int64 SkipFlags = 0;
		constexpr bool StrictMode = false;
		FText OutFailReason;
		if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject, FImagesRequestResult::StaticStruct(), &result, CheckFlags, SkipFlags, StrictMode, &OutFailReason))
		{
			UE_LOG_Figma2UMG(Display, TEXT("Post-Serialize"));
			OnImageRequestCompleteDelegate.ExecuteIfBound(true, result);
		}
		else
		{
			UE_LOG_Figma2UMG(Error, TEXT("Failed to parse restult of %s. Fail Reason = %s"), *URL, *OutFailReason.ToString());
			OnImageRequestCompleteDelegate.ExecuteIfBound(false, result);
		}
	}
	else
	{
		UE_LOG_Figma2UMG(Warning, TEXT("Failed to download image at %s."), *URL);
		OnImageRequestCompleteDelegate.ExecuteIfBound(false, result);
	}
}