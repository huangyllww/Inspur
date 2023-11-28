/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
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
#include <Library/AmdCapsuleLib.h>
#include <Protocol/SmmControl2.h>
#include <Protocol/MpService.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciEnumerationComplete.h>
#include <Protocol/SmmCommunication.h>

#include <Protocol/AmdPspCommonServiceProtocol.h>
#include <AGESA.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/AmdPspMboxLibV2.h>
#include <Library/ApcbLib.h>
#include <Library/AmdPspApobLib.h>
#include <Protocol/PspPlatformProtocol.h>
#include <Library/CcxPspLib.h>
#include <Library/AmdPspFlashUpdateLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/SmnAccessLib.h>
#include <AmdPspSmmCommunication.h>
#include <Filecode.h>

#define FILECODE PSP_AMDPSPDXEV2_AMDPSPDXEV2_FILECODE

///@todo move below assert definition to Ids header file
#define ASSERT_EFI_ERROR(x) ASSERT (!EFI_ERROR (x))
#define APOB_UNIQUE_APCB_INSTANCE_INVALID_VALUE     0xDEADBEEFul

#define AMD_SMM_COMMUNICATION_BUFFER_SIZE (4 * 1024)

extern EFI_GUID gPspFlashAccSmmCommReadyProtocolGuid;

EFI_SMM_CONTROL2_PROTOCOL              *mSmmControl;
PSP_PLATFORM_PROTOCOL                 *mPspPlatfromProtocol;

STATIC AMD_PSP_COMMON_SERVICE_PROTOCOL mPspCommonServiceProtocol = {
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

  UINT8 Cmd;
  EFI_STATUS Status;
  //Below two protocols haven't been added to driver's depex, assume it should be ready at RTB
  Status = gBS->LocateProtocol (&gEfiSmmControl2ProtocolGuid, NULL, &mSmmControl);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Locate SmmCtrl Protocol Fail\n");
    return;
  }

  //Locate Psp Platform Protocol to get customized information
  IDS_HDT_CONSOLE_PSP_TRACE ("Locate Psp Platform Protocol\n");
  Status = gBS->LocateProtocol (
                    &gPspPlatformProtocolGuid,
                    NULL,
                    &mPspPlatfromProtocol
                    );
  ASSERT_EFI_ERROR (Status);
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
  UINT64 Tmp64;
  UINT32 RegEax = 0;
  UINT32 RegEbx = 0;
  UINT32 RegEcx = 0;
  UINT32 RegEdx = 0;
  UINT32 NodeNum = 0;
  UINT64 PspAddr = 0;
  UINT32 RegValue = 0;
  UINT32 HostBridgeBusBase;
  UINT32 SocketId;
  UINT32 DieId;
  
  //Determine node #
  AsmCpuid(0x8000001E, &RegEax, &RegEbx, &RegEcx, &RegEdx);
  
  //
  // 0x8000001E.Ecx: bit 8~10: NodesPerProcessor, bit2: socket id, bit0~1: die id
  //
  NodeNum           = RegEcx & 0x7FF;
  SocketId          = (NodeNum & 0x4) >> 2;
  DieId             = NodeNum & 0x3;
  HostBridgeBusBase = (UINT32)FabricTopologyGetHostBridgeBusBase (SocketId, DieId, 0);
          
  //Read PSP BASE Address in NBMISC
  SmnRegisterRead(HostBridgeBusBase, 0x13B102E4, &RegValue);
  PspAddr = RegValue;
  PspAddr = PspAddr << 32;
  SmnRegisterRead(HostBridgeBusBase, 0x13B102E0, &RegValue);
  RegValue = RegValue & 0xFFF00000;
  PspAddr = PspAddr | RegValue;
     
  //Get PSP BAR Address
  //if (GetPspMmioBase (&PspAddr) == FALSE) {
  //  return;
  //}
  Tmp64 = 0;
  Tmp64 = PspAddr;

  UpdatePspAddr (Tmp64);
}


EFI_STATUS
SetPspAddrMsr (
  )
{
  EFI_STATUS  Status;
  VOID        *UnusedBufPtr;
  EFI_MP_SERVICES_PROTOCOL *MpServices;

  IDS_HDT_CONSOLE_PSP_TRACE ("Locate MP Protocol\n");
  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);

  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Set PSPADDR MSR\n");
  UnusedBufPtr = NULL;
  SetPspAddrMsrTask (UnusedBufPtr);
  if(PcdGetBool(PcdPspBaseSetAtAgesaEnable))
  {
      Status = MpServices->StartupAllAPs (
                            MpServices,                                    // EFI_MP_SERVICES_PROTOCOL *this
                            (EFI_AP_PROCEDURE)SetPspAddrMsrTask,              // EFI_AP_PROCEDURE
                            TRUE,                                          // BOOLEAN SingleThreaded? FALSE=execute all functions at the same time on all CPUs
                            NULL,                                          // EFI_EVENT WaitEvent OPTIONAL
                            0,                                             // UINTN Timeout (Unsupported)
                            NULL,                                          // VOID *ProcArguments OPTIONAL
                            NULL                                           // Failed CPUList OPTIONAL (unsupported)
                            );
  }
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

  AGESA_TESTPOINT (TpPspDxeV2MpCallBackEntry, NULL);
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
    return ;
  }

  //Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }
  SetPspAddrMsr ();
  AGESA_TESTPOINT (TpPspDxeV2MpCallBackExit, NULL);
}

/**
 *  Prepare the APOB Buffer and write to BIOS Directory Entry 0x63
 *
 *
 *  @retval EFI_SUCCESS       The Data save to FLASH successfully
 *
 **/
EFI_STATUS
PspWriteApobS3Entry (
  VOID
  )
{
  EFI_STATUS                      Status;
  APOB_HEADER                     *ApobDramPtr;
  BOOLEAN                         ApobEntryExist;
  TYPE_ATTRIB                     TypeAttrib;
  UINT64                          EntryAddress;
  UINT32                          EntrySize;
  UINT64                          EntryDest;
  APOBLIB_INFO                    *ApobInfo;
  BOOLEAN                         DimmCfgUpdatedFlag;
  APOB_GEN_INFO_TYPE_STRUCT       *ApobGenInfoType;
  UINT32                          PreApobUniqueApcbInstance;
  UINT32                          CurApobUniqueApcbInstance;
  IDS_HDT_CONSOLE_PSP_TRACE ("\nPspWriteApobS3Entry\n");

  DimmCfgUpdatedFlag = FALSE;
  PreApobUniqueApcbInstance = APOB_UNIQUE_APCB_INSTANCE_INVALID_VALUE;
  AmdPspGetDimmCfgUpdatedFlag (&DimmCfgUpdatedFlag);
  //Getting from the APOB SPI copy
  Status = AmdPspGetApobEntryInstance (APOB_GEN, APOB_GEN_CONFIGURATION_INFO_TYPE, 0, TRUE, (APOB_TYPE_HEADER **) &ApobGenInfoType);
  if (Status == EFI_SUCCESS) {
    PreApobUniqueApcbInstance = ApobGenInfoType->ApobParamInfo.ApobUniqueApcbInstance;
  }

  //Getting from the APOB DRAM copy
  Status = AmdPspGetApobEntryInstance (APOB_GEN, APOB_GEN_CONFIGURATION_INFO_TYPE, 0, FALSE, (APOB_TYPE_HEADER **) &ApobGenInfoType);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get APOB_GEN_CONFIGURATION_INFO_TYPE from DRAM\n");
    return EFI_UNSUPPORTED;
  }
  CurApobUniqueApcbInstance = ApobGenInfoType->ApobParamInfo.ApobUniqueApcbInstance;
  IDS_HDT_CONSOLE_PSP_TRACE ("PreApobUniqueApcbInstance %x, CurApobUniqueApcbInstance %x\n", PreApobUniqueApcbInstance, CurApobUniqueApcbInstance);

  //No Dimm update and No APCB changes versus the time saving APOB
  if ((DimmCfgUpdatedFlag == FALSE) && (PreApobUniqueApcbInstance == CurApobUniqueApcbInstance)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("No Dimm cfg & APCB update, skip the write\n");
    return EFI_UNSUPPORTED;
  }
  if (AmdPspGetApobInfo (&ApobInfo) != EFI_SUCCESS) {
    IDS_HDT_CONSOLE_PSP_TRACE ("No APOB memory copy detected\n");
    return EFI_UNSUPPORTED;
  }

  ApobDramPtr = (APOB_HEADER *) (UINTN) ApobInfo->ApobAddr;

  //Check the existence and size of APOB NV entry 0x63
  ApobEntryExist = BIOSEntryInfo (APOB_NV_COPY, INSTANCE_IGNORED, &TypeAttrib, &EntryAddress, &EntrySize, &EntryDest);
  if (!ApobEntryExist) {
    IDS_HDT_CONSOLE_PSP_TRACE ("PspWriteApobS3Entry Entry not exist\n");
    return EFI_UNSUPPORTED;
  }
  //Check the size of APOB DRAM copy
  if (ApobDramPtr->Size > EntrySize) {
    IDS_HDT_CONSOLE_PSP_TRACE ("APOB SPI Entry too small\n");
    return EFI_UNSUPPORTED;
  }
  //Check Dimm configuration updated, if yes update APOB NV storage at SPI
  IDS_HDT_CONSOLE_PSP_TRACE ("Update APOB SPI copy\n");
  PspUpdateFlash ((UINT32) EntryAddress, ApobDramPtr->Size, ApobDramPtr);
  return EFI_SUCCESS;
}

VOID
ApcbRecovery (
  )
{
  EFI_STATUS        Status;
  BOOLEAN           ApcbRecoveryFlag;
  UINT8             ApcbInstance;
  TYPE_ATTRIB       TypeAttrib;
  UINT64            ActiveApcbEntryAddress;
  UINT64            BackupApcbEntryAddress;
  UINT32            ActiveApcbSize;
  UINT32            BackUpApcbSize;
  UINT64            EntryDest;
  UINT8             *Buffer;
  APCB_HEADER       *ApcbHeader;
  UINT8             Value8;


  ApcbRecoveryFlag = FALSE;
  Status = AmdPspGetApobApcbRecoveryFlag (&ApcbRecoveryFlag);
  ASSERT (Status == EFI_SUCCESS);
  if (ApcbRecoveryFlag) {
    IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set\n");
    //Query APCB instace
    ApcbInstance = 0;
    Status = AmdPspGetApobApcbInstance (&ApcbInstance);
    ASSERT (Status == EFI_SUCCESS);
    IDS_HDT_CONSOLE_PSP_TRACE ("Recover Active APCB Instance %x\n", ApcbInstance);
    //Query APCB Backup instace information
    if (BIOSEntryInfo (BIOS_APCB_INFO_BACKUP, ApcbInstance, &TypeAttrib, &BackupApcbEntryAddress, &BackUpApcbSize, &EntryDest) == FALSE) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Fatal Error: Backup Apcb Not Detected\n");
      ASSERT (FALSE);
      AGESA_TESTPOINT (TpPspRecoverApcbFail, NULL);
      return;
    }
    //Query APCB active instace information
    if (BIOSEntryInfo (BIOS_APCB_INFO, ApcbInstance, &TypeAttrib, &ActiveApcbEntryAddress, &ActiveApcbSize, &EntryDest) == FALSE) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Fatal Error: Active Apcb instance Not Detected\n");
      ASSERT (FALSE);
      AGESA_TESTPOINT (TpPspRecoverApcbFail, NULL);
      return;
    }
    //Do size check between APCB active instace & APCB Backup instace
    ASSERT (ActiveApcbSize >= BackUpApcbSize);
    if (ActiveApcbSize < BackUpApcbSize) {
      IDS_HDT_CONSOLE_PSP_TRACE ("ActiveApcbSize too small\n");
      ASSERT (FALSE);
      AGESA_TESTPOINT (TpPspRecoverApcbFail, NULL);
      return;
    }
    //Start APCB recovery
    //Allocate temp memory to hold backup apcb
    Buffer = NULL;
    Buffer = AllocateZeroPool (BackUpApcbSize);
    ASSERT (Buffer != NULL);
    if (Buffer == NULL) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Allocate temp buffer fail\n");
      return;
    }
    CopyMem (Buffer, (VOID *) (UINTN) BackupApcbEntryAddress, BackUpApcbSize);
    //Fill the random value to UniqueApcbInstance
    ApcbHeader = (APCB_HEADER *) Buffer;
    IDS_HDT_CONSOLE_PSP_TRACE ("UniqueApcbInstance Update from %x", ApcbHeader->UniqueApcbInstance);
    ApcbHeader->UniqueApcbInstance = (UINT32) (AsmReadTsc () & 0x00FFFFFFul);
    IDS_HDT_CONSOLE_PSP_TRACE (" to  %x\n", ApcbHeader->UniqueApcbInstance);
    ApcbHeader->CheckSumByte = 0;
    ApcbHeader->CheckSumByte = (UINT8) (0x100 - ApcbCalcCheckSum (ApcbHeader, ApcbHeader->SizeOfApcb));

    //Restore APCB active instance SPI region
    IDS_HDT_CONSOLE_PSP_TRACE ("Restore APCB active instance SPI region [0x%x] with 0x%x bytes from Buffer [0x%x]\n", ActiveApcbEntryAddress, BackUpApcbSize, Buffer);
    PspUpdateFlash ((UINT32) ActiveApcbEntryAddress, BackUpApcbSize, Buffer);
    IDS_HDT_CONSOLE_PSP_TRACE ("Restore Completed, Restart the system");
    //Update the CMOS[6:7] with flag 0xA55A to indicate the APCB recovery succeed
    Value8 = 0x06;
    LibAmdIoWrite (AccessWidth8, 0x72, &Value8, NULL);
    Value8 = 0x5A;
    LibAmdIoWrite (AccessWidth8, 0x73, &Value8, NULL);

    Value8 = 0x07;
    LibAmdIoWrite (AccessWidth8, 0x72, &Value8, NULL);
    Value8 = 0xA5;
    LibAmdIoWrite (AccessWidth8, 0x73, &Value8, NULL);

    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
    //Should not come here
    CpuDeadLoop ();
  }
}

VOID
DumpApcbDimmConfigInfoId (
  DIMM_APCB_INFO_CONFIG_ID        *ApcbDimmConfigInfoId,
  UINT32                          ApcbDimmConfigInfoIdSize
  )
{
  UINT32 i;
  IDS_HDT_CONSOLE_PSP_TRACE ("DumpApcbDimmConfigInfoId\n");
  for (i = 0; i < ApcbDimmConfigInfoIdSize; i++) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Index:%d\n", i);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tSocket: %x\n", ApcbDimmConfigInfoId[i].Socket);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tChannel: %x\n", ApcbDimmConfigInfoId[i].Channel);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tDimm: %x\n", ApcbDimmConfigInfoId[i].Dimm);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPrevPage1SpdChecksumLow: %x\n", ApcbDimmConfigInfoId[i].PrevPage1SpdChecksumLow);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPrevPage1SpdChecksumHigh: %x\n", ApcbDimmConfigInfoId[i].PrevPage1SpdChecksumHigh);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPrevDimmManId: %x\n", ApcbDimmConfigInfoId[i].PrevDimmManId);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPrevDimmModId: %x\n", ApcbDimmConfigInfoId[i].PrevDimmModId);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tDimmPresentInConfig: %x\n", ApcbDimmConfigInfoId[i].DimmPresentInConfig);
  }
}

VOID
DumpApobCurrentBootDimmIdInfo (
  DIMM_APOB_OPT_ID                (*ApobCurrentBootDimmIdInfo) [ABL_APOB_MAX_CHANNELS_PER_SOCKET][ABL_APOB_MAX_DIMMS_PER_CHANNEL]
  )
{
  UINT32                          Socket;
  UINT32                          Channel;
  UINT32                          Dimm;
  UINT32 Page1SpdChecksumLow;       ///< DIMM checksum for Page 1
  UINT32 Page1SpdChecksumHigh;       ///< DIMM checksum for Page 1
  UINT32 DimmManId;     ///< DIMM manufacturer ID
  UINT32 DimmModId;       ///< DIMM Module ID
  UINT32 DimmPresentInConfig;  ///< Indicates that the DIMM config is present

  IDS_HDT_CONSOLE_PSP_TRACE ("DumpApobCurrentBootDimmIdInfo\n");
  for (Socket = 0; Socket < ABL_APOB_MAX_SOCKETS_SUPPORTED; Socket++) {
    for (Channel = 0; Channel < ABL_APOB_MAX_CHANNELS_PER_SOCKET; Channel++) {
      for (Dimm = 0; Dimm < ABL_APOB_MAX_DIMMS_PER_CHANNEL; Dimm++) {
        IDS_HDT_CONSOLE_PSP_TRACE ("\n\tSocket: %x\n", Socket);
        IDS_HDT_CONSOLE_PSP_TRACE ("\tChannel: %x\n", Channel);
        IDS_HDT_CONSOLE_PSP_TRACE ("\tDimm: %x\n", Dimm);
        Page1SpdChecksumLow = ApobCurrentBootDimmIdInfo[Socket][Channel][Dimm].Page1SpdChecksumLow;
        Page1SpdChecksumHigh = ApobCurrentBootDimmIdInfo[Socket][Channel][Dimm].Page1SpdChecksumHigh;
        DimmManId = ApobCurrentBootDimmIdInfo[Socket][Channel][Dimm].DimmManId;
        DimmModId = ApobCurrentBootDimmIdInfo[Socket][Channel][Dimm].DimmModId;
        DimmPresentInConfig = ApobCurrentBootDimmIdInfo[Socket][Channel][Dimm].DimmPresentInConfig;
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
UpdateApcbDimmConfigInfoId (
  )
{
  APOB_APCB_BOOT_INFO_STRUCT      *ApobEntry;
  EFI_STATUS                      Status;
  UINT64                          ApcbPtr;
  UINT32                          ApcbEntrySize;
  UINT64                          ApcbEntryDest;
  DIMM_APOB_OPT_ID                (*ApobCurrentBootDimmIdInfo) [ABL_APOB_MAX_CHANNELS_PER_SOCKET][ABL_APOB_MAX_DIMMS_PER_CHANNEL];
  UINT32                          TypeDataSize;
  UINT8                           *TypeDataStream;
  UINT32                          ApcbDimmConfigInfoIdSize;
  DIMM_APCB_INFO_CONFIG_ID        *ApcbDimmConfigInfoId;
  UINT32                          i;
  UINT32                          Socket;
  UINT32                          Channel;
  UINT32                          Dimm;
  UINT8                           *NewApcbPtr;
  TYPE_ATTRIB                     TypeAttrib;
  BOOLEAN                         Succeed;


  IDS_HDT_CONSOLE_PSP_TRACE ("\nUpdateApcbDimmConfigInfoId\n");
  //
  // Get the data from APOB
  //
  ApobEntry = NULL;
  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_APCB_BOOT_INFO_TYPE, 0, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get APOB_APCB_BOOT_INFO_TYPE\n");
    return;
  }
  // Check the existence and size of APCB entry 0x60
  Succeed = BIOSEntryInfo (BIOS_APCB_INFO, INSTANCE_IGNORED, &TypeAttrib, &ApcbPtr, &ApcbEntrySize, &ApcbEntryDest);
  if (Succeed == FALSE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get APCB Entry in BIOS DIR\n");
    return;
  }
  //
  // Compose the new APCB type data
  //
  ApobCurrentBootDimmIdInfo = ApobEntry->CurrentBootDimmIdInfo;
  //Get the size of APCB_MEM_TYPE_DIMM_CONFIG_INFO_ID type
  TypeDataSize = 0;
  Status = ApcbGetType (APCB_GROUP_MEMORY , APCB_MEM_TYPE_DIMM_CONFIG_INFO_ID, 0, NULL, &TypeDataSize);
  if (TypeDataSize == 0) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get the TypeDataSize\n");
    return;
  }
  TypeDataStream = AllocateZeroPool (TypeDataSize);
  ASSERT (TypeDataStream != NULL);

  Status = ApcbGetType (APCB_GROUP_MEMORY , APCB_MEM_TYPE_DIMM_CONFIG_INFO_ID, 0, TypeDataStream, &TypeDataSize);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get the TypeDataStream for APCB_MEM_TYPE_DIMM_CONFIG_INFO_ID\n");
    return;
  }
  ApcbDimmConfigInfoIdSize = TypeDataSize / sizeof (DIMM_APCB_INFO_CONFIG_ID);
  ApcbDimmConfigInfoId = (DIMM_APCB_INFO_CONFIG_ID *) TypeDataStream;
  // DumpApcbDimmConfigInfoId (ApcbDimmConfigInfoId, ApcbDimmConfigInfoIdSize);
  // DumpApobCurrentBootDimmIdInfo (ApobCurrentBootDimmIdInfo);
  IDS_HDT_CONSOLE_PSP_TRACE ("Total %d ApcbDimmConfigInfo found\n", ApcbDimmConfigInfoIdSize);
  for (i = 0; i < ApcbDimmConfigInfoIdSize; i++) {
    //Search according entry in APOB
    for (Socket = 0; Socket < ABL_APOB_MAX_SOCKETS_SUPPORTED; Socket++) {
      for (Channel = 0; Channel < ABL_APOB_MAX_CHANNELS_PER_SOCKET; Channel++) {
        for (Dimm = 0; Dimm < ABL_APOB_MAX_DIMMS_PER_CHANNEL; Dimm++) {
          if (ApcbDimmConfigInfoId[i].Socket == Socket) {
            if (ApcbDimmConfigInfoId[i].Channel == Channel) {  
              if (ApcbDimmConfigInfoId[i].Dimm == Dimm) {
                if (ApobCurrentBootDimmIdInfo[Socket][Channel][Dimm].DimmPresentInConfig) {
                  IDS_HDT_CONSOLE_PSP_TRACE ("Update ApcbDimmConfigInfoId [%d] from ApobCurrentBootDimmIdInfo [%d][%d][%d]\n", i, Socket, Channel, Dimm);
                  ApcbDimmConfigInfoId[i].PrevPage1SpdChecksumLow = ApobCurrentBootDimmIdInfo[Socket][Channel][Dimm].Page1SpdChecksumLow;
                  ApcbDimmConfigInfoId[i].PrevPage1SpdChecksumHigh = ApobCurrentBootDimmIdInfo[Socket][Channel][Dimm].Page1SpdChecksumHigh;
                  ApcbDimmConfigInfoId[i].PrevDimmManId = ApobCurrentBootDimmIdInfo[Socket][Channel][Dimm].DimmManId;
                  ApcbDimmConfigInfoId[i].PrevDimmModId = ApobCurrentBootDimmIdInfo[Socket][Channel][Dimm].DimmModId;
                  ApcbDimmConfigInfoId[i].DimmPresentInConfig = 1;
                } else {
                  IDS_HDT_CONSOLE_PSP_TRACE ("Clear ApcbDimmConfigInfoId [%d] for SCD[%d][%d][%d]\n", i, Socket, Channel, Dimm);
                  ApcbDimmConfigInfoId[i].PrevPage1SpdChecksumLow = 0;
                  ApcbDimmConfigInfoId[i].PrevPage1SpdChecksumHigh = 0;
                  ApcbDimmConfigInfoId[i].PrevDimmManId = 0;
                  ApcbDimmConfigInfoId[i].PrevDimmModId = 0;
                  ApcbDimmConfigInfoId[i].DimmPresentInConfig = 0;
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
  // Allocate buffer for new APCB data
  NewApcbPtr = NULL;
  NewApcbPtr = AllocateZeroPool (ApcbEntrySize);
  ASSERT (NewApcbPtr != NULL);
  if (NewApcbPtr != NULL) {
    // Replace Type stream data & write back
    Status = ApcbReplaceType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DIMM_CONFIG_INFO_ID, 0, (UINT8 *) ApcbDimmConfigInfoId, TypeDataSize, NewApcbPtr);
    ASSERT (Status == EFI_SUCCESS);
    if (Status == EFI_SUCCESS) {
      AmdPspWriteBackApcbShadowCopy ();
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

  AGESA_TESTPOINT (TpPspDxeV2FlashAccCallBackEntry, NULL);
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
    return ;
  }

  //Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }
  //Skip below steps if recovery flag set
  if (CheckPspRecoveryFlagV2 () == FALSE) {
    //Do APCB recovery if APOB.APCBRecovery Flag set
    ApcbRecovery ();
    //Update DimmConfigInfoId in APCB
    UpdateApcbDimmConfigInfoId ();
    //Save APOB to NV
    PspWriteApobS3Entry ();
  }
  AGESA_TESTPOINT (TpPspDxeV2FlashAccCallBackExit, NULL);
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
  EFI_STATUS Status;
  UINT8       *Buffer;
  EFI_SMM_COMMUNICATE_HEADER *SmmCommBuff;
  PSP_SMM_COMMUNICATION_CMN  *PspSmmCloseInterfaceBuff;
  UINTN       SmmCommBufferSize;
  EFI_SMM_COMMUNICATION_PROTOCOL *SmmCommunication = NULL;

  IDS_HDT_CONSOLE_PSP_TRACE ("ClosePspSmmCommunication\n");
  //Assume gEfiSmmCommunicationProtocolGuid should be ready at ExitBs
  Status = gBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &SmmCommunication);
  ASSERT (Status == EFI_SUCCESS);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Locate SmmCommunicationProtocol fail \n");
    return Status;
  }

  //Init SMM communication buffer header
  Buffer = (UINT8 *) (UINTN) PcdGet64 (PcdAmdSmmCommunicationAddress);
  ASSERT (Buffer != NULL);
  if (Buffer != NULL) {
    ZeroMem (Buffer, PSP_CMM_COMM_CLOSE_INTERFACE_BUFFER_SIZE);
    SmmCommBuff = (EFI_SMM_COMMUNICATE_HEADER *) Buffer;
    CopyGuid (&SmmCommBuff->HeaderGuid, &gPspSmmCommHandleGuid);
    SmmCommBuff->MessageLength = sizeof (PSP_SMM_COMMUNICATION_CMN);
    //Init PSP SMM communicate private data
    PspSmmCloseInterfaceBuff = (PSP_SMM_COMMUNICATION_CMN *) &(((EFI_SMM_COMMUNICATE_HEADER *) Buffer)->Data);
    PspSmmCloseInterfaceBuff->id = PSP_SMM_COMM_ID_CLOSE_INTERFACE;

    //Communicate AMD SMM communication handler to close the FlashACC SMI interface
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
  AGESA_TESTPOINT (TpPspDxeV2RTBCallbackEntry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("PspDxeReadyToBoot\n");

  //Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  PspMboxBiosCmdExitBootServices ();
  PspNotifySmmDrivers ();
  AGESA_TESTPOINT (TpPspDxeV2RTBCallbackExit, NULL);
}

VOID
PspDxeExitBS (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  AGESA_TESTPOINT (TpPspDxeV2ExitBsCallbackEntry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("PspDxeExitBS\n");

  //Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }
  //Close PSP SMM communication interface mainly Flash Access to void security vulnerability
  ClosePspSmmCommunication ();
  AGESA_TESTPOINT (TpPspDxeV2ExitBsCallbackExit, NULL);
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
    return ;
  }

  //Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  PspMboxBiosLockDFReg ();

}

EFI_STATUS
EFIAPI
AmdPspDxeV2DriverEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_EVENT                   Event;
  EFI_STATUS                  Status;
  EFI_HANDLE                  Handle;
  VOID                        *Registration;
  UINT8                       *Buffer;

  AGESA_TESTPOINT (TpPspDxeV2Entry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.Drv.AmdPspDxeV2 Enter\n");
  Handle = NULL;
  //Install PSP Common service Protocol
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdPspCommonServiceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPspCommonServiceProtocol
                  );
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
  }

  //Always PSP ADDR MSR for RdRand Instruction
  if (AmdCapsuleGetStatus () == FALSE) {
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

  //Allocate RT buffer for Smm communication before EndOfDxe Event to satisfie WSMT test
  Buffer = NULL;
  Buffer = AllocateRuntimePool (AMD_SMM_COMMUNICATION_BUFFER_SIZE);
  ASSERT (Buffer != NULL);
  //Save to PCD database
  if (Buffer != NULL) {
    PcdSet64 (PcdAmdSmmCommunicationAddress, (UINT64) (UINTN) Buffer);
  }
  //If PSP feature turn off, exit the driver
  if ((CheckPspDevicePresentV2 () == FALSE) ||
      (PcdGetBool (PcdAmdPspEnable) == FALSE)) {
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

  ASSERT_EFI_ERROR (Status);
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

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return (Status);
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspDxeV2 Exit\n");
  AGESA_TESTPOINT (TpPspDxeV2Exit, NULL);
  return EFI_SUCCESS;
}
