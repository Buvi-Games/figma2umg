// Fill out your copyright notice in the Description page of Project Settings.


#include "FigmaImportSubsystem.h"


UFigmaImporter* UFigmaImportSubsystem::Request(const FString& InAccessToken, const FString& InFileKey, const FString& InIds, const FString& InContentRootFolder, const FOnFigmaImportUpdateStatusCB& InRequesterCallback)
{
	UFigmaImporter* request = Requests.Emplace_GetRef(NewObject<UFigmaImporter>());
	request->Init(InAccessToken, InFileKey, InIds, InContentRootFolder, InRequesterCallback);
	request->Run();
	return request;
}

void UFigmaImportSubsystem::RemoveRequest(UFigmaImporter* FigmaImporter)
{
	Requests.Remove(FigmaImporter);
}
