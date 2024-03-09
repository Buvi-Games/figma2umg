// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/FileRequest.h"

#include "JsonObjectConverter.h"
#include "Parser/FigmaFile.h"


void UFileRequest::Setup(const FString& InAccessToken, const FString& InURL, const FOnFileRequestCompleteDelegate& Delegate, const FString& InContentRootFolder)
{
	AccessToken = InAccessToken;
	URL = InURL;
	OnRequestCompleteDelegate = Delegate;
	ContentRootFolder = InContentRootFolder;
}

void UFileRequest::HandleFigmaDownload(const TArray<uint8>& RawData, const TSharedRef<FJsonObject>& JsonObject)
{
	UFigmaFile* File = NewObject<UFigmaFile>();

	constexpr int64 CheckFlags = 0;
	constexpr int64 SkipFlags = 0;
	constexpr bool StrictMode = false;
	FText OutFailReason;
	if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject, File->StaticClass(), File, CheckFlags, SkipFlags, StrictMode, &OutFailReason))
	{
		UE_LOG_Figma2UMG(Display, TEXT("Post-Serialize"));
		File->PostSerialize(ContentRootFolder, JsonObject);
	}
	else
	{
		UE_LOG_Figma2UMG(Error, TEXT("Failed to parse restult of %s. Fail Reason = %s"), *URL, *OutFailReason.ToString());
	}

	OnRequestCompleteDelegate.ExecuteIfBound(File, RawData);
}
