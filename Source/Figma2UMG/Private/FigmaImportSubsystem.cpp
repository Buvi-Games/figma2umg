// Copyright 2024 Buvi Games. All Rights Reserved.

#include "FigmaImportSubsystem.h"

#include "REST/FigmaImporter.h"
#include "REST/RequestParams.h"


UFigmaImporter* UFigmaImportSubsystem::Request(const TObjectPtr<URequestParams> InProperties, const FOnFigmaImportUpdateStatusCB& InRequesterCallback)
{
	UFigmaImporter* request = Requests.Emplace_GetRef(NewObject<UFigmaImporter>());
	WidgetOverrides = &InProperties->WidgetOverrides;
	request->Init(InProperties, InRequesterCallback);;
	request->Run();
	return request;
}

void UFigmaImportSubsystem::RemoveRequest(UFigmaImporter* FigmaImporter)
{
	Requests.Remove(FigmaImporter);
}

void UFigmaImportSubsystem::TryRenameWidget(const FString& InName, TObjectPtr<UWidget> Widget)
{
	if (!Widget)
		return;

	if (Widget->GetName().Contains(InName, ESearchCase::IgnoreCase))
		return;

	const FString UniqueName = MakeUniqueObjectName(Widget->GetOuter(), Widget->GetClass(), *InName).ToString();
	Widget->Rename(*UniqueName);
}
