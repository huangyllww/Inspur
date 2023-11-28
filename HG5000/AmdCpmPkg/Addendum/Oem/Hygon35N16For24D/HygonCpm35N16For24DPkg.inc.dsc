#;*****************************************************************************
#;
#; 
#; Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
#; 
#; HYGON is granting you permission to use this software (the Materials)
#; pursuant to the terms and conditions of your Software License Agreement
#; with HYGON.  This header does *NOT* give you permission to use the Materials
#; or any rights under HYGON's intellectual property.  Your use of any portion
#; of these Materials shall constitute your acceptance of those terms and
#; conditions.  If you do not agree to the terms and conditions of the Software
#; License Agreement, please do not use any portion of these Materials.
#; 
#; CONFIDENTIALITY:  The Materials and all other information, identified as
#; confidential and provided to you by HYGON shall be kept confidential in
#; accordance with the terms and conditions of the Software License Agreement.
#; 
#; LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
#; PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
#; WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
#; MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
#; OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
#; IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
#; (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
#; INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
#; GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
#; RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
#; THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
#; EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
#; THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
#; 
#; HYGON does not assume any responsibility for any errors which may appear in
#; the Materials or any other related information provided to you by HYGON, or
#; result from use of the Materials or any related information.
#; 
#; You agree that you will not reverse engineer or decompile the Materials.
#; 
#; NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
#; further information, software, technical information, know-how, or show-how
#; available to you.  Additionally, HYGON retains the right to modify the
#; Materials at any time, without notice, and is not obligated to provide such
#; modified Materials to you.
#; 
#; AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
#; the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
#; the right to make the modified version available for use with HYGON's PRODUCT.
#;*****************************************************************************

[Defines]
 DEFINE AGESA_PKG_PATH   = AgesaModulePkg
 DEFINE AGESA_PKG_DEC    = AgesaCommonModulePkg
 DEFINE FCH_PKG_PATH     = $(AGESA_PKG_PATH)/Fch/Taishan
 DEFINE FCH_PKG_DEC      = FchTaishan

[LibraryClasses]
  #
  # Cpm specific common libraries
  #
  AmdCpmBaseLib|AmdCpmPkg/Library/Proc/Base/AmdCpmBaseLib.inf
  AmdCpmCpuLib|AmdCpmPkg/Library/Proc/Cpu/AmdCpmCpu.inf
  AmdCpmFchLib|AmdCpmPkg/Library/Proc/Fch/AmdCpmFch.inf

[Components.IA32]
  AmdCpmPkg/Addendum/Oem/Hygon35N16For24D/Pei/HygonCpmOemInitPei35N16/HygonCpmOemInitPeim35N16.inf
  AmdCpmPkg/Addendum/Oem/Hygon35N16For24D/Pei/HygonCpmOemInitPei35N16SL1r2/HygonCpmOemInitPeim35N16SL1r2.inf
  AmdCpmPkg/Addendum/Oem/Hygon35N16For24D/Pei/HygonSataWorkaround/HygonSataWorkaround.inf
  AmdCpmPkg/Kernel/Pei/AmdCpmInitPeim.inf
  AmdCpmPkg/Features/BoardId/Pei/AmdBoardIdPei.inf
  AmdCpmPkg/Features/GpioInit/Pei/AmdCpmGpioInitPeim.inf
  AmdCpmPkg/Features/I2cMaster/Pei/AmdI2cMasterPei.inf
  AmdCpmPkg/Features/PcieInit/Pei/AmdCpmPcieInitPeim.inf
  AmdCpmPkg/Devices/Ds125Br401a/Pei/Ds125Br401aPei.inf
  AmdCpmPkg/Devices/M24LC128/Pei/M24Lc128Pei.inf
  AmdCpmPkg/Devices/Pca9535a/Pei/Pca9535aPei.inf
  AmdCpmPkg/Devices/Pca9545a/Pei/Pca9545aPei.inf
  AmdCpmPkg/Devices/Sff8472/Pei/Sff8472Pei.inf
  AmdCpmPkg/Features/SataTxEq/Pei/SataTxEqPei.inf

[Components.X64]
  AmdCpmPkg/Kernel/Asl/AmdCpmInitAsl.inf
  AmdCpmPkg/Kernel/Dxe/AmdCpmInitDxe.inf
  AmdCpmPkg/Kernel/Smm/AmdCpmInitSmm.inf
  AmdCpmPkg/Features/BoardId/Dxe/AmdBoardIdDxe.inf
  AmdCpmPkg/Features/PcieInit/Asl/AmdCpmPcieInitAsl.inf
  AmdCpmPkg/Features/PcieInit/Dxe/AmdCpmPcieInitDxe.inf
  AmdCpmPkg/Features/I2cMaster/Dxe/AmdI2cMasterDxe.inf
  AmdCpmPkg/Features/GpioInit/Dxe/AmdCpmGpioInitDxe.inf
  AmdCpmPkg/Features/GpioInit/Smm/AmdCpmGpioInitSmm.inf
  AmdCpmPkg/Features/PlatformRas/Dxe/AmdPlatformRasDxe.inf
  AmdCpmPkg/Features/PlatformRas/Smm/AmdPlatformRasSmm.inf
  AmdCpmPkg/Features/xGbEI2cMaster/xGbEI2cMasterDxe.inf
  AmdCpmPkg/Devices/Pca9535a/Dxe/Pca9535aDxe.inf
  AmdCpmPkg/Devices/Pca9545a/Dxe/Pca9545aDxe.inf
  AmdCpmPkg/Devices/Sff8472/Dxe/Sff8472Dxe.inf

[PcdsFixedAtBuild]
  gAmdCpmPkgTokenSpaceGuid.ApuSupportMask|0x00000720  # CZ==5 & ST==8 & BR==9 & ZP==10

  # V9 Pcds
  # IDS Debug Lib Pcds
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintEnable|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintHdtOutEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintHdtOutForceEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintRedirectIOEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPortEnable|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPort|0x3F8
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintFilter|0xF3FFFFFFFFFFFF00

  # Customized DIMM Vendor Strings
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord0|0x2C00
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord1|0xAD00
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord2|0xC100
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord3|0xCE00
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord4|0x4F00
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord5|0x9801
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord6|0xFE02
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord7|0x0B03
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord8|0x2503
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord9|0x8303
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord10|0xCB04
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord11|0xC106

  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr0|"Micron Technology"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr1|"Hynix"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr2|"Infineon (Siemens)"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr3|"Samsung"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr4|"Transcend Information"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr5|"Kingston"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr6|"ELPIDA"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr7|"Nanya Technology"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr8|"Kingmax Semiconductor"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr9|"Buffalo (Formerly Melco)"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr10|"A-DATA Technology"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr11|"ASint Technology"

  #Fch
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFchCfgSioPmeBaseAddress|0x1640
  

[PcdsDynamicDefault]
  ## Customize AGESA GNB
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgGnbIoapicAddress|0xFEC01000

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgAzaliaSsid|0xC9501D94

  ## Customize AGESA FCH
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdLegacyFree|FALSE

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdHpetEnable|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdHpetMsiDis|FALSE

##  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdOemProgrammingTablePtr|{ \
##  0x00, 0x00, 0xB0, 0xAC, \
##  0x02, 0x48, 0x00, 0x08, \
##  0x02, 0x78, 0x00, 0x19, \
##  0x02, 0x79, 0x00, 0x19, \
##  0x02, 0x58, 0x00, 0x18, \
##  0x02, 0x59, 0x00, 0x18, \
##  0x02, 0x5A, 0x00, 0x18, \
##  0x02, 0x70, 0x00, 0x01, \
##  0xFF, 0xFF, 0xFF, 0xFF }
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdOemProgrammingTablePtr|{0x00, 0x00, 0xB0, 0xAC,  0x02, 0x48, 0x00, 0x08,  0x02, 0x78, 0x00, 0x19,  0x02, 0x79, 0x00, 0x19,  0x02, 0x58, 0x00, 0x18,  0x02, 0x59, 0x00, 0x18,  0x02, 0x5A, 0x00, 0x18,  0x02, 0x70, 0x00, 0x01,  0xFF, 0xFF, 0xFF, 0xFF}

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdNativePcieSupport|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdNoneSioKbcSupport|TRUE

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataEnable|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataClass|0x06

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXhci0Enable|TRUE

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdLpcClockDriveStrength|0x1

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSmbusSsid|0x790B1D94
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataAhciSsid|0x79011D94
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataRaidSsid|0x00000000
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataRaid5Ssid|0x00000000
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataIdeSsid|0x00000000
  #gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdEhciSsid|0x00000000
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXhciSsid|0x145C1D94
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdLpcSsid|0x790E1D94
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSdSsid|0x00000000

  gEfiAmdAgesaModulePkgTokenSpaceGuid.FchRTSataDevSlpPort0S5Pin|0x9

  # Disable CState
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdCStateMode|0
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdCStateIoBaseAddress|0

#  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdMemCfgBottomIo|0xC0

  # Platform RAS
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataRasSupport|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFchApuRasSmiSupport|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdNbioPoisonConsumption|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdNbioRASControl|TRUE

