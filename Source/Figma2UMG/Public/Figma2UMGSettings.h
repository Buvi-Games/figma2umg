#pragma once

#include "CoreMinimal.h"
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
};
