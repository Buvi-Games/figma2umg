// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "FigmaRequest.h"
#include "Interfaces/IHttpRequest.h"
#include "FileRequest.generated.h"

class UFigmaFile;

DECLARE_DELEGATE_TwoParams(FOnFileRequestCompleteDelegate, TObjectPtr<UFigmaFile>, const TArray<uint8>&);

UCLASS()
class UFileRequest : public UObject, public IFigmaRequest
{
	GENERATED_BODY()
public:
	void Setup(const FString& InAccessToken, const FString& InURL, const FOnFileRequestCompleteDelegate& Delegate, const FString& InContentRootFolder);

protected:
	virtual void HandleFigmaDownload(const TArray<uint8>& RawData, const TSharedRef<FJsonObject>& JsonObject) override;

	FString ContentRootFolder;
	FOnFileRequestCompleteDelegate OnRequestCompleteDelegate;
};

