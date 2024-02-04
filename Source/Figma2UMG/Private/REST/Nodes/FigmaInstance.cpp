// Fill out your copyright notice in the Description page of Project Settings.


#include "REST/Nodes/FigmaInstance.h"

#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "REST/FigmaFile.h"
#include "REST/Properties/FigmaComponentRef.h"
#include "Templates/WidgetTemplateBlueprintClass.h"

TObjectPtr<UWidget> UFigmaInstance::PatchWidgetImp(TObjectPtr<UWidget> WidgetToPatch)
{
	TObjectPtr<UFigmaFile> FigmaFile = GetFigmaFile();
	FFigmaComponentRef* ComponentRef = FigmaFile->FindComponentRef(ComponentId);
	UWidgetBlueprint* ComponentAsset = ComponentRef ? ComponentRef->GetAsset() : nullptr;
	if (ComponentAsset)
	{
		if (ParentNode)
		{
			TObjectPtr<UWidgetTree> OwningObject = Cast<UWidgetTree>(ParentNode->GetAssetOuter());
			TSubclassOf<UUserWidget> UserWidgetClass = ComponentAsset->GetBlueprintClass();

			TSharedPtr<FWidgetTemplateBlueprintClass> Template = MakeShared<FWidgetTemplateBlueprintClass>(FAssetData(ComponentAsset), UserWidgetClass);
			UWidget* NewWidget = Template->Create(OwningObject);

			if (NewWidget)
			{
				NewWidget->Rename(*GetName());
				NewWidget->CreatedFromPalette();
			}

			return NewWidget;
		}
	}
	return Super::PatchWidgetImp(WidgetToPatch);
}

void UFigmaInstance::PostInsert(UWidget* Widget) const
{
	UCanvasPanelSlot* CanvasSlot = Widget->Slot ? Cast<UCanvasPanelSlot>(Widget->Slot) : nullptr;
	if (CanvasSlot)
	{
		CanvasSlot->SetPosition(GetPosition());
		CanvasSlot->SetSize(GetSize());
	}

	// Todo: Any override attribute from Template
}