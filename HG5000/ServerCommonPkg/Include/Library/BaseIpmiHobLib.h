/** @file
 Byosoft IPMI hob library header file.
*/

#ifndef _IPMI_HOB_LIB_H_
#define _IPMI_HOB_LIB_H_

#include <BaseIpmi.h>

typedef struct _IPMI_HOB IPMI_HOB;

#pragma pack(1)

//
//HOB structure.
//
struct _IPMI_HOB {
  UINT8                           BmcStatus;
  UINT8                           InterfaceType;
  UINT8                           InterfaceAddressType;
  IPMI_INTERFACE_ADDRESS          InterfaceAddress[IPMI_MAX_BASE_ADDRESS_NUM];
  UINT16                          BmcFirmwareVersion;
  UINT8                           IpmiVersion;
};

#pragma pack()

/**
 Get Ipmi Hob.

 @param[out]        IpmiHob             A pointer to IPMI HOB.

 @retval EFI_SUCCESS                    Get Hob success.
 @retval !EFI_SUCCESS                   Cannot get HOB.
*/
EFI_STATUS
IpmiLibGetIpmiHob (
  OUT IPMI_HOB                      **IpmiHob
  );

#endif

