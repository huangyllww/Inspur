#ifndef _RAS_ELOG_EVENT_LIB_H_
#define _RAS_ELOG_EVENT_LIB_H_

#include <AmdRas.h>
#include <AmdCpmRas.h>
#include <Library/DebugLib.h>

typedef enum _MCA_STATUS_UMC_EXTERR_TYPE {
  DramEccErr = 0,
  WriteDataPoisonErr,
  SdpParityErr,
  ApbErr,
  AddressCommandParityErr,
  WriteDataCrcErr
} MCA_STATUS_UMC_EXTERR_TYPE;
//
// Common Defines
//
typedef enum {
  EfiElogSmSMBIOS,
  EfiElogSmIPMI,
  EfiElogSmMachineCritical,
  EfiElogSmASF,
  EfiElogSmOEM,
  EfiSmElogMax
} EFI_SM_ELOG_TYPE;

UINT32
PopCount16(
  UINT16    Testword
  );
  
UINT32
GetSymbolSize(
        VOID);
	EFI_STATUS
	McaToIpmi(
	  IN  RAS_MCA_ERROR_INFO	*McaErrorRecord,
	  IN  OUT UINT8 			*ErrorBuffer,
	  IN  DIMM_INFO 			*DimmInfo
	  );
	
	EFI_STATUS
	NbioToIpmi(
	  IN  RAS_NBIO_ERROR_INFO	*NbioErrorRecord,
	  IN OUT UINT8				*ErrorBuffer
	  );

	EFI_STATUS
	PcieToIpmi(
	  IN  GENERIC_PCIE_AER_ERR_ENTRY *GenPcieAerErrEntry,
	  IN OUT UINT8			*ErrorBuffer
	  );

EFI_STATUS
MemTestToIpmi(
  IN  UINT8                         Socket,
  IN  UINT8                         Channel,
  IN  UINT8                         Dimm,
  IN  OUT UINT8                     *ErrorBuffer,
  IN  OUT UINTN                     *ErrorBufferSize
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
SetEventLogData (
  IN  UINT8                         *ElogData,
  IN  EFI_SM_ELOG_TYPE              DataType,
  IN  BOOLEAN                       AlertEvent,
  IN  UINTN                         DataSize,
  OUT UINT64                        *RecordId
  );

EFI_STATUS
SendErrorLogDataToBmc (
  IN  UINT8                         *ElogData,
  IN  UINTN                         DataSize
  );

EFI_STATUS
GetGenericElogProtocol (
);

EFI_STATUS
EfiSmGetTimeStamp (
  OUT UINT32 *NumOfSeconds
  );

#endif
