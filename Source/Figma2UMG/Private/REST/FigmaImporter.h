// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FigmaImportSubsystem.h"
#include "ImageRequest.h"
#include "VaRestSubsystem.h"
#include "Parser/FigmaFile.h"
#include "Parser/ImagesRequestResult.h"
#include "Interfaces/IHttpRequest.h"

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
	void FixReferences();

	UFUNCTION()
	void OnBuildersCreated(bool Succeeded);

	UFUNCTION()
	void BuildImageDependency ();

	UFUNCTION()
	void RequestImageURLs();

	UFUNCTION()
	void DownloadNextImage();

	UFUNCTION()
	void OnFigmaImagesRequestReceived(UVaRestRequestJSON* Request);

	UFUNCTION()
	void HandleImageDownload(bool Succeeded);

	UFUNCTION()
	void FetchGoogleFontsList();

	void OnFetchGoogleFontsResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	UFUNCTION()
	void LoadOrCreateAssets();

	UFUNCTION()
	void OnAssetsCreated(bool Succeeded);

	UFUNCTION()
	void PatchAssets();

	UFUNCTION()
	void CreateWidgetBuilders();

	UFUNCTION()
	void PatchPreInsertWidget();

	UFUNCTION()
	void CompileBPs();

	UFUNCTION()
	void ReloadBPAssets();

	UFUNCTION()
	void PatchWidgetBinds();

	UFUNCTION()
	void PatchWidgetProperties();

	UFUNCTION()
	void OnPatchUAssets(bool Succeeded);

	UFUNCTION()
	void OnPostPatchUAssets(bool Succeeded);

	UFUNCTION()
	void SaveAll();

	FVaRestCallDelegate OnVaRestLibraryFileRequestDelegate;
	FVaRestCallDelegate OnVaRestFileRequestDelegate;
	FProcessFinishedDelegate OnBuildersCreatedDelegate;
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

	bool DownloadFontsFromGoogle = false;
	FString GFontsAPIKey;

	bool UsePrototypeFlow = false;
	bool SaveAllAtEnd = false;

	UPROPERTY()
	TObjectPtr<UFigmaFile> File = nullptr;

	UPROPERTY()
	TMap<FString, TObjectPtr<UFigmaFile>> LibraryFileKeys;

	UPROPERTY()
	TArray<TScriptInterface<IAssetBuilder>> AssetBuilders;

	UPROPERTY()
	FImagesRequestResult ImagesRequestResult;
	FImageRequests RequestedImages;

	FScopedSlowTask* Progress = nullptr;
	float ProgressThisFrame = 0.0f;
	FText ProgressMessage;

	int ImageDownloadCount = 0;
};
