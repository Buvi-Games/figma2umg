// MIT License
// Copyright (c) 2024 Buvi Games
#include "UI/Figma2UMGStyle.h"

#include "Brushes/SlateImageBrush.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/Paths.h"
#include "Styling/SlateStyle.h"
#include "Rendering/SlateRenderer.h"
#include "Styling/SlateStyleRegistry.h"

TUniquePtr<FSlateStyleSet> FFigma2UMGStyle::MSStyleInstance;

void FFigma2UMGStyle::Initialize()
{
	if (!MSStyleInstance.IsValid())
	{
		MSStyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*MSStyleInstance);
	}
}

void FFigma2UMGStyle::Shutdown()
{
	if (MSStyleInstance.IsValid())
	{		
		FSlateStyleRegistry::UnRegisterSlateStyle(*MSStyleInstance);
		MSStyleInstance.Reset();
	}
}

FName FFigma2UMGStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("Figma2UMGStyle"));
	return StyleSetName;
}

FName FFigma2UMGStyle::GetContextName()
{
	return FName(TEXT("Figma2UMG"));
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define IMAGE_BRUSH_SVG( RelativePath, ... ) FSlateVectorImageBrush( Style->RootToContentDir(RelativePath, TEXT(".svg")), __VA_ARGS__)
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TUniquePtr< FSlateStyleSet > FFigma2UMGStyle::Create()
{
	TUniquePtr< FSlateStyleSet > Style = MakeUnique<FSlateStyleSet>(GetStyleSetName());
	Style->SetContentRoot(FPaths::ProjectPluginsDir() / TEXT("Figma2UMG/Resources"));
	return Style;
}

void FFigma2UMGStyle::SetIcon(const FString& StyleName, const FString& ResourcePath)
{
	if (!FSlateApplication::IsInitialized())
		return;

	FSlateStyleSet* Style = MSStyleInstance.Get();

	FString Name(GetContextName().ToString());
	Name = Name + "." + StyleName;
	Style->Set(*Name, new IMAGE_BRUSH(ResourcePath, Icon40x40));

	Name += ".Small";
	Style->Set(*Name, new IMAGE_BRUSH(ResourcePath, Icon20x20));

	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

void FFigma2UMGStyle::SetSVGIcon(const FString& StyleName, const FString& ResourcePath)
{
	if (!FSlateApplication::IsInitialized())
		return;

	FSlateStyleSet* Style = MSStyleInstance.Get();

	FString Name(GetContextName().ToString());
	Name = Name + "." + StyleName;
	Style->Set(*Name, new IMAGE_BRUSH_SVG(ResourcePath, Icon40x40));

	Name += ".Small";
	Style->Set(*Name, new IMAGE_BRUSH_SVG(ResourcePath, Icon20x20));

	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

#undef IMAGE_BRUSH
#undef IMAGE_BRUSH_SVG

const ISlateStyle& FFigma2UMGStyle::Get()
{
	check(MSStyleInstance);
	return *MSStyleInstance;
}
