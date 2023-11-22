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
#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/HpcbSatLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/HygonPspBaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HygonPspHpobLib.h>
#include <Library/IdsLib.h>
#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_HPCBPEILIB_HPCBLIB_FILECODE

#define HPCB_SIGNATURE  0x42435041ul

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

EFI_STATUS
EFIAPI
HpcbEndOfPeiSignalPpiCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );

EFI_PEI_NOTIFY_DESCRIPTOR  mHpcbEndOfPeiSignalPpiCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  HpcbEndOfPeiSignalPpiCallback
};
/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*
 * Function to get HpcbShadowCopy Address, return NULL if fail
*/
VOID *
GetHpcbShadowCopy (
  )
{
  VOID  *HpcbData;

  HpcbData = NULL;

  HpcbData = (VOID *)(UINTN)PcdGet64 (PcdHpcbShadowAddress);

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
  PcdSet64S (PcdHpcbShadowAddress, (UINT64)(UINTN)HpcbShadowAddress);
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
  return EFI_UNSUPPORTED;
}

/**
 *
 *  HpcbPeiLibConstructor, initial the shadow copy of HPCB data, and save the address to PCD
 *
 **/
EFI_STATUS
EFIAPI
HpcbPeiLibConstructor (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  HPCB_HEADER             *HpcbShadowCopy;
  BOOLEAN                 HpcbEntryExist;
  TYPE_ATTRIB             TypeAttrib;
  UINT64                  EntryDest;
  UINT64                  HpcbEntryAddress;
  UINT32                  HpcbEntrySize;
  UINT8                   HpcbInstance;
  EFI_STATUS              Status;

  IDS_HDT_CONSOLE_PSP_TRACE ("HpcbPeiLibConstructor\n");
  // Check if Posttime HPCB shadow already initialized
  HpcbShadowCopy = (HPCB_HEADER *)(UINTN)PcdGet64 (PcdHpcbShadowAddress);
  if (HpcbShadowCopy == NULL) {
    // Check the existence and size of HPCB  entry 0x60
    Status = HygonPspGetHpobHpcbInstance (&HpcbInstance);
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("HPCB Instance get fail\n");
    }

    IDS_HDT_CONSOLE_PSP_TRACE ("Active HPCB Instance %x\n", HpcbInstance);
    HpcbEntryExist = BIOSEntryInfo (BIOS_HPCB_INFO, HpcbInstance, &TypeAttrib, &HpcbEntryAddress, &HpcbEntrySize, &EntryDest);
    if (!HpcbEntryExist) {
      IDS_HDT_CONSOLE_PSP_TRACE ("HPCB Entry not exist\n");
      return EFI_SUCCESS;
    }

    // Shadow Copy haven't init yet
    IDS_HDT_CONSOLE_PSP_TRACE ("Allocate Posttime HPCB Shadow Copy with 0x%x bytes\n", HpcbEntrySize);
    HpcbShadowCopy = AllocateZeroPool (HpcbEntrySize);
    ASSERT (HpcbShadowCopy != NULL);
    if (HpcbShadowCopy == NULL) {
      IDS_HDT_CONSOLE_PSP_TRACE ("HPCB Shadow Copy allocate fail\n");
      return EFI_SUCCESS;
    }

    IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", HpcbShadowCopy, HpcbEntrySize);

    // Copy the Hpcb from SPI to Shadow Copy
    CopyMem (HpcbShadowCopy, (VOID *)(UINTN)HpcbEntryAddress, HpcbEntrySize);

    PcdSet64S (PcdHpcbShadowAddress, (UINT64)(UINTN)HpcbShadowCopy);

    Status = PeiServicesNotifyPpi (&mHpcbEndOfPeiSignalPpiCallback);
  }
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
HpcbEndOfPeiSignalPpiCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  //
  // clear PcdHpcbShadowAddress at end of PEI phase
  //
  SetHpcbShadowCopy(NULL);
  return (EFI_SUCCESS);
}