// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RequestParams.generated.h"


UCLASS()
class URequestParams : public  UObject
{
	GENERATED_BODY()
public:
	URequestParams(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, meta = (Category = "Figma", ToolTip = "A personal access token gives the holder access to an account through the API as if they were the user who generated the token. See https://www.figma.com/developers/api#authentication"))
	FString AccessToken;

	UPROPERTY(EditAnywhere, meta = (Category = "Figma", ToolTip = "File to export JSON from. This can be a file key or branch key."))
	FString FileKey;

	UPROPERTY(EditAnywhere, meta = (Category = "Figma", ToolTip = "List of nodes that you care about in the document. If specified, only a subset of the document will be returned corresponding to the nodes listed, their children, and everything between the root node and the listed nodes. Format is separated by ':' eg. XXX:YYY"))
	TArray<FString> Ids;

	UPROPERTY(EditAnywhere, meta = (Category = "Figma", ToolTip = "List of Library files to get Components from."))
	TArray<FString> LibraryFileKeys;

	UPROPERTY(EditAnywhere, meta = (Category = "Figma|Fonts", ToolTip = "Try to download missing fonts from Google."))
	bool DownloadFontsFromGoogle;

	UPROPERTY(EditAnywhere, meta = (Category = "Unreal", ToolTip = "Local folder where the UAssets will be created. eg '/Game/MyFolder'"))
	FString ContentRootFolder;
};