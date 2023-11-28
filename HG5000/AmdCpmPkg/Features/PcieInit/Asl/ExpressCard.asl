/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM Express Card SSDT Table
 *
 * Contains CPM code to provide ASL code support for Express Card.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  PcieInit
 * @e \$Revision$   @e \$Date$
 *
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
 ******************************************************************************
 */

DefinitionBlock (
    "ExpressCard.aml",      // Output file
    "SSDT",                 // Signature
    0x01,                   // DSDT Revision
    "AMD",                  // OEM ID
    "CPMXPRES",             // OEM Table ID
    0x1                     // OEM Revision
    )
{
    #define CPM_PCIE_ASL

    #include "CmnDef.asi"
    #include "CmnExt.asi"
    #include "PostCode.asi"

    Scope(\_GPE)                                    // General-Purpose Event
    {
        Method(CpmExpressCardEventMethod)
        {
            Store(CpmReadTable(CpmExpressCardTable, CpmExpressCardBridgeDevice), Local0)
            Store(CpmReadTable(CpmExpressCardTable, CpmExpressCardBridgeFunction), Local1)
            Store(CpmReadTable(CpmExpressCardTable, CpmExpressCardEventPin), Local2)
            Store(CpmReadTable(CpmExpressCardTable, CpmExpressCardDeviceId), Local3)

            If (CpmReadGevent (Local2))             // hot remove
            {
                CpmWriteGeventTriggerType (Local2, 0)
                CpmSetPostCode (CpmTpExpressCardRemoveEventBegin)
                CpmPcieHotplug(Local0, Local1, 0)
                CpmSetPostCode (CpmTpExpressCardRemoveEventEnd)
            }
            else                                    // hot insert
            {
                CpmWriteGeventTriggerType (Local2, 1)
                Store(CpmGetDeviceStatus(Local3), Local4)
                If (Local4)
                {
                    CpmSetPostCode (CpmTpExpressCardInsertEventBegin)
                    CpmPcieHotplug(Local0, Local1, 1)
                    CpmSetPostCode (CpmTpExpressCardInsertEventEnd)
                }
            }
            If (CondRefOf(\_SB.PCI0.CpmExpressCardBridge))
            {
                Notify(\_SB.PCI0.CpmExpressCardBridge, 0x00)
            }
        }

        Method(CpmOtherHotplugCard0EventMethod)
        {
            Store(CpmReadTable(CpmOtherHotplugCardTable, CpmOtherHotplogCard0BridgeDevice), Local0)
            Store(CpmReadTable(CpmOtherHotplugCardTable, CpmOtherHotplogCard0BridgeFunction), Local1)
            Store(CpmReadTable(CpmOtherHotplugCardTable, CpmOtherHotplogCard0EventPin), Local2)
            Store(CpmReadTable(CpmOtherHotplugCardTable, CpmOtherHotplogCard0DeviceId), Local3)

            If (CpmReadGevent (Local2))             // hot remove
            {
                CpmSetPostCode (CpmTpOtherHotplugCard0RemoveEventBegin)
                CpmWriteGeventTriggerType (Local2, 0)
                CpmSetDeviceReset(Local3, 0)        // Assert Reset
                Sleep(10)                           // Delay
                CpmSetDevicePower(Local3, 0)        // Power Off
                Sleep(10)                           // Delay
                CpmSetDeviceClock(Local3, 0)        // Clock Off

                CpmPcieHotplug(Local0, Local1, 0)
                CpmSetPostCode (CpmTpOtherHotplugCard0RemoveEventEnd)
            }
            else                                    // hot insert
            {
                CpmWriteGeventTriggerType (Local2, 1)
                Store(CpmGetDeviceStatus(Local3), Local4)
                If (Local4)
                {
                    CpmSetPostCode (CpmTpOtherHotplugCard0InsertEventBegin)
                    CpmSetDeviceClock(Local3, 1)        // Clock On
                    CpmSetDeviceReset(Local3, 0)        // Assert Reset
                    CpmSetDevicePower(Local3, 1)
                    Sleep(10)                           // Delay
                    CpmSetDeviceReset(Local3, 2)        // Delay for Reset
                    CpmSetDeviceReset(Local3, 1)        // De-assert Reset
                    Sleep(10)                           // Delay
                    CpmPcieHotplug(Local0, Local1, 1)
                    CpmSetDeviceClock(Local3, 2)        // Clock On
                    CpmSetPostCode (CpmTpOtherHotplugCard0InsertEventEnd)
                }
            }
            If (CondRefOf(\_SB.PCI0.CpmOtherHotplugCard0Bridge))
            {
                Notify(\_SB.PCI0.CpmOtherHotplugCard0Bridge, 0x00)
            }
        }

        Method(CpmOtherHotplugCard1EventMethod)
        {
            Store(CpmReadTable(CpmOtherHotplugCardTable, CpmOtherHotplogCard1BridgeDevice), Local0)
            Store(CpmReadTable(CpmOtherHotplugCardTable, CpmOtherHotplogCard1BridgeFunction), Local1)
            Store(CpmReadTable(CpmOtherHotplugCardTable, CpmOtherHotplogCard1EventPin), Local2)
            Store(CpmReadTable(CpmOtherHotplugCardTable, CpmOtherHotplogCard1DeviceId), Local3)

            If (CpmReadGevent (Local2))             // hot remove
            {
                CpmSetPostCode (CpmTpOtherHotplugCard1RemoveEventBegin)
                CpmWriteGeventTriggerType (Local2, 0)
                CpmSetDeviceReset(Local3, 0)        // Assert Reset
                Sleep(10)                           // Delay
                CpmSetDevicePower(Local3, 0)        // Power Off
                Sleep(10)                           // Delay
                CpmSetDeviceClock(Local3, 0)        // Clock Off

                CpmPcieHotplug(Local0, Local1, 0)
                CpmSetPostCode (CpmTpOtherHotplugCard1RemoveEventEnd)
            }
            else                                    // hot insert
            {
                CpmWriteGeventTriggerType (Local2, 1)
                Store(CpmGetDeviceStatus(Local3), Local4)
                If (Local4)
                {
                    CpmSetPostCode (CpmTpOtherHotplugCard1InsertEventBegin)
                    CpmSetDeviceClock(Local3, 1)        // Clock On
                    CpmSetDeviceReset(Local3, 0)        // Assert Reset
                    CpmSetDevicePower(Local3, 1)
                    Sleep(10)                           // Delay
                    CpmSetDeviceReset(Local3, 2)        // Delay for Reset
                    CpmSetDeviceReset(Local3, 1)        // De-assert Reset
                    Sleep(10)                           // Delay
                    CpmPcieHotplug(Local0, Local1, 1)
                    CpmSetDeviceClock(Local3, 2)        // Clock On
                    CpmSetPostCode (CpmTpOtherHotplugCard1InsertEventEnd)
                }
            }
            If (CondRefOf(\_SB.PCI0.CpmOtherHotplugCard1Bridge))
            {
                Notify(\_SB.PCI0.CpmOtherHotplugCard1Bridge, 0x00)
            }
        }
    }
}
