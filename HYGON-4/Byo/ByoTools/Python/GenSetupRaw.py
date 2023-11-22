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
import re
import os
from io import StringIO, BytesIO
from collections import OrderedDict
import struct
import argparse
import uuid
#
# Globals for help information
#
__prog__        = 'GenSetupRaw'
__version__     = '%s Version %s' % (__prog__, '0.1 ')
__copyright__   = 'Copyright (c) 2020, Byosoft Corporation. All rights reserved.'
__description__ = 'Generate SetupItemsDB.raw\n'

VariableType = ['ONE_OF', 'NUMERIC', 'CHECKBOX', 'ORDERED_LIST']
ONE_OF = 0x05
CHECKBOX = 0x06
NUMERIC = 0x07
ORDERED_LIST = 0x23
OpCodeDict = {
    'ONE_OF'  : ONE_OF,
    'CHECKBOX': CHECKBOX,
    'NUMERIC' : NUMERIC,
    'ORDERED_LIST' : ORDERED_LIST,
}

SizeTypeDict = {
    1: 'UINT8',
    2: 'UINT16',
    4: 'UINT32',
    8: 'UINT64'
}

BiosId = 'BIOS:2.0.ID.AL.\n'
ByoCfgStructStart = 'BYOCFG_STRUCT_START\n'
ByoCfgStructEnd = 'BYOCFG_STRUCT_END\n'

UintToSize = {
    'BOOLEAN' : 1,
    'UINT8'   : 1,
    'UINT16'  : 2,
    'UINT32'  : 4,
    'UINT64'  : 8
}

SizeToCode = {
    1 : 0x00,
    2 : 0x01,
    4 : 0x02,
    8 : 0x03,
}

SizeToPack = {
    1 : 'B',
    2 : 'H',
    4 : 'I',
    8 : 'Q'
}

MAX_EFI_VARIABLE_NAME_LENGTH = 40

def ClearContents(Contents):
    Buffer = StringIO()
    for Line in Contents.split('\n'):
        Line = Line.strip()
        if not Line:
            continue
        elif Line.startswith('//') or Line.startswith('#'):
            continue
        elif Line.startswith('>'):
            continue
        else:
            Buffer.write(Line)
            Buffer.write('\n')
    return Buffer.getvalue()

def GetOffset(ItemSize, Name):
    if '[' not in Name:
        return UintToSize[ItemSize]
    else:
        NumPattern = re.compile('\[(\d+)\]')
        Num = NumPattern.search(Name)
        return UintToSize[ItemSize] * int(Num.group(1))

def GetAllStruct(Contents):#{struct:lst file}
    #StructsList = {"structName1":[{'ItemName':'Item1Name', 'offset': 0x0},{'ItemName':'Item2Name', 'offset': 0x1} ...]
    #          "structName2":[{'ItemName':'Item1Name', 'offset': 0x0},{'ItemName':'Item2Name', 'offset': 0x1} ...] ...}
    StructsDict={}
    #match struct CPU_SETUP { };
    StructPattern = re.compile('(struct\s+(\w+)\s+{.*?});', re.S|re.M)
    EfiVarStorePattern = re.compile('efivarstore.*?;', re.S|re.M)
    Structs = StructPattern.findall(Contents)
    EfiVars = EfiVarStorePattern.findall(Contents)
    EfiVarDict = OrderedDict()
    Index = 0
    #efivarstore SYSTEM_CONFIGURATION, attribute = 0x3, name = Setup, guid =...
    for Item in EfiVars:
        if '\n' in Item:
            Item = Item.replace('\n', '')
        StructName = ''
        VarName = ''
        Attribute = ''
        for SubItem in Item.split(','):
            SubItem = SubItem.strip()
            if SubItem.startswith("efivarstore"):
                StructName = SubItem.split()[1].strip()
            elif SubItem.startswith("name"):
                VarName = SubItem.split("=")[1].strip()
            elif SubItem.startswith("attribute"):
                Attribute = SubItem.split("=")[1].strip()
        if not StructName or not VarName or not Attribute:
            print ("efivarstore format not correct\n", Item)
        #
        # EfiVarDict =
        # {'SYSTEM_CONFIGURATION':('Setup', 1, 0x3, 0x218) ...}
        # Index rule follow read Advanced.lst efivarstore
        TotalSize = re.findall("struct %s .*mTotalSize\s+\[(.*)\]" % StructName, Contents)[0]
        if StructName.strip() not in EfiVarDict and int(Attribute, 16) == 0x7:
            #print ("struct %s %d %s %s"%(StructName, Index, Attribute, TotalSize))
            EfiVarDict[StructName.strip()] = (VarName.strip(), Index, Attribute, TotalSize)
            Index += 1
    for Struct in Structs:
        # StructName = Struct[1], FullStruct = Struct[0]
        StructName = Struct[1]
        if StructName not in EfiVarDict:
            continue
        for Line in Struct[0].split('\n'):
            if Line.strip().startswith('+') and Line.strip().endswith('>'):
                Name = Line.split()[1]
                Length = Line.split()[2][1:-1]
                Offset = int('0x' + Line.split()[0].split('[')[1][:-1], 0)
                StructNameInFce = EfiVarDict[StructName][0]
                StructNameIndex = EfiVarDict[StructName][1]
                TabAttr         = EfiVarDict[StructName][2]
                TotalSize         = EfiVarDict[StructName][3]
                if Length not in UintToSize:
                    continue
                if Name.endswith(']'):
                    ArrayLen = int(Name.split('[')[1].strip(']'), 0)
                    for Index in range(ArrayLen):
                        Name = Name.split('[')[0] + '[' + str(Index) + ']'
                        if StructNameInFce not in StructsDict:
                            StructsDict[StructNameInFce] = [
                                {'TabSize':TotalSize, 'TableAttr': TabAttr, 'Index': StructNameIndex, 'StructName': StructName, 'Name': Name, 'Offset': Offset,
                                 "Size": UintToSize[Length]}]
                        else:
                            StructsDict[StructNameInFce].append(
                                {'TabSize':TotalSize, 'TableAttr': TabAttr, 'Index': StructNameIndex, 'StructName': StructName, 'Name': Name, 'Offset': Offset,
                                 "Size": UintToSize[Length]})
                        Offset += UintToSize[Length]
                else:
                    if StructNameInFce not in StructsDict:
                        StructsDict[StructNameInFce] = [
                            {'TabSize':TotalSize, 'TableAttr': TabAttr,'Index': StructNameIndex, 'StructName': StructName, 'Name': Name, 'Offset': Offset, "Size": UintToSize[Length]}]
                    else:
                        StructsDict[StructNameInFce].append(
                            {'TabSize':TotalSize, 'TableAttr': TabAttr,'Index': StructNameIndex, 'StructName': StructName, 'Name': Name, 'Offset': Offset,
                             "Size": UintToSize[Length]})
    return StructsDict

def ParserLst(InFile):
    Contents = ''
    FileList = []

    if os.path.isdir(InFile):
        Files = os.listdir(InFile)
        for OneFile in Files:
            if OneFile.endswith('.lst'):
                FileList.append (os.path.join(InFile, OneFile))
        FileList.sort(key=lambda x: os.path.basename(x))
    else:
        FileList.append (InFile)

    for OneFile in FileList:
        with open(OneFile, 'r') as InFileHandle:
            Contents += InFileHandle.read()

    Contents = ClearContents(Contents)
    AllStruct = GetAllStruct(Contents)
    return AllStruct

def GetFieldValue(Field, Name):
    if Name not in Field:
        return ''
    RetValue = ''
    for Item in Field.split('\n'):
        if Item.startswith('// %s' % Name):
            RetValue = Item.split('=')[1].strip()
            try:
                RetValue = int(RetValue, 16)
            except:
                RetValue = RetValue
            break
    return RetValue

def GetFieldInfo(Field, Name):
    if Name not in Field:
        return ''
    RetValue = ''
    for Item in Field.split('\n'):
        if Item.startswith('// %s' % Name):
            RetValue = Item.split('=')[1].strip()
            break
    return RetValue

def GetFieldDefaultValue(Field, Type):
    NameString = ''
    RetValue = ''
    for Item in Field.split('\n'):
        if Item.startswith('Q'):
            RetValue = Item.split('//')[0].split(Type)[1].strip()
            try:
                RetValue = int(RetValue, 16)
            except:
                RetValue = RetValue.split()[1:]
            NameString = Item.split('//')[1].strip()
            break
    return NameString, RetValue

def GetFieldOptionsDescription(Field):
    RetOrderDict = OrderedDict()
    IndexSize = Field.index('// size')
    OptionList = Field[:IndexSize].split('\n')[1:-1]
    for Option in OptionList:
        if not Option:
            break
        if not Option.strip().startswith('//'):
            continue
        OptionName, OptionValue = Option[3:].split('=', 1)
        RetOrderDict[OptionName.strip()] = OptionValue.strip()
    return RetOrderDict

def FormatPrintBuffer(Buffer):
    FirstStrSize = 0
    SecondStrSize = 0
    ThirdStrSize = 0
    for Line in Buffer.split('\n'):
        if len(Line.split()) < 3:
            continue
        FirstSize = len(Line.split()[0])
        if FirstSize > FirstStrSize:
            FirstStrSize = FirstSize
        SecondSize = len(Line[FirstSize:].split(';')[0].strip()) + 1
        if SecondSize > SecondStrSize:
            SecondStrSize = SecondSize
        ThirdSize = len(Line.split(';')[1].strip())
        if ThirdSize > ThirdStrSize:
            ThirdStrSize = ThirdSize
    NewBuffer = StringIO()
    for Line in Buffer.split('\n'):
        if len(Line.split()) < 1:
            NewBuffer.write('\n')
            continue
        if len(Line.split()) == 1:
            NewBuffer.write(Line)
            NewBuffer.write('\n')
            continue
        Format = '{0:<%d}  {1:<%d}  {2}' % (FirstStrSize, SecondStrSize)
        FirstString = Line.split()[0]
        SecondString = Line[len(FirstString):].split(';')[0].strip() + ';'
        ThirdString = Line.split(';')[1].strip()
        FormatStr = Format.format(FirstString, SecondString, ThirdString)
        NewBuffer.write(FormatStr)
        NewBuffer.write('\n')
    return NewBuffer

'''
BIOS:2.0.ID.AL.
BYOCFG_STRUCT_START
NAME:Setup
GUID:ec87d643-eba4-4bb5-a1e5-3f3e36b20da9
OFFSET=0x9c   UINT8   "Spare Interrupt"=1;     (0:Disable 1:SMI 2:Error Pin 4:CMCI)
OFFSET=0x13c  UINT8   "SR-IOV Support"=1;     (0:Disable 1:Enable)
OFFSET=0xe5   UINT8   "System Debug Mode"=0;     (0:Disable 3:Enable)
BYOCFG_STRUCT_END
'''
def WriteCfg(DataList, OutputSetupCfg):
    ReservedIndex = 0
    Buffer = StringIO()
    Buffer.write(BiosId)
    for Key in DataList:
        Name, Guid = Key
        Buffer.write(ByoCfgStructStart)
        Buffer.write("NAME:%s\n" % Name)
        Buffer.write("GUID:%s\n" % Guid)
        for Item in sorted(DataList[Key], key=lambda x:x[3]):
            NameString, DefaultValue, Size, Offset, OptionsDescrption = Item
            SizeType = SizeTypeDict[Size]
            OptionString = ''
            for Option in OptionsDescrption:
                try:
                    OptionNum = int(Option, 16)
                    OptionStr = OptionsDescrption[Option]
                    OptionString += '%d:%s ' % (OptionNum, OptionStr)
                except:
                    Minimum = OptionsDescrption['Minimum']
                    Maximum = OptionsDescrption['Maximum']
                    OptionString += '%d - %d' % (int(Minimum, 16), int(Maximum, 16))
                    break

            Buffer.write(f'OFFSET=0x{Offset:0x}  {SizeType} "{NameString}"={DefaultValue};      ({OptionString.strip()})')
            Buffer.write('\n')
        Buffer.write(ByoCfgStructEnd)
        Buffer.write('\n\n')
    Buffer = FormatPrintBuffer(Buffer.getvalue())
    with open(OutputSetupCfg, 'w') as WriteFile:
        WriteFile.write(Buffer.getvalue())

'''
Q 0012 0041 0074 0074 0065 006D 0070 0074 0020 0054 0050 004D 0020 0044 0065 0076 0069 0063 0065 ONE_OF 01 // Attempt TPM Device
// 01 = TPM 1.2
// 02 = TPM 2.0
// size = 0x1
// offset = 0x0
// name = TCG2_CONFIGURATION
// guid = 6339d487-26ba-424b-9a5d-687e25d740bc
// attribute = 0x7
// help = Attempt TPM Device: TPM1.2, or TPM2.0
'''
def GenerateCfgAndRaw(StructList, Buffer, OutputCfg, OutputBinary, OutputStrBinary, OutputOptionBinary, OutputSetupItemNameBinary, OutputOptionStrBinary):
    Fields = Buffer.getvalue().split('\n\n')
    DataDict = {}
    for Field in Fields:
        CheckType = False
        for Type in VariableType:
            if Type in Field:
                CheckType = True
                break
        if not CheckType:
            continue
        NameString, DefaultValue = GetFieldDefaultValue(Field, Type)
        Name = GetFieldValue(Field, 'name')
        Guid = GetFieldValue(Field, 'guid')
        Size = GetFieldValue(Field, 'size')
        Attribute = GetFieldValue(Field, 'attribute')
        Offset = GetFieldValue(Field, 'offset')
        OptionsDescrption = GetFieldOptionsDescription(Field)
        if not Attribute == 0x7:
            continue
        Duplicated = False
        if (Name, Guid) in DataDict:
            for Item in DataDict[(Name, Guid)]:
                if Offset == Item[3]:
                    Duplicated = True
                    break
        if Duplicated:
            #print ("Found the %s have the same offset: %d" %(NameString, Offset))
            continue
        if Name not in StructList:
            continue
        if (Name, Guid) not in DataDict:
            DataDict[(Name, Guid)] = [(NameString, DefaultValue, Size, Offset, OptionsDescrption, OpCodeDict[Type])]
        else:
            DataDict[(Name, Guid)].append((NameString, DefaultValue, Size, Offset, OptionsDescrption, OpCodeDict[Type]))

    GenerateRaw(StructList, DataDict, OutputBinary, OutputStrBinary, OutputOptionBinary, OutputSetupItemNameBinary, OutputOptionStrBinary)
    #Sorted DataDict with Offset, calculate Offset and size, if not in DataDict, fill Reserverd
    if OutputCfg:
        WriteCfg(DataDict, OutputCfg)

def SplitFceReadOutWithName(InFile, Buffer):
    FieldDict = {}
    with open(InFile, 'r', encoding = 'utf-8', errors='ignore') as Inhandle:
        Fields = Inhandle.read().split('\n\n')
        for Field in Fields:
            Field = Field.strip()
            if not Field.startswith('Q'):
                continue
            Name = GetFieldInfo(Field, 'name')
            Guid = GetFieldInfo(Field, 'guid')
            if (Name, Guid) not in FieldDict:
                FieldDict[(Name, Guid)] = [Field]
            else:
                FieldDict[(Name, Guid)].append(Field)

    for Key in FieldDict:
        for Field in FieldDict[Key]:
            Buffer.write(Field)
            Buffer.write('\n\n')

#
# Add all string ascii
# return UINT32
#
def GenerateHash(NameString):
    Hash = 0
    for Char in NameString:
        Hash = Hash*131 + ord(Char)
    return Hash & 0xffffffff

def GetStructNameFromOffset(StructInfo, Offset):
    for Item in StructInfo:
        try:
            if Offset == Item['Offset']:
                return Item['Name']
        except:
            print (Item)
    else:
        return ''
#
# OutpuBinary struct
#  UINT32 SetupVariableNumber;
#  UINT32 SetupItemNumber;
#  SETUP_VARIABLE_TABLE VariableTable[];
#  SETUP_ITEM_INFO [];
#
# STATIC SETUP_ITEM_INFO mVariableTable[] = {
#   { GUID, VariableLen, Attribute, VariableName}
#   { GUID, VariableLen, Attribute, VariableName}
# }
# STATIC SETUP_ITEM_INFO mSetupItem[] = {
#    //{PATCH_CFG_RECORD_SIGNATURE, 0,0,0},
#    { 0x53524350,0,0,0},
#    {Index, ItemOffset, ItemSize, ItemNameHash}
#    // {MAX_UINT32, MAX_UINT16, MAX_UINT16, MAX_UINT32},//last member
#    { 0xFFFFFFFF, 0xFFFF, 0xFFFF,0xFFFFFFFF},
# }
#
#OutputOptionBinary format

#// oneof may have one or more option
##define EFI_IFR_ONE_OF_OP              0x05
#// checkbox has two option, 0 or 1
##define EFI_IFR_CHECKBOX_OP            0x06
#// numeric is value region: min ~ max
#define EFI_IFR_NUMERIC_OP             0x07

#typedef struct {
#  UINT8                    OpCode;
#  UINT8                    Count:5;
#  UINT8                    DataType:3; // 1, 2, 4, 8
#  union {
#    UINT8   U8;
#    UINT16  U16;
#    UINT32  U32;
#    UINT64  U64;
#  } Value [Count + 1]; // Value [0] is the default value; Value [1..Count] is valid value.
#};

def GenerateRaw(StructList, DataDict, OutputBinary, OutputStrBinary, OutputOptionBinary, OutputSetupItemNameBinary, OutputOptionStrBinary):
    OutputBuffer = BytesIO()
    VariableTable = BytesIO()
    NameStringBuffer = BytesIO()
    OrderListNameStringBuffer = BytesIO()
    OptionsBuffer = BytesIO()
    OrderListOptionsBuffer = BytesIO()
    OptionsStrBuffer = BytesIO()
    OrderListOptionsStrBuffer = BytesIO()
    SetupNameStringBuffer = BytesIO()
    OrderListSetupNameStringBuffer = BytesIO()
    OrderListOutputBuffer = BytesIO()
    #Start Data
    OutputBuffer.write(struct.pack('I', 0x53524350))
    OutputBuffer.write(struct.pack('H', 0x0))
    OutputBuffer.write(struct.pack('H', 0x0))
    OutputBuffer.write(struct.pack('I', 0x0))
    NewDataDict = {}
    for Item in DataDict:
        VarName, Guid = Item
        Index = StructList[VarName][0]["Index"]
        NewDataDict[Index, VarName, Guid] = DataDict[Item]
    TotalItems = 0
    VariableIndex = 0
    NameStringList = ['BootOrder','UefiBootGroupOrder','LegacyBootGroupOrder','PlatformLang','PlatformLangCodes']
    for Item in sorted(NewDataDict):
        Index, VarName, Guid = Item
        #print ("Struct: %s, Index: %s, efivarname: %s, size：%s, Attribute=%s" % (StructList[VarName][0]['StructName'],  StructList[VarName][0]['Index'],
        #                                                          VarName, StructList[VarName][0]['TabSize'], StructList[VarName][0]['TableAttr']))
        #write Variable table
        #typedef struct {
        #    EFI_GUID   VendorGuid;
        #    UINT32     Size;
        #UINT32     Attribute;
        #CHAR16     Name[MAX_EFI_VARIABLE_NAME_LENGTH];
        #} SETUP_VARIABLE_TABLE;
        Bytes = uuid.UUID(Guid).bytes_le
        VariableTable.write (Bytes)

        VariableTable.write(struct.pack('I', int(StructList[VarName][0]['TabSize'],16)))
        VariableTable.write(struct.pack('I', int(StructList[VarName][0]['TableAttr'], 16)))
        StrLen = len(VarName)
        if StrLen > MAX_EFI_VARIABLE_NAME_LENGTH:
            print ("Warning Varible item name is too long")
        for Char in VarName:
            VariableTable.write(struct.pack('H', ord(Char)))
        for Index in range(MAX_EFI_VARIABLE_NAME_LENGTH - StrLen):
            VariableTable.write(struct.pack('H', 0x0))

        VariableDataList = NewDataDict[Item]
        for NameString, DefaultValue, Size, Offset, OptionsDescrption, Type in sorted(VariableDataList, key=lambda x: x[3]):
            #Raw Data
            ItemOffset = Offset
            ItemSize = Size
            #print ("VarName:", VarName)
            #print ("Offset:", Offset)
            #print ("NameString:", NameString)
            StructItemName = GetStructNameFromOffset(StructList[VarName], Offset)
            if not StructItemName:
                print ("Not found Offset: 0x%x, Name:%s in %s" % (Offset, NameString, VarName))
                continue
            if StructItemName not in NameStringList:
                NameStringList.append(StructItemName)
            else:
                while True:
                    StructItemName = StructItemName + '_'
                    if StructItemName not in NameStringList:
                        NameStringList.append(StructItemName)
                        break
            if OutputStrBinary:
                if Type == ORDERED_LIST:
                    StructItemName = StructItemName[:-3]
                    OrderListNameStringBuffer.write(StructItemName.encode('utf-8'))
                    OrderListNameStringBuffer.write(struct.pack('B', 0x0))
                else:
                    NameStringBuffer.write(StructItemName.encode('utf-8'))
                    NameStringBuffer.write(struct.pack('B', 0x0))
            if OutputSetupItemNameBinary:
                if Type == ORDERED_LIST:
                    OrderListSetupNameStringBuffer.write(NameString.encode('utf-8'))
                    OrderListSetupNameStringBuffer.write(struct.pack('B', 0x0))
                else:
                    SetupNameStringBuffer.write(NameString.encode('utf-8'))
                    SetupNameStringBuffer.write(struct.pack('B', 0x0))
            if OutputOptionBinary:
                if Type == ORDERED_LIST:
                    OrderListOptionsBuffer.write(struct.pack('B', Type))
                    Count = len(OptionsDescrption) + 1
                    DataType = SizeToCode[round(len(DefaultValue) / Size)]
                    OrderListOptionsBuffer.write(struct.pack('B', Count))
                    OrderListOptionsBuffer.write(struct.pack('B', DataType))
                    OrderListOptionsBuffer.write(struct.pack(SizeToPack[round(len(DefaultValue) / Size)], 0))
                    for Option in OptionsDescrption:
                        Option = Option.lstrip('0')
                        if not Option:
                            Option = '0'
                        OrderListOptionsBuffer.write(struct.pack(SizeToPack[round(len(DefaultValue) / Size)], int(Option, 16)))
                else:
                    OptionsBuffer.write(struct.pack('B', Type))
                    Count = len(OptionsDescrption) + 1

                    DataType = SizeToCode[ItemSize]
                    OptionsBuffer.write(struct.pack('B', Count))
                    OptionsBuffer.write(struct.pack('B', DataType))
                    if Type == NUMERIC:
                        Minimum = OptionsDescrption['Minimum']
                        Maximum = OptionsDescrption['Maximum']
                        Step    = OptionsDescrption['Step']
                        OptionsBuffer.write(struct.pack(SizeToPack[ItemSize], DefaultValue))
                        OptionsBuffer.write(struct.pack(SizeToPack[ItemSize], int(Minimum, 16)))
                        OptionsBuffer.write(struct.pack(SizeToPack[ItemSize], int(Maximum, 16)))
                        OptionsBuffer.write(struct.pack(SizeToPack[ItemSize], int(Step, 16)))
                    else:
                        OptionsBuffer.write(struct.pack(SizeToPack[ItemSize], DefaultValue))
                        for Option in OptionsDescrption:
                            Option = Option.lstrip('0')
                            if not Option:
                                Option = '0'
                            OptionsBuffer.write(struct.pack(SizeToPack[ItemSize], int(Option, 16)))
            if OutputOptionStrBinary:
                if Type == ONE_OF:
                    for Name in OptionsDescrption:
                        OptionsStrBuffer.write(OptionsDescrption[Name].encode('utf-8'))
                        OptionsStrBuffer.write(struct.pack('B', 0x0))
                if Type == ORDERED_LIST:
                    for Name in OptionsDescrption:
                        OrderListOptionsStrBuffer.write(OptionsDescrption[Name].encode('utf-8'))
                        OrderListOptionsStrBuffer.write(struct.pack('B', 0x0))
            ItemNameHash = GenerateHash(StructItemName)
            if Type == ORDERED_LIST:
                OrderListOutputBuffer.write(struct.pack('I', VariableIndex))
                OrderListOutputBuffer.write(struct.pack('H', ItemOffset))
                OrderListOutputBuffer.write(struct.pack('H', ItemSize))
                OrderListOutputBuffer.write(struct.pack('I', ItemNameHash))
            else:
                OutputBuffer.write(struct.pack('I', VariableIndex))
                OutputBuffer.write(struct.pack('H', ItemOffset))
                OutputBuffer.write(struct.pack('H', ItemSize))
                OutputBuffer.write(struct.pack('I', ItemNameHash))
            TotalItems += 1
        #
        # Set Variable Index to next one
        #
        VariableIndex += 1
    OutputBuffer.write(OrderListOutputBuffer.getvalue())

    #End Data
    OutputBuffer.write(struct.pack('I', 0xFFFFFFFF))
    OutputBuffer.write(struct.pack('H', 0xFFFF))
    OutputBuffer.write(struct.pack('H', 0xFFFF))
    OutputBuffer.write(struct.pack('I', 0xFFFFFFFF))
    #print("TotalItem: %d" % TotalItems)
    with open(OutputBinary, 'wb') as OutFile:
        OutFile.write(struct.pack('I', VariableIndex))
        OutFile.write(struct.pack('I', TotalItems))
        OutFile.write(VariableTable.getvalue())
        OutFile.write(OutputBuffer.getvalue())
    if OutputStrBinary:
        with open(OutputStrBinary, 'wb') as OutFile:
            OutFile.write(NameStringBuffer.getvalue())
            OutFile.write(OrderListNameStringBuffer.getvalue())
    if OutputOptionBinary:
        with open(OutputOptionBinary, 'wb') as OutFile:
            OutFile.write(OptionsBuffer.getvalue())
            OutFile.write(OrderListOptionsBuffer.getvalue())
    if OutputSetupItemNameBinary:
        with open(OutputSetupItemNameBinary, 'wb') as OutFile:
            OutFile.write(SetupNameStringBuffer.getvalue())
            OutFile.write(OrderListSetupNameStringBuffer.getvalue())
    if OutputOptionStrBinary:
        with open(OutputOptionStrBinary, 'wb') as OutFile:
            OutFile.write(OptionsStrBuffer.getvalue())
            OutFile.write(OrderListOptionsStrBuffer.getvalue())

'''
 FCE read  -i Image.fd > Fce_Read.log
 Advanced.lst  from vfrcompile -l parameter
'''
if __name__ == "__main__":
    Argparser = argparse.ArgumentParser(prog=__prog__,
                                     description=__description__ + __copyright__,
                                     conflict_handler='resolve')
    Argparser.add_argument('-v', '--version', action='version', version=__version__,
                        help="show program's version number and exit")
    Argparser.add_argument('-i', '--input', metavar='Fce_Read.out', required=True, dest='Config', help="Input file from FCE read")
    Argparser.add_argument('-l', '--list', metavar='Advanced.lst', required=True, dest='List', help="Input file from vfrcompile generation")
    Argparser.add_argument('-o', '--output', metavar='SetupItemsDB.raw',required=True, dest='Output', help="output file")
    Argparser.add_argument('-on', '--outputname', metavar='SetupFieldDB.raw', required=False, dest='OutputName', help="output the binary file with struct items' name")
    Argparser.add_argument('-oo', '--outputoption', metavar='SetupItemsOption.raw', required=False, dest='OutputOption', help="output the binary file with struct items' options information")
    Argparser.add_argument('-oos', '--outputoptionstring', metavar='SetupItemsOptionStr.raw', required=False, dest='OutputOptionString', help="output the binary file with struct items' options string information")
    Argparser.add_argument('-os', '--setupname', metavar='SetupItemName.raw', required=False, dest='SetupItemName', help="output the binary file with struct items related setup name")
    Options = Argparser.parse_args()
    if len(sys.argv) < 4:
        print ('Error command, no output path, use -h for help')
        exit(1)

    FceReadFile = Options.Config
    LstFile = Options.List
    OutputBinary = Options.Output
    Buffer = StringIO()
    OutputCfg    = ""
    OutputStrBinary = Options.OutputName
    SplitFceReadOutWithName(FceReadFile, Buffer)
    StructList = ParserLst(LstFile)
    OutputOptionBinary = Options.OutputOption
    OutputOptionStrBinary = Options.OutputOptionString
    OutputSetupItemNameBinary = Options.SetupItemName
    GenerateCfgAndRaw(StructList, Buffer, OutputCfg, OutputBinary, OutputStrBinary, OutputOptionBinary, OutputSetupItemNameBinary, OutputOptionStrBinary)
    