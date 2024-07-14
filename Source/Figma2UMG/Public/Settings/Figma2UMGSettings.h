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

	UPROPERTY(EditAnywhere, meta = (Category = "Figma2UMG|Options", ToolTip = "Try to download missing fonts from Google."))
	bool DownloadFontsFromGoogle = true;

	UPROPERTY(Config, EditAnywhere, Category = "Figma2UMG")
	bool UsePrototypeFlow = true;

	UPROPERTY(EditAnywhere, meta = (Category = "Figma2UMG"))
	bool SaveAllAtEnd = true;

	UPROPERTY(Config, EditAnywhere, Category = "Figma2UMG")
	FClassOverrides WidgetOverrides;

	UPROPERTY(Config, EditAnywhere, Category = "Figma2UMG")
	FFrameToButtonOverride FrameToButton;
};
