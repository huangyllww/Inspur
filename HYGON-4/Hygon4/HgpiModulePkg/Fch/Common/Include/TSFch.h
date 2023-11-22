/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH registers definition
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: FCH
 *
 */
/*
*****************************************************************************
*
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 *
****************************************************************************
*/

#ifndef _TSFCH_H_
#define _TSFCH_H_

#define TAISHAN_SD_BUS_DEV_FUN  ((0x0B << 3) + 6)                   // SD Controller
#define TAISHAN_SD_PCI_DEV      11
#define TAISHAN_SD_PCI_FUNC     6

#define TAISHAN_BUSAPORT_BUS_DEV_FUN  ((0x8 << 3) + 1)
#define TAISHAN_BUSCPORT_BUS_DEV_FUN  ((0x8 << 3) + 1)
#define TAISHAN_BUSDPORT_BUS_DEV_FUN  ((0x9 << 3) + 1)
#define TAISHAN_SATA_DEV_FUN          ((0x0 << 3) + 2)
#define TAISHAN_SATA_DEV              0
#define TAISHAN_SATA_FUN              2
#define TAISHAN_SATA_PORT_NUM         8

#define TAISHAN_FCH_SATA_DID            0x7901
#define TAISHAN_FCH_SATA_AHCI_DID       0x7901
#define TAISHAN_FCH_SATA_HYGONAHCI_DID  0x7904
#define TAISHAN_FCH_SATA_RAID_DID       0x7916

#define TAISHAN_FCH_SMBUS_DID  0x790B         // Dev 0x0F Func 0
#define TAISHAN_FCH_LPC_DID    0x790E         // Dev 0x0F Func 3
#define TAISHAN_FCH_SD_DID     0x7906         // Dev 0x0F Func 6

#define HYGON_D0F0   ((0 << 3) + 0)
#define HYGON_D18F0  ((0x18 << 3) + 0)

#define PGFSM_CMD_POWER_DOWN    (0x00 << 1)
#define PGFSM_CMD_POWER_UP      (0x01 << 1)
#define PGFSM_CMD_POWER_WRITE   (0x02 << 1)
#define PGFSM_CMD_POWER_READ    (0x03 << 1)
#define PGFSM_SOFTWARE_CONTROL  BIT0

#define PGFSM_STATUS_PWR_STS  (BIT0 + BIT1)
#define PGFSM_STATUS_PWR_ON   0
#define PGFSM_STATUS_PWR_OFF  BIT1

#define PGFSM_SELECT_0  BIT8

// USB RSMU registers
// USB0 RSMU base 0x4D520
// USB1 RSMU base 0x4D5A0
#define FCH_TS_USB_RSMU_PGFSM_CNTL     0x0004D520ul
#define FCH_TS_USB_RSMU_PGFSM_WR_DATA  0x0004D524ul
#define FCH_TS_USB_RSMU_PGFSM_RD_DATA  0x0004D528ul

#define USB_RSMU_SPACE_HYEX(RbNum, Address)       (Address + (RbNum << 7))
#define USB_SPACE_HYEX(RbNum, Address)            (Address + (RbNum << 21))

// 4A usb0.UsbCfg	AppertureId 0x16C
#define USB0CFG_APPERTURE_ID_HYEX  0x16C
#define USB0PHY_APPERTURE_ID_HYEX  0x16D
#define USB_PORT_NUMBER_PER_NBIO_OF_SATORI 4
#define USB_NUMBER_PER_SATORI              2

// 4B usb0.UsbCfg	AppertureId 0x0A4
// 4B usb1.UsbCfg	AppertureId 0x0AA
#define USB0CFG_APPERTURE_ID_HYGX  0x0A4
#define USB0PHY_APPERTURE_ID_HYGX  0x0A5
#define USB1CFG_APPERTURE_ID_HYGX  0x0AA
#define USB1PHY_APPERTURE_ID_HYGX  0x0AB


#define USB_NUMBER_PER_DUJIANG 2
#define USB_PORT_NUMBER_PER_CONTROLLER 4
#define USB_PORT_NUMBER_PER_DJ  (USB_NUMBER_PER_DUJIANG * USB_PORT_NUMBER_PER_CONTROLLER)
#define USB_SPACE_HYGX(PhysicalDieId, UsbIndex, Address)  (UINT32)(0x10000000 + (PhysicalDieId << 28) + (0x600000 * UsbIndex) + Address)

#define USB_RSMU_SPACE_HYGX(UsbIndex, Address)       (Address + (UsbIndex << 7))

// USB3 IP registers
#define GSNPSID_REVISION_290A     0x5533290Aul
#define GSNPSID_REVISION_320A     0x5533320Aul
// HyEx
#define FCH_TS_USB_GUCTL1_HYEX         ((USB0CFG_APPERTURE_ID_HYEX << 20) + 0x0C11C)  // 0x16C0C11C
#define FCH_TS_USB_GSNPSID_HYEX        ((USB0CFG_APPERTURE_ID_HYEX << 20) + 0x0C120)  // 0x16C0C120
#define FCH_TS_USB_GUCTL_HYEX          ((USB0CFG_APPERTURE_ID_HYEX << 20) + 0x0C12C)  // 0x16C0C12C
#define FCH_TS_USB_GUCTL2_HYEX         ((USB0CFG_APPERTURE_ID_HYEX << 20) + 0x0C608)  // 0x16C0C608
#define FCH_TS_USB_GUSB3PIPECTL0_HYEX  ((USB0CFG_APPERTURE_ID_HYEX << 20) + 0x0C2C0)  // 0x16C0C2C0
#define FCH_TS_USB_GUSB3PIPECTL1_HYEX  ((USB0CFG_APPERTURE_ID_HYEX << 20) + 0x0C2C4)  // 0x16C0C2C4
#define FCH_TS_USB_GUSB3PIPECTL2_HYEX  ((USB0CFG_APPERTURE_ID_HYEX << 20) + 0x0C2C8)  // 0x16C0C2C8
#define FCH_TS_USB_GUSB3PIPECTL3_HYEX  ((USB0CFG_APPERTURE_ID_HYEX << 20) + 0x0C2CC)  // 0x16C0C2CC
#define FCH_TS_USB_GUSB2PHYCFG0_HYEX   ((USB0CFG_APPERTURE_ID_HYEX << 20) + 0x0C200)  // 0x16C0C200
#define FCH_TS_USB_GUSB2PHYCFG1_HYEX   ((USB0CFG_APPERTURE_ID_HYEX << 20) + 0x0C204)  // 0x16C0C204
#define FCH_TS_USB_GUSB2PHYCFG2_HYEX   ((USB0CFG_APPERTURE_ID_HYEX << 20) + 0x0C208)  // 0x16C0C208
#define FCH_TS_USB_GUSB2PHYCFG3_HYEX   ((USB0CFG_APPERTURE_ID_HYEX << 20) + 0x0C20C)  // 0x16C0C20C
#define FCH_TS_USB3_PORTSC_HYEX        ((USB0CFG_APPERTURE_ID_HYEX << 20) + 0x00460)  // 0x16C00460

// HyGx
#define FCH_TS_USB_GUCTL1_HYGX         ((USB0CFG_APPERTURE_ID_HYGX << 20) + 0x0C11C)  // 0x0A40C11C
#define FCH_TS_USB_GSNPSID_HYGX        ((USB0CFG_APPERTURE_ID_HYGX << 20) + 0x0C120)  // 0x0A40C120
#define FCH_TS_USB_GUCTL_HYGX          ((USB0CFG_APPERTURE_ID_HYGX << 20) + 0x0C12C)  // 0x0A40C12C
#define FCH_TS_USB_GUCTL2_HYGX         ((USB0CFG_APPERTURE_ID_HYGX << 20) + 0x0C608)  // 0x0A40C608
#define FCH_TS_USB_GUSB3PIPECTL0_HYGX  ((USB0CFG_APPERTURE_ID_HYGX << 20) + 0x0C2C0)  // 0x0A40C2C0
#define FCH_TS_USB_GUSB3PIPECTL1_HYGX  ((USB0CFG_APPERTURE_ID_HYGX << 20) + 0x0C2C4)  // 0x0A40C2C4
#define FCH_TS_USB_GUSB3PIPECTL2_HYGX  ((USB0CFG_APPERTURE_ID_HYGX << 20) + 0x0C2C8)  // 0x0A40C2C8
#define FCH_TS_USB_GUSB3PIPECTL3_HYGX  ((USB0CFG_APPERTURE_ID_HYGX << 20) + 0x0C2CC)  // 0x0A40C2CC
#define GUCTL2_ENABLE_EP_CACHE_EVICT         BIT12

// USB3 CONTAINER Registers
#define FCH_TS_USB_S5_RESET                    (1 << 0x00)
#define FCH_TS_USB_CONTROLLER_RESET            (1 << 0x01)
#define FCH_TS_USB_PHY_RESET                   (1 << 0x02)
#define FCH_TS_USB_PMCSR_PME_EN                (1 << 8)

// HyEx
#define FCH_SMN_USB_CONTAINER_HYEX                  ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x80000)  //0x16D80000ul
#define FCH_TS_USB_SHADOWED_PMCSR_HYEX              (FCH_SMN_USB_CONTAINER_HYEX + 0x10)       // 0x10
#define FCH_TS_USB_SHADOWED_POWERSTATE_HYEX         (FCH_SMN_USB_CONTAINER_HYEX + 0x14)       // 0x14
#define FCH_TS_USB_SMU_ASSIST_POWER_STATE_CTL_HYEX  (FCH_SMN_USB_CONTAINER_HYEX + 0x1C)       // 0x1C
#define FCH_TS_USB_RESET_HYEX                       (FCH_SMN_USB_CONTAINER_HYEX + 0x100)      // 0x100
#define FCH_TS_USB_PGFSM_OVERRIDE_HYEX              (FCH_SMN_USB_CONTAINER_HYEX + 0x104)      // 0x104
#define FCH_TS_USB_CURRENT_PWR_STS_HYEX             (FCH_SMN_USB_CONTAINER_HYEX + 0x108)      // 0x108
#define FCH_TS_USB_INTERRUPT_CONTROL_HYEX           (FCH_SMN_USB_CONTAINER_HYEX + 0x118)      // 0x118
#define FCH_TS_USB_PORT_OCMAPPING0_HYEX             (FCH_SMN_USB_CONTAINER_HYEX + 0x120)      // 0x120
#define FCH_TS_USB_PORT_DISABLE0_HYEX               (FCH_SMN_USB_CONTAINER_HYEX + 0x128)      // 0x128
#define FCH_TS_USB_PORT_CONTROL_HYEX                (FCH_SMN_USB_CONTAINER_HYEX + 0x130)      // 0x130
#define FCH_TS_USB_FORCE_GEN1_HYEX                  (FCH_SMN_USB_CONTAINER_HYEX + 0x234)
#define FCH_TS_USB_PHY_CR_SELECT_HYEX               (FCH_SMN_USB_CONTAINER_HYEX + 0x268)

// HyGx
#define FCH_SMN_USB_CONTAINER_HYGX                  ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x80000)  //0x0A580000ul
#define FCH_TS_USB_SHADOWED_PMCSR_HYGX              (FCH_SMN_USB_CONTAINER_HYGX + 0x10)       // 0x10
#define FCH_TS_USB_SHADOWED_POWERSTATE_HYGX         (FCH_SMN_USB_CONTAINER_HYGX + 0x14)       // 0x14
#define FCH_TS_USB_SMU_ASSIST_POWER_STATE_CTL_HYGX  (FCH_SMN_USB_CONTAINER_HYGX + 0x1C)       // 0x1C
#define FCH_TS_USB_RESET_HYGX                       (FCH_SMN_USB_CONTAINER_HYGX + 0x100)      // 0x100

#define FCH_TS_USB_PGFSM_OVERRIDE_HYGX              (FCH_SMN_USB_CONTAINER_HYGX + 0x104)      // 0x104
#define FCH_TS_USB_CURRENT_PWR_STS_HYGX             (FCH_SMN_USB_CONTAINER_HYGX + 0x108)      // 0x108
#define FCH_TS_USB_INTERRUPT_CONTROL_HYGX           (FCH_SMN_USB_CONTAINER_HYGX + 0x118)      // 0x118
#define FCH_TS_USB_PORT_OCMAPPING0_HYGX             (FCH_SMN_USB_CONTAINER_HYGX + 0x120)      // 0x120
#define FCH_TS_USB_PORT_DISABLE0_HYGX               (FCH_SMN_USB_CONTAINER_HYGX + 0x128)      // 0x128
#define FCH_TS_USB_PORT_CONTROL_HYGX                (FCH_SMN_USB_CONTAINER_HYGX + 0x130)      // 0x130
#define FCH_TS_USB_PHY_CR_SELECT_HYGX               (FCH_SMN_USB_CONTAINER_HYGX + 0x268)


#define RX_EQ_OVRD                               BIT11
#define RX_EQ                                    (BIT8 + BIT9 + BIT10)
#define RX_EQ_EN_OVRD                            BIT7
#define RX_EQ_EN                                 BIT6 
// HyEx
#define FCH_TS_USB_PHY_PCS_RAW_RAM_HYEX               ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x70000)  //0x16D70000ul

#define FCH_TS_USB_PHY0_RX_OVRD_IN_HI_HYEX            ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x4018)    // 0x4018
#define FCH_TS_USB_PHY1_RX_OVRD_IN_HI_HYEX            ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x4418)    // 0x4418
#define FCH_TS_USB_PHY2_RX_OVRD_IN_HI_HYEX            ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x4818)    // 0x4818
#define FCH_TS_USB_PHY3_RX_OVRD_IN_HI_HYEX            ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x4C18)    // 0x4C18
#define FCH_TS_USB_PHY0_PHYINTERNALLANEREGISTER_HYEX  ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x4098)    // 0x4098
#define FCH_TS_USB_PHY1_PHYINTERNALLANEREGISTER_HYEX  ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x4498)    // 0x4498
#define FCH_TS_USB_PHY2_PHYINTERNALLANEREGISTER_HYEX  ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x4898)    // 0x4898
#define FCH_TS_USB_PHY3_PHYINTERNALLANEREGISTER_HYEX  ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x4C98)    // 0x4C98
#define FCH_TS_USB_PHY0_PHYCFG0_HYEX                  ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x4C98)    // 0x8200
#define FCH_TS_USB_PHY0_EXTIDDONE_HYEX                ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x8210)    // 0x8210
#define FCH_TS_USB_PHY0_SRAMINITDONE_HYEX             ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x8214)    // 0x8214
#define FCH_TS_USB_PHY1_PHYCFG0_HYEX                  ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x8600)    // 0x8600
#define FCH_TS_USB_PHY2_PHYCFG0_HYEX                  ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x8A00)    // 0x8A00
#define FCH_TS_USB_PHY3_PHYCFG0_HYEX                  ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x8E00)    // 0x8E00
#define FCH_TS_USB_PHY0_LANEPARACTL0_HYEX             ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0xC000)    // 0xC000
#define FCH_TS_USB_PHY1_LANEPARACTL0_HYEX             ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0xC400)    // 0xC400
#define FCH_TS_USB_PHY2_LANEPARACTL0_HYEX             ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0xC800)    // 0xC800
#define FCH_TS_USB_PHY3_LANEPARACTL0_HYEX             ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0xCC00)    // 0xCC00
#define FCH_TS_USB_PHY0PARACTL2_HYEX                  ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x8008)    // 0x8008
#define FCH_TS_USB_PHY1PARACTL2_HYEX                  ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x8408)    // 0x8408
#define FCH_TS_USB_PHY2PARACTL2_HYEX                  ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x8808)    // 0x8808
#define FCH_TS_USB_PHY3PARACTL2_HYEX                  ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x8C08)    // 0x8C08
#define FCH_TS_USB_PHY_EXT_TX_EQ0_HYEX                ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x8270)    // 0x8270
#define FCH_TS_USB_PHY_EXT_TX_EQ1_HYEX                ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x8274)    // 0x8274
#define FCH_TS_USB_SPARE2_REG_HYEX                    ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x8014C)   // 0x8014C
#define FCH_TS_USB_SUP_DIG_LVL_OVER_IN_HYEX           ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x40084)   // 0x40084

// HyGx
#define FCH_TS_USB_PHY_PCS_RAW_RAM_HYGX               ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x70000)  //0x0A570000ul
#define FCH_TS_USB_PHY0_RX_OVRD_IN_HI_HYGX            ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x4018)    // 0x4018
#define FCH_TS_USB_PHY1_RX_OVRD_IN_HI_HYGX            ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x4418)    // 0x4418
#define FCH_TS_USB_PHY2_RX_OVRD_IN_HI_HYGX            ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x4818)    // 0x4818
#define FCH_TS_USB_PHY3_RX_OVRD_IN_HI_HYGX            ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x4C18)    // 0x4C18
#define FCH_TS_USB_PHY0_PHYINTERNALLANEREGISTER_HYGX  ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x4098)    // 0x4098
#define FCH_TS_USB_PHY1_PHYINTERNALLANEREGISTER_HYGX  ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x4498)    // 0x4498
#define FCH_TS_USB_PHY2_PHYINTERNALLANEREGISTER_HYGX  ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x4898)    // 0x4898
#define FCH_TS_USB_PHY3_PHYINTERNALLANEREGISTER_HYGX  ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x4C98)    // 0x4C98
#define FCH_TS_USB_PHY0_PHYCFG0_HYGX                  ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x4C98)    // 0x8200
#define FCH_TS_USB_PHY0_EXTIDDONE_HYGX                ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x8210)    // 0x8210
#define FCH_TS_USB_PHY0_SRAMINITDONE_HYGX             ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x8214)    // 0x8214
#define FCH_TS_USB_PHY1_PHYCFG0_HYGX                  ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x8600)    // 0x8600
#define FCH_TS_USB_PHY2_PHYCFG0_HYGX                  ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x8A00)    // 0x8A00
#define FCH_TS_USB_PHY3_PHYCFG0_HYGX                  ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x8E00)    // 0x8E00
#define FCH_TS_USB_PHY0_LANEPARACTL0_HYGX             ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0xC000)    // 0xC000
#define FCH_TS_USB_PHY1_LANEPARACTL0_HYGX             ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0xC400)    // 0xC400
#define FCH_TS_USB_PHY2_LANEPARACTL0_HYGX             ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0xC800)    // 0xC800
#define FCH_TS_USB_PHY3_LANEPARACTL0_HYGX             ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0xCC00)    // 0xCC00
#define FCH_TS_USB_PHY0PARACTL2_HYGX                  ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x8008)    // 0x8008
#define FCH_TS_USB_PHY1PARACTL2_HYGX                  ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x8408)    // 0x8408
#define FCH_TS_USB_PHY2PARACTL2_HYGX                  ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x8808)    // 0x8808
#define FCH_TS_USB_PHY3PARACTL2_HYGX                  ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x8C08)    // 0x8C08
#define FCH_TS_USB_PHY_EXT_TX_EQ0_HYGX                ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x8270)    // 0x8270
#define FCH_TS_USB_PHY_EXT_TX_EQ1_HYGX                ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x8274)    // 0x8274
#define FCH_TS_USB_SPARE2_REG_HYGX                    ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x8014C)   // 0x8014C
#define FCH_TS_USB_SUP_DIG_LVL_OVER_IN_HYGX           ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x40084)   // 0x40084

#define NBIF_SMN_VWR_VCHG_DIS_CTRL_1_HYEX         0x1013a058ul

#define PHY_CR_SELECT_HYEX                 ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x80268)   // 0x80268
#define FCH_TS_USB_PHY_BASE_HYEX           ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x40000)   // 0x40000
#define FCH_TS_USB_MAP_PHY_BASE_HYEX       ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x00000)   // 0x00000
#define USBDWC_HYEX                        ((USB0CFG_APPERTURE_ID_HYEX << 20) + 0x00000)   // 0x00000

#define PHY_CR_SELECT_HYGX                 ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x80268)   // 0x80268
#define FCH_TS_USB_PHY_BASE_HYGX           ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x40000)   // 0x40000
#define FCH_TS_USB_MAP_PHY_BASE_HYGX       ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x00000)   // 0x00000
#define USBDWC_HYGX                        ((USB0CFG_APPERTURE_ID_HYGX << 20) + 0x00000)   // 0x00000

//============================================================================================================================================================================================================
//USB controller regs smn address defines
//============================================================================================================================================================================================================
// HyEx
#define   USBDWC_SMN_ADDRESS_BASE_HYEX                       USBDWC_HYEX
#define   GUSB3PIPECTL_REGS0_GUSB3PIPECTL_SMN_ADDRESS_HYEX   (USBDWC_SMN_ADDRESS_BASE_HYEX + 0xC2C0)
#define   GUSB3PIPECTL_REGS1_GUSB3PIPECTL_SMN_ADDRESS_HYEX   (USBDWC_SMN_ADDRESS_BASE_HYEX + 0xC2C4)
#define   GUSB3PIPECTL_REGS2_GUSB3PIPECTL_SMN_ADDRESS_HYEX   (USBDWC_SMN_ADDRESS_BASE_HYEX + 0xC2C8)
#define   GUSB3PIPECTL_REGS3_GUSB3PIPECTL_SMN_ADDRESS_HYEX   (USBDWC_SMN_ADDRESS_BASE_HYEX + 0xC2CC)
// HyGx
#define   USBDWC_SMN_ADDRESS_BASE_HYGX                       USBDWC_HYGX
#define   GUSB3PIPECTL_REGS0_GUSB3PIPECTL_SMN_ADDRESS_HYGX   (USBDWC_SMN_ADDRESS_BASE_HYEX + 0xC2C0)
#define   GUSB3PIPECTL_REGS1_GUSB3PIPECTL_SMN_ADDRESS_HYGX   (USBDWC_SMN_ADDRESS_BASE_HYEX + 0xC2C4)
#define   GUSB3PIPECTL_REGS2_GUSB3PIPECTL_SMN_ADDRESS_HYGX   (USBDWC_SMN_ADDRESS_BASE_HYEX + 0xC2C8)
#define   GUSB3PIPECTL_REGS3_GUSB3PIPECTL_SMN_ADDRESS_HYGX   (USBDWC_SMN_ADDRESS_BASE_HYEX + 0xC2CC)
//============================================================================================================================================================================================================
//SYSHUB::USB::PHYCTRL smn address defines
//============================================================================================================================================================================================================
// HyEx
#define   USBPHYCTRL_SMN_ADDRESS_BASE_HYEX        ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x8000)
#define   PHYPARACTL1_LINK0_SMN_ADDRESS_HYEX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYEX + 0x0004)
#define   PHYPARACTL1_LINK1_SMN_ADDRESS_HYEX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYEX + 0x0404)
#define   PHYPARACTL1_LINK2_SMN_ADDRESS_HYEX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYEX + 0x0804)
#define   PHYPARACTL1_LINK3_SMN_ADDRESS_HYEX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYEX + 0x0C04)
#define   PHYPARACTL2_LINK0_SMN_ADDRESS_HYEX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYEX + 0x0008)
#define   PHYPARACTL2_LINK1_SMN_ADDRESS_HYEX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYEX + 0x0408)
#define   PHYPARACTL2_LINK2_SMN_ADDRESS_HYEX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYEX + 0x0808)
#define   PHYPARACTL2_LINK3_SMN_ADDRESS_HYEX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYEX + 0x0C08)
// HyGx
#define   USBPHYCTRL_SMN_ADDRESS_BASE_HYGX        ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x8000)
#define   PHYPARACTL1_LINK0_SMN_ADDRESS_HYGX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYGX + 0x0004)
#define   PHYPARACTL1_LINK1_SMN_ADDRESS_HYGX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYGX + 0x0404)
#define   PHYPARACTL1_LINK2_SMN_ADDRESS_HYGX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYGX + 0x0804)
#define   PHYPARACTL1_LINK3_SMN_ADDRESS_HYGX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYGX + 0x0C04)
#define   PHYPARACTL2_LINK0_SMN_ADDRESS_HYGX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYGX + 0x0008)
#define   PHYPARACTL2_LINK1_SMN_ADDRESS_HYGX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYGX + 0x0408)
#define   PHYPARACTL2_LINK2_SMN_ADDRESS_HYGX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYGX + 0x0808)
#define   PHYPARACTL2_LINK3_SMN_ADDRESS_HYGX      (USBPHYCTRL_SMN_ADDRESS_BASE_HYGX + 0x0C08)
//============================================================================================================================================================================================================
//SYSHUB::USB::LANECTRL smn address defines
//============================================================================================================================================================================================================
// HyEx
#define   USBLANCTRL_SMN_ADDRESS_BASE_HYEX        ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0xC000)
#define   LANEPARACTL0_LINK0_SMN_ADDRESS_HYEX     (USBLANCTRL_SMN_ADDRESS_BASE_HYEX + 0x0000)
#define   LANEPARACTL0_LINK1_SMN_ADDRESS_HYEX     (USBLANCTRL_SMN_ADDRESS_BASE_HYEX + 0x0400)
#define   LANEPARACTL0_LINK2_SMN_ADDRESS_HYEX     (USBLANCTRL_SMN_ADDRESS_BASE_HYEX + 0x0800)
#define   LANEPARACTL0_LINK3_SMN_ADDRESS_HYEX     (USBLANCTRL_SMN_ADDRESS_BASE_HYEX + 0x0C00)
// HyGx
#define   USBLANCTRL_SMN_ADDRESS_BASE_HYGX        ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0xC000)
#define   LANEPARACTL0_LINK0_SMN_ADDRESS_HYGX     (USBLANCTRL_SMN_ADDRESS_BASE_HYGX + 0x0000)
#define   LANEPARACTL0_LINK1_SMN_ADDRESS_HYGX     (USBLANCTRL_SMN_ADDRESS_BASE_HYGX + 0x0400)
#define   LANEPARACTL0_LINK2_SMN_ADDRESS_HYGX     (USBLANCTRL_SMN_ADDRESS_BASE_HYGX + 0x0800)
#define   LANEPARACTL0_LINK3_SMN_ADDRESS_HYGX     (USBLANCTRL_SMN_ADDRESS_BASE_HYGX + 0x0C00)
//============================================================================================================================================================================================================
//SYSHUB::USB::PHYLANE smn address defines
//============================================================================================================================================================================================================
// HyEx
#define   USBPHYLANE_SMN_ADDRESS_BASE_HYEX        ((USB0PHY_APPERTURE_ID_HYEX << 20) + 0x4000)
#define   TX_OVRD_DRV_LO_LANE0_HYEX               (USBPHYLANE_SMN_ADDRESS_BASE_HYEX + 0x0008)
#define   TX_OVRD_DRV_LO_LANE1_HYEX               (USBPHYLANE_SMN_ADDRESS_BASE_HYEX + 0x0408)
#define   TX_OVRD_DRV_LO_LANE2_HYEX               (USBPHYLANE_SMN_ADDRESS_BASE_HYEX + 0x0808)
#define   TX_OVRD_DRV_LO_LANE3_HYEX               (USBPHYLANE_SMN_ADDRESS_BASE_HYEX + 0x0C08)
// HyGx
#define   USBPHYLANE_SMN_ADDRESS_BASE_HYGX        ((USB0PHY_APPERTURE_ID_HYGX << 20) + 0x4000)
#define   TX_OVRD_DRV_LO_LANE0_HYGX               (USBPHYLANE_SMN_ADDRESS_BASE_HYGX + 0x0008)
#define   TX_OVRD_DRV_LO_LANE1_HYGX               (USBPHYLANE_SMN_ADDRESS_BASE_HYGX + 0x0408)
#define   TX_OVRD_DRV_LO_LANE2_HYGX               (USBPHYLANE_SMN_ADDRESS_BASE_HYGX + 0x0808)
#define   TX_OVRD_DRV_LO_LANE3_HYGX               (USBPHYLANE_SMN_ADDRESS_BASE_HYGX + 0x0C08)
//============================================================================================================================================================================================================
//USB controller regs  defines
//============================================================================================================================================================================================================
typedef struct {
  UINT32    ELASTIC_BUFFER_MODE               :1;
  UINT32    TX_DE_EPPHASIS                    :2;
  UINT32    TX_MARGIN                         :3;
  UINT32    TX_SWING                          :1;
  UINT32    SSICEn                            :1;
  UINT32    RX_DETECT_to_Polling_LFPS_Control :1;
  UINT32    LFPSFILTER                        :1;
  UINT32    P3ExSigP2                         :1;
  UINT32    P3P2TranOK                        :1;
  UINT32    LFPSP0Algn                        :1;
  UINT32    SkipRxDet                         :1;
  UINT32    AbortRxDetInU2                    :1;
  UINT32    DATWIDTH                          :2;
  UINT32    SUSPENDENABLE                     :1;
  UINT32    DELAYP1TRANS                      :1;
  UINT32    DelayP1P2P3                       :3;
  UINT32    DisRxDetU3RxDet                   :1;
  UINT32    StartRxDetU3RxDet                 :1;
  UINT32    request_p1p2p3                    :1;
  UINT32    u1u2exitfail_to_recov             :1;
  UINT32    ping_enhancement_en               :1;
  UINT32    Ux_exit_in_Px                     :1;
  UINT32    DisRxDetP3                        :1;
  UINT32    U2SSInactP3ok                     :1;
  UINT32    HstPrtCmpl                        :1;
  UINT32    PHYSoftRst                        :1;
} GUSB3PIPECTL_REG;

typedef union {
  GUSB3PIPECTL_REG   Reg;
  UINT32             Data32;
} GUSB3PIPECTL;

//============================================================================================================================================================================================================
//SYSHUB::USB::PHYCTRL register defines
//============================================================================================================================================================================================================
typedef struct {
  UINT32  los_level            :5;   //4:0 los_level. Read-write. Reset: 09h. Loss-of-Signal Detector Sensitivity Level Control.
  UINT32  los_bias             :3;   //7:5 los_bias. Read-write. Reset: 5h. Loss-of-Signal Detector Threshold Level Control.
  UINT32  pcs_rx_los_mask_val  :10;  //17:8 pcs_rx_los_mask_val. Read-write. Reset: 3E8h. Configurable Loss-of-Signal Mask Width. SNPS required it is 10'h0F0 for 10us@24M reference clock. 10'h3e8 is fine for us since our suspend_clk is 187.5KHz.
  UINT32  pcs_tx_deemph_3p5db  :6;   //23:18 pcs_tx_deemph_3p5db. Read-write. Reset: 1Ch. Tx De-Emphasis at 3.5 dB.
  UINT32  pcs_tx_deemph_6db    :6;   //29:24 pcs_tx_deemph_6db. Read-write. Reset: 1Ch. Tx De-Emphasis at 6 dB.
  UINT32  Reserved             :2;   //31:30 Reserved.
} PHYPARACTL1_REG;

typedef union {
  PHYPARACTL1_REG  Reg;
  UINT32           Data32;
} PHYPARACTL1;

typedef struct {
  UINT32  pcs_tx_swing_full :7;   //6:0 pcs_tx_swing_full. Read-write. Reset: 7Fh. Tx Amplitude (Full Swing Mode).
  UINT32  tx_vboost_lvl     :3;   //9:7 tx_vboost_lvl. Read-write. Reset: 4h. Tx Voltage Boost Level.
  UINT32  Reserved          :22;  //31:10 Reserved.
} PHYPARACTL2_REG;

typedef union {
  PHYPARACTL2_REG  Reg;
  UINT32           Data32;
} PHYPARACTL2;

//============================================================================================================================================================================================================
//SYSHUB::USB::LANECTRL register defines
//============================================================================================================================================================================================================
typedef  struct {
  UINT32  TXVREFTUNE          :4;  //3:0 TXVREFTUNE. Read-write. Reset: 3h. Check: 3h. HS DC Voltage Level Adjustment.
  UINT32  VDATREFTUNE         :2;  //5:4 VDATREFTUNE. Read-write. Reset: 1. Check: 1h. Data Detect Voltage Adjustment.
  UINT32  TXRISETUNE          :2;  //7:6 TXRISETUNE. Read-write. Reset: 2h. Check: 1h. HS Transmitter Rise/Fall Time Adjustment. Program this field to 2'b01.
  UINT32  TXRESTUNE           :2;  //9:8 TXRESTUNE. Read-write. Reset: 1. Check: 1h. USB Source Impedance Adjustment.
  UINT32  TXPREEMPAMPTUNE     :2;  //11:10 TXPREEMPAMPTUNE. Read-write. Reset: 0. Check: 0h. HS Transmitter Pre-Emphasis Curent Control.
  UINT32  TXPREEMPPULSETUNE   :1;  //12 TXPREEMPPULSETUNE. Read-write. Reset: 0. Check: 0h. HS Transmitter Pre-Emphasis Duration Control.
  UINT32  LANEPARACTL0_13     :1;  //13 LANEPARACTL0_13. Read-write. Reset: Fixed,0. Check: 0h. Reserved.
  UINT32  TXHSXVTUNE          :2;  //15:14 TXHSXVTUNE. Read-write. Reset: 3h. Check: 3h. Transmitter High-Speed Crossover Adjustment.
  UINT32  TXFSLSTUNE          :4;  //19:16 TXFSLSTUNE. Read-write. Reset: 3h. Check: 3h. FS/LS Source Impedance Adjustment.
  UINT32  SQRXTUNE            :3;  //22:20 SQRXTUNE. Read-write. Reset: 3h. Check: 3h. Squelch Threshold Adjustment.
  UINT32  COMPDISTUNE         :3;  //25:23 COMPDISTUNE. Read-write. Reset: 3h. Check: 3h. Disconnect Threshold Adjustment.
  UINT32  LANEPARACTL0_31_26  :6;  //31:26 LANEPARACTL0_31_26. Read-write. Reset: Fixed,0. Check: 00h. Reserved.
} LANEPARACTL0_REG;

typedef union {
  LANEPARACTL0_REG Reg;
  UINT32           Data32;
} LANEPARACTL0;
//============================================================================================================================================================================================================
//SYSHUB::USB::PHYLANE register defines 
//============================================================================================================================================================================================================
typedef  struct {
  UINT32  AMPLITUDE  :7;   //6:0 AMPLITUDE. Read-write. Reset: 0. Override value for transmit amplitude.
  UINT32  PREEMPH    :7;   //13:7 PREEMPH. Read-write. Reset: 0. Override value for transmit pre-emphasis.
  UINT32  EN         :1;   //14 EN. Read-write. Reset: 0. Enables override values for all inputs controlled by this register.
  UINT32  Reserved   :17;  //31:15 Reserved.
} TX_OVRD_DRV_LO_REG;

typedef union {
  TX_OVRD_DRV_LO_REG Reg;
  UINT32             Data32;
} TX_OVRD_DRV_LO;

#define PHY_EXT_TX_EQ0_OFFSET  0x8270

typedef struct {
  UINT32  PROTOCOL_EXT_TX_EQ_MAIN_G1  :6;
  UINT32  PROTOCOL_EXT_TX_EQ_MAIN_G2  :6;
  UINT32  PROTOCOL_EXT_TX_EQ_OVRD_G1  :1;
  UINT32  PROTOCOL_EXT_TX_EQ_OVRD_G2  :1;
  UINT32  Reserved_14_31              :18;
} PHY_EXT_TX_EQ0_REG;

typedef union {
  PHY_EXT_TX_EQ0_REG Reg;
  UINT32             Data32;
} PHY_EXT_TX_EQ0;

#define PHY_EXT_TX_EQ1_OFFSET  0x8274

typedef struct {
  UINT32  PROTOCOL_EXT_TX_EQ_POST_G1  :6;
  UINT32  PROTOCOL_EXT_TX_EQ_POST_G2  :6;
  UINT32  PROTOCOL_EXT_TX_EQ_PRE_G1   :6;
  UINT32  PROTOCOL_EXT_TX_EQ_PRE_G2   :6;
  UINT32  Reserved_24_31              :8;
} PHY_EXT_TX_EQ1_REG;

typedef union {
  PHY_EXT_TX_EQ1_REG  Reg;
  UINT32              Data32;
} PHY_EXT_TX_EQ1;

#define SUP_DIG_LVL_OVER_IN_OFFSET  0x21

typedef struct{
  UINT16  RX_VREF_CTRL     :3;
  UINT16  RX_VREF_CTRL_EN  :1;
  UINT16  TX_VBOOST_LVL    :3;
  UINT16  TX_VBOOST_LVL_EN :1;
  UINT16  RESERVED_8_15    :8;
}SUP_DIG_LVL_OVER_IN_REG;

typedef union{
  SUP_DIG_LVL_OVER_IN_REG  Reg;
  UINT16                   Data16;
}SUP_DIG_LVL_OVER_IN;

// SATA RSMU registers
#define FCH_SMN_SATA_CONTROL_APPERTURE_ID_HYEX  (0x031)
#define FCH_SMN_SATA_CONTROL_APPERTURE_ID_HYGX  (0x028)

// HyEx
#define SATA_NUMBER_PER_SATORI 4
#define SATA_PORT_NUMBER_PER_AHCI_ON_SATORI  TAISHAN_SATA_PORT_NUM

#define SATA_SPACE_HYEX(RbNum, Address)          (UINT32)(Address + (RbNum << 20))

#define FCH_SMN_SATA_CONTROL_RSMU_HYEX            ((FCH_SMN_SATA_CONTROL_APPERTURE_ID_HYEX << 20) + 0x0000)  // 0x03100000ul
#define FCH_SMN_SATA_CONTROL_BAR5_HYEX            ((FCH_SMN_SATA_CONTROL_APPERTURE_ID_HYEX << 20) + 0x1000)  // 0x03101000ul
#define FCH_SMN_SATA_CONTROL_SLOR_HYEX            ((FCH_SMN_SATA_CONTROL_APPERTURE_ID_HYEX << 20) + 0x1800)  // 0x03101800ul
#define FCH_SMN_SATA_GENII_SETTING_HYEX           ((FCH_SMN_SATA_CONTROL_APPERTURE_ID_HYEX << 20) + 0x1848)  // 0x03101848ul
#define FCH_SMN_SATA_GENIII_SETTING_HYEX          ((FCH_SMN_SATA_CONTROL_APPERTURE_ID_HYEX << 20) + 0x1850)  // 0x03101850ul

#define FCH_TS_SATA_MISC_CONTROL_HYEX             (FCH_SMN_SATA_CONTROL_RSMU_HYEX + 0x00)     //0x00
#define FCH_TS_SATA_OOB_CONTROL_HYEX              (FCH_SMN_SATA_CONTROL_RSMU_HYEX + 0x02)     //0x02
#define FCH_TS_SATA_AOAC_CONTROL_HYEX             (FCH_SMN_SATA_CONTROL_RSMU_HYEX + 0x04)     //0x04
#define FCH_TS_SATA_EVENT_SELECT_HYEX             (FCH_SMN_SATA_CONTROL_RSMU_HYEX + 0x08)     //0x08
#define FCH_TS_SATA_NBIF_CONTROL_HYEX             (FCH_SMN_SATA_CONTROL_RSMU_HYEX + 0x0A)     //0x0A

// HyGx
#define SATA_NUMBER_PER_DUJIANG  1
#define SATA_PORT_NUMBER_PER_DJ  TAISHAN_SATA_PORT_NUM
#define MAX_SATA_NUMBER_PER_SOCKET   4
#define SATA_SPACE_HYGX(PhysicalDieId, SataIndex, Address)  (UINT32)(0x10000000 + (PhysicalDieId << 28) + (SataIndex << 20) + Address)

#define FCH_SMN_SATA_CONTROL_RSMU_HYGX            ((FCH_SMN_SATA_CONTROL_APPERTURE_ID_HYGX << 20) + 0x0000)  // 0x02800000ul
#define FCH_SMN_SATA_CONTROL_BAR5_HYGX            ((FCH_SMN_SATA_CONTROL_APPERTURE_ID_HYGX << 20) + 0x1000)  // 0x02801000ul
#define FCH_SMN_SATA_CONTROL_SLOR_HYGX            ((FCH_SMN_SATA_CONTROL_APPERTURE_ID_HYGX << 20) + 0x1800)  // 0x02801800ul
#define FCH_SMN_SATA_GENII_SETTING_HYGX           ((FCH_SMN_SATA_CONTROL_APPERTURE_ID_HYGX << 20) + 0x1848)  // 0x02801848ul
#define FCH_SMN_SATA_GENIII_SETTING_HYGX          ((FCH_SMN_SATA_CONTROL_APPERTURE_ID_HYGX << 20) + 0x1850)  // 0x02801850ul

#define FCH_TS_SATA_MISC_CONTROL_HYGX             (FCH_SMN_SATA_CONTROL_RSMU_HYGX + 0x00)     // 0x00
#define FCH_TS_SATA_OOB_CONTROL_HYGX              (FCH_SMN_SATA_CONTROL_RSMU_HYGX + 0x02)     // 0x02
#define FCH_TS_SATA_AOAC_CONTROL_HYGX             (FCH_SMN_SATA_CONTROL_RSMU_HYGX + 0x04)     // 0x04
#define FCH_TS_SATA_EVENT_SELECT_HYGX             (FCH_SMN_SATA_CONTROL_RSMU_HYGX + 0x08)     // 0x08
#define FCH_TS_SATA_NBIF_CONTROL_HYGX             (FCH_SMN_SATA_CONTROL_RSMU_HYGX + 0x0A)     // 0x0A

// xGbE registers
#define FCH_SMN_XGBE_MMIO0_APPERTURE_ID_HYEX  (0x168)
#define FCH_SMN_XGBE_MMIO0_APPERTURE_ID_HYGX  (0x0E6)

#define FCH_SMN_MMDMA2AXI_0_HYGX              (0x0E7F0000)
#define FCH_SMN_MMDMA2AXI_1_HYGX              (0x0E7F0008)
#define FCH_SMN_MMDMA2AXI_2_HYGX              (0x0E7F0010)

// HyEx
#define FCH_SMN_XGBE_MMIO0_BASE_HYEX              ((FCH_SMN_XGBE_MMIO0_APPERTURE_ID_HYEX << 20) + 0x1F000)  // 0x1681F000ul

#define FCH_TS_XGBE_PAD_MUX0_HYEX                 (FCH_SMN_XGBE_MMIO0_BASE_HYEX + 0x00)
#define FCH_TS_XGBE_PAD_MUX1_HYEX                 (FCH_SMN_XGBE_MMIO0_BASE_HYEX + 0x04)
#define FCH_TS_XGBE_PORT0_CLK_GATE_HYEX           (FCH_SMN_XGBE_MMIO0_BASE_HYEX + 0x30)
#define FCH_TS_XGBE_PORT0_PROPERTY0_HYEX          (FCH_SMN_XGBE_MMIO0_BASE_HYEX + 0x800)
#define FCH_TS_XGBE_PORT0_PROPERTY1_HYEX          (FCH_SMN_XGBE_MMIO0_BASE_HYEX + 0x804)
#define FCH_TS_XGBE_PORT0_PROPERTY2_HYEX          (FCH_SMN_XGBE_MMIO0_BASE_HYEX + 0x808)
#define FCH_TS_XGBE_PORT0_PROPERTY3_HYEX          (FCH_SMN_XGBE_MMIO0_BASE_HYEX + 0x80C)
#define FCH_TS_XGBE_PORT0_PROPERTY4_HYEX          (FCH_SMN_XGBE_MMIO0_BASE_HYEX + 0x810)
#define FCH_TS_XGBE_PORT0_PROPERTY5_HYEX          (FCH_SMN_XGBE_MMIO0_BASE_HYEX + 0x814)
#define FCH_TS_XGBE_PORT0_MAC_ADDR_LOW_HYEX       (FCH_SMN_XGBE_MMIO0_BASE_HYEX + 0x880)
#define FCH_TS_XGBE_PORT0_MAC_ADDR_HIGH_HYEX      (FCH_SMN_XGBE_MMIO0_BASE_HYEX + 0x884)

// HyGx
#define FCH_SMN_XGBE_MMIO0_BASE_HYGX              ((FCH_SMN_XGBE_MMIO0_APPERTURE_ID_HYGX << 20) + 0x1F000)  //0x0E61F000ul

#define FCH_TS_XGBE_PAD_MUX0_HYGX                 (FCH_SMN_XGBE_MMIO0_BASE_HYGX + 0x00)
#define FCH_TS_XGBE_PAD_MUX1_HYGX                 (FCH_SMN_XGBE_MMIO0_BASE_HYGX + 0x04)
#define FCH_TS_XGBE_PORT0_CLK_GATE_HYGX           (FCH_SMN_XGBE_MMIO0_BASE_HYGX + 0x30)
#define FCH_TS_XGBE_PORT0_PROPERTY0_HYGX          (FCH_SMN_XGBE_MMIO0_BASE_HYGX + 0x800)
#define FCH_TS_XGBE_PORT0_PROPERTY1_HYGX          (FCH_SMN_XGBE_MMIO0_BASE_HYGX + 0x804)
#define FCH_TS_XGBE_PORT0_PROPERTY2_HYGX          (FCH_SMN_XGBE_MMIO0_BASE_HYGX + 0x808)
#define FCH_TS_XGBE_PORT0_PROPERTY3_HYGX          (FCH_SMN_XGBE_MMIO0_BASE_HYGX + 0x80C)
#define FCH_TS_XGBE_PORT0_PROPERTY4_HYGX          (FCH_SMN_XGBE_MMIO0_BASE_HYGX + 0x810)
#define FCH_TS_XGBE_PORT0_PROPERTY5_HYGX          (FCH_SMN_XGBE_MMIO0_BASE_HYGX + 0x814)
#define FCH_TS_XGBE_PORT0_MAC_ADDR_LOW_HYGX       (FCH_SMN_XGBE_MMIO0_BASE_HYGX + 0x880)
#define FCH_TS_XGBE_PORT0_MAC_ADDR_HIGH_HYGX      (FCH_SMN_XGBE_MMIO0_BASE_HYGX + 0x884)

// FCH register SMN address
#define FCH_SMN_GPIO_APPERTURE_ID_HYEX  (0x02D)
#define FCH_SMN_GPIO_APPERTURE_ID_HYGX  (0x031)

// HyEx
#define FCH_SMN_SMI_BASE_HYEX                     ((FCH_SMN_GPIO_APPERTURE_ID_HYEX << 20) + 0x1200)  // 0x02D01200ul
#define FCH_SMN_IOMUX_BASE_HYEX                   ((FCH_SMN_GPIO_APPERTURE_ID_HYEX << 20) + 0x1D00)  // 0x02D01D00ul
#define FCH_SMN_MISC_BASE_HYEX                    ((FCH_SMN_GPIO_APPERTURE_ID_HYEX << 20) + 0x1E00)  // 0x02D01E00ul
#define FCH_SMN_GPIO_BASE_HYEX                    ((FCH_SMN_GPIO_APPERTURE_ID_HYEX << 20) + 0x2500)  // 0x02D02500ul
#define FCH_SMN_REMOTE_GPIO_BASE_HYEX             ((FCH_SMN_GPIO_APPERTURE_ID_HYEX << 20) + 0x2200)  // 0x02D02200ul

// HyGx
#define FCH_SMN_SMI_BASE_HYGX                     ((FCH_SMN_GPIO_APPERTURE_ID_HYGX << 20) + 0x1200)  // 0x03101200ul
#define FCH_SMN_IOMUX_BASE_HYGX                   ((FCH_SMN_GPIO_APPERTURE_ID_HYGX << 20) + 0x1D00)  // 0x03101D00ul
#define FCH_SMN_MISC_BASE_HYGX                    ((FCH_SMN_GPIO_APPERTURE_ID_HYGX << 20) + 0x1E00)  // 0x03101E00ul
#define FCH_SMN_GPIO_BASE_HYGX                    ((FCH_SMN_GPIO_APPERTURE_ID_HYGX << 20) + 0x2500)  // 0x03102500ul
#define FCH_SMN_REMOTE_GPIO_BASE_HYGX             ((FCH_SMN_GPIO_APPERTURE_ID_HYGX << 20) + 0x2200)  // 0x03102200ul

// FCH GPIO Mux
#define FCH_TS_IOMUX_GPIO26   0x1A
#define FCH_TS_IOMUX_GPIO27   0x1B
#define FCH_TS_IOMUX_GPIO266  0xCA
#define FCH_TS_IOMUX_GPIO267  0xCB

// FCH GPIO Output
#define FCH_TS_CONTROL_GPIO26 0x68
#define FCH_TS_CONTROL_GPIO27 0x6C
#define FCH_TS_CONTROL_GPIO266 0x28
#define FCH_TS_CONTROL_GPIO267 0x2C

#define FCH_TS_IRQ_INTA       0x00
#define FCH_TS_IRQ_INTB       0x01
#define FCH_TS_IRQ_INTC       0x02
#define FCH_TS_IRQ_INTD       0x03
#define FCH_TS_IRQ_GENINT2    0x05
#define FCH_TS_IRQ_SCI        0x10      // SCI
#define FCH_TS_IRQ_SMBUS0     0x11      // SMBUS0
#define FCH_TS_IRQ_ASF        0x12      // ASF
#define FCH_TS_IRQ_GBE0       0x14      // GBE0
#define FCH_TS_IRQ_GBE1       0x15      // GBE1
#define FCH_TS_IRQ_SD         0x17      // SD
#define FCH_TS_IRQ_SDIO       0x1A      // SDIO
#define FCH_TS_IRQ_GPIOa      0x21
#define FCH_TS_IRQ_GPIOb      0x22
#define FCH_TS_IRQ_GPIOc      0x23
#define FCH_TS_IRQ_USB_EMU    0x30      // HYGON USB Emulation Interrupt
#define FCH_TS_IRQ_USB_DR0    0x31      // USB DualRole Intr0
#define FCH_TS_IRQ_USB_DR1    0x32      // USB DualRole Intr1
#define FCH_TS_IRQ_USB_XHCI0  0x34      // XHCI0
#define FCH_TS_IRQ_USB_SSIC   0x35      // SSIC
#define FCH_TS_IRQ_SATA       0x41      // SATA
#define FCH_TS_IRQ_UFS        0x42      // UFS
#define FCH_TS_IRQ_EMMC       0x43      // EMMC
#define FCH_TS_IRQ_GPIO       0x62      // GPIO
#define FCH_TS_IRQ_I2C0       0x70      // I2C0
#define FCH_TS_IRQ_I2C1       0x71      // I2C1
#define FCH_TS_IRQ_I2C2       0x72      // I2C2
#define FCH_TS_IRQ_I2C3       0x73      // I2C3
#define FCH_TS_IRQ_UART0      0x74      // UART0
#define FCH_TS_IRQ_UART1      0x75      // UART1
#define FCH_TS_IRQ_I2C4       0x76      // I2C4
#define FCH_TS_IRQ_I2C5       0x77      // I2C5
#define FCH_TS_IRQ_UART2      0x78      // UART2
#define FCH_TS_IRQ_UART3      0x79      // UART3

// SciMap
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

#endif
