// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SImporterWidget.h"

#include "Figma2UMGModule.h"
#include "Figma2UMGSettings.h"
#include "JsonObjectConverter.h"
#include "REST/FigmaFile.h"
#include "REST/RequestWrapper.h"
#include "Widgets/Input/SSlider.h"

#define LOCTEXT_NAMESPACE "Figma2UMG"

SImporterWidget::SImporterWidget()
{
	//OnVaRestDelegate.BindUFunction(this, FName("OnVaRestCB"));

	AccessTokenName = LOCTEXT("AccessTokenName", "Access Token");
	ContentRootFolderName = LOCTEXT("ContentRootFolderName", "Content Root Folder");
	FileKeyName = LOCTEXT("FileKeyName", "File Key");
	PagesName = LOCTEXT("PagesName", "Pages (-1 for all)");

	ImportButtonName = LOCTEXT("ImportButtonName", "Import");
	ImportButtonTooltip  = LOCTEXT("ImportButtonTooltip", "This will import the file from Figma according to the value above.");


	FFigma2UMGModule& Figma2UMGModule = FModuleManager::LoadModuleChecked<FFigma2UMGModule>("Figma2UMG");
	UFigma2UMGSettings* Settings = Figma2UMGModule.GetSettings();
	if (Settings)
	{
		AccessTokenValue = Settings->AccessToken;
		FileKeyValue = Settings->FileKey;// "";
	}

	ContentRootFolderValue = "/Game/";
}

SImporterWidget::~SImporterWidget()
{
	if (OnVaRestWrapper)
	{
		OnVaRestWrapper->Reset();
	}
}

void SImporterWidget::Construct(const FArguments& InArgs)
{
	TSharedRef<SGridPanel> Content = SNew(SGridPanel).FillColumn(1, 1.0f);
	TSharedRef<SBorder> MainContent = SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		.Padding(20.f, 5.f, 10.f, 5.f)
		[
			Content
		];

	ChildSlot[MainContent];

	Add(Content, AccessTokenName, AccessTokenValue, FOnTextChanged::CreateRaw(this, &SImporterWidget::OnAccessTokenChanged));
	Add(Content, FileKeyName, FileKeyValue, FOnTextChanged::CreateRaw(this, &SImporterWidget::OnFileURLChanged));
	Add(Content, PagesName, PagesValue, PagesValueTextPtr, FOnFloatValueChanged::CreateRaw(this, &SImporterWidget::OnPagesChanged));
	Add(Content, ContentRootFolderName, ContentRootFolderValue, FOnTextChanged::CreateRaw(this, &SImporterWidget::OnContentRootFolderChanged));

	Content->AddSlot(0, RowCount)
		.ColumnSpan(2)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(8.f)
				[
					SAssignNew(Message, STextBlock)
						.Visibility(EVisibility::Collapsed)
				]
		];
	RowCount++;

	Content->AddSlot(0, RowCount)
		.ColumnSpan(2)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			SAssignNew(ImportButton, SButton)
				.Text(ImportButtonName)
				.ToolTipText(ImportButtonTooltip)
				.OnClicked(this, &SImporterWidget::DoImport)
		];
}

void SImporterWidget::Add(TSharedRef<SGridPanel> Content, const FText& Name, const FString& Value, const FOnTextChanged& OnValueChanged)
{
	Content->AddSlot(0, RowCount)
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(8.f)
				[
					SNew(STextBlock)
						.Text(Name)
				]
		];

	Content->AddSlot(1, RowCount)
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(8.f, 3.0f)
				[
					SNew(SEditableTextBox)
						.Text(FText::FromString(Value))
						.Padding(FVector4f(10.0, 0.0, 0.0, 0.0))
						.OnTextChanged(OnValueChanged)
				]
		];

	RowCount++;
}

void SImporterWidget::Add(TSharedRef<SGridPanel> Content, const FText& Name, const int& Value, TSharedPtr<STextBlock>& ValueTextPtr, const FOnFloatValueChanged& OnValueChanged)
{
	Content->AddSlot(0, RowCount)
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(8.f)
				[
					SNew(STextBlock)
						.Text(Name)
				]
		];

	Content->AddSlot(1, RowCount)
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(8.f)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.AutoWidth()
							.MaxWidth(50.0f)
							.Padding(20.f, 0.0f)
							[
								SAssignNew(ValueTextPtr, STextBlock)
									.Text(FText::AsNumber(Value))
							] 
						+ SHorizontalBox::Slot()
							.AutoWidth()
							.FillWidth(1.0f)
							[
								SNew(SSlider)
									.Value(Value)
									.MinValue(-1)
									.MaxValue(100)
									.StepSize(1.0f)
									.MouseUsesStep(true)
									.OnValueChanged(OnValueChanged)
							]
				]
		];

	RowCount++;
}

void SImporterWidget::OnPagesChanged(float InValue)
{
	PagesValue = InValue;
	if (PagesValueTextPtr.IsValid())
	{
		PagesValueTextPtr->SetText(FText::AsNumber(InValue));
	}
}


FReply SImporterWidget::DoImport()
{
	if (!OnVaRestWrapper)
	{
		OnVaRestWrapper = NewObject<URequestWrapper>();
		OnVaRestWrapper->SetCallback(FOnVaRestCB::CreateRaw(this, &SImporterWidget::OnRequestResult));
	}

	SetMessage(TEXT("Connecting with Figma"));
	if (OnVaRestWrapper->Request(AccessTokenValue, FileKeyValue, PagesValue))
	{
		ImportButton->SetEnabled(false);
	}
	else
	{
		//TODO: Handle error
		SetMessage(TEXT("UNKNOWN ERROR"), true);
	}

	return FReply::Handled();
}

void SImporterWidget::OnRequestResult(UVaRestRequestJSON* Request)
{
	OnVaRestWrapper = nullptr;

	if (Request)
	{
		const EVaRestRequestStatus status = Request->GetStatus();
		switch (status)
		{
		case EVaRestRequestStatus::NotStarted:
			SetMessage(TEXT("EVaRestRequestStatus::NotStarted"));
			break;
		case EVaRestRequestStatus::Processing:
			SetMessage(TEXT("EVaRestRequestStatus::Processing"));
			break;
		case EVaRestRequestStatus::Failed:
			SetMessage(TEXT("EVaRestRequestStatus::Failed"), true);
			break;
		case EVaRestRequestStatus::Failed_ConnectionError:
			SetMessage(TEXT("EVaRestRequestStatus::Failed_ConnectionError"), true);
			break;
		case EVaRestRequestStatus::Succeeded:
			SetMessage(TEXT("EVaRestRequestStatus::Succeeded - Parsing"));
			UVaRestJsonObject* responseJson = Request->GetResponseObject();
			if (!responseJson)
			{
				SetMessage(TEXT("VaRestJson has no response object"), true);

				ImportButton->SetEnabled(true);

				return;
			}

			const TSharedRef<FJsonObject> JsonObj = responseJson->GetRootObject();
			UFigmaFile* File = NewObject<UFigmaFile>();
			File->AddToRoot();

			const int64 CheckFlags = 0;
			const int64 SkipFlags = 0;
			const bool StrictMode = false;
			FText OutFailReason;
			if (FJsonObjectConverter::JsonObjectToUStruct(JsonObj, File->StaticClass(), File, CheckFlags, SkipFlags, StrictMode, &OutFailReason))
			{
				File->PostSerialize(JsonObj);
				File->CreateOrUpdateAsset(ContentRootFolderValue);
				ResetMessage();
			}
			else
			{
				SetMessage(OutFailReason.ToString(), true);
			}
			File->RemoveFromRoot();
		}
	}

	ImportButton->SetEnabled(true);
}

void SImporterWidget::SetMessage(const FString& Text, bool IsError)
{
	Message->SetVisibility(EVisibility::Visible);
	Message->SetText(FText::FromString(Text));
	if(IsError)
	{
		Message->SetColorAndOpacity(FLinearColor::Red);
	}
	else
	{
		Message->SetColorAndOpacity(FLinearColor::White);
	}
}

void SImporterWidget::ResetMessage()
{
	Message->SetVisibility(EVisibility::Collapsed);
	Message->SetColorAndOpacity(FLinearColor::White);
}

#undef LOCTEXT_NAMESPACE
