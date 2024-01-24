// Fill out your copyright notice in the Description page of Project Settings.

#include "REST/RequestWrapper.h"

#include "Figma2UMGModule.h"
#include "REST/Defines.h"

#define LOCTEXT_NAMESPACE "Figma2UMG"

URequestWrapper::URequestWrapper(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OnVaRestDelegate.BindUFunction(this, FName("OnVaRestCB"));
}

void URequestWrapper::SetCallback(const FOnVaRestCB& callback)
{
	RequesterCallback = callback;
}

void URequestWrapper::Reset()
{
	RequesterCallback.Unbind();
}

bool URequestWrapper::Request(FString AccessToken, FString FileKey, int Pages)
{
	if (!RequesterCallback.IsBound())
		return false;

	UVaRestSubsystem* VARestSubsystem = GEngine->GetEngineSubsystem<UVaRestSubsystem>();
	if (!VARestSubsystem)
		return false;

	AddToRoot();

	TArray<FStringFormatArg> args;
	args.Add(FIGMA_BASE_URL);
	args.Add(FIGMA_ENDPOINT_FILES);
	args.Add(FileKey);
	FString URL = FString::Format(TEXT("{0}{1}{2}"), args);

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
	Response.Callback = OnVaRestDelegate;

	Response.CompleteDelegateHandle = Request->OnStaticRequestComplete.AddUObject(this, &URequestWrapper::OnRequestComplete);
	Response.FailDelegateHandle = Request->OnStaticRequestFail.AddUObject(this, &URequestWrapper::OnRequestFail);

	Request->ResetResponseData();
	Request->ProcessURL(URL);

	// This section bellow is a hack due to the FCurlHttpRequest::SetupRequest() always adding the header Content-Length. Adding it makes the Figma AIP return the error 400 
	// To avoid reimplementing the curl class, we need to maually remove the Header item.
	// This will need update and check if it has any change in the FCurlHttpRequest size so the memory offset of Header changed.

	int HeaderAddressOffset = 0;

#if WITH_CURL
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 3 && ENGINE_PATCH_VERSION == 2)
	HeaderAddressOffset = 256;
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


void URequestWrapper::OnVaRestCB(UVaRestRequestJSON* Request)
{
	if (RequesterCallback.IsBound())
	{
		RequesterCallback.Execute(Request);
	}
}


void URequestWrapper::OnRequestComplete(UVaRestRequestJSON* Request)
{
	Request->OnStaticRequestComplete.Remove(Response.CompleteDelegateHandle);
	Request->OnStaticRequestFail.Remove(Response.FailDelegateHandle);

	Response.Callback.ExecuteIfBound(Request);
	Response.Request = nullptr;
	RemoveFromRoot();
}


void URequestWrapper::OnRequestFail(UVaRestRequestJSON* Request)
{
	Request->OnStaticRequestComplete.Remove(Response.CompleteDelegateHandle);
	Request->OnStaticRequestFail.Remove(Response.FailDelegateHandle);

	Response.Callback.ExecuteIfBound(Request);
	Response.Request = nullptr;
	RemoveFromRoot();
}

#undef LOCTEXT_NAMESPACE
