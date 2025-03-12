// Copyright (c) Ubisoft. All Rights Reserved.
// Licensed under the Apache 2.0 License. See LICENSE.md in the project root for license information.
using System;
using System.IO;
using Sharpmake;

// 이 cs는 config 파일 관리를 위함이며, 빌드 목적이 아닙니다.
// 이 프로젝트로 OutDir/Config 폴더에 ini 파일들을 복사 합니다.
[Generate]
public class EngineShaders : CommonProject
{
    public EngineShaders() 
    {
        SourceFilesExtensions.Add(".hlsl");
        SourceFilesExtensions.Add(".fxh");
    }

    public override void ConfigureAll(Configuration conf, EngineTarget target)
    {
        base.ConfigureAll(conf, target);
        conf.Output = Configuration.OutputType.Utility;

        conf.TargetPath += "/Shaders";

        conf.EventPreBuild.Clear();
        conf.EventPreBuild.Add(@"(robocopy /s ""[project.SourceRootPath]"" " + @"""" + conf.TargetPath + @"""" + " *.hlsl *.fxh /njh /njs /ndl /nc /ns) ^& IF %ERRORLEVEL% LEQ 3 exit 0");
    }
}