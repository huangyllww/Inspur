
#ifndef __RSA_SIGN_DATA_HOB__
#define __RSA_SIGN_DATA_HOB__


#pragma pack(1)

typedef struct {
  UINT32    Crc32;
  UINT16    KeySize;
  BOOLEAN   IsProKey;
  UINT8     Reserved;
} PUBKEY_HEADER;

typedef struct {
  PUBKEY_HEADER     Hdr;
  UINT8             KeyN[256];
  UINT8             KeyE[3];
} PK_RSA2048_HOB_DATA;

#pragma pack()


#define RSA2048_SIGN_SIZE     256

extern EFI_GUID gByoSignBiosPubKeyFileNameGuid;

#endif
