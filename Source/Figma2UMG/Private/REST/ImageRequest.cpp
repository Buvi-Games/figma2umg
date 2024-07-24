// Copyright 2024 Buvi Games. All Rights Reserved.


#include "REST/ImageRequest.h"

#include "Figma2UMGModule.h"
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

		OnImageRawReceive.ExecuteIfBound(RawData);

		Status = eRequestStatus::Succeeded;
		OnImageRequestCompleteDelegate.ExecuteIfBound(true);
	}
	else
	{
		Status = eRequestStatus::Failed;

		UE_LOG_Figma2UMG(Warning, TEXT("Failed to download image at %s."), *URL);
		OnImageRequestCompleteDelegate.ExecuteIfBound(false);
	}
}

void FImageRequests::AddRequest(FString FileKey, const FString& ImageName, const FString& Id, const FOnRawImageReceiveDelegate& OnImageRawReceive)
{
	FImagePerFileRequests* FileRequest = RequestsPerFile.FindByPredicate([FileKey](const FImagePerFileRequests& Request) { return (Request.FileKey == FileKey); });
	if (!FileRequest)
	{
		FileRequest = &RequestsPerFile.Emplace_GetRef();
		FileRequest->FileKey = FileKey;
	}

	FImageRequest& Request = FileRequest->Requests.Emplace_GetRef();
	Request.ImageName = ImageName;
	Request.Id = Id;
	Request.OnImageRawReceive = OnImageRawReceive;
}

void FImageRequests::Reset()
{
	RequestsPerFile.Reset();
}

const FImagePerFileRequests* FImageRequests::GetRequests() const
{
	if (RequestsPerFile.IsEmpty())
		return nullptr;

	return &RequestsPerFile[0];
}

void FImageRequests::SetURL(const FString& Id, const FString& URL)
{
	FImagePerFileRequests& CurrentFile = RequestsPerFile[0];
	FImageRequest* FoundRequest = CurrentFile.Requests.FindByPredicate([Id](const FImageRequest& Request) { return (Request.Id == Id); });
	if (FoundRequest)
	{
		FoundRequest->URL = URL;
	}
}

FImageRequest* FImageRequests::GetNextToDownload()
{
	FImagePerFileRequests& CurrentFile = RequestsPerFile[0];
	FImageRequest* ImageRequest = CurrentFile.Requests.FindByPredicate([](const FImageRequest& Request) { return (Request.GetStatus() == eRequestStatus::NotStarted && !Request.URL.IsEmpty()); });
	if(ImageRequest == nullptr)
	{
		RequestsPerFile.RemoveAt(0);
	}
	return ImageRequest;
}

int FImageRequests::GetCurrentRequestTotalCount() const
{
	if (RequestsPerFile.Num() == 0)
		return 0;

	int Count = 0;
	const FImagePerFileRequests& CurrentFile = RequestsPerFile[0];
	for(const FImageRequest& Request : CurrentFile.Requests)
	{
		if (!Request.URL.IsEmpty())
			Count++;
	}

	return Count;
}
