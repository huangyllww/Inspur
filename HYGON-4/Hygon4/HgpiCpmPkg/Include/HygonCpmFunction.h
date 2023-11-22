/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM structures and definitions
 *
 * Contains HYGON CPM Common Function Interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  Include
 *
 */
/*****************************************************************************
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
 ******************************************************************************
 */

#ifndef _HYGON_CPM_FUNCTION_H_
#define _HYGON_CPM_FUNCTION_H_

typedef UINT8 (EFIAPI *HYGON_CPM_IOREAD8_FN)(
  IN       UINT16                      Port
  );

typedef UINT16 (EFIAPI *HYGON_CPM_IOREAD16_FN)(
  IN       UINT16                      Port
  );

typedef UINT32 (EFIAPI *HYGON_CPM_IOREAD32_FN)(
  IN       UINT16                      Port
  );

typedef void (EFIAPI *HYGON_CPM_IOWRITE8_FN)(
  IN       UINT16                      Port,
  IN       UINT8                       Value
  );

typedef void (EFIAPI *HYGON_CPM_IOWRITE16_FN)(
  IN       UINT16                      Port,
  IN       UINT16                      Value
  );

typedef void (EFIAPI *HYGON_CPM_IOWRITE32_FN)(
  IN       UINT16                      Port,
  IN       UINT32                      Value
  );

typedef UINT8 (EFIAPI *HYGON_CPM_MMIOREAD8_FN)(
  IN       UINTN                       Address
  );

typedef UINT16 (EFIAPI *HYGON_CPM_MMIOREAD16_FN)(
  IN       UINTN                       Address
  );

typedef UINT32 (EFIAPI *HYGON_CPM_MMIOREAD32_FN)(
  IN       UINTN                       Address
  );

typedef UINT8 (EFIAPI *HYGON_CPM_MMIOWRITE8_FN)(
  IN       UINTN                       Address,
  IN       UINT8                       Value
  );

typedef UINT16 (EFIAPI *HYGON_CPM_MMIOWRITE16_FN)(
  IN       UINTN                       Address,
  IN       UINT16                      Value
  );

typedef UINT32 (EFIAPI *HYGON_CPM_MMIOWRITE32_FN)(
  IN       UINTN                       Address,
  IN       UINT32                      Value
  );

typedef UINT8 (EFIAPI *HYGON_CPM_MMIOOR8_FN)(
  IN       UINTN                       Address,
  IN       UINT8                       OrData
  );

typedef UINT16 (EFIAPI *HYGON_CPM_MMIOOR16_FN)(
  IN       UINTN                       Address,
  IN       UINT16                      OrData
  );

typedef UINT32 (EFIAPI *HYGON_CPM_MMIOOR32_FN)(
  IN       UINTN                       Address,
  IN       UINT32                      OrData
  );

typedef UINT8 (EFIAPI *HYGON_CPM_MMIOAND8_FN)(
  IN       UINTN                       Address,
  IN       UINT8                       AndData
  );

typedef UINT16 (EFIAPI *HYGON_CPM_MMIOAND16_FN)(
  IN       UINTN                       Address,
  IN       UINT16                      AndData
  );

typedef UINT32 (EFIAPI *HYGON_CPM_MMIOAND32_FN)(
  IN       UINTN                       Address,
  IN       UINT32                      AndData
  );

typedef UINT8 (EFIAPI *HYGON_CPM_MMIOANDTHENOR8_FN)(
  IN       UINTN                       Address,
  IN       UINT8                       AndData,
  IN       UINT8                       OrData
  );

typedef UINT16 (EFIAPI *HYGON_CPM_MMIOANDTHENOR16_FN)(
  IN       UINTN                       Address,
  IN       UINT16                      AndData,
  IN       UINT16                      OrData
  );

typedef UINT32 (EFIAPI *HYGON_CPM_MMIOANDTHENOR32_FN)(
  IN       UINTN                       Address,
  IN       UINT32                      AndData,
  IN       UINT32                      OrData
  );

typedef UINT64 (EFIAPI *HYGON_CPM_MSRREAD_FN)(
  IN       UINT32                      Index
  );

typedef UINT64 (EFIAPI *HYGON_CPM_MSRWRITE_FN)(
  IN       UINT32                      Index,
  IN       UINT64                      Value
  );

typedef UINT64 (EFIAPI *HYGON_CPM_READTSC_FN)(
  );

typedef VOID (EFIAPI *HYGON_CPM_CPUIDRAWREAD_FN)(
  IN       UINT32                      CpuidFcnAddress,
  OUT   CPUID_DATA                  *Value
  );

typedef VOID (EFIAPI *HYGON_CPM_CPUIDREAD_FN)(
  IN       VOID                        *This,
  OUT   UINT32                      *CpuidFamilyModelStepping,
  OUT   UINT32                      *PkgType
  );

typedef void (EFIAPI *HYGON_CPM_POSTCODE_FN)(
  IN       UINT32                      PostCode
  );

typedef void (EFIAPI *HYGON_CPM_STALL_FN)(
  IN       VOID                        *This,
  IN       UINT32                      Microseconds
  );

typedef UINT8 (EFIAPI *HYGON_CPM_GETGPIO_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Pin
  );

typedef void (EFIAPI *HYGON_CPM_SETGPIO_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Pin,
  IN       UINT16                      Value
  );

typedef UINT8 (EFIAPI *HYGON_CPM_GETRTC_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Index
  );

typedef void (EFIAPI *HYGON_CPM_SETRTC_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Index,
  IN       UINT8                       Value
  );

typedef UINT8 (EFIAPI *HYGON_CPM_GETACPI_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Index
  );

typedef void (EFIAPI *HYGON_CPM_SETACPI_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Index,
  IN       UINT8                       Value
  );

typedef UINT8 (EFIAPI *HYGON_CPM_GETGEVENT_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Pin
  );

typedef void (EFIAPI *HYGON_CPM_SETGEVENT_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Pin,
  IN       UINT16                      Value
  );

typedef void (EFIAPI *HYGON_CPM_SETSMICONTROL_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Pin,
  IN       UINT16                      Value
  );

typedef void (EFIAPI *HYGON_CPM_SETGEVENTSCITRIG_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Pin,
  IN       UINT8                       Value
  );

typedef void (EFIAPI *HYGON_CPM_SETGEVENTSCI_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Pin
  );

typedef UINT32 (EFIAPI *HYGON_CPM_GETSTRAP_FN)(
  IN       VOID                        *This
  );

typedef void (EFIAPI *HYGON_CPM_SETCLKREQ_FN)(
  IN       VOID                        *This,
  IN       UINT8                       ClkId,
  IN       UINT8                       ClkReq
  );

typedef UINT32 (EFIAPI *HYGON_CPM_GETSBTSIADDR_FN)(
  IN       VOID                        *This
  );

typedef EFI_STATUS (EFIAPI *HYGON_CPM_SMBUSREAD_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Select,
  IN       UINT8                       Address,
  IN       UINT8                       Offset,
  IN       UINTN                       Length,
  OUT   UINT8                       *Value
  );

typedef EFI_STATUS (EFIAPI *HYGON_CPM_SMBUSWRITE_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Select,
  IN       UINT8                       Address,
  IN       UINT8                       Offset,
  IN       UINTN                       Length,
  IN       UINT8                       *Value
  );

typedef void *(EFIAPI *HYGON_CPM_GETTABLEPTR_FN)(
  IN       VOID                        *This,
  IN       UINT32                      TableId
  );

typedef void *(EFIAPI *HYGON_CPM_ADDTABLE_FN)(
  IN       VOID                        *This,
  IN       VOID                        *TablePtr
  );

typedef void *(EFIAPI *HYGON_CPM_REMOVETABLE_FN)(
  IN       VOID                        *This,
  IN       VOID                        *TablePtr
  );

typedef void (EFIAPI *HYGON_CPM_SETMEMVOLTAGE_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Voltage
  );

typedef void (EFIAPI *HYGON_CPM_SETVDDPVDDRVOLTAGE_FN)(
  IN       VOID                        *This,
  IN       VDDP_VDDR_VOLTAGE           Voltage
  );

typedef BOOLEAN (EFIAPI *HYGON_CPM_ADDSSDTCALLBACK_FN)(
  IN       VOID                        *This,
  IN       VOID                        *AcpiTablePtr,
  IN       VOID                        *Context
  );

typedef EFI_STATUS (EFIAPI *HYGON_CPM_ADDSSDTTABLE_FN)(
  IN       VOID                        *This,
  IN       VOID                        *EfiGuid,
  IN       UINT64                      *OemTableId,
  IN       HYGON_CPM_ADDSSDTCALLBACK_FN  Function,
  IN       VOID                        *Context
  );

typedef BOOLEAN (EFIAPI *HYGON_CPM_ISAMLOPREGIONOBJECT_FN)(
  IN       UINT8                       *TablePtr
  );

typedef UINT8 (EFIAPI *HYGON_CPM_CHECKPCIEDEVICE_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Device,
  IN       UINT8                       Function
  );

typedef void (EFIAPI *HYGON_CPM_PCIERESET_FN)(
  IN       VOID                        *This,
  IN       UINT8                       ResetId,
  IN       UINT8                       ResetControl
  );

typedef void (EFIAPI *HYGON_CPM_RESETDEVICE_FN)(
  IN       VOID                        *This,
  IN       UINT8                       DeviceId,
  IN       UINT8                       Mode
  );

typedef UINT8 (EFIAPI *HYGON_CPM_PCIREAD8_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset
  );

typedef UINT16 (EFIAPI *HYGON_CPM_PCIREAD16_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset
  );

typedef UINT32 (EFIAPI *HYGON_CPM_PCIREAD32_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset
  );

typedef void (EFIAPI *HYGON_CPM_PCIWRITE8_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset,
  IN       UINT8                       Data
  );

typedef void (EFIAPI *HYGON_CPM_PCIWRITE16_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset,
  IN       UINT16                      Data
  );

typedef void (EFIAPI *HYGON_CPM_PCIWRITE32_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset,
  IN       UINT32                      Data
  );

typedef void (EFIAPI *HYGON_CPM_PCIAND8_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset,
  IN       UINT8                       AndData
  );

typedef void (EFIAPI *HYGON_CPM_PCIAND16_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset,
  IN       UINT16                      AndData
  );

typedef void (EFIAPI *HYGON_CPM_PCIAND32_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset,
  IN       UINT32                      AndData
  );

typedef void (EFIAPI *HYGON_CPM_PCIOR8_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset,
  IN       UINT8                       OrData
  );

typedef void (EFIAPI *HYGON_CPM_PCIOR16_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset,
  IN       UINT16                      OrData
  );

typedef void (EFIAPI *HYGON_CPM_PCIOR32_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset,
  IN       UINT32                      OrData
  );

typedef void (EFIAPI *HYGON_CPM_PCIANDTHENOR8_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset,
  IN       UINT8                       AndData,
  IN       UINT8                       OrData
  );

typedef void (EFIAPI *HYGON_CPM_PCIANDTHENOR16_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset,
  IN       UINT16                      AndData,
  IN       UINT16                      OrData
  );

typedef void (EFIAPI *HYGON_CPM_PCIANDTHENOR32_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Bus,
  IN       UINT8                       Device,
  IN       UINT8                       Function,
  IN       UINT16                      Offset,
  IN       UINT32                      AndData,
  IN       UINT32                      OrData
  );

typedef UINT8 (EFIAPI *HYGON_CPM_SMNREAD8_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset
  );

typedef UINT16 (EFIAPI *HYGON_CPM_SMNREAD16_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset
  );

typedef UINT32 (EFIAPI *HYGON_CPM_SMNREAD32_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset
  );

typedef void (EFIAPI *HYGON_CPM_SMNWRITE8_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset,
  IN       UINT8                       Data
  );

typedef void (EFIAPI *HYGON_CPM_SMNWRITE16_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset,
  IN       UINT16                      Data
  );

typedef void (EFIAPI *HYGON_CPM_SMNWRITE32_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset,
  IN       UINT32                      Data
  );

typedef void (EFIAPI *HYGON_CPM_SMNAND8_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset,
  IN       UINT8                       AndData
  );

typedef void (EFIAPI *HYGON_CPM_SMNAND16_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset,
  IN       UINT16                      AndData
  );

typedef void (EFIAPI *HYGON_CPM_SMNAND32_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset,
  IN       UINT32                      AndData
  );

typedef void (EFIAPI *HYGON_CPM_SMNOR8_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset,
  IN       UINT8                       OrData
  );

typedef void (EFIAPI *HYGON_CPM_SMNOR16_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset,
  IN       UINT16                      OrData
  );

typedef void (EFIAPI *HYGON_CPM_SMNOR32_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset,
  IN       UINT32                      OrData
  );

typedef void (EFIAPI *HYGON_CPM_SMNANDTHENOR8_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset,
  IN       UINT8                       AndData,
  IN       UINT8                       OrData
  );

typedef void (EFIAPI *HYGON_CPM_SMNANDTHENOR16_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset,
  IN       UINT16                      AndData,
  IN       UINT16                      OrData
  );

typedef void (EFIAPI *HYGON_CPM_SMNANDTHENOR32_FN)(
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT32                      SmnAddr,
  IN       UINT32                      Offset,
  IN       UINT32                      AndData,
  IN       UINT32                      OrData
  );

typedef UINT8 (EFIAPI *HYGON_CPM_GETSATAMODE_FN)(
  IN       VOID                        *This
  );

typedef BOOLEAN (EFIAPI *HYGON_CPM_DETECTDEVICE_FN)(
  IN       VOID                        *This,
  IN       UINT8                       DeviceId,
  OUT   UINT8                       *Status
  );

typedef BOOLEAN (EFIAPI *HYGON_CPM_ISFCHDEVICE_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Device,
  IN       UINT8                       Function
  );

typedef BOOLEAN (EFIAPI *HYGON_CPM_ISTHERMALSUPPORT_FN)(
  IN       VOID                        *This
  );

typedef VOID (EFIAPI *HYGON_CPM_RELOCATETABLE_FN)(
  IN OUT   VOID                        *TablePtr
  );

typedef VOID (EFIAPI *HYGON_CPM_COPYMEM_FN)(
  IN OUT   VOID                        *destination,
  IN       VOID                        *source,
  IN       UINTN                       size
  );

typedef UINT8 (EFIAPI *HYGON_CPM_GETSCIMAP_FN)(
  IN       VOID                        *This,
  IN       UINT8                       GeventPin
  );

typedef UINT32 (EFIAPI *HYGON_CPM_GETPCIEASLNAME_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Device,
  IN       UINT8                       Function
  );

typedef EFI_STATUS (EFIAPI *HYGON_CPM_KBCREAD_FN)(
  IN       UINT16                      BaseAddr,
  IN       UINT8                       Command,
  IN       UINT8                       *Data,
  OUT   UINT8                       *Value
  );

typedef EFI_STATUS (EFIAPI *HYGON_CPM_KBCWRITE_FN)(
  IN       UINT16                      BaseAddr,
  IN       UINT8                       Command,
  IN       UINT8                       *Data
  );

typedef UINT8 (EFIAPI *HYGON_CPM_GETCPUREVISIONID_FN)(
  IN       VOID                        *This
  );

typedef BOOLEAN (EFIAPI *HYGON_CPM_ISUMI_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Device,
  IN       UINT8                       Function
  );

typedef UINT8 (EFIAPI *HYGON_CPM_GETBOOTMODE_FN)(
  );

typedef BOOLEAN (EFIAPI *HYGON_CPM_ISRTCWAKEUP_FN)(
  IN       VOID                        *This
  );

typedef VOID (EFIAPI *HYGON_CPM_LOADPREINITTABLE_FN)(
  IN       VOID                        *This,
  IN       UINT8                       Stage
  );

typedef HGPI_STATUS (EFIAPI *HYGON_CPM_GETPOSTEDVBIOSIMAGE_FN)(
  IN      GFX_VBIOS_IMAGE_INFO         *VbiosImageInfo
  );

typedef UINT16 (EFIAPI *HYGON_CPM_GETACPISMICMD_FN)(
  IN       VOID                        *This
  );

typedef VOID (EFIAPI *HYGON_CPM_POWERONDEVICE_FN)(
  IN       VOID                        *This,
  IN       UINT8                       DeviceId,
  IN       UINT8                       State
  );

typedef UINT8 (EFIAPI *HYGON_CPM_GETDEVICECONFIG_FN)(
  IN       VOID                        *This,
  IN       UINT8                       DeviceId
  );

/// Common Functions for CPM Drivers

typedef struct _HYGON_CPM_COMMON_FUNCTION {
  HYGON_CPM_IOREAD8_FN                IoRead8;                ///< Read IO byte
  HYGON_CPM_IOREAD16_FN               IoRead16;               ///< Read IO word
  HYGON_CPM_IOREAD32_FN               IoRead32;               ///< Read IO dword
  HYGON_CPM_IOWRITE8_FN               IoWrite8;               ///< Write IO byte
  HYGON_CPM_IOWRITE16_FN              IoWrite16;              ///< Write IO word
  HYGON_CPM_IOWRITE32_FN              IoWrite32;              ///< Write IO dword
  HYGON_CPM_MMIOREAD8_FN              MmioRead8;              ///< Read memory/MMIO byte
  HYGON_CPM_MMIOREAD16_FN             MmioRead16;             ///< Read memory/MMIO word
  HYGON_CPM_MMIOREAD32_FN             MmioRead32;             ///< Read memory/MMIO dword
  HYGON_CPM_MMIOWRITE8_FN             MmioWrite8;             ///< Write memory/MMIO byte
  HYGON_CPM_MMIOWRITE16_FN            MmioWrite16;            ///< Write memory/MMIO word
  HYGON_CPM_MMIOWRITE32_FN            MmioWrite32;            ///< Write memory/MMIO dword
  HYGON_CPM_MMIOAND8_FN               MmioAnd8;               ///< Read memory/MMIO byte, perform a bitwise AND
                                                              ///< and write the result back to memory/MMIO
  HYGON_CPM_MMIOAND16_FN              MmioAnd16;              ///< Read memory/MMIO word, perform a bitwise AND
                                                              ///< and write the result back to memory/MMIO
  HYGON_CPM_MMIOAND32_FN              MmioAnd32;              ///< Read memory/MMIO dword, perform a bitwise AND
                                                              ///< and write the result back to memory/MMIO
  HYGON_CPM_MMIOOR8_FN                MmioOr8;                ///< Read memory/MMIO byte, perform a bitwise OR
                                                              ///< and write the result back to memory/MMIO
  HYGON_CPM_MMIOOR16_FN               MmioOr16;               ///< Read memory/MMIO word, perform a bitwise OR
                                                              ///< and write the result back to memory/MMIO
  HYGON_CPM_MMIOOR32_FN               MmioOr32;               ///< Read memory/MMIO dword, perform a bitwise OR
                                                              ///< and write the result back to memory/MMIO
  HYGON_CPM_MMIOANDTHENOR8_FN         MmioAndThenOr8;         ///< Read memory/MMIO byte, perform a bitwise AND
                                                              ///< followed by a bitwise inclusive OR, and writes
                                                              ///< the result back to memory/MMIO
  HYGON_CPM_MMIOANDTHENOR16_FN        MmioAndThenOr16;        ///< Read memory/MMIO word, perform a bitwise AND
                                                              ///< followed by a bitwise inclusive OR, and writes
                                                              ///< the result back to memory/MMIO
  HYGON_CPM_MMIOANDTHENOR32_FN        MmioAndThenOr32;        ///< Read memory/MMIO dword, perform a bitwise AND
                                                              ///< followed by a bitwise inclusive OR, and writes
                                                              ///< the result back to memory/MMIO
  HYGON_CPM_MSRREAD_FN                MsrRead;                ///< Read MSR register
  HYGON_CPM_MSRWRITE_FN               MsrWrite;               ///< Write MSR register
  HYGON_CPM_PCIREAD8_FN               PciRead8;               ///< Read PCI register byte
  HYGON_CPM_PCIREAD16_FN              PciRead16;              ///< Read PCI register word
  HYGON_CPM_PCIREAD32_FN              PciRead32;              ///< Read PCI register dword
  HYGON_CPM_PCIWRITE8_FN              PciWrite8;              ///< Write PCI register byte
  HYGON_CPM_PCIWRITE16_FN             PciWrite16;             ///< Write PCI register word
  HYGON_CPM_PCIWRITE32_FN             PciWrite32;             ///< Write PCI register dword
  HYGON_CPM_PCIAND8_FN                PciAnd8;                ///< Read PCI register byte, perform a bitwise AND
                                                              ///< and write the result back to PCI register
  HYGON_CPM_PCIAND16_FN               PciAnd16;               ///< Read PCI register word, perform a bitwise AND
                                                              ///< and write the result back to PCI register
  HYGON_CPM_PCIAND32_FN               PciAnd32;               ///< Read PCI register dword, perform a bitwise AND
                                                              ///< and write the result back to PCI register
  HYGON_CPM_PCIOR8_FN                 PciOr8;                 ///< Read PCI register byte, perform a bitwise OR
                                                              ///< and write the result back to PCI register
  HYGON_CPM_PCIOR16_FN                PciOr16;                ///< Read PCI register word, perform a bitwise AND
                                                              ///< and write the result back to PCI register
  HYGON_CPM_PCIOR32_FN                PciOr32;                ///< Read PCI register dword, perform a bitwise AND
                                                              ///< and write the result back to PCI register
  HYGON_CPM_PCIANDTHENOR8_FN          PciAndThenOr8;          ///< Read PCI register byte, perform a bitwise AND
                                                              ///< followed by a bitwise inclusive OR, and writes
                                                              ///< the result back to the PCI register
  HYGON_CPM_PCIANDTHENOR16_FN         PciAndThenOr16;         ///< Read PCI register word, perform a bitwise AND
                                                              ///< followed by a bitwise inclusive OR, and writes
                                                              ///< the result back to the PCI register
  HYGON_CPM_PCIANDTHENOR32_FN         PciAndThenOr32;         ///< Read PCI register dword, perform a bitwise AND
                                                              ///< followed by a bitwise inclusive OR, and writes
                                                              ///< the result back to the PCI register
  HYGON_CPM_SMNREAD8_FN               SmnRead8;               ///< Read SMN register dword
  HYGON_CPM_SMNREAD16_FN              SmnRead16;              ///< Read SMN register dword
  HYGON_CPM_SMNREAD32_FN              SmnRead32;              ///< Read SMN register dword
  HYGON_CPM_SMNWRITE8_FN              SmnWrite8;              ///< Write SMN register dword
  HYGON_CPM_SMNWRITE16_FN             SmnWrite16;             ///< Write SMN register dword
  HYGON_CPM_SMNWRITE32_FN             SmnWrite32;             ///< Write SMN register dword
  HYGON_CPM_SMNAND8_FN                SmnAnd8;                ///< Read SMN register byte, perform a bitwise AND
                                                              ///< and write the result back to SMN register
  HYGON_CPM_SMNAND16_FN               SmnAnd16;               ///< Read SMN register byte, perform a bitwise AND
                                                              ///< and write the result back to SMN register
  HYGON_CPM_SMNAND32_FN               SmnAnd32;               ///< Read SMN register byte, perform a bitwise AND
                                                              ///< and write the result back to SMN register
  HYGON_CPM_SMNOR8_FN                 SmnOr8;                 ///< Read SMN register byte, perform a bitwise OR
                                                              ///< and write the result back to SMN register
  HYGON_CPM_SMNOR16_FN                SmnOr16;                ///< Read SMN register byte, perform a bitwise OR
                                                              ///< and write the result back to SMN register
  HYGON_CPM_SMNOR32_FN                SmnOr32;                ///< Read SMN register byte, perform a bitwise OR
                                                              ///< and write the result back to SMN register
  HYGON_CPM_SMNANDTHENOR8_FN          SmnAndThenOr8;          ///< Read SMN register byte, perform a bitwise AND
                                                              ///< followed by a bitwise inclusive OR, and writes
                                                              ///< the result back to the SMN register
  HYGON_CPM_SMNANDTHENOR16_FN         SmnAndThenOr16;         ///< Read SMN register byte, perform a bitwise AND
                                                              ///< followed by a bitwise inclusive OR, and writes
                                                              ///< the result back to the SMN register
  HYGON_CPM_SMNANDTHENOR32_FN         SmnAndThenOr32;         ///< Read SMN register byte, perform a bitwise AND
                                                              ///< followed by a bitwise inclusive OR, and writes
                                                              ///< the result back to the SMN register
  HYGON_CPM_READTSC_FN                ReadTsc;                ///< Read TSC
  HYGON_CPM_CPUIDRAWREAD_FN           CpuidRawRead;           ///< Read CPUID Raw Data
  HYGON_CPM_CPUIDREAD_FN              CpuidRead;              ///< Read CPUID
  HYGON_CPM_POSTCODE_FN               PostCode;               ///< Output a post code
  HYGON_CPM_CHECKPCIEDEVICE_FN        CheckPcieDevice;        ///< Check whether PCI space exists
  HYGON_CPM_DETECTDEVICE_FN           DetectDevice;           ///< Detect whether the device exists
  HYGON_CPM_POWERONDEVICE_FN          PowerOnDevice;          ///< Power on/off device
  HYGON_CPM_GETDEVICECONFIG_FN        GetDeviceConfig;        ///< Get the config of device

  HYGON_CPM_KBCREAD_FN                KbcRead;                ///< Read Kbc Register
  HYGON_CPM_KBCWRITE_FN               KbcWrite;               ///< Write Kbc Register

  HYGON_CPM_GETRTC_FN                 GetRtc;                 ///< Read RTC register
  HYGON_CPM_SETRTC_FN                 SetRtc;                 ///< Write RTC register
  HYGON_CPM_GETACPI_FN                GetAcpi;                ///< Read ACPI register
  HYGON_CPM_SETACPI_FN                SetAcpi;                ///< Write ACPI register
  HYGON_CPM_GETGPIO_FN                GetGpio;                ///< Get GPIO pin status: 0: Low. 1: High
  HYGON_CPM_SETGPIO_FN                SetGpio;                ///< Set GPIO and IO Mux register
  HYGON_CPM_GETGEVENT_FN              GetGevent;              ///< Get GEVENT pin status: 0: Low. 1: High
  HYGON_CPM_SETGEVENT_FN              SetGevent;              ///< Set GEVENT register
  HYGON_CPM_SETSMICONTROL_FN          SetSmiControl;          ///< Set GEVENT SMI Control Register
  HYGON_CPM_SETGEVENTSCITRIG_FN       SetGeventSciTrig;       ///< Set SCI trigger method of GEVENT pin
  HYGON_CPM_SETGEVENTSCI_FN           SetGeventSci;           ///< Trigger a GEVENT SCI interrupt
  HYGON_CPM_GETSTRAP_FN               GetStrap;               ///< Get FCH strap status
                                                              ///< BIT0: IMC
                                                              ///< BIT1: FCH Internal Clock
                                                              ///< BIT2: S5+
  HYGON_CPM_SETCLKREQ_FN              SetClkReq;              ///< Set FCH ClkReq register
  HYGON_CPM_STALL_FN                  Stall;                  ///< Delay in the unit of 1us
  HYGON_CPM_GETSATAMODE_FN            GetSataMode;            ///< Get current mode of SATA controller
  HYGON_CPM_ISFCHDEVICE_FN            IsFchDevice;            ///< Is a FCH embedded device
  HYGON_CPM_GETSCIMAP_FN              GetSciMap;              ///< Get SciMap value of GEVENT pin

  HYGON_CPM_GETCPUREVISIONID_FN       GetCpuRevisionId;       ///< Get CPU revision ID
  HYGON_CPM_GETSBTSIADDR_FN           GetSbTsiAddr;           ///< Get the address of SB-TSI register
  HYGON_CPM_ISTHERMALSUPPORT_FN       IsThermalSupport;       ///< Is thermal function supported in current CPU or APU
  HYGON_CPM_GETPCIEASLNAME_FN         GetPcieAslName;         ///< Get ASL name of PCIE brdige
  HYGON_CPM_GETPCIEASLNAME_FN         GetFchPcieAslName;      ///< Get ASL name of Fch PCIE brdige
  HYGON_CPM_GETBOOTMODE_FN            GetBootMode;            ///< Get Boot Mode: 0 - S0. 1 - S1. 3 - S3. 4 - S4. 5 - S5
  HYGON_CPM_ISRTCWAKEUP_FN            IsRtcWakeup;            ///< Is RTC Wakeup
  HYGON_CPM_ISUMI_FN                  IsUmi;                  ///< Is the device for UMI link

  HYGON_CPM_GETTABLEPTR_FN            GetTablePtr;            ///< Get CPM Table Pointer
  HYGON_CPM_GETTABLEPTR_FN            GetTablePtr2;           ///< Get CPM Table Pointer. The table can be re-writable
  HYGON_CPM_ADDTABLE_FN               AddTable;               ///< Add a table in Hob table list
  HYGON_CPM_REMOVETABLE_FN            RemoveTable;            ///< Remove a table from Gob table list
  HYGON_CPM_SMBUSREAD_FN              ReadSmbus;              ///< Read from a device on SMBUS
  HYGON_CPM_SMBUSWRITE_FN             WriteSmbus;             ///< Write to a device on SMBUS
  HYGON_CPM_SMBUSREAD_FN              ReadSmbusBlock;         ///< Read from a device on SMBUS in block mode
  HYGON_CPM_SMBUSWRITE_FN             WriteSmbusBlock;        ///< Write to a device on SMBUS in block mode
  HYGON_CPM_RESETDEVICE_FN            ResetDevice;            ///< Toggle reset pin of a device
  HYGON_CPM_RELOCATETABLE_FN          RelocateTable;          ///< Adjust the pointers of CPM table in Hob
  HYGON_CPM_COPYMEM_FN                CopyMem;                ///< Copy a memory block
  HYGON_CPM_LOADPREINITTABLE_FN       LoadPreInitTable;       ///< Load HYGON_CPM_PRE_INIT_TABLE

  HYGON_CPM_ADDSSDTTABLE_FN           AddSsdtTable;           ///< Register one SSDT table
  HYGON_CPM_ISAMLOPREGIONOBJECT_FN    IsAmlOpRegionObject;    ///< Is AML Op Region Object

  HYGON_CPM_GETACPISMICMD_FN          GetAcpiSmiCmd;          ///< Get ACPI SMI Command Base Register
} HYGON_CPM_COMMON_FUNCTION;

/// CPM Public Functions for platform PEI Driver to use

typedef struct _HYGON_CPM_PEIM_PUBLIC_FUNCTION {
  HYGON_CPM_SETMEMVOLTAGE_FN         SetMemVoltage;             ///< Set memory voltage
  HYGON_CPM_SETVDDPVDDRVOLTAGE_FN    SetVddpVddrVoltage;        ///< Set VDDP/VDDR voltage
  HYGON_CPM_PCIERESET_FN             PcieReset;                 ///< Toggle PCIE reset pin
  PCIe_COMPLEX_DESCRIPTOR            *PcieComplexDescriptorPtr; ///< The pointer of PCIE complex descriptor
  HSIO_COMPLEX_DESCRIPTOR            *HsioComplexDescriptorPtr; ///< The pointer of HSIO complex descriptor
} HYGON_CPM_PEIM_PUBLIC_FUNCTION;

/// CPM Public Functions for platform DXE Driver to use

typedef struct _HYGON_CPM_DXE_PUBLIC_FUNCTION {
  HYGON_CPM_GETPOSTEDVBIOSIMAGE_FN    GetPostedVbiosImage;        ///< Get posted VBIOS image
} HYGON_CPM_DXE_PUBLIC_FUNCTION;

#endif
