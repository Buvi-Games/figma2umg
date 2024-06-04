// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ImageRequest.h"
#include "VaRestSubsystem.h"
#include "Parser/FigmaFile.h"
#include "Parser/ImagesRequestResult.h"

#include "FigmaImporter.generated.h"

class URequestParams;
class UFigmaFile;

UCLASS()
class UFigmaImporter : public UObject
{
	GENERATED_BODY()
public:
	UFigmaImporter(const FObjectInitializer& ObjectInitializer);

	void Init(const TObjectPtr<URequestParams> InProperties, const FOnFigmaImportUpdateStatusCB& InRequesterCallback);
	void Run();

protected:
	bool CreateRequest(const char* EndPoint, const FString& CurrentFileKey, const FString& RequestIds, const FVaRestCallDelegate& VaRestCallDelegate);
	void UpdateStatus(eRequestStatus Status, FString Message);
	void UpdateProgress(float ExpectedWorkThisFrame, const FText& Message);
	void UpdateProgressGameThread();
	void ResetProgressBar();

	void OnCurrentRequestComplete(UVaRestRequestJSON* Request);
	void OnCurrentRequestFail(UVaRestRequestJSON* Request);

	UFUNCTION()
	bool ParseRequestReceived(FString MessagePrefix, UVaRestRequestJSON* Request);

	UFUNCTION()
	void OnFigmaFileRequestReceived(UVaRestRequestJSON* Request);

	UFUNCTION()
	void OnFigmaLibraryFileRequestReceived(UVaRestRequestJSON* Request);
	void DownloadNextDependency();

	UFUNCTION()
	void OnAssetsCreated(bool Succeeded);

	void RequestImageURLs();

	UFUNCTION()
	void OnFigmaImagesRequestReceived(UVaRestRequestJSON* Request);

	void DownloadNextImage();

	UFUNCTION()
	void HandleImageDownload(bool Succeeded);

	UFUNCTION()
	void OnPatchUAssets(bool Succeeded);

	UFUNCTION()
	void OnPostPatchUAssets(bool Succeeded);

	FVaRestCallDelegate OnVaRestLibraryFileRequestDelegate;
	FVaRestCallDelegate OnVaRestFileRequestDelegate;
	FProcessFinishedDelegate OnAssetsCreatedDelegate;
	FVaRestCallDelegate OnVaRestImagesRequestDelegate;
	FOnImageRequestCompleteDelegate OnImageDownloadRequestCompleted;
	FProcessFinishedDelegate OnPatchUAssetsDelegate;
	FProcessFinishedDelegate OnPostPatchUAssetsDelegate;

	FVaRestCallResponse Response;

	FString AccessToken;
	FString FileKey;
	FString Ids;
	FString CurrentLibraryFileKey;

	FString ContentRootFolder;

	FOnFigmaImportUpdateStatusCB RequesterCallback;

	bool UsePrototypeFlow = false;
	bool TestNewParserProcess = false;

	UPROPERTY()
	TObjectPtr<UFigmaFile> File = nullptr;

	UPROPERTY()
	TMap<FString, TObjectPtr<UFigmaFile>> LibraryFileKeys;

	UPROPERTY()
	FImagesRequestResult ImagesRequestResult;
	FImageRequests RequestedImages;

	FScopedSlowTask* Progress = nullptr;
	float ProgressThisFrame = 0.0f;
	FText ProgressMessage;
};
