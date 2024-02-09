// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VaRestSubsystem.h"
#include "Parser/FigmaFile.h"
#include "Parser/ImagesRequestResult.h"

#include "FigmaImporter.generated.h"

class URequestParams;
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

	void Init(const TObjectPtr<URequestParams> InProperties, const FOnFigmaImportUpdateStatusCB& InRequesterCallback);
	void Run();

protected:
	bool CreateRequest(const char* EndPoint, const FString& RequestIds, const FVaRestCallDelegate& VaRestCallDelegate);
	void UpdateStatus(eRequestStatus Status, FString Message);

	void OnCurrentRequestComplete(UVaRestRequestJSON* Request);
	void OnCurrentRequestFail(UVaRestRequestJSON* Request);

	UFUNCTION()
	bool ParseRequestReceived(FString MessagePrefix, UVaRestRequestJSON* Request);

	UFUNCTION()
	void OnFigmaFileRequestReceived(UVaRestRequestJSON* Request);

	UFUNCTION()
	void OnAssetsCreated(bool Succeeded);

	UFUNCTION()
	void OnFigmaImagesRequestReceived(UVaRestRequestJSON* Request);

	UFUNCTION()
	void OnPatchUAssets(bool Succeeded);

	UFUNCTION()
	void OnPostPatchUAssets(bool Succeeded);

	FVaRestCallDelegate OnVaRestFileRequestDelegate;
	FProcessFinishedDelegate OnAssetsCreatedDelegate;
	FVaRestCallDelegate OnVaRestImagesRequestDelegate;
	FProcessFinishedDelegate OnPatchUAssetsDelegate;
	FProcessFinishedDelegate OnPostPatchUAssetsDelegate;

	FVaRestCallResponse Response;

	FString AccessToken;
	FString FileKey;
	FString Ids;

	FString ContentRootFolder;

	FOnFigmaImportUpdateStatusCB RequesterCallback;


	UPROPERTY()
	TObjectPtr<UFigmaFile> File = nullptr;

	UPROPERTY()
	FImagesRequestResult ImagesRequestResult;
};
