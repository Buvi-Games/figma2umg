// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VaRestSubsystem.h"

#include "RequestWrapper.generated.h"

DECLARE_DELEGATE_OneParam(FOnVaRestCB, UVaRestRequestJSON*);

UCLASS()
class URequestWrapper : public UObject
{
	GENERATED_BODY()
public:
	URequestWrapper(const FObjectInitializer& ObjectInitializer);

	virtual ~URequestWrapper()
	{
	}

	void SetCallback(const FOnVaRestCB& callback);

	bool Request(FString AccessToken, FString FileKey, int Pages = -1);
	void Reset();

	FVaRestCallDelegate OnVaRestDelegate;
protected:
	UFUNCTION()
	void OnVaRestCB(UVaRestRequestJSON* Request);
	void OnRequestComplete(UVaRestRequestJSON* Request);
	void OnRequestFail(UVaRestRequestJSON* Request);

	FOnVaRestCB RequesterCallback;

	FVaRestCallResponse Response;
};