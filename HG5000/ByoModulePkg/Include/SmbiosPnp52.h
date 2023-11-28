#ifndef __SMBIOS_PNP52_H__
#define __SMBIOS_PNP52_H__


#pragma pack(1)

//
// Please note that SMBIOS structures can be odd byte aligned since the
//  unformated section of each record is a set of arbitrary size strings.
//
typedef struct {
    UINT8   Type;
    UINT8   Length;
    UINT16  Handle;
} SMBIOS_HEADER;

typedef struct {
  UINT8 Command;
  UINT8 FieldOffset;
  UINT32 ChangeMask;
  UINT32 ChangeValue;
  UINT16 DataLength;
  SMBIOS_HEADER StructureHeader;
  UINT8 StructureData[1];
} PNP_52_DATA_BUFFER;


typedef struct {
  UINT16 Signature;
  UINT16 RecordLen;
}SMBIOS_REC_HEADER;


typedef struct {
  UINT8               SubFun;
  PNP_52_DATA_BUFFER  Parameter;
} UPDATE_SMBIOS_PARAMETER;





//
// Params list for PNP function 50-52
//
typedef struct {
  UINT16 Function;
  UINT32 DmiBiosRevisionPtr;
  UINT32 NumStructuresPtr;
  UINT32 StructureSizePtr;
  UINT32 DmiStorageBasePtr;
  UINT32 DmiStorageSizePtr;
  UINT16 BiosSelector;
} PNP_PARAMS_50;

typedef struct {
  UINT16 Function;
  UINT32 StructurePtr;
  UINT32 DmiStrucBufferPtr;
  UINT16 DmiSelector;
  UINT16 BiosSelector;
} PNP_PARAMS_51;

typedef struct {
  UINT16 Function;
  UINT32 DmiDataBufferPtr;
  UINT32 DmiWorkBufferPtr;
  UINT8   Control;
  UINT16 DmiSelector;
  UINT16 BiosSelector;
} PNP_PARAMS_52;


#pragma pack()


typedef enum {
  ByteChanged,
  WordChanged,
  DoubleWordChanged,
  AddChanged,
  DeleteChanged,
  StringChanged,
  BlockChanged,
  Reseved
} FUNC52_CMD;


typedef enum {
    UPDATE_UUID = 1,
    UPDATE_SERIAL_NUMBER,
    UPDATE_ASSET_TAG,
    UPDATE_MODEL_NUMBER,
    UPDATE_BRAND_ID,
    UPDATE_LOCK_STATUS,
    UPDATE_BASE_BOARD_SERIAL_NUMBER,
    UPDATE_BASE_BOARD_ASSET_TAG,
    UPDATE_DMI_STRING_COMMON,
} UPDATE_SMBIOS_TYPE;

#define SMBIOS_REC_SIGNATURE      0x55AA

#endif
