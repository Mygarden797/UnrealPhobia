// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealPhobia : ModuleRules
{
	public UnrealPhobia(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Sockets", "Networking","AIModule","GameplayTasks","NavigationSystem" });

        PrivateDependencyModuleNames.AddRange(new string[] { "ProtobufCore" });

        PrivateIncludePaths.AddRange(new string[]
        {
            "UnrealPhobia/",
            "UnrealPhobia/Network/",
            "UnrealPhobia/Network/Contents/",
            "ProtobufCore/"
        });
    }
}
