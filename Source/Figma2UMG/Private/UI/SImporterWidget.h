// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VaRestSubsystem.h"

class URequestWrapper;

class SImporterWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SImporterWidget)
		{
		}

	SLATE_END_ARGS()

	SImporterWidget();
	~SImporterWidget();

	void Construct(const FArguments& InArgs);

private:
	void Add(TSharedRef<SGridPanel> Content, const FText& Name, const FString& Value, const FOnTextChanged& OnValueChanged);
	void Add(TSharedRef<SGridPanel> Content, const FText& Name, const int& Value, TSharedPtr<STextBlock>& ValueTextPtr, const FOnFloatValueChanged& OnValueChanged);

	FReply DoImport();

	void OnRequestResult(UVaRestRequestJSON* VaRestJson);

	void OnAccessTokenChanged(const FText& InValue) { AccessTokenValue = InValue.ToString(); }
	FText AccessTokenName;
	FString AccessTokenValue;

	void OnFileURLChanged(const FText& InValue) { FileKeyValue = InValue.ToString(); }
	FText FileKeyName;
	FString FileKeyValue;

	void OnPagesChanged(float InValue);
	FText PagesName;
	int PagesValue = -1;
	TSharedPtr<STextBlock> PagesValueTextPtr;

	void OnContentRootFolderChanged(const FText& InValue) { ContentRootFolderValue = InValue.ToString(); }
	FText ContentRootFolderName;
	FString ContentRootFolderValue;

	TSharedPtr<SButton> ImportButton;
	FText ImportButtonName;
	FText ImportButtonTooltip;

	int RowCount = 0;


	bool HasError = false;
	FString ErrorMsg;


	URequestWrapper* OnVaRestWrapper;
};
