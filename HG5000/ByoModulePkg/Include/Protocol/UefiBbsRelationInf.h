#ifndef __UEFI_BBS_RELATION_PROTOCOL_INFO__
#define __UEFI_BBS_RELATION_PROTOCOL_INFO__

#include <Uefi.h>

typedef struct _UEFI_BBS_RELATION_PROTOCOL  UEFI_BBS_RELATION_INFO_PROTOCOL;

typedef struct {
  UINTN						    UefiHddIndex;
  UINT16            			BbsIndex;
  UINTN							Bus;
  UINTN             			Device;
  UINTN             			Function;
  EFI_DEVICE_PATH_PROTOCOL		*HddDevicePath;	 
  UINTN 	  					HddPortIndex;  
  UINTN 						SataHostIndex;
  CHAR8							SerialNo[30];
} UefiBbsHddRL;


struct _UEFI_BBS_RELATION_PROTOCOL {
  UefiBbsHddRL  *UefiBbsHddTable;
  UINTN UefiSataHddCount;
};

extern EFI_GUID gUefiBbsRelationProtocolGuid;


#endif

