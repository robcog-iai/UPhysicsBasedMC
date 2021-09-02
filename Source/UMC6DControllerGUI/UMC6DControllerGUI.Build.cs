// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

using UnrealBuildTool;

public class UMC6DControllerGUI : ModuleRules
{
	public UMC6DControllerGUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		//PrivatePCHHeaderFile = "Public/UMC6DControllerGUI.h";

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				//EnginePath + "Source/Runtime/Launch/Resources", // #include "Version.h"; #if ENGINE_MINOR_VERSION
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
				"InputCore",
				"UMC6DController",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				//"KantanChartsSlate",
				// ... add private dependencies that you statically link with here ...	
			}
			);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		string KantanCharts = PrivateDependencyModuleNames.Find(DependencyName => DependencyName.Equals("KantanChartsSlate"));
		if (string.IsNullOrEmpty(KantanCharts))
		{
			PublicDefinitions.Add("UMC_WITH_KANTAN=0");
		}
		else
		{
			PublicDefinitions.Add("UMC_WITH_KANTAN=1");
		}
	}
}
