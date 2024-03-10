// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/FigmaRequest.h"

#include "Defines.h"
#include "Figma2UMGModule.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

void IFigmaRequest::StartDownload()
{
	Status = eRequestStatus::Processing;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindRaw(this, &IFigmaRequest::HandleFigmaDownload);
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(FIGMA_ACCESSTOLKENS_HEADER, AccessToken);
	HttpRequest->SetHeader(FString("Host"), FIGMA_HOST);
	HttpRequest->ProcessRequest();


	// This section bellow is a hack due to the FCurlHttpRequest::SetupRequest() always adding the header Content-Length. Adding it makes the Figma AIP return the error 400 
	// To avoid reimplementing the curl class, we need to maually remove the Header item.
	// This will need update and check if it has any change in the FCurlHttpRequest size so the memory offset of Header changed.

	if (!HttpRequest->GetHeader(TEXT("Content-Length")).IsEmpty())
	{
		int HeaderAddressOffset = 0;

#if WITH_CURL
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 3 && ENGINE_PATCH_VERSION == 2)
		HeaderAddressOffset = 256;
#endif
#endif

		if (HeaderAddressOffset > 0)
		{
			IHttpRequest* HttpRequestPtr = &(HttpRequest.Get());
			void* HeaderAddress = reinterpret_cast<void*>(reinterpret_cast<int64>(HttpRequestPtr) + HeaderAddressOffset);
			TMap<FString, FString>* HeadersPtr = static_cast<TMap<FString, FString>*>(HeaderAddress);
			HeadersPtr->Remove(TEXT("Content-Length"));
		}
	}
	// End of Hack
}

void IFigmaRequest::HandleFigmaDownload(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{
		const int dataSize = HttpResponse->GetContentLength();

		TArray<uint8> RawData;
		RawData.Empty(dataSize);
		RawData.AddUninitialized(dataSize);
		FMemory::Memcpy(RawData.GetData(), HttpResponse->GetContent().GetData(), dataSize);

		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
		TSharedPtr<FJsonObject> OutJsonObj;
		if (FJsonSerializer::Deserialize(Reader, OutJsonObj))
		{
			Status = eRequestStatus::Succeeded;
			HandleFigmaDownload(RawData, OutJsonObj.ToSharedRef());
		}
		else
		{
			Status = eRequestStatus::Failed;

			UE_LOG_Figma2UMG(Error, TEXT("Failed to download %s. Result= %s"), *URL, *HttpResponse->GetContentAsString());
			TArray<uint8> Empty;
			TSharedRef<FJsonObject> EmptyJsonObj = MakeShared<FJsonObject>();
			HandleFigmaDownload(Empty, EmptyJsonObj);
		}
	}
	else
	{
		Status = eRequestStatus::Failed;

		UE_LOG_Figma2UMG(Error, TEXT("Failed to download %s."), *URL);
		TArray<uint8> Empty;
		TSharedRef<FJsonObject> EmptyJsonObj = MakeShared<FJsonObject>();
		HandleFigmaDownload(Empty, EmptyJsonObj);
	}
}
