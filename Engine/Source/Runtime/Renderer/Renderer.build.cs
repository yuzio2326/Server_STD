// Copyright (c) Ubisoft. All Rights Reserved.
// Licensed under the Apache 2.0 License. See LICENSE.md in the project root for license information.
using System;
using System.IO;
using Sharpmake;

[Generate]
public class Renderer : CommonProject
{
    public Renderer() { }

    public override void ConfigureAll(Configuration conf, EngineTarget target)
    {
        base.ConfigureAll(conf, target);

        conf.AddPublicDependency<Engine>(target);
        conf.AddPublicDependency<RenderCore>(target);
        conf.AddPublicDependency<RHI>(target);
    }
}