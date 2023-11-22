
#include "PlatformPei.h"
#include <Guid/AcpiS3Context.h>
#include <Ppi/SmmAccess.h>
#include <Ppi/MpServices.h>
#include <HygonCpu.h>


#define SMM_ACCESS_PRIVATE_DATA_SIGNATURE   SIGNATURE_32('4','5','s','a')

typedef struct {
  UINT32                          Signature;
  PEI_SMM_ACCESS_PPI              SmmAccess;
  EFI_PEI_PPI_DESCRIPTOR          PpiList;
  UINT32                          SmramDescSize;
  UINT32                          SmramDescCount;
  EFI_SMRAM_DESCRIPTOR            *SmramDesc;
} SMM_ACCESS_PRIVATE_DATA;

#define SMM_ACCESS_PRIVATE_DATA_FROM_THIS(a) \
  CR(a, SMM_ACCESS_PRIVATE_DATA, SmmAccess, SMM_ACCESS_PRIVATE_DATA_SIGNATURE)

STATIC
EFI_STATUS
EFIAPI
PeiSmmOpen (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN PEI_SMM_ACCESS_PPI              *This,
  IN UINTN                           DescriptorIndex
  )
{
  SMM_ACCESS_PRIVATE_DATA  *Private;

  Private = SMM_ACCESS_PRIVATE_DATA_FROM_THIS(This);

  if(DescriptorIndex >= Private->SmramDescCount){
    return EFI_INVALID_PARAMETER;
  }

  if(Private->SmmAccess.LockState){
    return EFI_DEVICE_ERROR;
  } else {
    Private->SmmAccess.OpenState = TRUE;
    DEBUG((EFI_D_INFO, "PeiSmmOpen\n"));    
    return EFI_SUCCESS;
  }
}






STATIC
EFI_STATUS
EFIAPI
PeiSmmClose (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN PEI_SMM_ACCESS_PPI              *This,
  IN UINTN                           DescriptorIndex
  )
{
  SMM_ACCESS_PRIVATE_DATA  *Private;

  Private = SMM_ACCESS_PRIVATE_DATA_FROM_THIS(This);

  if(DescriptorIndex >= Private->SmramDescCount){
    return EFI_INVALID_PARAMETER;
  }
  
  if(Private->SmmAccess.LockState){
    return EFI_DEVICE_ERROR;
  } else {
    Private->SmmAccess.OpenState = FALSE;
    DEBUG((EFI_D_INFO, "PeiSmmClose\n"));    
    return EFI_SUCCESS;
  }
}


STATIC
EFI_STATUS
EFIAPI
PeiSmmLock (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN PEI_SMM_ACCESS_PPI              *This,
  IN UINTN                           DescriptorIndex
  )
{
  SMM_ACCESS_PRIVATE_DATA  *Private;

  Private = SMM_ACCESS_PRIVATE_DATA_FROM_THIS(This);

  if(DescriptorIndex >= Private->SmramDescCount){
    return EFI_INVALID_PARAMETER;
  }

  Private->SmmAccess.LockState = TRUE;
  DEBUG((EFI_D_INFO, "PeiSmmLock\n"));

  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
EFIAPI
PeiSmmGetCapabilities (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN PEI_SMM_ACCESS_PPI              *This,
  IN OUT UINTN                       *SmramMapSize,
  IN OUT EFI_SMRAM_DESCRIPTOR        *SmramMap
  )
{
  UINTN                    DescSize;
  EFI_STATUS               Status;
  SMM_ACCESS_PRIVATE_DATA  *Private;

  Private = SMM_ACCESS_PRIVATE_DATA_FROM_THIS(This);
  DescSize = sizeof(Private->SmramDescSize);
  if (*SmramMapSize < DescSize) {
    Status = EFI_BUFFER_TOO_SMALL;
  } else {
    CopyMem (SmramMap, Private->SmramDesc, DescSize);
    Status = EFI_SUCCESS;
  }

  *SmramMapSize = DescSize;
  return Status;
}


EFI_STATUS
SmmAccessPpiInstall (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS               Status;
  SMM_ACCESS_PRIVATE_DATA  *Private;
  EFI_PEI_HOB_POINTERS     GuidHob;
  EFI_SMRAM_DESCRIPTOR     *SmramDesc;
  UINTN                    SmramDescSize;


  DEBUG((EFI_D_INFO, "SmmAccessPpiInstall\n"));

  GuidHob.Raw = GetFirstGuidHob(&gSmramDescTableGuid);
  ASSERT(GuidHob.Raw != NULL);
  SmramDesc = (VOID*)(GuidHob.Guid+1);
  SmramDescSize = GET_GUID_HOB_DATA_SIZE(GuidHob.Guid);
  DEBUG((EFI_D_INFO, "SMRAM Desc Size:%d\n", SmramDescSize));

  Private = AllocateZeroPool(sizeof(*Private) + SmramDescSize - sizeof(EFI_SMRAM_DESCRIPTOR));
  if (Private == NULL) {
    DEBUG((EFI_D_ERROR, "Alloc SmmAccessPrivate fail.\n"));
    return EFI_OUT_OF_RESOURCES;
  }  
  Private->SmramDescSize  = SmramDescSize;
  Private->SmramDescCount = SmramDescSize/sizeof(EFI_SMRAM_DESCRIPTOR);
  CopyMem(Private->SmramDesc, SmramDesc, SmramDescSize);

  Private->Signature = SMM_ACCESS_PRIVATE_DATA_SIGNATURE;
  Private->SmmAccess.Open            = PeiSmmOpen;
  Private->SmmAccess.Close           = PeiSmmClose;
  Private->SmmAccess.Lock            = PeiSmmLock;
  Private->SmmAccess.GetCapabilities = PeiSmmGetCapabilities;
  Private->SmmAccess.OpenState = !!(SmramDesc[0].RegionState & EFI_SMRAM_OPEN);
  Private->SmmAccess.LockState = !!(SmramDesc[0].RegionState & EFI_SMRAM_LOCKED);

  Private->PpiList.Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  Private->PpiList.Guid  = &gPeiSmmAccessPpiGuid;
  Private->PpiList.Ppi   = &Private->SmmAccess;

  Status = PeiServicesInstallPpi(&Private->PpiList);
  ASSERT_EFI_ERROR (Status);

  return Status;
}




