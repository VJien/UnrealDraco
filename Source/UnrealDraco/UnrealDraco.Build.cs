// Copyright VJ. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System.Collections;
using System.Collections.Generic;


public class UnrealDraco : ModuleRules
{
	public UnrealDraco(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDefinitions.AddRange(new string[] {
            "DRACO_MESH_COMPRESSION_SUPPORTED",
            "DRACO_STANDARD_EDGEBREAKER_SUPPORTED",
            "DRACO_PREDICTIVE_EDGEBREAKER_SUPPORTED"
        });

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
				
				// ... add private dependencies that you statically link with here ...	
			}
			);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);



        string DracoPath = Path.Combine(ModuleDirectory, "../libdraco_ue4");
        //string DracoPath = Path.Combine(ThirdPartyPath, "Draco");
        string DracoInclude = Path.Combine(DracoPath, "include");
        string DracoLib = Path.Combine(DracoPath, "lib");

        List<string> LibPaths = new List<string>();
        List<string> LibFilePaths = new List<string>();

        if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        {
            string PlatformName = "";
#if UE_4_23_OR_LATER
            if (Target.Platform == UnrealTargetPlatform.Win32)
            {
                PlatformName = "win32";
            }
            else if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                PlatformName = "win64";
            }
#else
            switch (Target.Platform)
            {
                case UnrealTargetPlatform.Win32:
                    PlatformName = "win32";
                    break;
                case UnrealTargetPlatform.Win64:
                    PlatformName = "win64";
                    break;
            }
#endif

            string TargetConfiguration = "Release";
            if (Target.Configuration == UnrealTargetConfiguration.Debug)
            {
                TargetConfiguration = "Debug";
            }

            LibPaths.Add(System.IO.Path.Combine(DracoPath, "lib", PlatformName, "vs2019", TargetConfiguration));

            LibFilePaths.Add("dracodec.lib");
            LibFilePaths.Add("dracoenc.lib");
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            LibPaths.Add(System.IO.Path.Combine(DracoPath, "lib", "linux"));

            LibFilePaths.Add("libdracodec.a");
            LibFilePaths.Add("libdracoenc.a");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            LibPaths.Add(System.IO.Path.Combine(DracoPath, "lib", "macos"));

            LibFilePaths.Add("libdracodec.a");
            LibFilePaths.Add("libdracoenc.a");
        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
        {
            LibPaths.Add(System.IO.Path.Combine(DracoPath, "lib", "android", "armeabi-v7a"));
            LibPaths.Add(System.IO.Path.Combine(DracoPath, "lib", "android", "armeabi-v7a-with-neon"));
            LibPaths.Add(System.IO.Path.Combine(DracoPath, "lib", "android", "arm64-v8a"));
            LibPaths.Add(System.IO.Path.Combine(DracoPath, "lib", "android", "x86"));
            LibPaths.Add(System.IO.Path.Combine(DracoPath, "lib", "android", "x86_64"));

            LibFilePaths.Add("libdracodec.a");
            LibFilePaths.Add("libdracoenc.a");
        }
        else if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            LibPaths.Add(System.IO.Path.Combine(DracoPath, "lib", "ios", "os"));
            LibPaths.Add(System.IO.Path.Combine(DracoPath, "lib", "ios", "simulator"));
            LibPaths.Add(System.IO.Path.Combine(DracoPath, "lib", "ios", "watchos"));

            LibFilePaths.Add("libdracodec.a");
            LibFilePaths.Add("libdracoenc.a");
        }


        PublicIncludePaths.Add(Path.GetFullPath(DracoInclude));
        PublicSystemLibraryPaths.AddRange(LibPaths);

        PublicSystemLibraries.AddRange(LibFilePaths);


        // PublicLibraryPaths.Add(Path.GetFullPath(DracoLib));



    }
}
