// Copyright (c) Ubisoft. All Rights Reserved.
// Licensed under the Apache 2.0 License. See LICENSE.md in the project root for license information.
using System;
using System.IO;
using System.Collections.Generic;
using Sharpmake;

[module: Include(@"Utils.cs")]

public abstract class UserProject : CommonProject
{
    protected UserProject() { }

    public override void ConfigureAll(Configuration conf, EngineTarget target)
    {
        base.ConfigureAll(conf, target);
        conf.AddPublicDependency<Engine>(target);
    }
}