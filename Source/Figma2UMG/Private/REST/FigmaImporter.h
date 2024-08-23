// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FigmaImportSubsystem.h"
#include "ImageRequest.h"
#include "Parser/FigmaFile.h"
#include "Parser/ImagesRequestResult.h"
#include "Interfaces/IHttpRequest.h"

#include "FigmaImporter.generated.h"

class URequestParams;
class UFigmaFile;

UCLASS()
class FIGMA2UMG_API UFigmaImporter : public UObject
{
	GENERATED_BODY()
public:
	UFigmaImporter(const FObjectInitializer& ObjectInitializer);

	void Init(const TObjectPtr<URequestParams> InProperties, const FOnFigmaImportUpdateStatusCB& InRequesterCallback);
	void Run();

	template<class BuilderT>
	TObjectPtr<BuilderT> FintAssetBuilderForNode(const FString& Id) const;

protected:
	bool CreateRequest(const char* EndPoint, const FString& CurrentFileKey, const FString& RequestIds, const FHttpRequestCompleteDelegate& HttpRequestCompleteDelegate);
	bool CreateRequest(const char* EndPoint, const FString& CurrentFileKey, const FString& RequestIds, const FString& Suffix, const FHttpRequestCompleteDelegate& HttpRequestCompleteDelegate);
	void UpdateStatus(eRequestStatus Status, FString Message);
	void UpdateProgress(float ExpectedWorkThisFrame, const FText& Message);
	void UpdateProgressGameThread();
	void UpdateSubProgress(float ExpectedWorkThisFrame, const FText& Message);
	void UpdateSubProgressGameThread();
	void ResetProgressBar();

	TSharedPtr<FJsonObject> ParseRequestReceived(FString MessagePrefix, FHttpResponsePtr HttpResponse);

	void OnFigmaFileRequestReceived(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void OnFigmaLibraryFileRequestReceived(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void DownloadNextDependency();

	UFUNCTION()
	void FixReferences();

	UFUNCTION()
	void OnBuildersCreated(bool Succeeded);

	UFUNCTION()
	void BuildImageDependency ();

	UFUNCTION()
	void RequestImageRefURLs();

	void OnFigmaImagesRefURLReceived(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	UFUNCTION()
	void RequestImageURLs();

	void OnFigmaImagesURLReceived(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	UFUNCTION()
	void DownloadNextImage();

	UFUNCTION()
	void HandleImageDownload(bool Succeeded);

	UFUNCTION()
	void FetchGoogleFontsList();

	void OnFetchGoogleFontsResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	UFUNCTION()
	void BuildFontDependency();

	UFUNCTION()
	void DownloadNextFont();

	UFUNCTION()
	void HandleFontDownload(bool Succeeded);

	UFUNCTION()
	void LoadOrCreateAssets();

	UFUNCTION()
	void OnAssetsCreated(bool Succeeded);

	UFUNCTION()
	void CreateWidgetBuilders();

	UFUNCTION()
	void PatchPreInsertWidget();

	UFUNCTION()
	void CompileBPs(bool ProceedToNextState);

	UFUNCTION()
	void ReloadBPAssets(bool ProceedToNextState);

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

	FHttpRequestCompleteDelegate OnVaRestLibraryFileRequestDelegate;
	FHttpRequestCompleteDelegate OnVaRestFileRequestDelegate;
	FProcessFinishedDelegate OnBuildersCreatedDelegate;
	FProcessFinishedDelegate OnAssetsCreatedDelegate;
	FHttpRequestCompleteDelegate OnVaRestImagesRequestDelegate;
	FHttpRequestCompleteDelegate OnVaRestImagesRefRequestDelegate;
	FOnImageRequestCompleteDelegate OnImageDownloadRequestCompleted;
	FOnFontRequestCompleteDelegate OnFontDownloadRequestCompleted;
	FProcessFinishedDelegate OnPatchUAssetsDelegate;
	FProcessFinishedDelegate OnPostPatchUAssetsDelegate;

	FString AccessToken;
	FString FileKey;
	FString Ids;
	FString CurrentLibraryFileKey;

	FString ContentRootFolder;

	FOnFigmaImportUpdateStatusCB RequesterCallback;

	float NodeImageScale = 4.0f;

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
	int ImageDownloadCount = 0;

	FFontRequests RequestedFonts;
	int FontDownloadCount = 0;

	FScopedSlowTask* Progress = nullptr;
	float ProgressThisFrame = 0.0f;
	FText ProgressMessage;

	FScopedSlowTask* SubProgress = nullptr;
	float SubProgressThisFrame = 0.0f;
	FText SubProgressMessage;
};

template <class BuilderT>
TObjectPtr<BuilderT> UFigmaImporter::FintAssetBuilderForNode(const FString& Id) const
{
	for (const TScriptInterface<IAssetBuilder>& AssetBuilder : AssetBuilders)
	{
		if (BuilderT* Builder = Cast<BuilderT>(AssetBuilder.GetObject()))
		{
			if(const UFigmaNode* Node = Builder->GetNode())
			{
				if(Node->GetId().Equals(Id))
				{
					return Builder;
				}
			}
		}
	}

	return nullptr;
}
