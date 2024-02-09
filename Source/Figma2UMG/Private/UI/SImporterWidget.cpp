// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SImporterWidget.h"

#include "FigmaImportSubsystem.h"
#include "TimerManager.h"
#include "REST/RequestParams.h"

#define LOCTEXT_NAMESPACE "Figma2UMG"

SImporterWidget::SImporterWidget()
{
	ImportButtonName = LOCTEXT("ImportButtonName", "Import");
	ImportButtonTooltip  = LOCTEXT("ImportButtonTooltip", "This will import the file from Figma according to the value above.");
}

SImporterWidget::~SImporterWidget()
{
	if (Properties != nullptr)
	{
		Properties->RemoveFromRoot();
		Properties = nullptr;
	}
}

void SImporterWidget::Construct(const FArguments& InArgs)
{
	if (Properties == nullptr)
	{
		Properties = NewObject<URequestParams>();
		Properties->AddToRoot();
	}

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

	AddPropertyView(Content);

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
	RowCount++;

	Content->AddSlot(0, RowCount)
		.ColumnSpan(2)
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Fill)
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(8.f)
				[
					SAssignNew(Message, STextBlock)
						.Visibility(EVisibility::Collapsed)
						.Justification(ETextJustify::Center)
				]
		];
}

void SImporterWidget::AddPropertyView(TSharedRef<SGridPanel> Content)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bShowPropertyMatrixButton = false;
	DetailsViewArgs.NotifyHook = this;

	DetailsViewArgs.ViewIdentifier = FName("Figma2UMG Importer");
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.bAllowFavoriteSystem = false;
	DetailsViewArgs.bShowOptions = false;
	DetailsViewArgs.bShowObjectLabel = false;
	DetailsViewArgs.bShowModifiedPropertiesOption = true;
	DetailsViewArgs.bShowKeyablePropertiesOption = false;
	DetailsViewArgs.bShowAnimatedPropertiesOption = false;
	DetailsViewArgs.bShowScrollBar = true;
	DetailsViewArgs.bForceHiddenPropertyVisibility = false;
	//DetailsViewArgs.ColumnWidth = ColumnWidth;
	DetailsViewArgs.bShowCustomFilterOption = false;

	DetailViewWidget = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailViewWidget->SetObject(Properties);
	if (DetailViewWidget.IsValid())
	{
		Content->AddSlot(0, 0)
			.ColumnSpan(2)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Fill)
			[
				DetailViewWidget.ToSharedRef()
			];

		RowCount++;
	}
}

FReply SImporterWidget::DoImport()
{
	UFigmaImportSubsystem* Importer = GEditor->GetEditorSubsystem<UFigmaImportSubsystem>();
	if (Importer)
	{
		SetMessage(TEXT("Connecting with Figma"));
		ImportButton->SetEnabled(false);
		Importer->Request(Properties, FOnFigmaImportUpdateStatusCB::CreateRaw(this, &SImporterWidget::OnRequestFinished));
	}

	return FReply::Handled();
}

void SImporterWidget::OnRequestFinished(eRequestStatus Status, FString InMessage)
{
	bool Error = Status == eRequestStatus::Failed;
	if (Status == eRequestStatus::Succeeded || Status == eRequestStatus::Failed)
	{
		ImportButton->SetEnabled(true);
		SetMessage(InMessage);

		FTimerHandle UnusedHandle;
		TSharedRef<FTimerManager> WorldTimerManager = GEditor->GetTimerManager();
		WorldTimerManager->SetTimer(UnusedHandle, FTimerDelegate::CreateRaw(this, &SImporterWidget::ResetMessage), 5.0f, false, 5.0f);
	}
	else
	{
		SetMessage(InMessage);
	}
}

void SImporterWidget::SetMessage(const FString& Text, bool IsError)
{
	Message->SetVisibility(EVisibility::Visible);
	FString CurrentText = Message->GetText().ToString();
	CurrentText = CurrentText + TEXT('\r') + Text;
	Message->SetText(FText::FromString(CurrentText));
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
	Message->SetText(FText::FromString(""));
}

void SImporterWidget::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
}

#undef LOCTEXT_NAMESPACE
