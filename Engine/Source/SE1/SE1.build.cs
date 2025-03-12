using System.IO;
using Sharpmake;

[Generate]
public class SE1 : CommonProject
{
    public SE1()
    {
        ResourceFilesExtensions.Add(".rc");
        ResourceFilesExtensions.Add(".ico");
        ResourceFilesExtensions.Add(".bmp");
        ResourceFilesExtensions.Add(".rc2");
    }
    public override void ConfigureAll(Configuration conf, EngineTarget target)
    {
        base.ConfigureAll(conf, target);
        if (target.LaunchType == ELaunchType.Editor)
        {
            conf.Options.Add(Options.Vc.General.MfcSupport.UseMfcDynamic);
            conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);
        }
        else if(target.LaunchType == ELaunchType.Client)
        {
            conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);
        }

        //conf.SolutionFolder = "Engine";
        conf.Output = Configuration.OutputType.Exe;

        conf.AddPublicDependency<Core>(target);
        conf.AddPublicDependency<Launch>(target);

        if (target.LaunchType == ELaunchType.Editor || target.LaunchType == ELaunchType.Client)
        {
            //conf.AddPublicDependency<Slate>(target);
            conf.AddPublicDependency<Renderer>(target);
            //conf.AddPublicDependency<D3D11RHI>(target);
        }
    }
}
