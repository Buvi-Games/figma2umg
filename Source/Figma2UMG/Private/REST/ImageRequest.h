// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Interfaces/IHttpRequest.h"
#include "ImageRequest.generated.h"


DECLARE_DELEGATE_OneParam(FOnImageRequestCompleteDelegate, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRawImageReceive, const TArray<uint8>& );

USTRUCT()
struct FIGMA2UMG_API FImageRequest
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString ImageName;

	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString ImageRef;

	UPROPERTY()
	FString URL;

	FOnRawImageReceive OnImageRawReceive;

	void SetRequestedURL();
	bool GetRequestedURL() const;

	void StartDownload(const FOnImageRequestCompleteDelegate& Delegate);

	eRequestStatus GetStatus() const { return Status; }
private:
	void HandleImageDownload(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FOnImageRequestCompleteDelegate OnImageRequestCompleteDelegate;

	eRequestStatus Status = eRequestStatus::NotStarted;
	bool RequestedURL = false;
};

USTRUCT()
struct FIGMA2UMG_API FImagePerFileRequests
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString FileKey;

	UPROPERTY()
	bool ImageRefRequested = false;

	UPROPERTY()
	TArray<FImageRequest> Requests;
};

USTRUCT()
struct FIGMA2UMG_API FImageRequests
{
	GENERATED_BODY()
public:
	void AddFile(FString FileKey);
	void AddRequest(FString FileKey, const FString& ImageName, const FString& Id, const FOnRawImageReceive::FDelegate& OnImageRawReceive, const FString& ImageRef = FString());
	void Reset();

	FImagePerFileRequests* GetNextImageRefFile();

	FImagePerFileRequests* GetRequestsPendingURL();
	void SetURLFromImageRef(const FString& ImageRef, const FString& URL);
	void SetURL(const FString& Id, const FString& URL);

	FImageRequest* GetNextToDownload();
	int GetCurrentRequestTotalCount() const;	

private:
	UPROPERTY()
	TArray<FImagePerFileRequests> RequestsPerFile;
};
