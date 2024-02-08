// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VaRestSubsystem.h"

#include "FigmaImporter.generated.h"

class UFigmaFile;

enum class eRequestStatus
{
	NotStarted,
	Processing,
	Succeeded,
	Failed,
};

DECLARE_DELEGATE_TwoParams(FOnFigmaImportUpdateStatusCB, eRequestStatus, FString);

UCLASS()
class UFigmaImporter : public UObject
{
	GENERATED_BODY()
public:
	UFigmaImporter(const FObjectInitializer& ObjectInitializer);

	void Init(const FString& InAccessToken, const FString& InFileKey, const FString& InIds, const FString& InContentRootFolder, const FOnFigmaImportUpdateStatusCB& InRequesterCallback);
	void Run();

protected:
	bool CreateRequest(const char* EndPoint, const FString& RequestIds, const FVaRestCallDelegate& VaRestCallDelegate);
	void UpdateStatus(eRequestStatus Status, FString Message);

	void OnCurrentRequestComplete(UVaRestRequestJSON* Request);
	void OnCurrentRequestFail(UVaRestRequestJSON* Request);

	UFUNCTION()
	void OnFigmaFileRequestReceived(UVaRestRequestJSON* Request);

	FVaRestCallDelegate OnVaRestFileRequestDelegate;

	FVaRestCallResponse Response;

	FString AccessToken;
	FString FileKey;
	FString Ids;

	FString ContentRootFolder;

	FOnFigmaImportUpdateStatusCB RequesterCallback;

	UPROPERTY()
	TObjectPtr<UFigmaFile> File = nullptr;
};
