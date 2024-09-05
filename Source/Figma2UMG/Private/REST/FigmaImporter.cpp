// Copyright 2024 Buvi Games. All Rights Reserved.


#include "REST/FigmaImporter.h"

#include "Defines.h"
#include "Figma2UMGModule.h"
#include "FigmaImportSubsystem.h"
#include "FileHelpers.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "RequestParams.h"
#include "Async/Async.h"
#include "Builder/Asset/AssetBuilder.h"
#include "Builder/Asset/FontBuilder.h"
#include "Builder/Asset/Texture2DBuilder.h"
#include "Builder/Asset/WidgetBlueprintBuilder.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FeedbackContext.h"
#include "Misc/ScopedSlowTask.h"
#include "Misc/SlowTaskStack.h"
#include "Parser/FigmaFile.h"
#include "Misc/FileHelper.h"

UFigmaImporter::UFigmaImporter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OnVaRestFileRequestDelegate.BindUObject(this, &UFigmaImporter::OnFigmaFileRequestReceived);
	OnVaRestLibraryFileRequestDelegate.BindUObject(this, &UFigmaImporter::OnFigmaLibraryFileRequestReceived);
	OnBuildersCreatedDelegate.BindUObject(this, &UFigmaImporter::OnBuildersCreated);
	OnAssetsCreatedDelegate.BindUObject(this, &UFigmaImporter::OnAssetsCreated);
	OnVaRestImagesRefRequestDelegate.BindUObject(this, &UFigmaImporter::OnFigmaImagesRefURLReceived);
	OnVaRestImagesRequestDelegate.BindUObject(this, &UFigmaImporter::OnFigmaImagesURLReceived);
	OnImageDownloadRequestCompleted.BindUObject(this, &UFigmaImporter::HandleImageDownload);
	OnFontDownloadRequestCompleted.BindUObject(this, &UFigmaImporter::HandleFontDownload);
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

	NodeImageScale = InProperties->NodeImageScale;

	DownloadFontsFromGoogle = InProperties->DownloadFontsFromGoogle;
	GFontsAPIKey = InProperties->GFontsAPIKey;

	UsePrototypeFlow = InProperties->UsePrototypeFlow;
	SaveAllAtEnd = InProperties->SaveAllAtEnd;
}


void UFigmaImporter::Run()
{
	int WorkCount = (LibraryFileKeys.Num() * 3/*Request, Parse, PostSerialization*/) + 3/*Request, Parse, PostSerialization*/ + 15;//Fix, Builders, Images, Fonts, Load/Create, Patch(WidgetBuilders,PreInsert+Compiling+Reloading+Binds+Properties), Post-patch
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

bool UFigmaImporter::CreateRequest(const char* EndPoint, const FString& CurrentFileKey, const FString& RequestIds, const FHttpRequestCompleteDelegate& HttpRequestCompleteDelegate)
{
	return CreateRequest(EndPoint, CurrentFileKey, RequestIds, FString(), HttpRequestCompleteDelegate);
}

bool UFigmaImporter::CreateRequest(const char* EndPoint, const FString& CurrentFileKey, const FString& RequestIds, const FString& Suffix, const FHttpRequestCompleteDelegate& HttpRequestCompleteDelegate)
{
	FString URL;
	TArray<FStringFormatArg> args;
	args.Add(FIGMA_BASE_URL);
	args.Add(EndPoint);
	args.Add(CurrentFileKey);
	if (RequestIds.IsEmpty())
	{
		if (Suffix.IsEmpty())
		{
			URL = FString::Format(TEXT("{0}{1}{2}"), args);
		}
		else
		{
			args.Add(Suffix);
			URL = FString::Format(TEXT("{0}{1}{2}{3}"), args);
		}
	}
	else
	{
		args.Add(RequestIds);
		if (Suffix.IsEmpty())
		{
			URL = FString::Format(TEXT("{0}{1}{2}?ids={3}"), args);
		}
		else
		{
			args.Add(Suffix);
			URL = FString::Format(TEXT("{0}{1}{2}?ids={3}{4}"), args);
		}
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(FIGMA_ACCESSTOLKENS_HEADER, AccessToken);
	HttpRequest->SetHeader(FString("Host"), FIGMA_HOST);

	HttpRequest->OnProcessRequestComplete() = HttpRequestCompleteDelegate;

	HttpRequest->ProcessRequest();


	TArray<FString> Headers = HttpRequest->GetAllHeaders();
	if(Headers.ContainsByPredicate([](const FString& Header) { return Header.Contains(TEXT("Content-Length")); }))
	{
		// This section bellow is a hack due to the FCurlHttpRequest::SetupRequest() always adding the header Content-Length. Adding it makes the Figma AIP return the error 400 
		// To avoid reimplementing the curl class, we need to manually remove the Header item.
		// This will need update and check if it has any change in the FCurlHttpRequest size so the memory offset of Header changed.

		int HeaderAddressOffset = 0;

#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 4 && ENGINE_PATCH_VERSION <= 3)
		HeaderAddressOffset = 664;
#elif (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 3)
		HeaderAddressOffset = 256;
#elif (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 2)
		HeaderAddressOffset = 200;
#endif

		if (HeaderAddressOffset > 0)
		{
			IHttpRequest* HttpRequestPtr = &(HttpRequest.Get());
			void* HeaderAddress = reinterpret_cast<void*>(reinterpret_cast<int64>(HttpRequestPtr) + HeaderAddressOffset);
			TMap<FString, FString>* HeadersPtr = static_cast<TMap<FString, FString>*>(HeaderAddress);
			HeadersPtr->Remove(TEXT("Content-Length"));
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[UFigmaImporter] Unreal %i.%i.%i is not supported yet. Need to verify the Figma Request format."), ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ENGINE_PATCH_VERSION);
		}

		// End of Hack
	}

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
		ResetProgressBar();
	}
}

void UFigmaImporter::UpdateProgress(float ExpectedWorkThisFrame, const FText& Message)
{
	ProgressThisFrame += ExpectedWorkThisFrame;
	ProgressMessage = Message;
	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			UpdateProgressGameThread();
		});
}

void UFigmaImporter::UpdateProgressGameThread()
{
	const float WorkRemaining = Progress ? (Progress->TotalAmountOfWork - (Progress->CompletedWork + Progress->CurrentFrameScope)) : 0.0f;
	if (Progress && ProgressThisFrame > 0.0f && WorkRemaining > 0.0f)
	{
		Progress->EnterProgressFrame(FMath::Min(ProgressThisFrame, WorkRemaining), ProgressMessage);
		ProgressThisFrame = 0.0f;
	}
}

void UFigmaImporter::UpdateSubProgress(float ExpectedWorkThisFrame, const FText& Message)
{
	SubProgressThisFrame += ExpectedWorkThisFrame;
	SubProgressMessage = Message;
	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			UpdateSubProgressGameThread();
		});
}

void UFigmaImporter::UpdateSubProgressGameThread()
{
	const float WorkRemaining = SubProgress ? (SubProgress->TotalAmountOfWork - (SubProgress->CompletedWork + SubProgress->CurrentFrameScope)) : 0.0f;
	if (SubProgress && SubProgressThisFrame > 0.0f && WorkRemaining > 0.0f)
	{
		SubProgress->EnterProgressFrame(FMath::Min(SubProgressThisFrame, WorkRemaining), SubProgressMessage);
		SubProgressThisFrame = 0.0f;
	}
}

void UFigmaImporter::ResetProgressBar()
{
	if (Progress == nullptr)
		return;

	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		const FSlowTaskStack& Stack = GWarn->GetScopeStack();
		if(Progress != nullptr && Stack.Last() == Progress)
		{
		   delete Progress;
		   Progress = nullptr;
		   ProgressThisFrame = 0.0f;

		   delete SubProgress;
		   SubProgress = nullptr;
		   SubProgressThisFrame = 0.0f;
		}
		else
		{
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]()
				{
					ResetProgressBar();
				});		
		}
	});
}

TSharedPtr<FJsonObject> UFigmaImporter::ParseRequestReceived(FString MessagePrefix, FHttpResponsePtr HttpResponse)
{
	if (HttpResponse)
	{
#if (ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION > 3)
		const EHttpRequestStatus::Type status = HttpResponse->GetStatus();
		switch (status)
		{
		case EHttpRequestStatus::NotStarted:
			UE_LOG_Figma2UMG(Warning, TEXT("%s%s"), *MessagePrefix, TEXT("EVaRestRequestStatus::NotStarted."));
			break;
		case EHttpRequestStatus::Processing:
			UE_LOG_Figma2UMG(Warning, TEXT("%s%s"), *MessagePrefix, TEXT("EVaRestRequestStatus::Processing."));
			break;
		case EHttpRequestStatus::Failed:
			UpdateStatus(eRequestStatus::Failed, MessagePrefix + TEXT("EVaRestRequestStatus::Failed"));
			break;
#if (ENGINE_MAJOR_VERSION < 5 || ENGINE_MINOR_VERSION < 4)
		case EHttpRequestStatus::Failed_ConnectionError:
			UpdateStatus(eRequestStatus::Failed, MessagePrefix + TEXT("EVaRestRequestStatus::Failed_ConnectionError"));
			break;
#endif

		case EHttpRequestStatus::Succeeded:
#else
		const EHttpResponseCodes::Type status = static_cast<EHttpResponseCodes::Type>(HttpResponse->GetResponseCode());
		switch (status)
		{
		case EHttpResponseCodes::Ok:
#endif
		{
				UE_LOG_Figma2UMG(Display, TEXT("%s%s"), *MessagePrefix, TEXT("EVaRestRequestStatus::Succeeded"));
				TSharedPtr<FJsonObject> JsonObj;
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());

				bool DeserializeSuccess = FJsonSerializer::Deserialize(Reader, JsonObj) && JsonObj.IsValid();
				if (!DeserializeSuccess)
				{
					UpdateStatus(eRequestStatus::Failed, MessagePrefix + TEXT("HttpResponse has no response object"));

					return nullptr;
				}

				static FString StatusStr("status");
				static FString ErrorStr("err");
				if (JsonObj->HasField(StatusStr) && JsonObj->HasField(ErrorStr))
				{
					UpdateStatus(eRequestStatus::Failed, MessagePrefix + JsonObj->GetStringField(ErrorStr));

					return nullptr;
				}

				return JsonObj;
			}
#if (ENGINE_MAJOR_VERSION < 5 || ENGINE_MINOR_VERSION <= 3)
		default:
			UpdateStatus(eRequestStatus::Failed, MessagePrefix + TEXT("EHttpResponseCode(") + FString::FromInt(HttpResponse->GetResponseCode()) + TEXT(")"));
			break;
#endif
		}
	}
	else
	{
		UpdateStatus(eRequestStatus::Failed, MessagePrefix + TEXT("Result from Figma request is nullptr."));
	}
	return nullptr;
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

void UFigmaImporter::OnFigmaLibraryFileRequestReceived(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_ParseLib", "Parsing Library File."));
	;
	TSharedPtr<FJsonObject> JsonObj = ParseRequestReceived(TEXT("[Figma library file request] "), HttpResponse);
	if (JsonObj.IsValid())
	{
		static FString NameStr("Name");
		const FString FigmaFilename = UPackageTools::SanitizePackageName(JsonObj->GetStringField(NameStr));
		const FString FullFilename = FPaths::ProjectContentDir() + TEXT("../Downloads/") + FigmaFilename + TEXT("/") + FigmaFilename + TEXT(".figma");
		const FString RawText = HttpResponse->GetContentAsString();
		FFileHelper::SaveStringToFile(RawText, *FullFilename);

		UFigmaFile* CurrentFile = NewObject<UFigmaFile>();
		LibraryFileKeys[CurrentLibraryFileKey] = CurrentFile;

		AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, JsonObj, CurrentFile]()
			{
				constexpr int64 CheckFlags = 0;
				constexpr int64 SkipFlags = 0;
				constexpr bool StrictMode = false;
				FText OutFailReason;
				if (FJsonObjectConverter::JsonObjectToUStruct(JsonObj.ToSharedRef(), CurrentFile->StaticClass(), CurrentFile, CheckFlags, SkipFlags, StrictMode, &OutFailReason))
				{
					UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PostSerializeLib", "PostSerialize Library File."));
					CurrentFile->PostSerialize(CurrentLibraryFileKey, ContentRootFolder, JsonObj.ToSharedRef());
					CurrentFile->SetImporter(this);
					CurrentLibraryFileKey = nullptr;
					UE_LOG_Figma2UMG(Display, TEXT("Library file %s downloaded."), *CurrentFile->GetFileName());
					DownloadNextDependency();
				}
				else
				{
					CurrentLibraryFileKey = nullptr;
					UpdateStatus(eRequestStatus::Failed, OutFailReason.ToString());
				}
			});
	}
}

void UFigmaImporter::OnFigmaFileRequestReceived(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_ParseFile", "Parsing Design File."));

	TSharedPtr<FJsonObject> JsonObj = ParseRequestReceived(TEXT("[Figma file request] "), HttpResponse);
	if (JsonObj.IsValid())
	{
		static FString NameStr("Name");
		const FString FigmaFilename = UPackageTools::SanitizePackageName(JsonObj->GetStringField(NameStr));
		const FString FullFilename = FPaths::ProjectContentDir() + TEXT("../Downloads/") + FigmaFilename + TEXT("/") + FigmaFilename + TEXT(".figma");
		const FString RawText = HttpResponse->GetContentAsString();
		FFileHelper::SaveStringToFile(RawText, *FullFilename);

		File = NewObject<UFigmaFile>();

		AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, JsonObj]()
			{
				constexpr int64 CheckFlags = 0;
				constexpr int64 SkipFlags = 0;
				constexpr bool StrictMode = false;
				FText OutFailReason;
				if (FJsonObjectConverter::JsonObjectToUStruct(JsonObj.ToSharedRef(), File->StaticClass(), File, CheckFlags, SkipFlags, StrictMode, &OutFailReason))
				{
					UE_LOG_Figma2UMG(Display, TEXT("Post-Serialize"));
					UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PostSerializeFile", "PostSerialize Design File."));
					File->PostSerialize(FileKey, ContentRootFolder, JsonObj.ToSharedRef());
					File->SetImporter(this);

					FixReferences();
				}
				else
				{
					UpdateStatus(eRequestStatus::Failed, OutFailReason.ToString());
				}
			});
	}
}

void UFigmaImporter::FixReferences()
{
	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]()
		{
			UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_FixRefs", "Fixing component references."));
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

			UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_FixRefs", "Creating Builders."));
			File->CreateAssetBuilders(OnBuildersCreatedDelegate, AssetBuilders);
		});
}

void UFigmaImporter::OnBuildersCreated(bool Succeeded)
{
	if (Succeeded)
	{
		BuildImageDependency();
	}
	else
	{
		UpdateStatus(eRequestStatus::Failed, TEXT("Fail to create builders."));
	}
}

void UFigmaImporter::BuildImageDependency()
{
	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]()
		{
			UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_Image", "Building Image dependency."));
			UE_LOG_Figma2UMG(Display, TEXT("[Figma images Request]"));
			RequestedImages.Reset();

			RequestedImages.AddFile(FileKey);
			for (TScriptInterface<IAssetBuilder>& AssetBuilder : AssetBuilders)
			{
				if (UTexture2DBuilder* Texture2DBuilder = Cast<UTexture2DBuilder>(AssetBuilder.GetObject()))
				{
					Texture2DBuilder->AddImageRequest(RequestedImages);
				}
			}

			UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_Image", "Request Image's URLs."));
			RequestImageRefURLs();
		});
}

void UFigmaImporter::RequestImageRefURLs()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			if(FImagePerFileRequests* CurrentFile = RequestedImages.GetNextImageRefFile())
			{
				CurrentFile->ImageRefRequested = true;
				if (CreateRequest(FIGMA_ENDPOINT_FILES, CurrentFile->FileKey, FString(), "/images", OnVaRestImagesRefRequestDelegate))
				{
					UE_LOG_Figma2UMG(Display, TEXT("[Figma images Request] Requesting imageRefs for file %s from Figma API."), *CurrentFile->FileKey);
				}
			}
			else
			{
				AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]()
					{
						RequestImageURLs();
					});
			}
		});
}

void UFigmaImporter::OnFigmaImagesRefURLReceived(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TSharedPtr<FJsonObject> JsonObj = ParseRequestReceived(TEXT("[Figma images request] "), HttpResponse);
	if (JsonObj.IsValid())
	{
		constexpr int64 CheckFlags = 0;
		constexpr int64 SkipFlags = 0;
		constexpr bool StrictMode = false;
		FText OutFailReason;
		FImagesRefRequestResult ImagesRefRequestResult;
		if (FJsonObjectConverter::JsonObjectToUStruct(JsonObj.ToSharedRef(), &ImagesRefRequestResult, CheckFlags, SkipFlags, StrictMode, &OutFailReason))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[Figma images Request] %u images received from Figma API."), ImagesRefRequestResult.Meta.Images.Num());
			for (TPair<FString, FString> Element : ImagesRefRequestResult.Meta.Images)
			{
				if (Element.Value.IsEmpty())
				{
					UE_LOG_Figma2UMG(Warning, TEXT("[Figma images Request] Coudn't get URL for Id %s."), *Element.Key);
					continue;
				}
				RequestedImages.SetURLFromImageRef(Element.Key, Element.Value);
			}

			RequestImageRefURLs();
		}
		else
		{
			UpdateStatus(eRequestStatus::Failed, OutFailReason.ToString());
		}
	}
}

void UFigmaImporter::RequestImageURLs()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			if (FImagePerFileRequests* Requests = RequestedImages.GetRequestsPendingURL())
			{
				FString ImageIdsFormated;
				FString ImageRef;
				int RequestCount = 0;
				for (int i = 0; i < Requests->Requests.Num() && RequestCount < 30; i++)
				{
					if(!Requests->Requests[i].URL.IsEmpty())
						continue;

					if(ImageIdsFormated.IsEmpty())
					{
						ImageIdsFormated += Requests->Requests[i].Id;
					}
					else
					{
						ImageIdsFormated += "," + Requests->Requests[i].Id;
					}
					Requests->Requests[i].SetRequestedURL();
					RequestCount++;
				}

				if (!ImageIdsFormated.IsEmpty())
				{
					ImageIdsFormated += "&scale=" + FString::SanitizeFloat(NodeImageScale, 0);
				   if (CreateRequest(FIGMA_ENDPOINT_IMAGES, Requests->FileKey, ImageIdsFormated, OnVaRestImagesRequestDelegate))
				   {
					   UE_LOG_Figma2UMG(Display, TEXT("[Figma images Request] Requesting %u images in file %s from Figma API."), Requests->Requests.Num(), *Requests->FileKey);
				   }
				   return;
				}
			}

			if (SubProgress)
			{
				delete SubProgress;
				SubProgress = nullptr;
				SubProgressThisFrame = 0.0f;
			}
			SubProgress = new FScopedSlowTask(100, NSLOCTEXT("Figma2UMG", "Figma2UMG_ImportProgress", "Importing from FIGMA"));
			SubProgress->MakeDialog();

			UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_Image", "Downloading Images."));

			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]()
				{
					DownloadNextImage();
				});
		});
}

void UFigmaImporter::OnFigmaImagesURLReceived(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TSharedPtr<FJsonObject> JsonObj = ParseRequestReceived(TEXT("[Figma images request] "), HttpResponse);
	if (JsonObj.IsValid())
	{
		constexpr int64 CheckFlags = 0;
		constexpr int64 SkipFlags = 0;
		constexpr bool StrictMode = false;
		FText OutFailReason;
		if (FJsonObjectConverter::JsonObjectToUStruct(JsonObj.ToSharedRef(), &ImagesRequestResult, CheckFlags, SkipFlags, StrictMode, &OutFailReason))
		{
			UE_LOG_Figma2UMG(Display, TEXT("[Figma images Request] %u images received from Figma API."), ImagesRequestResult.Images.Num());
			for (TPair<FString, FString> Element : ImagesRequestResult.Images)
			{
				if(Element.Value.IsEmpty())
				{
					UE_LOG_Figma2UMG(Warning, TEXT("[Figma images Request] Coudn't get URL for Id %s."), *Element.Key);
					continue;
				}
				RequestedImages.SetURL(Element.Key, Element.Value);
			}

			ImageDownloadCount = 0;
			RequestImageURLs();
		}
		else
		{
			UpdateStatus(eRequestStatus::Failed, OutFailReason.ToString());
		}
	}
}

void UFigmaImporter::DownloadNextImage()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			FImageRequest* ImageRequest = RequestedImages.GetNextToDownload();
			if (ImageRequest && ImageRequest->GetRequestedURL() && !ImageRequest->URL.IsEmpty())
			{
				ImageDownloadCount++;
				const float ImageCountTotal = RequestedImages.GetCurrentRequestTotalCount();

				TArray<FStringFormatArg> args;
				args.Add(ImageDownloadCount);
				args.Add(static_cast<int>(ImageCountTotal));
				FString msg = FString::Format(TEXT("Downloading Image {0} of {1}"), args);

				UpdateSubProgress(100.f / ImageCountTotal, FText::FromString(msg));

				UE_LOG_Figma2UMG(Display, TEXT("Downloading image (%i/%i) %s at %s."), ImageDownloadCount, static_cast<int>(ImageCountTotal), *ImageRequest->ImageName, *ImageRequest->URL);
				ImageRequest->StartDownload(OnImageDownloadRequestCompleted);
			}
			else
			{
				AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]()
					{
						if (DownloadFontsFromGoogle)
						{
							FetchGoogleFontsList();
						}
						else
						{
							LoadOrCreateAssets();
						}
					});
			}
		});
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

void UFigmaImporter::FetchGoogleFontsList()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if (SubProgress)
		{
			delete SubProgress;
			SubProgress = nullptr;
			SubProgressThisFrame = 0.0f;
		}

		UpdateProgress(1, NSLOCTEXT("Figma2UMG", "Figma2UMG_GFontRequest", "Managing Fonts."));

		SubProgress = new FScopedSlowTask(100, NSLOCTEXT("Figma2UMG", "Figma2UMG_GFontRequest", "Requesting Font list from Google"));
		SubProgress->MakeDialog();
		UpdateSubProgress(5, NSLOCTEXT("Figma2UMG", "Figma2UMG_GFontRequest", "Requesting Font list from Google."));


		const UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
		if (Importer && !Importer->HasGoogleFontsInfo())
		{
				TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
				HttpRequest->OnProcessRequestComplete().BindUObject(this, &UFigmaImporter::OnFetchGoogleFontsResponse);
				FString URL = "https://www.googleapis.com/webfonts/v1/webfonts?key=" + GFontsAPIKey;
				HttpRequest->SetURL(URL);
				HttpRequest->SetVerb(TEXT("GET"));
				HttpRequest->ProcessRequest();
		}
		else
		{
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this](){BuildFontDependency();});
		}
	});
}

void UFigmaImporter::OnFetchGoogleFontsResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bWasSuccessful)
{
	UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
	if (Importer && bWasSuccessful && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == EHttpResponseCodes::Ok)
	{
		const FString FullFilename = FPaths::ProjectContentDir() + TEXT("../Downloads/Fonts/GFontList.json");
		FFileHelper::SaveArrayToFile(HttpResponse->GetContent(), *FullFilename);

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());

		TArray<FGFontFamilyInfo>& GoogleFontsInfo = Importer->GetGoogleFontsInfo();
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			const TArray<TSharedPtr<FJsonValue>>* Items;
			if (JsonObject->TryGetArrayField(TEXT("items"), Items))
			{
				for (const TSharedPtr<FJsonValue>& Item : *Items)
				{
					const TSharedPtr<FJsonObject> FontObject = Item->AsObject();
					FGFontFamilyInfo& FontFamilyInfo = GoogleFontsInfo.Emplace_GetRef();


					constexpr int64 CheckFlags = 0;
					constexpr int64 SkipFlags = 0;
					constexpr bool StrictMode = false;
					FText OutFailReason;
					if (FJsonObjectConverter::JsonObjectToUStruct(FontObject.ToSharedRef(), &FontFamilyInfo, CheckFlags, SkipFlags, StrictMode, &OutFailReason))
					{
						FontFamilyInfo.Family = UPackageTools::SanitizePackageName(FontFamilyInfo.Family.Replace(TEXT(" "), TEXT("")));
					}
					else
					{
						FString Family = FontObject->GetStringField(TEXT("family"));
						UE_LOG_Figma2UMG(Warning, TEXT("[UFigmaImporter] Failed to parse Google Font Family %s"), *Family);
					}
				}
			}
		}

		if (!GoogleFontsInfo.IsEmpty())
		{
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]() {BuildFontDependency(); });
		}
		else
		{
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]() {LoadOrCreateAssets(); });
		}
	}
	else
	{
		const TArray<uint8>& Content = HttpResponse.Get()->GetContent();
		FString ErrorContent = BytesToString(Content.GetData(), Content.Num());
		UE_LOG_Figma2UMG(Warning, TEXT("[UFigmaImporter] Failed to Fetch Google Fonts's list. Response %s"), *ErrorContent);

		AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]() {LoadOrCreateAssets(); });
	}
}

void UFigmaImporter::BuildFontDependency()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			UpdateSubProgress(5, NSLOCTEXT("Figma2UMG", "Figma2UMG_GFontRequest", "Requesting Font list from Google."));
			UE_LOG_Figma2UMG(Display, TEXT("[Figma GFonts Request]"));
			RequestedFonts.Reset();

			for (TScriptInterface<IAssetBuilder>& AssetBuilder : AssetBuilders)
			{
				if (UFontBuilder* FontBuilder = Cast<UFontBuilder>(AssetBuilder.GetObject()))
				{
					FontBuilder->AddFontRequest(RequestedFonts);
				}
			}

			FontDownloadCount = 0;
			DownloadNextFont();
		});
}

void UFigmaImporter::DownloadNextFont()
{
	FGFontRequest* FontRequest = RequestedFonts.GetNextToDownload();
	if (FontRequest)
	{
		FontDownloadCount++;
		const float FontCountTotal = RequestedFonts.GetRequestTotalCount();

		TArray<FStringFormatArg> args;
		args.Add(FontDownloadCount);
		args.Add(static_cast<int>(FontCountTotal));
		FString msg = FString::Format(TEXT("Downloading font {0} of {1}"), args);

		UpdateSubProgress(80.f / FontCountTotal, FText::FromString(msg));

		UE_LOG_Figma2UMG(Display, TEXT("Downloading font (%i/%i) %s(%s) at %s."), FontDownloadCount, static_cast<int>(FontCountTotal), *FontRequest->FamilyInfo->Family, *FontRequest->Variant, *FontRequest->GetURL());
		FontRequest->StartDownload(OnFontDownloadRequestCompleted);
	}
	else
	{
		LoadOrCreateAssets();
	}
}

void UFigmaImporter::HandleFontDownload(bool Succeeded)
{
	if (Succeeded)
	{
		DownloadNextFont();
	}
	else
	{
		UE_LOG_Figma2UMG(Error, TEXT("Failed to download font."));
		AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]() {LoadOrCreateAssets(); });
	}
}

void UFigmaImporter::LoadOrCreateAssets()
{
	UpdateProgress(1, NSLOCTEXT("Figma2UMG", "Figma2UMG_LoadOrCreateAssets", "Loading or create UAssets"));
	UE_LOG_Figma2UMG(Display, TEXT("Creating UAssets"));

	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			if (SubProgress)
			{
				delete SubProgress;
				SubProgress = nullptr;
				SubProgressThisFrame = 0.0f;
			}

			FGCScopeGuard GCScopeGuard;
			for (TScriptInterface<IAssetBuilder>& AssetBuilder : AssetBuilders)
			{
				AssetBuilder->LoadOrCreateAssets();
			}

			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]() {OnAssetsCreated(true); });
		});
}

void UFigmaImporter::OnAssetsCreated(bool Succeeded)
{
	if (!Succeeded)
	{
		UpdateStatus(eRequestStatus::Failed, TEXT("Fail to create UAssets."));
		return;
	}

	UE_LOG_Figma2UMG(Display, TEXT("Patching UAssets."));
	CreateWidgetBuilders();
}

void UFigmaImporter::CreateWidgetBuilders()
{
	UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_CreateWidgetBuilders", "Creating UWidget Builders"));

	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			FGCScopeGuard GCScopeGuard;
			for (TScriptInterface<IAssetBuilder>& AssetBuilder : AssetBuilders)
			{
				if (const TObjectPtr<UWidgetBlueprintBuilder> BlueprintBuilder = Cast<UWidgetBlueprintBuilder>(AssetBuilder.GetObject()))
				{
					BlueprintBuilder->CreateWidgetBuilders();
				}
			}

			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]() {PatchPreInsertWidget(); });
		});
}

void UFigmaImporter::PatchPreInsertWidget()
{
	UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PatchPreInsertWidget", "Patch PreInsert Widgets"));

	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			FGCScopeGuard GCScopeGuard;
			for (TScriptInterface<IAssetBuilder>& AssetBuilder : AssetBuilders)
			{
				if (const TObjectPtr<UWidgetBlueprintBuilder> BlueprintBuilder = Cast<UWidgetBlueprintBuilder>(AssetBuilder.GetObject()))
				{
					BlueprintBuilder->PatchAndInsertWidgets();
				}
			}

			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]() {CompileBPs(true); });
		});
}

void UFigmaImporter::CompileBPs(bool ProceedToNextState)
{
	UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PatchPreInsertWidget", "Compiling BluePrints"));

	AsyncTask(ENamedThreads::GameThread, [this, ProceedToNextState]()
		{
			FGCScopeGuard GCScopeGuard;
			for (TScriptInterface<IAssetBuilder>& AssetBuilder : AssetBuilders)
			{
				if (const TObjectPtr<UWidgetBlueprintBuilder> BlueprintBuilder = Cast<UWidgetBlueprintBuilder>(AssetBuilder.GetObject()))
				{
					BlueprintBuilder->CompileBP(EBlueprintCompileOptions::None);
				}
			}

			if (ProceedToNextState)
			{
				AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]() {ReloadBPAssets(true); });
			}
		});
}

void UFigmaImporter::ReloadBPAssets(bool ProceedToNextState)
{
	UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PatchPreInsertWidget", "Reloading compiled BluePrints"));

	AsyncTask(ENamedThreads::GameThread, [this, ProceedToNextState]()
		{
			FGCScopeGuard GCScopeGuard;
			for (TScriptInterface<IAssetBuilder>& AssetBuilder : AssetBuilders)
			{
				if (const TObjectPtr<UWidgetBlueprintBuilder> BlueprintBuilder = Cast<UWidgetBlueprintBuilder>(AssetBuilder.GetObject()))
				{
					BlueprintBuilder->LoadAssets();
					BlueprintBuilder->ResetWidgets();
				}
			}

			if (ProceedToNextState)
			{
				AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]() {PatchWidgetBinds(); });
			}
		});
}

void UFigmaImporter::PatchWidgetBinds()
{
	UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PatchPreInsertWidget", "Patching Widget Binds"));

	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			FGCScopeGuard GCScopeGuard;
			for (TScriptInterface<IAssetBuilder>& AssetBuilder : AssetBuilders)
			{
				if (const TObjectPtr<UWidgetBlueprintBuilder> BlueprintBuilder = Cast<UWidgetBlueprintBuilder>(AssetBuilder.GetObject()))
				{
					BlueprintBuilder->PatchWidgetBinds();
				}
			}

			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]() {PatchWidgetProperties(); });
		});
}

void UFigmaImporter::PatchWidgetProperties()
{
	UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PatchPreInsertWidget", "Patching Widget Properties"));

	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			FGCScopeGuard GCScopeGuard;
			for (TScriptInterface<IAssetBuilder>& AssetBuilder : AssetBuilders)
			{
				if (const TObjectPtr<UWidgetBlueprintBuilder> BlueprintBuilder = Cast<UWidgetBlueprintBuilder>(AssetBuilder.GetObject()))
				{
					BlueprintBuilder->PatchWidgetProperties();
				}
			}

			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]() {UFigmaImporter::OnPatchUAssets(true); });
		});
}

void UFigmaImporter::OnPatchUAssets(bool Succeeded)
{
	if (!Succeeded)
	{
		UpdateStatus(eRequestStatus::Failed, TEXT("Fail to patch UAssets."));
		return;
	}

	
	UE_LOG_Figma2UMG(Display, TEXT("Post-patch UAssets."));
	UpdateProgress(1.0f, NSLOCTEXT("Figma2UMG", "Figma2UMG_PostPatch", "Post-patch UAssets"));

	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			if(SaveAllAtEnd)
			{
				CompileBPs(false);
				ReloadBPAssets(false);
				SaveAll();
			}
			else
			{
				CompileBPs(false);
			}

			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]() {OnPostPatchUAssets(true); });
		});
}

void UFigmaImporter::SaveAll()
{
	TArray<UPackage*> Packages;
	for (const TScriptInterface<IAssetBuilder>& AssetBuilder : AssetBuilders)
	{
		AssetBuilder->AddPackages(Packages);
	}
#if (ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3)
	FEditorFileUtils::FPromptForCheckoutAndSaveParams Params;
	FEditorFileUtils::PromptForCheckoutAndSave(Packages, Params);
#else
	FEditorFileUtils::PromptForCheckoutAndSave(Packages, true, false);
#endif
}

void UFigmaImporter::OnPostPatchUAssets(bool Succeeded)
{
	AsyncTask(ENamedThreads::GameThread, [this, Succeeded]()
		{
			for (const TScriptInterface<IAssetBuilder>& AssetBuilder : AssetBuilders)
			{
				AssetBuilder->Reset();
			}
			AssetBuilders.Reset();
			if (Succeeded)
			{
				UpdateStatus(eRequestStatus::Succeeded, File->GetFileName() + TEXT(" was successfully imported."));
			}
			else
			{
				UpdateStatus(eRequestStatus::Failed, TEXT("Failed at Post-patch of UAssets."));
			}
		});
}