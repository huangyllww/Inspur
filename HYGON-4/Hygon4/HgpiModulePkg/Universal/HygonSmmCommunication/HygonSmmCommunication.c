/* $NoKeywords:$ */

/**
 * @file
 *
 * FchPcieWakeWA Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HYGON PSP UEFI Driver
 * @e sub-project   PspSmm Driver
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
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/S3BootScriptLib.h>
#include "Library/HygonBaseLib.h"
#include "Library/IdsLib.h"
#include "HygonSmmCommunication.h"
#include "HygonS3SaveLibData.h"
#include "Filecode.h"

#define FILECODE  PSP_HYGONPSPSMM_HYGONPSPSMM_FILECODE
EFI_HANDLE  mHygonSmmCommunicationHandle;

VOID
HygonS3SaveScriptSmmDispatch (
  IN OUT   VOID    *PrivateDataAddress
  )
{
  HYGON_S3_LIB_PRIVATE_DATA              *HygonS3LibPrivateData;
  HYGON_S3_LIB_BOOT_SCRIPT_TABLE_HEADER  *HygonS3LibTblHdr;
  UINT8                                  *OpData;
  UINT16                                 OpCode;

  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3SaveScriptSmmDispatch Enter\n");

  // Locate the global data structure via PCD
  HygonS3LibPrivateData = NULL;
  HygonS3LibPrivateData = PrivateDataAddress;
  ASSERT (HygonS3LibPrivateData != NULL);

  // Validate the save table
  HygonS3LibTblHdr = (HYGON_S3_LIB_BOOT_SCRIPT_TABLE_HEADER *)HygonS3LibPrivateData->TableBase;
  ASSERT (HygonS3LibTblHdr->Sinature == HYGON_S3LIB_BOOT_SCRIPT_TABLE_SIG);
  if (HygonS3LibTblHdr->Sinature != HYGON_S3LIB_BOOT_SCRIPT_TABLE_SIG) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3LibTbl Signature Check Fail\n");
    return;
  }

  ASSERT (*((UINT32 *)(HygonS3LibPrivateData->TableBase + HygonS3LibTblHdr->TableLength)) == HYGON_S3LIB_TABLE_WATCHER);
  if (*((UINT32 *)(HygonS3LibPrivateData->TableBase + HygonS3LibTblHdr->TableLength)) != HYGON_S3LIB_TABLE_WATCHER) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Potencial memory overflow observed in HygonS3LibTbl\n");
    return;
  }

  // Loop the table, and invoke S3 Script Lib to save the script data
  OpData = &HygonS3LibTblHdr->Data;
  OpCode = ((HYGON_S3LIB_BOOT_SCRIPT_COMMON *)OpData)->OpCode;
  while (OpCode != HYGON_S3LIB_TABLE_END) {
    switch (OpCode) {
      case HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE_OP:
        S3BootScriptSaveIoWrite (
          ((HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE *)OpData)->Width,
          ((HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE *)OpData)->Address,
          1,
          &((HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE *)OpData)->Value
          );
        OpData += sizeof (HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE);
        break;
      case HYGON_S3LIB_BOOT_SCRIPT_IO_RMW_OP:
        S3BootScriptSaveIoReadWrite (
          ((HYGON_S3LIB_BOOT_SCRIPT_IO_RMW *)OpData)->Width,
          ((HYGON_S3LIB_BOOT_SCRIPT_IO_RMW *)OpData)->Address,
          &((HYGON_S3LIB_BOOT_SCRIPT_IO_RMW *)OpData)->Data,
          &((HYGON_S3LIB_BOOT_SCRIPT_IO_RMW *)OpData)->DataMask
          );
        OpData += sizeof (HYGON_S3LIB_BOOT_SCRIPT_IO_RMW);
        break;
      case HYGON_S3LIB_BOOT_SCRIPT_IO_POLL_OP:
        OpData += sizeof (HYGON_S3LIB_BOOT_SCRIPT_IO_POLL);
        S3BootScriptSaveIoPoll (
          ((HYGON_S3LIB_BOOT_SCRIPT_IO_POLL *)OpData)->Width,
          ((HYGON_S3LIB_BOOT_SCRIPT_IO_POLL *)OpData)->Address,
          &((HYGON_S3LIB_BOOT_SCRIPT_IO_POLL *)OpData)->Data,
          &((HYGON_S3LIB_BOOT_SCRIPT_IO_POLL *)OpData)->DataMask,
          ((HYGON_S3LIB_BOOT_SCRIPT_IO_POLL *)OpData)->Delay
          );
        break;
      case HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE_OP:
        S3BootScriptSaveMemWrite (
          ((HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE *)OpData)->Width,
          ((HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE *)OpData)->Address,
          1,
          &((HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE *)OpData)->Value
          );
        OpData += sizeof (HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE);
        break;
      case HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW_OP:
        S3BootScriptSaveMemReadWrite (
          ((HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW *)OpData)->Width,
          ((HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW *)OpData)->Address,
          &((HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW *)OpData)->Data,
          &((HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW *)OpData)->DataMask
          );
        OpData += sizeof (HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW);
        break;
      case HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL_OP:
        S3BootScriptSaveMemPoll (
          ((HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL *)OpData)->Width,
          ((HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL *)OpData)->Address,
          &((HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL *)OpData)->DataMask,
          &((HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL *)OpData)->Data,
          1,
          ((HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL *)OpData)->LoopTimes
          );
        OpData += sizeof (HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL);
        break;
      case HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE_OP:
        S3BootScriptSavePciCfg2Write (
          ((HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE *)OpData)->Width,
          ((HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE *)OpData)->Segment,
          ((HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE *)OpData)->Address,
          1,
          &((HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE *)OpData)->Value
          );
        OpData += sizeof (HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE);
        break;
      case HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW_OP:
        S3BootScriptSavePciCfg2ReadWrite (
          ((HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW *)OpData)->Width,
          ((HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW *)OpData)->Segment,
          ((HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW *)OpData)->Address,
          &((HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW *)OpData)->Data,
          &((HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW *)OpData)->DataMask
          );
        OpData += sizeof (HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW);
        break;
      case HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL_OP:
        OpData += sizeof (HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL);
        S3BootScriptSavePci2Poll (
          ((HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL *)OpData)->Width,
          ((HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL *)OpData)->Segment,
          ((HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL *)OpData)->Address,
          &((HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL *)OpData)->Data,
          &((HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL *)OpData)->DataMask,
          ((HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL *)OpData)->Delay
          );
        break;
      case HYGON_S3LIB_BOOT_SCRIPT_DISPATCH_OP:
        OpData += sizeof (HYGON_S3LIB_BOOT_SCRIPT_DISPATCH);
        S3BootScriptSaveDispatch2 (
          ((HYGON_S3LIB_BOOT_SCRIPT_DISPATCH *)OpData)->EntryPoint,
          ((HYGON_S3LIB_BOOT_SCRIPT_DISPATCH *)OpData)->Context
          );
        break;
      default:
        ASSERT (FALSE);
        break;
    }

    OpCode = ((HYGON_S3LIB_BOOT_SCRIPT_COMMON *)OpData)->OpCode;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3SaveScriptSmmDispatch Exit\n");
}

EFI_STATUS
HygonS3SaveSmmCommunicateHandler (
  IN       EFI_HANDLE  DispatchHandle,
  IN       CONST VOID  *Context,
  IN OUT   VOID    *CommBuffer,
  IN OUT   UINTN   *CommBufferSize
  )
{
  HYGON_SMM_COMMUNICATION_CMN       *CmnBuffer;
  HYGON_SMM_COMMUNICATION_S3SCRIPT  *S3ScriptBuffer;
  EFI_STATUS                        Status;

  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3SaveSmmCommunicateHandler Enter\n");

  S3ScriptBuffer = NULL;
  CmnBuffer = (HYGON_SMM_COMMUNICATION_CMN *)CommBuffer;
  if (CmnBuffer->id == HYGON_SMM_COMMUNICATION_ID_S3SCRIPT) {
    S3ScriptBuffer = (HYGON_SMM_COMMUNICATION_S3SCRIPT *)CmnBuffer;
    HygonS3SaveScriptSmmDispatch (S3ScriptBuffer->PrivateDataAddress);
  }

  // Unregister the hanlder to make sure execute only once
  Status = gSmst->SmiHandlerUnRegister (mHygonSmmCommunicationHandle);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3SaveSmmCommunicateHandler Unregister Fail\n");
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
HygonSmmCommunicationEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;

  //
  // We're now in SMM!
  //

  // Register SMM Handler for handle the communication from outside of SMM
  mHygonSmmCommunicationHandle = NULL;
  Status = gSmst->SmiHandlerRegister (HygonS3SaveSmmCommunicateHandler, &gHygonSmmCommunicationHandleGuid, &mHygonSmmCommunicationHandle);
  ASSERT (Status == EFI_SUCCESS);

  return EFI_SUCCESS;
}
