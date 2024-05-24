// Copyright 2024 Buvi Games. All Rights Reserved.


#include "REST/FigmaImporter.h"

#include "Defines.h"
#include "Figma2UMGModule.h"
#include "FigmaImportSubsystem.h"
#include "JsonObjectConverter.h"
#include "RequestParams.h"
#include "Async/Async.h"
#include "Parser/FigmaFile.h"

UFigmaImporter::UFigmaImporter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OnVaRestFileRequestDelegate.BindUFunction(this, FName("OnFigmaFileRequestReceived"));
	OnVaRestLibraryFileRequestDelegate.BindUFunction(this, FName("OnFigmaLibraryFileRequestReceived"));
	OnAssetsCreatedDelegate.BindUObject(this, &UFigmaImporter::OnAssetsCreated);
	OnVaRestImagesRequestDelegate.BindUFunction(this, FName("OnFigmaImagesRequestReceived"));
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

	UsePrototypeFlow = InProperties->UsePrototypeFlow;
}


void UFigmaImporter::Run()
{
	int WorkCount = (LibraryFileKeys.Num() * 3/*Request, Parse, PostSerialization*/) + 4;//Request, Parse, PostSerialization, Fix
	Progress = new FScopedSlowTask(WorkCount, NSLOCTEXT("Figma2UMG", "Figma2UMG_ImportProgress", "Importing from FIGMA"));
	Progress->MakeDialog();
	if(LibraryFileKeys.IsEmpty())
	{
		UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_RequestFile", "Downloading Design File."));
		if (CreateRequest(FIGMA_ENDPOINT_FILES, FileKey, Ids, OnVaRestFileRequestDelegate))
		{
			UE_LOG_Figma2UMG(Display, TEXT("Requesting file %s from Figma API"), *FileKey);
		}
	}
	else
	{
		DownloadNextDependency();
	}
}	

bool UFigmaImporter::CreateRequest(const char* EndPoint, const FString& CurrentFileKey, const FString& RequestIds, const FVaRestCallDelegate& VaRestCallDelegate)
{
	UVaRestSubsystem* VARestSubsystem = GEngine->GetEngineSubsystem<UVaRestSubsystem>();
	if (!VARestSubsystem)
	{
		UpdateStatus(eRequestStatus::Failed, TEXT("Can't find EngineSubsystem UVaRest"));
		return false;
	}

	FString URL;
	TArray<FStringFormatArg> args;
	args.Add(FIGMA_BASE_URL);
	args.Add(EndPoint);
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

	UVaRestJsonObject* VaRestJson = VARestSubsystem->ConstructVaRestJsonObject();
	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField(FIGMA_ACCESSTOLKENS_HEADER, AccessToken);
	VaRestJson->SetRootObject(JsonObject);

	//pVARestSubsystem->CallURL(URL, EVaRestRequestVerb::GET, EVaRestRequestContentType::json, VaRestJson, OnVaRestDelegate);

	UVaRestRequestJSON* Request = VARestSubsystem->ConstructVaRestRequest();

	Request->SetVerb(EVaRestRequestVerb::GET);
	Request->SetContentType(static_cast<EVaRestRequestContentType>(-1));
	Request->SetHeader(FIGMA_ACCESSTOLKENS_HEADER, AccessToken);
	Request->SetHeader(FString("Host"), FIGMA_HOST);

	Response.Request = Request;
	Response.Callback = VaRestCallDelegate;

	Response.CompleteDelegateHandle = Request->OnStaticRequestComplete.AddUObject(this, &UFigmaImporter::OnCurrentRequestComplete);
	Response.FailDelegateHandle = Request->OnStaticRequestFail.AddUObject(this, &UFigmaImporter::OnCurrentRequestFail);

	Request->ResetResponseData();
	Request->ProcessURL(URL);

	// This section bellow is a hack due to the FCurlHttpRequest::SetupRequest() always adding the header Content-Length. Adding it makes the Figma AIP return the error 400 
	// To avoid reimplementing the curl class, we need to manually remove the Header item.
	// This will need update and check if it has any change in the FCurlHttpRequest size so the memory offset of Header changed.

	int HeaderAddressOffset = 0;

#if WITH_CURL
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 4 && ENGINE_PATCH_VERSION == 1)
	HeaderAddressOffset = 664;
#elif (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 3 && ENGINE_PATCH_VERSION == 2)
	HeaderAddressOffset = 256;
#elif (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 2)
	HeaderAddressOffset = 200;
#endif
#endif

	if (HeaderAddressOffset > 0)
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = Request->GetHttpRequest();
		IHttpRequest* HttpRequestPtr = &(HttpRequest.Get());
		void* HeaderAddress = reinterpret_cast<void*>(reinterpret_cast<int64>(HttpRequestPtr) + HeaderAddressOffset);
		TMap<FString, FString>* HeadersPtr = static_cast<TMap<FString, FString>*>(HeaderAddress);
		HeadersPtr->Remove(TEXT("Content-Length"));
	}

	// End of Hack

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
		delete Progress;
		Progress = nullptr;
	}
}

void UFigmaImporter::UpdateProgress(float ExpectedWorkThisFrame, const FText& Message)
{
	AsyncTask(ENamedThreads::GameThread, [this, ExpectedWorkThisFrame, Message]()
		{
			Progress->EnterProgressFrame(ExpectedWorkThisFrame, Message);
		});
}

void UFigmaImporter::OnCurrentRequestComplete(UVaRestRequestJSON* Request)
{
	Request->OnStaticRequestComplete.Remove(Response.CompleteDelegateHandle);
	Request->OnStaticRequestFail.Remove(Response.FailDelegateHandle);

	Response.Callback.ExecuteIfBound(Request);

	Response.Request = nullptr;
	Response.Callback.Unbind();
}

void UFigmaImporter::OnCurrentRequestFail(UVaRestRequestJSON* Request)
{
	Request->OnStaticRequestComplete.Remove(Response.CompleteDelegateHandle);
	Request->OnStaticRequestFail.Remove(Response.FailDelegateHandle);

	Response.Callback.ExecuteIfBound(Request);

	Response.Request = nullptr;
	Response.Callback.Unbind();
}

bool UFigmaImporter::ParseRequestReceived(FString MessagePrefix, UVaRestRequestJSON* Request)
{
	if (Request)
	{
		const EVaRestRequestStatus status = Request->GetStatus();
		switch (status)
		{
		case EVaRestRequestStatus::NotStarted:
			UE_LOG_Figma2UMG(Warning, TEXT("%s%s"), *MessagePrefix, TEXT("EVaRestRequestStatus::NotStarted."));
			break;
		case EVaRestRequestStatus::Processing:
			UE_LOG_Figma2UMG(Warning, TEXT("%s%s"), *MessagePrefix, TEXT("EVaRestRequestStatus::Processing."));
			break;
		case EVaRestRequestStatus::Failed:
			UpdateStatus(eRequestStatus::Failed, MessagePrefix + TEXT("EVaRestRequestStatus::Failed"));
			break;
		case EVaRestRequestStatus::Failed_ConnectionError:
			UpdateStatus(eRequestStatus::Failed, MessagePrefix + TEXT("EVaRestRequestStatus::Failed_ConnectionError"));
			break;
		case EVaRestRequestStatus::Succeeded:
			UE_LOG_Figma2UMG(Display, TEXT("%s%s"), *MessagePrefix, TEXT("EVaRestRequestStatus::Succeeded"));
			UVaRestJsonObject* responseJson = Request->GetResponseObject();
			if (!responseJson)
			{
				UpdateStatus(eRequestStatus::Failed, MessagePrefix + TEXT("VaRestJson has no response object"));

				return false;
			}

			static FString StatusStr("status");
			static FString ErrorStr("err");
			const TSharedRef<FJsonObject> JsonObj = responseJson->GetRootObject();
			if (JsonObj->HasField(StatusStr) && JsonObj->HasField(ErrorStr))
			{
				UpdateStatus(eRequestStatus::Failed, MessagePrefix + JsonObj->GetStringField(ErrorStr));

				return false;
			}

			return true;
			
		}
	}
	else
	{
		UpdateStatus(eRequestStatus::Failed, MessagePrefix + TEXT("Result from Figma request is nullptr."));
	}
	return false;
}

void UFigmaImporter::OnFigmaFileRequestReceived(UVaRestRequestJSON* Request)
{
	UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_ParseFile", "Parsing Design File."));
	if (ParseRequestReceived(TEXT("[Figma file request] "), Request))
	{
		UVaRestJsonObject* responseJson = Request->GetResponseObject();
		const TSharedRef<FJsonObject> JsonObj = responseJson->GetRootObject();

		static FString NameStr("Name");
		const FString FigmaFilename = JsonObj->GetStringField(NameStr);
		const FString FullFilename = FPaths::ProjectContentDir() + TEXT("../Downloads/") + FigmaFilename + TEXT("/") + FigmaFilename + TEXT(".figma");
		const FString RawText = Request->GetResponseContentAsString(false);
		FFileHelper::SaveStringToFile(RawText, *FullFilename);

		File = NewObject<UFigmaFile>();

		AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, JsonObj]()
			{
				constexpr int64 CheckFlags = 0;
				constexpr int64 SkipFlags = 0;
				constexpr bool StrictMode = false;
				FText OutFailReason;
				if (FJsonObjectConverter::JsonObjectToUStruct(JsonObj, File->StaticClass(), File, CheckFlags, SkipFlags, StrictMode, &OutFailReason))
				{
					UE_LOG_Figma2UMG(Display, TEXT("Post-Serialize"));
					UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PostSerializeFile", "PostSerialize Design File."));
					File->PostSerialize(ContentRootFolder, JsonObj);

					if (UsePrototypeFlow)
					{
						File->PrepareForFlow();
					}

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

					UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_LoadOrCreateAssets", "Loading or create UAssets."));
					UE_LOG_Figma2UMG(Display, TEXT("Creating UAssets"));
					File->LoadOrCreateAssets(OnAssetsCreatedDelegate);
				}
				else
				{
					UpdateStatus(eRequestStatus::Failed, OutFailReason.ToString());
				}
			});
	}
}

void UFigmaImporter::DownloadNextDependency()
{
	for (TPair<FString, TObjectPtr<UFigmaFile>> Lib : LibraryFileKeys)
	{
		if (Lib.Value == nullptr)
		{
			UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_RequestLib", "Downloading Library File."));
			CurrentLibraryFileKey = Lib.Key;
			if (CreateRequest(FIGMA_ENDPOINT_FILES, CurrentLibraryFileKey, FString(), OnVaRestLibraryFileRequestDelegate))
			{
				UE_LOG_Figma2UMG(Display, TEXT("Requesting library file %s from Figma API"), *CurrentLibraryFileKey);
			}
			return;
		}
	}

	UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_RequestFile", "Downloading Design File."));
	if (CreateRequest(FIGMA_ENDPOINT_FILES, FileKey, Ids, OnVaRestFileRequestDelegate))
	{
		UE_LOG_Figma2UMG(Display, TEXT("Requesting file %s from Figma API"), *FileKey);
	}
}

void UFigmaImporter::OnFigmaLibraryFileRequestReceived(UVaRestRequestJSON* Request)
{
	UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_ParseLib", "Parsing Library File."));
	if (ParseRequestReceived(TEXT("[Figma library file request] "), Request))
	{
		UVaRestJsonObject* responseJson = Request->GetResponseObject();
		const TSharedRef<FJsonObject> JsonObj = responseJson->GetRootObject();


		static FString NameStr("Name");
		const FString FigmaFilename = JsonObj->GetStringField(NameStr);
		const FString FullFilename = FPaths::ProjectContentDir() + TEXT("../Downloads/") + FigmaFilename + TEXT("/") + FigmaFilename + TEXT(".figma");
		const FString RawText = Request->GetResponseContentAsString(false);
		FFileHelper::SaveStringToFile(RawText, *FullFilename);

		UFigmaFile* CurrentFile = NewObject<UFigmaFile>();
		LibraryFileKeys[CurrentLibraryFileKey] = CurrentFile;
		CurrentLibraryFileKey = nullptr;

		AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, JsonObj, CurrentFile]()
			{
				constexpr int64 CheckFlags = 0;
				constexpr int64 SkipFlags = 0;
				constexpr bool StrictMode = false;
				FText OutFailReason;
				if (FJsonObjectConverter::JsonObjectToUStruct(JsonObj, CurrentFile->StaticClass(), CurrentFile, CheckFlags, SkipFlags, StrictMode, &OutFailReason))
				{
					UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PostSerializeLib", "PostSerialize Library File."));
					CurrentFile->PostSerialize(ContentRootFolder, JsonObj);
					UE_LOG_Figma2UMG(Display, TEXT("Library file %s downloaded."), *CurrentFile->GetFileName());
					DownloadNextDependency();
				}
				else
				{
					UpdateStatus(eRequestStatus::Failed, OutFailReason.ToString());
				}
			});
	}
}

void UFigmaImporter::OnAssetsCreated(bool Succeeded)
{
	if(!Succeeded)
	{
		UpdateStatus(eRequestStatus::Failed, TEXT("Fail to create UAssets."));
		return;
	}

	UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_ImageDependency", "Build Image Dependency."));
	UE_LOG_Figma2UMG(Display, TEXT("[Figma images Request]"));
	RequestedImages.Reset();
	File->BuildImageDependency(FileKey, RequestedImages);

	delete Progress;
	Progress = nullptr;
	if (const FImagePerFileRequests* Requests = RequestedImages.GetRequests())
	{
		RequestImageURLs();
	}
	else
	{
		Progress = new FScopedSlowTask(7, NSLOCTEXT("Figma2UMG", "Figma2UMG_Patch", "Patching Widgets"));
		Progress->MakeDialog();

		UE_LOG_Figma2UMG(Display, TEXT("Patching UAssets."));
		File->Patch(OnPatchUAssetsDelegate, Progress);
	}
}

void UFigmaImporter::RequestImageURLs()
{
	delete Progress;
	const FImagePerFileRequests* Requests = RequestedImages.GetRequests();
	if (Requests)
	{
		Progress = new FScopedSlowTask(100, NSLOCTEXT("Figma2UMG", "Figma2UMG_ImportProgress", "Importing from FIGMA"));
		Progress->MakeDialog();
		UpdateProgress(10, NSLOCTEXT("Figma2UMG", "Figma2UMG_ImageRequest", "Requesting Image."));
		FString ImageIdsFormated = Requests->Requests[0].Id;
		for (int i = 1; i < Requests->Requests.Num(); i++)
		{
			ImageIdsFormated += "," + Requests->Requests[i].Id;
		}

		//Todo: Manage images from Libs
		if (CreateRequest(FIGMA_ENDPOINT_IMAGES, Requests->FileKey, ImageIdsFormated, OnVaRestImagesRequestDelegate))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[Figma images Request] Requesting %u images in file %s from Figma API."), Requests->Requests.Num(), *Requests->FileKey);
		}
	}
	else
	{
		Progress = new FScopedSlowTask(7, NSLOCTEXT("Figma2UMG", "Figma2UMG_Patch", "Patching Widgets"));
		Progress->MakeDialog();
		UE_LOG_Figma2UMG(Display, TEXT("Patching UAssets."));
		File->Patch(OnPatchUAssetsDelegate, Progress);
	}
}

void UFigmaImporter::OnFigmaImagesRequestReceived(UVaRestRequestJSON* Request)
{
	UpdateProgress(10, NSLOCTEXT("Figma2UMG", "Figma2UMG_ImageParser", "Parsing Image Result."));
	if (ParseRequestReceived(TEXT("[Figma images request] "), Request))
	{
		UVaRestJsonObject* responseJson = Request->GetResponseObject();
		const TSharedRef<FJsonObject> JsonObj = responseJson->GetRootObject();

		constexpr int64 CheckFlags = 0;
		constexpr int64 SkipFlags = 0;
		constexpr bool StrictMode = false;
		FText OutFailReason;
		if (FJsonObjectConverter::JsonObjectToUStruct(JsonObj, &ImagesRequestResult, CheckFlags, SkipFlags, StrictMode, &OutFailReason))
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
			UpdateStatus(eRequestStatus::Failed, OutFailReason.ToString());
		}
	}
}

void UFigmaImporter::DownloadNextImage()
{
	FImageRequest* ImageRequest = RequestedImages.GetNextToDownload();
	if (ImageRequest)
	{
		UpdateProgress(80 / RequestedImages.GetCurrentRequestTotalCount(), NSLOCTEXT("Figma2UMG", "Figma2UMG_ImageDownloading", "Downloading Image."));
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
	UpdateProgress(1, NSLOCTEXT("Figma2UMG", "Figma2UMG_PostPatch", "Post-patch UAssets"));
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
