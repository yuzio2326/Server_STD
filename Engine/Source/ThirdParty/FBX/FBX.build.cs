// Copyright (c) Ubisoft. All Rights Reserved.
// Licensed under the Apache 2.0 License. See LICENSE.md in the project root for license information.
using System;
using System.IO;
using Sharpmake;

[Generate]
public class FBX : CommonProject
{
    public FBX() { }

    public override void ConfigureAll(Configuration conf, EngineTarget target)
    {
        base.ConfigureAll(conf, target);
        //conf.SolutionFolder = @"Engine/Source/ThirdParty";
        conf.AddPublicDependency<Core>(target);

        string FBXSDKDir = "[project.SourceRootPath]/" + "2020.3.7/" ;
        conf.IncludePaths.Add(FBXSDKDir + "include");
        conf.IncludePaths.Add(FBXSDKDir + "include/fbxsdk");
        if (target.Optimization == Optimization.Debug)
        {
            conf.LibraryPaths.Add(FBXSDKDir + "lib/x64/debug");
        }
        else
        {
            conf.LibraryPaths.Add(FBXSDKDir + "lib/x64/release");
        }
        if (target.LaunchType == ELaunchType.Editor /*| target.LaunchType == ELaunchType.Client*/)
        {
            conf.LibraryFiles.Add("libfbxsdk.lib");
            conf.EventPreBuild.Add(@"(robocopy ""[project.SourceRootPath]/2020.3.7/lib/x64/release"" " + @"""" + conf.TargetPath + @"""" + " libfbxsdk.dll /njh /njs /ndl /nc /ns) ^& IF %ERRORLEVEL% LEQ 3 exit 0");
        }
        else if (target.LaunchType == ELaunchType.Client)
        {
            conf.LibraryFiles.Add("libfbxsdk-md.lib");
            conf.LibraryFiles.Add("libxml2-md.lib");
            conf.LibraryFiles.Add("zlib-md.lib");
        }
    }
}