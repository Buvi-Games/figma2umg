// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FileRequest.h"
#include "ImageRequest.h"
#include "ImagesGenerationRequest.h"
#include "Parser/FigmaFile.h"
#include "Parser/ImagesRequestResult.h"

#include "FigmaImporter.generated.h"

class URequestParams;
class UFigmaFile;
class UFileRequest;
class UImagesGenerationRequest;

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
	bool CreateRequest(const FString& CurrentFileKey, const FString& RequestIds, const FOnFileRequestCompleteDelegate& CallDelegate);
	bool CreateRequest(const FString& CurrentFileKey, const FString& RequestIds, const FOnImageGenerationRequestCompleteDelegate& CallDelegate);
	void UpdateStatus(eRequestStatus Status, FString Message);

	void OnFigmaFileRequestReceived(TObjectPtr<UFigmaFile> File, const TArray<uint8>& RawData);

	void OnFigmaLibraryFileRequestReceived(TObjectPtr<UFigmaFile> LibraryFile, const TArray<uint8>& RawData);
	void DownloadNextDependency();

	UFUNCTION()
	void OnAssetsCreated(bool Succeeded);

	void RequestImageURLs();

	UFUNCTION()
	void OnFigmaImagesRequestReceived(bool Succeeded, const FImagesRequestResult& result);

	void DownloadNextImage();

	UFUNCTION()
	void HandleImageDownload(bool Succeeded);

	UFUNCTION()
	void OnPatchUAssets(bool Succeeded);

	UFUNCTION()
	void OnPostPatchUAssets(bool Succeeded);

	FOnFileRequestCompleteDelegate OnLibraryFileRequestDelegate;
	FOnFileRequestCompleteDelegate OnFileRequestDelegate;
	FProcessFinishedDelegate OnAssetsCreatedDelegate;
	FOnImageGenerationRequestCompleteDelegate OnImagesRequestDelegate;
	FOnImageRequestCompleteDelegate OnImageDownloadRequestCompleted;
	FProcessFinishedDelegate OnPatchUAssetsDelegate;
	FProcessFinishedDelegate OnPostPatchUAssetsDelegate;

	FString AccessToken;
	FString FileKey;
	FString Ids;
	FString CurrentLibraryFileKey;

	FString ContentRootFolder;

	FOnFigmaImportUpdateStatusCB RequesterCallback;

	UPROPERTY()
	TObjectPtr<UFigmaFile> File = nullptr;

	UPROPERTY()
	TMap<FString, TObjectPtr<UFigmaFile>> LibraryFileKeys;

	UPROPERTY()
	TObjectPtr<UFileRequest> FileRequest = nullptr;

	UPROPERTY()
	TObjectPtr<UImagesGenerationRequest> ImagesGenerationRequest = nullptr;

	UPROPERTY()
	FImagesRequestResult ImagesRequestResult;
	FImageRequests RequestedImages;
};
