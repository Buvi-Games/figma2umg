// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/ImagesGenerationRequest.h"

#include "Figma2UMGModule.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"


void FImagesGenerationRequest::StartDownload(const FOnImageGenerationRequestCompleteDelegate& Delegate)
{
	OnImageRequestCompleteDelegate = Delegate;
	Status = eRequestStatus::Processing;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FImagesGenerationRequest::HandleFigmaDownload);
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();

}

void FImagesGenerationRequest::HandleFigmaDownload(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{
		const int dataSize = HttpResponse->GetContentLength();

		TArray<uint8> RawData;
		RawData.Empty(dataSize);
		RawData.AddUninitialized(dataSize);
		FMemory::Memcpy(RawData.GetData(), HttpResponse->GetContent().GetData(), dataSize);

		Status = eRequestStatus::Succeeded;
		FImagesRequestResult result;
		OnImageRequestCompleteDelegate.ExecuteIfBound(true, result);
	}
	else
	{
		Status = eRequestStatus::Failed;

		UE_LOG_Figma2UMG(Warning, TEXT("Failed to download image at %s."), *URL);
		OnImageRequestCompleteDelegate.ExecuteIfBound(false, FImagesRequestResult());
	}

	//if (HttpRequest)
	//{
	//	HttpRequest->OnProcessRequestComplete().Unbind();
	//}
}
