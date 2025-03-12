using System;
using System.IO;
using Sharpmake;

[module: Include(@"%EngineDir%/Engine/FrontendEngine.build.cs")]

[Generate]
public class FrontendGame : CommonProject
{
    public FrontendGame() : base(false)
    {
        Name = "Game";

        SourceFilesExtensions.Add(".ini");

        AddTargets(new EngineTarget(
            ELaunchType.Editor | ELaunchType.Client | ELaunchType.Server,
            Platform.win64,
            DevEnv.vs2022,
            Optimization.Debug | Optimization.Release,
            OutputType.Lib,
            Blob.NoBlob,
            BuildSystem.FastBuild
        ));
    }

    public override void ConfigureAll(Configuration conf, EngineTarget target)
    {
        base.ConfigureAll(conf, target);
        conf.SolutionFolder = @"Games";
        conf.Output = Configuration.OutputType.Utility;
        conf.EventPreBuild.Clear();
    }
}


[Generate]
public class FrontendGameSolution : FrontendEngineSolution
{
    public FrontendGameSolution()
    {
        //Name = "SE";
    }

    public override void ConfigureAll(Configuration conf, EngineTarget target)
    {
        base.ConfigureAll(conf, target);
        conf.AddProject<FrontendGame>(target);
    }
}

public static class Main
{
    [Sharpmake.Main]
    public static void SharpmakeMain(Sharpmake.Arguments arguments)
    {
        KitsRootPaths.SetUseKitsRootForDevEnv(DevEnv.vs2022, KitsRootEnum.KitsRoot10, Options.Vc.General.WindowsTargetPlatformVersion.Latest);
        arguments.Generate<FrontendGameSolution>();
    }
}
