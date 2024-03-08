// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Interfaces/IHttpRequest.h"
#include "FileRequest.generated.h"

class UFigmaFile;

DECLARE_DELEGATE_TwoParams(FOnFileRequestCompleteDelegate, TObjectPtr<UFigmaFile>, const TArray<uint8>&);

USTRUCT()
struct FFileRequest
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString ImageName;

	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString URL;

	void StartDownload(const FOnFileRequestCompleteDelegate& Delegate);

	eRequestStatus GetStatus() const { return Status; }
private:
	void HandleFigmaDownload(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FOnFileRequestCompleteDelegate OnImageRequestCompleteDelegate;

	eRequestStatus Status = eRequestStatus::NotStarted;
};

