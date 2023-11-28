
#ifndef __PLATFORM_DEFINITION_H__
#define __PLATFORM_DEFINITION_H__

#include <uefi.h>
#include <Library/IoLib.h>



//------------------------- Platform define -------------------------
#define PCI_DEV_MMBASE(Bus, Device, Function) \
    ( \
      (UINTN)PcdGet64(PcdPciExpressBaseAddress) + (UINTN) ((Bus) << 20) + (UINTN) ((Device) << 15) + (UINTN) \
        ((Function) << 12) \
    )

/// Standard PCI Config Space registers definitions.
#define PCI_VID_REG                          0x00
#define PCI_DID_REG                          0x02
#define PCI_CMD_REG                          0x04
#define   PCI_CMD_IO_EN                        BIT0
#define   PCI_CMD_MEM_EN                       BIT1
#define   PCI_CMD_BM_EN                        BIT2
#define   PCI_CMD_INT_DIS                      BIT10
#define PCI_PRI_STS_REG                      0x06
#define   PCI_STS_CAP_LIST                     BIT4
#define PCI_REV_ID_REG                       0x08
#define PCI_CC_PI_REG                        0x09
#define PCI_SCC_REG                          0x0A
#define   PCI_SCC_IDE                          0x01
#define   PCI_SCC_AHCI                         0x06
#define   PCI_SCC_RAID                         0x04
#define PCI_BCC_REG                          0x0B
#define   PCI_BCC_STORAGE                      0x01
#define   PCI_BCC_NETWORK                      0x02
#define   PCI_BCC_DISPLAY                      0x03
#define PCI_HDR_REG                          0x0E
#define PCI_BAR0_REG                         0x10
#define PCI_BAR1_REG                         0x14
#define PCI_BAR2_REG                         0x18
#define PCI_BAR3_REG                         0x1C
#define PCI_BAR4_REG                         0x20
#define PCI_BAR5_REG                         0x24
#define PCI_SSID_REG                         0x2C

#define PCI_PBN_REG                          0x18
#define PCI_SCBN_REG                         0x19
#define PCI_SBBN_REG                         0x1A
#define PCI_PBU32_REG                        0x28
#define PCI_PLU32_REG                        0x2C

#define PCI_CAP_POINT_REG                    0x34
#define PCI_EXROM_ADDR_REG                   0x38
#define PCI_INT_LINE_REG                     0x3C

#define PCI_CAP_ID_PM                        0x01


#define HOST0_PCI_REG(Reg)        (PCI_DEV_MMBASE(0, 0, 0)+Reg)

#define SMBUS_PCI_REG(Reg)        (PCI_DEV_MMBASE(0, 0x14, 0)+Reg)
#define LPC_PCI_REG(Reg)          (PCI_DEV_MMBASE(0, 0x14, 3)+Reg)

#define DF0_PCI_REG(Reg)          (PCI_DEV_MMBASE(0, 0x18, 0)+Reg)
#define DF0F1_PCI_REG(Reg)        (PCI_DEV_MMBASE(0, 0x18, 1)+Reg)
#define DF1_PCI_REG(Reg)          (PCI_DEV_MMBASE(0, 0x19, 0)+Reg)
#define DF2_PCI_REG(Reg)          (PCI_DEV_MMBASE(0, 0x1A, 0)+Reg)
#define DF3_PCI_REG(Reg)          (PCI_DEV_MMBASE(0, 0x1B, 0)+Reg)
#define DF4_PCI_REG(Reg)          (PCI_DEV_MMBASE(0, 0x1C, 0)+Reg)
#define DF5_PCI_REG(Reg)          (PCI_DEV_MMBASE(0, 0x1D, 0)+Reg)
#define DF6_PCI_REG(Reg)          (PCI_DEV_MMBASE(0, 0x1E, 0)+Reg)
#define DF7_PCI_REG(Reg)          (PCI_DEV_MMBASE(0, 0x1F, 0)+Reg)


#define DF0_IOBASE_REG             0xC0
#define DF0_IOLIMIT_REG            0xC4

#define PLAT_PCI_IO_LIMIT               0xFFFF
#define PLAT_PCI_IO_BASE                0x1000

#define S3_PEI_MEMORY_SIZE             0x100000

#define S3_DATA_RECORD_SIZE            0x1000


#define ASPEED_VIDEO_ROM_FILE_GUID \
    {0xf27751d8, 0x67af, 0x48af, 0xb6, 0xf4, 0x6b, 0x80, 0x8b, 0x76, 0xb5, 0xda}




#define PMIO_STS_REG          0x00
#define   PMIO_STS_TMROF        BIT0
#define   PMIO_STS_BM           BIT4
#define   PMIO_STS_GBL          BIT5
#define   PMIO_STS_PWRBTN       BIT8
#define   PMIO_STS_RTC          BIT10
#define   PMIO_STS_PWBTNOR      BIT11
#define   PMIO_STS_PCIEW        BIT14
#define   PMIO_STS_WAK          BIT15

#define PMIO_PM_EN            0x02
#define   PMIO_PM_EN_PWRBTN     BIT8
#define   PMIO_PM_EN_RTC        BIT10
#define   PMIO_PM_DIS_PCIEW     BIT14




#define WAK_TYPE_NONE                0
#define WAK_TYPE_POWERBUTTON         1
#define WAK_TYPE_RTC                 2
#define WAK_TYPE_PS2_KB              3
#define WAK_TYPE_PS2_MS              4
#define WAK_TYPE_PCIE                5
#define WAK_TYPE_OBLAN               6
#define WAK_TYPE_USB                 7
#define WAK_TYPE_PBOR                8						// power button override
#define WAK_TYPE_RING                9
#define WAK_TYPE_POWER_LOSS          10
#define WAK_TYPE_UNKNOWN             0xFF


extern EFI_GUID gCarTopDataHobGuid;

typedef struct {
  UINT32  Microcode;       // include header
  UINT32  ResetTsc;
  UINT32  JmpSecCoreTsc;
} CAR_TOP_DATA;



#define __STR(a)        #a
#define TKN2STR(a)      __STR(a)

#define __TKN2VERSTR(a,b)  #a#b
#define TKN2VERSTR(a,b)    __TKN2VERSTR(a,b)


#define REALTEK_VENDOR_ID            0x10EC  
#define HG_VENDOR_ID                 0x1D94

#define PM_IO_INDEX                  0xCD6
#define PM_IO_DATA                   0xCD7

#define SB_IOAPIC_BASE_ADDRESS       0xFEC00000




#endif


