// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaInstance.h"

#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "REST/FigmaFile.h"
#include "REST/Properties/FigmaComponentRef.h"
#include "Templates/WidgetTemplateBlueprintClass.h"

void UFigmaInstance::ForEach(const IWidgetOwner::FOnEachFunction& Function)
{
	if (TObjectPtr<UWidget> Widget = GetTopWidget())
	{
		Function.ExecuteIfBound(*Widget);
	}
}

TObjectPtr<UWidget> UFigmaInstance::Patch(TObjectPtr<UWidget> WidgetToPatch)
{
	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	FFigmaComponentRef* ComponentRef = FigmaFile->FindComponentRef(ComponentId);
	UWidgetBlueprint* ComponentAsset = ComponentRef ? ComponentRef->GetAsset() : nullptr;
	if (ComponentAsset)
	{
		if (WidgetToPatch)
		{
			//TODO: Check if it's the correct Template
			return WidgetToPatch;
		}
		else if (ParentNode)
		{
			TObjectPtr<UWidgetTree> OwningObject = Cast<UWidgetTree>(ParentNode->GetAssetOuter());
			TSubclassOf<UUserWidget> UserWidgetClass = ComponentAsset->GetBlueprintClass();

			TSharedPtr<FWidgetTemplateBlueprintClass> Template = MakeShared<FWidgetTemplateBlueprintClass>(FAssetData(ComponentAsset), UserWidgetClass);
			UWidget* NewWidget = Template->Create(OwningObject);

			if (NewWidget)
			{
				//if (NewWidget->GetName() != GetUniqueName())
				//{
				//	NewWidget->Rename(*GetUniqueName());
				//}
				NewWidget->CreatedFromPalette();
			}

			InstanceAsset = NewWidget;
			return NewWidget;
		}
	}
	return WidgetToPatch;
}

TObjectPtr<UWidget> UFigmaInstance::GetTopWidget() const
{
	return Cast<UWidget>(InstanceAsset);
}
