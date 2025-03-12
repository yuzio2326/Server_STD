// Copyright (c) Ubisoft. All Rights Reserved.
// Licensed under the Apache 2.0 License. See LICENSE.md in the project root for license information.
using System;
using System.IO;
using Sharpmake;

[Generate]
public class D3D11RHI : CommonProject
{
    public D3D11RHI() { }

    public override void ConfigureAll(Configuration conf, EngineTarget target)
    {
        base.ConfigureAll(conf, target);
        conf.AddPublicDependency<Core>(target);
        conf.AddPublicDependency<RenderCore>(target);
        conf.AddPublicDependency<RHI>(target);

        string[] AllD3DLibs = new string[]
        {
            "dxgi.lib",
            //"d3d9.lib",
            "d3d11.lib",
            "dxguid.lib",
            "D3DCompiler.lib",
            //"dinput8.lib",
            //"xapobase.lib",
        };
        conf.LibraryFiles.AddRange(AllD3DLibs);
    }
}