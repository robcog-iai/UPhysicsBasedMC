// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

using System.IO;
using UnrealBuildTool;

public class UPhysicsBasedMC : ModuleRules
{
	public UPhysicsBasedMC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        // https://answers.unrealengine.com/questions/258689/how-to-include-private-header-files-of-other-modul.html        
        string EnginePath = Path.GetFullPath(Target.RelativeEnginePath);

        PublicIncludePaths.AddRange(
			new string[] {
				"UPhysicsBasedMC/Public",
				// ... add public include paths required here ...
                EnginePath + "Source/Runtime/Launch/Resources", // #include "Version.h"; #if ENGINE_MINOR_VERSION
            }
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"UPhysicsBasedMC/Private",
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
				"Slate",
				"SlateCore",
				"HeadMountedDisplay",
                "UPIDController",
                "SemLog",
                "UTags"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
