// Fill out your copyright notice in the Description page of Project Settings.


#include "FigmaImportSubsystem.h"

#include "UI/SImporterWidget.h"


UFigmaImporter* UFigmaImportSubsystem::Request(const TObjectPtr<URequestParams> InProperties, const FOnFigmaImportUpdateStatusCB& InRequesterCallback)
{
	UFigmaImporter* request = Requests.Emplace_GetRef(NewObject<UFigmaImporter>());
	request->Init(InProperties, InRequesterCallback);
	request->Run();
	return request;
}

void UFigmaImportSubsystem::RemoveRequest(UFigmaImporter* FigmaImporter)
{
	Requests.Remove(FigmaImporter);
}
