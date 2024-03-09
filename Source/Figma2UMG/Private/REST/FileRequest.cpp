// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/FileRequest.h"

#include "Parser/FigmaFile.h"


void UFileRequest::Setup(const FString& InAccessToken, const FString& InURL, const FOnFileRequestCompleteDelegate& Delegate)
{
	AccessToken = InAccessToken;
	URL = InURL;
	OnRequestCompleteDelegate = Delegate;
}

void UFileRequest::HandleFigmaDownload(const TArray<uint8>& RawData)
{
	UFigmaFile* File = NewObject<UFigmaFile>();
	// TODO: Parse
	OnRequestCompleteDelegate.ExecuteIfBound(File, RawData);
}
