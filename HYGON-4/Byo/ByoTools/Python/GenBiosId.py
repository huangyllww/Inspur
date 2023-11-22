## @file
# Trim files preprocessed by compiler
#
# Copyright (c) 2022, Byosoft Corporation.<BR>
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
import time
import logging
import struct
import datetime
import argparse
import platform
from collections import OrderedDict
try:
    from configparser import ConfigParser
except:
    from ConfigParser import ConfigParser

# Config message
_BIOS_Signature = "$IBIOSI$"

_ConfigItem = {
    "BOARD_ID": {'Value': '', 'Length': 7, 'InVal':''},
    "BOARD_REV": {'Value': '', 'Length': 1, 'InVal':''},
    "OEM_ID": {'Value': '', 'Length': 3, 'InVal':''},
    "VERSION_MAJOR": {'Value': '0000', 'Length': 4, 'InVal':''},
    "REVISION": {'Value': '00', 'Length': 2, 'InVal':''},    
}

# Version message
__prog__ = 'GenBiosld'
__description__ = 'Trim files preprocessed by compiler'
__copyright__ = 'Copyright (c) 2021, ByoSoft. All rights reserved.<BR> '
__version__ = '%s Version %s' % (__prog__, '0.1 ')

# ExtraData message
_Usage = "Usage: GenBiosId -i Configfile -o OutputFile [-ot OutputTextFile]"
_ConfigSectionNotDefine = "Not support the config file format, need config section"
_ErrorMessageTemplate = '\n\n%(tool)s...\n : error: %(msg)s\n\t%(extra)s'
_ErrorLogger = logging.getLogger("tool_error")
_ErrorFormatter = logging.Formatter("%(message)s")
_ConfigLenInvalid = "Config item %s length is invalid"
_ConfigItemInvalid = "Item %s is invalid"

# Error message
INFO = 20
ERRORCODE = 50
OPTION_MISSING = 'Missing option'
FORMAT_INVALID = 'Invalid syntax/format'
FILE_NOT_FOUND = 'File/directory not found in workspace'
FORMAT_UNKNOWN_ERROR = 'Unknown error in syntax/format'
FORMAT_NOT_SUPPORTED = 'Not supported syntax/format'

BUILD_TYPE = 'D'

def SetEdkLogger():
    _ErrorLogger.setLevel(INFO)
    _ErrorCh = logging.StreamHandler(sys.stderr)
    _ErrorCh.setFormatter(_ErrorFormatter)
    _ErrorLogger.addHandler(_ErrorCh)
    return _ErrorLogger


# Output the error message and exit the tool
def EdkLogger(ToolName, Message, ExtraData):
    _ErrorLogger = SetEdkLogger()
    TemplateDict = {"tool": ToolName, "msg": Message, "extra": ExtraData}
    LogText = _ErrorMessageTemplate % TemplateDict
    _ErrorLogger.log(ERRORCODE, LogText)
    sys.exit(1)


# Open the file in the correct way
def FileOpen(FileName, Mode, Buffer=-1):
    def LongFilePath(FileName):
        FileName = os.path.normpath(FileName)
        if platform.system() == 'Windows':
            if FileName.startswith('\\\\?\\'):
                return FileName
            if FileName.startswith('\\\\'):
                return '\\\\?\\UNC\\' + FileName[2:]
            if os.path.isabs(FileName):
                return '\\\\?\\' + FileName
        return FileName

    return open(LongFilePath(FileName), Mode, Buffer)


# Parse command line options
def MyOptionParser():
    parser = argparse.ArgumentParser(prog=__prog__,
                                     description=__description__ + __copyright__ + _Usage,
                                     conflict_handler='resolve')
    parser.add_argument('-v', '--version', action='version', version=__version__,
                        help="show program's version number and exit")
    parser.add_argument('-i', '--int', metavar='FILENAME', dest='InputFile', help="Input Config file")
    parser.add_argument('-o', '--out', metavar='FILENAME', dest='OutputFile', help="Output file")
    parser.add_argument('-ofn', '--text', metavar='FILENAME', dest='OutputTextFile', help="file name for biosid")
    parser.add_argument('-nt', '--notimestamp', dest='NoTimestamp', action='store_true', default=False, help="Set timestamp to zero")
    parser.add_argument('-r', '--release', dest='BuildRelease', action='store_true', default=False, help="build release version")
    Options = parser.parse_args()
    return Options


# Check the Tool for missing variables
def CheckOptions(Options):
    if not Options.InputFile or not Options.OutputFile:
        EdkLogger("GenBiosId", OPTION_MISSING, ExtraData=_Usage)
    InputFile = Options.InputFile
    OutputFile = Options.OutputFile
    OutputTextFile = Options.OutputTextFile
    NoTimestamp = Options.NoTimestamp
    BuildRelease = Options.BuildRelease
    if not os.path.exists(InputFile):
        EdkLogger("GenBiosId", FILE_NOT_FOUND, ExtraData="Input file not found")
    return InputFile, OutputFile, OutputTextFile, NoTimestamp, BuildRelease

# Read input file and get config
def ReadInputFile(InputFile):
    InputDict = OrderedDict()
    with open(InputFile) as File:
        FileLines = File.readlines()
    for Line in FileLines:
        if Line.strip().startswith('#'):
            continue
        if '=' in Line:
            Key, Value = Line.split('=')
            InputDict[Key.strip()] = Value.strip()
    return InputDict


# Parse the input file and extract the information
def ParserInputFile(InputDict, NoTimestamp):
    for Item in InputDict:
        if Item not in _ConfigItem:
            EdkLogger("GenBiosId", FORMAT_INVALID, ExtraData=_ConfigItemInvalid % Item)
        _ConfigItem[Item]['Value'] = InputDict[Item]
        _ConfigItem[Item]['InVal'] = InputDict[Item]

        if Item == "BOARD_ID" or Item == "OEM_ID" :
          ItemStrLen = len(_ConfigItem[Item]['Value'])
          if ItemStrLen < _ConfigItem[Item]['Length'] :
            _ConfigItem[Item]['Value'] = _ConfigItem[Item]['Value'] + '_' * (_ConfigItem[Item]['Length'] - ItemStrLen)

        if len(_ConfigItem[Item]['Value']) != _ConfigItem[Item]['Length']:
            EdkLogger("GenBiosId", FORMAT_INVALID, ExtraData=_ConfigLenInvalid % Item)
    for Item in _ConfigItem:
        if not _ConfigItem[Item]['Value']:
            EdkLogger("GenBiosId", FORMAT_UNKNOWN_ERROR, ExtraData="Item %s is missing" % Item)
    utcnow = datetime.datetime.now()
    if NoTimestamp:
        TimeStamp = "\0\0\0\0\0\0\0\0\0\0"
    else:
        TimeStamp = time.strftime("%y%m%d%H%M", utcnow.timetuple())

    Id_Str = _ConfigItem['BOARD_ID']['Value'] + \
             _ConfigItem['BOARD_REV']['Value'] + '.' + \
             _ConfigItem['OEM_ID']['Value'] + '.' + \
             BUILD_TYPE + \
             _ConfigItem['VERSION_MAJOR']['Value'] + '.' + \
             TimeStamp
    return Id_Str


# Output information to a file
def PrintOutputFile(OutputFile, OutputTextFile, Id_Str):
    with FileOpen(OutputFile, 'wb') as FdOut:
        for i in _BIOS_Signature:
            FdOut.write(struct.pack('B', ord(i)))

        for i in Id_Str:
            FdOut.write(struct.pack('H', ord(i)))

        FdOut.write(struct.pack('H', 0x00))
        
        if _ConfigItem["REVISION"]['InVal']:
            for i in _ConfigItem["REVISION"]['InVal']:
                FdOut.write(struct.pack('H', ord(i)))
            FdOut.write(struct.pack('H', 0x00))

    if OutputTextFile:
        MyFileName =  _ConfigItem['BOARD_ID']['InVal'] + '_' + \
                      _ConfigItem['OEM_ID']['InVal'] + '_' + \
                      BUILD_TYPE + \
                      _ConfigItem['VERSION_MAJOR']['InVal']
                      
        if _ConfigItem["REVISION"]['InVal']:
            MyFileName += _ConfigItem["REVISION"]['InVal']
                      
        with FileOpen(OutputTextFile, 'w') as FdOut:
            FdOut.write(MyFileName)


# Tool entrance method
def Main():
    global BUILD_TYPE
    Options = MyOptionParser()
    InputFile, OutputFile, OutputTextFile, NoTimestamp, BuildRelease = CheckOptions(Options)

    if BuildRelease:
      BUILD_TYPE = 'R'

#   print(BUILD_TYPE)

    InputDict = ReadInputFile(InputFile)
    Id_Str = ParserInputFile(InputDict, NoTimestamp)
    PrintOutputFile(OutputFile, OutputTextFile, Id_Str)
    return 0


if __name__ == '__main__':
    r = Main()
    ## 0-127 is a safe return range, and 1 is a standard default error
    if r < 0 or r > 127: r = 1
    sys.exit(r)
