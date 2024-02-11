// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Interfaces/IHttpRequest.h"
#include "ImageRequest.generated.h"


DECLARE_DELEGATE_OneParam(FOnImageRequestCompleteDelegate, bool);
DECLARE_DELEGATE_OneParam(FOnRawImageReceiveDelegate, TArray<uint8>& );

USTRUCT()
struct FImageRequest
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString ImageName;

	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString URL;

	FOnRawImageReceiveDelegate OnImageRawReceive;

	void StartDownload(const FOnImageRequestCompleteDelegate& Delegate);

	eRequestStatus GetStatus() const { return Status; }
private:
	void HandleImageDownload(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FOnImageRequestCompleteDelegate OnImageRequestCompleteDelegate;

	eRequestStatus Status = eRequestStatus::NotStarted;
};

USTRUCT()
struct FImageRequests
{
	GENERATED_BODY()
public:
	void AddRequest(const FString& ImageName, const FString& Id, const FOnRawImageReceiveDelegate& OnImageRawReceive);
	void Reset();

	const TArray<FImageRequest>& GetRequests() const { return Requests; }
	void SetURL(const FString& Id, const FString& URL);

	FImageRequest* GetNextToDownload();

private:
	UPROPERTY()
	TArray<FImageRequest> Requests;
};
