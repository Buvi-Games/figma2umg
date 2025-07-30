// MIT License
// Copyright (c) 2024 Buvi Games

using UnrealBuildTool;

public class Figma2UMG : ModuleRules
{
	public Figma2UMG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        //bUsePrecompiled = true;

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "HTTP",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
                "UnrealEd",
                "ImageCore",
                "ImageWrapper",
                "Slate",
                "SlateCore",
                "InputCore",
                "EditorFramework",
                "EditorSubsystem",
                "EditorStyle",
                "UnrealEd",
                "BlueprintGraph",
                "MaterialEditor",
                "ToolMenus",
                "ContentBrowserData",
                "Json",
                "JsonUtilities",
                "HTTP",
                "AssetTools",
                "AssetRegistry", 
                "UMGEditor",
                "UMG"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        //This needs to match the HTTP module
        PrivateDefinitions.Add("WITH_CURL= " + (bPlatformSupportsCurl ? "1" : "0"));
    }
    private bool bPlatformSupportsCurl { get { return bPlatformSupportsLibCurl || bPlatformSupportsXCurl; } }

    protected virtual bool bPlatformSupportsLibCurl
    {
        get
        {
            return (Target.Platform.IsInGroup(UnrealPlatformGroup.Windows) /*&& !Target.WindowsPlatform.bUseXCurl For 5.2. Remove this when 5.2 is not supported anymore*/) ||
                   Target.IsInPlatformGroup(UnrealPlatformGroup.Unix) ||
                   Target.IsInPlatformGroup(UnrealPlatformGroup.Android);
        }
    }

    protected virtual bool bPlatformSupportsXCurl { get { return false; } }
}
