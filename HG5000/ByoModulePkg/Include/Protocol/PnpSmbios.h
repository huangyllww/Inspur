
#ifndef _PNP_SMBIOS_PROTOCOL_H_
#define _PNP_SMBIOS_PROTOCOL_H_

typedef struct _EFI_PNP_SMBIOS_PROTOCOL EFI_PNP_SMBIOS_PROTOCOL;

/**
  Pnp 50 function get the PNP_50_DATA_BUFFER to Pnp50DataPtr:
    out:
    UINT8     SmbiosBcdRevision;
    UINT16    NumberOfSmbiosStructures;
    UINT16    MaxStructureSize;
    UINT32    TableAddress;
    UINT16    TableLength;
**/
typedef
EFI_STATUS
(EFIAPI *PNP_50_GET_SMBIOS_INFORMATION)(
  IN      EFI_PNP_SMBIOS_PROTOCOL   *This,
  IN OUT  VOID                      *Pnp50DataPtr
  );

/**
  Pnp 51 function to get PNP_51_DATA_BUFFER due to Pnp51DataPtr->StrucHandle:
    in out  UINT16                      StrucHandle;
    out     SMBIOS_STRUCTURE_POINTER    *StructPtr;
**/
typedef
EFI_STATUS
(EFIAPI *PNP_51_GET_SMBIOS_STRUCTURE)(
  IN      EFI_PNP_SMBIOS_PROTOCOL   *This,
  IN OUT  VOID                      *Pnp51DataPtr
  );

typedef
EFI_STATUS
(EFIAPI *PNP_51_GET_SMBIOS_STRUCT_TYPE)(
  IN      EFI_PNP_SMBIOS_PROTOCOL   *This,
  IN OUT  VOID                      *Pnp51DataPtr
  );

/**
  Pnp52 Function to set PNP_52_DATA_BUFFER to memory Smbios Memory and Flash
    in:
      UINT8 Command;
      UINT8 FieldOffset;
      UINT32 ChangeMask;
      UINT32 ChangeValue;
      UINT16 DataLength;
      SMBIOS_HEADER StructureHeader;
      UINT8 StructureData[1];
    if Control == 0, do not update to mem and flash, else update to mem and flash
**/
typedef
EFI_STATUS
(EFIAPI *PNP_52_SET_SMBIOS_STRUCTURE)(
  IN      EFI_PNP_SMBIOS_PROTOCOL   *This,
  IN OUT  VOID                      *Pnp52DataPtr,
  IN      UINT8                     Control
  );

struct _EFI_PNP_SMBIOS_PROTOCOL {
  PNP_50_GET_SMBIOS_INFORMATION   Pnp50GetSmbiosInformation;
  PNP_51_GET_SMBIOS_STRUCTURE     Pnp51GetSmbiosStructure;
  PNP_52_SET_SMBIOS_STRUCTURE     Pnp52SetSmbiosStructure;
  PNP_51_GET_SMBIOS_STRUCT_TYPE   Pnp51GetSmbiosStructByType;
};

extern EFI_GUID gEfiPnpSmbiosProtocolGuid;

#endif
