/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Library
 *
 * Contains interface to the HYGON HGPI library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Lib
 *
 */
/*
 ******************************************************************************
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

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * HYGON processors and its use is subject to the terms and conditions of the
 * HYGON Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an HYGON processor outside of
 * the applicable HYGON product specifications will void any HYGON warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and HYGON disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of HYGON processors.
 *
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/HpcbSatLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/HygonPspBaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HygonPspFlashUpdateLib.h>
#include <Library/HygonPspHpobLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmReadyToBoot.h>
#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_HPCBDXELIB_HPCBLIB_FILECODE

#define HPCB_SIGNATURE  0x42435041ul

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern EFI_GUID gSmmHpcbRtShadowAddressProtocolGuid;

/*----------------------------------------------------------------------------------------
 *                          G L O B A L        V A L U E S
 *----------------------------------------------------------------------------------------
 */
HPCB_HEADER            *mHpcbSmmRtShadowCopyProtocol = NULL;
HPCB_HEADER            **mHpcbSmmRtShadowCopyPtr     = NULL;
BOOLEAN                mInSmm     = FALSE;
BOOLEAN                mHpcbAtRuntime = FALSE;      // byo231107 -
UINT64                 mHpcbEntryAddress = 0;
UINT32                 mHpcbEntrySize    = 0;
EFI_SMM_SYSTEM_TABLE2  *mSmst = NULL;
UINT8                  mHpcbInstance = 0;

/*
 * Function to get HpcbShadowCopy Address, return NULL if fail
*/
VOID *
GetHpcbShadowCopy (
  )
{
  VOID  *HpcbData;

  HpcbData = NULL;
  if (mHpcbAtRuntime) {                             // byo231107 -
    HpcbData = *mHpcbSmmRtShadowCopyPtr;
  } else {
    HpcbData = (VOID *)(UINTN)PcdGet64 (PcdHpcbShadowAddress);
  }

  ASSERT (HpcbData != NULL);
  ASSERT ((*(UINT32 *)HpcbData) == HPCB_SIGNATURE);
  IDS_HDT_CONSOLE_PSP_TRACE ("GetHpcbShadowCopy %x\n", HpcbData);

  return HpcbData;
}

/*
 * Function to set HpcbShadowCopy Address
*/
VOID
SetHpcbShadowCopy (
  VOID *HpcbShadowAddress
  )
{
  IDS_HDT_CONSOLE_PSP_TRACE ("SetHpcbShadowCopy %x\n", HpcbShadowAddress);
  if (mHpcbAtRuntime) {                             // byo231107 -
    *mHpcbSmmRtShadowCopyPtr = HpcbShadowAddress;
  } else {
    PcdSet64S (PcdHpcbShadowAddress, (UINT64)(UINTN)HpcbShadowAddress);
  }
}

/*
 * Function to calculate HPCB checksum
*/
UINT8
HpcbCalcCheckSum (
  IN       VOID             *HpcbPtr,
  IN       UINT32           Length
  )
{
  UINT8  CheckSum;
  UINT8  *DataPtr;

  CheckSum = 0;
  DataPtr  = HpcbPtr;
  while (Length--) {
    CheckSum += *(DataPtr++);
  }

  return CheckSum;
}

/*
 * Function to get the HPCB data of a given type
*/
EFI_STATUS
HpcbGetType (
  IN     UINT16          GroupId,
  IN     HPCB_PARAM_TYPE HpcbParamType,
  IN     UINT16          InstanceId,
  IN     UINT8           *TypeDataStream,
  IN OUT UINT32          *TypeDataSize
  )
{
  EFI_STATUS           Status;
  BOOLEAN              HpcbRecoveryFlag;
  HPCB_HEADER          *HpcbHeader;
  UINT8                *HpcbEnding;
  HPCB_GROUP_HEADER    *HpcbGroupHeader;
  HPCB_TYPE_HEADER     *HpcbTypeHeader;
  UINT8                *HpcbData;
  UINT32               HpcbDataSize;
  HYGON_CONFIG_PARAMS  StdHeader;

  IDS_HDT_CONSOLE_PSP_TRACE (
    "HpcbGetType G:%x T:%x I:%x Data:&%x Size:%x\n",
    GroupId,
    HpcbParamType,
    InstanceId,
    TypeDataStream,
    TypeDataSize
    );
  // Exit service, if recovery flag set
  if (mHpcbAtRuntime == FALSE) {                    // byo231107 -
    Status = HygonPspGetHpobHpcbRecoveryFlag (&HpcbRecoveryFlag);
    if (HpcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("HPCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  // Exit service, if recovery flag set
  if (CheckPspRecoveryFlag () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  HpcbData   = GetHpcbShadowCopy ();
  HpcbHeader = (HPCB_HEADER *)(HpcbData);
  ASSERT (HPCB_SIGNATURE == HpcbHeader->Signature);
  HpcbEnding = (UINT8 *)(HpcbData + HpcbHeader->SizeOfHpcb - 1);
  HpcbGroupHeader = (HPCB_GROUP_HEADER *)(VOID *)((UINT8 *)HpcbHeader + sizeof (HPCB_HEADER));
  HpcbTypeHeader  = (HPCB_TYPE_HEADER *)(VOID *)((UINT8 *)HpcbGroupHeader + sizeof (HPCB_GROUP_HEADER));
  IDS_HDT_CONSOLE_PSP_TRACE ("\n\t\tHPCB Size: 0x%04X\n", HpcbHeader->SizeOfHpcb);
  while ((((UINT8 *)HpcbGroupHeader + sizeof (HPCB_GROUP_HEADER) - 1) <= HpcbEnding) &&
         ((UINT8 *)((UINTN)(VOID *)HpcbTypeHeader + HpcbTypeHeader->SizeOfType - 1) <= HpcbEnding)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\n\t\tCurrent Group: 0x%04X, Current Type: 0x%04X\n", HpcbGroupHeader->GroupId, HpcbTypeHeader->TypeId);
    if (HpcbGroupHeader->GroupId == GroupId) {
      if (HpcbTypeHeader->TypeId == HpcbParamType) {
        if (HpcbTypeHeader->InstanceId == InstanceId) {
          //
          // Found match!
          //
          IDS_HDT_CONSOLE_PSP_TRACE ("\n\t\tFound TypeId: %x,  Total TypeSize: %x\n", HpcbTypeHeader->TypeId, HpcbTypeHeader->SizeOfType);
          // Check the input buffer size, it too small return the Actual type size
          HpcbDataSize = HpcbTypeHeader->SizeOfType - sizeof (HPCB_TYPE_HEADER);
          if (*TypeDataSize < HpcbDataSize) {
            IDS_HDT_CONSOLE_PSP_TRACE ("\n\t\tSize too small\n");
            *TypeDataSize = HpcbDataSize;
            return EFI_BUFFER_TOO_SMALL;
          }

          *TypeDataSize = HpcbDataSize;
          LibHygonMemCopy (TypeDataStream, HpcbTypeHeader + 1, HpcbDataSize, &StdHeader);
          return EFI_SUCCESS;
        }
      } else {
        HpcbTypeHeader = (HPCB_TYPE_HEADER *)((UINTN)(CHAR8 *)HpcbTypeHeader + HpcbTypeHeader->SizeOfType);
        // Adjust alignment
        HpcbTypeHeader = (HPCB_TYPE_HEADER *)((UINT8 *)HpcbTypeHeader + ((ALIGN_SIZE_OF_TYPE - ((UINTN)(UINT8 *)HpcbTypeHeader) % ALIGN_SIZE_OF_TYPE) & (ALIGN_SIZE_OF_TYPE - 1)));
        if ((UINT8 *)HpcbTypeHeader > (UINT8 *)((UINTN)(CHAR8 *)HpcbGroupHeader + HpcbGroupHeader->SizeOfGroup - 1)) {
          return EFI_NOT_FOUND;
        }
      }
    } else {
      HpcbGroupHeader = (HPCB_GROUP_HEADER *)((UINTN)(VOID *)HpcbGroupHeader + HpcbGroupHeader->SizeOfGroup);
      //
      // HPCB Type header following APCP Group header should already be 4-bytes aligned, there's no need for
      // extra adjustment on HpcbTypeHeader pointer.
      //
      HpcbTypeHeader = (HPCB_TYPE_HEADER *)(VOID *)(HpcbGroupHeader + 1);
    }
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("\n\t\tNo matching type found in HPCB.\n");

  return EFI_UNSUPPORTED;
}

/**
 *  Write HPCB data to BIOS Directory HPCB Entry 0x60
 *
 *
 *  @retval EFI_SUCCESS       The Data save to FLASH successfully
 *  @retval Other             Some error occurs when executing this function.
 *
 **/
EFI_STATUS
HygonPspWriteBackHpcbShadowCopy (
  VOID
  )
{
  HPCB_HEADER  *HpcbHeader;
  UINT8        *HpcbData;
  EFI_STATUS   Status;
  BOOLEAN      HpcbRecoveryFlag;
  UINT8        *TempBuf;
  UINT32       SizeOfOldHpcb;

  Status = EFI_SUCCESS;
  IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspWriteBackHpcbShadowCopy Enter\n");

  // Exit service, if recovery flag set
  if (mHpcbAtRuntime == FALSE) {                    // byo231107 -
    Status = HygonPspGetHpobHpcbRecoveryFlag (&HpcbRecoveryFlag);
    if (HpcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("HPCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  // Exit service, if recovery flag set
  if (CheckPspRecoveryFlag () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  // Check the size of HPCB DRAM copy
  HpcbData   = GetHpcbShadowCopy ();
  HpcbHeader = (HPCB_HEADER *)HpcbData;
  if (HpcbHeader->SizeOfHpcb > mHpcbEntrySize) {
    IDS_HDT_CONSOLE_PSP_TRACE ("HPCB SPI Entry too small\n");
    return EFI_UNSUPPORTED;
  }

  // Compare if any changes
  if (CompareMem (HpcbData, (VOID *)(UINTN)mHpcbEntryAddress, HpcbHeader->SizeOfHpcb)) {
    // Update UniqueHpcbInstance when HPCB content has been updated
    HpcbHeader->UniqueHpcbInstance += 1;
    HpcbHeader->CheckSumByte = 0;
    HpcbHeader->CheckSumByte = (UINT8)(0x100 - HpcbCalcCheckSum (HpcbData, HpcbHeader->SizeOfHpcb));
    IDS_HDT_CONSOLE_PSP_TRACE ("UniqueHpcbInstance %x\n", HpcbHeader->UniqueHpcbInstance);
    IDS_HDT_CONSOLE_PSP_TRACE ("Data difference between HPCB buffer & SPI copy\n");

    SizeOfOldHpcb = ((HPCB_HEADER *)mHpcbEntryAddress)->SizeOfHpcb;
    if (SizeOfOldHpcb > HpcbHeader->SizeOfHpcb) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Old HPCB data length 0x%X is more than new 0x%X\n", SizeOfOldHpcb, HpcbHeader->SizeOfHpcb);
      TempBuf = AllocatePool (SizeOfOldHpcb);
      SetMem(TempBuf, SizeOfOldHpcb, 0xFF);
      CopyMem (TempBuf, HpcbData, HpcbHeader->SizeOfHpcb);

      Status = PspUpdateFlash ((UINT32) mHpcbEntryAddress, SizeOfOldHpcb, TempBuf);
      //Free temporary buffer
      FreePool (TempBuf);  
    } else {
      Status = PspUpdateFlash ((UINT32)mHpcbEntryAddress, HpcbHeader->SizeOfHpcb, HpcbData);
    }

  } else {                                                       // byo231102 +
    IDS_HDT_CONSOLE_PSP_TRACE ("Hpcb NoChanges\n");              // byo231102 +
    Status = EFI_WARN_WRITE_FAILURE;                             // byo231102 +
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspWriteBackHpcbShadowCopy Exit\n");

  return Status;
}

/**
 *
 *  Init mHpcbSmmRtShadowCopy
 *
 **/
EFI_STATUS
EFIAPI
HpcbRTBCallBack (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  IDS_HDT_CONSOLE_PSP_TRACE ("HpcbRTBCallBack\n");
  // Check if mHpcbSmmRtShadowCopy already initialized
  // Use HPCB signature to do the simple check below
  if (((HPCB_HEADER *)*mHpcbSmmRtShadowCopyPtr)->Signature != HPCB_SIGNATURE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Copy the Hpcb from SPI %x to HPCB RT copy %x\n", mHpcbEntryAddress, *mHpcbSmmRtShadowCopyPtr);
    // Copy the Hpcb from SPI to Shadow Copy
    CopyMem (*mHpcbSmmRtShadowCopyPtr, (VOID *)(UINTN)mHpcbEntryAddress, mHpcbEntrySize);
  }

  mHpcbAtRuntime = TRUE;                            // byo231107 -
  return EFI_SUCCESS;
}

/**
 *
 *  HpcbDxeLibConstructor, initial the shadow copy of HPCB data, and save the address to PCD
 *
 **/
EFI_STATUS
EFIAPI
HpcbDxeLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  HPCB_HEADER             *HpcbShadowCopy;
  BOOLEAN                 HpcbEntryExist;
  TYPE_ATTRIB             TypeAttrib;
  UINT64                  EntryDest;
  EFI_SMM_BASE2_PROTOCOL  *SmmBase2;
  EFI_STATUS              Status;
  VOID                    *Registration;
  EFI_HANDLE              Handle;
  BOOLEAN                 HpcbRecoveryFlag;

  // Locate the global data structure via PCD
  HpcbShadowCopy = NULL;
  mInSmm = FALSE;
  IDS_HDT_CONSOLE_PSP_TRACE ("HpcbDxeLibConstructor\n");

  HpcbRecoveryFlag = FALSE;
  Status = HygonPspGetHpobHpcbRecoveryFlag (&HpcbRecoveryFlag);
  if (HpcbRecoveryFlag) {
    IDS_HDT_CONSOLE_PSP_TRACE ("HPCB.RecoveryFlag Set, exit service\n");
    return EFI_SUCCESS;
  }

  // Exit service, if recovery flag set
  if (CheckPspRecoveryFlag () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_SUCCESS;
  }

  // Check the existence and size of HPCB  entry 0x60
  Status = HygonPspGetHpobHpcbInstance (&mHpcbInstance);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("HPCB Instance get fail\n");
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Active HPCB Instance %x\n", mHpcbInstance);
  HpcbEntryExist = BIOSEntryInfo (BIOS_HPCB_INFO, mHpcbInstance, &TypeAttrib, &mHpcbEntryAddress, &mHpcbEntrySize, &EntryDest);
  if (!HpcbEntryExist) {
    IDS_HDT_CONSOLE_PSP_TRACE ("HPCB Entry not exist\n");
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID **)&SmmBase2);
  if (!EFI_ERROR (Status)) {
    SmmBase2->InSmm (SmmBase2, &mInSmm);
  } else {
    IDS_HDT_CONSOLE_PSP_TRACE ("gEfiSmmBase2ProtocolGuid Locate Fail\n");
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("%a\n", mInSmm ? "InSmm" : "OutSmm");
  // Check if Posttime HPCB shadow already initialized
  HpcbShadowCopy = (HPCB_HEADER *)(UINTN)PcdGet64 (PcdHpcbShadowAddress);
  if (HpcbShadowCopy == NULL) {
    // Shadow Copy haven't init yet
    IDS_HDT_CONSOLE_PSP_TRACE ("Allocate Posttime HPCB Shadow Copy with 0x%x bytes\n", mHpcbEntrySize);
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    mHpcbEntrySize,
                    (VOID **)&HpcbShadowCopy
                    );
    ASSERT (HpcbShadowCopy != NULL);
    if (HpcbShadowCopy == NULL) {
      IDS_HDT_CONSOLE_PSP_TRACE ("HPCB Shadow Copy allocate fail\n");
      return EFI_SUCCESS;
    }

    IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", HpcbShadowCopy, mHpcbEntrySize);

    ZeroMem (HpcbShadowCopy, mHpcbEntrySize);
    // Copy the Hpcb from SPI to Shadow Copy
    CopyMem (HpcbShadowCopy, (VOID *)(UINTN)mHpcbEntryAddress, mHpcbEntrySize);
    // Calc the checksum, and report the warning
    if (HpcbCalcCheckSum (HpcbShadowCopy, HpcbShadowCopy->SizeOfHpcb) != 0) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Warning: HPCB Checksum not zero\n");
    }

    PcdSet64S (PcdHpcbShadowAddress, (UINT64)(UINTN)HpcbShadowCopy);
  }

  if (!mInSmm) {
    return EFI_SUCCESS;
  }

  // We are now in SMM
  // get SMM table base
  Status = SmmBase2->GetSmstLocation (SmmBase2, &mSmst);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("GetSmstLocation Fail\n");
    return EFI_SUCCESS;
  }

  // Check if Posttime HPCB shadow already initialized by locate SmmProtocol
  Status = mSmst->SmmLocateProtocol (&gSmmHpcbRtShadowAddressProtocolGuid, NULL, (VOID **)&mHpcbSmmRtShadowCopyPtr);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Allocate RT HPCB Shadow Copy with 0x%x bytes\n", mHpcbEntrySize);
    // Allocate HPCB shadow for SMM RT
    Status = mSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      mHpcbEntrySize,
                      (VOID **)&HpcbShadowCopy
                      );
    ASSERT (HpcbShadowCopy != NULL);
    if (HpcbShadowCopy == NULL) {
      IDS_HDT_CONSOLE_PSP_TRACE ("HPCB Shadow Copy allocate fail\n");
      return EFI_SUCCESS;
    }

    IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", HpcbShadowCopy, mHpcbEntrySize);

    ZeroMem (HpcbShadowCopy, mHpcbEntrySize);
    mHpcbSmmRtShadowCopyProtocol = HpcbShadowCopy;
    mHpcbSmmRtShadowCopyPtr = &mHpcbSmmRtShadowCopyProtocol;
    Handle = NULL;
    Status = mSmst->SmmInstallProtocolInterface (
                      &Handle,
                      &gSmmHpcbRtShadowAddressProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &mHpcbSmmRtShadowCopyProtocol
                      );
  }

  // Register SMM Exit boot service callback used init mHpcbSmmRtShadowCopy
  mSmst->SmmRegisterProtocolNotify (
           &gEdkiiSmmReadyToBootProtocolGuid,
           HpcbRTBCallBack,
           &Registration
           );

  return EFI_SUCCESS;
}
