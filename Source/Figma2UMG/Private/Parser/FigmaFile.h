// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FigmaDocument.h"
#include "Properties/FigmaComponentRef.h"
#include "Properties/FigmaComponentSetRef.h"

#include "FigmaFile.generated.h"

struct FImageRequests;
class UFigmaDocument;

DECLARE_DELEGATE_OneParam(FProcessFinishedDelegate, bool);

UCLASS()
class UFigmaFile : public UObject
{
public:
	GENERATED_BODY()

	void PostSerialize(const FString& InPackagePath, const TSharedRef<FJsonObject> JsonObj);

	void ConvertToAssets();

	FString GetFileName() const { return Name; }
	FString GetPackagePath() const { return PackagePath; }

	FString FindComponentName(const FString& ComponentId);
	FFigmaComponentRef* FindComponentRef(const FString& ComponentId);

	TObjectPtr<UFigmaComponent> FindComponentByKey(const FString& Key);

	void FixRemoteReferences(const TMap<FString, TObjectPtr<UFigmaFile>>& LibraryFiles);
	void LoadOrCreateAssets(const FProcessFinishedDelegate& ProcessDelegate);
	void BuildImageDependency(FString FileKey, FImageRequests& ImageRequests);
	void Patch(const FProcessFinishedDelegate& ProcessDelegate);
	void PostPatch(const FProcessFinishedDelegate& ProcessDelegate);

	template<class NodeType>
	TObjectPtr<NodeType> FindByID(FString ID)
	{
		if (Document)
		{
			return Cast<NodeType>(Document->FindTypeByID(NodeType::StaticClass(), ID));
		}

		return nullptr;
	}

protected:
	void AddRemoteComponent(FFigmaComponentRef& ComponentRef, const TPair<FString, TObjectPtr<UFigmaFile>> LibraryFile, TObjectPtr<UFigmaComponent> Component, TMap<FString, FFigmaComponentRef>& PendingComponents);
	void ExecuteDelegate(const bool Succeeded);

	UPROPERTY()
	int SchemaVersion = 0;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString LastModified;

	UPROPERTY()
	FString ThumbnailUrl;

	UPROPERTY()
	int Version = 0;

	UPROPERTY()
	FString Role;

	UPROPERTY()
	FString EditorType;

	UPROPERTY()
	FString LinkAccess;

	UPROPERTY()
	UFigmaDocument* Document;

	UPROPERTY()
	TMap<FString, FFigmaComponentRef> Components;

	UPROPERTY()
	TMap<FString, FFigmaComponentSetRef> ComponentSets;

	UPROPERTY()
	TMap<FString, FFigmaStyleRef> Styles;//Not sure if this is correct, probably not

	FString PackagePath;

	FProcessFinishedDelegate CurrentProcessDelegate;
};
