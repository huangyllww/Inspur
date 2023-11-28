/**
  USB register define
*/
#ifndef __HYGON_USB_REG__
#define __HYGON_USB_REG__
#include <Uefi.h>
//============================================================================================================================================================================================================
//USB controller regs smn address defines
//============================================================================================================================================================================================================
#define   USBDWC_SMN_ADDRESS_BASE    0x16C00000
#define   GUSB3PIPECTL_REGS0_GUSB3PIPECTL_SMN_ADDRESS   (USBDWC_SMN_ADDRESS_BASE+0xC2C0)
#define   GUSB3PIPECTL_REGS1_GUSB3PIPECTL_SMN_ADDRESS   (USBDWC_SMN_ADDRESS_BASE+0xC2C4)
#define   GUSB3PIPECTL_REGS2_GUSB3PIPECTL_SMN_ADDRESS   (USBDWC_SMN_ADDRESS_BASE+0xC2C8)
#define   GUSB3PIPECTL_REGS3_GUSB3PIPECTL_SMN_ADDRESS   (USBDWC_SMN_ADDRESS_BASE+0xC2CC)
//============================================================================================================================================================================================================
//SYSHUB::USB::PHYCTRL smn address defines
//============================================================================================================================================================================================================
#define   USBPHYCTRL_SMN_ADDRESS_BASE  0x16D08000
#define   PHYPARACTL1_LINK0_SMN_ADDRESS  (USBPHYCTRL_SMN_ADDRESS_BASE+0x0004)
#define   PHYPARACTL1_LINK1_SMN_ADDRESS  (USBPHYCTRL_SMN_ADDRESS_BASE+0x0404)
#define   PHYPARACTL1_LINK2_SMN_ADDRESS  (USBPHYCTRL_SMN_ADDRESS_BASE+0x0804)
#define   PHYPARACTL1_LINK3_SMN_ADDRESS  (USBPHYCTRL_SMN_ADDRESS_BASE+0x0C04)
#define   PHYPARACTL2_LINK0_SMN_ADDRESS  (USBPHYCTRL_SMN_ADDRESS_BASE+0x0008)
#define   PHYPARACTL2_LINK1_SMN_ADDRESS  (USBPHYCTRL_SMN_ADDRESS_BASE+0x0408)
#define   PHYPARACTL2_LINK2_SMN_ADDRESS  (USBPHYCTRL_SMN_ADDRESS_BASE+0x0808)
#define   PHYPARACTL2_LINK3_SMN_ADDRESS  (USBPHYCTRL_SMN_ADDRESS_BASE+0x0C08)
//============================================================================================================================================================================================================
//SYSHUB::USB::LANECTRL smn address defines
//============================================================================================================================================================================================================
#define   USBLANCTRL_SMN_ADDRESS_BASE  0x16D0C000
#define   LANEPARACTL0_LINK0_SMN_ADDRESS  (USBLANCTRL_SMN_ADDRESS_BASE+0x0000)
#define   LANEPARACTL0_LINK1_SMN_ADDRESS  (USBLANCTRL_SMN_ADDRESS_BASE+0x0400)
#define   LANEPARACTL0_LINK2_SMN_ADDRESS  (USBLANCTRL_SMN_ADDRESS_BASE+0x0800)
#define   LANEPARACTL0_LINK3_SMN_ADDRESS  (USBLANCTRL_SMN_ADDRESS_BASE+0x0C00)
//TX_OVRD_DRV_LO
#define   USBTX_OVRD_DRV_LO_ADDRESS_BASE  0x16d04000
#define   USBTX_OVRD_DRV_LO_LINK0_ADDRESS  (USBTX_OVRD_DRV_LO_ADDRESS_BASE + 0x0008)
#define   USBTX_OVRD_DRV_LO_LINK1_ADDRESS  (USBTX_OVRD_DRV_LO_ADDRESS_BASE + 0x0408)
#define   USBTX_OVRD_DRV_LO_LINK2_ADDRESS  (USBTX_OVRD_DRV_LO_ADDRESS_BASE + 0x0808)
#define   USBTX_OVRD_DRV_LO_LINK3_ADDRESS  (USBTX_OVRD_DRV_LO_ADDRESS_BASE + 0x0c08)
//============================================================================================================================================================================================================
//USB controller regs  defines
//============================================================================================================================================================================================================
typedef struct{
	UINT32    ELASTIC_BUFFER_MODE:1;
    UINT32    TX_DE_EPPHASIS:2;
	UINT32    TX_MARGIN:3;
    UINT32    TX_SWING:1;
    UINT32    SSICEn:1;
    UINT32    RX_DETECT_to_Polling_LFPS_Control:1;
	UINT32    LFPSFILTER:1;
    UINT32    P3ExSigP2:1;
    UINT32    P3P2TranOK:1;
	UINT32    LFPSP0Algn:1;
    UINT32    SkipRxDet:1;
    UINT32    AbortRxDetInU2:1;
	UINT32    DATWIDTH:2;
	UINT32    SUSPENDENABLE:1;
	UINT32    DELAYP1TRANS:1;
	UINT32    DelayP1P2P3:3;
    UINT32    DisRxDetU3RxDet:1;
    UINT32    StartRxDetU3RxDet:1;
    UINT32    request_p1p2p3:1;
    UINT32    u1u2exitfail_to_recov:1;
    UINT32    ping_enhancement_en:1;
	UINT32    Ux_exit_in_Px:1;
	UINT32    DisRxDetP3:1;
	UINT32    U2SSInactP3ok:1;
	UINT32    HstPrtCmpl:1;
    UINT32    PHYSoftRst:1;
}GUSB3PIPECTL_REG;
typedef union{
    GUSB3PIPECTL_REG   Reg;
	UINT32  Data32;
}GUSB3PIPECTL;

//============================================================================================================================================================================================================
//SYSHUB::USB::PHYCTRL register defines
//============================================================================================================================================================================================================
typedef struct{
    UINT32  los_level:5;//4:0 los_level. Read-write. Reset: 09h. Loss-of-Signal Detector Sensitivity Level Control.
	UINT32  los_bias:3;//7:5 los_bias. Read-write. Reset: 5h. Loss-of-Signal Detector Threshold Level Control.
	UINT32  pcs_rx_los_mask_val:10;//17:8 pcs_rx_los_mask_val. Read-write. Reset: 3E8h. Configurable Loss-of-Signal Mask Width. SNPS required it is 10'h0F0 for 10us@24M reference clock. 10'h3e8 is fine for us since our suspend_clk is 187.5KHz.
    UINT32  pcs_tx_deemph_3p5db:6;//23:18 pcs_tx_deemph_3p5db. Read-write. Reset: 1Ch. Tx De-Emphasis at 3.5 dB.
	UINT32  pcs_tx_deemph_6db:6;//29:24 pcs_tx_deemph_6db. Read-write. Reset: 1Ch. Tx De-Emphasis at 6 dB.
	UINT32  Reserved:2;//31:30 Reserved.
}PHYPARACTL1_REG;
typedef union{
    PHYPARACTL1_REG  Reg;
	UINT32  Data32;
}PHYPARACTL1;

typedef struct{
	UINT32  pcs_tx_swing_full:7;//6:0 pcs_tx_swing_full. Read-write. Reset: 7Fh. Tx Amplitude (Full Swing Mode).
	UINT32  tx_vboost_lvl:3;//9:7 tx_vboost_lvl. Read-write. Reset: 4h. Tx Voltage Boost Level.
	UINT32  Reserved:22;//31:10 Reserved.
}PHYPARACTL2_REG;
typedef union{
    PHYPARACTL2_REG  Reg;
	UINT32  Data32;
}PHYPARACTL2;

//============================================================================================================================================================================================================
//SYSHUB::USB::LANECTRL register defines
//============================================================================================================================================================================================================
typedef  struct{
    UINT32  TXVREFTUNE:4;//3:0 TXVREFTUNE. Read-write. Reset: 3h. Check: 3h. HS DC Voltage Level Adjustment.
	UINT32  VDATREFTUNE:2;//5:4 VDATREFTUNE. Read-write. Reset: 1. Check: 1h. Data Detect Voltage Adjustment.
    UINT32  TXRISETUNE:2;//7:6 TXRISETUNE. Read-write. Reset: 2h. Check: 1h. HS Transmitter Rise/Fall Time Adjustment. Program this field to 2'b01.
	UINT32  TXRESTUNE:2;//9:8 TXRESTUNE. Read-write. Reset: 1. Check: 1h. USB Source Impedance Adjustment.
	UINT32  TXPREEMPAMPTUNE:2;//11:10 TXPREEMPAMPTUNE. Read-write. Reset: 0. Check: 0h. HS Transmitter Pre-Emphasis Curent Control.
	UINT32  TXPREEMPPULSETUNE:1;//12 TXPREEMPPULSETUNE. Read-write. Reset: 0. Check: 0h. HS Transmitter Pre-Emphasis Duration Control.
	UINT32  LANEPARACTL0_13:1;//13 LANEPARACTL0_13. Read-write. Reset: Fixed,0. Check: 0h. Reserved.
	UINT32  TXHSXVTUNE:2;//15:14 TXHSXVTUNE. Read-write. Reset: 3h. Check: 3h. Transmitter High-Speed Crossover Adjustment.
	UINT32  TXFSLSTUNE:4;//19:16 TXFSLSTUNE. Read-write. Reset: 3h. Check: 3h. FS/LS Source Impedance Adjustment.
	UINT32  SQRXTUNE:3;//22:20 SQRXTUNE. Read-write. Reset: 3h. Check: 3h. Squelch Threshold Adjustment.
	UINT32  COMPDISTUNE:3;//25:23 COMPDISTUNE. Read-write. Reset: 3h. Check: 3h. Disconnect Threshold Adjustment.
	UINT32  LANEPARACTL0_31_26:6;//31:26 LANEPARACTL0_31_26. Read-write. Reset: Fixed,0. Check: 00h. Reserved.
}LANEPARACTL0_REG;
typedef union{
    LANEPARACTL0_REG Reg;
	UINT32  Data32;
}LANEPARACTL0;

typedef struct{
    UINT32  AMPLITUDE:7;
	UINT32  PREEMPH:7;
	UINT32  EN:1;
	UINT32  RESERVED:17;
}TXOVRDDEVLO_REG;
typedef union{
   TXOVRDDEVLO_REG Reg;
   UINT32  Data32;
}TXOVRDDEVLO;
#endif