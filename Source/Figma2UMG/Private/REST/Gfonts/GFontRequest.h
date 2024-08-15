// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Rest/Enums.h"

#include "GFontRequest.generated.h"

DECLARE_DELEGATE_OneParam(FOnFontRequestCompleteDelegate, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRawFontFileReceive, const FString&, const TArray<uint8>&);

USTRUCT()
struct FIGMA2UMG_API FGFontFamilyInfo
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString Family;

	UPROPERTY()
	TArray<FString> Variants;

	UPROPERTY()
	TArray<FString> Subsets;

	UPROPERTY()
	FString Version;

	UPROPERTY()
	TMap<FString, FString> Files;

	UPROPERTY()
	FString Category;

	UPROPERTY()
	FString Kind;

	UPROPERTY()
	FString Menu;

};

USTRUCT()
struct FIGMA2UMG_API FGFontRequest
{
	GENERATED_BODY()
public:
	FGFontFamilyInfo* FamilyInfo;
	FString Variant;

	FOnRawFontFileReceive OnFontRawReceive;

	void StartDownload(const FOnFontRequestCompleteDelegate& Delegate);

	FString GetURL() const;
	eRequestStatus GetStatus() const { return Status; }
private:
	void HandleFontDownload(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FOnFontRequestCompleteDelegate OnFontRequestCompleteDelegate;

	eRequestStatus Status = eRequestStatus::NotStarted;
};

USTRUCT()
struct FIGMA2UMG_API FFontRequests
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
