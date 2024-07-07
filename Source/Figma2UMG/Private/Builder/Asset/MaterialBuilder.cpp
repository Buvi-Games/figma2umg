// Copyright 2024 Buvi Games. All Rights Reserved.


#include "MaterialBuilder.h"

#include "AssetToolsModule.h"
#include "Figma2UMGModule.h"
#include "MaterialDomain.h"
#include "MaterialEditorUtilities.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor/MaterialEditor/Public/MaterialEditingLibrary.h"
#include "Factories/MaterialFactoryNew.h"
#include "Materials/MaterialExpressionComponentMask.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionSubtract.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Parser/FigmaFile.h"
#include "Parser/Nodes/FigmaNode.h"

void UMaterialBuilder::LoadOrCreateAssets()
{
	UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>(UMaterialFactoryNew::StaticClass());

	UMaterial* MaterialAsset = Cast<UMaterial>(Asset);
	if (MaterialAsset == nullptr)
	{
		const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this));
		const FString AssetName = ObjectTools::SanitizeInvalidChars(Node->GetUAssetName(), INVALID_OBJECTNAME_CHARACTERS);
		const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

		UClass* AssetClass = UMaterial::StaticClass();
		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
		MaterialAsset = Cast<UMaterial>(AssetData.FastGetAsset(true));

		if (MaterialAsset == nullptr)
		{
			static const FName NAME_AssetTools = "AssetTools";
			IAssetTools* AssetTools = &FModuleManager::GetModuleChecked<FAssetToolsModule>(NAME_AssetTools).Get();
			UE_LOG_Figma2UMG(Display, TEXT("Create UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
			MaterialAsset = Cast<UMaterial>(AssetTools->CreateAsset(AssetName, PackagePath, AssetClass, Factory, FName("Figma2UMG")));
		}
		else
		{
			UE_LOG_Figma2UMG(Display, TEXT("Loading UAsset %s/%s of type %s"), *PackagePath, *AssetName, *AssetClass->GetDisplayNameText().ToString());
		}

		Asset = MaterialAsset;
	}

	if (MaterialAsset)
	{
		Setup();

		MaterialAsset->SetFlags(RF_Transactional);
		MaterialAsset->Modify();
	}
}

void UMaterialBuilder::LoadAssets()
{
	const FString PackagePath = UPackageTools::SanitizePackageName(Node->GetPackageNameForBuilder(this));
	const FString AssetName = ObjectTools::SanitizeInvalidChars(Node->GetUAssetName(), INVALID_OBJECTNAME_CHARACTERS);
	const FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(*PackageName, *AssetName, FString()));
	Asset = Cast<UMaterial>(AssetData.FastGetAsset(true));
}

void UMaterialBuilder::Reset()
{
	Asset = nullptr;
}

UPackage* UMaterialBuilder::GetAssetPackage() const
{
	return Asset ? Asset->GetPackage() : nullptr;
}

const TObjectPtr<UMaterial>& UMaterialBuilder::GetAsset() const
{
	return Asset;
}

void UMaterialBuilder::SetPaint(const FFigmaPaint* InPaint)
{
	Paint = InPaint;
}

void UMaterialBuilder::Setup() const
{
	Asset->MaterialDomain = MD_UI;
	FMaterialEditorUtilities::InitExpressions(Asset);
	
	int OutputIndex = 0;
	UMaterialExpression* PositionInput = SetupGradientInput(OutputIndex);
	if (UMaterialExpression* GradientExpression = SetupColorExpression(PositionInput, OutputIndex))
	{
		Asset->GetEditorOnlyData()->EmissiveColor.Connect(0, GradientExpression);
		Asset->PreEditChange(NULL);
		Asset->PostEditChange();
	}
	else
	{
		UE_LOG_Figma2UMG(Error, TEXT("[UMaterialBuilder::Setup] Node %s failed to create Gradient."), *Node->GetNodeName());
	}

	// TODO: Is the Editor is open, need to rebuild the graph to be up-to-date with the patch.
	//if (Asset->MaterialGraph)
	//{
	//	Asset->MaterialGraph->RebuildGraph();
	//}
}

UMaterialExpression* UMaterialBuilder::SetupGradientInput(int& OutputIndex) const
{
	if (!Paint)
		return nullptr;

	UMaterialExpression* InputExpression = nullptr;
	switch (Paint->Type)
	{
	case EPaintTypes::SOLID:
		return nullptr;
	case EPaintTypes::GRADIENT_LINEAR:
		InputExpression = SetupLinearGradientInput(OutputIndex);
		break;
	case EPaintTypes::GRADIENT_RADIAL:
		InputExpression = SetupRadialGradientInput(OutputIndex);
		break;
	case EPaintTypes::GRADIENT_ANGULAR:
		break;
	case EPaintTypes::GRADIENT_DIAMOND:
		break;
	case EPaintTypes::IMAGE:
		return nullptr;
	case EPaintTypes::EMOJI:
		return nullptr;
	case EPaintTypes::VIDEO:
		return nullptr;
	}
	return InputExpression;
}

UMaterialExpression* UMaterialBuilder::SetupLinearGradientInput(int& OutputIndex) const
{
	if (!Paint)
		return nullptr;

	if (Paint->GradientHandlePositions.Num() < 2)
		return nullptr;

	const FString LinearGradientFunctionPath(TEXT("/Engine/Functions/Engine_MaterialFunctions01/Gradient/LinearGradient.LinearGradient"));
	const FVector2D GradientStart = Paint->GradientHandlePositions[0].ToVector2D();
	const FVector2D GradientEnd = Paint->GradientHandlePositions[1].ToVector2D();
	const FVector2D GradientDir = (GradientEnd - GradientStart).GetSafeNormal();
	if (FMath::Abs(GradientDir.X) >= 0.999f)
	{
		OutputIndex = 0;
		UMaterialExpressionMaterialFunctionCall* LinearGradient = SetupMaterialFunction(LinearGradientFunctionPath);
		if (GradientDir.X < 0.0f)
		{
			return InvertOutput(LinearGradient, 0);
		}
		return LinearGradient;
	}
	else if (FMath::Abs(GradientDir.Y) >= 0.999f)
	{
		UMaterialExpressionMaterialFunctionCall* LinearGradient = SetupMaterialFunction(LinearGradientFunctionPath);
		if (GradientDir.Y < 0.0f)
		{
			OutputIndex = 0;
			return InvertOutput(LinearGradient, 1);
		}
		OutputIndex = 1;
		return LinearGradient;
	}
	else
	{
		OutputIndex = 0;
		UMaterialExpressionMaterialFunctionCall* LinearGradient = SetupMaterialFunction(LinearGradientFunctionPath , -1700.0f);
		return SetupLinearGradientCustomInput(LinearGradient);
	}
}

UMaterialExpression* UMaterialBuilder::SetupLinearGradientCustomInput(UMaterialExpressionMaterialFunctionCall* LinearGradientExpression) const
{
	const FString UVTransform("UVTransform");
	UMaterialExpressionCustom* UVTransformExpression = nullptr;
	for (UMaterialExpression* Expression : Asset->GetExpressions())
	{
		UMaterialExpressionCustom* ExpressionCustom = Cast<UMaterialExpressionCustom>(Expression);
		if (ExpressionCustom && ExpressionCustom->Description == UVTransform)
		{
			UVTransformExpression = ExpressionCustom;
			break;
		}
	}
	if (!UVTransformExpression)
	{
		UVTransformExpression = Cast<UMaterialExpressionCustom>(UMaterialEditingLibrary::CreateMaterialExpressionEx(Asset, nullptr, UMaterialExpressionCustom::StaticClass(), Asset, -1300.0f, 0.0f));
		UVTransformExpression->Description = UVTransform;
	}

	if (UVTransformExpression)
	{
		const FName UGradient("UGradient");
		const FName VGradient("VGradient");
		UVTransformExpression->OutputType = CMOT_Float1;
		if (UVTransformExpression->Inputs.Num() == 0)
		{
			FCustomInput InputU;
			InputU.InputName = UGradient;
			UVTransformExpression->Inputs.Add(InputU);
			UVTransformExpression->Inputs[0].Input.Connect(0, LinearGradientExpression);

			FCustomInput InputV;
			InputV.InputName = VGradient;
			UVTransformExpression->Inputs.Add(InputV);
			UVTransformExpression->Inputs[1].Input.Connect(1, LinearGradientExpression);
		}
		else if (UVTransformExpression->Inputs.Num() == 1)
		{
			UVTransformExpression->Inputs[0].InputName = UGradient;
			UVTransformExpression->Inputs[0].Input.Connect(0, LinearGradientExpression);

			FCustomInput InputV;
			InputV.InputName = VGradient;
			UVTransformExpression->Inputs.Add(InputV);
			UVTransformExpression->Inputs[1].Input.Connect(1, LinearGradientExpression);
		}
		else
		{
			UVTransformExpression->Inputs[0].InputName = UGradient;
			UVTransformExpression->Inputs[0].Input.Connect(0, LinearGradientExpression);

			UVTransformExpression->Inputs[1].InputName = VGradient;
			UVTransformExpression->Inputs[1].Input.Connect(1, LinearGradientExpression);
		}


		const FVector2D Start = Paint->GradientHandlePositions[0].ToVector2D();
		const FVector2D End = Paint->GradientHandlePositions[1].ToVector2D();
		const FVector2D Dir = End - Start;
		const FVector2D DirNorm = Dir.GetSafeNormal();
		UVTransformExpression->Code = "float2 Start = float2(" + FString::SanitizeFloat(Start.X) + ", " + FString::SanitizeFloat(Start.Y) + ");\n";
		UVTransformExpression->Code += "float2 DirNorm = float2(" + FString::SanitizeFloat(DirNorm.X) + ", " + FString::SanitizeFloat(DirNorm.Y) + ");\n";
		UVTransformExpression->Code += "float DirMag = " + FString::SanitizeFloat(Dir.Size()) + ";\n";
		UVTransformExpression->Code += "float2 UV = float2(UGradient, VGradient);\n";
		UVTransformExpression->Code += "float2 UVDir = UV - Start;\n";
		UVTransformExpression->Code += "float result = dot(UVDir,DirNorm)/DirMag;\n";
		UVTransformExpression->Code += "result = clamp(result, 0.0f, 1.0f);\n";
		UVTransformExpression->Code += "return result;";
	}

	return UVTransformExpression;
}

UMaterialExpression* UMaterialBuilder::SetupRadialGradientInput(int& OutputIndex) const
{
	UMaterialExpression* UVExpression = SetupUVInputExpression();

	const FString UVTransform("UVTransform");
	UMaterialExpressionCustom* UVTransformExpression = nullptr;
	for (UMaterialExpression* Expression : Asset->GetExpressions())
	{
		UMaterialExpressionCustom* ExpressionCustom = Cast<UMaterialExpressionCustom>(Expression);
		if (ExpressionCustom && ExpressionCustom->Description == UVTransform)
		{
			UVTransformExpression = ExpressionCustom;
			break;
		}
	}
	if (!UVTransformExpression)
	{
		UVTransformExpression = Cast<UMaterialExpressionCustom>(UMaterialEditingLibrary::CreateMaterialExpressionEx(Asset, nullptr, UMaterialExpressionCustom::StaticClass(), Asset, -1300.0f, 0.0f));
		UVTransformExpression->Description = UVTransform;
	}

	if (UVTransformExpression)
	{
		const FName UVGradient("UVGradient");
		UVTransformExpression->OutputType = CMOT_Float1;
		if (UVTransformExpression->Inputs.Num() == 0)
		{
			FCustomInput InputU;
			InputU.InputName = UVGradient;
			UVTransformExpression->Inputs.Add(InputU);
			UVTransformExpression->Inputs[0].Input.Connect(0, UVExpression);
		}
		else
		{
			UVTransformExpression->Inputs[0].InputName = UVGradient;
			UVTransformExpression->Inputs[0].Input.Connect(0, UVExpression);
		}


		const FVector2D Center = Paint->GradientHandlePositions[0].ToVector2D();
		const FVector2D Width = Paint->GradientHandlePositions[1].ToVector2D() - Center;
		const FVector2D Height = Paint->GradientHandlePositions[2].ToVector2D() - Center;
		const float Radius1 = Width.Size();
		const float Radius2 = Height.Size();
		if (FMath::Abs(Radius1 - Radius2) < 0.01f)
		{
			UVTransformExpression->Code = "float2 Center = float2(" + FString::SanitizeFloat(Center.X / 2) + ", " + FString::SanitizeFloat(Center.Y / 2) + ");\n";
			UVTransformExpression->Code += "float Radius = " + FString::SanitizeFloat(Radius1) + ";\n";
			UVTransformExpression->Code += "float result = distance(Center,UVGradient)/Radius;\n";
			UVTransformExpression->Code += "result = clamp(result, 0.0f, 1.0f);\n";
			UVTransformExpression->Code += "return result;";
		}
	}

	return UVTransformExpression;
}

UMaterialExpression* UMaterialBuilder::SetupColorExpression(UMaterialExpression* PositionInput, const int OutputIndex) const
{
	if (!Paint)
		return nullptr;

	UMaterialExpression* GradientExpression = nullptr;
	switch (Paint->Type)
	{
	case EPaintTypes::SOLID:
		return nullptr;
	case EPaintTypes::GRADIENT_LINEAR:
	case EPaintTypes::GRADIENT_RADIAL:
	case EPaintTypes::GRADIENT_ANGULAR:
	case EPaintTypes::GRADIENT_DIAMOND:
		GradientExpression = SetupGradientColorExpression(PositionInput, OutputIndex);
		break;
	case EPaintTypes::IMAGE:
		return nullptr;
	case EPaintTypes::EMOJI:
		return nullptr;
	case EPaintTypes::VIDEO:
		return nullptr;
	}
	return GradientExpression;
}

UMaterialExpression* UMaterialBuilder::SetupGradientColorExpression(UMaterialExpression* PositionInput, const int OutputIndex) const
{
	const FString Gradient("Gradient");
	UMaterialExpressionCustom* GradientLinearExpression = nullptr;
	for (UMaterialExpression* Expression : Asset->GetExpressions())
	{
		UMaterialExpressionCustom* ExpressionCustom = Cast<UMaterialExpressionCustom>(Expression);
		if (ExpressionCustom && ExpressionCustom->Description == Gradient)
		{
			GradientLinearExpression = ExpressionCustom;
			break;
		}
	}
	if (!GradientLinearExpression)
	{
		GradientLinearExpression = Cast<UMaterialExpressionCustom>(UMaterialEditingLibrary::CreateMaterialExpressionEx(Asset, nullptr, UMaterialExpressionCustom::StaticClass(), Asset, -700.0f, 0.0f));
		GradientLinearExpression->Description = Gradient;
	}

	if (GradientLinearExpression)
	{
		const FName InputPosition("InputPosition");
		GradientLinearExpression->OutputType = CMOT_Float4;
		if (GradientLinearExpression->Inputs.Num() == 0)
		{
			FCustomInput input;
			input.InputName = InputPosition;
			GradientLinearExpression->Inputs.Add(input);
			GradientLinearExpression->Inputs[0].Input.Connect(OutputIndex, PositionInput);
		}
		else
		{
			GradientLinearExpression->Inputs[0].InputName = InputPosition;
			if (GradientLinearExpression->Inputs[0].Input.Expression != PositionInput)
			{
				GradientLinearExpression->Inputs[0].Input.Connect(OutputIndex, PositionInput);
			}
		}

		if(Paint->GradientStops.Num() == 2)
		{
			GradientLinearExpression->Code = "float4 Color1 = float4(" + FString::SanitizeFloat(Paint->GradientStops[0].Color.R);
			GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[0].Color.G);
			GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[0].Color.B);
			GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[0].Color.A) + ");\n";

			GradientLinearExpression->Code += "float4 Color2 = float4(" + FString::SanitizeFloat(Paint->GradientStops[1].Color.R);
			GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[1].Color.G);
			GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[1].Color.B);
			GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[1].Color.A) + ");\n";

			GradientLinearExpression->Code += "if (InputPosition <= " + FString::SanitizeFloat(Paint->GradientStops[0].Position) + ")\n{\n  return Color1;\n}\n";
			GradientLinearExpression->Code += "else if (InputPosition >= " + FString::SanitizeFloat(Paint->GradientStops[1].Position) + ")\n{\n  return Color2;\n}\n";
			GradientLinearExpression->Code += "else \n{\n";
			GradientLinearExpression->Code += "  float4 ColorResult = lerp(Color1, Color2, InputPosition);\n  return ColorResult;\n}";
		}
		else
		{
			for (int i = 1; i < Paint->GradientStops.Num(); i++)
			{
				if (i == 1)
				{
					GradientLinearExpression->Code = "float4 Color1;\nfloat4 Color2;\nfloat Position = InputPosition;\n";
					GradientLinearExpression->Code += "if (InputPosition < " + FString::SanitizeFloat(Paint->GradientStops[i].Position) + ")\n{\n";
					GradientLinearExpression->Code += "  Position = InputPosition / " + FString::SanitizeFloat(Paint->GradientStops[i].Position) + ";\n";
				}
				else if (i == Paint->GradientStops.Num() - 1)
				{
					GradientLinearExpression->Code += "else\n{\n";
					GradientLinearExpression->Code += "  Position = (InputPosition - " + FString::SanitizeFloat(Paint->GradientStops[i - 1].Position) + ") / (" + FString::SanitizeFloat(Paint->GradientStops[i].Position - Paint->GradientStops[i - 1].Position) + ");\n";
				}
				else
				{
					GradientLinearExpression->Code += "else if (InputPosition < " + FString::SanitizeFloat(Paint->GradientStops[i].Position) + ")\n{\n";
					GradientLinearExpression->Code += "  Position = (InputPosition - " + FString::SanitizeFloat(Paint->GradientStops[i - 1].Position) + ") / (" + FString::SanitizeFloat(Paint->GradientStops[i].Position - Paint->GradientStops[i - 1].Position) + ");\n";
				}

				GradientLinearExpression->Code += "  Color1 = float4(" + FString::SanitizeFloat(Paint->GradientStops[i - 1].Color.R);
				GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[i - 1].Color.G);
				GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[i - 1].Color.B);
				GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[i - 1].Color.A) + ");\n";

				GradientLinearExpression->Code += "  Color2 = float4(" + FString::SanitizeFloat(Paint->GradientStops[i].Color.R);
				GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[i].Color.G);
				GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[i].Color.B);
				GradientLinearExpression->Code += ", " + FString::SanitizeFloat(Paint->GradientStops[i].Color.A) + ");\n";

				GradientLinearExpression->Code += "}\n";

			}

			GradientLinearExpression->Code += "float4 ColorResult = lerp(Color1, Color2, Position);\nreturn ColorResult;";
		}
	}
	return GradientLinearExpression;
}

UMaterialExpression* UMaterialBuilder::SetupUVInputExpression(float NodePosX /*= -1800.0f*/) const
{
	UMaterialExpressionTextureCoordinate* TextureCoord = nullptr;
	UMaterialExpressionComponentMask* MaskR = nullptr;
	for (UMaterialExpression* Expression : Asset->GetExpressions())
	{
		TextureCoord = Cast<UMaterialExpressionTextureCoordinate>(Expression);
		if (TextureCoord)
		{
			for (UMaterialExpression* Expression2 : Asset->GetExpressions())
			{
				UMaterialExpressionComponentMask* PossibleMask = Cast<UMaterialExpressionComponentMask>(Expression2);
				if (PossibleMask && PossibleMask->Input.Expression == TextureCoord)
				{
					MaskR = PossibleMask;
				}
			}
			break;
		}
	}

	if (!TextureCoord)
	{
		TextureCoord = Cast<UMaterialExpressionTextureCoordinate>(UMaterialEditingLibrary::CreateMaterialExpressionEx(Asset, nullptr, UMaterialExpressionTextureCoordinate::StaticClass(), Asset, NodePosX, 0.0f));
	}
	if (!MaskR)
	{
		MaskR = Cast<UMaterialExpressionComponentMask>(UMaterialEditingLibrary::CreateMaterialExpressionEx(Asset, nullptr, UMaterialExpressionComponentMask::StaticClass(), Asset, NodePosX + 200.0f, 0.0f));
	}


	if (TextureCoord && MaskR)
	{
		MaskR->R = 1;
		MaskR->G = 1;
		MaskR->B = 0;
		MaskR->A = 0;
		MaskR->Input.Connect(0, TextureCoord);
	}

	return MaskR;
}

UMaterialExpressionMaterialFunctionCall* UMaterialBuilder::SetupMaterialFunction(const FString& FunctionPath, float NodePosX /*= -1200.0f*/) const
{
	UMaterialFunction* MaterialFunction = LoadObject<UMaterialFunction>(NULL, *FunctionPath, NULL, 0, NULL);

	UMaterialExpressionMaterialFunctionCall* LinearGradientFunction = nullptr;
	for (UMaterialExpression* Expression : Asset->GetExpressions())
	{
		UMaterialExpressionMaterialFunctionCall* FunctionCall = Cast<UMaterialExpressionMaterialFunctionCall>(Expression);
		if (FunctionCall && FunctionCall->MaterialFunction && FunctionCall->MaterialFunction == MaterialFunction)
		{
			LinearGradientFunction = FunctionCall;
			break;
		}
	}

	if (!LinearGradientFunction)
	{
		LinearGradientFunction = Cast<UMaterialExpressionMaterialFunctionCall>(UMaterialEditingLibrary::CreateMaterialExpressionEx(Asset, nullptr, UMaterialExpressionMaterialFunctionCall::StaticClass(), Asset, NodePosX, 0.0f));
	}

	if (!LinearGradientFunction->MaterialFunction)
	{
		if (LinearGradientFunction->SetMaterialFunction(MaterialFunction))
		{
			LinearGradientFunction->PostEditChange();
		}
	}

	return LinearGradientFunction;
}

UMaterialExpression* UMaterialBuilder::InvertOutput(UMaterialExpression* OutputExpression, const int OutputIndex) const
{
	UMaterialExpressionSubtract* Subtract = nullptr;
	for (UMaterialExpression* Expression : Asset->GetExpressions())
	{
		UMaterialExpressionSubtract* MaterialExpressionMultiply = Cast<UMaterialExpressionSubtract>(Expression);
		if (MaterialExpressionMultiply && MaterialExpressionMultiply->B.Expression == OutputExpression)
		{
			Subtract = MaterialExpressionMultiply;
			break;
		}
	}

	if (!Subtract)
	{
		Subtract = Cast<UMaterialExpressionSubtract>(UMaterialEditingLibrary::CreateMaterialExpressionEx(Asset, nullptr, UMaterialExpressionSubtract::StaticClass(), Asset, -900.0f, 0.0f));
		Subtract->B.Connect(OutputIndex, OutputExpression);
	}

	Subtract->ConstA = 1.0f;

	return Subtract;
}
