// Fvirtual ill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FigmaNode.generated.h"

class IAssetBuilder;
class UWidgetBlueprint;
class UFigmaFile;
class UWidgetTree;
class UPanelWidget;
class UWidget;

UENUM()
enum class ENodeTypes
{
	DOCUMENT,
	CANVAS,
	FRAME,
	GROUP,
	SECTION,
	VECTOR,
	BOOLEAN_OPERATION,
	STAR,
	LINE,
	ELLIPSE,
	REGULAR_POLYGON,
	RECTANGLE,
	TABLE,
	TABLE_CELL,
	TEXT,
	SLICE,
	COMPONENT,
	COMPONENT_SET,
	INSTANCE,
	STICKY,
	SHAPE_WITH_TEXT,
	CONNECTOR,
	WASHI_TAPE,
};

UCLASS()
class FIGMA2UMG_API UFigmaNode : public UObject
{
public:
	GENERATED_BODY()

	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj);

	//virtual void PostInsert(UWidget* Widget) const;

	virtual void PrepareForFlow();
	virtual void PrePatchWidget();
	virtual TObjectPtr<UWidget> PatchPreInsertWidget(TObjectPtr<UWidget> WidgetToPatch);
	virtual void SetWidget(TObjectPtr<UWidget> WidgetToPatch);
	virtual void InsertSubWidgets();
	virtual void PatchPostInsertWidget();
	void PostPatchWidget();

	FString GetId() const { return Id; }
	FString GetIdForName() const;

	FString GetNodeName() const;
	FString GetUniqueName() const;
	ESlateVisibility GetVisibility() const;

	FVector2D GetPosition() const;

	virtual FVector2D GetAbsolutePosition() const PURE_VIRTUAL(UFigmaNode::GetAbsolutePosition(), return FVector2D::ZeroVector;)

	void SetCurrentPackagePath(const FString & InPackagePath);
	virtual FString GetCurrentPackagePath() const;

	virtual TObjectPtr<UFigmaFile> GetFigmaFile() const;

	virtual UObject* GetAssetOuter() const;
	TObjectPtr<UFigmaNode> GetParentNode() const { return ParentNode; }
	TObjectPtr<UFigmaNode> FindTypeByID(const UClass* Class, const FString& ID);
	TObjectPtr<UWidget> FindWidgetForNode(const TObjectPtr<UPanelWidget>& ParentWidget) const;

	void ProcessComponentPropertyReferences(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget) const;

	const TMap<FString, FString>& GetComponentPropertyReferences() const { return ComponentPropertyReferences; }

	//New Builder API
	virtual IAssetBuilder* CreateAssetBuilder(const FString& InFileKey) { return nullptr; }
	virtual FString GetPackageName() const { return FString(); }
protected:
	void SerializeArray(TArray<UFigmaNode*>& Array, const TSharedRef<FJsonObject> JsonObj, const FString& arrayName);

	template<class PropertyT>
	void PostSerializeProperty(const TSharedRef<FJsonObject> JsonObj, const FString& ArrayName, TArray<PropertyT>& PropertyArray) const
	{
		if (JsonObj->HasTypedField<EJson::Array>(ArrayName))
		{
			const TArray<TSharedPtr<FJsonValue>>& ArrayJson = JsonObj->GetArrayField(ArrayName);
			for (int i = 0; i < ArrayJson.Num() && PropertyArray.Num(); i++)
			{
				const TSharedPtr<FJsonValue>& Item = ArrayJson[i];
				if (Item.IsValid() && Item->Type == EJson::Object)
				{
					const TSharedPtr<FJsonObject>& ItemObject = Item->AsObject();
					PropertyArray[i].PostSerialize(ItemObject);
				}
			}
		}
	}

	UFigmaNode* CreateNode(const TSharedPtr<FJsonObject>& JsonObj);

	virtual void ProcessComponentPropertyReference(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget, const TPair<FString, FString>& PropertyReference) const;

	TObjectPtr<UFigmaNode> ParentNode = nullptr;

	FString PackagePath;

private:
	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	bool Visible = true;

	UPROPERTY()
	ENodeTypes Type;

	UPROPERTY()
	float Rotation = 0.0f;

	UPROPERTY()
	FString PluginData;

	UPROPERTY()
	FString SharedPluginData;

protected:
	UPROPERTY()
	TMap<FString, FString> ComponentPropertyReferences;

	//boundVariablesMap - beta
	//explicitVariableModesMap - beta

	UPROPERTY()
	FString ScrollBehaviour; //Not in doc
};
