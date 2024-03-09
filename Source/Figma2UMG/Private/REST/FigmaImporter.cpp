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
	OnVaRestFileRequestDelegate.BindUObject(this, &UFigmaImporter::OnFigmaFileRequestReceived);
	OnVaRestLibraryFileRequestDelegate.BindUObject(this, &UFigmaImporter::OnFigmaLibraryFileRequestReceived);
	OnAssetsCreatedDelegate.BindUObject(this, &UFigmaImporter::OnAssetsCreated);
	OnVaRestImagesRequestDelegate.BindUObject(this, &UFigmaImporter::OnFigmaImagesRequestReceived);
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
		if (CreateRequest(FileKey, Ids, OnVaRestFileRequestDelegate))
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
	FileRequest->Setup(AccessToken, URL, CallDelegate);
	FileRequest->StartDownload();
//	UVaRestJsonObject* VaRestJson = VARestSubsystem->ConstructVaRestJsonObject();
//	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
//	JsonObject->SetStringField(FIGMA_ACCESSTOLKENS_HEADER, AccessToken);
//	VaRestJson->SetRootObject(JsonObject);
//
//	//pVARestSubsystem->CallURL(URL, EVaRestRequestVerb::GET, EVaRestRequestContentType::json, VaRestJson, OnVaRestDelegate);
//
//	UVaRestRequestJSON* Request = VARestSubsystem->ConstructVaRestRequest();
//
//	Request->SetVerb(EVaRestRequestVerb::GET);
//	Request->SetContentType(static_cast<EVaRestRequestContentType>(-1));
//	Request->SetHeader(FIGMA_ACCESSTOLKENS_HEADER, AccessToken);
//	Request->SetHeader(FString("Host"), FIGMA_HOST);
//
//	//Response.Request = Request;
//	//Response.Callback = VaRestCallDelegate;
//	//
//	//Response.CompleteDelegateHandle = Request->OnStaticRequestComplete.AddUObject(this, &UFigmaImporter::OnCurrentRequestComplete);
//	//Response.FailDelegateHandle = Request->OnStaticRequestFail.AddUObject(this, &UFigmaImporter::OnCurrentRequestFail);
//
//	Request->ResetResponseData();
//	Request->ProcessURL(URL);
//
//	// This section bellow is a hack due to the FCurlHttpRequest::SetupRequest() always adding the header Content-Length. Adding it makes the Figma AIP return the error 400 
//	// To avoid reimplementing the curl class, we need to maually remove the Header item.
//	// This will need update and check if it has any change in the FCurlHttpRequest size so the memory offset of Header changed.
//
//	int HeaderAddressOffset = 0;
//
//#if WITH_CURL
//#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 3 && ENGINE_PATCH_VERSION == 2)
//	HeaderAddressOffset = 256;
//#endif
//#endif
//
//	if (HeaderAddressOffset > 0)
//	{
//		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = Request->GetHttpRequest();
//		IHttpRequest* HttpRequestPtr = &(HttpRequest.Get());
//		void* HeaderAddress = reinterpret_cast<void*>(reinterpret_cast<int64>(HttpRequestPtr) + HeaderAddressOffset);
//		TMap<FString, FString>* HeadersPtr = static_cast<TMap<FString, FString>*>(HeaderAddress);
//		HeadersPtr->Remove(TEXT("Content-Length"));
//	}
//
//	// End of Hack

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

//void UFigmaImporter::OnCurrentRequestComplete(UVaRestRequestJSON* Request)
//{
//	Request->OnStaticRequestComplete.Remove(Response.CompleteDelegateHandle);
//	Request->OnStaticRequestFail.Remove(Response.FailDelegateHandle);
//
//	Response.Callback.ExecuteIfBound(Request);
//
//	Response.Request = nullptr;
//	Response.Callback.Unbind();
//}
//
//void UFigmaImporter::OnCurrentRequestFail(UVaRestRequestJSON* Request)
//{
//	Request->OnStaticRequestComplete.Remove(Response.CompleteDelegateHandle);
//	Request->OnStaticRequestFail.Remove(Response.FailDelegateHandle);
//
//	Response.Callback.ExecuteIfBound(Request);
//
//	Response.Request = nullptr;
//	Response.Callback.Unbind();
//}

//bool UFigmaImporter::ParseRequestReceived(FString MessagePrefix, UVaRestRequestJSON* Request)
//{
//	if (Request)
//	{
//		const EVaRestRequestStatus status = Request->GetStatus();
//		switch (status)
//		{
//		case EVaRestRequestStatus::NotStarted:
//			UE_LOG_Figma2UMG(Warning, TEXT("%s%s"), *MessagePrefix, TEXT("EVaRestRequestStatus::NotStarted."));
//			break;
//		case EVaRestRequestStatus::Processing:
//			UE_LOG_Figma2UMG(Warning, TEXT("%s%s"), *MessagePrefix, TEXT("EVaRestRequestStatus::Processing."));
//			break;
//		case EVaRestRequestStatus::Failed:
//			UpdateStatus(eRequestStatus::Failed, MessagePrefix + TEXT("EVaRestRequestStatus::Failed"));
//			break;
//		case EVaRestRequestStatus::Failed_ConnectionError:
//			UpdateStatus(eRequestStatus::Failed, MessagePrefix + TEXT("EVaRestRequestStatus::Failed_ConnectionError"));
//			break;
//		case EVaRestRequestStatus::Succeeded:
//			UE_LOG_Figma2UMG(Display, TEXT("%s%s"), *MessagePrefix, TEXT("EVaRestRequestStatus::Succeeded"));
//			UVaRestJsonObject* responseJson = Request->GetResponseObject();
//			if (!responseJson)
//			{
//				UpdateStatus(eRequestStatus::Failed, MessagePrefix + TEXT("VaRestJson has no response object"));
//
//				return false;
//			}
//
//			const TSharedRef<FJsonObject> JsonObj = responseJson->GetRootObject();
//			if (JsonObj->HasField("status") && JsonObj->HasField("err"))
//			{
//				UpdateStatus(eRequestStatus::Failed, MessagePrefix + JsonObj->GetStringField("err"));
//
//				return false;
//			}
//
//			return true;
//			
//		}
//	}
//	else
//	{
//		UpdateStatus(eRequestStatus::Failed, MessagePrefix + TEXT("Result from Figma request is nullptr."));
//	}
//	return false;
//}

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
				constexpr int64 CheckFlags = 0;
				constexpr int64 SkipFlags = 0;
				constexpr bool StrictMode = false;
				FText OutFailReason;
				//if (FJsonObjectConverter::JsonObjectToUStruct(JsonObj, File->StaticClass(), File, CheckFlags, SkipFlags, StrictMode, &OutFailReason))
				//{
				//	UE_LOG_Figma2UMG(Display, TEXT("Post-Serialize"));
				//	File->PostSerialize(ContentRootFolder, JsonObj);
				//
				//	for (TPair<FString, TObjectPtr<UFigmaFile>> LibPair : LibraryFileKeys)
				//	{
				//		LibPair.Value->FixComponentSetRef();
				//	}
				//
				//	File->FixComponentSetRef();
				//
				//	if (LibraryFileKeys.Num() > 0)
				//	{
				//		UE_LOG_Figma2UMG(Display, TEXT("Fix Remote References"));
				//		File->FixRemoteReferences(LibraryFileKeys);
				//	}
				//
				//	UE_LOG_Figma2UMG(Display, TEXT("Creating UAssets"));
				//	File->LoadOrCreateAssets(OnAssetsCreatedDelegate);
				//}
				//else
				{
					UpdateStatus(eRequestStatus::Failed, OutFailReason.ToString());
				}
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
			if (CreateRequest(CurrentLibraryFileKey, FString(), OnVaRestLibraryFileRequestDelegate))
			{
				UE_LOG_Figma2UMG(Display, TEXT("Requesting library file %s from Figma API"), *CurrentLibraryFileKey);
			}
			return;
		}
	}

	if (CreateRequest(FileKey, Ids, OnVaRestFileRequestDelegate))
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
				constexpr int64 CheckFlags = 0;
				constexpr int64 SkipFlags = 0;
				constexpr bool StrictMode = false;
				FText OutFailReason;
				//if (FJsonObjectConverter::JsonObjectToUStruct(JsonObj, LibraryFile->StaticClass(), LibraryFile, CheckFlags, SkipFlags, StrictMode, &OutFailReason))
				{
					//LibraryFile->PostSerialize(ContentRootFolder, JsonObj);
					//UE_LOG_Figma2UMG(Display, TEXT("Library file %s downloaded."), *LibraryFile->GetFileName());
					//DownloadNextDependency();
				}
				//else
				{
					UpdateStatus(eRequestStatus::Failed, OutFailReason.ToString());
				}
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
		if (CreateRequest(Requests->FileKey, ImageIdsFormated, OnVaRestImagesRequestDelegate))
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
		constexpr int64 CheckFlags = 0;
		constexpr int64 SkipFlags = 0;
		constexpr bool StrictMode = false;
		FText OutFailReason;
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
