 # *****************************************************************************
 # *
 # * 
 # * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 # * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 # * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 # * the right to make the modified version available for use with HYGON's PRODUCT.
 # ******************************************************************************

# ###############################################################################
# Check_ABL_Version.py - Check the version number in the ABL Portion of the
#                        Bootloader Binary Header against APCB/APPB/APOB
#                        
#   Usage:  Check_ABL_Version.py <AgesaBootloader.bin>
#
# AgesaBootloader.bin :  Compilers Agesa Bootloader Binary
#
from sys import *
from getopt import *
from string import *
import os
import re
# ################################################################################
# Definitions
#
ScriptName = "Check_ABL_Version"

#
# APCB/APPB/APOB header files
#
ApcbHeaderPath = "APCB.h"
AppbHeaderPath = "APPB.h"
ApobHeaderPath = "APOB.h"

#
# Search Patterns
#
ApcbVersionDefinition = '^#define\s+APCB_HEADER_VERSION\s+0x([0-9A-F]+)'
AppbVersionDefinition = '^#define\s+APPB_VERSION\s+0x([0-9A-F]+)'
ApobVersionDefinition = '^#define\s+APOB_VERSION\s+0x([0-9A-F]+)'
#
# ABL Binary Header Offsets
#
AblVersionOffset  = 0x60
ApcbVersionOffset = 0xA0
ApobVersionOffset = 0xA4
AppbVersionOffset = 0xA8
SvcVersionOffset  = 0xAC
#
# APCB Binary Header Offset
#
ApcbBinVersionOffset = 0x06
#
# PSP BL Binary Offset
#
PspBlBinSvcVersionOffset = 0xA8
#
#  Table of Input Paramweters
#  This table matched the signature string with the offset in the header where it should be stored
#
VersionTable = [
	[ ApcbHeaderPath, ApcbVersionDefinition, ApcbVersionOffset],
	[ AppbHeaderPath, AppbVersionDefinition, AppbVersionOffset],
	[ ApobHeaderPath, ApobVersionDefinition, ApobVersionOffset]
	]

# ##############################################################
#
#  Display Usage
#
def Usage ():
	print '\n Usage 1: ' + argv[0] + ' --abl_bin=<AgesaBootloader> --apxb_path=<HeaderFilePath> --apcb_bin=<APCB>\n'
	print '  AgesaBootloader :  Compiled Agesa Bootloader Binary'
	print '  HeaderFilePath  :  Path of APCB.h/APPB.h/APOB.h'
	print '  APCB            :  APCB Binary'
	print
	print '\n Usage 2: ' + argv[0] + ' --abl_bin=<AgesaBootloader.bin> --apxb_path=<HeaderFilePath> --apcb_bin=<APCB> --psp_bl_bin=<PspBootloader>\n'
	print '  AgesaBootloader :  Compiled Agesa Bootloader Binary'
	print '  HeaderFilePath  :  Path of APCB.h/APPB.h/APOB.h'
	print '  APCB            :  APCB Binary'
	print '  PspBootloader   :  Compiled PSP Bootloader Binary'
	print
	exit (1)
	return

# ##############################################################
#
# Entry Point

# ##############################################################
#
# Process Arguments
#
print
print ScriptName
print

opts, arge = getopt (argv[1:], "h", ["abl_bin=", "apxb_path=", "apcb_bin=", "psp_bl_bin="])

AblBinNamePassed = 0
ApxbHeaderPathPassed = 0
ApcbBinNamePassed = 0
PspBlBinNamePassed = 0
AblBinName = ''
ApxbHeaderPath = ''
ApcbBinName = ''
PspBlBinName = ''

for op, value in opts:
	if op == '-h':
		Usage ()
		exit (1)
	elif op == '--abl_bin':
		AblBinName = value
		print '\t--abl_bin = ' +  AblBinName
		print
		AblBinNamePassed = 1
	elif op == '--apxb_path':
		ApxbHeaderPath = value
		ApxbHeaderPathPassed = 1
		print '\t--apxb_path = ' +  ApxbHeaderPath
		print
	elif op == '--apcb_bin':
		ApcbBinName = value
		ApcbBinNamePassed = 1
		print '\t--apcb_bin = ' +  ApcbBinName
		print
	elif op == '--psp_bl_bin':
		PspBlBinName = value
		PspBlBinNamePassed = 1
		print '\t--psp_bl_bin = ' +  PspBlBinName
		print

if AblBinNamePassed == 0 or ApxbHeaderPathPassed == 0 or ApcbBinNamePassed == 0:
	Usage ()
	exit (1)

#
# Read in the ABL Binary File 
#
if (os.path.exists (AblBinName)):
	with open (AblBinName,'rb') as AblBinFile:
		AblBinData = AblBinFile.read ()
else:
	print "--> Error: ABL binary file {} is required to determine APCB/APPB/APOB version information.".format (AblBinName)
	exit (1)

print "\tABL Binary File: " + AblBinName

if PspBlBinNamePassed == 1:
	#
	# Read in the PSP BL Binary File 
	#
	if (os.path.exists (PspBlBinName)):
		with open (PspBlBinName,'rb') as PspBlBinFile:
			PspBlBinData = PspBlBinFile.read ()
	else:
		print "--> Error: PSP BL binary file {} is required to determine SVC version information.".format (PspBlBinName)
		exit (1)
	print "\tPSP BL Binary File: " + PspBlBinName
	AblBinSvcVersion = AblBinData[SvcVersionOffset : SvcVersionOffset + 4]
	print "\tABL Binary SVC Version: {}".format (AblBinSvcVersion[::-1].encode('hex'))
	PspBlBinSvcVersion = PspBlBinData[PspBlBinSvcVersionOffset: PspBlBinSvcVersionOffset + 4]
	print "\tPSP BL Binary SVC Version: {}".format (PspBlBinSvcVersion[::-1].encode('hex'))
	if int (AblBinSvcVersion[::-1].encode('hex'), 16) > int (PspBlBinSvcVersion[::-1].encode('hex'), 16):
		print "--> Error: ABL Syscall Version is out of sync with te SysCall version in PSP BL!"
		exit (1)

#
# Read in the APCB Binary File 
#
if (os.path.exists (ApcbBinName)):
	with open (ApcbBinName,'rb') as ApcbBinFile:
		ApcbBinData = ApcbBinFile.read ()
else:
	print "--> Error: APCB binary file {} is required to determine APCB version information.".format (AblBinName)
	exit (1)

print "\tAPCB Binary File: " + ApcbBinName

for InputParams in VersionTable:
	DefinitionFound = 0
	APXBName = InputParams[0]
	APXBHeaderName = ApxbHeaderPath + "\\" + InputParams[0]
	APXBVersionDef = InputParams[1]
	APXBVersionOffset = InputParams[2]

	Version = AblBinData[APXBVersionOffset : APXBVersionOffset + 4]
	if (os.path.exists (APXBHeaderName)):
		#
		# Open APXB Header File
		#
		with open (APXBHeaderName,'r') as APXBHeaderFile:
			for line in APXBHeaderFile:
				if re.match (APXBVersionDef, line):
					DefinitionFound = 1
					VersionString = re.match (APXBVersionDef, line)
					print "\tABL Binary {} Version: {}".format (APXBName, Version[::-1].encode('hex'))
					print "\t{} Version: {}".format (APXBName, VersionString.group(1))
					if int (VersionString.group(1), 16) < int (Version[::-1].encode('hex'), 16):
						print "--> Error: ABL {} Version is out of sync with {}!".format (APXBName[0:4], APXBName)
						exit (1)
					#
					# Check against APCB Binary
					#
					if (re.match ('APCB', APXBName)):
						ApcbBinVersion = ApcbBinData[ApcbBinVersionOffset : ApcbBinVersionOffset + 2]
						print "\tAPCB Binary Version: {}".format (ApcbBinVersion[::-1].encode('hex'))
						if int (VersionString.group(1), 16) < int (ApcbBinVersion[::-1].encode('hex'), 16):
							print "--> Error: {} Version is out of sync with {}!".format (APXBName[0:4], APXBName)
							exit (1)
	else:
		print "--> Error: Header file {} is required to determine version information.".format (APXBHeaderName)
		exit (1)

	if DefinitionFound == 0:
		print "--> Error: No version defined in Header file {}".format (APXBHeaderName)
		exit (1)
