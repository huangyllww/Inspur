#ifndef _RAS_ELOG_EVENT_LIB_H_
#define _RAS_ELOG_EVENT_LIB_H_

#include <PiDxe.h>
#include <HygonRas.h>
#include <HygonCpmRas.h>
#include <IndustryStandard/IpmiNetFnStorage.h>
#include <Ipmi/IpmiNetFnStorageDefinitions.h>
#include <Library/IpmiBaseLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/BaseMemoryLib.h> 
#include <Library/MemoryAllocationLib.h> 
#include <Library/PciLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/TimeStampLib.h>



EFI_STATUS
McaToIpmi(
  IN  RAS_MCA_ERROR_INFO	*McaErrorRecord,
  IN  UINT8               BankIndex,  
  IN  DIMM_INFO 			    *DimmInfo,  
  IN  OUT UINT8 			    *ErrorBuffer,
  OUT UINTN               *DataSize
  );

EFI_STATUS
NbioToIpmi(
	  IN  RAS_NBIO_ERROR_INFO	*NbioErrorRecord,
	  IN OUT UINT8				    *ErrorBuffer,
	     OUT UINTN            *DataSize
  );

EFI_STATUS
PcieToIpmi(
  IN     GENERIC_PCIE_AER_ERR_ENTRY *GenPcieAerErrEntry,
  IN OUT UINT8		                *ErrorBuffer,
  OUT    UINTN                      *DataSize
  );


EFI_STATUS
MemTrainingToIpmi(
  IN  UINT8                         Socket,
  IN  UINT8                         Channel,
  IN  UINT8                         Dimm,
  IN  OUT UINT8                     *ErrorBuffer,
  IN  OUT UINTN                     *ErrorBufferSize
  );


EFI_STATUS
SendErrorLogDataToBmc (
  IN  UINT8                         *ElogData,
  IN  UINTN                         DataSize
  );


BOOLEAN
IsIgnoreThisMemCe (
  UINT32     TimeStamp,
  UINT8      DimmIndex
  );


#endif
