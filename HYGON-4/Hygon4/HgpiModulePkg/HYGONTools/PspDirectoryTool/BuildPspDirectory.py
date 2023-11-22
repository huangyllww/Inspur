 # *****************************************************************************
 # *
 # * 
 # * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 # * 
 # * HYGON is granting you permission to use this software (the Materials)
 # * pursuant to the terms and conditions of your Software License Agreement
 # * with HYGON.  This header does *NOT* give you permission to use the Materials
 # * or any rights under HYGON's intellectual property.  Your use of any portion
 # * of these Materials shall constitute your acceptance of those terms and
 # * conditions.  If you do not agree to the terms and conditions of the Software
 # * License Agreement, please do not use any portion of these Materials.
 # * 
 # * CONFIDENTIALITY:  The Materials and all other information, identified as
 # * confidential and provided to you by HYGON shall be kept confidential in
 # * accordance with the terms and conditions of the Software License Agreement.
 # * 
 # * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 # * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 # * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 # * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 # * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 # * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 # * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 # * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 # * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 # * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 # * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 # * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 # * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 # * 
 # * HYGON does not assume any responsibility for any errors which may appear in
 # * the Materials or any other related information provided to you by HYGON, or
 # * result from use of the Materials or any related information.
 # * 
 # * You agree that you will not reverse engineer or decompile the Materials.
 # * 
 # * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 # * further information, software, technical information, know-how, or show-how
 # * available to you.  Additionally, HYGON retains the right to modify the
 # * Materials at any time, without notice, and is not obligated to provide such
 # * modified Materials to you.
 # * 
 # * 
 # ******************************************************************************
import os
import sys
#Module Data Packing
import struct
#Module for arguments parsing
import argparse
#Module for logging, 
import logging
from pprint import pprint
#Module for XML parsing
import xml.etree.ElementTree as ET
from xml.etree.ElementTree import Element, SubElement, Comment, tostring
from xml.dom import minidom
import types
import pdb
import traceback
# pdb.set_trace()

Version = "0.0.01"
#Version 0.0.01  02/18/2023
  # * Add SAT support

#Static Value
ROMSIG_SIG_OFFSET = 0
ROMSIG_PSPDIR_OFFSET = 5
ROMSIG_BIOSDIR_OFFSET = 6
PSP_DIR_SIGNATURE = 0x50535024 #$PSP
BIOS_DIR_SIGNATURE = 0x44484224 #$BHD
#Global Variable
DEBUG = 0
gAlignment= 0x100
DirHeaderSize = 0x400   #Always reserve 0x200 for Dir Header 
OutputPath='/'
AddressMode = 0
ProgramStr = 'None'
PspDirectoryEntryName = {
#                         Type:   DIR, Entry Type    ShortName    Full description  Modifiable 
                          0x00: ['PSP' ,'IMAGE_ENTRY','HygonPubKey','Hygon public Key', 0],
                          0x01: ['PSP' ,'IMAGE_ENTRY','PspBootLoader','PSP Boot Loader firmware', 0],
                          0x02: ['PSP' ,'IMAGE_ENTRY','PspOs','PSP Secure OS firmware', 0],
                          0x03: ['PSP' ,'IMAGE_ENTRY','PspRecBL','PSP Recovery Boot Loader', 0],
                          0x04: ['PSP' ,'IMAGE_ENTRY','PspNv','PSP Non Volatile data', 1],
                          0x05: ['BIOS','IMAGE_ENTRY','OemPubKey','[OEM] BIOS public key signed with hygon key', 0],
                          0x06: ['BIOS','POINT_ENTRY','Rtm','BIOS RTM Volume', 0],
                          0x07: ['BIOS','IMAGE_ENTRY','RtmSignature','BIOS RTM volume Signature using OEM private key', 0],
                          0x08: ['PSP' ,'IMAGE_ENTRY','Smu','SMU offchip firmware', 0],
                          0x09: ['PSP' ,'IMAGE_ENTRY','DbgKey','Hygon Secure Debug Key', 0],
                          0x0A: ['PSP' ,'IMAGE_ENTRY','OemPspPubkey','PSP Secure OS public key signed with Hygon key', 0],
                          0x0B: ['PSP' ,'VALUE_ENTRY','Value','Secure Unlock Enable flag', 0],
                          0x0C: ['PSP' ,'IMAGE_ENTRY','PspTrustlet','PSP trustlet Binary', 0],
                          0x0D: ['PSP' ,'IMAGE_ENTRY','TrusletKey','truslet key(Signed public portion)', 0],
                          0x13: ['PSP' ,'IMAGE_ENTRY','PspEarlyUnlock','PSP early secure unlock debug image', 0],
                          0x20: ['PSP' ,'IMAGE_ENTRY','HwIpCfg', 'HW IP configuration file', 0],
                          0x21: ['PSP' ,'IMAGE_ENTRY','ikek', 'Wrapped iKEK', 0],
                          0x22: ['PSP' ,'IMAGE_ENTRY','PspTokenUnlockData', 'PSP token unlock data', 1],
                          0x23: ['PSP' ,'IMAGE_ENTRY','PspDiagBL', 'Entry to load PSP Diag BL on non-secure part via fuse', 0],
                          0x24: ['PSP' ,'IMAGE_ENTRY','SecureGasket', 'Secure Gasket', 0],
                          0x25: ['PSP' ,'IMAGE_ENTRY','Mp2Fw', 'MP2 FW', 0],
                          0x26: ['PSP' ,'IMAGE_ENTRY','Mp2Fw2', 'Reserved for MP2 FW part2', 0],
                          0x27: ['PSP' ,'IMAGE_ENTRY','UserModeUnitTest', 'User mode unit tests binary', 0],
                          0x28: ['PSP' ,'IMAGE_ENTRY','PspSystemDriver', 'PSP Entry points to system driver in SPI space', 0],
                          0x38: ['PSP' ,'IMAGE_ENTRY','VmGuardData', 'VM Guard Data', 1],
                          0x49: ['PSP' ,'IMAGE_ENTRY','CsvFirmware', 'Csv Firmware', 0],
                          0x5F: ['PSP' ,'IMAGE_ENTRY','Scs','SCS Binary', 0],
                          0x60: ['BIOS','IMAGE_ENTRY','Hpcb','Hgpi PSP Customization Block', 1],
                          0x61: ['BIOS','POINT_ENTRY','Hpob','Hgpi PSP Output Block', 0],
                          0x62: ['BIOS','POINT_ENTRY','BiosFirmware','Bios firmware', 0],
                          0x63: ['BIOS','IMAGE_ENTRY','HpobNv','HPOB NV Copy', 1],
                          0x66: ['BIOS','IMAGE_ENTRY','UCodePatch','Microcode Patch', 0],
                          0x67: ['BIOS','IMAGE_ENTRY','CoreMceData','core machine check exception data', 1],
                          0x68: ['BIOS','IMAGE_ENTRY','Hpcb','Hgpi PSP Customization Block Backup', 1],
                          0x71: ['BIOS','POINT_ENTRY','FV_MAIN', 'Point to FV_MAIN', 0],
                          0x19040: ['BIOS','IMAGE_ENTRY','UCodePatchE0','Microcode Patch', 0],
                                 }

ProgramTable = {  
  'SAT': {"PSPID": 0xBC090000, "BIOSDIR_OFFSET": 6},
}

#Set log level to debug, internal debug only
log = logging.getLogger()
FORMAT = '%(message)s'  #message ONLY
if (DEBUG):
  logging.basicConfig(format= FORMAT,stream=sys.stderr,level=logging.DEBUG)
else:
  #Set log level to debug, internal debug only
  logging.basicConfig(format= FORMAT,stream=sys.stderr,level=logging.INFO)

#Utility routines
def StrToNum (Str):
  if (Str != None):
    if type(Str) is str:
      if (Str.endswith ('L')):
        Str = Str.rstrip ('L')
      num = int(Str, 0)
      return num
    elif type(Str) is int:
      return Str
    else:
      print "unexpected Data Type ", type(Str)
      FatalErrorExit ()

def CheckFileExistance (file):
  return os.path.exists(file)

def IsImageEntry (Entry):
  if (Entry['Entry'] == 'IMAGE_ENTRY'):
    return True
  return False

def IsValueEntry (Entry):
  if (Entry['Entry'] == 'VALUE_ENTRY'):
    return True
  return False

def IsPointEntry (Entry):
  if (Entry['Entry'] == 'POINT_ENTRY'):
    return True
  return False

def IsResetImageEntry (Entry):
  if ((Entry['Entry'] == 'POINT_ENTRY') and
      (Entry['Type'] == 0x62)  and
      (Entry['ResetImage'] == 1)) :
    return True
  return False

def FatalErrorExit ():
  log.info (traceback.extract_stack())
  log.info ("ERORR Exit\n")
  sys.exit (2)

def prettify(elem):
    """Return a pretty-printed XML string for the Element.
    """
    rough_string = tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")

def GetOccupiedSize (ActualSize, Alignment):
  """Calculate occupied size considering the alignment"""
  return   (ActualSize + (((Alignment) - ((ActualSize) & ((Alignment) - 1))) & ((Alignment) - 1)))

def AlignAddress (Address, Alignment):
  """Calculate occupied size considering the alignment"""
  return   (Address + (((Alignment) - ((Address) & ((Alignment) - 1))) & ((Alignment) - 1)))


def getEntryType (DirectoryInfo, EntryInfo):
  global PspDirectoryEntryName
  if EntryInfo["Type"] in PspDirectoryEntryName:
    #Check if inside of Current DIR region if not set to "POINT_ENTRY"
    if ((EntryInfo["Address"] > DirectoryInfo["DirOffset"]) and (EntryInfo["Address"] < DirectoryInfo["DirOffset"] + DirectoryInfo["DirSize"])): 
      EntryType = PspDirectoryEntryName[EntryInfo["Type"]][1]
    else:
      EntryType = "POINT_ENTRY"
  else:
    EntryType = "UNKNOWN_ENTRY"
  return EntryType

def getEntryShortName (Type):
  global PspDirectoryEntryName
  if Type in PspDirectoryEntryName:
    return PspDirectoryEntryName[Type][2]
  else:
    return "unknown"

def getEntryName (Type):
  global PspDirectoryEntryName
  if Type in PspDirectoryEntryName:
    return PspDirectoryEntryName[Type][3]
  else:
    return "Unknown Type"

def IsEntryModifiable (Type):
  global PspDirectoryEntryName
  if Type in PspDirectoryEntryName:
    return PspDirectoryEntryName[Type][4]
  else:
    return 0

def GetOutEntryFileBaseName (entry, DirStr, Level):
  if (IsImageEntry (entry)):
    if ((entry['Type'] == 7) and (Level == 1)):
    #Use constant name for RTM signature
      return "RTMSignature"
    elif ((entry['Type'] == 7) and (Level == 2)):
    #Use constant name for RTM signature
      return "RTMSignatureL1L2"
    else:
      return "%sL%x_Typex%x_%s" %(DirStr, Level, entry['Type'], getEntryShortName(entry['Type']))
  elif (IsResetImageEntry (entry)):
      return "ResetImage"
  else:
    return "Unsupported"

def GetOutEntryFileName (entry, DirStr, Level):
  if (IsImageEntry (entry)):
    if ((entry['Type'] == 7) and (Level == 1)):
    #Use constant name for RTM signature
      return "RTMSignature.bin"
    elif ((entry['Type'] == 7) and (Level == 2)):
    #Use constant name for RTM signature
      return "RTMSignatureL1L2.bin"
    else:
      return "%sL%x_Typex%x_%s.bin" %(DirStr, Level, entry['Type'], getEntryShortName(entry['Type']))
  elif (IsResetImageEntry (entry)):
      return "ResetImage.bin"
  else:
    return "Unsupported"

def Fletcher32Crc (data, words):
  sum1 = 0xffff
  sum2 = 0xffff
  tlen = 0
  i = 0

  while (words):
    tlen = 359
    if (words < 359):
      tlen = words
    words -=tlen

    while tlen:
      s = struct.Struct('H')
      val = s.unpack(data [i: i+2])[0]
      #print "%x"%(val),
      i +=2
      sum1 += val
      sum2 += sum1
      tlen -=1

    sum1 = (sum1 & 0xffff) + (sum1 >> 16)
    sum2 = (sum2 & 0xffff) + (sum2 >> 16)

  sum1 = (sum1 & 0xffff) + (sum1 >> 16)
  sum2 = (sum2 & 0xffff) + (sum2 >> 16)
  return (sum2 << 16 | sum1)

def WriteBinaryFile (File, buffer):
  global OutputPath
  try:
    FileHandle = open (OutputPath + File,'wb')
  except:
    print "Error: Opening ", OutputPath + File
    FatalErrorExit ()
  FileHandle.write(buffer)
  FileHandle.close()


def ReadBinaryFile (File):

  if (os.path.exists(File) == False):
    print "\nError :", File, " does not exist"
    return (None)

  try:
    FileHandle = open (File,'rb')
  except:
    print "Error: Opening ", File
    FatalErrorExit ()
  lines = FileHandle.read()
  FileHandle.close()
  return lines

def getOffsetInBin (romsize, offset):
  '''
    romsize equal zero indicate it is a relative address, no need to do the translation
  '''
  if (romsize):
    #Use 0xFFFFFFFF + 1 instead of 0x100000000 to avoid long number which will leading "L" trailing when print out
    return romsize-(0xFFFFFFFF - offset + 1)
  else:
    return offset

def getMmioAddress (romsize, offset):
  #Use 0xFFFFFFFF + 1 instead of 0x100000000 to avoid long number which will leading "L" trailing when print out
  return 0xFFFFFFFF - romsize + offset + 1

def PatchBinary (OrgBuffer, ModBuffer, Offset, Message):
  log.info ("<%s> content copied between [0x%04x ~ 0x%04x]" , Message, Offset, Offset + len(ModBuffer) -1)
  if (Offset + len (ModBuffer) > len (OrgBuffer)):
    log.info ("ERROR: Size error in PatchBinary")
    FatalErrorExit ()
  buffer = OrgBuffer[0:Offset] + ModBuffer + OrgBuffer[Offset+ len(ModBuffer):]
  return buffer

def StringlizeDict (Dict):
  for (k,v) in Dict.iteritems():
    if (type(v) == types.IntType or type(v) == types.LongType):
      Dict[k]=hex(v)
  return Dict

#Functional routines
def ParseArg ():
  global Version
  global ProgramStr
  """Parse Input arguments, and return the parsed result"""
  Parser = argparse.ArgumentParser (description='Tool used to Build PSP DirTable & Embed/Dump PSP entries')
  Parser.add_argument ('--version', action='version', version='BuildPspDirectory %s' %(Version))
  Parser.add_argument ('-o','--outputpath',default="Output")
  SubParser = Parser.add_subparsers(dest='subcommand',help="Type '<subcommand> -h' for help on a specific subcommand")
  #Build Directory table
  BDParser = SubParser.add_parser ('bd',help='Build Directory table header only')
  BDParser.add_argument ("InBiosImage", help="Specify the INPUT BIOS image file name")
  BDParser.add_argument ("CfgFile", help="Specify the configure file for build the PSP Directory")
  #Build Bios image with psp entry file embedded
  BBParser = SubParser.add_parser ('bb',help='Build Bios image with psp entry file embedded')
  BBParser.add_argument ("InBiosImage", help="Specify the INPUT BIOS image file name")
  BBParser.add_argument ("CfgFile", help="Specify the config file for build the PSP Directory")
  BBParser.add_argument ("OutBiosImage", help="Specify the OUTPUT BIOS image file name")
  #DumP psp entry of given BIOS image
  DPParser = SubParser.add_parser ('dp',help='DumP psp entry of given BIOS image')
  DPParser.add_argument ("InBiosImage", help="Specify the INPUT BIOS image file name")
  DPParser.add_argument ("-p", "--program", help="Specify the program name, Valid choices: SAT", required=True)
  DPParser.add_argument ("-x","--xml", help="Output the information in XML format to PspDirInfo.xml",action="store_true")
  DPParser.add_argument ("-b","--binary", help="Output  psp binaries to outputpath",action="store_true")
  DPParser.add_argument ("-d","--directory", help="Output PspDirectory.cfg to outputpath",action="store_true")

  args = Parser.parse_args ()
  #Parameter check
  if (args.subcommand == 'bd' or args.subcommand == 'bb'):
    if (CheckFileExistance(args.CfgFile) == False):
      log.info ("[Error] Can't Open CfgFile '%s'", args.CfgFile)
      FatalErrorExit ()
      #To be Done: Check PSP Directory Format
      #if (CheckCfgFileFormat(args.CfgFile) == False):
        # print "[Error] CfgFile '%s' doesn't exists" %args.CfgFile
        # FatalErrorExit ()

  if (args.subcommand == 'dp' or args.subcommand == 'bd' or args.subcommand == 'bb'):
    if (CheckFileExistance(args.InBiosImage) == False):
      log.info ("[Error] Can't Open BiosImage '%s'", args.InBiosImage)
      FatalErrorExit ()

  if (args.subcommand == 'dp'):
    if args.program.upper() not in ["SAT"]:
      log.info ("[Error] Not a valid program string, only SAT are supported")
      FatalErrorExit ()
    ProgramStr = args.program.upper ()
  return args

def ValidatePspDirAttrib (PspDirAttrib):
  # The valid attributes for PSP Dir tag can be like below:
  # *Note* the attributes are case sensitive
  # 1) None       |Used when only generate the directory header with "bd" command
  # 2) Base       |Used for fix address format, which all address need explicit defined, include where to embed the Directory Header.
  # 3) Base, Size |Used for dynamic address format, user reserve a region for hold all PSP entries include the header
  if (((PspDirAttrib['Base'] == None) and  (PspDirAttrib['Size'] != None))):
    return False

  if (PspDirAttrib['Size'] != None):
    if ((StrToNum(PspDirAttrib['Size']) % 0x1000) != 0):
      log.info ("ERROR: Dir Size must be 0x1000 aligned")
      return False
    if (StrToNum(PspDirAttrib['Size']) > 0x400000):
      log.info ("ERROR: Exceed Max Dir Size 0x400000")
      return False

  if (PspDirAttrib['SpiBlockSize'] != None):
    if ((StrToNum(PspDirAttrib['SpiBlockSize']) % 0x1000) != 0):
      log.info ("ERROR: SpiBlockSize must be 0x1000 aligned")
      return False
    if (StrToNum(PspDirAttrib['SpiBlockSize']) > 0x10000):
      log.info ("ERROR: Exceed Max SpiBlockSize 0x10000")
      return False
  return True

def ValidateBiosDirAttrib (BiosDirAttrib):
  # The valid attributes for PSP Dir tag can be like below:
  # *Note* the attributes are case sensitive
  # 1) None       |Used when only generate the directory header with "bd" command
  # 2) Address    |Used for fix address format, which all address need explicit defined, include where to embed the Directory Header.
  # 3) Base, Size |Used for dynamic address format, user reserve a region for hold all PSP entries include the header
  if (((BiosDirAttrib['Base'] == None) and  (BiosDirAttrib['Size'] != None))):
    return False

  if (BiosDirAttrib['Size'] != None):
    if ((StrToNum(BiosDirAttrib['Size']) % 0x1000) != 0):
      log.info ("ERROR: Dir Size must be 0x1000 aligned")
      return False
    if (StrToNum(BiosDirAttrib['Size']) > 0x400000):
      log.info ("ERROR: Exceed Max Dir Size 0x400000")
      return False

  if (BiosDirAttrib['SpiBlockSize'] != None):
    if ((StrToNum(BiosDirAttrib['SpiBlockSize']) % 0x1000) != 0):
      log.info ("ERROR: SpiBlockSize must be 0x1000 aligned")
      return False
    if (StrToNum(BiosDirAttrib['SpiBlockSize']) > 0x10000):
      log.info ("ERROR: Exceed Max SpiBlockSize 0x10000")
      return False
  return True


def ValidatePspEntry (PspEntry):
  #Below items will be checked
  #entry: should be one of three: IMAGE_ENTRY;VALUE_ENTRY;POINT_ENTRY
  if (not (IsImageEntry (PspEntry) or IsValueEntry(PspEntry) or IsPointEntry (PspEntry))):
    return False
  #Type should be explicit defined
  if (PspEntry['Type'] == None):
    log.info ("ERROR: Type haven't been explicit defined")
    return False
  #"File" need be defined for Image entry:
  if (IsImageEntry (PspEntry)):
    if (PspEntry['File'] == None):
      log.info ("ERROR: File haven't been explicit defined for IMAGE_ENTRY")
      return False
  #"Value" need be defined for Value entry
  if (IsValueEntry (PspEntry)):
    if (PspEntry['Value'] == None):
      log.info ("ERROR: Value haven't been explicit defined for VALUE_ENTRY")
      return False
  #"Address" "Size" need be defined for Point entry
  if (IsPointEntry(PspEntry)):
    if ((PspEntry['Address'] == None) or (PspEntry['Size'] == None)):
      log.info ("ERROR: Address or Size haven't been explicit defined for VALUE_ENTRY")
      return False
  return True

def ValidateBiosEntry (BiosEntry):
  #Below items will be checked
  #entry: should be one of three: IMAGE_ENTRY;VALUE_ENTRY;POINT_ENTRY
  if (not (IsImageEntry (BiosEntry) or IsValueEntry(BiosEntry) or IsPointEntry (BiosEntry))):
    return False
  #Type should be explicit defined
  if (BiosEntry['Type'] == None):
    log.info ("ERROR: Type haven't been explicit defined")
    return False
  #"File" need be defined for Image entry:
  if (IsImageEntry (BiosEntry)):
    if (BiosEntry['File'] == None):
      return False
  #"Value" need be defined for Value entry
  if (IsValueEntry (BiosEntry)):
    if (BiosEntry['Value'] == None):
      return False
  #"Address" "Size" need be defined for Point entry
  if (IsPointEntry(BiosEntry)):
    if ((BiosEntry['Address'] == None) or (BiosEntry['Size'] == None)):
      return False
  if (BiosEntry['Type'] > 0xFF):
      log.info ("ERROR: Type exceed limit 0xFF")
      return False

  if (BiosEntry['RegionType'] > 0xFF):
      log.info ("ERROR: RegionType exceed limit 0xFF")
      return False

  if (BiosEntry['ResetImage'] > 0x1):
      log.info ("ERROR: BiosResetImage exceed limit 0x1")
      return False

  if (BiosEntry['Copy'] > 0x1):
      log.info ("ERROR: Copy exceed limit 0x1")
      return False

  if (BiosEntry['ReadOnly'] > 0x1):
      log.info ("ERROR: ReadOnly exceed limit 0x1")
      return False

  if (BiosEntry['Compressed'] > 0x1):
      log.info ("ERROR: Compressed exceed limit 0x1")
      return False

  if (BiosEntry['Instance'] > 0xF):
      log.info ("ERROR: Instance exceed limit 0xF")
      return False
  return True

def BuildPspEntries (PspDir):
  PspEntries = []
  for entry in PspDir:
    log.debug ("Psp Entry %s->%s",entry.tag, entry.attrib)
    PspEntry = {}
    # Initial default value
    PspEntry['Type'] = None
    PspEntry['File'] = None
    PspEntry['Address'] = None
    PspEntry['Size'] = None
    PspEntry['Entry'] = None
    PspEntry['Value'] = None
    PspEntry['Recovery'] = 0
    #Update the attribute get from XML
    PspEntry.update (entry.attrib)
    PspEntry['Entry'] = entry.tag
    #Transfer all numeric field
    PspEntry['Type'] = StrToNum(PspEntry['Type'] )
    PspEntry['Address'] = StrToNum(PspEntry['Address'])
    PspEntry['Size'] = StrToNum(PspEntry['Size'])
    PspEntry['Value'] = StrToNum(PspEntry['Value'])
    PspEntry['Recovery'] = StrToNum(PspEntry['Recovery'])
    log.debug ("PspEntry %s", PspEntry)
    if (ValidatePspEntry (PspEntry) == False):
      log.info ("ERROR: Unrecognized Attribute/Tag found in %s->%s", entry.tag, entry.attrib)
      FatalErrorExit ()
    PspEntries.append(PspEntry)
  return PspEntries


def BuildBiosEntries (BiosDir):
  BiosEntries = []
  for entry in BiosDir:
    BiosEntry = {}
    log.debug ("%s->%s",entry.tag, entry.attrib)
    # Initial default value
    BiosEntry['Type'] = None
    BiosEntry['File'] = None
    BiosEntry['Address'] = None
    BiosEntry['Size'] = None
    BiosEntry['Entry'] = None
    BiosEntry['Destination'] = '0xFFFFFFFFFFFFFFFF'
    BiosEntry['Instance'] = 0
    #Set all attributes default to 0
    BiosEntry['RegionType'] = 0
    BiosEntry['ResetImage'] = 0
    BiosEntry['Copy'] = 0
    BiosEntry['ReadOnly'] = 0
    BiosEntry['Compressed'] = 0
    BiosEntry['Recovery'] = 0
    #Update the attribute get from XML
    BiosEntry.update (entry.attrib)
    BiosEntry['Entry'] = entry.tag
    #Transfer all numeric field
    BiosEntry['Type'] = StrToNum(BiosEntry['Type'] )
    BiosEntry['Address'] = StrToNum(BiosEntry['Address'])
    BiosEntry['Size'] = StrToNum(BiosEntry['Size'])
    BiosEntry['Destination'] = StrToNum(BiosEntry['Destination'])
    BiosEntry['Instance'] = StrToNum(BiosEntry['Instance'])
    BiosEntry['Recovery'] = StrToNum(BiosEntry['Recovery'])

    for typeAttrib in entry:
      log.debug ("%s->%s",typeAttrib.tag, typeAttrib.attrib)
      #Update the attribute get from XML
      BiosEntry.update (typeAttrib.attrib)
      #Transfer all numeric field
      BiosEntry['RegionType'] = StrToNum(BiosEntry['RegionType'] )
      BiosEntry['ResetImage'] = StrToNum(BiosEntry['ResetImage'])
      BiosEntry['Copy'] = StrToNum(BiosEntry['Copy'])
      BiosEntry['ReadOnly'] = StrToNum(BiosEntry['ReadOnly'])
      BiosEntry['Compressed'] = StrToNum(BiosEntry['Compressed'])
    log.debug ("BiosEntry %s", BiosEntry)
    if (ValidateBiosEntry (BiosEntry) == False):
      log.info ("ERROR: Unrecognized Attribute/Tag found in %s->%s", entry.tag, entry.attrib)
      FatalErrorExit ()
    BiosEntries.append(BiosEntry)
  return BiosEntries

def ParseCfgFile (CfgFile):
  global AddressMode
  # To support multiple PSP Dir
  PspDicts = []
  BiosDicts = []

  try:
    tree = ET.parse (CfgFile)
  except:
    log.info ("ERROR: Parse the %s fail, please check file's format", CfgFile)
    FatalErrorExit ()
  root = tree.getroot ()
  # Get Address mode, if have been defined, or else use the default value
  if 'AddressMode' in root.attrib:
    log.info ("AddressMode explicit defined as %s", root.attrib['AddressMode'])

    AddressMode = StrToNum (root.attrib['AddressMode'])
    # Validate AddressMode Value
    if (not ((AddressMode == 0) or (AddressMode == 1))):
      log.info ("ERROR: Invalid AddressMode: 0 SPI(Default, if no explicit defined) 1: eMMC/UFS")
      FatalErrorExit ()
  for Dir in root:
    print Dir.tag
    #PSP DIR
    if (Dir.tag == 'PSP_DIR'):
      PspDict = {}
      #Init default value
      PspDict['Base'] = None
      PspDict['Size'] = None
      PspDict['SpiBlockSize'] = None
      #Default Level 1 
      PspDict['Level'] = 1
      #Update the attribute get from XML
      PspDict.update (Dir.attrib)
      #Checking Attribute Keyword (Case sensitive)
      if (ValidatePspDirAttrib (PspDict) == False):
        log.info ("ERROR: Unrecognized Attribute found in %s", Dir.attrib)
        FatalErrorExit ()
      # log.debug ("PspDict: %s",PspDict)
      #Build PSP Entries
      PspDict['Entries'] = BuildPspEntries (Dir)
      #Transfer all numeric field
      PspDict['Base'] = StrToNum (PspDict['Base'])
      PspDict['Size'] = StrToNum (PspDict['Size'])
      PspDict['SpiBlockSize'] = StrToNum (PspDict['SpiBlockSize'])
      PspDict['Level'] = StrToNum (PspDict['Level'])
      log.debug ("PspDict: %s", PspDict)
      PspDicts.append (PspDict)
    elif (Dir.tag == 'BIOS_DIR'):
      #BIOS DIR
      BiosDict = {}
      #Init default value
      BiosDict['Base'] = None
      BiosDict['Size'] = None
      BiosDict['SpiBlockSize'] = None
      #Default Level 1 
      BiosDict['Level'] = 1
      #Update the attribute get from XML
      BiosDict.update (Dir.attrib)
      #Checking Attribute Keyword (Case sensitive)
      if (ValidateBiosDirAttrib (BiosDict) == False):
        log.info ("ERROR: Unrecognized Attribute found in %s", Dir.attrib)
      #Build PSP Entries
      BiosDict['Entries'] = BuildBiosEntries (Dir)
      #Transfer all numeric field
      BiosDict['Base'] = StrToNum (BiosDict['Base'])
      BiosDict['Size'] = StrToNum (BiosDict['Size'])
      BiosDict['SpiBlockSize'] = StrToNum (BiosDict['SpiBlockSize'])
      BiosDict['Level'] = StrToNum (BiosDict['Level'])
      log.debug ("BiosDict: %s", BiosDict)
      BiosDicts.append (BiosDict)
    else:
      log.info ("ERROR: Unrecognized Tag (%s) found in %s", Dir.tag, CfgFile)
      FatalErrorExit ()
  #Check DIR region overlap
  #Make a generic DIR array with base size information
  DirList = []
  if (len(BiosDicts) != 0):
    for BiosDict in BiosDicts:
      if ((BiosDict['Base'] != None) and (BiosDict['Size'] != None)):
        DirElmt = {}
        DirElmt ['Base'] = BiosDict ['Base']
        DirElmt ['Size'] = BiosDict ['Size']
        DirList.append (DirElmt)

  if (len(PspDicts) != 0):
    for PspDict in PspDicts:
      if ((PspDict['Base'] != None) and (PspDict['Size'] != None)):
        DirElmt = {}
        DirElmt ['Base'] = PspDict ['Base']
        DirElmt ['Size'] = PspDict ['Size']
        DirList.append (DirElmt)

  for i in range (len (DirList)):
    for j in range (len (DirList)):
      #exclude Current index
      if (i != j):
        CurDirStart = DirList[i]['Base']
        CurDirEnd = DirList[i]['Base']+DirList[i]['Size'] - 1;
        OtherDirStart = DirList[j]['Base']
        OtherDirEnd = DirList[j]['Base']+DirList[j]['Size'] - 1;
        if (not((CurDirEnd < OtherDirStart) or (OtherDirEnd < CurDirStart))):
          log.info ("\nERROR: DIR [%x~%x] and DIR [%x~%x] region overlapped, please check PSP_DIR & BIOS_DIR \"Base\" & \"Size\" definition\n", CurDirStart, CurDirEnd, OtherDirStart, OtherDirEnd)
          FatalErrorExit ()

  return (PspDicts, BiosDicts)

def OptPspEntryOrder (PspDict):
# To be Done Optimize PSP entry Order
  return PspDict

def CalcEntryOffset (BiosSize, Directory):
  global DirHeaderSize
  global gAlignment
  Alignment = 0
  # Calculate the Entry offset for dynamic format configure file
  # Which Base and Size have been explicit define in the DIR Node(PSP_DIR or BIOS_DIR)
  if ((Directory['Base'] != None) and (Directory['Size'] != None)):
    #PSP Entry Start Address
    EntryOffset = AlignAddress (Directory['Base'] + DirHeaderSize, gAlignment)
    #Validate all entries:
    for Entry in Directory['Entries']:
      #Image Entry should not have Address attribute
      if (IsImageEntry(Entry)):
        log.debug (Entry)
        if (Entry['Address'] != None ):
          log.info ("ERROR: Entry should not have Address attribute as the 'Base' & 'Size' have been defined in parameter node")
          FatalErrorExit ()
        if (Entry['Size'] == None):
          if (CheckFileExistance (Entry['File']) == False):
            log.info ("ERROR: Can't open %s", Entry['File'])
            FatalErrorExit ()
          #If Size has been specified it will override the actual file size
          EntrySize = os.path.getsize(Entry['File'])
          Entry['Size'] = EntrySize
        else:
          if Entry['Type'] == 0x60 or Entry['Type'] == 0x68:
            if os.path.getsize(Entry['File']) > Entry['Size']:
              log.info("ERROR: HPCB size exceeds that defined in xml")
              FatalErrorExit()
        #Worker functions>>
        #Check if this entry is modifiable & SpiBlockSize is defined, using SpiBlockSize as Alignment, else using default Alignment
        if ((IsEntryModifiable (Entry['Type'])) and (Directory['SpiBlockSize'] != None)):
          Alignment = Directory['SpiBlockSize']
        else:
          Alignment = gAlignment
        #log.info("Before Align %x", EntryOffset)
        EntryOffset = AlignAddress (EntryOffset, Alignment)
        if (EntryOffset + Entry['Size'] > (Directory['Base'] + Directory['Size'])):
          log.info ("[Error] Exceed the limit of Dir Size 0x%xBytes\n", Directory['Size'])
          FatalErrorExit()
        #Record to structure
        Entry['Address'] = EntryOffset
        #Update to Next Entry offset
        EntryOffset += Entry['Size']
        #log.info("Type 0x%x Base %x Size %x", Entry['Type'], Entry['Address'], Entry['Size'])
    # Result
    UsedSize = EntryOffset - Directory['Base']
    log.info("PSP FV 0x%x bytes used, 0x%x bytes free [%.2f%% full]" ,UsedSize, (Directory['Size'] -  UsedSize), float (UsedSize)/float (Directory['Size']) *100 )
  return Directory

def DumpPspDict (BiosSize, PspDict):
  for Entry in PspDict['Entries']:
    if (IsImageEntry(Entry) or IsPointEntry(Entry)):
      log.info ("%s 0x%02X 0x%08x 0x%08x 0x%08x(R) (%s)", Entry['Entry'], Entry['Type'], Entry['Size'], Entry['Address'], Entry['Address'], getEntryName(Entry['Type']))
    elif (IsValueEntry(Entry)):
      log.info ("%s 0x%02X 0x%08x (%s)", Entry['Entry'], Entry['Type'], Entry['Value'], getEntryName(Entry['Type']))

def DumpBiosDict (BiosSize, BiosDict):
  for Entry in BiosDict['Entries']:
    if (IsImageEntry(Entry) or IsPointEntry(Entry)):
      log.info ("%s Type[%02X] RegionType[%02X] Reset[%01x] Copy[%01x] RO[%01x] ZIP[%01x] 0x%08x 0x%08x 0x%08x(R) 0x%x (%s)",Entry['Entry'], Entry['Type'],Entry['RegionType'],Entry['ResetImage'],Entry['Copy'],Entry['ReadOnly'], Entry['Compressed'], Entry['Size'], Entry['Address'],  Entry['Address'], Entry['Destination'], getEntryName(Entry['Type']))
    elif (IsValueEntry(Entry)):
      log.info ("%s 0x%02X 0x%08x (%s)", Entry['Entry'], Entry['Type'], Entry['Value'], getEntryName(Entry['Type']))

def BuildPspDir (BiosSize, PspDict):
  global AddressMode
  PspEntry=""
  PspEntryFmt = 'I I Q'
  PspEntrySize = struct.calcsize(PspEntryFmt)
  # Process each PSP entry
  for Entry in PspDict['Entries']:
    if (IsImageEntry(Entry) or IsPointEntry(Entry)):
      if (AddressMode == 0) :
        EntryData = (Entry['Type'], Entry['Size'], getMmioAddress (BiosSize, Entry['Address']))
      elif (AddressMode == 1):
        EntryData = (Entry['Type'], Entry['Size'], Entry['Address'])
    elif (IsValueEntry(Entry)):
      # Size field always be 0xFFFFFFFF for Value 
      EntryData = (Entry['Type'], 0xFFFFFFFF, Entry['Value'])
    s = struct.Struct(PspEntryFmt)
    #Pack Type
    PspEntry += s.pack(*EntryData)

  s = struct.Struct('I I')

  if (PspDict['Base'] != None and PspDict['Size'] != None):
    # Put PspDict['Size'] to the reserved field for Binary level replacement usage
    # Size:10  Max:1K * 4K = 4M
    # SpiBlockSize:4, 16 * 4K = 64K
    Rsvd =  ((PspDict['Size'] >> 12) << 0)
    if (PspDict['SpiBlockSize'] != None):
      Rsvd +=  ((PspDict['SpiBlockSize'] >> 12) << 10)
  else:
    Rsvd =  0
  values = (len (PspEntry)/PspEntrySize , Rsvd)
  CrcList = s.pack(*values) + PspEntry
  crc = Fletcher32Crc (CrcList, len (CrcList)/2)
  if (PspDict["Level"] == 1):
    Signature = "$PSP"
  elif (PspDict["Level"] == 2):
    Signature = "$PL2"
  else:
    log.info ("Unsupported Level detected.")
    FatalErrorExit ()
    
  values = (Signature, crc, len (PspEntry)/PspEntrySize , Rsvd)
  s = struct.Struct('4s I I I')
  PspHeader = s.pack(*values)
  PspDir = PspHeader+PspEntry
  return PspDir

def BuildBiosDir (BiosSize, BiosDict):
  BiosEntry=""
  BiosEntryFmt = 'I I Q Q'
  BiosEntrySize = struct.calcsize(BiosEntryFmt)
  MmioAddress = 0
  # Process each PSP entry
  for Entry in BiosDict['Entries']:
    if (IsImageEntry(Entry) or IsPointEntry(Entry)):
      # typedef struct {
      #         PSP_UINT32 Type : 8 ;      
      #         PSP_UINT32 RegionType : 8; 
      #         PSP_UINT32 BiosResetImage: 1;  //Set for SEC or EL3 fw
      #         PSP_UINT32 Copy: 1  
      #         PSP_UINT32 ReadOnly : 1;
      #         PSP_UINT32 Compressed : 1;   
      #         UINT32 Instance : 4;       ///< Specify the Instance of an entry
      #         UINT32 Reserved : 8;      ///< Reserve for future use
      #  } TypeAttrib;
      TypeAttrib = (Entry['Type'] + (Entry['RegionType'] << 8) + (Entry['ResetImage'] << 16)\
                   + (Entry['Copy'] << 17) + (Entry['ReadOnly'] <<18) + (Entry['Compressed'] <<19) + (Entry['Instance'] <<20))
      if (AddressMode == 0) :
        # Always Set Source to 0 for HPOB entry
        if (Entry['Type'] == 0x61):
          MmioAddress = 0
        else:
          MmioAddress = getMmioAddress (BiosSize, Entry['Address'])
        EntryData = (TypeAttrib, Entry['Size'], MmioAddress, Entry['Destination'])
      elif (AddressMode == 1):
        EntryData = (TypeAttrib, Entry['Size'], Entry['Address'], Entry['Destination'])
    elif (IsValueEntry(Entry)):
      # Size field always be 0xFFFFFFFF for Value 
      EntryData = (Entry['Type'], 0xFFFFFFFF, Entry['Value'], Entry['Destination'])
    s = struct.Struct(BiosEntryFmt)
    #Pack Type
    BiosEntry += s.pack(*EntryData)

  s = struct.Struct('I I')
  if (BiosDict['Base'] != None and BiosDict['Size'] != None):
    # Put BiosDict['Size'] to the reserved field for Binary level replacement usage
    # Size:10  Max:1K * 4K = 4M
    # SpiBlockSize:4, 16 * 4K = 64K
    Rsvd =  ((BiosDict['Size'] >> 12) << 0)
    if (BiosDict['SpiBlockSize'] != None):
      Rsvd +=  ((BiosDict['SpiBlockSize'] >> 12) << 10)
  else:
    Rsvd =  0
  values = (len (BiosEntry)/BiosEntrySize , Rsvd)
  CrcList = s.pack(*values) + BiosEntry
  crc = Fletcher32Crc (CrcList, len (CrcList)/2)
  if (BiosDict["Level"] == 1):
    Signature = "$BHD"
  elif (BiosDict["Level"] == 2):
    Signature = "$BL2"
  else:
    log.info ("Unsupport Level detected.")
    FatalErrorExit ()
  values = (Signature, crc, len (BiosEntry)/BiosEntrySize , Rsvd)
  s = struct.Struct('4s I I I')
  BiosHeader = s.pack(*values)
  BiosDir = BiosHeader+BiosEntry
  return BiosDir

def GetBiosTypeAttribs (TypeAttrib):
  Type = (TypeAttrib & 0xFF) >> 0
  RegionType = (TypeAttrib & 0xFF00) >> 8
  Reset = (TypeAttrib & 0x10000) >> 16
  Copy = (TypeAttrib & 0x20000) >> 17
  ReadOnly = (TypeAttrib & 0x40000) >> 18
  Compressed = (TypeAttrib & 0x80000) >> 19
  Instance = (TypeAttrib & 0xF00000) >> 20
  return (Type, RegionType, Reset, Copy, ReadOnly, Compressed, Instance)

def ParsePspDirBin (FileHandle, BinarySize, PspDirOffSetInBin, Level, PspDirectoryInfos):
  BiosDirOffset = 0
  BiosDirRawSize = 0
  #Verify PSP Directory blob
  #read Psp Header
  PspDirectoryInfo = {}
  PspDirectoryInfos.append (PspDirectoryInfo)
  FileHandle.seek (PspDirOffSetInBin)
  PspDirTblRaw = FileHandle.read (16)
  (Signature, Checksum, NumEntries, Rsvd) = struct.unpack('4L',PspDirTblRaw)
  PspDirSize = ((Rsvd & 0x3FF) >> 0) << 12
  SpiBlockSize = ((Rsvd & 0x3C00) >> 10) << 12
  PspDirectoryInfo ['DirSize'] = PspDirSize
  PspDirectoryInfo ['DirOffset'] = PspDirOffSetInBin
  PspDirectoryInfo ['SpiBlockSize'] = SpiBlockSize
  PspDirectoryInfo ['Level'] = Level
  
  log.info ("Psp Directory Offset 0x%x FV Size 0x%X SpiBlockSize 0x%X" ,PspDirOffSetInBin, PspDirSize, SpiBlockSize)

  #Check Signature
  if (Signature != PSP_DIR_SIGNATURE):
    log.info ("Psp Directory Table Signature Verify Fail")
    FatalErrorExit ()
  #To be Done Check CRC Checksum

  #Render & build structure
  #Read the whole Dir Table
  EntryInfoArray = []
  for i in range (NumEntries):
    EntryRaw = FileHandle.read (16)
    (Type, Size, Location) = struct.unpack ('LLQ', EntryRaw)
    if (Size != 0xFFFFFFFF):
      log.info ("Type [0x%02X] Size [0x%08X] Location [0x%08X ~ 0x%08X] (%s)",  Type,Size,Location, Location + Size -1, getEntryName(Type))
      EntryInfo = {'Type':Type,'Size':Size,'RTOffset':Location, 'Description':getEntryName(Type), 'Address':getOffsetInBin(BinarySize, Location)}
      EntryInfo['Entry'] = getEntryType(PspDirectoryInfo, EntryInfo)
      # Found a unknown type, set it to IMAGE_ENTRY to allow replace the unknown type
      if (EntryInfo['Entry'] == "UNKNOWN_ENTRY"):
        EntryInfo['Entry'] = "IMAGE_ENTRY"
      #Only given File attribute for IMAGE_ENTRY
      if (GetOutEntryFileName (EntryInfo, "PspDir", Level) != "Unsupported"):
        EntryInfo ['File'] = GetOutEntryFileName (EntryInfo, "PspDir", Level)
    else:
      log.info ("Type [0x%02X] VALUE [0x%08X] (%s)",  Type, Location, getEntryName(Type))
      #Ignore Size in Value entry
      EntryInfo = {'Type':Type,'Entry':'VALUE_ENTRY','Value':Location, 'Description':getEntryName(Type)}
    EntryInfoArray.append (EntryInfo)
  PspDirectoryInfo['Entries'] = EntryInfoArray


def ParseBiosDirBin (FileHandle, BinarySize, BiosDirOffset, Level, BiosDirectoryInfos):
  '''
      Build BIOS Dir structure
  '''
  BiosFwId = 0
  BiosDirectoryInfo = {}
  BiosDirectoryInfos.append (BiosDirectoryInfo)
  FileHandle.seek (BiosDirOffset)
  BiosDirTblRaw = FileHandle.read (16)
  (Signature, Checksum, NumEntries, Rsvd) = struct.unpack('4L',BiosDirTblRaw)
  BiosDirSize = ((Rsvd & 0x3FF) >> 0) << 12
  SpiBlockSize = ((Rsvd & 0x3C00) >> 10) << 12
  BiosDirectoryInfo ['DirOffset'] = BiosDirOffset  
  BiosDirectoryInfo ['DirSize'] = BiosDirSize
  BiosDirectoryInfo ['SpiBlockSize'] = SpiBlockSize
  BiosDirectoryInfo ['Level'] = Level
  log.info ("BIOS Directory FV Offset 0x%x Size 0x%X SpiBlockSize 0x%X" ,BiosDirOffset, BiosDirSize, SpiBlockSize)
  #Check Signature
  if (Signature != BIOS_DIR_SIGNATURE):
    log.info ("BIOS Directory Table Signature Verify Fail")
    FatalErrorExit ()

  EntryInfoArray = []
  FileNameDict = {}
  FileIndex = 0x30
  FileName = ''
  for i in range (NumEntries):
    EntryRaw = FileHandle.read (24)
    (TypeAttrib, Size, Location, Destination) = struct.unpack ('LLQQ', EntryRaw)
    if (Size != 0xFFFFFFFF):
      (Type, RegionType, ResetImage, Copy, ReadOnly, Compressed, Instance)= GetBiosTypeAttribs (TypeAttrib)
      log.info ("Type[%02X] Instance[%02X] RegionType[%02X] Reset[%01x] Copy[%01x] RO[%01x] ZIP[%01x]\n>>Size [0x%08X] Source [0x%08X ~ 0x%08X] Dest[0x%08x] (%s)",\
      Type, Instance, RegionType, ResetImage, Copy, ReadOnly, Compressed,\
      Size, Location, Location + Size -1, Destination, getEntryName(Type))
      EntryInfo = {'Type':Type, 'Instance':Instance, 'RegionType':RegionType, 'ResetImage':ResetImage, 'Copy': Copy, 'ReadOnly': ReadOnly, 'Compressed':Compressed, \
      'Size':Size,'RTOffset':Location, 'Description':getEntryName(Type), \
      'Address':getOffsetInBin(BinarySize, Location), 'Destination':Destination}
      EntryInfo['Entry'] = getEntryType(BiosDirectoryInfo, EntryInfo);
      # Found a unknown type, set it to IMAGE_ENTRY to allow replace the unknown type
      if (EntryInfo['Entry'] == "UNKNOWN_ENTRY"):
        EntryInfo['Entry'] = "IMAGE_ENTRY"
      #Only given File attribute for IMAGE_ENTRY
      if (GetOutEntryFileBaseName (EntryInfo, "BiosDir", Level) != "Unsupported"):
        FileName = GetOutEntryFileBaseName (EntryInfo, "BiosDir", Level)
        #Check if duplicated file name
        if (FileName in FileNameDict):
          FileName += ("_" + chr (FileIndex))
          FileIndex += 1
        FileNameDict[FileName] = 1
        #add file surfix
        EntryInfo ['File'] = FileName + ".bin"
        
      #Always return physical address for HPOB, and should be ZERO
      if (EntryInfo['Type'] == 0x61):
        EntryInfo['Address'] = getOffsetInBin(0, Location)
    else:
      log.info ("Type [0x%02X] VALUE [0x%08X] (%s)",  Type, Location, getEntryName(Type))
      #Ignore size,Destination in Value entry
      EntryInfo = {'Type':Type, 'Entry':'VALUE_ENTRY','Value':Location, 'Description':getEntryName(Type)}

    EntryInfoArray.append (EntryInfo)
  BiosDirectoryInfo['Entries'] = EntryInfoArray

def GetPspDirectory (BinaryFile):
  global AddressMode
  global ProgramTable
  PspDirectoryInfos = []
  BiosDirectoryInfos = []
  BinaryInfo = {}
  try:
    FileHandle = open (BinaryFile,'rb')
  except:
    log.info ("Error: Opening ", BinaryFile)
    FatalErrorExit ()
  #read whole binary to buffer
  FileHandle.seek (0)
  BinarySize = os.path.getsize(BinaryFile)
  BinaryInfo ['Binary'] = FileHandle.read (BinarySize)
  BinaryInfo ['BinaryName'] = BinaryFile
  BinaryInfo ['BinarySize'] = BinarySize

  ######################################
  # 1st try the Traditional ROMSIG way
  ######################################
  # Check Signature
  # log.info ("2 try the Traditional ROMSIG way")
  FileHandle.seek (0x20000)
  OemSigRaw = FileHandle.read (0x24)
  OemSigArray = struct.unpack ('9L',OemSigRaw)
  #Check OEM signature, ang get Psp Directory offset
  if (OemSigArray[ROMSIG_SIG_OFFSET] == 0x55aa55aa):
    log.info ("Oem Sig Table Found")
    PspDirRTOffset = OemSigArray[ROMSIG_PSPDIR_OFFSET]
    log.info ("Psp Directory Offset 0x%X" ,PspDirRTOffset)
    #convert to raw binary offset
    PspDirOffSetInBin = getOffsetInBin (BinarySize, PspDirRTOffset)
    log.info ("Psp Directory Offset in binary 0x%X" ,PspDirOffSetInBin)
    FileHandle.seek (PspDirOffSetInBin)
    PspDirTblRaw = FileHandle.read (16)
    (Signature, Checksum, NumEntries, PspDirSize) = struct.unpack('4L',PspDirTblRaw)
    if (Signature == PSP_DIR_SIGNATURE):
      log.info ("PSP Directory Found")
      #We found a Valid PSP Directory Header through ROM SIG
      ParsePspDirBin (FileHandle, BinarySize, PspDirOffSetInBin, 1, PspDirectoryInfos)
      BiosDirRTOffset = OemSigArray[ProgramTable[ProgramStr]["BIOSDIR_OFFSET"]]
      if (BiosDirRTOffset != 0):
        log.info ("BIOS Directory Offset 0x%X" ,BiosDirRTOffset)
        #convert to raw binary offset
        BiosDirOffSetInBin = getOffsetInBin (BinarySize, BiosDirRTOffset)
        ParseBiosDirBin (FileHandle, BinarySize, BiosDirOffSetInBin, 1, BiosDirectoryInfos)
      return (BinaryInfo, PspDirectoryInfos, BiosDirectoryInfos)

  # # Check BIOS SPI Image layout Or PSP SPI image layout
  # ######################################
  # # 2nd check if  PSP Image  # $PSP
  # ######################################
  FileHandle.seek (0)
  PspDirTblRaw = FileHandle.read (16)
  (Signature, Checksum, NumEntries, PspDirSize) = struct.unpack('4L',PspDirTblRaw)
  log.info ("2 check if PSP Image")  
  if (Signature == PSP_DIR_SIGNATURE):
    # Parse PSP DIR Header
    # Binary Size set 0, implies it is eMMC image, no need to do the address translation
    log.info ("PSP Image identified")
    AddressMode = 1
    ParsePspDirBin (FileHandle, 0, 0, 1, PspDirectoryInfos)
    return (BinaryInfo, PspDirectoryInfos, BiosDirectoryInfos)

def OutPutPspBinaries (bios,Entries):
  for Entry in Entries:
    if (GetOutEntryFileBaseName (Entry, "PspDir", 0) != "Unsupported"):
      outputFileName = Entry['File']
      #slice the binary
      print "Output %s of BIOS [%x:%x]" %(outputFileName, Entry['Address'], Entry['Address'] + Entry ['Size'] - 1)
      buffer = bios [Entry['Address']: Entry['Address'] + Entry ['Size']]
      WriteBinaryFile (outputFileName, buffer)

def OutPutBiosBinaries (bios,Entries):
  BiosFwId = 0
  for Entry in Entries:
    if (GetOutEntryFileBaseName (Entry, "BiosDir", 0) != "Unsupported"):
      outputFileName =  Entry['File']
      #slice the binary
      print "Output %s of BIOS [%x:%x]" %(outputFileName, Entry['Address'], Entry['Address'] + Entry ['Size'] - 1)
      buffer = bios [Entry['Address']: Entry['Address'] + Entry ['Size']]
      WriteBinaryFile (outputFileName, buffer)

def pspBootLoaderInfo (bios, entry):
  pspBLHeaderSize = 0x100
  verOffset = 0x60
  pspBLInfo = {}
  pspBLHeader = bios [entry['Address']:entry['Address']+pspBLHeaderSize]
  pspVer = struct.unpack ('L',pspBLHeader[verOffset:verOffset+4])
  verStr ="%X.%X.%X.%X" %(((pspVer[0] & 0xFF000000)>>24), ((pspVer[0] & 0xFF0000) >> 16),((pspVer[0] & 0xFF00) >> 8), ((pspVer[0] & 0xFF) >> 0))
  pspBLInfo['Version'] = verStr
  return pspBLInfo


def pspSecureOsInfo (bios, entry):
  pspOSHeaderSize = 0x100
  verOffset = 0x60
  pspOSInfo = {}
  pspOSHeader = bios [entry['Address']:entry['Address']+pspOSHeaderSize]
  pspVer = struct.unpack ('L',pspOSHeader[verOffset:verOffset+4])
  verStr ="%X.%X.%X.%X" %(((pspVer[0] & 0xFF000000)>>24), ((pspVer[0] & 0xFF0000) >> 16),((pspVer[0] & 0xFF00) >> 8), ((pspVer[0] & 0xFF) >> 0))
  pspOSInfo['Version'] = verStr
  return pspOSInfo

def smuFwInfo (bios, entry):
  SmuHeaderSize = 0x100
  verOffset = 0x60
  SmuInfo = {}
  SmuHeader = bios [entry['Address']:entry['Address']+SmuHeaderSize]
  SmuVer = struct.unpack ('L',SmuHeader[verOffset:verOffset+4])
  verStr ="%d.%d.%d" %(((SmuVer[0] & 0xFFFF0000)>>16), ((SmuVer[0] & 0xFF00) >> 8),((SmuVer[0] & 0x00FF) >> 0))
  SmuInfo['Version'] = verStr
  return SmuInfo

def ucodeInfo (bios, entry):
  HeaderSize = 0x30
  ucodeInfo = {}
  ucodeHeader = bios [entry['Address']:entry['Address']+HeaderSize]
  DateCode = struct.unpack ('L',ucodeHeader[0:4])
  PatchId = struct.unpack ('L',ucodeHeader[4:8])
  ProcessorRevisionID = struct.unpack ('H',ucodeHeader[24:26])
  ucodeInfo['DateCode'] = "%x/%x/%x" %(((DateCode[0] & 0xFF000000) >> 24),((DateCode[0] & 0xFF0000) >> 16), ((DateCode[0] & 0xFFFF) >> 0))
  ucodeInfo['PatchId'] = "%x" %(PatchId[0])
  ucodeInfo['EquivalentProcessorRevisionID'] = "%x" %(ProcessorRevisionID[0])
  return ucodeInfo

def OutPutPspDirInfoXml (BinaryInfo, PspDirectoryInfos, BiosDirectoryInfos, xmlFileName):
  try:
    FileHandle = open (xmlFileName,'w')
  except:
    print "Error: Opening ", xmlFileName
    FatalErrorExit ()
  root = Element('Dirs')
  
  if (len (PspDirectoryInfos) != 0):
    for PspDirectoryInfo in PspDirectoryInfos:
      DirDict = {}
      DirDict ['Level'] = PspDirectoryInfo ['Level']
      DirDict = StringlizeDict (DirDict)
      PspDirElmt = SubElement(root, 'PspDirectory', DirDict)
      for entry in PspDirectoryInfo['Entries']:
        _entry = entry.copy()
        #convert all numberic type to string, for tostring function only accept string
        _entry = StringlizeDict (_entry)
        #Override the default if it has been defined
        if (entry['Type'] in PspDirectoryEntryName):
            EntryType = entry['Entry']
        #Get Entry type from PspDirectoryEntryName
        #                          Type:   DIR, Entry Type    ShortName    Full description
        #                            0x00: ['PSP' ,'IMAGE_ENTRY','HygonPubKey','HYGON public Key', ],
        entryElmt = SubElement (PspDirElmt, EntryType, _entry)
        if (entry['Type'] == 1) or (entry['Type'] == 3):
          SubElement (entryElmt, 'Detail', pspBootLoaderInfo(BinaryInfo['Binary'], entry))
        elif (entry['Type'] == 2):
          SubElement (entryElmt, 'Detail', pspSecureOsInfo(BinaryInfo['Binary'], entry))
        elif (entry['Type'] == 8):
          SubElement (entryElmt, 'Detail', smuFwInfo(BinaryInfo['Binary'], entry))
        elif (entry['Type'] == 0xb):
          SubElement (entryElmt, 'Detail', {"Help":'''BIT0:PSP Secure Debug Control Flag (0-Disabled, 1-Enabled);\
           BIT1:Unified boot Or Conventional boot selection Flag ( 0-Unified boot, 1-Conventional boot)'''})

  if (len (BiosDirectoryInfos) != 0):
    for BiosDirectoryInfo in BiosDirectoryInfos:
      DirDict = {}
      DirDict ['Level'] = BiosDirectoryInfo ['Level']
      DirDict = StringlizeDict (DirDict)
      BiosDirElmt = SubElement(root, 'BiosDirectory',DirDict)
      for entry in BiosDirectoryInfo['Entries']:
        _entry = entry.copy()
        #convert all numberic type to string, for tostring function only accept string
        _entry = StringlizeDict (_entry)
        #Override the default if it has been defined
        if (entry['Type'] in PspDirectoryEntryName):
            EntryType = entry['Entry']
        entryElmt = SubElement (BiosDirElmt, EntryType, _entry)
        if (entry['Type'] == 0x66):
          SubElement (entryElmt, 'Detail', ucodeInfo(BinaryInfo['Binary'], entry))

  print >>FileHandle, prettify(root)

def OutPutBiosDirectoryXml (root, BiosDirectoryInfo):
  DirDict = {}
  DirDict ['Base'] = BiosDirectoryInfo ['DirOffset']
  if (BiosDirectoryInfo ['DirSize'] != 0):
    DirDict ['Size'] = BiosDirectoryInfo ['DirSize']
  if (BiosDirectoryInfo ['SpiBlockSize'] != 0):
    DirDict ['SpiBlockSize'] = BiosDirectoryInfo ['SpiBlockSize']
  DirDict ['Level'] = BiosDirectoryInfo ['Level']
  DirDict = StringlizeDict (DirDict)
  DirElmt = SubElement (root, 'BIOS_DIR', DirDict)
  for entry in BiosDirectoryInfo['Entries']:
    EntryDict = {}
    if (IsValueEntry (entry)):
      EntryDict = dict ((k, entry[k]) for k in ['Type', 'Value'])
      EntryDict = StringlizeDict (EntryDict)
      SubElement (DirElmt, 'VALUE_ENTRY', EntryDict)
    elif (IsImageEntry(entry)):
      EntryDict['File'] = OutputPath + entry['File']
      EntryDict['Type'] = entry ['Type']
      #Prepare for the 'Size' & 'Address'
      #fill the Size, Address attributes when Dir Size is 0, which means
      #User disable the auto allocation function
      if (BiosDirectoryInfo ['DirSize'] == 0):
        EntryDict['Size'] = entry ['Size']
        EntryDict['Address'] = entry ['Address']
      #Prepare for the 'Destination'
      if (entry['Destination'] != 0xFFFFFFFFFFFFFFFF):
        EntryDict['Destination'] = entry ['Destination']
      #Fill the instance, if instance is not 0
      if (entry['Instance'] != 0):
        EntryDict['Instance'] = entry ['Instance']

      EntryDict = StringlizeDict (EntryDict)
      EntryElmt = SubElement (DirElmt, 'IMAGE_ENTRY', EntryDict)

      #Prepare for the TypeAttrib tag, if any below attributes in non-zero
      if (not ((entry['RegionType'] == 0) and\
          (entry['ResetImage'] == 0) and\
          (entry['Copy'] == 0) and\
          (entry['ReadOnly'] == 0) and\
          (entry['Compressed'] == 0))):
        AttribsDict = {}
        AttribsDict = dict ((k, entry[k]) for k in ['RegionType', 'ResetImage', 'Copy', 'ReadOnly', 'Compressed'])
        AttribsDict = StringlizeDict (AttribsDict)
        SubElement (EntryElmt, 'TypeAttrib', AttribsDict)

    elif (IsPointEntry (entry)):
      EntryDict['Type'] = entry ['Type']
      EntryDict['Size'] = entry ['Size']
      EntryDict['Address'] = entry ['Address']  
      #Prepare for the 'Destination'
      if (entry['Destination'] != 0xFFFFFFFFFFFFFFFF):
        EntryDict['Destination'] = entry ['Destination']
      EntryDict = StringlizeDict (EntryDict)
      EntryElmt = SubElement (DirElmt, 'POINT_ENTRY', EntryDict)

      #Prepare for the TypeAttrib tag, if any below attributes in non-zero
      if (not ((entry['RegionType'] == 0) and\
          (entry['ResetImage'] == 0) and\
          (entry['Copy'] == 0) and\
          (entry['ReadOnly'] == 0) and\
          (entry['Compressed'] == 0))):
        AttribsDict = {}
        AttribsDict = dict ((k, entry[k]) for k in ['RegionType', 'ResetImage', 'Copy', 'ReadOnly', 'Compressed'])
        AttribsDict = StringlizeDict (AttribsDict)
        SubElement (EntryElmt, 'TypeAttrib', AttribsDict)
    else:
      log.info ("Unrecognized entry Type")
      # FatalErrorExit ()
  
def OutPutPspDirectoryXml (PspDirectoryInfos, BiosDirectoryInfos, cfgFileName):
  global AddressMode
  try:
    FileHandle = open (cfgFileName,'w')
  except:
    print "Error: Opening ", cfgFileName
    sys.exit(2)
  RootDict = {}
  if (AddressMode):
    RootDict['AddressMode'] = AddressMode
  RootDict = StringlizeDict (RootDict)
  root = Element('DIRS', RootDict)
  DirDict = {}
  if (len (PspDirectoryInfos) != 0):
    for PspDirectoryInfo in PspDirectoryInfos:
      # DirDict ['Base'] = PspDirectoryInfo ['PspDirOffset']
      DirDict ['Base'] = PspDirectoryInfo ['DirOffset']
      if (PspDirectoryInfo ['DirSize'] != 0):
        DirDict ['Size'] = PspDirectoryInfo ['DirSize']
      if (PspDirectoryInfo ['SpiBlockSize'] != 0):
        DirDict ['SpiBlockSize'] = PspDirectoryInfo ['SpiBlockSize']
      DirDict ['Level'] = PspDirectoryInfo ['Level']
      DirDict = StringlizeDict (DirDict)
      DirElmt = SubElement (root, 'PSP_DIR', DirDict)
      for entry in PspDirectoryInfo['Entries']:
        EntryDict = {}
        if (IsValueEntry (entry)):
          EntryDict = dict ((k, entry[k]) for k in ['Type', 'Value'])
          EntryDict = StringlizeDict (EntryDict)
          SubElement (DirElmt, 'VALUE_ENTRY', EntryDict)
        elif (IsImageEntry (entry)):
          # Dynamic Address
          # EntryInfo = {'Type':Type,'Size':Size,'RTOffset':Location, 'Description':getEntryName(Type), 'Address':getOffsetInBin(biosSize, Location)}
          EntryDict['File'] = OutputPath +  entry['File']
          if (PspDirectoryInfo ['DirSize'] != 0):
            EntryDict['Type'] = entry['Type']
          else:
            EntryDict['Type'] = entry ['Type']
            EntryDict['Size'] = entry ['Size']
            EntryDict['Address'] = entry ['Address']  
          EntryDict = StringlizeDict (EntryDict)
          SubElement (DirElmt, 'IMAGE_ENTRY', EntryDict)
        elif (IsPointEntry (entry)):
          EntryDict['Type'] = entry ['Type']
          EntryDict['Size'] = entry ['Size']
          EntryDict['Address'] = entry ['Address']  
          # EntryDict.update = {(k, entry[k]) for k in ['Type', 'Size', 'Address']}
          EntryDict = StringlizeDict (EntryDict)
          SubElement (DirElmt, 'POINT_ENTRY', EntryDict)
        else:
          log.info ("Unrecognized entry Type")
          # FatalErrorExit ()

  if (len (BiosDirectoryInfos) != 0):
    for BiosDirectoryInfo in BiosDirectoryInfos:
      OutPutBiosDirectoryXml (root, BiosDirectoryInfo)

  print >>FileHandle, prettify(root)

def main ():
  global OutputPath
  log.info ("%s","Parse Input Parameters:")
  Args = ParseArg()
  log.debug ("Args: %s",Args)
  OutputPath = Args.outputpath+"/"
  print "BuildPspDirectory   Version "+ Version+"\n"
  #Always create output folder except dp command with no additional parameters
  if not (Args.subcommand == 'dp' and
          Args.binary != True and
          Args.xml != True and 
          Args.directory != True):
    if not os.path.exists(OutputPath):
            os.makedirs(OutputPath)
  if (Args.subcommand == 'bd' or Args.subcommand == 'bb'):
    BiosSize=os.path.getsize(Args.InBiosImage)
    log.info ("Parse Configure File: %s\n", Args.CfgFile)
    (PspDicts, BiosDicts) = ParseCfgFile (Args.CfgFile)
    if (len(PspDicts) != 0):
      # Traverse PspDicts array
      for PspDict in PspDicts:
        log.info ("Optimize PSP Entry Order\n")
        PspDict = OptPspEntryOrder (PspDict)

        log.info ("Calculate PSP Entry Address\n")
        PspDict = CalcEntryOffset(BiosSize, PspDict)
        DumpPspDict (BiosSize, PspDict)
        log.info ("Prepare for PspDirecotry Header Data")
        PspDict ["PspDirHeader"] = BuildPspDir (BiosSize, PspDict)

    if (len(BiosDicts) != 0):
      # Traverse PspDicts array
      for BiosDict in BiosDicts:
        log.info ("Calculate BIOS Entry Address\n")    
        BiosDict = CalcEntryOffset (BiosSize, BiosDict)
        DumpBiosDict (BiosSize, BiosDict)

        log.info ("Prepare for BiosDirecotry Header Data")
        BiosDict ["BiosDirHeader"] = BuildBiosDir (BiosSize, BiosDict)


    if (Args.subcommand == 'bd'):
      if (len(PspDicts) != 0):
        for PspDict in PspDicts:
          log.info ("Write the data to file %s%s", OutputPath, "PspDirHeaderL"+str (PspDict["Level"])+".bin")
          WriteBinaryFile ("PspDirHeaderL"+str (PspDict["Level"])+".bin", PspDict["PspDirHeader"])
      if (len(BiosDicts) != 0):
        for BiosDict in BiosDicts:
          log.info ("Write the data to file %s%s", OutputPath, "BiosDirHeaderL"+ str (BiosDict["Level"]) +".bin")
          # Only write the Level 1 Header
          WriteBinaryFile ("BiosDirHeaderL"+ str (BiosDict["Level"]) +".bin", BiosDict["BiosDirHeader"])

    elif (Args.subcommand == 'bb'):
      log.info ("Patch BIOS Image")
      # Read the BIOS file
      BiosBinary = ReadBinaryFile (Args.InBiosImage)
      #First update the PSPDirectory in the binary image
      if (len(PspDicts) != 0):
        for PspDict in PspDicts:
          if (PspDict['Base'] == None):
            log.info ("Error: Base attribute haven't been defined in PSP_DIR tag")
            FatalErrorExit()
          BiosBinary = PatchBinary (BiosBinary, PspDict["PspDirHeader"], PspDict['Base'], "PSPDirecory ")
          # Process each PSP entry
          for Entry in PspDict['Entries']:
            if (IsImageEntry(Entry)):
              PspBinary = ReadBinaryFile (Entry['File'])
              if (PspBinary == None):
                log.info ("Error Reading %s\n", Entry['File'])
                FatalErrorExit()
              # And patch the output image
              BiosBinary = PatchBinary (BiosBinary, PspBinary, Entry['Address'], Entry['File']+" "+hex(Entry['Type']))

      #Second update the BiosDirectory in the binary image
      if (len(BiosDicts) != 0):
        for BiosDict in BiosDicts:
          if (BiosDict['Base'] == None):
            log.info ("Error: Base attribute haven't been defined in BIOS_DIR tag")
            FatalErrorExit()
          BiosBinary = PatchBinary (BiosBinary, BiosDict["BiosDirHeader"], BiosDict['Base'], "BIOSDirecory ")
          # Process each BIOS entry
          for Entry in BiosDict['Entries']:
            if (IsImageEntry(Entry)):
              PspBinary = ReadBinaryFile (Entry['File'])
              if (PspBinary == None):
                log.info ("Error Reading %s\n", Entry['File'])
                FatalErrorExit()
              # And patch the output image
              BiosBinary = PatchBinary (BiosBinary, PspBinary, Entry['Address'], Entry['File']+" "+hex(Entry['Type']))

      log.info ("Generate BIOS image [%s]" ,OutputPath + Args.OutBiosImage)
      WriteBinaryFile (Args.OutBiosImage, BiosBinary)
  elif (Args.subcommand == 'dp'):
    log.info ("Dump BIOS Psp DirectoryOut for %s" %(Args.program))
    (BinaryInfo, PspDirectoryInfos, BiosDirectoryInfos) = GetPspDirectory (Args.InBiosImage)
    if ((len (PspDirectoryInfos) == 0) and (len (BiosDirectoryInfos) == 0)):
      print "Error: Result of GetPspDirectory is invalid"
      FatalErrorExit ()
    if (Args.binary == True):
      print "Output  PSP binaries" 
      if (len (PspDirectoryInfos) != 0):
        for PspDirectoryInfo in PspDirectoryInfos:
          OutPutPspBinaries (BinaryInfo['Binary'],PspDirectoryInfo['Entries'])

      print "Output  BIOS binaries" 
      if (len (BiosDirectoryInfos) != 0):
        for BiosDirectoryInfo in BiosDirectoryInfos:
          OutPutBiosBinaries (BinaryInfo['Binary'],BiosDirectoryInfo['Entries'])
    if (Args.xml == True):
      print "Output  PspDirInfo.xml"
      OutPutPspDirInfoXml (BinaryInfo, PspDirectoryInfos, BiosDirectoryInfos, OutputPath + "PspDirInfo.xml")
    if (Args.directory == True):
      print "Output  PspDirectory.xml"
      OutPutPspDirectoryXml (PspDirectoryInfos, BiosDirectoryInfos, OutputPath + "PspDirectory.xml")
if __name__ == "__main__":
  main()