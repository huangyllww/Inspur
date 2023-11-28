/** @file
 Byosoft IPMI Interface DXE common header file.
*/

#ifndef _IPMI_INTERFACE_COMMON_H_
#define _IPMI_INTERFACE_COMMON_H_

#include <Guid/HobList.h>
#include <Protocol/IpmiInterfaceProtocol.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseIpmiHobLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>
#include <IpmiDefinitions.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Smbios.h>

//
// Private data signature
//
#define IPMI_PRIVATE_DATA_SIGNATURE       SIGNATURE_32 ('I','P','M','I')
#define IPMI_PRIVATE_DATA_FROM_THIS(a)    BASE_CR(a, IPMI_PRIVATE_DATA, IpmiProtocol)

#define BMC_KCS_TIMEOUT            5   // [s] Single KSC request timeout
#define BMC_TIMEOUT                300  // [s] How long shall BIOS wait for BMC


#define MAX_SOFT_COUNT             10
#define COMP_CODE_NORMAL           0x00
#define COMP_CODE_TOO_MANY_BYTES   0xca
#define BMC_SLAVE_ADDRESS          0x20

typedef struct _IPMI_PRIVATE_DATA IPMI_PRIVATE_DATA;

/**
 Return SMS_ATN Flag set or not.

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA.

 @retval TRUE                           SMS_ATN bit is set.
 @retval FALSE                          SMS_ATN bit is not set.
*/
typedef
BOOLEAN
(EFIAPI *READ_SMS_ATN_FLAG) (
  IN  IPMI_PRIVATE_DATA             *IpmiData
  );


//
// Private data structure
//
struct _IPMI_PRIVATE_DATA {
  UINTN                                  Signature;
  IPMI_INTERFACE_PROTOCOL                IpmiProtocol;
  EFI_HANDLE                             ProtocolHandle;
  UINT64                                 IpmiBaseAddress;
  UINT64                                 IpmiBaseAddressOffset;
  UINT16                                 BmcFirmwareVersion;
  UINT8                                  IpmiVersion;
  IPMI_BMC_STATUS                        BmcStatus;
  UINTN                                  TotalTimeTicks;
  READ_SMS_ATN_FLAG                      ReadSmsAtnFlag;
  UINT8                                  LastCompleteCode;
};


/**
 Fill private data content

 @param[in]         IpmiHob             A pointer to IPMI Hob
 @param[in]         InSmm               In SMM or not. It is possible to use different base address in SMM.
 @param[out]        IpmiData            A pointer to IPMI_PRIVATE_DATA
*/
VOID
InitialIpmiInterfaceContent (
  OUT IPMI_PRIVATE_DATA             *IpmiData
  );


/**
 Initial function pointer of BT system interface to corrsponding function.

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA
*/
VOID
InitialBtSystemInterface (
  IN  IPMI_PRIVATE_DATA             *IpmiData
  );


/**
 Initial function pointer of KCS system interface to corrsponding function.

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA
*/
VOID
InitialKcsSystemInterface (
  IN  IPMI_PRIVATE_DATA             *IpmiData
  );


/**
 Initial function pointer of SMIC system interface to corrsponding function.

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA
*/
VOID
InitialSmicSystemInterface (
  IN  IPMI_PRIVATE_DATA             *IpmiData
  );

#endif
