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

#include <Base.h>
#include <Uefi.h>

#include <Protocol/HiiConfigRouting.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>

#include <Guid/MdeModuleHii.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>

#include <Library/AmdCbsSetupLib.h>
#include <Protocol/AmdCbsHookProtocol.h>
#include "AmdCbsVariable.h"
#include "AmdCbsFormID.h"
#include "AmdSoc.h"

#include "PiDxe.h"
#include "AGESA.h"
#include "Firmwares/ZP/APCB.h"
#include "ApcbCommon.h"
#include "Protocol/AmdApcbProtocol.h"
#include "Library/AmdPspBaseLibV2.h"
#include "CbsCustomCorePstates.h"

VOID
PrepareCmnTypeAttrib (
  UINT32 *Size,
  APCB_PARAM_ATTRIBUTE *ApcbParaAttrib,
  VOID *CbsVariable
  );

VOID
PrepareCmnTypeValue (
  UINT32 *Size,
  UINT8 *ApcbParaValue,
  VOID *CbsVariable
  );


extern  EFI_BOOT_SERVICES *gBS;
extern  EFI_GUID gAmdApcbDxeServiceProtocolGuid;

UINTN CbsVariableSize = sizeof(CBS_CONFIG);

EFI_STATUS
AmdCbsSetupCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest,
  OUT UINT8                                  *IfrData,
  IN  EFI_HII_HANDLE                         HiiHandle
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  CBS_CONFIG *pSetup_Config;

  pSetup_Config = (CBS_CONFIG *)IfrData;

  switch (Action) {
    case EFI_BROWSER_ACTION_CHANGING:
    case EFI_BROWSER_ACTION_CHANGED:
      switch (QuestionId) {
      case KEY_OC_MODE:
      case KEY_CUSTOM_PSTATE0:
      case KEY_CUSTOM_PSTATE1:
      case KEY_CUSTOM_PSTATE2:
      case KEY_CUSTOM_PSTATE3:
      case KEY_CUSTOM_PSTATE4:
      case KEY_CUSTOM_PSTATE5:
      case KEY_CUSTOM_PSTATE6:
      case KEY_CUSTOM_PSTATE7:
      case KEY_PSTATE0_FID:
      case KEY_PSTATE0_DID:
      case KEY_PSTATE0_VID:
      case KEY_PSTATE1_FID:
      case KEY_PSTATE1_DID:
      case KEY_PSTATE1_VID:
      case KEY_PSTATE2_FID:
      case KEY_PSTATE2_DID:
      case KEY_PSTATE2_VID:
      case KEY_PSTATE3_FID:
      case KEY_PSTATE3_DID:
      case KEY_PSTATE3_VID:
      case KEY_PSTATE4_FID:
      case KEY_PSTATE4_DID:
      case KEY_PSTATE4_VID:
      case KEY_PSTATE5_FID:
      case KEY_PSTATE5_DID:
      case KEY_PSTATE5_VID:
      case KEY_PSTATE6_FID:
      case KEY_PSTATE6_DID:
      case KEY_PSTATE6_VID:
      case KEY_PSTATE7_FID:
      case KEY_PSTATE7_DID:
      case KEY_PSTATE7_VID:
        CustomCorePstate (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
        break;
      default:
        break;
      }

      break;
    case EFI_BROWSER_ACTION_DEFAULT_STANDARD:
      CbsWriteDefalutValue(IfrData);
      switch (QuestionId) {
      default:
        break;
      }
      break;
    default:
      break;
  }

  return Status;
}

VOID
AmdSaveCbsConfigData (
  IN UINT8   *IfrData
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         ApcbEntryExist;
  TYPE_ATTRIB                     TypeAttrib;
  UINT64                          OldApcbPtr;
  UINT32                          EntrySize;
  UINT64                          EntryDest;
  UINT32                          AttributeSize;
  UINT32                          ValueSize;
  UINT32                          TotalSizeExt;
  UINT8                           *DataStream;
  UINT8                           *ValueStream;
  UINT8                           *NewApcbPtr;
  UINT32                          EAX_Reg;
  UINT32                          EBX_Reg;
  UINT32                          ECX_Reg;
  UINT32                          EDX_Reg;
  UINT64                          SocFamilyID;
  AMD_APCB_SERVICE_PROTOCOL       *mApcbDxeServiceProtocol;

  AsmCpuid (
      0x80000001,
      &EAX_Reg,
      &EBX_Reg,
      &ECX_Reg,
      &EDX_Reg
      );
  SocFamilyID = EAX_Reg & RAW_FAMILY_ID_MASK;

  if (SocFamilyID != F15_BR_RAW_ID) {
    NewApcbPtr = NULL;
    // Check the existence and size of APCB  entry 0x60
    ApcbEntryExist = BIOSEntryInfo (BIOS_APCB_INFO, INSTANCE_IGNORED, &TypeAttrib, &OldApcbPtr, &EntrySize, &EntryDest);
    ASSERT (ApcbEntryExist == TRUE);

    // Calculate the buffer needed to store the external byte stream
    AttributeSize = 0;
    ValueSize = 0;
    DataStream = NULL;
    PrepareCmnTypeAttrib (&AttributeSize, (APCB_PARAM_ATTRIBUTE *)DataStream, (CBS_CONFIG *)IfrData);
    PrepareCmnTypeValue (&ValueSize, DataStream, (CBS_CONFIG *)IfrData);
    TotalSizeExt = AttributeSize + ValueSize;
    TotalSizeExt += ((ALIGN_SIZE_OF_TYPE - (TotalSizeExt % ALIGN_SIZE_OF_TYPE)) & (ALIGN_SIZE_OF_TYPE - 1));

    if (0 != TotalSizeExt) {
      DEBUG ((EFI_D_ERROR, "Replace CBS Common Options\n"));
      // Allocate buffer for new Type data stream
      DataStream = NULL;
      DataStream = AllocateZeroPool (TotalSizeExt);
      ASSERT (DataStream != NULL);

      // Create new Type data stream
      PrepareCmnTypeAttrib (&AttributeSize, (APCB_PARAM_ATTRIBUTE *)DataStream, (CBS_CONFIG *)IfrData);
      ValueStream = &DataStream[AttributeSize];
      PrepareCmnTypeValue (&ValueSize, ValueStream, (CBS_CONFIG *)IfrData);

      Status = gBS->LocateProtocol (&gAmdApcbDxeServiceProtocolGuid, NULL, &mApcbDxeServiceProtocol);
      if (EFI_ERROR (Status)) {
        ASSERT (FALSE);
      }

      Status = mApcbDxeServiceProtocol->ApcbUpdateCbsData (mApcbDxeServiceProtocol, DataStream, TotalSizeExt, FALSE);
      if (EFI_ERROR (Status)) {
        ASSERT (FALSE);
      }

      Status = mApcbDxeServiceProtocol->ApcbFlushData (mApcbDxeServiceProtocol);
      if (EFI_ERROR (Status)) {
        ASSERT (FALSE);
      }

      // Free temporary buffer
      Status = gBS->FreePool (DataStream);
      ASSERT (EFI_SUCCESS == Status);
    }
  }

  return;
}

VOID
CbsComboIdentify (
  IN UINT8   *IfrData
  )
{
  UINT8      PkgType;
  UINT32     EAX_Reg;
  UINT32     EBX_Reg;
  UINT32     ECX_Reg;
  UINT32     EDX_Reg;
  UINT64     SocFamilyID;
  CBS_CONFIG *Setup_Config;

  Setup_Config = (CBS_CONFIG *)IfrData;

  Setup_Config->CbsComboFlag = 2;


  AsmCpuid (
      0x80000001,
      &EAX_Reg,
      &EBX_Reg,
      &ECX_Reg,
      &EDX_Reg
      );
  SocFamilyID = EAX_Reg & RAW_FAMILY_ID_MASK;
  PkgType = (EBX_Reg >> 28) & 0x0F;

  //DEBUG ((EFI_D_ERROR, "SocFamilyID = %x, %x\n", SocFamilyID, PkgType));
  switch (SocFamilyID) {
  case F15_BR_RAW_ID:
    // BR
    Setup_Config->CbsComboFlag = 0;
    break;
  case F17_ZP_RAW_ID:
  case F18_DN_RAW_ID:

    Setup_Config->CbsComboFlag = 2;
   if (PkgType == ZP_PKG_AM4 || PkgType == ZP_PKG_DM1) {
    // AM4
     Setup_Config->CbsComboFlag = 1;
     }

    if (PkgType == ZP_SP4) {
      // SP4
      Setup_Config->CbsComboFlag = 3;
    }
    if (PkgType == ZP_SP3) {
      // SP3
      Setup_Config->CbsComboFlag = 2;
    }
    if (PkgType == TR_SP3r2) {
      // SP3r2
      Setup_Config->CbsComboFlag = 4;
    }
    break;
  default:

    Setup_Config->CbsComboFlag = 2;

    break;
  }
}

VOID
CbsOptionControlDependOnStepping (
  IN UINT8   *IfrData
  )
{
    UINT16     ProcessorRevisionId;
    CBS_CONFIG *Setup_Config;
	CPUID_DATA   CpuIdData;
    
    Setup_Config = (CBS_CONFIG *)IfrData;
    Setup_Config->CbsCmnCpuRdseedRdrandCap = 1;
	Setup_Config->CbsCmnCpuSmeeCap = 0;

  	// Read CPUID for family information
  	AsmCpuid (AMD_CPUID_FMF,
      	&(CpuIdData.EAX_Reg),
      	&(CpuIdData.EBX_Reg),
      	&(CpuIdData.ECX_Reg),
      	&(CpuIdData.EDX_Reg)
    );
    
    //<PS-625>
    // High byte contains extended model and extended family
  	ProcessorRevisionId  = (UINT16) ((CpuIdData.EAX_Reg & (CPUID_EXT_MODEL_MASK | CPUID_EXT_FAMILY_MASK)) >> 8);
  	// Low byte contains model and family
  	ProcessorRevisionId |= (CpuIdData.EAX_Reg & (CPUID_STEPPING_MASK | CPUID_BASE_MODEL_MASK));
	if (ProcessorRevisionId == HYGON_REV_F18_DN_A0) {
		Setup_Config->CbsCmnCpuRdseedRdrandCap = 0;
	}
	if (ProcessorRevisionId >= HYGON_REV_F18_DN_B0) {
		Setup_Config->CbsCmnCpuSmeeCap = 1;
	}
    
}
