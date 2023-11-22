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
import sys
import subprocess
import time
import shutil
import glob
import datetime
from struct import pack
from GlobalVariable import *


def IsWindows():
    if sys.platform.startswith('win'):
        return True
    else:
        return False

def RunCommand(Cmd, log = None, WorkDir = None, IsPython = False):
    if IsPython:
        if 'PYTHON_COMMAND' in os.environ:
            Cmd = os.environ['PYTHON_COMMAND'] + ' ' + Cmd
        elif 'PYTHON_HOME' in os.environ:
            Cmd = os.path.join(os.environ['PYTHON_HOME'], 'python.exe') + ' ' + Cmd
        else:
            if log:
                log.error("Not found PYTHON_HOME or PYTHON_COMMAND")
            else:
                print ("Not found PYTHON_HOME or PYTHON_COMMAND")
            return COMMAND_FAILURE

    if log:
        log.info("Cmd: %s" % Cmd)
    Failed = False
    if Cmd.strip().startswith('None '):
        Failed = True
    try:
        Start = int(round(time.time() * 1000))
        try:
            p = subprocess.Popen(Cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=WorkDir)

            while True:
                Line = p.stdout.readline()
                if not Line:
                    break
                Line = Line.decode(encoding='ascii', errors='ignore').strip()
                if log:
                    log.info(Line)
                else:
                    if ('): error' in Line) or ('): warning' in Line) or (': fatal error' in Line):
                        print (Line)
                TmpStr = Line.split()
                if Line.strip().startswith('- Failed -') or \
                        (len(TmpStr) >= 2 and TmpStr[0].endswith(':') \
                         and TmpStr[1] == 'ERROR') or (len(TmpStr) >= 3 \
                         and TmpStr[0] == 'Error' and TmpStr[2] == '-') or \
                        ((len(TmpStr) >= 3) and ('error:' in TmpStr[2])):
                    Failed = True

            p.communicate()

        except subprocess.CalledProcessError as  e:
            Ret = e.returncode
        else:
            if p.returncode:
                Ret = p.returncode
            else:
                Ret = COMMAND_SUCCESS
        finally:
            if log:
                log.info('[cmd=%s]' % Cmd)
            elif Failed:
                print ('[cmd=%s]' % Cmd)
        End = int(round(time.time() * 1000))

    except Exception as e:
        if log:
            log.error(e)
        else:
            print (e)
        raise RuntimeError
    else:
        if Failed:
            Ret = COMMAND_FAILURE
        if Ret == COMMAND_SUCCESS:
            Ret_str = 'SUCCESS'
        else:
            Ret_str = 'FAIL'
        if log:
            log.info("\t - %s : %d ms\n" % (Ret_str, End - Start))
        else:
            if (Ret_str == 'FAIL'):
                print ("\t - %s : %d ms\n" % (Ret_str, End - Start))
    return Ret

#
#From is file or directory
#To is file or directory
#if From is directory,  To must be directory.
#
def CopyFile(From, To, log):
    log.info("Copy %s to %s" % (From, To))
    if not os.path.exists(From):
        log.info(f'{From}  not found')
        return FILE_NOT_FOUND
    if os.path.isdir(From):
        if not os.path.isdir(To):
            log.error(f'{To} should be directory')
            return COMMAND_FAILURE
        for file in os.listdir(From):
            shutil.copy2(os.path.join(From, file), To)
    else:
        shutil.copy2(From, To)

#
#Merge FileList into file
#
def MergeFile(FileList, To, log):
    log.info("Merge %s to %s" % (' '.join(FileList), To))
    Buffer = bytearray()
    try:
        for File in FileList:
            with open(File, 'rb') as InFile:
                Buffer += InFile.read()
        with open(To, 'wb') as OutFile:
            OutFile.write(Buffer)
    except:
        log.error(f"Merge {FileList} to {To} Fail")

#Run make command
def RunMake(WorkDir, log):
    if IsWindows():
        cmd = 'nmake'
    else:
        cmd = 'make'
    ExitCode = RunCommand(cmd, log, WorkDir=WorkDir)
    return ExitCode

def BuildTools(Workspace, log):
    if IsWindows() and os.path.exists(os.environ['EDK_TOOLS_BIN']):
        #
        # Use pre-built binary tools on windows
        #
        return 0

    #
    #Build Edk2/BaseTools
    #
    BasetoolsPath = os.path.join(Workspace, 'Edk2', 'BaseTools')
    ExitCode = RunMake(BasetoolsPath, log)
    if ExitCode:
        return ExitCode

    #
    #Build Edk2Platforms/Silicon/Intel/Tools
    #
    SiliconToolsPath = os.path.join(Workspace, 'Edk2Platforms', 'Silicon', 'Intel', 'Tools')
    if os.path.exists(SiliconToolsPath):
        ExitCode = RunMake(SiliconToolsPath, log)
        if ExitCode:
            return ExitCode
    return ExitCode

def GetPackagesPath(root):
    """ Gets all recursive package paths in specified directory.
        A directory is a package path if it satisfies conditions below:
        1. it is a directory
        2. it is not an EDK II Package. An EDK II Package (directory) is
           a directory that contains an EDK II package declaration (DEC) file.
        3. it contains at least one first level EDK II Package.
        Note: A directory is not package path but its subdirectory could be.
        Example: edk2-platforms/Features is not package path
        but edk2-platforms/Features/Intel is.

        :param root: The specified directory to find package paths in it,
            the caller should ensure it is an valid directory
        :type root: String
        :returns: Return all recursive package paths
        :rtype: String list
    """

    Paths = []
    ContainPackage = False
    for Filename in os.listdir(root):
        # skip files whose name starts with ".", such as ".git"
        if Filename.startswith('.') or Filename.startswith('Build'):
            continue
        Filepath = os.path.join(root, Filename)
        if os.path.isdir(Filepath):
            if glob.glob(os.path.join(Filepath, '*.dec')) or Filepath.endswith('Pkg'):
                # it is an EDK II Package
                ContainPackage = True
            else:
                # get package paths for subdirectory if it is not package
                Paths = Paths + GetPackagesPath(Filepath)

    if ContainPackage:
        # root is a package path because it contains EDK II Package
        # in first level folder, inset it to head of list
        Paths.insert(0, root)

    # return package paths
    return Paths

#
#Get UTC time data
#
def GetUtcTimeData():
    Today = datetime.datetime.utcnow()
    ReversedYear = str(Today.year)[::-1]
    ReversedDate = str(Today.strftime("%m%d"))[::-1]
    ReversedTime = str(Today.strftime("%H%M"))[::-1]

    Year, Date, Time = map(lambda x: ''.join([hex(ord(num))[2:] for num in x]), [ReversedYear, ReversedDate, ReversedTime])
    return Year, Date, Time

#
#Fill file with 0xff or 0x00
#
def FillPad(InFile, OutFile, Size, Prefix=False, FillValue=0xff):
    BufferFull = bytearray()
    with open(InFile, 'rb') as InHandle:
        BufferInFile =bytearray(InHandle.read())
    Length = len(BufferInFile)
    Index = 0
    while Index < Size - Length:
        if Prefix:
            BufferFull += pack('=B', FillValue)
            if Index + 1 == Size - Length:
                BufferFull += BufferInFile
        else:
            if Index == 0:
                BufferFull += BufferInFile
            BufferFull += pack('=B', FillValue)
        Index += 1

    with open(OutFile, 'wb') as OutHandle:
        OutHandle.write(BufferFull)
#
# remove file or directory
#
def Remove(Path, Log):
    if not os.path.exists(Path):
        return
    Log.info(f"Remove {Path}")
    if os.path.isdir(Path):
        shutil.rmtree(Path)
    else:
        os.remove(Path)

#
#Clean build files
#
def CleanBuildFiles(Workspace, ByoCrbPkg, PlatformFspPkg, BuildTarget, ToolChain, LogFile, ReportFile, Log):
    Build = os.path.join(Workspace, "Build")
    BuildOutput = os.path.join(Build, ByoCrbPkg, BuildTarget + '_' + ToolChain)
    BuildFspOutput = os.path.join(Build, PlatformFspPkg, BuildTarget + '_' + ToolChain)
    BuildBin = os.path.join(Build, "BIN")
    BuildRom = os.path.join(Build, "ROM")
    Conf = os.path.join(Workspace, "Conf")
    Edk2BaseTools = os.path.join(Workspace, 'Edk2', 'BaseTools')
    SiliconToolsPath = os.path.join(Workspace, 'Edk2Platforms', 'Silicon', 'Intel', 'Tools')
    for File in [BuildOutput, BuildFspOutput, BuildBin, BuildRom, Conf, LogFile, ReportFile]:
        if os.path.exists(File):
            Remove(File, Log)
    for ToolPath in [Edk2BaseTools, SiliconToolsPath]:
        if not os.path.exists(ToolPath):
            continue
        if IsWindows():
            Cmd = "nmake clean"
            RunCommand(Cmd, Log, ToolPath)
            EdkToolBin = os.environ['EDK_TOOLS_BIN']
            if os.path.exists(EdkToolBin) and EdkToolBin.startswith(Edk2BaseTools):
                for File in os.listdir(EdkToolBin):
                    Remove(File, Log)
        else:
            Cmd = "make clean"
            RunCommand(Cmd, Log, ToolPath)

#
#Trim binary file
#
def TrimFile(InFile, OutFile, From, To):
    BufferOut = bytearray()
    with open(InFile, 'rb') as InHandle:
        BufferInFile =bytearray(InHandle.read())
    Length = len(BufferInFile)
    if From > To or To > Length:
        print ("Input Offset in invalid")
        return 1
    BufferOut += BufferInFile[From:To]
    with open(OutFile, 'wb') as OutHandle:
        OutHandle.write(BufferOut)