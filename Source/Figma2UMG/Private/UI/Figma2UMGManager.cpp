// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Figma2UMGManager.h"

#include "ContentBrowserDataMenuContexts.h"
#include "Figma2UMGStyle.h"

#define LOCTEXT_NAMESPACE "Figma2UMG"
#define CONTENTBROWSER_MODULE_NAME TEXT("ContentBrowser")

FFigma2UMGManager::FFigma2UMGManager()
{
}


FFigma2UMGManager::~FFigma2UMGManager()
{
}

void FFigma2UMGManager::Initialize()
{
	FFigma2UMGStyle::Initialize();
	SetupMenuItem();
}

void FFigma2UMGManager::Shutdown()
{
}

void FFigma2UMGManager::SetupMenuItem()
{
	//FFigma2UMGStyle::SetIcon("MenuLogo", "Icon128");
	FFigma2UMGStyle::SetSVGIcon("MenuLogo", "Figma2UMG");

	UToolMenu* ContextMenu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AddNewContextMenu");
	FToolMenuSection& ContextMenuSection = ContextMenu->FindOrAddSection("ContentBrowserGetContent");

	TWeakPtr<FFigma2UMGManager> WeakPtr = AsShared();
	ContextMenuSection.AddDynamicEntry("ImpportFigma", FNewToolMenuSectionDelegate::CreateLambda([WeakPtr](FToolMenuSection& InSection)
		{
			UContentBrowserDataMenuContext_AddNewMenu* AddNewMenuContext = InSection.FindContext<UContentBrowserDataMenuContext_AddNewMenu>();
			if (AddNewMenuContext && AddNewMenuContext->bCanBeModified && AddNewMenuContext->bContainsValidPackagePath && WeakPtr.IsValid())
			{
				InSection.AddMenuEntry(
					"ImpportFigma",
					LOCTEXT("OpenImportFigmaFileText", "Import Figma File"),
					LOCTEXT("GetImportFigmaFileTooltip", "Import a Figma File and create assets."),
					FSlateIcon(FFigma2UMGStyle::GetStyleSetName(), "Figma2UMG.MenuLogo"),
					FUIAction(FExecuteAction::CreateSP(WeakPtr.Pin().ToSharedRef(), &FFigma2UMGManager::CreateWindow), FCanExecuteAction())
				);
			}
		}));
}

void FFigma2UMGManager::CreateWindow()
{
	//FGlobalTabmanager::Get()->TryInvokeTab(BridgeTabName);
	//
	//// Set desired window size (if the desired window size is less than main window size)
	//// Rationale: the main window is mostly maximized - so the size is equal to screen size
	//TArray<TSharedRef<SWindow>> Windows = FSlateApplication::Get().GetTopLevelWindows();
	//if (Windows.Num() > 0)
	//{
	//	FVector2D MainWindowSize = Windows[0]->GetSizeInScreen();
	//	float DesiredWidth = 1650;
	//	float DesiredHeight = 900;
	//
	//	if (DesiredWidth < MainWindowSize.X && DesiredHeight < MainWindowSize.Y && LocalBrowserDock->GetParentWindow().IsValid())
	//	{
	//		// If Bridge is docked as a tab, the parent window will be the main window
	//		if (LocalBrowserDock->GetParentWindow() == Windows[0])
	//		{
	//			return;
	//		}
	//
	//		LocalBrowserDock->GetParentWindow()->Resize(FVector2D(DesiredWidth, DesiredHeight));
	//		LocalBrowserDock->GetParentWindow()->MoveWindowTo(FVector2D((MainWindowSize.X - DesiredWidth) - 17, MainWindowSize.Y - DesiredHeight) / 2);
	//	}
	//}
}