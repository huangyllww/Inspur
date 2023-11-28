/* $NoKeywords:$ */
/**
 * @file
 *
 * Agesa structures and definitions
 *
 * Contains AMD AGESA core interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Include
 * @e \$Revision: 317069 $   @e \$Date: 2015-04-20 11:25:50 -0700 (Mon, 20 Apr 2015) $
 */
/*****************************************************************************
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
 *
 ***************************************************************************/


#ifndef _FCHPT_H_
#define _FCHPT_H_

#pragma pack (push, 1)

//++++++++++++++++++++++++++++++++++ Promontory param structure
/// PT_USB structure
typedef struct {
  UINT8         PTXhciGen1;                     ///< PTXhciGen1
  UINT8         PTXhciGen2;                     ///< PTXhciGen2
  UINT8         PTAOAC;                         ///< PTAOAC
  UINT8         PTHW_LPM ;                      ///< PTHW_LPM
  UINT8         PTDbC;                          ///< PTDbC
  UINT8         PTXHC_PME;                      ///< PTXHC_PME
  UINT8         PTSystemSpreadSpectrum;         ///< PTSystemSpreadSpectrum
  UINT8         Equalization4;                  ///< Enable/Disable Equalization 4
  UINT8         Redriver;                       ///< Enable/Disable Redriver Setting
} PT_USB;
/// PT_SATA structure
typedef struct {
  UINT8         PTSataPortEnable;               ///< PTSataEnable
  UINT8         PTSataMode;                     ///< PTSataMode
  UINT8         PTSataAggresiveDevSlpP0;        ///< PTSataAggresiveDevSlpP0
  UINT8         PTSataAggresiveDevSlpP1;        ///< PTSataAggresiveDevSlpP1
  UINT8         PTSataAggrLinkPmCap;            ///< PTSataAggrLinkPmCap
  UINT8         PTSataPscCap;                   ///< PTSataPscCap
  UINT8         PTSataSscCap;                   ///< PTSataSscCap
  UINT8         PTSataMsiCapability;            ///< PTSataPscCap
  UINT8         PTSataPortMdPort0;              ///< PTSataPortMdPort0
  UINT8         PTSataPortMdPort1;              ///< PTSataPortMdPort1
  UINT8         PTSataHotPlug;                  ///< PTSataHotPlug
} PT_SATA;
/// PT_PCIE structure
typedef struct {
  UINT8         PromontoryPCIeEnable;                   ///< PCIeEnable
  UINT8         PromontoryPCIeASPM;                     ///< PCIeASPM
} PT_PCIE;
/// PT_DIDVID structure
typedef struct {
  UINT8         GppNumber;                             ///< GppNumber
  UINT32        XhciID;                                ///< XhciDIDVID
  UINT32        SataID;                                ///< SataDIDVID
  UINT32        GpioID;                                ///< GpioDIDVID
  UINT64        FwVersion;                                ///< FwVersion
} PT_ADDR;
/// PT_USBPort structure
typedef struct {                                
  UINT8         PTUsb31P0;                    ///< PTUsb31Port0 Enable/Disable for PROM3/4
  UINT8         PTUsb31P1;                    ///< PTUsb31Port0 Enable/Disable for PROM3/4
  UINT8         PTUsb30P0;                    ///< PTUsb30Port0 Enable/Disable for PROM3/4
  UINT8         PTUsb30P1;                    ///< PTUsb30Port1 Enable/Disable for PROM3/4
  UINT8         PTUsb30P2;                    ///< PTUsb30Port2 Enable/Disable for PROM3/4
  UINT8         PTUsb30P3;                    ///< PTUsb30Port3 Enable/Disable for PROM3/4
  UINT8         PTUsb30P4;                    ///< PTUsb30Port4 Enable/Disable for PROM3/4
  UINT8         PTUsb30P5;                    ///< PTUsb30Port5 Enable/Disable for PROM3/4
  UINT8         PTUsb20P0;                    ///< PTUsb20Port0 Enable/Disable for PROM3/4
  UINT8         PTUsb20P1;                    ///< PTUsb20Port1 Enable/Disable for PROM3/4
  UINT8         PTUsb20P2;                    ///< PTUsb20Port2 Enable/Disable for PROM3/4
  UINT8         PTUsb20P3;                    ///< PTUsb20Port3 Enable/Disable for PROM3/4
  UINT8         PTUsb20P4;                    ///< PTUsb20Port4 Enable/Disable for PROM3/4
  UINT8         PTUsb20P5;                    ///< PTUsb20Port5 Enable/Disable for PROM3/4
} PT_USBPort;

/// PT_USBPort structure for PROM2
typedef struct {                                
  UINT8         PTUsb31P0;                    ///< PTUsb31Port0 Enable/Disable for PROM2
  UINT8         PTUsb31P1;                    ///< PTUsb31Port0 Enable/Disable for PROM2
  UINT8         PTUsb30P0;                    ///< PTUsb30Port0 Enable/Disable for PROM2
  UINT8         PTUsb30P1;                    ///< PTUsb30Port1 Enable/Disable for PROM2
  UINT8         PTUsb20P0;                    ///< PTUsb20Port0 Enable/Disable for PROM2
  UINT8         PTUsb20P1;                    ///< PTUsb20Port1 Enable/Disable for PROM2
  UINT8         PTUsb20P2;                    ///< PTUsb20Port2 Enable/Disable for PROM2
  UINT8         PTUsb20P3;                    ///< PTUsb20Port3 Enable/Disable for PROM2
  UINT8         PTUsb20P4;                    ///< PTUsb20Port4 Enable/Disable for PROM2
  UINT8         PTUsb20P5;                    ///< PTUsb20Port5 Enable/Disable for PROM2
} PT_USBPortPROM2;


/// PT_USBPort structure for PROM1
typedef struct {                                
  UINT8         PTUsb31P0;                    ///< PTUsb31Port0 Enable/Disable for PROM1
  UINT8         PTUsb31P1;                    ///< PTUsb31Port0 Enable/Disable for PROM1
  UINT8         PTUsb30P0;                    ///< PTUsb30Port0 Enable/Disable for PROM1
  UINT8         PTUsb20P0;                    ///< PTUsb20Port0 Enable/Disable for PROM1
  UINT8         PTUsb20P1;                    ///< PTUsb20Port1 Enable/Disable for PROM1
  UINT8         PTUsb20P2;                    ///< PTUsb20Port2 Enable/Disable for PROM1
  UINT8         PTUsb20P3;                    ///< PTUsb20Port3 Enable/Disable for PROM1
  UINT8         PTUsb20P4;                    ///< PTUsb20Port4 Enable/Disable for PROM1
  UINT8         PTUsb20P5;                    ///< PTUsb20Port5 Enable/Disable for PROM1
} PT_USBPortPROM1;


///PTUSB31TxStructure
typedef struct {
  UINT8         USB31Gen1Swing;           ///< PTUSB31PCS_B1 genI swing
  UINT8         USB31Gen2Swing;           ///< PTUSB31PCS_B1 genII swing
  UINT8         USB31Gen1PreEmEn;         ///< PTUSB31PCS_B1 genI pre-emphasis enable
  UINT8         USB31Gen2PreEmEn;         ///< PTUSB31PCS_B1 genII pre-emphasis enable
  UINT8         USB31Gen1PreEmLe;         ///< PTUSB31PCS_B1 genI pre-emphasis level
  UINT8         USB31Gen2PreEmLe;         ///< PTUSB31PCS_B1 genII pre-emphasis level
  UINT8         USB31Gen1PreShEn;         ///< PTUSB31PCS_B1 genI pre-shoot enable
  UINT8         USB31Gen2PreShEn;         ///< PTUSB31PCS_B1 genII pre-shoot enable
  UINT8         USB31Gen1PreShLe;         ///< PTUSB31PCS_B1 genI pre-shoot level
  UINT8         USB31Gen2PreShLe;         ///< PTUSB31PCS_B1 genII pre-shoot level
} PT_USB31Tx;

///PTUSB30TxStructure
typedef struct {
  UINT8         USB30Gen1Swing;           ///< PTUSB30PCS_B3 genI swing
  UINT8         USB30Gen1PreEmEn;         ///< PTUSB30PCS_B3 genI pre-emphasis enable
  UINT8         USB30Gen1PreEmLe;         ///< PTUSB30PCS_B3 genI pre-emphasis level
} PT_USB30Tx;

///PTUSBTxStructure
typedef struct {
  PT_USB31Tx    USB31Tx[2];               ///< USB31Tx setting
  PT_USB30Tx    USB30Tx[6];               ///< USB30Tx setting
  UINT8         USB20B2Tx00;              ///< USB2.0 TX driving current, 7: largest By USB_HSDP/N[0]
  UINT8         USB20B2Tx05;              ///< USB2.0 TX driving current, 7: largest By USB_HSDP/N[5]
  UINT8         USB20B3Tx1113;            ///< USB2.0 TX driving current, 7: largest By USB_HSDP/N[13][11]
  UINT8         USB20B3Tx1012;            ///< USB2.0 TX driving current, 7: largest By USB_HSDP/N[12][10]
  UINT8         USB20B4Tx0206;            ///< USB2.0 TX driving current, 7: largest By USB_HSDP/N[2][6]
  UINT8         USB20B4Tx0307;            ///< USB2.0 TX driving current, 7: largest By USB_HSDP/N[3][7]
  UINT8         USB20B5Tx0408;            ///< USB2.0 TX driving current, 7: largest By USB_HSDP/N[4][8]
  UINT8         USB20B5Tx0109;            ///< USB2.0 TX driving current, 7: largest By USB_HSDP/N[1][9]
} PT_USBTx;

///PTSataTxStructure
typedef struct {
  UINT8        SATAGen1Swing;             ///< genI swing
  UINT8        SATAGen2Swing;             ///< genII swing
  UINT8        SATAGen3Swing;             ///< genIII swing
  UINT8        SATAGen1PreEmEn;           ///< genI pre-emphasis enable
  UINT8        SATAGen2PreEmEn;           ///< genII pre-emphasis enable
  UINT8        SATAGen3PreEmEn;           ///< genIII pre-emphasis enable
  UINT8        SATAGen1PreEmLevel;        ///< genI pre-emphasis level
  UINT8        SATAGen2PreEmLevel;        ///< genII pre-emphasis level
  UINT8        SATAGen3PreEmLevel;        ///< genIII pre-emphasis level
} PT_SATATx;

///PT Svid Ssid Structure
typedef struct {
  UINT16        SVID;             ///< Svid
  UINT16        SSID;             ///< Ssid
} PT_ID;


///Promontory param structure
typedef struct _FCH_PT {
  PT_USB          PromontoryUSB;             ///<PTUSBStructure
  PT_SATA         PromontorySATA;            ///<PTSATAStructure
  PT_PCIE         PromontoryPCIE;            ///<PTPCIEStructure
  PT_ADDR         PromontoryAddr;            ///<PTIDStructure
  PT_USBPort      PromontoryUSBPort;         ///<PTUSBPortStructure
  PT_USBPortPROM2 PTUSBPortPROM2;            ///<PTUSBPortStructure for PROM2
  PT_USBPortPROM1 PTUSBPortPROM1;            ///<PTUSBPortStructure for PROM2
  PT_USBTx        PTUSBTX;                   ///<PTUSBTX
  PT_SATATx       PTSATATX[8];               ///<PTSATATX
  BOOLEAN         PCIEPorts[8];              ///<Enable/Disable PCIE Switch Downstream Ports 
  PT_ID           PTID[8];                   ///<PTID
} FCH_PT;

//-------------------------------------------- Promontory param structure
#pragma pack (pop)

#endif // _FCHPT_H_
