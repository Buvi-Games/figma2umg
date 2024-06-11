// Copyright 2024 Buvi Games. All Rights Reserved.

#include "FigmaImportSubsystem.h"

#include "REST/FigmaImporter.h"
#include "REST/RequestParams.h"


UFigmaImporter* UFigmaImportSubsystem::Request(const TObjectPtr<URequestParams> InProperties, const FOnFigmaImportUpdateStatusCB& InRequesterCallback)
{
	UFigmaImporter* request = Requests.Emplace_GetRef(NewObject<UFigmaImporter>());
	WidgetOverrides = &InProperties->WidgetOverrides;
	FrameToButtonOverride = &InProperties->FrameToButton;
	request->Init(InProperties, InRequesterCallback);;
	request->Run();
	return request;
}

void UFigmaImportSubsystem::RemoveRequest(UFigmaImporter* FigmaImporter)
{
	WidgetOverrides = nullptr;
	FrameToButtonOverride = nullptr;
	Requests.Remove(FigmaImporter);
}

bool UFigmaImportSubsystem::ShouldGenerateButton(const FString& NodeName) const
{
	if (!FrameToButtonOverride)
		return false;

	if (NodeName.IsEmpty())
		return false;

	for (const FWidgetOverride& Override : FrameToButtonOverride->Rules)
	{
		if (Override.Match(NodeName))
			return true;
	}

	return false;
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
