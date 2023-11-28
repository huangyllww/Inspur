/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH routine definition
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
****************************************************************************
*/
#ifndef _FCH_DEF_H_
#define _FCH_DEF_H_

AGESA_STATUS  FchInitReset (IN FCH_RESET_DATA_BLOCK *FchParams);
AGESA_STATUS  FchInitEnv (IN FCH_DATA_BLOCK *FchDataBlock);
AGESA_STATUS  FchInitMid (IN FCH_DATA_BLOCK *FchDataBlock);
AGESA_STATUS  FchInitLate (IN FCH_DATA_BLOCK *FchDataBlock);

UINT32        ReadAlink (IN UINT32 Index, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          WriteAlink (IN UINT32 Index, IN UINT32 Data, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          RwAlink (IN UINT32 Index, IN UINT32 AndMask, IN UINT32 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ReadMem (IN UINT32 Address, IN UINT8 OpFlag, IN VOID *ValuePtr);
VOID          WriteMem (IN UINT32 Address, IN UINT8 OpFlag, IN VOID *ValuePtr);
VOID          RwMem (IN UINT32 Address, IN UINT8 OpFlag, IN UINT32 Mask, IN UINT32 Data);
VOID          ReadPci (IN UINT32 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          WritePci (IN UINT32 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          RwPci (IN UINT32 Address, IN UINT8 OpFlag, IN UINT32 Mask, IN UINT32  Data, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ProgramPciByteTable (IN REG8_MASK* pPciByteTable, IN UINT16 dwTableSize, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ProgramFchAcpiMmioTbl (IN ACPI_REG_WRITE  *pAcpiTbl, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ProgramFchSciMapTbl (IN SCI_MAP_CONTROL  *pSciMapTbl, IN FCH_RESET_DATA_BLOCK *FchResetDataBlock);
VOID          ProgramFchGpioTbl (IN GPIO_CONTROL  *pGpioTbl, IN FCH_RESET_DATA_BLOCK *FchResetDataBlock);
VOID          ProgramFchSataPhyTbl (IN SATA_PHY_CONTROL  *pSataPhyTbl, IN FCH_RESET_DATA_BLOCK *FchResetDataBlock);
VOID          GetChipSysMode (IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
BOOLEAN       IsImcEnabled (IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ReadPmio (IN UINT8 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          WritePmio (IN UINT8 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          RwPmio (IN UINT8 Address, IN UINT8 OpFlag, IN UINT32 AndMask, IN UINT32 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ReadPmio2 (IN UINT8 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          WritePmio2 (IN UINT8 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          RwPmio2 (IN UINT8 Address, IN UINT8  OpFlag, IN UINT32 AndMask, IN UINT32 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          ReadBiosram (IN UINT8 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          WriteBiosram (IN UINT8 Address, IN UINT8 OpFlag, IN VOID *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          GetFchAcpiMmioBase (OUT UINT32 *AcpiMmioBase, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          GetFchAcpiPmBase (OUT  UINT16 *AcpiPmBase, IN AMD_CONFIG_PARAMS *StdHeader);
UINT8         ReadFchSleepType (IN AMD_CONFIG_PARAMS *StdHeader);
UINT8         ReadFchChipsetRevision (IN AMD_CONFIG_PARAMS *StdHeader);
UINT32        ReadSocType (VOID);
UINT32        ReadSocDieBusNum (UINT8 Die);
BOOLEAN       CheckZP (VOID);

VOID          FchSmnRead (IN UINT32 IohcBus, IN UINT32 SmnAddress, IN UINT32 *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          FchSmnWrite (IN UINT32 IohcBus, IN UINT32 SmnAddress, IN UINT32 *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          FchSmnRW (IN UINT32 IohcBus, IN UINT32 SmnAddress, IN UINT32 AndMask, IN UINT32 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          FchSmnRead8 (IN UINT32 IohcBus, IN UINT32 SmnAddress, IN UINT8 *Value8, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          FchSmnWrite8 (IN UINT32 IohcBus, IN UINT32 SmnAddress, IN UINT8 *Value8, IN AMD_CONFIG_PARAMS *StdHeader);
VOID          FchSmnRW8 (IN UINT32 IohcBus, IN UINT32 SmnAddress, IN UINT8 AndMask, IN UINT8 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);

///
/// Fch Ab Routines
///
///  Pei Phase
///
VOID  FchInitResetAb (IN VOID* FchDataPtr);
VOID  FchProgramAbPowerOnReset (IN VOID* FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitEnvAb (IN VOID* FchDataPtr);
VOID  FchInitEnvAbSpecial (IN VOID* FchDataPtr);
VOID  FchInitMidAb (IN VOID* FchDataPtr);
VOID  FchInitLateAb (IN VOID* FchDataPtr);
///
///  Other Public Routines
///
VOID  FchInitEnvAbLinkInit (IN VOID* FchDataPtr);
VOID  FchAbLateProgram (IN VOID* FchDataPtr);

///
/// Fch Pcie Routines
///
///
///  Dxe Phase
///
VOID  ProgramPcieNativeMode (IN VOID* FchDataPtr);


///
/// Fch HwAcpi Routines
///
///  Pei Phase
///
VOID  FchInitResetHwAcpiP    (IN VOID  *FchDataPtr);
VOID  FchInitResetHwAcpi     (IN VOID  *FchDataPtr);
VOID  ProgramFchHwAcpiResetP (IN VOID  *FchDataPtr);
VOID  FchInitEnableWdt       (IN VOID  *FchDataPtr);
VOID  ProgramResetRtcExt     (IN VOID  *FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitEnvHwAcpiP                      (IN VOID  *FchDataPtr);
VOID  FchInitEnvHwAcpi                       (IN VOID  *FchDataPtr);
VOID  ProgramEnvPFchAcpiMmio                 (IN VOID *FchDataPtr);
VOID  ProgramFchEnvHwAcpiPciReg              (IN VOID *FchDataPtr);
VOID  ProgramSpecificFchInitEnvAcpiMmio      (IN VOID *FchDataPtr);
VOID  ProgramFchEnvSpreadSpectrum            (IN VOID *FchDataPtr);
VOID  PciIntVwInit                           (IN VOID *FchDataPtr);
VOID  FchInternalDeviceIrqInit               (IN VOID *FchDataPtr);
VOID  FchInitMidHwAcpi      (IN VOID  *FchDataPtr);
VOID  FchInitLateHwAcpi     (IN VOID  *FchDataPtr);

///
///  Other Public Routines
///
VOID HpetInit               (IN VOID  *FchDataPtr);
VOID MtC1eEnable            (IN VOID  *FchDataPtr);
VOID GcpuRelatedSetting     (IN VOID  *FchDataPtr);
VOID StressResetModeLate    (IN VOID  *FchDataPtr);
VOID FchEventInitUsbGpe     (IN VOID  *FchDataPtr);
VOID FchAl2ahbInit          (IN VOID  *FchDataPtr);
VOID FchI2cUartInit         (IN VOID  *FchDataPtr);
VOID FchI2cUartInitLate     (IN VOID  *FchDataPtr);

///
/// Fch SATA Routines
///
///  Pei Phase
///
VOID  FchInitResetSata          (IN VOID  *FchDataPtr);
VOID  FchInitResetSataProgram   (IN UINT32 DieBusNum, IN VOID  *FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitMidSata                   (IN VOID  *FchDataPtr);
VOID  FchInitEnvSata                   (IN VOID  *FchDataPtr);
VOID  FchInitEnvProgramSata            (IN UINT32 DieBusNum, IN VOID  *FchDataPtr);
VOID  FchInitMidProgramSataRegs        (IN UINT32 DieBusNum, IN VOID  *FchDataPtr);
VOID  FchInitLateProgramSataRegs       (IN UINT32 DieBusNum, IN VOID  *FchDataPtr);

VOID  FchInitLateSata           (IN VOID  *FchDataPtr);
VOID  FchInitEnvSataAhci        (IN VOID  *FchDataPtr);
VOID  FchInitMidSataAhci        (IN UINT32 DieBusNum, IN VOID  *FchDataPtr);
VOID  FchInitLateSataAhci       (IN UINT32 DieBusNum, IN VOID  *FchDataPtr);
VOID  FchInitEnvSataRaid        (IN VOID  *FchDataPtr);
VOID  FchInitMidSataRaid        (IN UINT32 DieBusNum, IN VOID  *FchDataPtr);
VOID  FchInitLateSataRaid       (IN UINT32 DieBusNum, IN VOID  *FchDataPtr);

VOID  SataAhciSetDeviceNumMsi     (IN UINT32 DieBusNum, IN VOID  *FchDataPtr);
VOID  SataRaidSetDeviceNumMsi     (IN UINT32 DieBusNum, IN VOID  *FchDataPtr);
VOID  SataSetIrqIntResource (IN VOID  *FchDataPtr, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  SataBar5setting (IN VOID *FchDataPtr, IN UINT32 *Bar5Ptr);
VOID  SataEnableWriteAccess (IN AMD_CONFIG_PARAMS *StdHeader);
VOID  SataDisableWriteAccess (IN AMD_CONFIG_PARAMS *StdHeader);
VOID  SataSetDeviceNumMsi (IN UINT32 DieBusNum, IN VOID  *FchDataPtr);
VOID  FchSataSetDeviceNumMsi (IN UINT32 DieBusNum, IN VOID  *FchDataPtr);
VOID  ShutdownUnconnectedSataPortClock (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID  SataDriveDetection (IN VOID *FchDataPtr, IN UINT32 *Bar5Ptr);
VOID  FchSataDriveDetection (IN VOID *FchDataPtr, IN UINT32 *Bar5Ptr);
VOID  SataBar5RegSet (IN UINT32 DieBusNum, IN  VOID  *FchDataPtr);
VOID  SataSetPortGenMode (IN UINT32 DieBusNum, IN  VOID  *FchDataPtr);
VOID  FchInitEnvSataRaidProgram  (IN VOID  *FchDataPtr);

UINT32 SataGetBusCNumber (IN UINT32 DieBusNum, IN AMD_CONFIG_PARAMS *StdHeader);
UINT32 SataSetBusCTempNumber (IN AMD_CONFIG_PARAMS *StdHeader);
UINT32 SataEnableBusCMmioDecode (IN AMD_CONFIG_PARAMS *StdHeader);
VOID  SataRestoreBusCMmioDecode (IN UINT32 SavedCmdReg, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  SataBar5settingTS (IN VOID *FchDataPtr, IN UINT32 SataBusNum, IN UINT32 *Bar5Ptr);
VOID  SataEnableWriteAccessTS (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID  SataDisableWriteAccessTS (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID  FchInitEnvSataAhciTS (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID  FchInitEnvSataRaidTS (IN UINT32 DieBusNum, IN VOID *FchDataPtr);

VOID  FchTSSataInitPortOffline (IN UINT32 DieBusNum, IN UINT32 PortNum, IN VOID *FchDataPtr);
VOID  FchTSSataInitPortActive  (IN UINT32 DieBusNum, IN UINT32 PortNum, IN VOID *FchDataPtr);
VOID  FchTSSataInitEnableErr (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID  FchTSSataInitEsata (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID  FchTSSataInitDevSlp (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID  FchTSSataInitMpssMap (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID  FchTSSataInitRsmuCtrl (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID  FchTSSataInitCtrlReg (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID  FchTSSataInitMMC (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID  FchTSSataSetPortGenMode (IN UINT32 DieBusNum, IN  VOID  *FchDataPtr);
VOID  FchTSSataShutdownUnconnectedSataPortClock (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID  FchTSSataGpioInitial (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
///
/// FCH USB Controller Public Function
///
///  Pei Phase
///
VOID  FchInitResetUsb            (IN VOID  *FchDataPtr);
VOID  FchInitResetXhci           (IN VOID  *FchDataPtr);
VOID  FchInitResetXhciProgram    (IN VOID  *FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitEnvUsb              (IN VOID  *FchDataPtr);
VOID  FchInitMidUsb              (IN VOID  *FchDataPtr);
VOID  FchInitLateUsb             (IN VOID  *FchDataPtr);
VOID  FchInitEnvUsbXhci          (IN VOID  *FchDataPtr);
VOID  FchInitMidUsbXhci          (IN VOID  *FchDataPtr);
VOID  FchInitLateUsbXhci         (IN VOID  *FchDataPtr);
///
///  Other Public Routines
///
VOID FchTSXhciInitBootProgram    (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID FchTSXhciInitS3ExitProgram  (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID FchTSXhciInitS3EntryProgram (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID FchTSXhciProgramInternalRegStepOne (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID FchTSXhciProgramInternalRegStepTwo (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
VOID FchTSXhciInitSsid (IN UINT32 DieBusNum, IN UINT32 Ssid);
BOOLEAN FchTSXhciDisablePort     (IN UINT32 DieBusNum, IN UINT32 USB3DisableMap, IN UINT32 USB2DisableMap);
BOOLEAN FchTSXhciDisablePortMCM  (IN UINT8 Socket, IN UINT32 USB3DisableMap, IN UINT32 USB2DisableMap);
BOOLEAN FchTSXhciDisablePortSp3R2  (IN UINT8 Socket, IN UINT32 USB3DisableMap, IN UINT32 USB2DisableMap);
BOOLEAN FchXhciDisablePortSoc    (IN UINT8 Socket, IN UINT32 USB3DisableMap, IN UINT32 USB2DisableMap);
BOOLEAN FchTSXhciOCmapping       (IN UINT32 DieBusNum, IN UINT8 Port, IN UINT8 OCPin);
BOOLEAN FchTSXhciOCmappingMCM    (IN UINT8 Socket, IN UINT8 Port, IN UINT8 OCPin);
BOOLEAN FchTSXhciOCmappingSp3R2  (IN UINT8 Socket, IN UINT8 Port, IN UINT8 OCPin);
BOOLEAN FchXhciOCmappingSoc      (IN UINT8 Socket, IN UINT8 Port, IN UINT8 OCPin);

///
/// Fch Sd Routines
///
VOID  FchInitEnvSd  (IN VOID  *FchDataPtr);
VOID  FchInitMidSd  (IN VOID  *FchDataPtr);
VOID  FchInitLateSd (IN VOID  *FchDataPtr);

///
///  Other Public Routines
///

VOID FchInitEnvSdProgram (IN VOID  *FchDataPtr);

///
/// Fch Spi Routines
///
///  Pei Phase
///
VOID  FchInitResetSpi        (IN VOID  *FchDataPtr);
VOID  FchInitResetLpc        (IN VOID  *FchDataPtr);
VOID  FchInitResetLpcProgram (IN VOID  *FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitEnvSpi          (IN VOID  *FchDataPtr);
VOID  FchInitMidSpi          (IN VOID  *FchDataPtr);
VOID  FchInitLateSpi         (IN VOID  *FchDataPtr);
VOID  FchInitEnvLpc          (IN VOID  *FchDataPtr);
VOID  FchInitMidLpc          (IN VOID  *FchDataPtr);
VOID  FchInitLateLpc         (IN VOID  *FchDataPtr);
VOID  FchInitEnvLpcProgram   (IN VOID  *FchDataPtr);
///
///  Other Public Routines
///
VOID  FchSpiUnlock       (IN VOID  *FchDataPtr);
VOID  FchSpiLock         (IN VOID  *FchDataPtr);

///
/// Fch ESPI Routines
///
///
VOID  FchInitResetEspi   (IN VOID  *FchDataPtr);

VOID  FchinitTSEspiEnableKbc6064 (IN BOOLEAN  Enable);
VOID  FchinitTSEspiTimer (IN UINT32 EspiBase);
VOID  FchinitTSEspiIoMux ();
VOID  FchinitTSEspiEc0 (IN VOID  *FchDataPtr);
UINT32 getESPIBase ();


///
/// Fch eMMC Routines
///
///  Pei Phase
///
VOID  FchInitResetEmmc (IN VOID* FchDataPtr);
///
///  Dxe Phase
///
VOID  FchInitEnvEmmc (IN VOID* FchDataPtr);
VOID  FchInitMidEmmc (IN VOID* FchDataPtr);
VOID  FchInitLateEmmc (IN VOID* FchDataPtr);

///
/// Fch XgbE Routines
///
///  Pei Phase
///

///
///  Dxe Phase
///
VOID  FchInitEnvXGbe (IN VOID* FchDataPtr);

VOID  FchTsXgbePortClockGate (IN UINT32 DieBusNum, IN UINT8 Port);
VOID  FchTsXgbePortPlatformConfig (IN UINT32 DieBusNum, IN UINT8 PortNum, IN UINT32 SystemPort, IN VOID *PortData);
VOID  FchTsXgbePortMacAddress (IN UINT32 DieBusNum, IN UINT8 PortNum, IN VOID *MacData);
VOID  FchTsXgbePortPtpInit (IN UINT32 DieBusNum, IN UINT8 PortNum, IN VOID *PortData);
VOID  FchTsXgbePadEnable (IN UINT32 DieBusNum, IN VOID *FchDataPtr);
/*--------------------------- Documentation Pages ---------------------------*/
VOID  FchStall (IN UINT32 uSec, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  CimFchStall (IN UINT32 uSec, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  FchPciReset (IN AMD_CONFIG_PARAMS *StdHeader);
VOID  OutPort80 (IN UINT32 pcode, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  OutPort1080 (IN UINT32 pcode, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  GetEfuseStatus (IN VOID* Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  TurnOffCG2 (OUT VOID);
VOID  BackUpCG2 (OUT VOID);
VOID  FchCopyMem (IN VOID* pDest, IN VOID* pSource, IN UINTN Length);
VOID* GetRomSigPtr (IN UINTN* RomSigPtr, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  RwXhciIndReg (IN UINT32 Index, IN UINT32 AndMask, IN UINT32 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  RwXhci0IndReg (IN UINT32 Index, IN UINT32 AndMask, IN UINT32 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  RwXhci1IndReg (IN UINT32 Index, IN UINT32 AndMask, IN UINT32 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  ReadXhci0Phy (IN UINT32 Port, IN UINT32 Address, IN UINT32 *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  ReadXhci1Phy (IN UINT32 Port, IN UINT32 Address, IN UINT32 *Value, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  RwSsicIndReg (IN UINT32 Index, IN UINT32 AndMask, IN UINT32 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);
VOID  AcLossControl (IN UINT8 AcLossControlValue);
VOID  FchVgaInit (OUT VOID);
VOID  RecordFchConfigPtr (IN UINT32 FchConfigPtr);
VOID  ValidateFchVariant (IN VOID  *FchDataPtr);
VOID  RecordSmiStatus (IN AMD_CONFIG_PARAMS *StdHeader);
VOID  ClearAllSmiStatus (IN AMD_CONFIG_PARAMS *StdHeader);
BOOLEAN  IsExternalClockMode (IN VOID  *FchDataPtr);
VOID  SbSleepTrapControl (IN BOOLEAN SleepTrap);

AGESA_STATUS
FchSpiTransfer (
  IN       UINT8    PrefixCode,
  IN       UINT8    Opcode,
  IN  OUT  UINT8    *DataPtr,
  IN       UINT8    *AddressPtr,
  IN       UINT8    Length,
  IN       BOOLEAN  WriteFlag,
  IN       BOOLEAN  AddressFlag,
  IN       BOOLEAN  DataFlag,
  IN       BOOLEAN  FinishedFlag
  );

BOOLEAN
FchConfigureSpiDeviceDummyCycle (
  IN       UINT32     DeviceID,
  IN       UINT8      SpiMode
  );

UINT32
FchReadSpiId (
  IN       BOOLEAN    Flag
  );

BOOLEAN
FchPlatformSpiQe (
  IN       VOID     *FchDataPtr
  );


BOOLEAN
FchCheckZPB2 (
  );

VOID TurnOffUsbPme (IN UINT32 DieBusNum, IN VOID *FchDataPtr); //DM1-271
#endif

