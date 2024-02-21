// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/RequestParams.h"

#include "Figma2UMGModule.h"
#include "Figma2UMGSettings.h"

URequestParams::URequestParams(const FObjectInitializer& ObjectInitializer)
{
	FFigma2UMGModule& Figma2UMGModule = FModuleManager::LoadModuleChecked<FFigma2UMGModule>("Figma2UMG");
	UFigma2UMGSettings* Settings = Figma2UMGModule.GetSettings();
	if (Settings)
	{
		AccessToken = Settings->AccessToken;
		FileKey = Settings->FileKey;
		LibraryFileKeys = Settings->LibraryFileKeys;
	}

	ContentRootFolder = "/Game/Figma";

	//#if !UE_BUILD_SHIPPING
	Ids.Add("146:1357");
	//#endif

}
