// Copyright (c) Ubisoft. All Rights Reserved.
// Licensed under the Apache 2.0 License. See LICENSE.md in the project root for license information.
using System;
using System.IO;
using Sharpmake;

[module: Include(@"%EngineDir%/Engine/Source/EngineSolution.build.cs")]
[module: Include(@"%EngineDir%/Engine/Source/UserProject.build.cs")]
[module: Include(@"../Config/UserConfig.build.cs")]

[Generate]
public class GameProject : UserProject
{
    public GameProject()
    {
        Name = "GameProject";
    }

    public override void ConfigureAll(Configuration conf, EngineTarget target)
    {
        base.ConfigureAll(conf, target);
    }
}

[Generate]
public class GameSolution : EngineSolution
{
    public override void ConfigureAll(Configuration conf, EngineTarget target)
    {
        base.ConfigureAll(conf, target);

        // Add Projects
        {
            conf.AddProject<GameProject>(target);
            conf.AddProject<UserConfig>(target);
        }
    }
}

public static class Main
{
    [Sharpmake.Main]
    public static void SharpmakeMain(Arguments arguments)
    {
        KitsRootPaths.SetUseKitsRootForDevEnv(DevEnv.vs2022, KitsRootEnum.KitsRoot10, Options.Vc.General.WindowsTargetPlatformVersion.Latest);
        arguments.Generate<GameSolution>();
    }
}