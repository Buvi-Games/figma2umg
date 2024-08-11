// Copyright 2024 Buvi Games. All Rights Reserved.


#include "SingleChildBuilder.h"

#include "Figma2UMGModule.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ContentWidget.h"
#include "Components/Widget.h"
#include "Parser/Nodes/FigmaNode.h"

void USingleChildBuilder::SetChild(const TScriptInterface<IWidgetBuilder>& WidgetBuilder)
{
	if (ChildWidgetBuilder)
	{
		ChildWidgetBuilder->SetParent(nullptr);
	}
	ChildWidgetBuilder = WidgetBuilder;
	if (ChildWidgetBuilder)
	{
		ChildWidgetBuilder->SetParent(this);
	}
}

void USingleChildBuilder::PostInsertWidgets(TObjectPtr<UWidgetBlueprint> WidgetBlueprint)
{
	if (ChildWidgetBuilder)
	{
		ChildWidgetBuilder->PostInsertWidgets(WidgetBlueprint);
	}
}

bool USingleChildBuilder::TryInsertOrReplace(const TObjectPtr<UWidget>& PrePatchWidget, const TObjectPtr<UWidget>& PostPatchWidget)
{
	if (!PostPatchWidget)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[USingleChildBuilder::TryInsertOrReplace] Trying to insert a <null> Widget at Node %s."), *Node->GetNodeName());
		return false;
	}
	const TObjectPtr<UContentWidget> ContentWidget = GetContentWidget();
	if (!ContentWidget)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[USingleChildBuilder::TryInsertOrReplace] Node %s doesn't have ContentWidget."), *Node->GetNodeName());
		return false;
	}

	if (ContentWidget->GetContent() == PostPatchWidget)
	{
		UE_LOG_Figma2UMG(Display, TEXT("[USingleChildBuilder::TryInsertOrReplace] Node %s ContentWidget %s alreay have Widget %s."), *Node->GetNodeName(), *ContentWidget->GetName(), *PostPatchWidget->GetName());
		return true;
	}

	UE_LOG_Figma2UMG(Display, TEXT("[USingleChildBuilder::TryInsertOrReplace] Node %s ContentWidget %s inserted Widget %s."), *Node->GetNodeName(), *ContentWidget->GetName(), *PostPatchWidget->GetName());
	ContentWidget->SetContent(PostPatchWidget);
	return true;
}

void USingleChildBuilder::PatchWidgetBinds(const TObjectPtr<UWidgetBlueprint>& WidgetBlueprint)
{
	IWidgetBuilder::PatchWidgetBinds(WidgetBlueprint);

	if (ChildWidgetBuilder)
	{
		ChildWidgetBuilder->PatchWidgetBinds(WidgetBlueprint);
	}
}

void USingleChildBuilder::PatchWidgetProperties()
{
	if (ChildWidgetBuilder)
	{
		ChildWidgetBuilder->PatchWidgetProperties();
	}
}

TObjectPtr<UWidget> USingleChildBuilder::GetWidget() const
{
	return GetContentWidget();
}

TObjectPtr<UWidget> USingleChildBuilder::FindWidgetRecursive(const FString& WidgetName) const
{
	if (TObjectPtr<UWidget> FoundWidget = IWidgetBuilder::FindWidgetRecursive(WidgetName))
		return FoundWidget;

	if (ChildWidgetBuilder)
	{
		return ChildWidgetBuilder->FindWidgetRecursive(WidgetName);
	}

	return nullptr;
}

void USingleChildBuilder::ResetWidget()
{
	if (ChildWidgetBuilder)
	{
		ChildWidgetBuilder->ResetWidget();
	}
}

void USingleChildBuilder::PatchAndInsertChild(TObjectPtr<UWidgetBlueprint> WidgetBlueprint, const TObjectPtr<UContentWidget>& ParentWidget)
{
	if (!ParentWidget)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[USingleChildBuilder::PatchAndInsertChildren] ParentWidget is null at Node %s."), *Node->GetNodeName());
		return;
	}

	if (ChildWidgetBuilder)
	{
		TObjectPtr<UWidget> ChildWidget = ParentWidget->GetContent();
		ChildWidgetBuilder->PatchAndInsertWidget(WidgetBlueprint, ChildWidget);
	}
}

void USingleChildBuilder::SetChildWidget(TObjectPtr<UContentWidget> ParentWidget)
{
	if (!ParentWidget)
	{
		UE_LOG_Figma2UMG(Warning, TEXT("[USingleChildBuilder::SetChildWidget] ParentWidget is null at Node %s."), *Node->GetNodeName());
		return;
	}

	if (ChildWidgetBuilder)
	{
		ChildWidgetBuilder->SetWidget(ParentWidget->GetContent());
	}
}
