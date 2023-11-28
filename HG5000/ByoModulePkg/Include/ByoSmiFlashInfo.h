
#ifndef __BYO_SMIFLASH_INFO_H__
#define __BYO_SMIFLASH_INFO_H__


#define SW_SMI_FLASH_SERVICES	                0xec

#define SUBFUNCTION_GET_FLASH_INFO            0x01
#define SUBFUNCTION_GET_FD_AREA               0x02
#define SUBFUNCTION_READ_FLASH_BLOCK          0x03
#define SUBFUNCTION_COMPARE_BIOS_ID           0x04
#define SUBFUNCTION_UPDATE_SMBIOS_DATA        0x05
#define SUBFUNCTION_BIOS_VERIFY               0X06
#define SUBFUNCTION_CLEAR_EVENT_LOG           0X07
#define SUBFUNCTION_CHECK_MFG                 0x08
#define SUBFUNCTION_PROGRAM_FLASH             0x09
#define SUBFUNCTION_MODE_CHECK_BIOS_VER       0x0A
#define SUBFUNCTION_TOPSWAP_ON                0x0B
#define SUBFUNCTION_TOPSWAP_OFF               0x0C
#define SUBFUNCTION_OA3_UPDATE                0x0D
#define SUBFUNCTION_CLEAR                     0x0E
#define SUBFUNCTION_WRITECMOS                 0x0F
#define SUBFUNCTION_DISABLE_USB_POWERBUTTON   0x10
#define SUBFUNCTION_ENABLE_USB_POWERBUTTON    0x11
#define SUBFUNCTION_CHECK_BIOS_LOCK           0x12
#define SUBFUNCTION_PATCH_FOR_USB             0x13
#define SUBFUNCTION_CHECK_BIOS_ID             0x14
#define SUBFUNCTION_CHECK_FOR_PASSWORD        0x18
#define SUBFUNCTION_SET_FOR_FIRST_POWERON     0x19

#define SUBFUNCTION_WRITE_FLASH               0x80
#define SUBFUNCTION_ERASE_FLASH               0x81
#define SUBFUNCTION_IF_UNLOCK                 0x82
#define SUBFUNCTION_IF_LOCK                   0x83
#define SUBFUNCTION_IF_REMOVE_LOGO            0x84
#define SUBFUNCTION_IF_RST_PASSWD             0x85
#define SUBFUNCTION_IF_IS_DMI_LOCK            0x86
#define SUBFUNCTION_IF_SIGN_UPDATE            0x87
#define SUBFUNCTION_IF_SIGN_VERIFY            0x88
#define SUBFUNCTION_IF_SIGN_PREPARE           0x89
#define SUBFUNCTION_IF_UPDATE_UCODE           0x8A       // wz191108 +

#pragma pack(1)

typedef struct {
    UINT32                Offset;
    UINT32                Size;
    EFI_PHYSICAL_ADDRESS  Buffer;
} BIOS_UPDATE_BLOCK_PARAMETER;

#pragma pack()

#endif        // __BYO_SMIFLASH_INFO_H__



