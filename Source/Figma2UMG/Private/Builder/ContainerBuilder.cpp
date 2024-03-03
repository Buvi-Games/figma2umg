// Fill out your copyright notice in the Description page of Project Settings.


#include "Builder/ContainerBuilder.h"

#include "Figma2UMGModule.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"

void FContainerBuilder::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	if (!Function.IsBound())
		return;

	Super::ForEach(Function);

	if (Conainter)
	{
		Function.ExecuteIfBound(*Conainter);
	}
}

void FContainerBuilder::SetLayout(EFigmaLayoutMode InLayoutMode, EFigmaLayoutWrap InLayoutWrap)
{
	LayoutMode = InLayoutMode;
	LayoutWrap = InLayoutWrap;
}

TObjectPtr<UWidget> FContainerBuilder::Patch(TObjectPtr<UWidget> WidgetToPatch, UObject* AssetOuter, const FString& WidgetName)
{
	Super::Patch(WidgetToPatch, AssetOuter, WidgetName);
	Conainter = nullptr;
	
	switch (LayoutMode)
	{
	case EFigmaLayoutMode::NONE:
	{
		Patch<UCanvasPanel>(WidgetToPatch, AssetOuter, WidgetName);
	}
	break;
	case EFigmaLayoutMode::HORIZONTAL:
	{
		if(LayoutWrap == EFigmaLayoutWrap::NO_WRAP)
		{
			Patch<UHorizontalBox>(WidgetToPatch, AssetOuter, WidgetName);
		}
		else
		{
			TObjectPtr<UWrapBox> WrapBox = Patch<UWrapBox>(WidgetToPatch, AssetOuter, WidgetName);
			WrapBox->SetOrientation(EOrientation::Orient_Horizontal);
		}
	}
	break;
	case EFigmaLayoutMode::VERTICAL:
	{
		if (LayoutWrap == EFigmaLayoutWrap::NO_WRAP)
		{
			Patch<UVerticalBox>(WidgetToPatch, AssetOuter, WidgetName);
		}
		else
		{
			TObjectPtr<UWrapBox> WrapBox = Patch<UWrapBox>(WidgetToPatch, AssetOuter, WidgetName);
			WrapBox->SetOrientation(EOrientation::Orient_Vertical);
		}
	}
	break;
	}

	return GetTopWidget();

}

void FContainerBuilder::SetupWidget(TObjectPtr<UWidget> Widget)
{
	FBorderBuilder::SetupWidget(Widget);
	if (const TObjectPtr<UBorder> BorderWrapper = GetBorder())
	{
		Conainter = Cast<UPanelWidget>(BorderWrapper->GetContent());
	}
	else
	{
		Conainter = Cast<UPanelWidget>(Widget);
	}

	if(!Conainter)
	{
		if (Widget)
		{
			UE_LOG_Figma2UMG(Error, TEXT("[FContainerBuilder::SetupWidget] Fail to setup UPanelWidget from UWidget %s of type %s."), *Widget->GetName(), *Widget->GetClass()->GetDisplayNameText().ToString());
		}
		else
		{
			UE_LOG_Figma2UMG(Warning, TEXT("[FContainerBuilder::SetupWidget] Fail to setup UPanelWidget from a null Widget."));
		}
	}
}

void FContainerBuilder::Reset()
{
	Super::Reset();
	Conainter = nullptr;
}

TObjectPtr<UWidget> FContainerBuilder::GetTopWidget() const
{
	TObjectPtr<UBorder> BorderWrapper = GetBorder();
	if (BorderWrapper)
	{
		return BorderWrapper;
	}
	else
	{
		return Conainter;
	}
}

TObjectPtr<UPanelWidget> FContainerBuilder::GetContainerWidget() const
{
	return Conainter;
}