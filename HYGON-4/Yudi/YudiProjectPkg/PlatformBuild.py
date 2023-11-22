#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
## @file
#
# Copyright (c) 2023, Byosoft Corporation.<BR>
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
import shutil
import os
sys.path.append('./Byo/ByoTools/Python/Common')
from SetupEnv import SetupEnv
from Log import BuildLog
from ArgParser import ArgParser
from Misc import *
from collections import OrderedDict

HygonRepo = 'Hygon4'
HygonCrbPkg = 'CrbProject'
ProjectRepo = 'Yudi'
ProjectPkgDir = 'YudiProjectPkg'
ProjectSetupVariableHFile  = '_PlatSetupVariable.h'
PlatformSetupVariableHFile = 'PlatSetupVariable.h'

# 0 - NanHaiVtb1 (2 sockets, 1DPC, 24 dimms)
# 1 - NanHaiVtb3 (1 socket, 2DPC, 24 dimms)
# 2 - OEM, same with NanHaiVtb1.
BoardIdType = 2

TKN_BUILD_TOOLS = 1
TKN_INSTANT_DEBUG_ENABLE = 0
TKN_HYGON_PSP_TPCM_ENABLE = 1
TKN_DATE_IN_FILENAME = 1
TKN_FVMAIN_HASH_CHECK = 0
TKN_CUSTOMER_ID = 0x0

# 1 - append 256 sig data to bios data. (16M + 256)
# 2 - sig info and data at [0,4K].      (16M)
# 3 - sig info and data at target FV    (16M)
TKN_SIGN_METHOD = 2

TKN_CBS_OVERRIDE = 0

gBinOutput   = ""
gFvPath      = ""
gByoToolPath = ""

def RmFilesInDir(path_file):
    ls = os.listdir(path_file)
    for i in ls:
        f_path = os.path.join(path_file, i)
        os.remove(f_path)


def FileStrReplace(file_path, old_str, new_str):
  try:
    f = open(file_path,'r+')
    all_lines = f.readlines()
    f.seek(0)
    f.truncate()
    for line in all_lines:
      line = line.replace(old_str, new_str)
      f.write(line)
    f.close()
  except Exception as e:
    print(e)


def FileCmp(f1, f2):
    st1 = os.stat(f1)
    st2 = os.stat(f2)

    if st1.st_size != st2.st_size:
        return False

    bufsize = 8*1024
    with open(f1, 'rb') as fp1, open(f2, 'rb') as fp2:
        while True:
            b1 = fp1.read(bufsize)
            b2 = fp2.read(bufsize)
            if b1 != b2:
                return False
            if not b1:
                return True


def CopyIfChanged(Src, Target):
  if not os.path.exists(Src):
    return

  if not os.path.exists(Target) or not FileCmp(Target, Src):
    print("copy %s to %s" % (Src, Target))
    shutil.copy(Src, Target)




def MergePspAndSign(Workspace, IsMp, BiosIdBaseName, PlatformFmmtFd, PspLog, log):

    global gBinOutput
    global gFvPath
    global gByoToolPath

    # HgpiModulePkg\HYGONTools\PspDirectoryTool
    # Hygon4\HgpiModulePkg\Firmwares
    PspTool     = os.path.join(Workspace, HygonRepo, 'HgpiModulePkg', 'HYGONTools', 'PspDirectoryTool', 'BuildPspDirectory.exe')
    PspDataPath = os.path.join(Workspace, HygonRepo, 'HgpiModulePkg', 'Firmwares')
    SignTool    = os.path.join(gByoToolPath, 'SignBios', 'ByoBiosSign.exe')
    SignPvkKey  = os.path.join(gByoToolPath, 'SignBios', 'pvk.pem')    
    
    if IsMp:
      EsMpFolder = 'mp'
      OutputFileName = BiosIdBaseName + ".bin"
      PspDataSoc0 = os.path.join(PspDataPath, 'Soc0MP.xml') 
    else:
      EsMpFolder = 'es'
      OutputFileName = BiosIdBaseName + "_" + EsMpFolder + ".bin"
      PspDataSoc0 = os.path.join(PspDataPath, 'Soc0ES.xml')      

    OutputFilePathName = os.path.join(gBinOutput, OutputFileName)

    os.chdir(os.path.join(Workspace, HygonRepo))
    Cmd = f'{PspTool} -o {gBinOutput} bb {PlatformFmmtFd} {PspDataSoc0} {OutputFileName} >> {PspLog} 2>&1'
    ExitCode = RunCommand(Cmd, None)

    if ExitCode:
        return ExitCode

    os.chdir(Workspace)

    Cmd = f'{SignTool} -sign {OutputFilePathName} {SignPvkKey} m{TKN_SIGN_METHOD}'
    ExitCode = RunCommand(Cmd, log)
    

def MainBuild(Workspace, BuildTarget, ToolChain, log, MacroStr = '', ReportFile = '', ModuleOption = ''):

    global TKN_INSTANT_DEBUG_ENABLE
    global gBinOutput
    global gFvPath
    global gByoToolPath    
    InputDict = OrderedDict()

    BuildOutput = os.path.join(Workspace, 'Build', ProjectPkgDir, BuildTarget + '_' + ToolChain)
    BUILD_TYPE = ''

    gByoToolPath = os.path.join(Workspace, 'Byo', 'ByoTools', 'Bin')
    Win32Tool   = os.path.join(gByoToolPath, 'Win32Tool.exe')

    global TKN_CUSTOMER_ID
    if 'ALIBABA' in MacroStr:
        CustomerId = 0x1
        TKN_CUSTOMER_NAME = 'Ali'
    elif 'TENCENT' in MacroStr:
        CustomerId = 0x2
        TKN_CUSTOMER_NAME = 'Tencent'
    elif 'PINGAN' in MacroStr:
        CustomerId = 0x3
        TKN_CUSTOMER_NAME = 'PingAn'
    elif 'BAIDU' in MacroStr:
        CustomerId = 0x4
        TKN_CUSTOMER_NAME = 'BaiDu'
    elif 'JINGDONG' in MacroStr:
        CustomerId = 0x5
        TKN_CUSTOMER_NAME = 'JingDong'
    elif 'STORAGE' in MacroStr:
        CustomerId = 0x7
        TKN_CUSTOMER_NAME = 'Storage'
    else:
        CustomerId = 0x0
        TKN_CUSTOMER_NAME = 'Standard'
    MacroStr += ' -D TKN_CUSTOMER_ID=0x%x' % CustomerId

    #prebuild step
    #generate biosid.bin
    if not os.path.exists(BuildOutput):
        os.makedirs(BuildOutput)
    BiosIdBin = os.path.join(BuildOutput, 'BiosId.bin')
    if BuildTarget.upper() == 'RELEASE':
        BUILD_TYPE = '-r'

    GenBiosIdPy = os.path.join(Workspace, 'Byo', 'ByoTools', 'Python', 'GenBiosId.py')
    if BoardIdType == 2:
      BiosIdEnv   = os.path.join(ProjectRepo, ProjectPkgDir, 'BiosIdInspur.env')        
    else:
      return -1
      
    BiosIdBaseNameFile = os.path.join(BuildOutput, 'biosidbasename')

    # Hygon4\HgpiPkg\Addendum\Hpcb\HyEx\NanHaiVtb1\Release\HPCB.bin -> Hygon4\Build\Hpcb\HyEx\Release\HPCB.bin
    # Hygon4\HgpiPkg\Addendum\Hpcb\HyEx\NanHaiVtb3\Release\HPCB.bin -> Hygon4\Build\Hpcb\HyEx\Release\HPCB.bin
    if BoardIdType == 0:
      HpcbFile = os.path.join(Workspace, HygonRepo, 'HgpiPkg', 'Addendum', 'Hpcb', 'HyEx', 'NanHaiVtb1', 'Release', 'HPCB.bin')
    elif BoardIdType == 1:
      HpcbFile = os.path.join(Workspace, HygonRepo, 'HgpiPkg', 'Addendum', 'Hpcb', 'HyEx', 'NanHaiVtb3', 'Release', 'HPCB.bin')
    elif BoardIdType == 2:
      HpcbFile = os.path.join(Workspace, ProjectRepo, ProjectPkgDir, 'Hpcb', 'HyEx', 'Oem', 'Release', 'HPCB.bin')       
    else:
      return -1
   
    ApcbTargetPath = os.path.join(Workspace, HygonRepo, 'Build', 'Hpcb', 'HyEx', 'Release')
    if not os.path.exists(ApcbTargetPath):
        os.makedirs(ApcbTargetPath)
    shutil.copy(HpcbFile, ApcbTargetPath)

    Cmd = f'{GenBiosIdPy} -i {BiosIdEnv} {BUILD_TYPE} -o {BiosIdBin} -ofn {BiosIdBaseNameFile}'
    ExitCode = RunCommand(Cmd, None, IsPython=True)
    if ExitCode:
        return ExitCode

    #with open(BiosIdBaseNameFile, 'r') as f:
    #  BiosIdBaseName = f.read()

    # Read input file and get config
    with open(BiosIdEnv, 'r') as File:
      FileLines = File.readlines()
    for Line in FileLines:
        if Line.strip().startswith('#'):
            continue
        if '=' in Line:
            Key, Value = Line.split('=')
            InputDict[Key.strip()] = Value.strip()

    BiosIdBaseName = InputDict['BOARD_ID'] + InputDict['BOARD_REV'] + '_' + 'BIOS' + '_' +\
                     str(int(InputDict['REVISION'])) + '.' + InputDict['VERSION_MAJOR'][0:2].zfill(2) + '.' + InputDict['VERSION_MAJOR'][2:].zfill(2) + '_' + TKN_CUSTOMER_NAME 

    BiosVersion = str(int(InputDict['REVISION'])) + '.' + InputDict['VERSION_MAJOR'][0:2].zfill(2) + '.' + InputDict['VERSION_MAJOR'][2:].zfill(2)
    MacroStr += ' -D TKN_BIOS_VERSION=%s' % BiosVersion
						
    if TKN_DATE_IN_FILENAME:
        with open(BiosIdBin, 'r') as f:
          biosid = f.read()
          if len(biosid) == 74:
            date = biosid[0x2e] + '' + biosid[0x30] + biosid[0x32] + biosid[0x34] + biosid[0x36] + biosid[0x38]
            BiosIdBaseName = BiosIdBaseName + '_' + '20'+date

    if BuildTarget.upper() == 'DEBUG':
        BiosIdBaseName = BiosIdBaseName + '_D'
    
    MacroStr += ' -D TKN_BIOS_IMAGE_BASENAME=%s' % BiosIdBaseName
    if BuildTarget.upper() == 'DEBUG':     
        MapFile = os.path.join(BuildOutput, ProjectPkgDir + '.map')
        MapDbRaw = os.path.join(BuildOutput, 'MapDb.raw')
        if not os.path.exists(MapDbRaw):
            open (MapDbRaw, "wb").close()
    else:
        TKN_INSTANT_DEBUG_ENABLE = 0

    if TKN_INSTANT_DEBUG_ENABLE:
      MacroStr += ' -D INSTANT_DEBUG_ENABLE=TRUE'
    else:
      MacroStr += ' -D INSTANT_DEBUG_ENABLE=FALSE'

    MacroStr += ' -D TKN_SIGN_METHOD=%d' % TKN_SIGN_METHOD
    MacroStr += ' -D TKN_BOARD_ID_TYPE=%d' % BoardIdType
    
    if TKN_HYGON_PSP_TPCM_ENABLE:
      MacroStr += ' -D TKN_HYGON_PSP_TPCM_ENABLE=TRUE'    
    else:
      MacroStr += ' -D TKN_HYGON_PSP_TPCM_ENABLE=FALSE'      

    if TKN_FVMAIN_HASH_CHECK:
      MacroStr += ' -D TKN_FVMAIN_HASH_CHECK=TRUE'  
    else:
      MacroStr += ' -D TKN_FVMAIN_HASH_CHECK=FALSE' 
      
    #
    #Platform Build Step
    #
    PlatformDscFile = os.path.join(ProjectPkgDir, 'ProjectPkg.dsc')
    if ReportFile:
        ReportFile = " -y %s" % ReportFile
    Cmd = f'build -a IA32 -a X64 -b {BuildTarget} -p {PlatformDscFile} {ModuleOption} -t {ToolChain} {MacroStr} {ReportFile}'
    ExitCode = RunCommand(Cmd, log)
    if ExitCode:
        return ExitCode

    if ModuleOption:
        return 0
        
    if TKN_INSTANT_DEBUG_ENABLE: 
        #
        #Generate MapDbRaw
        #
        GenSetupRawPy = os.path.join(Workspace, 'Byo', 'ByoTools', 'Python', 'ConvertMapToFunctionBin.py')
        Cmd = f'{GenSetupRawPy} -i {MapFile} -o {MapDbRaw}'
        ExitCode = RunCommand(Cmd, log, IsPython=True)
        if ExitCode:
            return ExitCode

        #
        #Generate FD image again with MapDbRaw
        #
        Cmd = f'build -a IA32 -a X64 -b {BuildTarget} -p {PlatformDscFile} {ModuleOption} -t {ToolChain} {MacroStr} {ReportFile} -u fds'
        ExitCode = RunCommand(Cmd, log)
        if ExitCode:
            return ExitCode

    #
    #post build step
    #
    gBinOutput = os.path.join(Workspace, 'Build', 'BIN')
    if not os.path.exists(gBinOutput):
        os.mkdir(gBinOutput)
        
    RmFilesInDir(gBinOutput)    
        
    #FCE generates the default setting
    PlatformSetupFd = 'CRB2021.fd'
    gFvPath = os.path.join(BuildOutput, 'FV')
    PlatformFdPath = os.path.join(gFvPath, PlatformSetupFd)
    PlatformOutFd  = os.path.join(gFvPath, 'Bios.fd')
    #FCE read
    FceReadOut = os.path.join(gFvPath, 'FceRead.out')
    Cmd = f'FCE read -i {PlatformFdPath} 0006 005C 0078 0030 0030 0030 0031 > {FceReadOut}'
    ExitCode = RunCommand(Cmd, log)
    if ExitCode:
        return ExitCode
    #FCE update
    Cmd = f'FCE update -i {PlatformFdPath} -s {FceReadOut} -g 004AE66F-F074-4398-B47F-F73BA682C7BE -o {PlatformOutFd}'
    ExitCode = RunCommand(Cmd, log)
    if ExitCode:
        return ExitCode
    
    #Generate SetupItemsDb.ffs
    SetupItemsDbRaw = os.path.join(gFvPath, 'SetupItemsDb.raw')
    SetupItemsNameRaw = os.path.join(gFvPath, 'SetupItemsNameField.raw')
    SetupItemsOptionsRaw = os.path.join(gFvPath, 'SetupItemsOptions.raw')
    SetupItemsNameString = os.path.join(gFvPath, 'SetupItemsNameString.raw')
    SetupItemsOptionsStr = os.path.join(gFvPath, 'SetupItemsOptionsStr.raw')
    AdvancedLstDir = os.path.join(BuildOutput, 'X64')
    PlatformFmmtFd = os.path.join(gFvPath, "Bios_Fmmt.fd")

    LstFiles = [] 
    AllFiles = os.listdir(AdvancedLstDir)
    for OneFile in AllFiles:
        if OneFile.endswith('.lst'):
            LstFiles.append (OneFile)

    if len (LstFiles) == 0:
        log.error ("VFR lst file can not found")
        log.error ("Can't generate %s " % SetupItemsDbRaw)
        return FILE_NOT_FOUND
    else:
        # Generate SetupItemsDb.raw
        GenSetupRawPy = os.path.join(Workspace, 'Byo', 'ByoTools', 'Python', 'GenSetupRaw.py')
        Cmd = f'{GenSetupRawPy} -i {FceReadOut} -l {AdvancedLstDir} -o {SetupItemsDbRaw} -on {SetupItemsNameRaw} -oo {SetupItemsOptionsRaw} -os {SetupItemsNameString} -oos {SetupItemsOptionsStr}'
        ExitCode = RunCommand(Cmd, log, IsPython=True)
        if ExitCode:
            return ExitCode
        # GenSec
        SecName = os.path.join(gFvPath, 'SetupItemsDbSEC1.raw')
        Cmd = f'GenSec -s EFI_SECTION_RAW -o {SecName} {SetupItemsDbRaw}'
        ExitCode = RunCommand(Cmd, log)
        if ExitCode:
            return ExitCode
        SecName1 = os.path.join(gFvPath, 'SetupItemsNameFieldSEC1.raw')
        Cmd = f'GenSec -s EFI_SECTION_RAW -o {SecName1} {SetupItemsNameRaw}'
        ExitCode = RunCommand(Cmd, log)
        if ExitCode:
            return ExitCode
        SecName2 = os.path.join(gFvPath, 'SetupItemsOptionSEC2.raw')
        Cmd = f'GenSec -s EFI_SECTION_RAW -o {SecName2} {SetupItemsOptionsRaw}'
        ExitCode = RunCommand(Cmd, log)
        if ExitCode:
            return ExitCode
        SecName3 = os.path.join(gFvPath, 'SetupItemsNameStringSEC3.raw')
        Cmd = f'GenSec -s EFI_SECTION_RAW -o {SecName3} {SetupItemsNameString}'
        ExitCode = RunCommand(Cmd, log)
        if ExitCode:
            return ExitCode
        SecName4 = os.path.join(gFvPath, 'SetupItemsNameStringSEC4.raw')
        Cmd = f'GenSec -s EFI_SECTION_RAW -o {SecName4} {SetupItemsOptionsStr}'
        ExitCode = RunCommand(Cmd, log)
        if ExitCode:
            return ExitCode            
        # GenFfs
        FfsName = os.path.join(gFvPath, 'SetupItemsDb.ffs')
        FileGuid = "cee2ff3a-a442-4cb0-b92d-d4cdd23f56aa"
        Cmd = f'GenFfs -t EFI_FV_FILETYPE_FREEFORM -g {FileGuid} -o {FfsName} -i {SecName} -i {SecName1} -i {SecName2} -i {SecName3} -i {SecName4}'
        ExitCode = RunCommand(Cmd, log)
        if ExitCode:
            return ExitCode
        # fmmt -a image.fd DE8EDB3B-3119-4436-9662-13680908B2F3 SetupItemsDb.ffs Platform_Fmmt.fd
        #FVMAIN_COMPACT FvNameGuid  = DE8EDB3B-3119-4436-9662-13680908B2F3
        FvNameGuid = '2BB161D9-DC5E-4FCF-8358-7C1B91930679'
        Cmd = f'fmmt -a {PlatformOutFd} {FvNameGuid} {FfsName} {PlatformFmmtFd}'
        ExitCode = RunCommand(Cmd, log)
        if ExitCode:
            return ExitCode

    if TKN_FVMAIN_HASH_CHECK:
        ByoHygonTool = os.path.join(Workspace, HygonCrbPkg, 'Platform4Pkg', 'Tool', 'ByoHygonTool.exe')
        Cmd = f'{ByoHygonTool} -hash {PlatformFmmtFd}'
        ExitCode = RunCommand(Cmd, log)

    #
    # PSP generation
    #
    PspLog = os.path.join(Workspace, 'Build', 'psp.log')
    if os.path.exists(PspLog):
        os.remove(PspLog)    
    MergePspAndSign(Workspace, True, BiosIdBaseName, PlatformFmmtFd, PspLog, log)
    MergePspAndSign(Workspace, False, BiosIdBaseName, PlatformFmmtFd, PspLog, log)


def main():

    if not os.path.exists('Build'):
        os.mkdir('Build')

    Arg = ArgParser()
    LogFile = 'Build/build.log'
    if Arg.LogFile:
        LogFile = Arg.LogFile

    if Arg.Clean:
        Buildlog = BuildLog()
    else:
        Buildlog = BuildLog(LogFile)

    Log = Buildlog.log
    RunCommand("subst O: .", Log)
    
    Workspace = "O:\\"
    if Arg.ToolChain:
        ToolChain = Arg.ToolChain[0]
    else:
        ToolChain = 'VS2015x86'

    if Arg.BuildTarget:
        BuildTarget = Arg.BuildTarget[0]
    else:
        BuildTarget = 'RELEASE'

    MacroStr = ' -D PROJECT_PKG=%s' % ProjectPkgDir

    if Arg.Macros:
        Macros = Arg.Macros
        for Item in Macros:
            MacroStr += ' -D %s' % Item

    ReportFile = ''
    if Arg.ReportFile:
        ReportFile = Arg.ReportFile

    ModuleOption = ''
    if Arg.ModuleFile:
        ModuleOption = " -m %s" % Arg.ModuleFile

    ExitCode = SetupEnv(Workspace, ToolChain, Log)
    if ExitCode:
       RunCommand("subst /D O:", Log)
       return ExitCode

    if Arg.Clean:
        CleanBuildFiles(Workspace, ProjectPkgDir, "", BuildTarget, ToolChain, LogFile, ReportFile, Log)
        RunCommand("subst /D O:", Log)
        return

    #
    #BuildTools
    #
    if not os.getenv('TKN_BUILD_TOOLS', str(TKN_BUILD_TOOLS)) == '0':
        print('BuildTools')
        ExitCode = BuildTools(Workspace, Log)
        if ExitCode:
            RunCommand("subst /D O:", Log)
            return ExitCode

    SetupVariableHSrc = os.path.join(Workspace, ProjectRepo, ProjectPkgDir, 'Include', ProjectSetupVariableHFile)
    SetupVariableHTar = os.path.join(Workspace, HygonCrbPkg, 'Platform4Pkg', 'Include', PlatformSetupVariableHFile)
    CopyIfChanged(SetupVariableHSrc, SetupVariableHTar)
    
    if TKN_CBS_OVERRIDE:
      CbsOrPath = os.path.join(Workspace, ProjectRepo, ProjectPkgDir, 'CbsOverride')
      CbsPath   = os.path.join(Workspace, HygonRepo, 'AmdCbsPkg', 'Library', 'Family', '0x17', 'ZP', 'External')
      CopyIfChanged(os.path.join(CbsOrPath, '_AmdCbsForm.vfr'), os.path.join(CbsPath, 'Resource', 'AmdCbsForm.vfr'))
      CopyIfChanged(os.path.join(CbsOrPath, '_AmdCbsStrings.uni'), os.path.join(CbsPath, 'Resource', 'AmdCbsStrings.uni'))
      CopyIfChanged(os.path.join(CbsOrPath, '_AmdCbsDefault.c'), os.path.join(CbsPath, 'Resource', 'AmdCbsDefault.c'))     
      

    ExitCode = MainBuild(Workspace, BuildTarget, ToolChain, Log, MacroStr, ReportFile, ModuleOption)
    if ExitCode:
        RunCommand("subst /D O:", Log)
        return ExitCode

    RunCommand("subst /D O:", Log)

if __name__ == "__main__":
    sys.exit(main())
