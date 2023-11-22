#ifndef __BYO_HYGON_COMM_LIB_H__
#define __BYO_HYGON_COMM_LIB_H__



#define WAK_TYPE_NONE                0
#define WAK_TYPE_POWERBUTTON         1
#define WAK_TYPE_RTC                 2
#define WAK_TYPE_PS2_KB              3
#define WAK_TYPE_PS2_MS              4
#define WAK_TYPE_PCIE                5
#define WAK_TYPE_OBLAN               6
#define WAK_TYPE_USB                 7
#define WAK_TYPE_PBOR                8            // power button override
#define WAK_TYPE_RING                9
#define WAK_TYPE_POWER_LOSS          10
#define WAK_TYPE_SYS_RESET           11
#define WAK_TYPE_UNKNOWN             0xFF



#define PCI_DEV_MMBASE(Bus, Device, Function) \
    ( \
      (UINTN)PcdGet64(PcdPciExpressBaseAddress) + (UINTN) ((Bus) << 20) + (UINTN) ((Device) << 15) + (UINTN) \
        ((Function) << 12) \
    )


#define HOST0_PCI_REG(Reg)        (PCI_DEV_MMBASE(0, 0, 0)+Reg)

#define SMBUS_PCI_REG(Reg)        (PCI_DEV_MMBASE(0, 0xb, 0)+Reg)
#define LPC_PCI_REG(Reg)          (PCI_DEV_MMBASE(0, 0xb, 3)+Reg)

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





#define FCH_PM_MMIO_BASE             0xFED80300
#define   PM_S5_RESET_STATUS           0xC0
#define     RESET_STATUS_SYNC_FLOOD      BIT27
#define     RESET_STATUS_DO_RESET        BIT19
#define     RESET_STATUS_USER_RST        BIT16



#define PM_IO_INDEX                  0xCD6
#define PM_IO_DATA                   0xCD7



#define TKN_NB_PROG_DEVICE_REMAP         0x13b100c0
#define TKN_IOHC_NB_SMN_INDEX_2_BIOS     0x00B8
#define TKN_IOHC_NB_SMN_DATA_2_BIOS      0x00BC

#define TKN_NB_SMN_INDEX_EXT_3_BIOS      0x00C0
#define TKN_NB_SMN_INDEX_3_BIOS          0x00C4
#define TKN_NB_SMN_DATA_3_BIOS           0x00C8


#define ACPI_S0       (0 << 10)
#define ACPI_S3       (3 << 10)
#define ACPI_S4       (4 << 10)
#define ACPI_S5       (5 << 10)
#define ACPI_BM_MASK  (7 << 10)


VOID LibHygonEarlyDecode(UINT16 IpmiIoBase, UINT16 PmBase);
VOID LibTpmDecode(UINT8 TpmSelect);
UINT8 LibGetCpuPhyAddrBits();
UINT8 LibGetAcpiWakeUpSrc(UINT16 PmBase);
UINT8 LibGetCpuModel();

EFI_STATUS
LibSocSerialPortInitialize (
    UINT8  UartIndex
  );

VOID LibEnableLpcSioUart0Decode();

BOOLEAN
LibCheckCmosBad (
  VOID
  );

VOID
LibClearCmosBad (
  VOID
  );

UINT8 CmosRead(UINT8 Address);
VOID CmosWrite(UINT8 Address, UINT8 Data);


#define CPU_MODEL_749X       0
#define CPU_MODEL_748X       1
#define CPU_MODEL_UNKNOWN    0xFF


#endif
