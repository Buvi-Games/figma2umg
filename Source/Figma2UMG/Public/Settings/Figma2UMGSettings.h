#pragma once

#include "CoreMinimal.h"
#include "ClassOverrides.h"

#include "Figma2UMGSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class FIGMA2UMG_API UFigma2UMGSettings : public UObject
{
	GENERATED_BODY()
public:
	UFigma2UMGSettings(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Config, EditAnywhere, Category = "Figma2UMG")
	FString AccessToken;

	UPROPERTY(Config, EditAnywhere, Category = "Figma2UMG")
	FString FileKey;

	UPROPERTY(Config, EditAnywhere, Category = "Figma2UMG")
	TArray<FString> LibraryFileKeys;

	UPROPERTY(Config, EditAnywhere, meta = (Category = "Figma2UMG|Options", ToolTip = "Try to download missing fonts from Google."))
	bool DownloadFontsFromGoogle = true;

	UPROPERTY(Config, EditAnywhere, meta = (Category = "Figma2UMG|Options", AdvancedDisplay, EditCondition = "DownloadFontsFromGoogle", ToolTip = "Your application needs to identify itself every time it sends a request to the Google Fonts Developer API. See https://developers.google.com/fonts/docs/developer_api."))
	FString GFontsAPIKey;

	UPROPERTY(Config, EditAnywhere, Category = "Figma2UMG")
	bool UsePrototypeFlow = true;

	UPROPERTY(Config, EditAnywhere, meta = (Category = "Figma2UMG"))
	bool SaveAllAtEnd = true;

	UPROPERTY(Config, EditAnywhere, Category = "Figma2UMG")
	FClassOverrides WidgetOverrides;

	UPROPERTY(Config, EditAnywhere, Category = "Figma2UMG")
	FFrameToButtonOverride FrameToButton;
};
