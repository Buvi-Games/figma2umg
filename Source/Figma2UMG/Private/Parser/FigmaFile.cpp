// Fill out your copyright notice in the Description page of Project Settings.


#include "Parser/FigmaFile.h"

#include "Nodes/FigmaDocument.h"
#include "Properties/FigmaComponentRef.h"



void UFigmaFile::PostSerialize(const FString& InPackagePath, const TSharedRef<FJsonObject> fileJsonObject)
{
	PackagePath = InPackagePath;
	if(Document)
	{
		Document->SetFigmaFile(this);
		Document->PostSerialize(nullptr, fileJsonObject->GetObjectField("Document").ToSharedRef());
	}
}

FString UFigmaFile::FindComponentName(const FString& ComponentId)
{
	if (Components.Contains(ComponentId))
	{
		const FFigmaComponentRef& Component = Components[ComponentId];
		return Component.Name;
	}

	return FString();
}

FFigmaComponentRef* UFigmaFile::FindComponentRef(const FString& ComponentId)
{
	if (Components.Contains(ComponentId))
	{
		FFigmaComponentRef& Component = Components[ComponentId];
		return &Component;
	}

	return nullptr;
}

void UFigmaFile::LoadOrCreateAssets(const FProcessFinishedDelegate& ProcessDelegate)
{
	CurrentProcessDelegate = ProcessDelegate;
	if (Document)
	{
		AsyncTask(ENamedThreads::GameThread, [this]()
			{
				TArray<IFigmaFileHandle*> AllFiles;
				AllFiles.Add(Document);
				Document->GetAllChildrenByType(AllFiles);
				UFigmaNode* CurrentNode = Document;

				FGCScopeGuard GCScopeGuard;
				for (IFigmaFileHandle* FileNode : AllFiles)
				{
					FileNode->LoadOrCreateAssets();
				}

				ExecuteDelegate(true);
			});
	}
	else
	{
		ExecuteDelegate(false);
	}
}

void UFigmaFile::BuildImageDependency(TArray<FString>& ImageIds)
{
	if (Document)
	{
		//TArray<IFigmaFileHandle*> AllFiles;
		//AllFiles.Add(Document);
		//Document->GetAllChildrenByType(AllFiles);
		//UFigmaNode* CurrentNode = Document;
		//
		//FGCScopeGuard GCScopeGuard;
		//for (IFigmaFileHandle* FileNode : AllFiles)
		//{
		//	FileNode->LoadOrCreateAssets();
		//}
	}
}

void UFigmaFile::Patch(const FProcessFinishedDelegate& ProcessDelegate)
{
	CurrentProcessDelegate = ProcessDelegate;
	if (Document)
	{
		AsyncTask(ENamedThreads::GameThread, [this]()
			{
				Document->PatchPreInsertWidget(nullptr);
				Document->PatchPostInsertWidget();

				ExecuteDelegate(true);
			});
	}
	else
	{
		ExecuteDelegate(false);
	}
}

void UFigmaFile::PostPatch(const FProcessFinishedDelegate& ProcessDelegate)
{
	CurrentProcessDelegate = ProcessDelegate;
	if (Document)
	{
		AsyncTask(ENamedThreads::GameThread, [this]()
			{
				Document->PostPatchWidget();

				ExecuteDelegate(true);
			});
	}
	else
	{
		ExecuteDelegate(false);
	}
}

void UFigmaFile::ExecuteDelegate(const bool Succeeded)
{
	if (CurrentProcessDelegate.ExecuteIfBound(Succeeded))
	{
//		CurrentProcessDelegate.Unbind();
	}
}
