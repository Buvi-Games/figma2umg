// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class SImporterWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SImporterWidget)
		{
		}

	SLATE_END_ARGS()

	SImporterWidget();
	~SImporterWidget() = default;

	void Construct(const FArguments& InArgs);

private:
	void Add(TSharedRef<SGridPanel> Content, const FText& Name, const FText& Value, const FOnTextChanged& OnValueChanged);
	void Add(TSharedRef<SGridPanel> Content, const FText& Name, const int& Value, TSharedPtr<STextBlock>& ValueTextPtr, const FOnFloatValueChanged& OnValueChanged);

	FReply DoImport();

	void OnAccessTokenChanged(const FText& InValue) { AccessTokenValue = InValue; }
	FText AccessTokenName;
	FText AccessTokenValue;
	void OnRequestResult(UVaRestRequestJSON* VaRestJson);

	void OnContentRootFolderChanged(const FText& InValue) { ContentRootFolderValue = InValue; }
	FText ContentRootFolderName;
	FText ContentRootFolderValue;

	void OnFileURLChanged(const FText& InValue) { FileURLValue = InValue; }
	FText FileURLName;
	FText FileURLValue;

	void OnPagesChanged(float InValue);
	FText PagesName;
	int PagesValue = -1;
	TSharedPtr<STextBlock> PagesValueTextPtr;

	FText ImportButtonName;
	FText ImportButtonTooltip;

	int RowCount = 0;
};
