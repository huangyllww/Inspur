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
#define FILECODE  LIBRARY_HPCBDXELIB_GETSETPARAMS_FILECODE

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
extern UINT8    mHpcbInstance;
extern BOOLEAN  mHpcbAtRuntime;          // byo231107 -

/*
 * Internal Function to retrieve HPCB Data for given Type under a given Group
*/
EFI_STATUS
InternalHpcbLocateType (
  IN       UINT16   GroupId,
  IN       UINT16   TypeId,
  IN       UINT16   InstanceId,
  IN       UINT32   *Size,
  IN OUT   VOID     **TypeData
  )
{
  HPCB_HEADER        *HpcbHeader;
  UINT8              *HpcbEnding;
  HPCB_GROUP_HEADER  *HpcbGroupHeader;
  HPCB_TYPE_HEADER   *HpcbTypeHeader;

  IDS_HDT_CONSOLE_PSP_TRACE ("HpcbLocateType GroupId : 0x%04X,  TypeId : 0x%04X, InstanceId:0x%x\n", GroupId, TypeId, InstanceId);
  HpcbHeader = GetHpcbShadowCopy ();
  HpcbEnding = (UINT8 *)((UINT8 *)HpcbHeader + HpcbHeader->SizeOfHpcb - 1);
  HpcbGroupHeader = (HPCB_GROUP_HEADER *)(VOID *)((UINT8 *)HpcbHeader + sizeof (HPCB_HEADER));
  HpcbTypeHeader  = (HPCB_TYPE_HEADER *)(VOID *)((UINT8 *)HpcbGroupHeader + sizeof (HPCB_GROUP_HEADER));

  //
  // Travers all HPCB Group and Type
  //
  while (  (((UINT8 *)HpcbGroupHeader + sizeof (HPCB_GROUP_HEADER) - 1) <= HpcbEnding)
        && (((UINT8 *)HpcbTypeHeader + sizeof (HPCB_TYPE_HEADER) - 1) <= HpcbEnding)
        && ((UINT8 *)((UINT8 *)HpcbTypeHeader + HpcbTypeHeader->SizeOfType - 1) <= HpcbEnding)) {
    if (HpcbGroupHeader->GroupId == GroupId) {
      if ((HpcbTypeHeader->TypeId == TypeId)  && (HpcbTypeHeader->InstanceId == InstanceId)) {
        *TypeData = (VOID *)((UINTN)(VOID *)HpcbTypeHeader + sizeof (HPCB_TYPE_HEADER));
        *Size     = (HpcbTypeHeader->SizeOfType - sizeof (HPCB_TYPE_HEADER));
        IDS_HDT_CONSOLE_PSP_TRACE ("\t\tFound matched type id @0x%x!\n", HpcbTypeHeader);
        return EFI_SUCCESS;
      } else {
        HpcbTypeHeader = (HPCB_TYPE_HEADER *)((UINTN)(CHAR8 *)HpcbTypeHeader + HpcbTypeHeader->SizeOfType);
        // Adjust alignment
        HpcbTypeHeader = (HPCB_TYPE_HEADER *)((UINT8 *)HpcbTypeHeader + ((ALIGN_SIZE_OF_TYPE - ((UINTN)(UINT8 *)HpcbTypeHeader) % ALIGN_SIZE_OF_TYPE) & (ALIGN_SIZE_OF_TYPE - 1)));
        if ((UINT8 *)HpcbTypeHeader > (UINT8 *)((UINTN)(CHAR8 *)HpcbGroupHeader + HpcbGroupHeader->SizeOfGroup)) {
          return EFI_NOT_FOUND;
        }
      }
    } else {
      HpcbGroupHeader = (HPCB_GROUP_HEADER *)((UINTN)(VOID *)HpcbGroupHeader + HpcbGroupHeader->SizeOfGroup);
      // HPCB Type header following APCP Group header should already be 4-bytes aligned, there's no need for extra adjustment on HpcbTypeHeader pointer.
      HpcbTypeHeader = (HPCB_TYPE_HEADER *)(VOID *)((CHAR8 *)HpcbGroupHeader + sizeof (HPCB_GROUP_HEADER));
    }
  }

  return EFI_NOT_FOUND;
}

/*
 * Internal Function to retrieve an HPCB Parameter detail info with a given type and token
 * If given token ID is not found, ParamAttribOffset &  ParamValueOffset will return the next offset of last valid elements
*/
EFI_STATUS
InternalHpcbGetParameterDetails (
  IN       HPCB_PARAM_TYPE    HpcbParamType,
  IN       UINT16             HpcbParamToken,
  IN OUT   UINT16             *GroupIdPtr,
  IN OUT   HPCB_PARAM_ATTRIBUTE      **HpcbParamAttributeStreamPtr,
  IN OUT   UINT8                     **HpcbParamValueStreamPtr,
  IN OUT   UINT32                    *ParamAttribOffsetPtr,
  IN OUT   UINT32                    *ParamValueOffsetPtr
  )
{
  EFI_STATUS            Status;
  UINT16                TokenBegin;
  UINT16                TokenEnd;
  UINT16                TokenLimit;
  UINT32                i;
  UINT32                ParamOffset;
  UINT8                 *HpcbParamValueStream;
  UINT32                HpcbParamBlockSize;
  HPCB_PARAM_ATTRIBUTE  *HpcbParamAttributeStream;

  //
  // Ensure the input type is legal
  //
  ASSERT (HpcbParamType >= HPCB_PSP_TYPE_CONFIG_DEFAULT_PARAMETERS && HpcbParamType <= HPCB_TYPE_CBS_DEBUG_DUMMY_PARAMETERS);
  if ((HpcbParamType < HPCB_PSP_TYPE_CONFIG_DEFAULT_PARAMETERS) || (HpcbParamType > HPCB_TYPE_CBS_DEBUG_DUMMY_PARAMETERS)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Invalid HpcbParamType\n");
    return EFI_INVALID_PARAMETER;
  }

  ASSERT (
    (GroupIdPtr != NULL) &&
    (HpcbParamAttributeStreamPtr != NULL) &&
    (HpcbParamValueStreamPtr != NULL) &&
    (ParamAttribOffsetPtr != NULL) &&
    (ParamValueOffsetPtr != NULL)
    );

  if ((GroupIdPtr == NULL) ||
      (HpcbParamAttributeStreamPtr == NULL) ||
      (HpcbParamValueStreamPtr == NULL) ||
      (ParamAttribOffsetPtr == NULL) ||
      (ParamValueOffsetPtr == NULL)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Input parameters Pointer set to NULL\n");
    return EFI_INVALID_PARAMETER;
  }

  switch (HpcbParamType) {
    case HPCB_PSP_TYPE_CONFIG_DEFAULT_PARAMETERS:
    case HPCB_PSP_TYPE_CONFIG_PARAMETERS:
      *GroupIdPtr = HPCB_GROUP_PSP;
      TokenBegin  = HPCB_TOKEN_CONFIG_PSP_BEGIN + 1;
      TokenEnd    = HPCB_TOKEN_CONFIG_PSP_END;
      TokenLimit  = HPCB_TOKEN_CONFIG_CCX_BEGIN - 1;
      break;

    case HPCB_CCX_TYPE_CONFIG_DEFAULT_PARAMETERS:
    case HPCB_CCX_TYPE_CONFIG_PARAMETERS:
      *GroupIdPtr = HPCB_GROUP_CCX;
      TokenBegin  = HPCB_TOKEN_CONFIG_CCX_BEGIN + 1;
      TokenEnd    = HPCB_TOKEN_CONFIG_CCX_END;
      TokenLimit  = HPCB_TOKEN_CONFIG_DF_BEGIN - 1;
      break;

    case HPCB_DF_TYPE_CONFIG_DEFAULT_PARAMETERS:
    case HPCB_DF_TYPE_CONFIG_PARAMETERS:
      *GroupIdPtr = HPCB_GROUP_DF;
      TokenBegin  = HPCB_TOKEN_CONFIG_DF_BEGIN + 1;
      TokenEnd    = HPCB_TOKEN_CONFIG_DF_END;
      TokenLimit  = HPCB_TOKEN_CONFIG_MEM_BEGIN - 1;
      break;

    case HPCB_MEM_TYPE_CONFIG_DEFAULT_PARAMETERS:
    case HPCB_MEM_TYPE_CONFIG_PARAMETERS:
      *GroupIdPtr = HPCB_GROUP_MEMORY;
      TokenBegin  = HPCB_TOKEN_CONFIG_MEM_BEGIN + 1;
      TokenEnd    = HPCB_TOKEN_CONFIG_MEM_END;
      TokenLimit  = HPCB_TOKEN_CONFIG_GNB_BEGIN - 1;
      break;

    case HPCB_GNB_TYPE_CONFIG_DEFAULT_PARAMETERS:
    case HPCB_GNB_TYPE_CONFIG_PARAMETERS:
      *GroupIdPtr = HPCB_GROUP_GNB;
      TokenBegin  = HPCB_TOKEN_CONFIG_GNB_BEGIN + 1;
      TokenEnd    = HPCB_TOKEN_CONFIG_GNB_END;
      TokenLimit  = HPCB_TOKEN_CONFIG_FCH_BEGIN - 1;
      break;

    case HPCB_FCH_TYPE_CONFIG_DEFAULT_PARAMETERS:
    case HPCB_FCH_TYPE_CONFIG_PARAMETERS:
      *GroupIdPtr = HPCB_GROUP_FCH;
      TokenBegin  = HPCB_TOKEN_CONFIG_FCH_BEGIN + 1;
      TokenEnd    = HPCB_TOKEN_CONFIG_FCH_END;
      TokenLimit  = HPCB_TOKEN_CONFIG_LIMIT - 1;
      break;

    case HPCB_TYPE_CBS_COMMON_PARAMETERS:
      *GroupIdPtr = HPCB_GROUP_CBS;
      TokenBegin  = HPCB_TOKEN_CBS_CMN_BEGIN + 1;
      TokenEnd    = HPCB_TOKEN_CBS_CMN_END;
      TokenLimit  = HPCB_TOKEN_CBS_CMN_LIMIT - 1;
      break;

    case HPCB_TYPE_CBS_DEBUG_PARAMETERS:
      *GroupIdPtr = HPCB_GROUP_CBS;
      TokenBegin  = HPCB_TOKEN_CBS_DBG_BEGIN + 1;
      TokenEnd    = HPCB_TOKEN_CBS_DBG_END;
      TokenLimit  = HPCB_TOKEN_CBS_DBG_LIMIT - 1;
      break;

    default:
      IDS_HDT_CONSOLE_PSP_TRACE ("Invalid HpcbParamType\n");
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
  }

  Status = InternalHpcbLocateType (*GroupIdPtr, HpcbParamType, 0, &HpcbParamBlockSize, (VOID *)&HpcbParamAttributeStream);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Can't Locate HpcbParamType\n");
    return Status;
  }

  *HpcbParamAttributeStreamPtr = HpcbParamAttributeStream;

  // Get Value Stream offset
  i = 0;
  while (HpcbParamAttributeStream[i].Token != HPCB_TOKEN_CONFIG_LIMIT) {
    //
    // The starting token of each component should remain constant across different versions of ABL
    //
    if ((HpcbParamAttributeStream[i].Token < TokenBegin || HpcbParamAttributeStream[i].Token >= TokenLimit)) {
      ASSERT (FALSE);
    }

    i++;
  }

  HpcbParamValueStream     = (UINT8 *)&HpcbParamAttributeStream[i + 1];
  *HpcbParamValueStreamPtr = HpcbParamValueStream;

  //
  // Search for Input Token
  //
  i = 0;
  ParamOffset = 0;
  while (HpcbParamAttributeStream[i].Token != HPCB_TOKEN_CONFIG_LIMIT) {
    if (HpcbParamAttributeStream[i].Token < TokenEnd) {
      //
      // If the token exceeds the end set by the current ABL, it maybe a newly added token
      // and can be safely ignored in the current version
      //
      if (HpcbParamAttributeStream[i].Token == HpcbParamToken) {
        IDS_HDT_CONSOLE_PSP_TRACE ("Token found @ AttribOffset=%d ValueOffset=0x%x \n", i, ParamOffset);
        *ParamAttribOffsetPtr = i;
        *ParamValueOffsetPtr  = ParamOffset;
        return EFI_SUCCESS;
      }

      ParamOffset += (HpcbParamAttributeStream[i].Size + 1);
    }

    i++;
  }

  // Not found return the next offset of last valid elements
  *ParamAttribOffsetPtr = i;
  *ParamValueOffsetPtr  = ParamOffset;
  return EFI_NOT_FOUND;
}

/*
 * Internal Function to retrieve an HPCB Parameter with a given type and token
*/
EFI_STATUS
InternalHpcbGetParameter (
  IN       HPCB_PARAM_TYPE    HpcbParamType,
  IN       UINT16             HpcbParamToken,
  IN OUT   UINT32           *SizeInByte,
  IN OUT   UINT64           *Value
  )
{
  EFI_STATUS            Status;
  UINT16                GroupId;
  HPCB_PARAM_ATTRIBUTE  *HpcbParamAttributeStream;
  UINT8                 *HpcbParamValueStream;
  UINT32                ParamAttribOffset;
  UINT32                ParamValueOffset;

  //
  // Ensure the input type is legal
  //
  ASSERT (HpcbParamType >= HPCB_PSP_TYPE_CONFIG_DEFAULT_PARAMETERS && HpcbParamType <= HPCB_TYPE_CBS_DEBUG_DUMMY_PARAMETERS);
  if ((HpcbParamType < HPCB_PSP_TYPE_CONFIG_DEFAULT_PARAMETERS) || (HpcbParamType > HPCB_TYPE_CBS_DEBUG_DUMMY_PARAMETERS)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Invalid HpcbParamType\n");
    return EFI_INVALID_PARAMETER;
  }

  ASSERT ((Value != NULL) && (SizeInByte != NULL));
  if ((Value == NULL) || (SizeInByte == NULL)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Parameter Pointer set to NULL\n");
    return EFI_INVALID_PARAMETER;
  }

  Status = InternalHpcbGetParameterDetails (
             HpcbParamType,
             HpcbParamToken,
             &GroupId,
             &HpcbParamAttributeStream,
             &HpcbParamValueStream,
             &ParamAttribOffset,
             &ParamValueOffset
             );
  // Token has been found
  if (Status == EFI_SUCCESS) {
    *SizeInByte = HpcbParamAttributeStream[ParamAttribOffset].Size + 1;
    *Value = 0;
    CopyMem (Value, HpcbParamValueStream + ParamValueOffset, *SizeInByte);
  }

  return Status;
}

/*
 * Internal Function to Set an HPCB Parameter with a given type and token
*/
EFI_STATUS
InternalHpcbSetParameter (
  IN       HPCB_PARAM_TYPE    HpcbParamType,
  IN       UINT16             TokenId,
  IN OUT   UINT32           *SizeInByte,
  IN OUT   UINT64           *Value
  )
{
  EFI_STATUS            Status;
  UINT16                GroupId;
  HPCB_PARAM_ATTRIBUTE  *HpcbParamAttributeStream;
  UINT8                 *HpcbParamValueStream;
  UINT32                ParamAttribOffset;
  UINT32                ParamValueOffset;
  UINT8                 *NewHpcbParamDataStart;
  UINT8                 *NewHpcbParamData;
  UINT32                NewHpcbParamDataSize;
  UINT8                 *NewHpcb;
  UINT32                NewHpcbSize;
  UINT16                HpcbTypeSize;
  HPCB_HEADER           *HpcbShadowCopy;

  // Check if token already exist in the custom blob
  Status = InternalHpcbGetParameterDetails (
             HpcbParamType,
             TokenId,
             &GroupId,
             &HpcbParamAttributeStream,
             &HpcbParamValueStream,
             &ParamAttribOffset,
             &ParamValueOffset
             );
  // Update the value, if token found
  if (EFI_SUCCESS == Status) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Token found in shadow copy\n");

    ASSERT ((HpcbParamAttributeStream[ParamAttribOffset].Size + 1) == *SizeInByte);
    // Return if size not match
    if ((HpcbParamAttributeStream[ParamAttribOffset].Size + 1) != *SizeInByte) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Size mismatch\n");
      return EFI_UNSUPPORTED;
    }

    IDS_HDT_CONSOLE_PSP_TRACE ("Update Value &[%x]=%x\n", HpcbParamValueStream + ParamValueOffset, *Value);
    CopyMem (HpcbParamValueStream + ParamValueOffset, Value, *SizeInByte);
    return EFI_SUCCESS;
  } else {
    // Compose new HPCB Insert the value to tail if token not found
    // Allocate memory to hold size increased HPCB type blob
    // ParamAttrib size need to add one additional byte to hold the end flag
    IDS_HDT_CONSOLE_PSP_TRACE ("Token not found in shadow copy, compose a new shadow copy and insert the token\n");
    HpcbTypeSize = ((HPCB_TYPE_HEADER *)((UINT8 *)HpcbParamAttributeStream - sizeof (HPCB_TYPE_HEADER)))->SizeOfType;
    NewHpcbParamDataSize = HpcbTypeSize - sizeof (HPCB_TYPE_HEADER) + sizeof (HPCB_PARAM_ATTRIBUTE)  + *SizeInByte + ALIGN_SIZE_OF_TYPE;
    NewHpcbParamData     = NULL;
    NewHpcbParamData     = AllocateZeroPool (NewHpcbParamDataSize);
    IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", NewHpcbParamData, NewHpcbParamDataSize);
    ASSERT (NewHpcbParamData != NULL);
    if (NewHpcbParamData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    NewHpcbParamDataStart = NewHpcbParamData;
    // Copy Original Attrib data
    CopyMem (NewHpcbParamData, HpcbParamAttributeStream, ParamAttribOffset * sizeof (HPCB_PARAM_ATTRIBUTE));
    // Insert new attrib
    NewHpcbParamData += ParamAttribOffset * sizeof (HPCB_PARAM_ATTRIBUTE);
    ((HPCB_PARAM_ATTRIBUTE *)NewHpcbParamData)->TimePoint = HPCB_TIME_POINT_ANY;
    ((HPCB_PARAM_ATTRIBUTE *)NewHpcbParamData)->Token     = TokenId;
    ((HPCB_PARAM_ATTRIBUTE *)NewHpcbParamData)->Size = (*SizeInByte - 1);
    // Insert the END flag
    NewHpcbParamData += sizeof (HPCB_PARAM_ATTRIBUTE);
    ((HPCB_PARAM_ATTRIBUTE *)NewHpcbParamData)->TimePoint = 0;
    ((HPCB_PARAM_ATTRIBUTE *)NewHpcbParamData)->Token     = HPCB_TOKEN_CONFIG_LIMIT;
    ((HPCB_PARAM_ATTRIBUTE *)NewHpcbParamData)->Size     = 0;
    ((HPCB_PARAM_ATTRIBUTE *)NewHpcbParamData)->Reserved = 0;
    // Copy Original value data
    NewHpcbParamData += sizeof (HPCB_PARAM_ATTRIBUTE);
    CopyMem (NewHpcbParamData, HpcbParamValueStream, ParamValueOffset);
    // Insert new value
    NewHpcbParamData += ParamValueOffset;
    CopyMem (NewHpcbParamData, Value, *SizeInByte);
    // Insert End flag
    NewHpcbParamData += *SizeInByte;
    *NewHpcbParamData = 0xFF;
    // Now the we have the new composed HPCB param type blob
    // Allocate space for hold new HPCB
    HpcbShadowCopy = GetHpcbShadowCopy ();
    NewHpcbSize    = HpcbShadowCopy->SizeOfHpcb + *SizeInByte + sizeof (HPCB_PARAM_ATTRIBUTE) * 2 + ALIGN_SIZE_OF_TYPE;
    NewHpcb = NULL;
    NewHpcb = AllocateZeroPool (NewHpcbSize);
    IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", NewHpcb, NewHpcbSize);
    ASSERT (NewHpcb != NULL);
    if (NewHpcb == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    // Shadow Copy pointer will be updated in below routine
    Status = HpcbReplaceType (GroupId, HpcbParamType, 0, NewHpcbParamDataStart, NewHpcbParamDataSize, NewHpcb);
    // Free resource
    FreePool (NewHpcbParamDataStart);
    IDS_HDT_CONSOLE_PSP_TRACE ("FREE[%x]\n", NewHpcbParamDataStart);

    return Status;
  }
}

/**
 *
 *  Get HPCB config parameters
 *  This function always get the parameters from shadow memory copy instead of read from real SPI
 *
 *  @param[in]     TokenId      HPCB token ID defined in HPCB_PARAM_CONFIG_TOKEN of HPCB.h
 *  @param[in,out] SizeInByte   Set to non-NULL to get Size in bytes of the HPCB token
 *  @param[in,out] Value        Value of HPCB token ID
 *
 *  @retval EFI_SUCCESS    Get HPCB value successfully
 *  @retval EFI_NOT_FOUND  Can't find the according HPCB token
 *  @retval EFI_INVALID_PARAMETER  Invalid parameters
 *
 **/
EFI_STATUS
HygonPspHpcbGetCfgParameter (
  IN       HPCB_PARAM_CONFIG_TOKEN TokenId,
  IN OUT   UINT32          *SizeInByte,
  IN OUT   UINT64          *Value
  )
{
  EFI_STATUS       Status;
  HPCB_PARAM_TYPE  HpcbParamType;
  BOOLEAN          HpcbRecoveryFlag;

  IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspHpcbGetCfgParameter 0x%x\n", TokenId);

  // Exit service, if recovery flag set
  if (mHpcbAtRuntime == FALSE) {         // byo231107 -
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

  if ((Value == NULL) || (SizeInByte == NULL)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Input parameters error\n");
    return EFI_INVALID_PARAMETER;
  }

  // Get According HPCB Group & Type ID, Check if TokenId in valid range
  if (TokenId > HPCB_TOKEN_CONFIG_PSP_BEGIN && TokenId < HPCB_TOKEN_CONFIG_PSP_END) {
    HpcbParamType = HPCB_PSP_TYPE_CONFIG_PARAMETERS;
  } else if (TokenId > HPCB_TOKEN_CONFIG_CCX_BEGIN && TokenId < HPCB_TOKEN_CONFIG_CCX_END) {
    HpcbParamType = HPCB_CCX_TYPE_CONFIG_PARAMETERS;
  } else if (TokenId > HPCB_TOKEN_CONFIG_DF_BEGIN && TokenId < HPCB_TOKEN_CONFIG_DF_END) {
    HpcbParamType = HPCB_DF_TYPE_CONFIG_PARAMETERS;
  } else if (TokenId > HPCB_TOKEN_CONFIG_MEM_BEGIN && TokenId < HPCB_TOKEN_CONFIG_MEM_END) {
    HpcbParamType = HPCB_MEM_TYPE_CONFIG_PARAMETERS;
  } else if (TokenId > HPCB_TOKEN_CONFIG_GNB_BEGIN && TokenId < HPCB_TOKEN_CONFIG_GNB_END) {
    HpcbParamType = HPCB_GNB_TYPE_CONFIG_PARAMETERS;
  } else if (TokenId > HPCB_TOKEN_CONFIG_FCH_BEGIN && TokenId < HPCB_TOKEN_CONFIG_FCH_END) {
    HpcbParamType = HPCB_FCH_TYPE_CONFIG_PARAMETERS;
  } else {
    IDS_HDT_CONSOLE_PSP_TRACE ("Can't find TokenId in valid HPCB range\n");
    return EFI_INVALID_PARAMETER;
  }

  // Get from custom blob 1st, if not found try default blob
  Status = InternalHpcbGetParameter (HpcbParamType, TokenId, SizeInByte, Value);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Can't find TokenId in custom blob, try to find in default blob\n");
    Status = InternalHpcbGetParameter ((HPCB_PARAM_TYPE)(HpcbParamType - 1), TokenId, SizeInByte, Value);
  }

  if (Status == EFI_SUCCESS) {
    IDS_HDT_CONSOLE_PSP_TRACE ("SizeInByte 0x%x, Value 0x%x\n", (SizeInByte != NULL) ? *SizeInByte : 0, *Value);
  }

  return Status;
}

EFI_STATUS
HygonPspHpcbSetCfgParameter (
  IN       HPCB_PARAM_CONFIG_TOKEN TokenId,
  IN       UINT32           *SizeInByte,
  IN       UINT64          *Value
  )
{
  EFI_STATUS       Status;
  HPCB_PARAM_TYPE  HpcbParamType;
  BOOLEAN          HpcbRecoveryFlag;

  IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspHpcbSetCfgParameter 0x%x SizeInByte 0x%x, Value 0x%x\n", TokenId, (SizeInByte != NULL) ? *SizeInByte : 0, *Value);

  // Exit service, if recovery flag set
  if (mHpcbAtRuntime == FALSE) {         // byo231107 -
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

  if ((Value == NULL) || (SizeInByte == NULL)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Input parameters error\n");
    return EFI_INVALID_PARAMETER;
  }

  // Get According HPCB Group & Type ID, Check if TokenId in valid range
  if (TokenId > HPCB_TOKEN_CONFIG_PSP_BEGIN && TokenId < HPCB_TOKEN_CONFIG_PSP_END) {
    HpcbParamType = HPCB_PSP_TYPE_CONFIG_PARAMETERS;
  } else if (TokenId > HPCB_TOKEN_CONFIG_CCX_BEGIN && TokenId < HPCB_TOKEN_CONFIG_CCX_END) {
    HpcbParamType = HPCB_CCX_TYPE_CONFIG_PARAMETERS;
  } else if (TokenId > HPCB_TOKEN_CONFIG_DF_BEGIN && TokenId < HPCB_TOKEN_CONFIG_DF_END) {
    HpcbParamType = HPCB_DF_TYPE_CONFIG_PARAMETERS;
  } else if (TokenId > HPCB_TOKEN_CONFIG_MEM_BEGIN && TokenId < HPCB_TOKEN_CONFIG_MEM_END) {
    HpcbParamType = HPCB_MEM_TYPE_CONFIG_PARAMETERS;
  } else if (TokenId > HPCB_TOKEN_CONFIG_GNB_BEGIN && TokenId < HPCB_TOKEN_CONFIG_GNB_END) {
    HpcbParamType = HPCB_GNB_TYPE_CONFIG_PARAMETERS;
  } else if (TokenId > HPCB_TOKEN_CONFIG_FCH_BEGIN && TokenId < HPCB_TOKEN_CONFIG_FCH_END) {
    HpcbParamType = HPCB_FCH_TYPE_CONFIG_PARAMETERS;
  } else {
    IDS_HDT_CONSOLE_PSP_TRACE ("Can't find TokenId in valid HPCB range\n");
    return EFI_INVALID_PARAMETER;
  }

  Status = InternalHpcbSetParameter (HpcbParamType, TokenId, SizeInByte, Value);
  return Status;
}

/**
 *
 *  Get Common CBS config parameters
 *  This function always get the parameters from shadow memory copy instead of read from real SPI
 *
 *  @param[in]     TokenId      HPCB token ID defined in HPCB_PARAM_CONFIG_TOKEN of HPCB.h
 *  @param[in,out] SizeInByte   Set to non-NULL to get Size in bytes of the HPCB token
 *  @param[in,out] Value        Value of HPCB token ID
 *
 *  @retval EFI_SUCCESS    Get HPCB value successfully
 *  @retval EFI_NOT_FOUND  Can't find the according HPCB token
 *  @retval EFI_INVALID_PARAMETER  Invalid parameters
 *
 **/
EFI_STATUS
HygonPspHpcbGetCbsCmnParameter (
  IN       UINT16          TokenId,
  IN OUT   UINT32          *SizeInByte,
  IN OUT   UINT64          *Value
  )
{
  EFI_STATUS       Status;
  HPCB_PARAM_TYPE  HpcbParamType;
  BOOLEAN          HpcbRecoveryFlag;

  IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspHpcbGetCbsCmnParameter 0x%x\n", TokenId);

  // Exit service, if recovery flag set
  if (mHpcbAtRuntime == FALSE) {         // byo231107 -
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

  if ((Value == NULL) || (SizeInByte == NULL)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Input parameters error\n");
    return EFI_INVALID_PARAMETER;
  }

  // Check validity of TokenId
  if (TokenId > HPCB_TOKEN_CBS_CMN_BEGIN && TokenId < HPCB_TOKEN_CBS_CMN_END) {
    HpcbParamType = HPCB_TYPE_CBS_COMMON_PARAMETERS;
  } else {
    IDS_HDT_CONSOLE_PSP_TRACE ("Invalid Cbs TokenId\n");
    return EFI_UNSUPPORTED;
  }

  Status = InternalHpcbGetParameter (HpcbParamType, TokenId, SizeInByte, Value);
  return Status;
}

/**
 *
 *  Set Common CBS config parameters
 *  This function always get the parameters from shadow memory copy instead of read from real SPI
 *
 *  @param[in]     TokenId      HPCB token ID defined in HPCB_PARAM_CONFIG_TOKEN of HPCB.h
 *  @param[in,out] SizeInByte   Set to non-NULL to get Size in bytes of the HPCB token
 *  @param[in,out] Value        Value of HPCB token ID
 *
 *  @retval EFI_SUCCESS    Get HPCB value successfully
 *  @retval EFI_NOT_FOUND  Can't find the according HPCB token
 *  @retval EFI_INVALID_PARAMETER  Invalid parameters
 *
 **/
EFI_STATUS
HygonPspHpcbSetCbsCmnParameter (
  IN       UINT16          TokenId,
  IN OUT   UINT32          *SizeInByte,
  IN OUT   UINT64          *Value
  )
{
  EFI_STATUS       Status;
  HPCB_PARAM_TYPE  HpcbParamType;
  BOOLEAN          HpcbRecoveryFlag;

  IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspHpcbSetCbsCmnParameter 0x%x\n", TokenId);

  // Exit service, if recovery flag set
  if (mHpcbAtRuntime == FALSE) {         // byo231107 -
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

  if ((Value == NULL) || (SizeInByte == NULL)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Input parameters error\n");
    return EFI_INVALID_PARAMETER;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Size: 0x%x Value:0x%x\n", *SizeInByte, *Value);

  // Check validity of TokenId
  if (TokenId > HPCB_TOKEN_CBS_CMN_BEGIN && TokenId < HPCB_TOKEN_CBS_CMN_END) {
    HpcbParamType = HPCB_TYPE_CBS_COMMON_PARAMETERS;
  } else {
    IDS_HDT_CONSOLE_PSP_TRACE ("Invalid Cbs TokenId\n");
    return EFI_UNSUPPORTED;
  }

  Status = InternalHpcbSetParameter (HpcbParamType, TokenId, SizeInByte, Value);
  return Status;
}

/**
 *
 *  Set Common CBS debug parameters
 *  This function always get the parameters from shadow memory copy instead of read from real SPI
 *
 *  @param[in]     TokenId      HPCB token ID defined in HPCB_PARAM_CONFIG_TOKEN of HPCB.h
 *  @param[in,out] SizeInByte   Set to non-NULL to get Size in bytes of the HPCB token
 *  @param[in,out] Value        Value of HPCB token ID
 *
 *  @retval EFI_SUCCESS    Get HPCB value successfully
 *  @retval EFI_NOT_FOUND  Can't find the according HPCB token
 *  @retval EFI_INVALID_PARAMETER  Invalid parameters
 *
 **/
EFI_STATUS
HygonPspHpcbSetCbsDbgParameter (
  IN       UINT16          TokenId,
  IN OUT   UINT32          *SizeInByte,
  IN OUT   UINT64          *Value
  )
{
  EFI_STATUS       Status;
  HPCB_PARAM_TYPE  HpcbParamType;
  BOOLEAN          HpcbRecoveryFlag;

  IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspHpcbSetCbsDbgParameter 0x%x\n", TokenId);

  // Exit service, if recovery flag set
  if (mHpcbAtRuntime == FALSE) {         // byo231107 -
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

  if ((Value == NULL) || (SizeInByte == NULL)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Input parameters error\n");
    return EFI_INVALID_PARAMETER;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Size: 0x%x Value:0x%x\n", *SizeInByte, *Value);

  // Check validity of TokenId
  if (TokenId > HPCB_TOKEN_CBS_DBG_BEGIN && TokenId < HPCB_TOKEN_CBS_DBG_END) {
    HpcbParamType = HPCB_TYPE_CBS_DEBUG_PARAMETERS;
  } else {
    IDS_HDT_CONSOLE_PSP_TRACE ("Invalid Cbs TokenId\n");
    return EFI_UNSUPPORTED;
  }

  Status = InternalHpcbSetParameter (HpcbParamType, TokenId, SizeInByte, Value);
  return Status;
}
