// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VRRacer : ModuleRules
{
	public VRRacer(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "eXiSoundVis" });
		//PrivateDependencyModuleNames.AddRange(new string[] { "SoundVisualizations" });
		//PrivateIncludePathModuleNames.AddRange(new string[] { "SoundVisualizations" });
	}
}
