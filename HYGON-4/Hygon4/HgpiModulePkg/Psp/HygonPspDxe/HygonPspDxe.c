/* $NoKeywords:$ */

/**
 * @file
 *
 * PSP DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  PSP
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
#include <PiDxe.h>
#include <Guid/EventGroup.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HygonCapsuleLib.h>
#include <Protocol/SmmControl2.h>
#include <Protocol/MpService.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciEnumerationComplete.h>
#include <Protocol/SmmCommunication.h>

#include <Protocol/HygonPspCommonServiceProtocol.h>
#include <HGPI.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/HygonPspBaseLib.h>
#include <Library/HygonPspMboxLib.h>
#include <Library/HpcbLib.h>
#include <Library/HygonPspHpobLib.h>
#include <Protocol/PspPlatformProtocol.h>
#include <Library/CcxPspLib.h>
#include <Library/HygonPspFlashUpdateLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/SmnAccessLib.h>
#include <HygonPspSmmCommunication.h>
#include <Filecode.h>
#include <CddRegistersDm.h>

#define FILECODE  PSP_HYGONPSPDXE_HYGONPSPDXE_FILECODE

#define HPOB_UNIQUE_HPCB_INSTANCE_INVALID_VALUE  0xDEADBEEFul

#define HYGON_SMM_COMMUNICATION_BUFFER_SIZE  (4 * 1024)

extern EFI_GUID gPspFlashAccSmmCommReadyProtocolGuid;

EFI_SMM_CONTROL2_PROTOCOL  *mSmmControl;
PSP_PLATFORM_PROTOCOL      *mPspPlatfromProtocol;

STATIC HYGON_PSP_COMMON_SERVICE_PROTOCOL  mPspCommonServiceProtocol = {
  GetFtpmControlAreaV2,
  SwitchPspMmioDecodeV2,
  CheckPspDevicePresentV2,
  CheckFtpmCapsV2,
  PSPEntryInfoV2,
  PspLibTimeOutV2
};

/*++

Routine Description:

  This function is invoked by EFI_EVENT_SIGNAL_LEGACY_BOOT.
  Before booting to legacy OS, inform SMM

Arguments:

  Event   - The triggered event.
  Context - Context for this event.

Returns:

  None

--*/
VOID
PspNotifySmmDrivers (
  VOID
  )
{
  UINT8       Cmd;
  EFI_STATUS  Status;

  // Below two protocols haven't been added to driver's depex, assume it should be ready at RTB
  Status = gBS->LocateProtocol (&gEfiSmmControl2ProtocolGuid, NULL, &mSmmControl);
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Locate SmmCtrl Protocol Fail\n");
    return;
  }

  // Locate Psp Platform Protocol to get customized information
  IDS_HDT_CONSOLE_PSP_TRACE ("Locate Psp Platform Protocol\n");
  Status = gBS->LocateProtocol (
                  &gPspPlatformProtocolGuid,
                  NULL,
                  &mPspPlatfromProtocol
                  );
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Locate Psp Platform Protocol Fail\n");
    return;
  }

  Cmd = mPspPlatfromProtocol->SwSmiCmdtoBuildContext;

  IDS_HDT_CONSOLE_PSP_TRACE ("PspNotifySmmDrivers Cmd[%x]\n", Cmd);
  mSmmControl->Trigger (mSmmControl, &Cmd, NULL, FALSE, 0);
}

VOID
SetPspAddrMsrTask (
  IN  VOID                                     *Buffer
  )
{
  UINT32 PspAddr;
  //Get PSP BAR Address
  if (GetPspMmioBase (&PspAddr) == FALSE) {
    return;
  }
  UpdatePspAddr (PspAddr);
}

EFI_STATUS
SetPspAddrMsr (
  )
{
  EFI_STATUS                Status;
  VOID                      *UnusedBufPtr;
  EFI_MP_SERVICES_PROTOCOL  *MpServices;

  IDS_HDT_CONSOLE_PSP_TRACE ("Locate MP Protocol\n");
  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);

  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Set PSPADDR MSR\n");
  UnusedBufPtr = NULL;
  SetPspAddrMsrTask (UnusedBufPtr);

  Status = MpServices->StartupAllAPs (
                           MpServices,                                     // EFI_MP_SERVICES_PROTOCOL *this
                           (EFI_AP_PROCEDURE)SetPspAddrMsrTask,            // EFI_AP_PROCEDURE
                           TRUE,                                           // BOOLEAN SingleThreaded? FALSE=execute all functions at the same time on all CPUs
                           NULL,                                           // EFI_EVENT WaitEvent OPTIONAL
                           0,                                              // UINTN Timeout (Unsupported)
                           NULL,                                           // VOID *ProcArguments OPTIONAL
                           NULL                                            // Failed CPUList OPTIONAL (unsupported)
                           );

  return EFI_SUCCESS;
}

VOID
PspMpServiceCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;
  VOID        *Interface;

  HGPI_TESTPOINT (TpPspDxeMpCallBackEntry, NULL);
  //
  // Try to locate it because EfiCreateProtocolNotifyEvent will trigger it once when registration.
  // Just return if it is not found.
  //
  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  &Interface
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  // Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  SetPspAddrMsr ();
  HGPI_TESTPOINT (TpPspDxeMpCallBackExit, NULL);
}

/**
 *  Prepare the HPOB Buffer and write to BIOS Directory Entry 0x63
 *
 *
 *  @retval EFI_SUCCESS       The Data save to FLASH successfully
 *
 **/
EFI_STATUS
PspWriteHpobS3Entry (
  VOID
  )
{
  HPOB_HEADER                *HpobDramPtr;
  BOOLEAN                    HpobEntryExist;
  TYPE_ATTRIB                TypeAttrib;
  UINT64                     EntryAddress;
  UINT32                     EntrySize;
  UINT64                     EntryDest;
  HPOBLIB_INFO               *HpobInfo;
  BOOLEAN                    DimmCfgUpdatedFlag;

  IDS_HDT_CONSOLE_PSP_TRACE ("\nPspWriteHpobS3Entry\n");

  if (!PcdGetBool (PcdHygonAcpiS3Support)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\nS3 not support...\n");
    return EFI_UNSUPPORTED;
  }

  DimmCfgUpdatedFlag = FALSE;
  HygonPspGetDimmCfgUpdatedFlag (&DimmCfgUpdatedFlag);

  // No Dimm update and No HPCB changes versus the time saving HPOB
  if (DimmCfgUpdatedFlag == FALSE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("No Dimm cfg & HPCB update, skip the write\n");
    return EFI_UNSUPPORTED;
  }

  if (HygonPspGetHpobInfo (&HpobInfo) != EFI_SUCCESS) {
    IDS_HDT_CONSOLE_PSP_TRACE ("No HPOB memory copy detected\n");
    return EFI_UNSUPPORTED;
  }

  HpobDramPtr = (HPOB_HEADER *)(UINTN)HpobInfo->HpobAddr;

  // Check the existence and size of HPOB NV entry 0x63
  HpobEntryExist = BIOSEntryInfo (HPOB_NV_COPY, INSTANCE_IGNORED, &TypeAttrib, &EntryAddress, &EntrySize, &EntryDest);
  if (!HpobEntryExist) {
    IDS_HDT_CONSOLE_PSP_TRACE ("PspWriteHpobS3Entry Entry not exist\n");
    return EFI_UNSUPPORTED;
  }

  // Check the size of HPOB DRAM copy
  if (HpobDramPtr->Size > EntrySize) {
    IDS_HDT_CONSOLE_PSP_TRACE ("HPOB SPI Entry too small\n");
    return EFI_UNSUPPORTED;
  }

  // Check Dimm configuration updated, if yes update HPOB NV storage at SPI
  IDS_HDT_CONSOLE_PSP_TRACE ("Update HPOB SPI copy\n");
  PspUpdateFlash ((UINT32)EntryAddress, HpobDramPtr->Size, HpobDramPtr);
  return EFI_SUCCESS;
}

VOID
HpcbRecovery (
  )
{
  EFI_STATUS   Status;
  BOOLEAN      HpcbRecoveryFlag;
  UINT8        HpcbInstance;
  TYPE_ATTRIB  TypeAttrib;
  UINT64       ActiveHpcbEntryAddress;
  UINT64       BackupHpcbEntryAddress;
  UINT32       ActiveHpcbSize;
  UINT32       BackUpHpcbSize;
  UINT64       EntryDest;
  UINT8        *Buffer;
  HPCB_HEADER  *HpcbHeader;
  UINT8        Value8;

  HpcbRecoveryFlag = FALSE;
  Status = HygonPspGetHpobHpcbRecoveryFlag (&HpcbRecoveryFlag);
  ASSERT (Status == EFI_SUCCESS);
  if (HpcbRecoveryFlag) {
    IDS_HDT_CONSOLE_PSP_TRACE ("HPCB.RecoveryFlag Set\n");
    // Query HPCB instace
    HpcbInstance = 0;
    Status = HygonPspGetHpobHpcbInstance (&HpcbInstance);
    ASSERT (Status == EFI_SUCCESS);
    IDS_HDT_CONSOLE_PSP_TRACE ("Recover Active HPCB Instance %x\n", HpcbInstance);
    // Query HPCB Backup instace information
    if (BIOSEntryInfo (BIOS_HPCB_INFO_BACKUP, HpcbInstance, &TypeAttrib, &BackupHpcbEntryAddress, &BackUpHpcbSize, &EntryDest) == FALSE) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Fatal Error: Backup Hpcb Not Detected\n");
      ASSERT (FALSE);
      HGPI_TESTPOINT (TpPspRecoverHpcbFail, NULL);
      return;
    }

    // Query HPCB active instace information
    if (BIOSEntryInfo (BIOS_HPCB_INFO, HpcbInstance, &TypeAttrib, &ActiveHpcbEntryAddress, &ActiveHpcbSize, &EntryDest) == FALSE) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Fatal Error: Active Hpcb instance Not Detected\n");
      ASSERT (FALSE);
      HGPI_TESTPOINT (TpPspRecoverHpcbFail, NULL);
      return;
    }

    // Do size check between HPCB active instace & HPCB Backup instace
    ASSERT (ActiveHpcbSize >= BackUpHpcbSize);
    if (ActiveHpcbSize < BackUpHpcbSize) {
      IDS_HDT_CONSOLE_PSP_TRACE ("ActiveHpcbSize too small\n");
      ASSERT (FALSE);
      HGPI_TESTPOINT (TpPspRecoverHpcbFail, NULL);
      return;
    }

    // Start HPCB recovery
    // Allocate temp memory to hold backup hpcb
    Buffer = NULL;
    Buffer = AllocateZeroPool (BackUpHpcbSize);
    ASSERT (Buffer != NULL);
    if (Buffer == NULL) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Allocate temp buffer fail\n");
      return;
    }

    CopyMem (Buffer, (VOID *)(UINTN)BackupHpcbEntryAddress, BackUpHpcbSize);
    // Fill the random value to UniqueHpcbInstance
    HpcbHeader = (HPCB_HEADER *)Buffer;
    IDS_HDT_CONSOLE_PSP_TRACE ("UniqueHpcbInstance Update from %x", HpcbHeader->UniqueHpcbInstance);
    HpcbHeader->UniqueHpcbInstance = (UINT32)(AsmReadTsc () & 0x00FFFFFFul);
    IDS_HDT_CONSOLE_PSP_TRACE (" to  %x\n", HpcbHeader->UniqueHpcbInstance);
    HpcbHeader->CheckSumByte = 0;
    HpcbHeader->CheckSumByte = (UINT8)(0x100 - HpcbCalcCheckSum (HpcbHeader, HpcbHeader->SizeOfHpcb));

    // Restore HPCB active instance SPI region
    IDS_HDT_CONSOLE_PSP_TRACE ("Restore HPCB active instance SPI region [0x%x] with 0x%x bytes from Buffer [0x%x]\n", ActiveHpcbEntryAddress, BackUpHpcbSize, Buffer);
    PspUpdateFlash ((UINT32)ActiveHpcbEntryAddress, BackUpHpcbSize, Buffer);
    IDS_HDT_CONSOLE_PSP_TRACE ("Restore Completed, Restart the system");
    // Update the CMOS[6:7] with flag 0xA55A to indicate the HPCB recovery succeed
    Value8 = 0x06;
    LibHygonIoWrite (AccessWidth8, 0x72, &Value8, NULL);
    Value8 = 0x5A;
    LibHygonIoWrite (AccessWidth8, 0x73, &Value8, NULL);

    Value8 = 0x07;
    LibHygonIoWrite (AccessWidth8, 0x72, &Value8, NULL);
    Value8 = 0xA5;
    LibHygonIoWrite (AccessWidth8, 0x73, &Value8, NULL);

    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
    // Should not come here
    CpuDeadLoop ();
  }
}

VOID
DumpHpcbDimmConfigInfoId (
  DIMM_HPCB_INFO_CONFIG_ID        *HpcbDimmConfigInfoId,
  UINT32                          HpcbDimmConfigInfoIdSize
  )
{
  UINT32  i;

  IDS_HDT_CONSOLE_PSP_TRACE ("DumpHpcbDimmConfigInfoId\n");
  for (i = 0; i < HpcbDimmConfigInfoIdSize; i++) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Index:%d\n", i);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tSocket: %x\n", HpcbDimmConfigInfoId[i].Socket);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tChannel: %x\n", HpcbDimmConfigInfoId[i].Channel);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tDimm: %x\n", HpcbDimmConfigInfoId[i].Dimm);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPrevPage1SpdChecksumLow: %x\n", HpcbDimmConfigInfoId[i].PrevPage1SpdChecksumLow);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPrevPage1SpdChecksumHigh: %x\n", HpcbDimmConfigInfoId[i].PrevPage1SpdChecksumHigh);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPrevDimmManId: %x\n", HpcbDimmConfigInfoId[i].PrevDimmManId);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPrevDimmModId: %x\n", HpcbDimmConfigInfoId[i].PrevDimmModId);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tDimmPresentInConfig: %x\n", HpcbDimmConfigInfoId[i].DimmPresentInConfig);
  }
}

VOID
DumpHpobCurrentBootDimmIdInfo (
  DIMM_HPOB_OPT_ID (*HpobCurrentBootDimmIdInfo)[HPOB_MAX_CHANNELS_PER_SOCKET_HYGX][HPOB_MAX_DIMMS_PER_CHANNEL]
  )
{
  UINT32  Socket;
  UINT32  Channel;
  UINT32  Dimm;
  UINT32  Page1SpdChecksumLow;  ///< DIMM checksum for Page 1
  UINT32  Page1SpdChecksumHigh; ///< DIMM checksum for Page 1
  UINT32  DimmManId;            ///< DIMM manufacturer ID
  UINT32  DimmModId;            ///< DIMM Module ID
  UINT32  DimmPresentInConfig;  ///< Indicates that the DIMM config is present

  IDS_HDT_CONSOLE_PSP_TRACE ("DumpHpobCurrentBootDimmIdInfo\n");
  for (Socket = 0; Socket < HPOB_MAX_SOCKETS_SUPPORTED; Socket++) {
    for (Channel = 0; Channel < HPOB_MAX_CHANNELS_PER_SOCKET_HYGX; Channel++) {
      for (Dimm = 0; Dimm < HPOB_MAX_DIMMS_PER_CHANNEL; Dimm++) {
        IDS_HDT_CONSOLE_PSP_TRACE ("\n\tSocket: %x\n", Socket);
        IDS_HDT_CONSOLE_PSP_TRACE ("\tChannel: %x\n", Channel);
        IDS_HDT_CONSOLE_PSP_TRACE ("\tDimm: %x\n", Dimm);
        Page1SpdChecksumLow  = HpobCurrentBootDimmIdInfo[Socket][Channel][Dimm].Page1SpdChecksumLow;
        Page1SpdChecksumHigh = HpobCurrentBootDimmIdInfo[Socket][Channel][Dimm].Page1SpdChecksumHigh;
        DimmManId = HpobCurrentBootDimmIdInfo[Socket][Channel][Dimm].DimmManId;
        DimmModId = HpobCurrentBootDimmIdInfo[Socket][Channel][Dimm].DimmModId;
        DimmPresentInConfig = HpobCurrentBootDimmIdInfo[Socket][Channel][Dimm].DimmPresentInConfig;
        IDS_HDT_CONSOLE_PSP_TRACE ("\tDimmPresentInConfig: %x\n", DimmPresentInConfig);
        IDS_HDT_CONSOLE_PSP_TRACE ("\tPage1SpdChecksumLow: %x\n", Page1SpdChecksumLow);
        IDS_HDT_CONSOLE_PSP_TRACE ("\tPage1SpdChecksumHigh: %x\n", Page1SpdChecksumHigh);
        IDS_HDT_CONSOLE_PSP_TRACE ("\tDimmManId: %x\n", DimmManId);
        IDS_HDT_CONSOLE_PSP_TRACE ("\tDimmModId: %x\n", DimmModId);
      }
    }
  }
}

VOID
UpdateHpcbDimmConfigInfoId (
  )
{
  HPOB_HPCB_BOOT_INFO_STRUCT  *HpobEntry;
  EFI_STATUS                  Status;
  UINT64                      HpcbPtr;
  UINT32                      HpcbEntrySize;
  UINT64                      HpcbEntryDest;

  DIMM_HPOB_OPT_ID          (*HpobCurrentBootDimmIdInfo)[HPOB_MAX_CHANNELS_PER_SOCKET_HYGX][HPOB_MAX_DIMMS_PER_CHANNEL];
  UINT32                    TypeDataSize;
  UINT8                     *TypeDataStream;
  UINT32                    HpcbDimmConfigInfoIdSize;
  DIMM_HPCB_INFO_CONFIG_ID  *HpcbDimmConfigInfoId;
  UINT32                    i;
  UINT32                    Socket;
  UINT32                    Channel;
  UINT32                    Dimm;
  UINT8                     *NewHpcbPtr;
  TYPE_ATTRIB               TypeAttrib;
  BOOLEAN                   Succeed;

  IDS_HDT_CONSOLE_PSP_TRACE ("\nUpdateHpcbDimmConfigInfoId\n");
  //
  // Get the data from HPOB
  //
  HpobEntry = NULL;
  Status    = HygonPspGetHpobEntryInstance (HPOB_GROUP_MEM, HPOB_HPCB_BOOT_INFO_TYPE, 0, FALSE, (HPOB_TYPE_HEADER **)&HpobEntry);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get HPOB_HPCB_BOOT_INFO_TYPE\n");
    return;
  }

  // Check the existence and size of HPCB entry 0x60
  Succeed = BIOSEntryInfo (BIOS_HPCB_INFO, INSTANCE_IGNORED, &TypeAttrib, &HpcbPtr, &HpcbEntrySize, &HpcbEntryDest);
  if (Succeed == FALSE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get HPCB Entry in BIOS DIR\n");
    return;
  }

  //
  // Compose the new HPCB type data
  //
  HpobCurrentBootDimmIdInfo = HpobEntry->CurrentBootDimmIdInfo;
  // Get the size of HPCB_MEM_TYPE_DIMM_CONFIG_INFO_ID type
  TypeDataSize = 0;
  Status = HpcbGetType (HPCB_GROUP_MEMORY, HPCB_MEM_TYPE_DIMM_CONFIG_INFO_ID, 0, NULL, &TypeDataSize);
  if (TypeDataSize == 0) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get the TypeDataSize\n");
    return;
  }

  TypeDataStream = AllocateZeroPool (TypeDataSize);
  ASSERT (TypeDataStream != NULL);
  if (TypeDataStream == NULL) {
    return;
  }

  Status = HpcbGetType (HPCB_GROUP_MEMORY, HPCB_MEM_TYPE_DIMM_CONFIG_INFO_ID, 0, TypeDataStream, &TypeDataSize);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get the TypeDataStream for HPCB_MEM_TYPE_DIMM_CONFIG_INFO_ID\n");
    return;
  }

  HpcbDimmConfigInfoIdSize = TypeDataSize / sizeof (DIMM_HPCB_INFO_CONFIG_ID);
  HpcbDimmConfigInfoId     = (DIMM_HPCB_INFO_CONFIG_ID *)TypeDataStream;
  // DumpHpcbDimmConfigInfoId (HpcbDimmConfigInfoId, HpcbDimmConfigInfoIdSize);
  // DumpHpobCurrentBootDimmIdInfo (HpobCurrentBootDimmIdInfo);
  IDS_HDT_CONSOLE_PSP_TRACE ("Total %d HpcbDimmConfigInfo found\n", HpcbDimmConfigInfoIdSize);
  for (i = 0; i < HpcbDimmConfigInfoIdSize; i++) {
    // Search according entry in HPOB
    for (Socket = 0; Socket < HPOB_MAX_SOCKETS_SUPPORTED; Socket++) {
      for (Channel = 0; Channel < HPOB_MAX_CHANNELS_PER_SOCKET_HYGX; Channel++) {
        for (Dimm = 0; Dimm < HPOB_MAX_DIMMS_PER_CHANNEL; Dimm++) {
          if (HpcbDimmConfigInfoId[i].Socket == Socket) {
            if (HpcbDimmConfigInfoId[i].Channel == Channel) {
              if (HpcbDimmConfigInfoId[i].Dimm == Dimm) {
                if (HpobCurrentBootDimmIdInfo[Socket][Channel][Dimm].DimmPresentInConfig) {
                  IDS_HDT_CONSOLE_PSP_TRACE ("Update HpcbDimmConfigInfoId [%d] from HpobCurrentBootDimmIdInfo [%d][%d][%d]\n", i, Socket, Channel, Dimm);
                  HpcbDimmConfigInfoId[i].PrevPage1SpdChecksumLow  = HpobCurrentBootDimmIdInfo[Socket][Channel][Dimm].Page1SpdChecksumLow;
                  HpcbDimmConfigInfoId[i].PrevPage1SpdChecksumHigh = HpobCurrentBootDimmIdInfo[Socket][Channel][Dimm].Page1SpdChecksumHigh;
                  HpcbDimmConfigInfoId[i].PrevDimmManId = HpobCurrentBootDimmIdInfo[Socket][Channel][Dimm].DimmManId;
                  HpcbDimmConfigInfoId[i].PrevDimmModId = HpobCurrentBootDimmIdInfo[Socket][Channel][Dimm].DimmModId;
                  HpcbDimmConfigInfoId[i].DimmPresentInConfig = 1;
                } else {
                  IDS_HDT_CONSOLE_PSP_TRACE ("Clear HpcbDimmConfigInfoId [%d] for SCD[%d][%d][%d]\n", i, Socket, Channel, Dimm);
                  HpcbDimmConfigInfoId[i].PrevPage1SpdChecksumLow  = 0;
                  HpcbDimmConfigInfoId[i].PrevPage1SpdChecksumHigh = 0;
                  HpcbDimmConfigInfoId[i].PrevDimmManId = 0;
                  HpcbDimmConfigInfoId[i].PrevDimmModId = 0;
                  HpcbDimmConfigInfoId[i].DimmPresentInConfig = 0;
                }
              }
            }
          }
        }
      }
    }
  }

  //
  // Replace the exist entry with new composed one
  //
  // Allocate buffer for new HPCB data
  NewHpcbPtr = NULL;
  NewHpcbPtr = AllocateZeroPool (HpcbEntrySize);
  ASSERT (NewHpcbPtr != NULL);
  if (NewHpcbPtr != NULL) {
    // Replace Type stream data & write back
    Status = HpcbReplaceType (HPCB_GROUP_MEMORY, HPCB_MEM_TYPE_DIMM_CONFIG_INFO_ID, 0, (UINT8 *)HpcbDimmConfigInfoId, TypeDataSize, NewHpcbPtr);
    ASSERT (Status == EFI_SUCCESS);
    if (Status == EFI_SUCCESS) {
      HygonPspWriteBackHpcbShadowCopy ();
    }
  }
}

VOID
FlashAccSmmReadyCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;
  VOID        *Interface;

  HGPI_TESTPOINT (TpPspDxeFlashAccCallBackEntry, NULL);
  //
  // Try to locate it because EfiCreateProtocolNotifyEvent will trigger it once when registration.
  // Just return if it is not found.
  //
  Status = gBS->LocateProtocol (
                  &gPspFlashAccSmmCommReadyProtocolGuid,
                  NULL,
                  &Interface
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  // Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  // Skip below steps if recovery flag set
  if (CheckPspRecoveryFlag () == FALSE) {
    // Do HPCB recovery if HPOB.HPCBRecovery Flag set
    HpcbRecovery ();
    // Update DimmConfigInfoId in HPCB
    UpdateHpcbDimmConfigInfoId ();
    // Save HPOB to NV
    PspWriteHpobS3Entry ();
  }

  HGPI_TESTPOINT (TpPspDxeFlashAccCallBackExit, NULL);
}

/**
 *  Close Psp Smm Communication Interface
 *
 *
 *  @retval EFI_SUCCESS       The Data save to FLASH successfully
 *
 **/
EFI_STATUS
ClosePspSmmCommunication (
  )
{
  EFI_STATUS                      Status;
  UINT8                           *Buffer;
  EFI_SMM_COMMUNICATE_HEADER      *SmmCommBuff;
  PSP_SMM_COMMUNICATION_CMN       *PspSmmCloseInterfaceBuff;
  UINTN                           SmmCommBufferSize;
  EFI_SMM_COMMUNICATION_PROTOCOL  *SmmCommunication = NULL;

  IDS_HDT_CONSOLE_PSP_TRACE ("ClosePspSmmCommunication\n");
  // Assume gEfiSmmCommunicationProtocolGuid should be ready at ExitBs
  Status = gBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **)&SmmCommunication);
  ASSERT (Status == EFI_SUCCESS);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Locate SmmCommunicationProtocol fail \n");
    return Status;
  }

  // Init SMM communication buffer header
  Buffer = (UINT8 *)(UINTN)PcdGet64 (PcdHygonSmmCommunicationAddress);
  ASSERT (Buffer != NULL);
  if (Buffer != NULL) {
    ZeroMem (Buffer, PSP_CMM_COMM_CLOSE_INTERFACE_BUFFER_SIZE);
    SmmCommBuff = (EFI_SMM_COMMUNICATE_HEADER *)Buffer;
    CopyGuid (&SmmCommBuff->HeaderGuid, &gPspSmmCommHandleGuid);
    SmmCommBuff->MessageLength = sizeof (PSP_SMM_COMMUNICATION_CMN);
    // Init PSP SMM communicate private data
    PspSmmCloseInterfaceBuff     = (PSP_SMM_COMMUNICATION_CMN *)&(((EFI_SMM_COMMUNICATE_HEADER *)Buffer)->Data);
    PspSmmCloseInterfaceBuff->id = PSP_SMM_COMM_ID_CLOSE_INTERFACE;

    // Communicate HYGON SMM communication handler to close the FlashACC SMI interface
    SmmCommBufferSize = PSP_CMM_COMM_CLOSE_INTERFACE_BUFFER_SIZE;
    SmmCommunication->Communicate (SmmCommunication, Buffer, &SmmCommBufferSize);
  }

  return EFI_SUCCESS;
}

/*++

Routine Description:

  This function is invoked by gEfiEventReadyToBootGuid.
  Before booting to legacy OS, inform PSP that BIOS is transitioning
  preparing for S3 usage.

Arguments:

  Event   - The triggered event.
  Context - Context for this event.

Returns:

  None

--*/
VOID
PspDxeReadyToBoot (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  HGPI_TESTPOINT (TpPspDxeRTBCallbackEntry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("PspDxeReadyToBoot\n");

  // Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  PspNotifySmmDrivers ();
  HGPI_TESTPOINT (TpPspDxeRTBCallbackExit, NULL);
}

VOID
PspDxeExitBS (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  HGPI_TESTPOINT (TpPspDxeExitBsCallbackEntry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("PspDxeExitBS\n");

  // Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  // Close PSP SMM communication interface mainly Flash Access to void security vulnerability
  ClosePspSmmCommunication ();
  PspMboxBiosCmdExitBootServices ();
  HGPI_TESTPOINT (TpPspDxeExitBsCallbackExit, NULL);
}

VOID
PspPciEnumerationCompleteCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;
  VOID        *Interface;

  //
  // Try to locate it because EfiCreateProtocolNotifyEvent will trigger it once when registration.
  // Just return if it is not found.
  //
  Status = gBS->LocateProtocol (
                  &gEfiPciEnumerationCompleteProtocolGuid,
                  NULL,
                  &Interface
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  // Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  PspMboxBiosLockDFReg ();
}

EFI_STATUS
EFIAPI
HygonPspDxeDriverEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_EVENT   Event;
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;
  VOID        *Registration;
  UINT8       *Buffer;

  HGPI_TESTPOINT (TpPspDxeEntry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.Drv.HygonPspDxe Enter\n");
  Handle = NULL;
  // Install PSP Common service Protocol
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonPspCommonServiceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPspCommonServiceProtocol
                  );
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
  }

  // Always PSP ADDR MSR for RdRand Instruction
  if (HygonCapsuleGetStatus () == FALSE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\tCreate MpProtocol event for RdRand\n");
    Event = NULL;
    Event = EfiCreateProtocolNotifyEvent (
              &gEfiMpServiceProtocolGuid,
              TPL_NOTIFY,
              PspMpServiceCallBack,
              NULL,
              &Registration
              );
    ASSERT (Event != NULL);
    if (Event == NULL) {
      IDS_HDT_CONSOLE_PSP_TRACE ("\tCreate event fail\n");
    }

    IDS_HDT_CONSOLE_PSP_TRACE ("\tCreate Protocol event for Install FlashAcc Protocol\n");
    Event = NULL;
    Event = EfiCreateProtocolNotifyEvent (
              &gPspFlashAccSmmCommReadyProtocolGuid,
              TPL_NOTIFY,
              FlashAccSmmReadyCallBack,
              NULL,
              &Registration
              );
    ASSERT (Event != NULL);
    if (Event == NULL) {
      IDS_HDT_CONSOLE_PSP_TRACE ("\tCreate event fail\n");
    }
  }

  // Allocate RT buffer for Smm communication before EndOfDxe Event to satisfie WSMT test
  Buffer = NULL;
  Buffer = AllocateRuntimePool (HYGON_SMM_COMMUNICATION_BUFFER_SIZE);
  ASSERT (Buffer != NULL);
  // Save to PCD database
  if (Buffer != NULL) {
    PcdSet64S (PcdHygonSmmCommunicationAddress, (UINT64)(UINTN)Buffer);
  }

  // If PSP feature turn off, exit the driver
  if ((CheckPspDevicePresentV2 () == FALSE) ||
      (PcdGetBool (PcdHygonPspEnable) == FALSE)) {
    return EFI_SUCCESS;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("\tCreate PciEnumerationCompleteProtocol callback\n");
  Event = NULL;
  Event = EfiCreateProtocolNotifyEvent (
            &gEfiPciEnumerationCompleteProtocolGuid,
            TPL_NOTIFY,
            PspPciEnumerationCompleteCallBack,
            NULL,
            &Registration
            );
  ASSERT (Event != NULL);
  if (Event == NULL) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\tCreate event fail\n");
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("\tCreate ReadyToBoot Event\n");
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  PspDxeReadyToBoot,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &Event
                  );

  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return (Status);
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("\tCreate ExitBootServicesEvent\n");
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  PspDxeExitBS,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &Event
                  );

  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return (Status);
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspDxe Exit\n");
  HGPI_TESTPOINT (TpPspDxeExit, NULL);
  return EFI_SUCCESS;
}
