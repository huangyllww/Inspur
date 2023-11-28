

#ifndef __BYO_BIOS_INFO2_H__
#define __BYO_BIOS_INFO2_H__

#include <Uefi.h>

#pragma pack(1)


#define BYO_BIOS_INFO2_SIGNATURE         SIGNATURE_32('$', 'B', 'I', '$')

//Flash Tool basic op code
#define OP_PROGRAM_FLASH                   0x09
#define OP_BACKUP_FLASH_                   0x14
#define OP_RESERVE_OEM__                   0x91
#define OP_RESERVE_NVM__                   0x92

//Platform defined op code
#define OP_BIOS_VERIFY__                   0x06
#define OP_TOPSWAP_ON___                   0x0B
#define OP_TOPSWAP_OFF__                   0x0C
#define OP_CLEAR________                   0x0E

// 0 - 15 is valid
#define FLASH_STEP_OK                   0
#define FLASH_STEP_PREP                 0 
#define FLASH_STEP_BK                   1
#define FLASH_STEP_RUN                  2
#define FLASH_STEP_NV                   3                


typedef struct {
  UINT32                 Signature;
  UINT32                 HeaderLength;
  UINT32                 Step_NUM;
  UINT32                 BIOS_Size;  
}FLASH_HEADER;

typedef struct {
  UINT8            Name[8];
  UINT32           Offset;
  UINT32           Size;
}FLASH_INFO;

typedef struct {
  FLASH_INFO        Info;   //Flash info
  FLASH_INFO        Info2;  //File info
  UINT32            Step;
  UINT32            Op;
  UINT8             Name[28];
}FLASH_STEP;

typedef struct {
  FLASH_HEADER           Header;
  FLASH_STEP             Step[1];
} MY_BYO_BIOS_INFO2_TMP;

#pragma pack()

#endif

