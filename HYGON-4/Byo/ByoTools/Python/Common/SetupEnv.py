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
import os
import subprocess
from Misc import CopyFile, IsWindows, GetPackagesPath
from GlobalVariable import *
#
#Setup build environment
#
def SetupEnv(Workspace, ToolChain, log):
    #
    #on Windows, run like edksetup.bat VS2015 and use set get environment variable,
    #then parser console output and set os.environ
    #on Linux set related environment variable

    os.environ['WORKSPACE'] = Workspace
    EdkToolPath = os.path.join(Workspace, 'Edk2', 'BaseTools')
    os.environ['EDK_TOOLS_PATH'] = EdkToolPath
    if not os.path.exists(EdkToolPath):
        log.error("Workspace must be the root directory")
        return FILE_NOT_FOUND

    #
    #set PACKAGES_PATH
    #
    PackagesPath = GetPackagesPath(Workspace)
    os.environ["PACKAGES_PATH"] = os.pathsep.join(PackagesPath)

    #run windows edkset.bat and set environment variable
    if IsWindows():
        #Change the active console Code Page
        os.system('chcp 65001')

        EdkToolBinPath = os.path.join(Workspace, 'Byo', 'ByoTools', 'Bin', 'Win32')
        if os.path.exists(EdkToolBinPath):
            os.environ['EDK_TOOLS_BIN'] = EdkToolBinPath

        edksetup = os.path.join('Edk2', 'edksetup.bat')
        if ToolChain.lower() == 'vs2015x86':
            ToolChain = 'VS2015'
        if ToolChain:
            Cmd = edksetup + ' Reconfig ' + ToolChain + '&&set'
        else:
            Cmd = edksetup + ' Reconfig VS2015' + '&&set'
        log.info("Cmd: %s" % Cmd)
        try:
            p = subprocess.Popen(Cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=os.environ, cwd=Workspace, shell=True)
            while True:
                Line = p.stdout.readline()
                if not Line:
                    break
                Line = Line.decode(encoding='ascii', errors='ignore').strip()
                log.info(Line)
                if '=' in Line:
                    Name, Value = Line.split('=', 1)
                    if Name and Value:
                        os.environ[Name.strip()] = Value.strip()
            if p.returncode:
                return p.returncode
        except:
            log.error("Run cmd error")
            return COMMAND_FAILURE
        ToolChainPrefixEnv = ToolChain + '_PREFIX'
        if ToolChainPrefixEnv not in os.environ or not os.environ[ToolChainPrefixEnv]:
            log.error("Set build environment error")
            return COMMAND_FAILURE
        
        # Need to add Edk2 BaseTools Bin Win32 directory into PATH
        BinSubDir = os.path.join(Workspace, 'Edk2', 'BaseTools', 'Bin', 'Win32')
        if not os.path.exists(BinSubDir):
            os.mkdir(BinSubDir)
        if os.path.exists(EdkToolBinPath):
            os.environ['PATH'] = os.pathsep.join([BinSubDir, os.environ['PATH']])
        OpenSslPath = os.path.join(Workspace, 'Byo', 'ByoTools', 'Bin', 'OpenSSL')
        if os.path.exists(OpenSslPath):
            os.environ['PATH'] = os.pathsep.join([OpenSslPath, os.environ['PATH']])
        return COMMAND_SUCCESS
    else:
        #set Linux environment variable
        os.environ['PYTHONHASHSEED'] = '1'
        os.environ['PYTHON_COMMAND'] = 'python3'
        RunPythonPath = os.path.join(EdkToolPath, 'Source/Python')
        if 'PYTHONPATH' in os.environ:
            os.environ['PYTHONPATH'] = os.pathsep.join([RunPythonPath, os.environ.get('PYTHONPATH')])
        else:
            os.environ['PYTHONPATH'] = RunPythonPath
        ConfPath = os.path.join(Workspace, 'Conf')
        if not os.path.exists(ConfPath):
            os.mkdir(ConfPath)
        EdkConfPath = os.path.join(EdkToolPath, 'Conf')
        #copy conf files
        FromFiles = ['target.template', 'tools_def.template', 'build_rule.template']
        ToFiles = ['target.txt', 'tools_def.txt', 'build_rule.txt']
        for Index, File in enumerate(ToFiles):
            FromFile = os.path.join(EdkConfPath, FromFiles[Index])
            ToFile = os.path.join(ConfPath, File)
            if not os.path.exists(ToFile):
                CopyFile(FromFile, ToFile, log)

        #set PATH
        BinWrappers = os.path.join(EdkToolPath, 'BinWrappers/PosixLike')
        uname = os.uname()
        SubDir = '_'.join([uname.sysname, uname.machine])
        BinSubDir = os.path.join(EdkToolPath, 'Bin', SubDir)
        os.environ['PATH'] = os.pathsep.join([BinSubDir, BinWrappers, os.environ['PATH']])
        return COMMAND_SUCCESS
