// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Rest/Enums.h"

#include "GFontRequest.generated.h"

DECLARE_DELEGATE_OneParam(FOnFontRequestCompleteDelegate, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRawFontFileReceive, const TArray<uint8>&);

USTRUCT()
struct FGFontFamilyInfo
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString Family;

	UPROPERTY()
	FString URL;

	UPROPERTY()
	TArray<FString> Variants;
};

USTRUCT()
struct FGFontRequest
{
	GENERATED_BODY()
public:
	FGFontFamilyInfo* FamilyInfo;

	FOnRawFontFileReceive OnFontRawReceive;

	void StartDownload(const FOnFontRequestCompleteDelegate& Delegate);

	eRequestStatus GetStatus() const { return Status; }
private:
	void HandleFontDownload(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FOnFontRequestCompleteDelegate OnFontRequestCompleteDelegate;

	eRequestStatus Status = eRequestStatus::NotStarted;
};

USTRUCT()
struct FFontRequests
{
	GENERATED_BODY()
public:
	void AddRequest(const FString& FamilyName, const FOnRawFontFileReceive::FDelegate& OnImageRawReceive);
	void Reset();

	FGFontRequest* GetNextToDownload();
	int GetRequestTotalCount();

private:
	UPROPERTY()
	TArray<FGFontRequest> RequestedFamilies;
};
