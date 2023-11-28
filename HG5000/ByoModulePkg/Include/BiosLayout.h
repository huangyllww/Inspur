#ifndef __BIOS_LAYOUT_H__
#define __BIOS_LAYOUT_H__

#include <Base.h>

//typedef struct {
//  UINT32  Signature;
//  UINT32  Length;
//} EFI_ACPI_COMMON_HEADER;


#define BYO_BIOS_LAYOUT_INFO_SIGNATURE         SIGNATURE_32('$', 'B', 'Y', 'B')
#define BYO_BIOS_LAYOUT_INFO_SIGNATURE2        SIGNATURE_32('L', 'I', 'F', '$')


// bios normal FV we could get it by FV guid.
// so here we should define romhole as target.
typedef struct {
  UINT32  Signature;
  UINT32  Length;
  UINT32  Signature2;
  UINT32  Version;
  UINT32  Flags;
  
  UINT16  SmiPort;
  UINT16  SmiValue;
  
  UINT32  BiosStart;  
  UINT32  BiosSize;
  
  UINT32  CpuMcOffset;
  UINT32  CpuMcSize;
  
  UINT32  SmbiosOffset;
  UINT32  SmbiosSize;
  
  UINT32  Oa3Offset;
  UINT32  Oa3Size;

  UINT32  LogoOffset;
  UINT32  LogoSize;  

} BIOS_LAYOUT_INFO_TABLE;

#define BIOS_LAYOUT_INFO_TABLE_VERSION           1


#endif