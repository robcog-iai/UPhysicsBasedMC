// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

using UnrealBuildTool;

public class UMC6DController : ModuleRules
{
	public UMC6DController(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		//PrivatePCHHeaderFile = "Public/UMCDController.h";

		//// https://answers.unrealengine.com/questions/258689/how-to-include-private-header-files-of-other-modul.html
		//string EnginePath = Path.GetFullPath(Target.RelativeEnginePath);

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
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"UMCPIDController",
				"HeadMountedDisplay", // UMotionControllerComponent				
				// ... add private dependencies that you statically link with here ...	
			}
			);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		PublicDefinitions.Add("UMC_WITH_CHART=0");
	}
}
