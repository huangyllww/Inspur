#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
## @file
#
# Copyright (c) 2020, Byosoft Corporation.<BR>
# All rights reserved.This software and associated documentation (if any)
# is furnished under a license and may only be used or copied in
# accordance with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be reproduced,
# stored in a retrieval system, or transmitted in any form or by any
# means without the express written consent of Byosoft Corporation.
#

##
# Import Modules
#
import sys
sys.path.append('./Byo/ByoTools/Python/Common')
from SetupEnv import SetupEnv
from Log import BuildLog
from ArgParser import ArgParser
from Misc import *

def main():
    #Parser argument
    Arg = ArgParser()
    LogFile = 'Build.log'
    if Arg.LogFile:
        LogFile = Arg.LogFile
    if Arg.Clean:
        Buildlog = BuildLog()
    else:
        Buildlog = BuildLog(LogFile)
    Log = Buildlog.log
    Workspace = os.getcwd()
    Conf = os.path.join(Workspace, 'Conf')
    if not os.path.exists(Conf):
        os.mkdir(Conf)
    if Arg.ToolChain:
        ToolChain = Arg.ToolChain[0]
    else:
        ToolChain = 'VS2015x86'

    if Arg.BuildTarget:
        BuildTarget = Arg.BuildTarget[0]
    else:
        BuildTarget = 'DEBUG'

    MacroStr = ''
    if Arg.Macros:
        Macros = Arg.Macros
        for Item in Macros:
            MacroStr += ' -D %s' % Item

    ReportFile = ''
    if Arg.ReportFile:
        ReportFile = " -y %s" % Arg.ReportFile

    TargetArch = ''
    if Arg.TargetArch:
        for Arch in Arg.TargetArch:
            TargetArch += " -a %s" % Arch

    PlatformOption = ''
    if Arg.PlatformFile:
        PlatformOption = " -p %s" % Arg.PlatformFile

    ModuleOption = ''
    if Arg.ModuleFile:
        ModuleOption = " -m %s" % Arg.ModuleFile

    ExitCode = SetupEnv(Workspace, ToolChain, Log)
    if ExitCode:
        return ExitCode

    if Arg.Clean:
        CleanBuildFiles(Workspace, "ByoModule", "", BuildTarget, ToolChain, LogFile, ReportFile, Log)
        return
    #
    #BuildTools
    #
    ExitCode = BuildTools(Workspace, Log)
    if ExitCode:
        return ExitCode

    #
    #Build Command
    #
    Cmd = f"build {PlatformOption} {ModuleOption} {TargetArch} -b {BuildTarget} -t {ToolChain} {MacroStr} {ReportFile}"
    ExitCode = RunCommand(Cmd, Log)
    if ExitCode:
        return ExitCode

if __name__ == "__main__":
    sys.exit(main())

