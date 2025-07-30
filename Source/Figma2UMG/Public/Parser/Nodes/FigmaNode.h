// Fvirtual ill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Parser/Properties/FigmaEnums.h"
#include "Dom/JsonObject.h"
#include "FigmaNode.generated.h"

struct FFigmaPaint;
struct FFigmaInteraction;
class IWidgetBuilder;
class IAssetBuilder;
class UWidgetBlueprint;
class UFigmaFile;
class UWidgetTree;
class UPanelWidget;
class UWidget;


UCLASS()
class FIGMA2UMG_API UFigmaNode : public UObject
{
public:
	GENERATED_BODY()

	virtual void PostSerialize(const TObjectPtr<UFigmaNode> InParent, const TSharedRef<FJsonObject> JsonObj);

	//virtual void PostInsert(UWidget* Widget) const;

	virtual void PrepareForFlow();

	void InitializeFrom(const UFigmaNode* Other, const FString& NewId);

	FString GetId() const { return Id; }
	FString GetIdForName() const;

	FString GetNodeName() const;
	FString GetUniqueName(bool RemoveInstanceId = false) const;
	virtual FString GetUAssetName() const;
	ESlateVisibility GetVisibility() const;

	FVector2D GetPosition() const;
	float GetRotation() const;
	float GetAbsoluteRotation() const;

	virtual FVector2D GetAbsolutePosition(const bool IsTopWidgetForNode) const PURE_VIRTUAL(UFigmaNode::GetAbsolutePosition(), return FVector2D::ZeroVector;)
	virtual FVector2D GetAbsoluteSize(const bool IsTopWidgetForNode) const PURE_VIRTUAL(UFigmaNode::GetAbsoluteSize(), return FVector2D::ZeroVector;)
	virtual FVector2D GetAbsoluteCenter() const PURE_VIRTUAL(UFigmaNode::GetAbsoluteCenter(), return FVector2D::ZeroVector;)

	void SetCurrentPackagePath(const FString & InPackagePath);
	virtual FString GetCurrentPackagePath() const;

	virtual TObjectPtr<UFigmaFile> GetFigmaFile() const;

	TObjectPtr<UFigmaNode> GetParentNode() const { return ParentNode; }
	TObjectPtr<UFigmaNode> FindTypeByID(const UClass* Class, const FString& ID);
	TObjectPtr<UWidget> FindWidgetForNode(const TObjectPtr<UPanelWidget>& ParentWidget) const;

	void ProcessComponentPropertyReferences(TObjectPtr<UWidgetBlueprint> WidgetBP, TObjectPtr<UWidget> Widget) const;

	const TMap<FString, FString>& GetComponentPropertyReferences() const { return ComponentPropertyReferences; }

	virtual bool CreateAssetBuilder(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders) { return false; }
	virtual FString GetPackageNameForBuilder(const TScriptInterface<IAssetBuilder>& InAssetBuilder) const { return FString(); }
	void CreatePaintAssetBuilderIfNeeded(const FString& InFileKey, TArray<TScriptInterface<IAssetBuilder>>& AssetBuilders, TArray<FFigmaPaint>& InFills, TArray<FFigmaPaint>& InStrokes) const;

	virtual TScriptInterface<IWidgetBuilder> CreateWidgetBuilders(bool IsRoot = false, bool AllowFrameButton = true) const { return nullptr; }
	
	ENodeTypes GetType() const {return Type;}
protected:
	void SerializeArray(TArray<UFigmaNode*>& Array, const TSharedRef<FJsonObject> JsonObj, const FString& arrayName);

	const FFigmaInteraction& GetInteractionFromTrigger(const TArray<FFigmaInteraction>& InInteractions, const EFigmaTriggerType TriggerType) const;
	const FFigmaInteraction& GetInteractionFromAction(const TArray<FFigmaInteraction>& InInteractions, const EFigmaActionType ActionType, const EFigmaActionNodeNavigation Navigation) const;

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

	//boundVariablesMap - beta
	//explicitVariableModesMap - beta
	
protected:
	UPROPERTY()
	TMap<FString, FString> ComponentPropertyReferences;

	UPROPERTY()
	FString ScrollBehaviour; //Not in doc
};
