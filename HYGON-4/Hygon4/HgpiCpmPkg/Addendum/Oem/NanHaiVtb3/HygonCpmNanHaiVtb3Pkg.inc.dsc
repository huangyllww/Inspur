#;*****************************************************************************
#;
#; 
#; Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
#; 
#;*****************************************************************************

[Defines]
 DEFINE HGPI_PKG_PATH   = HgpiModulePkg
 DEFINE HGPI_PKG_DEC    = HgpiCommonModulePkg
 DEFINE FCH_PKG_PATH     = $(HGPI_PKG_PATH)/Fch/Taishan
 DEFINE FCH_PKG_DEC      = FchTaishan

[LibraryClasses]
  #
  # Cpm specific common libraries
  #
  HygonCpmBaseLib|HgpiCpmPkg/Library/Proc/Base/HygonCpmBaseLib.inf
  HygonCpmCpuLib|HgpiCpmPkg/Library/Proc/Cpu/HygonCpmCpu.inf
  HygonCpmFchLib|HgpiCpmPkg/Library/Proc/Fch/HygonCpmFch.inf
  RtcIntrDetLib|HgpiCpmPkg/Library/RtcIntrDetLib/RtcIntrDetLib.inf
  
[Components.IA32]
  HgpiCpmPkg/Addendum/Oem/NanHaiVtb3/Pei/HygonCpmOemInitPei/HygonCpmOemInitPeimNanHaiVtb3.inf
  HgpiCpmPkg/Kernel/Pei/HygonCpmInitPeim.inf
  HgpiCpmPkg/Features/ProjectInfo/Pei/HygonProjectInfoPei.inf
  HgpiCpmPkg/Features/GpioInit/Pei/HygonCpmGpioInitPeim.inf
  HgpiCpmPkg/Features/I2cMaster/Pei/HygonI2cMasterPei.inf
  HgpiCpmPkg/Features/I3cMaster/Pei/HygonI3cMasterPei.inf
  HgpiCpmPkg/Features/RtcIntrDet/Pei/HygonRtcIntrDetPei.inf
  HgpiCpmPkg/Features/PcieInit/Pei/HygonCpmPcieInitPeim.inf
  HgpiCpmPkg/Features/EqTable/Pei/EqTable.inf
  HgpiCpmPkg/Features/SataEq/Pei/SataEqPei.inf
  HgpiCpmPkg/Features/UsbEq/Pei/UsbEqPei.inf
  HgpiCpmPkg/Features/CodecInit/Pei/HygonCpmCodecInitPeim.inf
  HgpiCpmPkg/Devices/Pca9555a/Pei/Pca9555aPei.inf
  HgpiCpmPkg/Devices/Pca9545a/Pei/Pca9545aPei.inf
  HgpiCpmPkg/Devices/At24c08d/Pei/At24c08dPei.inf

[Components.X64]
#  HgpiCpmPkg/Addendum/Oem/NanHaiVtb3/Dxe/HygonXgbeWorkaround/HygonXgbeWorkaround.inf
  HgpiCpmPkg/Addendum/Oem/NanHaiVtb3/Dxe/PspPlatformDriver/PspPlatform.inf
  HgpiCpmPkg/Kernel/Asl/HygonCpmInitAsl.inf
  HgpiCpmPkg/Kernel/Dxe/HygonCpmInitDxe.inf
  HgpiCpmPkg/Kernel/Smm/HygonCpmInitSmm.inf
  HgpiCpmPkg/Features/ProjectInfo/Dxe/HygonProjectInfoDxe.inf
  HgpiCpmPkg/Features/PcieInit/Asl/HygonCpmPcieInitAsl.inf
  HgpiCpmPkg/Features/PcieInit/Dxe/HygonCpmPcieInitDxe.inf
  HgpiCpmPkg/Features/I2cMaster/Dxe/HygonI2cMasterDxe.inf
  HgpiCpmPkg/Features/I3cMaster/Dxe/HygonI3cMasterDxe.inf
  HgpiCpmPkg/Features/RtcIntrDet/Dxe/HygonRtcIntrDetDxe.inf
  HgpiCpmPkg/Features/GpioInit/Dxe/HygonCpmGpioInitDxe.inf
  HgpiCpmPkg/Features/GpioInit/Smm/HygonCpmGpioInitSmm.inf
  HgpiCpmPkg/Features/PlatformRas/Dxe/HygonPlatformRasDxe.inf
  HgpiCpmPkg/Features/PlatformRas/Smm/HygonPlatformRasSmm.inf
  HgpiCpmPkg/Devices/Pca9555a/Dxe/Pca9555aDxe.inf
  HgpiCpmPkg/Devices/Pca9545a/Dxe/Pca9545aDxe.inf
  HgpiCpmPkg/Devices/GenericCxl/Dxe/CxlDeviceDriver.inf
  HgpiCpmPkg/Addendum/Oem/NanHaiVtb3/Dxe/ServerHotplugDxe/ServerHotplugDxe.inf

[PcdsFixedAtBuild]
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonNumberOfPhysicalSocket|1

  # V9 Pcds
  # IDS Debug Lib Pcds
# gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonIdsDebugPrintEnable|TRUE        # byo230914 -
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonIdsDebugPrintHdtOutEnable|FALSE
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonIdsDebugPrintHdtOutForceEnable|FALSE
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonIdsDebugPrintRedirectIOEnable|FALSE
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonIdsDebugPrintSerialPortEnable|TRUE
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonIdsDebugPrintSerialPort|0xfedc9000
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonIdsDebugPrintFilter|0x100401008A300408

[PcdsDynamicDefault]
  ## Customize HGPI GNB
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdCfgGnbIoapicAddress|0xFEC01000

  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdCfgAzaliaSsid|0xC9501D94

  ## Customize HGPI FCH
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdLegacyFree|FALSE

  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdHpetEnable|TRUE
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdHpetMsiDis|FALSE

##  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdOemProgrammingTablePtr|{ \
##  0x00, 0x00, 0xB0, 0xAC, \
##  0x02, 0x48, 0x00, 0x08, \
##  0x02, 0x78, 0x00, 0x19, \
##  0x02, 0x79, 0x00, 0x19, \
##  0x02, 0x58, 0x00, 0x18, \
##  0x02, 0x59, 0x00, 0x18, \
##  0x02, 0x5A, 0x00, 0x18, \
##  0x02, 0x70, 0x00, 0x01, \
##  0xFF, 0xFF, 0xFF, 0xFF }
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdOemProgrammingTablePtr|{0x00, 0x00, 0xB0, 0xAC,  0x02, 0x48, 0x00, 0x08,  0x02, 0x78, 0x00, 0x19,  0x02, 0x79, 0x00, 0x19,  0x02, 0x58, 0x00, 0x18,  0x02, 0x59, 0x00, 0x18,  0x02, 0x5A, 0x00, 0x18,  0x02, 0x70, 0x00, 0x01,  0xFF, 0xFF, 0xFF, 0xFF}

  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdNativePcieSupport|TRUE
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdNoneSioKbcSupport|TRUE

  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSataEnableHyGx|0xFFFFFFFF
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSataClass|0x06

  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdXhci0EnableHyEx|TRUE
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdXhciEnableHyGx|0xFFFFFFFFFFFFFFFF

  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdLpcClockDriveStrength|0x1

  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSmbusSsid|0x00000000
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSataAhciSsid|0x00000000
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSataRaidSsid|0x00000000
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSataRaid5Ssid|0x00000000
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSataIdeSsid|0x00000000
  #gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdEhciSsid|0x00000000
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdXhciSsid|0x00000000
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdLpcSsid|0x00000000
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSdSsid|0x00000000

  #gEfiHygonHgpiPkgTokenSpaceGuid.PcdFchRTDeviceD3ColdMap|0
  # gEfiHygonHgpiPkgTokenSpaceGuid.PcdFchRTSataDevSlpPort0S5Pin|0x9

  # Disable CState
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonCStateMode|0
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonCStateIoBaseAddress|0

  # gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdHygonMemCfgBottomIo|0xC0

  # Platform RAS
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdSataRasSupport|TRUE
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonFchApuRasSmiSupport|TRUE
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonNbioPoisonConsumption|TRUE
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHygonNbioRASControl|TRUE
  gEfiHygonHgpiPkgTokenSpaceGuid.PcdHpcbToBoardChanXLatTab|{0x0,0x0,0x1,0x1,0x2,0x2,0x3,0x3,0x4,0x4,0x5,0x5,0x6,0x6,0x7,0x7,0x8,0x8,0x9,0x9,0xA,0xA,0xB,0xB,0xFF,0xFF}
  
  # USB feature configuration
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdDieNumUsbPortAndOCPinMapHyEx|{0x0,0x0,0x0,0x0, 0x0,0x0,0x4,0x0, 0x0,0x0,0x1,0x1, 0x0,0x0,0x5,0x1, 0x0,0x0,0x2,0x2, 0x0,0x0,0x6,0x2, 0x0,0x0,0x3,0x3, 0x0,0x1,0x2,0x2, 0x0,0x1,0x3,0x3, 0xFF,0xFF,0xFF,0xFF}
  gEfiHygonHgpiModulePkgTokenSpaceGuid.PcdDieNumUsbPortAndOCPinMapHyGx|{0xFF}

