/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH registers definition
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
****************************************************************************
*/
#define TAISHAN_SD_BUS_DEV_FUN               ((0x14 << 3) + 6)      // SD Controller
#define TAISHAN_SD_PCI_DEV                   20
#define TAISHAN_SD_PCI_FUNC                  6

#define TAISHAN_BUSAPORT_BUS_DEV_FUN         ((0x8 << 3) + 1)
#define TAISHAN_SATA_DEV_FUN                 ((0x0 << 3) + 2)
#define TAISHAN_SATA_DEV                     0
#define TAISHAN_SATA_FUN                     2
#define TAISHAN_SATA_PORT_NUM                8

#define TAISHAN_MAX_FCH_SUPPORT              8

#define TAISHAN_FCH_SATA_DID                 0x7901
#define TAISHAN_FCH_SATA_AHCI_DID            0x7901
#define TAISHAN_FCH_SATA_AMDAHCI_DID         0x7904
#define TAISHAN_FCH_SATA_RAID_DID            0x7916

#define TAISHAN_FCH_USB_XHCI_DID             0x7914      // Dev 0x10 Func 0
#define TAISHAN_FCH_SMBUS_DID                0x790B      // Dev 0x14 Func 0
#define TAISHAN_FCH_LPC_DID                  0x790E      // Dev 0x14 Func 3
#define TAISHAN_FCH_SD_DID                   0x7906      // Dev 0x14 Func 7

#define AMD_D0F0   ((0 << 3) + 0)
#define AMD_D18F0  ((0x18 << 3) + 0)

#define SOC_ZPID                             0x14501022ul
#define DF_ZPID                              0x14601022ul

#define HYGON_SOC_ZPID                       0x14501D94
#define HYGON_DF_ZPID                        0x14601D94


//CPU Package Type
#define PKG_SP4                              0x1
#define PKG_AM4                              0x2
#define PKG_SP4_R2                           0x3
#define PKG_SP3                              0x4
#define PKG_DM1                              0x6  //DM1-186
#define PKG_SP3_R2                           0x7

#define PGFSM_CMD_POWER_DOWN                 (0x00 << 1)
#define PGFSM_CMD_POWER_UP                   (0x01 << 1)
#define PGFSM_CMD_POWER_WRITE                (0x02 << 1)
#define PGFSM_CMD_POWER_READ                 (0x03 << 1)
#define PGFSM_SOFTWARE_CONTROL               BIT0

#define PGFSM_STATUS_PWR_STS                 (BIT0 + BIT1)
#define PGFSM_STATUS_PWR_ON                  0
#define PGFSM_STATUS_PWR_OFF                 BIT1

#define PGFSM_SELECT_0                       BIT8

//USB RSMU registers
#define FCH_TS_USB_RSMU_PGFSM_CNTL           0x0004D520ul        //0x0004D520ul
#define FCH_TS_USB_RSMU_PGFSM_WR_DATA        0x0004D524ul        //0x0004D524ul
#define FCH_TS_USB_RSMU_PGFSM_RD_DATA        0x0004D528ul        //0x0004D528ul

//USB3 IP registers
#define FCH_TS_USB_GUCTL1                    0x16C0C11Cul        //0x16C0C11C
#define FCH_TS_USB_GSNPSID                   0x16C0C120ul        //0x16C0C120
#define GSNPSID_REVISION_290A                0x5533290Aul
#define GSNPSID_REVISION_320A                0x5533320Aul
#define FCH_TS_USB_GUCTL                     0x16C0C12Cul        //0x16C0C12C
#define FCH_TS_USB_GUCTL2                    0x16C0C19Cul        //0x16C0C19C
#define GUCTL2_ENABLE_EP_CACHE_EVICT         BIT12
#define FCH_TS_USB_GUSB3PIPECTL0             0x16C0C2C0ul        //0x16C0C2C0
#define FCH_TS_USB_GUSB3PIPECTL1             0x16C0C2C4ul        //0x16C0C2C4
#define FCH_TS_USB_GUSB3PIPECTL2             0x16C0C2C8ul        //0x16C0C2C8
#define FCH_TS_USB_GUSB3PIPECTL3             0x16C0C2CCul        //0x16C0C2CC

//USB3 CONTAINER Registers
#define FCH_SMN_USB_CONTAINER                0x16D80000ul
#define FCH_TS_USB_SHADOWED_PMCSR            FCH_SMN_USB_CONTAINER + 0x10         //0x10
  #define FCH_TS_USB_PMCSR_PME_EN            (1 << 8)
#define FCH_TS_USB_SHADOWED_POWERSTATE       FCH_SMN_USB_CONTAINER + 0x14         //0x14
#define FCH_TS_USB_RESET                     FCH_SMN_USB_CONTAINER + 0x100        //0x100
  #define FCH_TS_USB_S5_RESET                 (1 << 0x00)
  #define FCH_TS_USB_CONTROLLER_RESET         (1 << 0x01)
  #define FCH_TS_USB_PHY_RESET                (1 << 0x02)
#define FCH_TS_USB_PGFSM_OVERRIDE            FCH_SMN_USB_CONTAINER + 0x104        //0x104
#define FCH_TS_USB_CURRENT_PWR_STS           FCH_SMN_USB_CONTAINER + 0x108        //0x108
#define FCH_TS_USB_INTERRUPT_CONTROL         FCH_SMN_USB_CONTAINER + 0x118        //0x118
#define FCH_TS_USB_PORT_OCMAPPING0           FCH_SMN_USB_CONTAINER + 0x120        //0x120
#define FCH_TS_USB_PORT_DISABLE0             FCH_SMN_USB_CONTAINER + 0x128        //0x128
#define FCH_TS_USB_PORT_CONTROL              FCH_SMN_USB_CONTAINER + 0x130        //0x130

#define FCH_TS_USB_PHY0_RX_OVRD_IN_HI        0x16D04018ul        //0x4018
#define FCH_TS_USB_PHY1_RX_OVRD_IN_HI        0x16D04418ul        //0x4418
#define FCH_TS_USB_PHY2_RX_OVRD_IN_HI        0x16D04818ul        //0x4818
#define FCH_TS_USB_PHY3_RX_OVRD_IN_HI        0x16D04C18ul        //0x4C18
  #define RX_EQ_OVRD                         BIT11
  #define RX_EQ                              (BIT8 + BIT9 + BIT10)
  #define RX_EQ_EN_OVRD                      BIT7
  #define RX_EQ_EN                           BIT6															 // 
#define FCH_TS_USB_PHY0_PHYINTERNALLANEREGISTER        0x16D04098ul        //0x4098
#define FCH_TS_USB_PHY1_PHYINTERNALLANEREGISTER        0x16D04498ul        //0x4498
#define FCH_TS_USB_PHY2_PHYINTERNALLANEREGISTER        0x16D04898ul        //0x4898
#define FCH_TS_USB_PHY3_PHYINTERNALLANEREGISTER        0x16D04C98ul        //0x4C98

#define FCH_TS_USB_PHY0_PHYCFG0              0x16D08200ul        //0x8200
#define FCH_TS_USB_PHY1_PHYCFG0              0x16D08600ul        //0x8600
#define FCH_TS_USB_PHY2_PHYCFG0              0x16D08A00ul        //0x8A00
#define FCH_TS_USB_PHY3_PHYCFG0              0x16D08E00ul        //0x8E00
#define FCH_TS_USB_PHY0_LANEPARACTL0         0x16D0C000ul        //0xC000
#define FCH_TS_USB_PHY1_LANEPARACTL0         0x16D0C400ul        //0xC400
#define FCH_TS_USB_PHY2_LANEPARACTL0         0x16D0C800ul        //0xC800
#define FCH_TS_USB_PHY3_LANEPARACTL0         0x16D0CC00ul        //0xCC00
#define FCH_TS_USB_PHY0PARACTL2              0x16D08008ul        //0x8008
#define FCH_TS_USB_PHY1PARACTL2              0x16D08408ul        //0x8408
#define FCH_TS_USB_PHY2PARACTL2              0x16D08808ul        //0x8808
#define FCH_TS_USB_PHY3PARACTL2              0x16D08C08ul        //0x8C08

#define FCH_TS_USB_NBIF_STRAP0               0x10134600ul
#define FCH_TS_USB_SMN_PCICFG                0x10143000ul

//SATA RSMU registers
#define FCH_SMN_SATA_RSMU_BASE               0x010A8000ul
#define FCH_SMN_SATA_CONTROL_RSMU            0x03100000ul
#define FCH_SMN_SATA_CONTROL_BAR5            0x03101000ul
#define FCH_SMN_SATA_CONTROL_SLOR            0x03101800ul

#define FCH_SATA_RSMU_COLD_RESETB            FCH_SMN_SATA_RSMU_BASE + 0x04
#define FCH_SATA_RSMU_HARD_RESETB            FCH_SMN_SATA_RSMU_BASE + 0x08

#define FCH_TS_SATA_MISC_CONTROL             FCH_SMN_SATA_CONTROL_RSMU + 0x00     //0x00
#define FCH_TS_SATA_OOB_CONTROL              FCH_SMN_SATA_CONTROL_RSMU + 0x02     //0x02
#define FCH_TS_SATA_AOAC_CONTROL             FCH_SMN_SATA_CONTROL_RSMU + 0x04     //0x04
#define FCH_TS_SATA_EVENT_SELECT             FCH_SMN_SATA_CONTROL_RSMU + 0x08     //0x08
#define FCH_TS_SATA_NBIF_CONTROL             FCH_SMN_SATA_CONTROL_RSMU + 0x0A     //0x0A

#define FCH_SMN_SATA_NBIF_STRAP0             0x10234400ul
#define FCH_SMN_SATA_NBIF_STRAP3             0x1023440Cul
#define FCH_SMN_SATA_NBIF_STRAP13            0x10234434ul

#define FCH_TS_SATA_SMN_PCICFG               0x10242000ul

//xGbE registers
#define FCH_SMN_XGBE_MMIO0_BASE              0x1631F000ul

#define FCH_TS_XGBE_PAD_MUX0                 FCH_SMN_XGBE_MMIO0_BASE + 0x00
#define FCH_TS_XGBE_PAD_MUX1                 FCH_SMN_XGBE_MMIO0_BASE + 0x04
#define FCH_TS_XGBE_PORT0_CLK_GATE           FCH_SMN_XGBE_MMIO0_BASE + 0x30
#define FCH_TS_XGBE_PORT0_PROPERTY0          FCH_SMN_XGBE_MMIO0_BASE + 0x800
#define FCH_TS_XGBE_PORT0_PROPERTY1          FCH_SMN_XGBE_MMIO0_BASE + 0x804
#define FCH_TS_XGBE_PORT0_PROPERTY2          FCH_SMN_XGBE_MMIO0_BASE + 0x808
#define FCH_TS_XGBE_PORT0_PROPERTY3          FCH_SMN_XGBE_MMIO0_BASE + 0x80C
#define FCH_TS_XGBE_PORT0_PROPERTY4          FCH_SMN_XGBE_MMIO0_BASE + 0x810
#define FCH_TS_XGBE_PORT0_PROPERTY5          FCH_SMN_XGBE_MMIO0_BASE + 0x814
#define FCH_TS_XGBE_PORT0_MAC_ADDR_LOW       FCH_SMN_XGBE_MMIO0_BASE + 0x880
#define FCH_TS_XGBE_PORT0_MAC_ADDR_HIGH      FCH_SMN_XGBE_MMIO0_BASE + 0x884

#define FCH_SMN_XGBE0_NBIF_STRAP0            0x10234800ul

//FCH register SMN address
#define FCH_SMN_SMI_BASE                     0x02D01200ul
#define FCH_SMN_IOMUX_BASE                   0x02D01D00ul
#define FCH_SMN_MISC_BASE                    0x02D01E00ul
#define FCH_SMN_GPIO_BASE                    0x02D02500ul

#define FCH_TS_IRQ_INTA         0x00
#define FCH_TS_IRQ_INTB         0x01
#define FCH_TS_IRQ_INTC         0x02
#define FCH_TS_IRQ_INTD         0x03
#define FCH_TS_IRQ_GENINT2      0x05
#define FCH_TS_IRQ_SCI          0x10    // SCI
#define FCH_TS_IRQ_SMBUS0       0x11    // SMBUS0
#define FCH_TS_IRQ_ASF          0x12    // ASF
#define FCH_TS_IRQ_GBE0         0x14    // GBE0
#define FCH_TS_IRQ_GBE1         0x15    // GBE1
#define FCH_TS_IRQ_SD           0x17    // SD
#define FCH_TS_IRQ_SDIO         0x1A    // SDIO
#define FCH_TS_IRQ_GPIOa        0x21
#define FCH_TS_IRQ_GPIOb        0x22
#define FCH_TS_IRQ_GPIOc        0x23
#define FCH_TS_IRQ_USB_EMU      0x30    // AMD USB Emulation Interrupt
#define FCH_TS_IRQ_USB_DR0      0x31    // USB DualRole Intr0
#define FCH_TS_IRQ_USB_DR1      0x32    // USB DualRole Intr1
#define FCH_TS_IRQ_USB_XHCI0    0x34    // XHCI0
#define FCH_TS_IRQ_USB_SSIC     0x35    // SSIC
#define FCH_TS_IRQ_SATA         0x41    // SATA
#define FCH_TS_IRQ_UFS          0x42    // UFS
#define FCH_TS_IRQ_EMMC         0x43    // EMMC
#define FCH_TS_IRQ_GPIO         0x62    // GPIO
#define FCH_TS_IRQ_I2C0         0x70    // I2C0
#define FCH_TS_IRQ_I2C1         0x71    // I2C1
#define FCH_TS_IRQ_I2C2         0x72    // I2C2
#define FCH_TS_IRQ_I2C3         0x73    // I2C3
#define FCH_TS_IRQ_UART0        0x74    // UART0
#define FCH_TS_IRQ_UART1        0x75    // UART1
#define FCH_TS_IRQ_I2C4         0x76    // I2C4
#define FCH_TS_IRQ_I2C5         0x77    // I2C5
#define FCH_TS_IRQ_UART2        0x78    // UART2
#define FCH_TS_IRQ_UART3        0x79    // UART3

//SciMap
#define FCH_SMI_SciMap0         0x40                          // SciMap0
#define FCH_SMI_TS_GENINT1      (FCH_SMI_SciMap0 + 0)         // SciMap0
#define FCH_SMI_TS_GENINT2      (FCH_SMI_SciMap0 + 1)         // SciMap1
#define FCH_SMI_TS_AGPIO3       (FCH_SMI_SciMap0 + 2)         // SciMap2
#define FCH_SMI_TS_LpcPme       (FCH_SMI_SciMap0 + 3)         // SciMap3
#define FCH_SMI_TS_AGPIO4       (FCH_SMI_SciMap0 + 4)         // SciMap4
#define FCH_SMI_TS_LpcPd        (FCH_SMI_SciMap0 + 5)         // SciMap5
#define FCH_SMI_TS_SPKR         (FCH_SMI_SciMap0 + 6)         // SciMap6
#define FCH_SMI_TS_AGPIO5       (FCH_SMI_SciMap0 + 7)         // SciMap7
#define FCH_SMI_TS_WAKE         (FCH_SMI_SciMap0 + 8)         // SciMap8
#define FCH_SMI_TS_LpcSmi       (FCH_SMI_SciMap0 + 9)         // SciMap9
#define FCH_SMI_TS_AGPIO6       (FCH_SMI_SciMap0 + 10)        // SciMap10
#define FCH_SMI_TS_AGPIO76      (FCH_SMI_SciMap0 + 11)        // SciMap11
#define FCH_SMI_TS_UsbOc0       (FCH_SMI_SciMap0 + 12)        // SciMap12
#define FCH_SMI_TS_UsbOc1       (FCH_SMI_SciMap0 + 13)        // SciMap13
#define FCH_SMI_TS_UsbOc2       (FCH_SMI_SciMap0 + 14)        // SciMap14
#define FCH_SMI_TS_UsbOc3       (FCH_SMI_SciMap0 + 15)        // SciMap15
#define FCH_SMI_TS_AGPIO23      (FCH_SMI_SciMap0 + 16)        // SciMap16
#define FCH_SMI_TS_EspiReset    (FCH_SMI_SciMap0 + 17)        // SciMap17
#define FCH_SMI_TS_FanIn0       (FCH_SMI_SciMap0 + 18)        // SciMap18
#define FCH_SMI_TS_SysReset     (FCH_SMI_SciMap0 + 19)        // SciMap19
#define FCH_SMI_TS_AGPIO40      (FCH_SMI_SciMap0 + 20)        // SciMap20
#define FCH_SMI_TS_PwrBtn       (FCH_SMI_SciMap0 + 21)        // SciMap21
#define FCH_SMI_TS_AGPIO9       (FCH_SMI_SciMap0 + 22)        // SciMap22
#define FCH_SMI_TS_AGPIO8       (FCH_SMI_SciMap0 + 23)        // SciMap23
#define FCH_SMI_TS_eSPI         (FCH_SMI_SciMap0 + 26)        // SciMap26
#define FCH_SMI_TS_EspiWakePme  (FCH_SMI_SciMap0 + 27)        // SciMap27
#define FCH_SMI_TS_NbGppPme     (FCH_SMI_SciMap0 + 29)        // SciMap29
#define FCH_SMI_TS_NbGppHp      (FCH_SMI_SciMap0 + 30)        // SciMap30
#define FCH_SMI_TS_FakeSts0     (FCH_SMI_SciMap0 + 33)        // SciMap33
#define FCH_SMI_TS_FakeSts1     (FCH_SMI_SciMap0 + 34)        // SciMap34
#define FCH_SMI_TS_FakeSts2     (FCH_SMI_SciMap0 + 35)        // SciMap35
#define FCH_SMI_TS_Sata0Pme     (FCH_SMI_SciMap0 + 37)        // SciMap37
#define FCH_SMI_TS_Sata1Pme     (FCH_SMI_SciMap0 + 38)        // SciMap38
#define FCH_SMI_TS_AzaliaPme    (FCH_SMI_SciMap0 + 39)        // SciMap39
#define FCH_SMI_TS_GpioInt      (FCH_SMI_SciMap0 + 41)        // SciMap41
#define FCH_SMI_TS_AltHPET      (FCH_SMI_SciMap0 + 43)        // SciMap43
#define FCH_SMI_TS_FanThGevent  (FCH_SMI_SciMap0 + 44)        // SciMap44
#define FCH_SMI_TS_ASF          (FCH_SMI_SciMap0 + 45)        // SciMap45
#define FCH_SMI_TS_I2sWake      (FCH_SMI_SciMap0 + 46)        // SciMap46
#define FCH_SMI_TS_Smbus0       (FCH_SMI_SciMap0 + 47)        // SciMap47
#define FCH_SMI_TS_TWRN         (FCH_SMI_SciMap0 + 48)        // SciMap48
#define FCH_SMI_TS_TrafficMon   (FCH_SMI_SciMap0 + 49)        // SciMap49
#define FCH_SMI_TS_PwrBtnEvt    (FCH_SMI_SciMap0 + 51)        // SciMap51
#define FCH_SMI_TS_PROHOT       (FCH_SMI_SciMap0 + 52)        // SciMap52
#define FCH_SMI_TS_ApuHw        (FCH_SMI_SciMap0 + 53)        // SciMap53
#define FCH_SMI_TS_ApuSci       (FCH_SMI_SciMap0 + 54)        // SciMap54
#define FCH_SMI_TS_RasEvt       (FCH_SMI_SciMap0 + 55)        // SciMap55
#define FCH_SMI_TS_XhcWake      (FCH_SMI_SciMap0 + 57)        // SciMap57
#define FCH_SMI_TS_AcDcTimer    (FCH_SMI_SciMap0 + 58)        // SciMap58


// HW ACPI
#define TAISHAN_EVENT_HWACPI_PROG_ACPI_PMTBL              0x02010200ul   // FCH program ACPI PM Controller Base Address.
#define TAISHAN_EVENT_HWACPI_PROG_ACPI_MMIO_IN_RESET      0x02010201ul   // FCH program ACPIMMIO registers in RESET.
#define TAISHAN_EVENT_HWACPI_PROG_SCI_MAP                 0x02010202ul   // FCH program SCI map by OEM table (ACPIMMIO 0xFED803xx).
#define TAISHAN_EVENT_HWACPI_PROG_GPIO_MAP                0x02010203ul   // FCH program GPIO setting by OEM table (ACPIMMIO 0xFED801xx).
#define TAISHAN_EVENT_HWACPI_PROG_SATA_PHY                0x02010204ul   // FCH program SATA PHY by OEM table.
#define TAISHAN_EVENT_RTC_DAYLIGHT_SAVING_TIME            0x02010205ul   // FCH RTC DayLight Saving Workaround.
#define TAISHAN_EVENT_RTC_CLEAR_BANK_SELECTION            0x02010206ul   // FCH RTC clear BankSelection 0x0A Bit4 prevent error.
#define TAISHAN_EVENT_RTC_WA                              0x02010207ul   // FCH RTC workaround is set.
#define TAISHAN_EVENT_KBD_CONNECTED_THRU_USB              0x02010208ul   // FCH IRQ1/IRQ12 connected to USB controller.
#define TAISHAN_EVENT_LEGACY_FREE_SET                     0x02010209ul   // FCH None Legacy IO setting.
#define TAISHAN_EVENT_ENABLED_ASF_REMOTE_CTRL             0x0201020Aul   // FCH Enable ASF remote control function.
#define TAISHAN_EVENT_PCIEXP_WAKESTATUS_WA                0x0201020Bul   // FCH PciExp Wake Status Workaround.
#define TAISHAN_EVENT_HWACPI_PROG_OEM_MMIOTBL_IN_RESET    0x0201020Cul   // FCH program OEM/Platform BIOS ACPIMMIO registers.
#define TAISHAN_EVENT_HWACPI_PROG_MMIOTBL                 0x02010200ul   // FCH program ACPIMMIO registers. (1st.)
#define TAISHAN_EVENT_HWACPI_NO_SIOKBC_ENABLE             0x02010201ul   // FCH No SIO/KBC function is enabled.
#define TAISHAN_EVENT_HWACPI_NO_SIOKBC_DISABLE            0x02010202ul   // FCH No SIO/KBC function is disabled.
#define TAISHAN_EVENT_HWACPI_PROG_SPECIFIC_MMIOTBL        0x02010203ul   // FCH program Specific ACPIMMIO registers. (2nd.)
#define TAISHAN_EVENT_HWACPI_PROG_OEM_MMIOTBL             0x02010204ul   // FCH program OEM/Platform BIOS ACPIMMIO registers (3rd.)
#define TAISHAN_EVENT_HWACPI_SPREAD_SPECTRUM              0x02010205ul   // FCH Enable Spread Spectrum function.
#define TAISHAN_EVENT_HWACPI_INT_CLK_SET                  0x02010206ul   // FCH Set Intermal Clock Display as 48Mhz.
#define TAISHAN_EVENT_HPET_TIMER_TICK_INTERVAL_WA_SET     0x02010207ul   // FCH HPET timer tick interval workaround is set.
#define TAISHAN_EVENT_C1E_ENABLE                          0x02010208ul   // FCH C1e Enabled.
#define TAISHAN_EVENT_NATIVEPCIE_MODE_SET                 0x02010208ul   // FCH Native PCIe mode is set.
#define TAISHAN_ERROR_HPET_TBL_NOT_FOUND                  0x02030200ul   // FCH HPET Table does not found.
// HWM
#define TAISHAN_EVENT_IMC_DISABLE                         0x02010300ul   // FCH HWM/IMC is disabled.
#define TAISHAN_EVENT_IMC_ENABLE                          0x02010301ul   // FCH HWM/IMC is enabled.
// IMC
#define TAISHAN_EVENT_IMC_SW_TOGGLE_EVENT                 0x02010302ul   // FCH IMC Software Toggle Strapping. (IMC statement change)
#define TAISHAN_EVENT_EC_ENABLE                           0x02020303ul   // FCH EC enabled.
#define TAISHAN_EVENT_EC_KBD_ENABLE                       0x02010304ul   // FCH IMC EC KBD enabled.
#define TAISHAN_EVENT_EC_CHANNEL0_ENABLE                  0x02010305ul   // FCH IMC EC channel0 function enabled.
#define TAISHAN_EVENT_IMC_INTO_SLEEP_MODE                 0x02010306ul   // FCH IMC goes into sleep mode.
#define TAISHAN_EVENT_IMC_CRASH_RESET                     0x02010307ul   // FCH IMC Crash Reset to prevent repeating the reset forever.
#define TAISHAN_EVENT_IMC_SW_DISABLE_IMC                  0x02010308ul   // FCH Software disable IMC thru mailbox command.
#define TAISHAN_EVENT_IMC_DISABLE_SUREBOOT_TIMMER         0x02010309ul   // FCH Disable SureBoot Timmer.
#define TAISHAN_EVENT_IMC_WAKEUP                          0x0201030Aul   // FCH IMC wakeup command.
#define TAISHAN_EVENT_IMC_IDLE                            0x0201030Bul   // FCH IMC idle command.
#define TAISHAN_EVENT_EC_DISABLE                          0x0201030Cul   // FCH EC disable by jumper setting or board config.
#define TAISHAN_ERROR_IMC_FW_VALIDATE_FAILED              0x02030300ul   // FCH IMC firmware validation failure.
// PCIE/AB
#define TAISHAN_EVENT_AB_SLOW_SPEED_ABLINK_CLOCK          0x02010400ul   // FCH Set Low Speed AB link clock.
#define TAISHAN_EVENT_AB_RESET_CPU_ON_SYNC_FLOOD          0x02010401ul   // FCH set AB reset CPU on sync flood enabled.
#define TAISHAN_EVENT_AB_CLOCK_GATING_ENABLE              0x02010402ul   // FCH set AB Clock Gating function enabled.
#define TAISHAN_EVENT_AB_CLOCK_GATING_DISABLE             0x02010403ul   // FCH set AB clock Gating function disabled.
#define TAISHAN_EVENT_AB_DMA_MEMORY_W3264B_ENABLE         0x02010404ul   // FCH set AB DMA MEMORY Write 32/64B enabled.
#define TAISHAN_EVENT_AB_DMA_MEMORY_W3264B_DISABLED       0x02010405ul   // FCH set AB DMA MEMORY Write 32/64B disabled.
#define TAISHAN_EVENT_AB_MEMORY_POWERSAVING_ENABLED       0x02010406ul   // FCH Set AB Memory Power Saving enabled.
#define TAISHAN_EVENT_AB_MEMORY_POWERSAVING_DISABLED      0x02010407ul   // FCH Set AB Memory Power Saving disabled.
#define TAISHAN_EVENT_AB_ALINK_CLK_GATEOFF_ENABLED        0x02010408ul   // FHC set ALink clock Gate-off enabled.
#define TAISHAN_EVENT_AB_ALINK_CLK_GATEOFF_DISABLED       0x02010409ul   // FHC set ALink clock Gate-off disabled.
#define TAISHAN_EVENT_AB_BLINK_CLK_GATEOFF_ENABLED        0x0201040Aul   // FHC set BLink clock Gate-off enabled.
#define TAISHAN_EVENT_AB_BLINK_CLK_GATEOFF_DISABLED       0x0201040Bul   // FHC set BLink clock Gate-off disabled.
// SATA
#define TAISHAN_EVENT_SATA_ENABLE                         0x02010500ul   // FCH set SATA controller enabled.
#define TAISHAN_EVENT_SATA_MAX_GEN2_MODE_ENABLE           0x02010501ul   // FCH set SATA support only Max. GEN2 mode.
#define TAISHAN_EVENT_SATA_SET_CLK_SOURCE_ORG_EXT         0x02010502ul   // FCH set SATA clock source as external 48Mhz.
#define TAISHAN_EVENT_SATA_SET_CLK_SOURCE_ORG_INT         0x02010502ul   // FCH set SATA clock source as internal 48Mhz.
#define TAISHAN_EVENT_SATA_SET_CLK_SOURCE_100_INT         0x02010503ul   // FCH set SATA clock source as internal 100Mhz.
#define TAISHAN_EVENT_SATA_STABLE_MEM_SHUTDOWN_ENABLE     0x02010504ul   // FCH set SATA stable memory sutdown enable * Misc.
#define TAISHAN_EVENT_SATA_PROG_SATA_PORT_PHY             0x02010505ul   // FCH programming SATA port's PHY.
#define TAISHAN_EVENT_SATA_READ_SQUELCH_FROM_EFUSE        0x02010506ul   // FCH programming SATA squelch value from eFuse.
#define TAISHAN_EVENT_SATA_DISABLE                        0x02010507ul   // FCH set SATA controller disabled.
#define TAISHAN_EVENT_SATA_AHCI_MODE                      0x02010508ul   // FCH set SATA as AHCI mode.
#define TAISHAN_EVENT_SATA_IDE_2_AHCI_MODE                0x02010509ul   // FCH set SATA as IDE_2_AHCI mode.
#define TAISHAN_EVENT_SATA_IDE_MODE                       0x0201050Aul   // FCH set SATA as IDE mode.
#define TAISHAN_EVENT_SATA_RAID_MODE                      0x0201050Bul   // FCH set SATA as RAID mode.
#define TAISHAN_EVENT_SATA_MSI_CAP_ENABLE                 0x0201050Cul   // FCH set SATA MSI Capability Enabled.
#define TAISHAN_EVENT_SATA_SUPPORT_8_DEVICE               0x0201050Dul   // FCH set SATA support 8 device mode.
#define TAISHAN_EVENT_SATA_DISABLED_GENERIC_MODE          0x0201050Eul   // FCH set SATA disable generic mode.
#define TAISHAN_EVENT_SATA_PHY_PLL_SHUTDOWN               0x0201050Ful   // FCH set SATA PHY PLL shutdown.
#define TAISHAN_EVENT_SATA_OOB_DETECTION_ENH              0x02010510ul   // FCH set SATA OOB Detection Enhance Mode.
#define TAISHAN_EVENT_SATA_MEM_POWER_SAVING               0x02010511ul   // FCH set SATA memory power saving.
#define TAISHAN_EVENT_SATA_DEV_SLP_PORT0                  0x02010512ul   // FCH set SATA Deep Sleep Mode on Port0.
#define TAISHAN_EVENT_SATA_DEV_SLP_PORT1                  0x02010513ul   // FCH set SATA Deep Sleep Mode on Port1.
#define TAISHAN_EVENT_SATA_AHCI_DIS_PREFETCH              0x02010514ul   // FCH set SATA AHCI disable Prefetch.
#define TAISHAN_EVENT_SATA_PORT_MULT_CAP                  0x02010515ul   // FCH set SATA Port Mult Capability Enabled.
#define TAISHAN_EVENT_SATA_FIS_BASE_SWITCHING             0x02010516ul   // FCH set SATA support FIS-based switching.
#define TAISHAN_EVENT_SATA_AGGR_LINK_PM_CAP               0x02010517ul   // FCH set SATA aggressive link power management.
#define TAISHAN_EVENT_SATA_PSC_CAP                        0x02010518ul   // FCH set SATA support partial state.
#define TAISHAN_EVENT_SATA_SSC_CAP                        0x02010519ul   // FCH set SATA support Slumber mode.
#define TAISHAN_EVENT_SATA_CCC_CAP                        0x0201051Aul   // FCH set SATA support command completion coalescing.
#define TAISHAN_EVENT_SATA_AHCI_ENCLOSURE_MANAGEMENT      0x0201051Bul   // FCH set SATA support Enclosure Management.
#define TAISHAN_EVENT_SATA_ESP_PORT_ENABLE                0x0201051Cul   // FCH set SATA ESP ports (one of ESP ports are set).
#define TAISHAN_EVENT_SATA_BIOS_OS_HANDOFF                0x0201051Dul   // FCH set SATA HBA supports the BIOS/OS handoff mechanism.
#define TAISHAN_EVENT_SATA_DRIVE_DETECTION                0x0201051Eul   // FCH SATA is excuting SATA drive detection.
#define TAISHAN_EVENT_SATA_CLK_AUTO_OFF                   0x0201051Ful   // FCH set SATA Auto Clock off function.
#define TAISHAN_EVENT_SATA_PORT_GEN_MODE                  0x02010520ul   // FCH set SATA port GEN mode.
#define TAISHAN_EVENT_SATA_HOT_REMOVAL_ENH                0x02010521ul   // FCH set SATA hot removal enhance mode.
// SD
#define TAISHAN_EVENT_SD_ENABLE                           0x02010600ul   // FCH set SD controller to enable.
#define TAISHAN_EVENT_SD_AS_DMA_MODE                      0x02010601ul   // FCH set SD as DMA mode.
#define TAISHAN_EVENT_SD_AS_PIO_MODE                      0x02010602ul   // FCH set SD as PIO mode.
#define TAISHAN_EVENT_SD_AS_2_0_MODE                      0x02010603ul   // FCH set SD as 2.0 mode.
#define TAISHAN_EVENT_SD_AS_3_0_MODE                      0x02010604ul   // FCH set SD as 3.0 mode.
#define TAISHAN_EVENT_SD_CLOCK_MULTIPLIER                 0x02010605ul   // FCH set SD clock multiplier.
#define TAISHAN_EVENT_SD_DISABLE                          0x02010606ul   // FCH set SD controller to disable.
// LPC/SPI
#define TAISHAN_EVENT_LPC_CLK0_DISABLE                    0x02010700ul   // FCH set LPC0 clock disabled.
#define TAISHAN_EVENT_LPC_CLK1_DISABLE                    0x02010701ul   // FCH set LPC1 clock disabled.
#define TAISHAN_EVENT_LPC_LEGACY_FREE_MODE                0x02010702ul   // FCH set LPC as legacy free mode.
#define TAISHAN_EVENT_SPI_QUAL_MODE                       0x02010703ul   // FCH SPI Qual Mode is enabled (by user selection).
#define TAISHAN_EVENT_SPI_SPEED                           0x02010704ul   // FCH SPI speed is set by user selection.
#define TAISHAN_EVENT_SPI_FAST_SPEED                      0x02010705ul   // FCH SPI fast speed is set.
#define TAISHAN_EVENT_SPI_QUALIFY_QUAL_MODE               0x02010706ul   // FCH SPI Qual Mode is validated.
// USB
#define TAISHAN_ERROR_USB_ROMSIG_NOT_FOUND                0x02030800ul   // FCH ROMSIG not found.
#define TAISHAN_ERROR_USB_XHCI_FW_FOUND                   0x02030801ul   // FCH XHCI firmware does not exist.
#define TAISHAN_EVENT_USB_XHCI_DISABLE                    0x02010803ul   // FCH set XHCI disable.
#define TAISHAN_EVENT_USB_PHY_POWER_DOWN                  0x02010804ul   // FCH set USB PHY powerdown enable.
#define TAISHAN_EVENT_USB_PHY_CALIBRATED                  0x02010805ul   // FCH set USB PHY calibrated.
#define TAISHAN_EVENT_USB_PORT_PHY_SETTING                0x02010806ul   // FCH set USB Port PHY setting.
#define TAISHAN_EVENT_USB_PORT_PHY_CLK_GATING             0x02010807ul   // FCH set USB Port PHY Clock Gating.
#define TAISHAN_EVENT_USB_XHCI_USED_PREDEFINE_ADDRESS     0x02010808ul   // FCH XHCI ROM location is used user-define address.
#define TAISHAN_EVENT_USB_XHCI_BOOTRAM_PRELOAD            0x02010809ul   // FCH XHCI preload its BOOTRAM.
#define TAISHAN_EVENT_USB_XHCI_INSTRUCTRAM_PRELOAD        0x0201080Aul   // FCH XHCI preload its InstructionRAM.
#define TAISHAN_EVENT_USB_XHCI_ROM_PREINIT_COMPLETED      0x0201080Bul   // FCH XHCI preinit completed.
#define TAISHAN_EVENT_USB_XHCI0_ENABLE                    0x0201080Cul   // FCH set XHCI0 enable.
#define TAISHAN_EVENT_USB_XHCI1_ENABLE                    0x0201080Dul   // FCH set XHCI1 enable.
