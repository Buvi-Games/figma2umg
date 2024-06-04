// Copyright 2024 Buvi Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "REST/FigmaImporter.h"

class IDetailsView;
class SGridPanel;

class SImporterWidget : public SCompoundWidget, public FNotifyHook
{
public:
	SLATE_BEGIN_ARGS(SImporterWidget)
		{
		}

	SLATE_END_ARGS()

	SImporterWidget();
	virtual ~SImporterWidget() override;

	void Construct(const FArguments& InArgs);

private:
	void AddPropertyView(TSharedRef<SGridPanel> Content);

	FReply DoImport();
	void OnRequestFinished(eRequestStatus Status, FString InMessage);

	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;

	TObjectPtr<URequestParams> Properties;

	TSharedPtr<IDetailsView> DetailViewWidget;
	TSharedPtr<SButton> ImportButton;
	FText ImportButtonName;
	FText ImportButtonTooltip;

	int RowCount = 0;
};
