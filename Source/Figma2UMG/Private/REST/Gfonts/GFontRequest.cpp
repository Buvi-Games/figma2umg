// Copyright 2024 Buvi Games. All Rights Reserved.


#include "REST/Gfonts/GFontRequest.h"

#include "FigmaImportSubsystem.h"
#include "Figma2UMGModule.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

void FGFontRequest::StartDownload(const FOnFontRequestCompleteDelegate& Delegate)
{
	OnFontRequestCompleteDelegate = Delegate;
	if(FamilyInfo == nullptr)
	{
		Status = eRequestStatus::Failed;
		OnFontRequestCompleteDelegate.ExecuteIfBound(false);

		return;
	}
	Status = eRequestStatus::Processing;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FGFontRequest::HandleFontDownload);
	HttpRequest->SetURL(FamilyInfo->URL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();

}

void FGFontRequest::HandleFontDownload(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{
		const int dataSize = HttpResponse->GetContentLength();

		TArray<uint8> RawData;
		RawData.Empty(dataSize);
		RawData.AddUninitialized(dataSize);
		FMemory::Memcpy(RawData.GetData(), HttpResponse->GetContent().GetData(), dataSize);

		if (OnFontRawReceive.IsBound())
		{
			OnFontRawReceive.Broadcast(RawData);
		}

		Status = eRequestStatus::Succeeded;
		OnFontRequestCompleteDelegate.ExecuteIfBound(true);
	}
	else
	{
		Status = eRequestStatus::Failed;

		UE_LOG_Figma2UMG(Warning, TEXT("Failed to download image at %s."), *FamilyInfo->URL);
		OnFontRequestCompleteDelegate.ExecuteIfBound(false);
	}
}

void FFontRequests::AddRequest(const FString& FamilyName, const FOnRawFontFileReceive::FDelegate& OnFontRawReceive)
{
	UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
	if (FGFontFamilyInfo* FontFamilyInfo = Importer ? Importer->FindGoogleFontsInfo(FamilyName) : nullptr)
	{
		FGFontRequest* Request = RequestedFamilies.FindByPredicate([FontFamilyInfo](const FGFontRequest& GFontRequest)
			{
				return GFontRequest.FamilyInfo == FontFamilyInfo;
			});

		if(Request)
		{
			Request->OnFontRawReceive.Add(OnFontRawReceive);
		}
		else
		{
			FGFontRequest& NewRequest = RequestedFamilies.Emplace_GetRef();
			NewRequest.FamilyInfo = FontFamilyInfo;
			NewRequest.OnFontRawReceive.Add(OnFontRawReceive);
		}
		
	}
}

void FFontRequests::Reset()
{
	RequestedFamilies.Reset();
}

FGFontRequest* FFontRequests::GetNextToDownload()
{
	return RequestedFamilies.FindByPredicate([](const FGFontRequest& Request) { return (Request.GetStatus() == eRequestStatus::NotStarted && Request.FamilyInfo != nullptr); });
}

int FFontRequests::GetRequestTotalCount()
{
	int Count = 0;
	for (const FGFontRequest& Request : RequestedFamilies)
	{
		if (Request.FamilyInfo != nullptr)
			Count++;
	}

	return Count;
}
