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

#include <Library/HygonPspHpobLib.h>

#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_HPCBLIB_HPCBREPLACETYPE_FILECODE

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

/*----------------------------------------------------------------------------------------
 *                          G L O B A L        V A L U E S
 *----------------------------------------------------------------------------------------
 */
extern UINT8                     mHpcbInstance;
extern BOOLEAN  mHpcbAtRuntime;           // byo231107 -

UINT8
HpcbCalcCheckSum (
  IN       VOID             *HpcbPtr,
  IN       UINT32           Length
  );

/*
 * Function to update the HPCB parameters of a given type
*/
EFI_STATUS
HpcbReplaceType (
  IN       UINT16          GroupId,
  IN       HPCB_PARAM_TYPE HpcbParamType,
  IN       UINT16          InstanceId,
  IN       UINT8           *TypeDataStream,
  IN       UINT32          TypeDataSize,
  IN OUT   UINT8           *NewHpcb
  )
{
  EFI_STATUS           Status;
  BOOLEAN              HpcbRecoveryFlag;
  HPCB_HEADER          *HpcbHeader;
  UINT8                *HpcbEnding;
  HPCB_GROUP_HEADER    *HpcbGroupHeader;
  HPCB_TYPE_HEADER     *HpcbTypeHeader;
  UINT32               OldTypeSize;
  UINT32               PreTypeDataSize;
  UINT8                *PostTypeData;
  UINT64               PostTypeDataSize;
  UINT8                *NewHpcbPtr;
  UINT32               *NewSizeOfHpcbPtr;
  UINT32               *NewSizeOfGroupPtr;
  UINT32               *NewSizeOfTypePtr;
  UINT32               NewTypeSize;
  UINT8                *OldHpcb;
  HYGON_CONFIG_PARAMS  StdHeader;

  ASSERT (0 != TypeDataSize);
  IDS_HDT_CONSOLE_PSP_TRACE (
    "HpcbReplaceType G:%x T:%x I:%x Data:&%x Size:%x NewHpcb:%x\n",
    GroupId,
    HpcbParamType,
    InstanceId,
    TypeDataStream,
    TypeDataSize,
    NewHpcb
    );

  // Exit service, if recovery flag set
  if (mHpcbAtRuntime == FALSE) {          // byo231107 -
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

  NewTypeSize = TypeDataSize + ((ALIGN_SIZE_OF_TYPE - (TypeDataSize % ALIGN_SIZE_OF_TYPE)) & (ALIGN_SIZE_OF_TYPE - 1));
  OldHpcb     = GetHpcbShadowCopy ();
  HpcbHeader  = (HPCB_HEADER *)(OldHpcb);
  ASSERT (HPCB_SIGNATURE == HpcbHeader->Signature);
  HpcbEnding = (UINT8 *)(OldHpcb + HpcbHeader->SizeOfHpcb - 1);
  HpcbGroupHeader = (HPCB_GROUP_HEADER *)(VOID *)((UINT8 *)HpcbHeader + sizeof (HPCB_HEADER));
  HpcbTypeHeader  = (HPCB_TYPE_HEADER *)(VOID *)((UINT8 *)HpcbGroupHeader + sizeof (HPCB_GROUP_HEADER));
  IDS_HDT_CONSOLE_PSP_TRACE ("\n\t\tOriginal HPCB Size: 0x%04X\n", HpcbHeader->SizeOfHpcb);
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

          NewHpcbPtr  = NewHpcb;
          OldTypeSize = HpcbTypeHeader->SizeOfType + ((ALIGN_SIZE_OF_TYPE - (HpcbTypeHeader->SizeOfType % ALIGN_SIZE_OF_TYPE)) & (ALIGN_SIZE_OF_TYPE - 1)) - sizeof (HPCB_TYPE_HEADER);
          //
          // Copy the HPCB data prior to the to-be-replaced type to the new HPCB Buffer as-is
          //
          PreTypeDataSize = (UINT32)((UINT8 *)HpcbTypeHeader - (UINT8 *)OldHpcb + sizeof (HPCB_TYPE_HEADER));
          LibHygonMemCopy (NewHpcbPtr, OldHpcb, PreTypeDataSize, &StdHeader);
          NewHpcbPtr += PreTypeDataSize;

          //
          // Copy the to-be-replaced type to the new HPCB Buffer as-is
          //
          LibHygonMemCopy (NewHpcbPtr, TypeDataStream, NewTypeSize, &StdHeader);
          NewHpcbPtr += NewTypeSize;

          //
          // Adjust size in the headers
          //
          NewSizeOfHpcbPtr  = &(((HPCB_HEADER *)NewHpcb)->SizeOfHpcb);
          NewSizeOfGroupPtr = (UINT32 *)((UINTN)&HpcbGroupHeader->SizeOfGroup + (UINTN)NewHpcb - (UINTN)OldHpcb);
          NewSizeOfTypePtr  = (UINT32 *)((UINTN)&HpcbTypeHeader->SizeOfType + (UINTN)NewHpcb - (UINTN)OldHpcb);
          if (NewTypeSize >= OldTypeSize) {
            *NewSizeOfHpcbPtr  += (NewTypeSize - OldTypeSize);
            *NewSizeOfGroupPtr += (NewTypeSize - OldTypeSize);
            *NewSizeOfTypePtr  += (NewTypeSize - OldTypeSize);
          } else {
            *NewSizeOfHpcbPtr  -= (OldTypeSize - NewTypeSize);
            *NewSizeOfGroupPtr -= (OldTypeSize - NewTypeSize);
            *NewSizeOfTypePtr  -= (OldTypeSize - NewTypeSize);
          }

          IDS_HDT_CONSOLE_PSP_TRACE ("\n\t\tSize of HPCB updated to: 0x%04X\n", *NewSizeOfHpcbPtr);
          IDS_HDT_CONSOLE_PSP_TRACE ("\n\t\tSize of Group updated to: 0x%04X\n", *NewSizeOfGroupPtr);
          IDS_HDT_CONSOLE_PSP_TRACE ("\n\t\tSize of Type updated to: 0x%04X\n", *NewSizeOfTypePtr);

          //
          // Copy the HPCB data after the to-be-replaced type to the new HPCB Buffer as-is
          //
          PostTypeData = (UINT8 *)HpcbTypeHeader + sizeof (HPCB_TYPE_HEADER) + OldTypeSize;
          // Check If reach the last type
          if (PostTypeData < HpcbEnding) {
            PostTypeDataSize = HpcbEnding - PostTypeData + 1;
            LibHygonMemCopy (NewHpcbPtr, PostTypeData, PostTypeDataSize, &StdHeader);
          }

          // Calc the checksum
          IDS_HDT_CONSOLE_PSP_TRACE ("Calc the checksum\n");
          ((HPCB_HEADER *)NewHpcb)->CheckSumByte = 0;
          ((HPCB_HEADER *)NewHpcb)->CheckSumByte = (UINT8)(0x100 - HpcbCalcCheckSum (NewHpcb, ((HPCB_HEADER *)NewHpcb)->SizeOfHpcb));
          // Point the shadow copy to new version, and free old version
          FreePool (OldHpcb);
          IDS_HDT_CONSOLE_PSP_TRACE ("FREE[%x]\n", OldHpcb);

          // Update Shadow Copy public data
          IDS_HDT_CONSOLE_PSP_TRACE ("Update PSP HpcbShadowCopy from %x to %x\n", OldHpcb, NewHpcb);
          SetHpcbShadowCopy (NewHpcb);

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

  return EFI_NOT_FOUND;
}
