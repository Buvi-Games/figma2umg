// MIT License
// Copyright (c) 2024 Buvi Games

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

	UPROPERTY(EditAnywhere, meta = (Category = "Figma2UMG", ToolTip = "Local folder where the UAssets will be created. eg '/Game/MyFolder'"))
	FString ContentRootFolder = "/Game/Figma";

	UPROPERTY(Config, EditAnywhere, meta = (Category = "Figma2UMG|Options", ToolTip = "Try to download missing fonts from Google."))
	bool DownloadFontsFromGoogle = true;

	UPROPERTY(Config, EditAnywhere, meta = (Category = "Figma2UMG|Options", AdvancedDisplay, EditCondition = "DownloadFontsFromGoogle", ToolTip = "Your application needs to identify itself every time it sends a request to the Google Fonts Developer API. See https://developers.google.com/fonts/docs/developer_api."))
	FString GFontsAPIKey;

	UPROPERTY(Config, EditAnywhere, Category = "Figma2UMG|Options")
	bool UsePrototypeFlow = true;

	UPROPERTY(EditAnywhere, meta = (Category = "Figma2UMG|Options", ToolTip = "Maximun amount of Images requested per REST request.", ClampMin = "1", ClampMax = "30", UIMin = "1", UIMax = "30"))
	int MaxURLImageRequest = 20;

	UPROPERTY(EditAnywhere, meta = (Category = "Figma2UMG|Options", ToolTip = "Scale for node images requested from Figma (between 0.01 and 4).", ClampMin = "0.1", ClampMax = "4.0", UIMin = "0.1", UIMax = "4.0"))
	float NodeImageScale = 4.0f;

	UPROPERTY(Config, EditAnywhere, meta = (Category = "Figma2UMG|Options"))
	bool SaveAllAtEnd = true;

	UPROPERTY(Config, EditAnywhere, Category = "Figma2UMG")
	FClassOverrides WidgetOverrides;

	UPROPERTY(Config, EditAnywhere, Category = "Figma2UMG")
	FFrameToButtonOverride FrameToButton;
};
