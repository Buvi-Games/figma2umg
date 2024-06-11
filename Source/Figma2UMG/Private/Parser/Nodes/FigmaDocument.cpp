// Copyright 2024 Buvi Games. All Rights Reserved.


#include "Parser/Nodes/FigmaDocument.h"

#include "Builder/Asset/WidgetBlueprintBuilder.h"
#include "Builder/Widget/PanelWidgetBuilder.h"
#include "Builder/Widget/WidgetSwitcherBuilder.h"
#include "Parser/FigmaFile.h"

void UFigmaDocument::SetFigmaFile(UFigmaFile* InFigmaFile)
{
	FigmaFile = InFigmaFile;
	SetCurrentPackagePath(FigmaFile->GetPackagePath());
}

TScriptInterface<IAssetBuilder> UFigmaDocument::CreateAssetBuilder(const FString& InFileKey)
{
	UWidgetBlueprintBuilder* AssetBuilder = NewObject<UWidgetBlueprintBuilder>();
	AssetBuilder->SetNode(InFileKey, this);
	return AssetBuilder;
}

FString UFigmaDocument::GetPackageName() const
{
	return PackagePath;
}

FString UFigmaDocument::GetUAssetName() const
{
	return FigmaFile ? FigmaFile->GetFileName() : FString();
}

TScriptInterface<IWidgetBuilder> UFigmaDocument::CreateWidgetBuilders(bool IsRoot /*= false*/, bool AllowFrameButton/*= true*/) const
{
	if (Children.Num() > 1)
	{
		UWidgetSwitcherBuilder* Builder = NewObject<UWidgetSwitcherBuilder>();
		Builder->SetNode(this);

		for (UFigmaNode* Child : Children)
		{
			if(!Child)
				continue;

			TScriptInterface<IWidgetBuilder> SubBuilder = Child->CreateWidgetBuilders();
			if (SubBuilder)
			{
				Builder->AddChild(SubBuilder);
			}

		}

		return Builder;
	}
	else if (Children.Num() == 1)
	{
		TScriptInterface<IWidgetBuilder> SubBuilder = Children[0] ? Children[0]->CreateWidgetBuilders() : nullptr;
		return SubBuilder;
	}

	return nullptr;
}
