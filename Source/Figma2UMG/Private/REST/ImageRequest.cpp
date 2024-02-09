// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/ImageRequest.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"


void FImageRequest::StartDownload(const FOnImageRequestCompleteDelegate& Delegate)
{
	OnImageRequestCompleteDelegate = Delegate;
	Status = eRequestStatus::Processing;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FImageRequest::HandleImageDownload);
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();

}

void FImageRequest::HandleImageDownload(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{
		const int dataSize = HttpResponse->GetContentLength();

		TArray<uint8> RawData;
		RawData.Empty(dataSize);
		RawData.AddUninitialized(dataSize);
		FMemory::Memcpy(RawData.GetData(), HttpResponse->GetContent().GetData(), dataSize);

		//ImportedTexture.Set(RawData);

		OnImageRequestCompleteDelegate.ExecuteIfBound(true);
	}
	else
	{
		OnImageRequestCompleteDelegate.ExecuteIfBound(false);
	}

	//if (HttpRequest)
	//{
	//	HttpRequest->OnProcessRequestComplete().Unbind();
	//}
}

void FImageRequests::AddRequest(const FString& ImageName, const FString& Id)
{
	FImageRequest& Request = Requests.Emplace_GetRef();
	Request.ImageName = ImageName;
	Request.Id = Id;
}

void FImageRequests::Reset()
{
	Requests.Reset();
}

void FImageRequests::SetURL(const FString& Id, const FString& URL)
{
	FImageRequest* FoundRequest = Requests.FindByPredicate([Id](const FImageRequest& Request) { return (Request.Id == Id); });
	if (FoundRequest)
	{
		FoundRequest->URL = URL;
	}
}

FImageRequest* FImageRequests::GetNextToDownload() const
{
	return nullptr;
}