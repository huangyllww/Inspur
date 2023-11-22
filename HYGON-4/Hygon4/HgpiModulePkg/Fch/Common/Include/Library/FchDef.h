/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH routine definition
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: FCH
 *
 */
/*
*****************************************************************************
*
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 *
****************************************************************************
*/

#ifndef _FCH_DEF_H_
#define _FCH_DEF_H_
#include <HYGON.h>
#include <Library/AcpiLib.h>

///
/// - Byte Register R/W structure
///
typedef struct _REG8_MASK {
  UINT8    RegIndex;                                    /// RegIndex - Reserved
  UINT8    AndMask;                                     /// AndMask - Reserved
  UINT8    OrMask;                                      /// OrMask - Reserved
} REG8_MASK;

UINT32
ReadAlink (
  IN UINT32 Index,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
WriteAlink (
  IN UINT32            Index,
  IN UINT32            Data,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
RwAlink (
  IN UINT32            Index,
  IN UINT32            AndMask,
  IN UINT32            OrMask,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
ReadMem (
  IN UINT32            Address,
  IN UINT8             OpFlag,
  IN VOID              *ValuePtr
  );

VOID
WriteMem (
  IN UINT32            Address,
  IN UINT8             OpFlag,
  IN VOID              *ValuePtr
  );

VOID
RwMem (
  IN UINT32            Address,
  IN UINT8             OpFlag,
  IN UINT32            Mask,
  IN UINT32            Data
  );

VOID
ReadPci (
  IN UINT32            Address,
  IN UINT8             OpFlag,
  IN VOID              *Value,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
WritePci (
  IN UINT32            Address,
  IN UINT8             OpFlag,
  IN VOID              *Value,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
RwPci (
  IN UINT32            Address,
  IN UINT8             OpFlag,
  IN UINT32            Mask,
  IN UINT32            Data,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
ProgramPciByteTable (
  IN REG8_MASK         *pPciByteTable,
  IN UINT16            dwTableSize,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
ProgramFchAcpiMmioTbl (
  IN ACPI_REG_WRITE    *pAcpiTbl,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
GetChipSysMode (
  IN VOID              *Value,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
IsImcEnabled (
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
ReadPmio (
  IN UINT8             Address,
  IN UINT8             OpFlag,
  IN VOID              *Value,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
WritePmio (
  IN UINT8             Address,
  IN UINT8             OpFlag,
  IN VOID              *Value,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
RwPmio (
  IN UINT8              Address,
  IN UINT8              OpFlag,
  IN UINT32             AndMask,
  IN UINT32             OrMask,
  IN HYGON_CONFIG_PARAMS  *StdHeader
  );

VOID
ReadPmio2 (
  IN UINT8              Address,
  IN UINT8              OpFlag,
  IN VOID               *Value,
  IN HYGON_CONFIG_PARAMS  *StdHeader
  );

VOID
WritePmio2 (
  IN UINT8              Address,
  IN UINT8              OpFlag,
  IN VOID               *Value,
  IN HYGON_CONFIG_PARAMS  *StdHeader
  );

VOID
RwPmio2 (
  IN UINT8              Address,
  IN UINT8              OpFlag,
  IN UINT32             AndMask,
  IN UINT32             OrMask,
  IN HYGON_CONFIG_PARAMS  *StdHeader
  );

VOID
ReadBiosram (
  IN UINT8              Address,
  IN UINT8              OpFlag,
  IN VOID               *Value,
  IN HYGON_CONFIG_PARAMS  *StdHeader
  );

VOID
WriteBiosram (
  IN UINT8              Address,
  IN UINT8              OpFlag,
  IN VOID               *Value,
  IN HYGON_CONFIG_PARAMS  *StdHeader
  );

VOID
GetFchAcpiMmioBase (
  OUT UINT32            *AcpiMmioBase,
  IN HYGON_CONFIG_PARAMS  *StdHeader
  );

VOID
GetFchAcpiPmBase (
  OUT  UINT16            *AcpiPmBase,
  IN HYGON_CONFIG_PARAMS   *StdHeader
  );

UINT8
ReadFchSleepType (
  IN HYGON_CONFIG_PARAMS   *StdHeader
  );

UINT8
ReadFchChipsetRevision (
  IN HYGON_CONFIG_PARAMS   *StdHeader
  );

UINT32
ReadSocType (
  VOID
  );

UINT32
ReadSocDieBusNum (
  IN UINTN  SocketId,
  IN UINTN  LogicalDieId,
  IN UINTN  RbId
  );

BOOLEAN
CheckHygonSoc (
  VOID
  );

VOID
FchSmnRead (
  IN UINT32             IohcBus,
  IN UINT32             SmnAddress,
  IN UINT32             *Value,
  IN HYGON_CONFIG_PARAMS  *StdHeader
  );

VOID
FchSmnWrite (
  IN UINT32              IohcBus,
  IN UINT32              SmnAddress,
  IN UINT32              *Value,
  IN HYGON_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSmnRW (
  IN UINT32              IohcBus,
  IN UINT32              SmnAddress,
  IN UINT32              AndMask,
  IN UINT32              OrMask,
  IN HYGON_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSmnRead8 (
  IN UINT32              IohcBus,
  IN UINT32              SmnAddress,
  IN UINT8               *Value8,
  IN HYGON_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSmnWrite8 (
  IN UINT32              IohcBus,
  IN UINT32              SmnAddress,
  IN UINT8               *Value8,
  IN HYGON_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSmnRW8 (
  IN UINT32              IohcBus,
  IN UINT32              SmnAddress,
  IN UINT8               AndMask,
  IN UINT8               OrMask,
  IN HYGON_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSmnRead16 (
  IN UINT32              IohcBus,
  IN UINT32              SmnAddress,
  IN UINT16              *Value16,
  IN HYGON_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSmnWrite16 (
  IN UINT32              IohcBus,
  IN UINT32              SmnAddress,
  IN UINT16              *Value16,
  IN HYGON_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSmnRW16 (
  IN UINT32              IohcBus,
  IN UINT32              SmnAddress,
  IN UINT16              AndMask,
  IN UINT16              OrMask,
  IN HYGON_CONFIG_PARAMS   *StdHeader
  );

///
/// Fch Ab Routines
///
/// Pei Phase
///

VOID
FchInitResetAb (
  IN VOID *FchDataPtr
  );

VOID
FchProgramAbPowerOnReset (
  IN VOID *FchDataPtr
  );

///
/// Dxe Phase
///

VOID
FchInitEnvAb (
  IN VOID *FchDataPtr
  );

VOID
FchInitEnvAbSpecial (
  IN VOID *FchDataPtr
  );

VOID
FchInitMidAb (
  IN VOID *FchDataPtr
  );

VOID
FchInitLateAb (
  IN VOID *FchDataPtr
  );

///
/// Other Public Routines
///

VOID
FchInitEnvAbLinkInit (
  IN VOID *FchDataPtr
  );

VOID
FchAbLateProgram (
  IN VOID *FchDataPtr
  );

///
/// Fch Pcie Routines
///
///
/// Dxe Phase
///
VOID
ProgramPcieNativeMode (
  IN VOID *FchDataPtr
  );

///
/// Fch HwAcpi Routines
///
/// Pei Phase
///
VOID
FchInitResetHwAcpiP (
  IN VOID  *FchDataPtr
  );

VOID
FchInitResetHwAcpi (
  IN VOID  *FchDataPtr
  );

VOID
ProgramFchHwAcpiResetP (
  IN VOID  *FchDataPtr
  );

VOID
FchInitEnableWdt (
  IN VOID  *FchDataPtr
  );

VOID
ProgramResetRtcExt (
  IN VOID  *FchDataPtr
  );

///
/// Dxe Phase
///
VOID
FchInitEnvHwAcpiP (
  IN VOID  *FchDataPtr
  );

VOID
FchInitEnvHwAcpi (
  IN VOID  *FchDataPtr
  );

VOID
ProgramEnvPFchAcpiMmio (
  IN VOID *FchDataPtr
  );

VOID
ProgramFchEnvHwAcpiPciReg (
  IN VOID *FchDataPtr
  );

VOID
ProgramSpecificFchInitEnvAcpiMmio (
  IN VOID *FchDataPtr
  );

VOID
ProgramFchEnvSpreadSpectrum (
  IN VOID *FchDataPtr
  );

VOID
PciIntVwInit (
  IN VOID *FchDataPtr
  );

VOID
FchInternalDeviceIrqInit (
  IN VOID *FchDataPtr
  );

VOID
FchInitMidHwAcpi (
  IN VOID  *FchDataPtr
  );

VOID
FchInitLateHwAcpi (
  IN VOID  *FchDataPtr
  );

///
/// Other Public Routines
///
VOID
HpetInit (
  IN VOID  *FchDataPtr
  );

VOID
MtC1eEnable (
  IN VOID  *FchDataPtr
  );

VOID
GcpuRelatedSetting (
  IN VOID  *FchDataPtr
  );

VOID
StressResetModeLate (
  IN VOID  *FchDataPtr
  );

VOID
FchEventInitUsbGpe (
  IN VOID  *FchDataPtr
  );

VOID
FchAl2ahbInit (
  IN VOID  *FchDataPtr
  );

VOID
FchI2cUartInit (
  IN VOID  *FchDataPtr
  );

VOID
FchI2cUartInitLate (
  IN VOID  *FchDataPtr
  );

///
/// Fch SATA Routines
///
/// Pei Phase
///
VOID
FchInitResetSata (
  IN VOID  *FchDataPtr
  );

VOID
FchInitResetSataProgram (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

///
/// Dxe Phase
///
VOID
FchInitEnvSata (
  IN VOID  *FchDataPtr
  );

VOID
FchInitLateSata (
  IN VOID  *FchDataPtr
  );

VOID
FchInitEnvProgramSata (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchInitMidProgramSataRegs (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchInitLateProgramSataRegs (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchInitEnvSataAhci (
  IN VOID         *FchDataPtr
  );

VOID
FchInitEnvSataRaid (
  IN VOID         *FchDataPtr
  );

VOID
SataAhciSetDeviceNumMsi (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
SataRaidSetDeviceNumMsi (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
SataSetIrqIntResource (
  IN  VOID                 *FchDataPtr,
  IN  HYGON_CONFIG_PARAMS    *StdHeader
  );

VOID
SataSetDeviceNumMsi (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchSataSetDeviceNumMsi (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchInitEnvSataRaidProgram (
  IN VOID         *FchDataPtr
  );

VOID
FchTSSataInitPortOffline (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  UINT32   PortNum,
  IN  VOID     *FchDataPtr
  );

///
/// FCH USB Controller Public Function
///
/// Pei Phase
///
VOID
FchInitResetUsb (
  IN VOID  *FchDataPtr
  );

VOID
FchInitResetXhci (
  IN VOID  *FchDataPtr
  );

VOID
FchInitResetXhciProgram (
  IN VOID  *FchDataPtr
  );

///
/// Dxe Phase
///
VOID
FchInitEnvUsb (
  IN VOID  *FchDataPtr
  );

VOID
FchInitMidUsb (
  IN VOID  *FchDataPtr
  );

VOID
FchInitLateUsb (
  IN VOID  *FchDataPtr
  );

VOID
FchInitEnvUsbXhci (
  IN VOID  *FchDataPtr
  );

VOID
FchInitMidUsbXhci (
  IN VOID  *FchDataPtr
  );

VOID
FchInitLateUsbXhci (
  IN VOID  *FchDataPtr
  );

///
/// Other Public Routines
///
VOID
FchTSXhciInitBootProgram (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchTSXhciInitS3ExitProgram (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchTSXhciInitS3EntryProgram (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchTSXhciProgramInternalRegStepOne (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchTSXhciGen31ProgramInternalRegStepTwo (
  IN UINT8 PhysicalDieId,
  IN UINT8 UsbIndex,
  IN UINT32 DieBusNum,
  IN VOID *FchDataPtr
  );

VOID
FchTSXhciInitSsid (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  UINT32   Ssid
  );

BOOLEAN
FchTSXhciDisablePort (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   USB3DisableMap,
  IN  UINT32   USB2DisableMap
  );

BOOLEAN
FchTSXhciDisablePortMCM (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   USB3DisableMap,
  IN  UINT32   USB2DisableMap
  );

BOOLEAN
FchXhciDisablePortSoc (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   USB3DisableMap,
  IN  UINT32   USB2DisableMap
  );

BOOLEAN
FchTSXhciOCmapping (
  IN  UINT8    Socket,
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT8    Port,
  IN  UINT8    OCPin
  );

BOOLEAN
FchXhciOCmappingSoc (
  IN UINT8        Socket,
  IN UINT8        PhysicalDieId,
  IN UINT8        UsbIndex,
  IN UINT8        Port,
  IN UINT8        OCPin
  );

///
/// Fch Sd Routines
///
VOID
FchInitEnvSd  (
  IN VOID  *FchDataPtr
  );

///
/// Other Public Routines
///

VOID
FchInitEnvSdProgram (
  IN VOID  *FchDataPtr
  );

///
/// Fch Spi Routines
///
/// Pei Phase
///
VOID
FchInitResetSpi (
  IN VOID  *FchDataPtr
  );

VOID
FchInitResetLpc (
  IN VOID  *FchDataPtr
  );

VOID
FchInitResetLpcProgram (
  IN VOID  *FchDataPtr
  );

///
/// Dxe Phase
///
VOID
FchInitEnvSpi (
  IN VOID  *FchDataPtr
  );

VOID
FchInitMidSpi (
  IN VOID  *FchDataPtr
  );

VOID
FchInitLateSpi (
  IN VOID  *FchDataPtr
  );

VOID
FchInitEnvLpc (
  IN VOID  *FchDataPtr
  );

VOID
FchInitMidLpc (
  IN VOID  *FchDataPtr
  );

VOID
FchInitLateLpc (
  IN VOID  *FchDataPtr
  );

VOID
FchInitEnvLpcProgram (
  IN VOID  *FchDataPtr
  );

///
/// Other Public Routines
///
VOID
FchSpiUnlock (
  IN VOID  *FchDataPtr
  );

VOID
FchSpiLock (
  IN VOID  *FchDataPtr
  );

///
/// Fch ESPI Routines
///
///
VOID
FchInitResetEspi (
  IN VOID  *FchDataPtr
  );

VOID
FchinitTSEspiEnableKbc6064 (
  IN BOOLEAN  Enable
  );

VOID
FchinitTSEspiTimer (
  IN UINT32 EspiBase
  );

VOID
FchinitTSEspiIoMux (
  VOID
  );

VOID
EspiDisableLpcDecoding (
  VOID
  );

VOID
FchinitTSEspiBmc (
  IN VOID  *FchDataPtr
  );

UINT32
getESPIBase (
  );

///
/// Fch eMMC Routines
///
/// Pei Phase
///
VOID
FchInitResetEmmc (
  IN VOID *FchDataPtr
  );

///
/// Dxe Phase
///
VOID
FchInitEnvEmmc (
  IN VOID *FchDataPtr
  );

VOID
FchInitMidEmmc (
  IN VOID *FchDataPtr
  );

VOID
FchInitLateEmmc (
  IN VOID *FchDataPtr
  );

///
/// Fch XgbE Routines
///
/// Pei Phase
///

///
/// Dxe Phase
///
VOID
FchInitEnvXGbe (
  IN VOID *FchDataPtr
  );

VOID
FchTsXgbePadEnable (
  IN UINT32       DieBusNum,
  IN VOID         *FchDataPtr
  );

/*--------------------------- Documentation Pages ---------------------------*/
VOID
FchStall (
  IN UINT32            uSec,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
CimFchStall (
  IN UINT32            uSec,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
FchPciReset (
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
OutPort80 (
  IN UINT32            pcode,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
OutPort1080 (
  IN UINT32            pcode,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
GetEfuseStatus (
  IN VOID              *Value,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
TurnOffCG2 (
  OUT VOID
  );

VOID
BackUpCG2 (
  OUT VOID
  );

VOID
FchCopyMem (
  IN VOID              *pDest,
  IN VOID              *pSource,
  IN UINTN             Length
  );

VOID *
GetRomSigPtr (
  IN UINTN             *RomSigPtr,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
RwSsicIndReg (
  IN UINT32            Index,
  IN UINT32            AndMask,
  IN UINT32            OrMask,
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
AcLossControl (
  IN UINT8 AcLossControlValue
  );

VOID
FchVgaInit (
  OUT VOID
  );

VOID
RecordFchConfigPtr (
  IN UINT32 FchConfigPtr
  );

VOID
ValidateFchVariant (
  IN VOID  *FchDataPtr
  );

VOID
RecordSmiStatus (
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
ClearAllSmiStatus (
  IN HYGON_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
IsLpcModeEnabled (
  VOID
  );

BOOLEAN
IsExternalClockMode (
  IN VOID  *FchDataPtr
  );

VOID
SbSleepTrapControl (
  IN BOOLEAN SleepTrap
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

VOID
TurnOffUsbPme (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    UsbIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

#endif
