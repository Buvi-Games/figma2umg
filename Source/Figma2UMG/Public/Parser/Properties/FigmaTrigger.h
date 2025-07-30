// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"
#include "FigmaEnums.h"

#include "FigmaTrigger.generated.h"

UCLASS()
class FIGMA2UMG_API UFigmaTrigger : public UObject
{
public:
	GENERATED_BODY()

	static UFigmaTrigger* CreateTrigger(const TSharedPtr<FJsonObject>& ObjectJson);

	virtual bool MatchEvent(FString EventName) const;

	UPROPERTY()
	EFigmaTriggerType Type;
};

UCLASS()
class FIGMA2UMG_API UFigmaTriggerAfterTimeOut : public UFigmaTrigger
{
public:
	GENERATED_BODY()

	UPROPERTY()
	float TimeOut;
};

UCLASS()
class FIGMA2UMG_API UFigmaTriggerMouse : public UFigmaTrigger
{
public:
	GENERATED_BODY()

	UPROPERTY()
	float Delay;

	UPROPERTY()
	bool DeprecatedVersion;
};

UCLASS()
class FIGMA2UMG_API UFigmaTriggerKey : public UFigmaTrigger
{
public:
	GENERATED_BODY()

	UPROPERTY()
	EFigmaInputDevice Device;

	UPROPERTY()
	TArray<int> KeyCodes;
};

UCLASS()
class FIGMA2UMG_API UFigmaTriggerMedia : public UFigmaTrigger
{
public:
	GENERATED_BODY()

	UPROPERTY()
	float MediaHitTime;
};