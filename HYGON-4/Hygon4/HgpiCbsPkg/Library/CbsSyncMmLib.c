

#include <Uefi.h>
#include <HPCB.h>
#include <Guid/HygonCbsConfig.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Protocol/HygonHpcbProtocol.h>
#include <HygonSoc.h>
#include <Library/HobLib.h>
#include <Library/ByoSharedSmmData.h>
#include <Library/HygonPspBaseLib.h>
#include <Library/HygonCbsVariable.h>


#define EFI_VAR_BS_RT_NV   (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE)

VOID
PrepareCmnTypeAttrib (
  UINT32               *SizeOfAttrib,
  HPCB_PARAM_ATTRIBUTE *HpcbParaAttrib,
  UINT32               *SizeOfValue,
  UINT8                *HpcbParaValue,
  VOID                 *CbsVariable
  );


UINT8   gOldDowncoreCtrl;


VOID
SaveCbsConfigDataMm (
  IN UINT8     *IfrData,
  IN UINTN     IfrDataSize,
  OUT BOOLEAN  *Updated
  )
{
  EFI_STATUS                   Status;
  UINT32                       AttributeSize;
  UINT32                       ValueSize;
  UINT32                       TotalSizeExt;
  UINT8                        *DataStream = NULL;
  UINT8                        *ValueStream;
  HYGON_HPCB_SERVICE_PROTOCOL  *ApcbSvr;


  DEBUG((EFI_D_INFO, "SaveCbsConfigDataMm\n"));
  *Updated = FALSE;

  Status = gSmst->SmmLocateProtocol (
                    &gHygonHpcbSmmServiceProtocolGuid,
                    NULL,
                    (VOID**)&ApcbSvr
                    );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "MmApcbSvr not found\n"));
    return;
  }

  // Calculate the buffer needed to store the external byte stream
  AttributeSize = 0;
  ValueSize     = 0;
  DataStream    = NULL;
  PrepareCmnTypeAttrib (&AttributeSize, (HPCB_PARAM_ATTRIBUTE *)DataStream, &ValueSize, DataStream, (CBS_CONFIG *)IfrData);
  DEBUG ((EFI_D_INFO, "PrepareCmnTypeAttrib: AttributeSize=%d, ValueSize=%d\n", AttributeSize, ValueSize));
  TotalSizeExt  = AttributeSize + ValueSize;
  TotalSizeExt += ((ALIGN_SIZE_OF_TYPE - (TotalSizeExt % ALIGN_SIZE_OF_TYPE)) & (ALIGN_SIZE_OF_TYPE - 1));

  if (0 != TotalSizeExt) {
    DEBUG ((EFI_D_INFO, "Replace CBS Common Options\n"));
    // Allocate buffer for new Type data stream
    DataStream = NULL;
    DataStream = AllocateZeroPool (TotalSizeExt);
    ASSERT (DataStream != NULL);
    if (DataStream == NULL) {
      return;
    }

    // Create new Type data stream
    ValueStream = &DataStream[AttributeSize];
    PrepareCmnTypeAttrib (&AttributeSize, (HPCB_PARAM_ATTRIBUTE *)DataStream, &ValueSize, ValueStream, (CBS_CONFIG *)IfrData);

    Status = ApcbSvr->HpcbUpdateCbsData(ApcbSvr, DataStream, TotalSizeExt, FALSE);
    DEBUG ((EFI_D_INFO, "HpcbUpdateCbsData:%r\n", Status));
    if (EFI_ERROR (Status)) {
      ASSERT (FALSE);
      goto ProcExit;
    }

    Status = ApcbSvr->HpcbFlushData(ApcbSvr);
    if (EFI_ERROR (Status)) {
      ASSERT (FALSE);
      goto ProcExit;
    }
    if(Status != EFI_WARN_WRITE_FAILURE){
      *Updated = TRUE;
    }
  }

ProcExit:
  if(DataStream != NULL){
    FreePool (DataStream);
  }
}






VOID
EFIAPI
CbsAfterSetVariable (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid,
  IN UINT32                  Attributes,
  IN UINTN                   DataSize,
  IN VOID                    *Data
  )  
{
  BOOLEAN      ApcbWritten;
  BOOLEAN      NeedColdReset;
  CBS_CONFIG   *CbsConfig;

  if(StrCmp(VariableName, CBS_SYSTEM_CONFIGURATION_NAME) == 0 &&
     CompareGuid(VendorGuid, &gCbsSystemConfigurationGuid) &&
     Attributes == EFI_VAR_BS_RT_NV){
    SaveCbsConfigDataMm(Data, DataSize, &ApcbWritten);
    
    NeedColdReset = FALSE;
    CbsConfig     = (CBS_CONFIG*)Data;
    DEBUG ((EFI_D_INFO, "DowncoreCtrl %d -> %d\n", gOldDowncoreCtrl, CbsConfig->CbsCmnCpuGenDowncoreCtrl));
    if(gOldDowncoreCtrl != CbsConfig->CbsCmnCpuGenDowncoreCtrl){
      DEBUG((EFI_D_INFO, "DowncoreCtrl changed\n"));
      NeedColdReset = TRUE;
    }

    if(NeedColdReset){
      IoWrite8(0xCD6, 0x10);
      IoWrite8(0xCD7, IoRead8(0xCD7) | BIT1);
      gByoSharedSmmData->ForceColdReset = 1;
      DEBUG((EFI_D_INFO, "NeedColdReset\n"));
    }
    
    if(ApcbWritten){
      gByoSharedSmmData->ApcbWritten = 1;
    } else {
      DEBUG ((EFI_D_INFO, "apcb no change\n"));
    }
  }
}  



EFI_STATUS
EFIAPI
CbsSyncMmLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_PEI_HOB_POINTERS       GuidHob;
  CBS_CONFIG                 *pCbsConfig;
 
  GuidHob.Raw = GetFirstGuidHob(&gHygonCbsVariableHobGuid);
  ASSERT(GuidHob.Raw != NULL);
  pCbsConfig = (CBS_CONFIG*)(GuidHob.Guid + 1);
  gOldDowncoreCtrl = pCbsConfig->CbsCmnCpuGenDowncoreCtrl;

  gByoSharedSmmData->CbsSyncMmLibOn = 1;                        // byo230906 +
  
  return EFI_SUCCESS;
}


