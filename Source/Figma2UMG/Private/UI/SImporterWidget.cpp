// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SImporterWidget.h"

#include "Widgets/Input/SSlider.h"

#define LOCTEXT_NAMESPACE "Figma2UMG"

SImporterWidget::SImporterWidget()
{
	AccessTokenName = LOCTEXT("AccessTokenName", "Access Token");
	ContentRootFolderName = LOCTEXT("ContentRootFolderName", "Content Root Folder");
	FileURLName = LOCTEXT("FileURLName", "File URL");
	PagesName = LOCTEXT("PagesName", "Pages (-1 for all)");

	ImportButtonName = LOCTEXT("ImportButtonName", "Import");
	ImportButtonTooltip  = LOCTEXT("ImportButtonTooltip", "This will import the file from Figma according to the value above.");


	//TODO: Load some config file to get the defaulf values of AccessTokenValue, ContentRootFolderValue and FileURLValue
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
	Add(Content, ContentRootFolderName, ContentRootFolderValue, FOnTextChanged::CreateRaw(this, &SImporterWidget::OnContentRootFolderChanged));
	Add(Content, FileURLName, FileURLValue, FOnTextChanged::CreateRaw(this, &SImporterWidget::OnFileURLChanged));
	Add(Content, PagesName, PagesValue, PagesValueTextPtr, FOnFloatValueChanged::CreateRaw(this, &SImporterWidget::OnPagesChanged));
	Content->AddSlot(0, RowCount)
		.ColumnSpan(2)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			SNew(SButton)
				.Text(ImportButtonName)
				.ToolTipText(ImportButtonTooltip)
				.OnClicked(this, &SImporterWidget::DoImport)
		];
}

void SImporterWidget::Add(TSharedRef<SGridPanel> Content, const FText& Name, const FText& Value, const FOnTextChanged& OnValueChanged)
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
						.Text(Value)
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
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
