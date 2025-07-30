// MIT License
// Copyright (c) 2024 Buvi Games

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FigmaDocument.h"
#include "Properties/FigmaComponentRef.h"
#include "Properties/FigmaComponentSetRef.h"

#include "FigmaFile.generated.h"

class IAssetBuilder;
struct FImageRequests;
class UFigmaDocument;
class UFigmaImporter;

DECLARE_DELEGATE_OneParam(FProcessFinishedDelegate, bool);

UCLASS()
class FIGMA2UMG_API UFigmaFile : public UObject
{
public:
	GENERATED_BODY()

	void PostSerialize(const FString& InFileKey, const FString& InPackagePath, const TSharedRef<FJsonObject> JsonObj);

	FString GetFileName() const { return Name; }
	FString GetPackagePath() const { return PackagePath; }

	FString FindComponentName(const FString& ComponentId);
	FFigmaComponentRef* FindComponentRef(const FString& ComponentId);
	FFigmaComponentRef* FindComponentRefByKey(const FString& Key);
	TObjectPtr<UFigmaComponent> FindComponentByKey(const FString& Key);
	const TObjectPtr<UFigmaComponent> FindComponentByKey(const FString& Key) const;

	FString FindComponentSetName(const FString& ComponentSetId);
	FFigmaComponentSetRef* FindComponentSetRef(const FString& ComponentSetId);
	FFigmaComponentSetRef* FindComponentSetRefByKey(const FString& Key);
	TObjectPtr<UFigmaComponentSet> FindComponentSetByKey(const FString& Key);
	const TObjectPtr<UFigmaComponentSet> FindComponentSetByKey(const FString& Key) const;

	void PrepareForFlow();
	void FixComponentSetRef();
	void FixRemoteReferences(const TMap<FString, TObjectPtr<UFigmaFile>>& LibraryFiles);
	void CreateAssetBuilders(const FProcessFinishedDelegate& ProcessDelegate, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders);

	template<class NodeType>
	TObjectPtr<NodeType> FindByID(FString ID)
	{
		if (Document)
		{
			return Cast<NodeType>(Document->FindTypeByID(NodeType::StaticClass(), ID));
		}

		return nullptr;
	}
	template<class NodeType>
	const TObjectPtr<NodeType> FindByID(FString ID) const
	{
		if (Document)
		{
			return Cast<NodeType>(Document->FindTypeByID(NodeType::StaticClass(), ID));
		}

		return nullptr;
	}

	void SetImporter(UFigmaImporter* InFigmaImporter);
	UFigmaImporter* GetImporter() const;

protected:
	void FixRemoteComponentReferences(const TMap<FString, TObjectPtr<UFigmaFile>>& LibraryFiles);
	void FixRemoteComponentSetReferences(const TMap<FString, TObjectPtr<UFigmaFile>>& LibraryFiles);

	void AddRemoteComponent(FFigmaComponentRef& ComponentRef, const TPair<FString, TObjectPtr<UFigmaFile>>& LibraryFile, TObjectPtr<UFigmaComponent> Component, TMap<FString, FFigmaComponentRef>& PendingComponents);
	void AddRemoteComponentSet(FFigmaComponentSetRef& ComponentSetRef, const TPair<FString, TObjectPtr<UFigmaFile>>& LibraryFile, TObjectPtr<UFigmaComponentSet> ComponentSet, TMap<FString, FFigmaComponentRef>& PendingComponents, TMap<FString, FFigmaComponentSetRef>& PendingComponentSets);
	void ExecuteDelegate(const bool Succeeded);

	bool CreateAssetBuilder(const FString& InFileKey, UFigmaNode& Node, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders);

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

	FString FileKey;
	FString PackagePath;

	UFigmaImporter* FigmaImporter = nullptr;

	FProcessFinishedDelegate CurrentProcessDelegate;
};
