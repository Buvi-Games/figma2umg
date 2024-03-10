// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/FigmaImporter.h"

#include "Defines.h"
#include "Figma2UMGModule.h"
#include "FigmaImportSubsystem.h"
#include "JsonObjectConverter.h"
#include "RequestParams.h"
#include "Parser/FigmaFile.h"

UFigmaImporter::UFigmaImporter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OnFileRequestDelegate.BindUObject(this, &UFigmaImporter::OnFigmaFileRequestReceived);
	OnLibraryFileRequestDelegate.BindUObject(this, &UFigmaImporter::OnFigmaLibraryFileRequestReceived);
	OnAssetsCreatedDelegate.BindUObject(this, &UFigmaImporter::OnAssetsCreated);
	OnImagesRequestDelegate.BindUObject(this, &UFigmaImporter::OnFigmaImagesRequestReceived);
	OnImageDownloadRequestCompleted.BindUObject(this, &UFigmaImporter::HandleImageDownload);
	OnPatchUAssetsDelegate.BindUObject(this, &UFigmaImporter::OnPatchUAssets);
	OnPostPatchUAssetsDelegate.BindUObject(this, &UFigmaImporter::OnPostPatchUAssets);
}

void UFigmaImporter::Init(const TObjectPtr<URequestParams> InProperties, const FOnFigmaImportUpdateStatusCB& InRequesterCallback)
{
	AccessToken = InProperties->AccessToken;
	FileKey = InProperties->FileKey;
	if(!InProperties->Ids.IsEmpty())
	{

		Ids = InProperties->Ids[0];
		for (int i = 1; i < InProperties->Ids.Num(); i++)
		{
			Ids += "," + InProperties->Ids[i];
		}

	}

	for (FString Element : InProperties->LibraryFileKeys)
	{
		LibraryFileKeys.Add(Element);
	}

	ContentRootFolder = InProperties->ContentRootFolder;
	RequesterCallback = InRequesterCallback;
}


void UFigmaImporter::Run()
{
	if(LibraryFileKeys.IsEmpty())
	{
		if (CreateRequest(FileKey, Ids, OnFileRequestDelegate))
		{
			UE_LOG_Figma2UMG(Display, TEXT("Requesting file %s from Figma API"), *FileKey);
		}
	}
	else
	{
		DownloadNextDependency();
	}
}	

bool UFigmaImporter::CreateRequest(const FString& CurrentFileKey, const FString& RequestIds, const FOnFileRequestCompleteDelegate& CallDelegate)
{
	FString URL;
	TArray<FStringFormatArg> args;
	args.Add(FIGMA_BASE_URL);
	args.Add(FIGMA_ENDPOINT_FILES);
	args.Add(CurrentFileKey);
	if (RequestIds.IsEmpty())
	{
		URL = FString::Format(TEXT("{0}{1}{2}"), args);
	}
	else
	{
		args.Add(RequestIds);
		URL = FString::Format(TEXT("{0}{1}{2}?ids={3}"), args);
	}

	FileRequest = NewObject<UFileRequest>();
	FileRequest->Setup(AccessToken, URL, CallDelegate, ContentRootFolder);
	FileRequest->StartDownload();

	return true;
}

bool UFigmaImporter::CreateRequest(const FString& CurrentFileKey, const FString& RequestIds, const FOnImageGenerationRequestCompleteDelegate& CallDelegate)
{
	FString URL;
	TArray<FStringFormatArg> args;
	args.Add(FIGMA_BASE_URL);
	args.Add(FIGMA_ENDPOINT_IMAGES);
	args.Add(CurrentFileKey);
	if (RequestIds.IsEmpty())
	{
		URL = FString::Format(TEXT("{0}{1}{2}"), args);
	}
	else
	{
		args.Add(RequestIds);
		URL = FString::Format(TEXT("{0}{1}{2}?ids={3}"), args);
	}

	ImagesGenerationRequest = NewObject<UImagesGenerationRequest>();
	ImagesGenerationRequest->Setup(AccessToken, URL, CallDelegate);
	ImagesGenerationRequest->StartDownload();

	return true;
}

void UFigmaImporter::UpdateStatus(eRequestStatus Status, FString Message)
{
	AsyncTask(ENamedThreads::GameThread, [this, Status, Message]()
		{
			RequesterCallback.ExecuteIfBound(Status, Message);
		});

	if (Status == eRequestStatus::Failed || Status == eRequestStatus::Succeeded)
	{
		UFigmaImportSubsystem* ImporterSubsystem = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		if (ImporterSubsystem)
		{
			ImporterSubsystem->RemoveRequest(this);
		}
	}
}

void UFigmaImporter::OnFigmaFileRequestReceived(TObjectPtr<UFigmaFile> InFile, const TArray<uint8>& RawData)
{
	if (InFile)
	{
		const FString FigmaFilename = InFile->GetFileName();
		const FString FullFilename = FPaths::ProjectContentDir() + TEXT("../Downloads/") + FigmaFilename + TEXT("/") + FigmaFilename + TEXT(".figma");
		FFileHelper::SaveArrayToFile(RawData, *FullFilename);

		File = InFile;

		AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, InFile]()
			{
				for (TPair<FString, TObjectPtr<UFigmaFile>> LibPair : LibraryFileKeys)
				{
					LibPair.Value->FixComponentSetRef();
				}
			
				File->FixComponentSetRef();
			
				if (LibraryFileKeys.Num() > 0)
				{
					UE_LOG_Figma2UMG(Display, TEXT("Fix Remote References"));
					File->FixRemoteReferences(LibraryFileKeys);
				}
			
				UE_LOG_Figma2UMG(Display, TEXT("Creating UAssets"));
				File->LoadOrCreateAssets(OnAssetsCreatedDelegate);
			});
	}
	else
	{
		UpdateStatus(eRequestStatus::Failed, TEXT("[Figma file request] Failed to download file."));
	}
}

void UFigmaImporter::DownloadNextDependency()
{
	for (TPair<FString, TObjectPtr<UFigmaFile>> Lib : LibraryFileKeys)
	{
		if (Lib.Value == nullptr)
		{
			CurrentLibraryFileKey = Lib.Key;
			if (CreateRequest(CurrentLibraryFileKey, FString(), OnLibraryFileRequestDelegate))
			{
				UE_LOG_Figma2UMG(Display, TEXT("Requesting library file %s from Figma API"), *CurrentLibraryFileKey);
			}
			return;
		}
	}

	if (CreateRequest(FileKey, Ids, OnFileRequestDelegate))
	{
		UE_LOG_Figma2UMG(Display, TEXT("Requesting file %s from Figma API"), *FileKey);
	}
}

void UFigmaImporter::OnFigmaLibraryFileRequestReceived(TObjectPtr<UFigmaFile> LibraryFile, const TArray<uint8>& RawData)
{
	if (LibraryFile)
	{
		const FString FigmaFilename = LibraryFile->GetFileName();
		const FString FullFilename = FPaths::ProjectContentDir() + TEXT("../Downloads/") + FigmaFilename + TEXT("/") + FigmaFilename + TEXT(".figma");
		FFileHelper::SaveArrayToFile(RawData, *FullFilename);

		LibraryFileKeys[CurrentLibraryFileKey] = LibraryFile;
		CurrentLibraryFileKey = nullptr;

		AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, LibraryFile]()
			{
				UE_LOG_Figma2UMG(Display, TEXT("Library file %s downloaded."), *LibraryFile->GetFileName());
				DownloadNextDependency();
			});
	}
	else
	{
		UpdateStatus(eRequestStatus::Failed, TEXT("[Figma library file request] Failed to download file."));
	}
}

void UFigmaImporter::OnAssetsCreated(bool Succeeded)
{
	if(!Succeeded)
	{
		UpdateStatus(eRequestStatus::Failed, TEXT("Fail to create UAssets."));
		return;
	}

	UE_LOG_Figma2UMG(Display, TEXT("[Figma images Request]"));
	RequestedImages.Reset();
	File->BuildImageDependency(FileKey, RequestedImages);

	if (const FImagePerFileRequests* Requests = RequestedImages.GetRequests())
	{
		RequestImageURLs();
	}
	else
	{
		UE_LOG_Figma2UMG(Display, TEXT("Patching UAssets."));
		File->Patch(OnPatchUAssetsDelegate);
	}
}

void UFigmaImporter::RequestImageURLs()
{
	const FImagePerFileRequests* Requests = RequestedImages.GetRequests();
	if (Requests)
	{
		FString ImageIdsFormated = Requests->Requests[0].Id;
		for (int i = 1; i < Requests->Requests.Num(); i++)
		{
			ImageIdsFormated += "," + Requests->Requests[i].Id;
		}

		//Todo: Manage images from Libs
		if (CreateRequest(Requests->FileKey, ImageIdsFormated, OnImagesRequestDelegate))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[Figma images Request] Requesting %u images in file %s from Figma API."), Requests->Requests.Num(), *Requests->FileKey);
		}
	}
	else
	{
		UE_LOG_Figma2UMG(Display, TEXT("Patching UAssets."));
		File->Patch(OnPatchUAssetsDelegate);
	}
}

void UFigmaImporter::OnFigmaImagesRequestReceived(bool Succeeded, const FImagesRequestResult& result)
{
	if (Succeeded)
	{
		ImagesRequestResult = result;
		if (ImagesRequestResult.Err.IsEmpty())
		{
			UE_LOG_Figma2UMG(Display, TEXT("[Figma images Request] %u images received from Figma API."), ImagesRequestResult.Images.Num());
			for (TPair<FString, FString> Element : ImagesRequestResult.Images)
			{
				RequestedImages.SetURL(Element.Key, Element.Value);
			}
			DownloadNextImage();
		}
		else
		{
			UpdateStatus(eRequestStatus::Failed, TEXT("[Figma images request]") + ImagesRequestResult.Err);
		}
	}
	else
	{
		UpdateStatus(eRequestStatus::Failed, TEXT("[Figma images request] Failed to download file."));
	}
}

void UFigmaImporter::DownloadNextImage()
{
	FImageRequest* ImageRequest = RequestedImages.GetNextToDownload();
	if (ImageRequest)
	{
		UE_LOG_Figma2UMG(Display, TEXT("Downloading image %s at %s."), *ImageRequest->ImageName, *ImageRequest->URL);
		ImageRequest->StartDownload(OnImageDownloadRequestCompleted);
	}
	else
	{
		RequestImageURLs();
	}
}

void UFigmaImporter::HandleImageDownload(bool Succeeded)
{
	if (Succeeded)
	{
		DownloadNextImage();
	}
	else
	{
		UpdateStatus(eRequestStatus::Failed, TEXT("Fail to download Image."));
	}
}

void UFigmaImporter::OnPatchUAssets(bool Succeeded)
{
	if (!Succeeded)
	{
		UpdateStatus(eRequestStatus::Failed, TEXT("Fail to patch UAssets."));
		return;
	}

	UE_LOG_Figma2UMG(Display, TEXT("Post-patch UAssets."));
	File->PostPatch(OnPostPatchUAssetsDelegate);
}

void UFigmaImporter::OnPostPatchUAssets(bool Succeeded)
{
	if (Succeeded)
	{
		UpdateStatus(eRequestStatus::Succeeded, File->GetFileName() + TEXT(" was successfully imported."));
	}
	else
	{
		UpdateStatus(eRequestStatus::Failed, TEXT("Failed at Post-patch of UAssets."));
	}
}
