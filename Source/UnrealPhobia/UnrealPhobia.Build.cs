// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealPhobia : ModuleRules
{
	public UnrealPhobia(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Sockets", "Networking","AIModule","GameplayTasks","NavigationSystem","UMG",
            "Slate",      // Slate 위젯 스타일
            "SlateCore",   // FSlateBrush 등
            "Json",       // JSON 파싱
            "HTTP"       // HTTP 요청

         });

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
