/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe configuration data
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 312288 $   @e \$Date: 2015-02-04 00:39:01 -0600 (Wed, 04 Feb 2015) $
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <PiDxe.h>
#include <GnbRegistersZP.h>
#include <GnbDxio.h>
#include <IdsHookId.h>
#include <Guid/HobList.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/GnbPciLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <Protocol/AmdNbioPcieAerProtocol.h>
#include "AmdNbioPcieAerZP.h"
#include "Filecode.h"

#define FILECODE NBIO_PCIE_AMDNBIOPCIEZPDXE_AMDNBIOPCIEAERZP_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
NbioSetPcieAerFeature (
  IN       AMD_NBIO_PCIE_AER_PROTOCOL  *This,
  IN       PCIe_PORT_AER_CONFIG        *PcieAerSetting
);

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
AMD_NBIO_PCIe_AER_CONFIG NbioPcieAerConfig = {
  NUMBER_OF_PORTS,
  {
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      1,          ///< PciDev
      1,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      1,          ///< PciDev
      2,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      1,          ///< PciDev
      3,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      1,          ///< PciDev
      4,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      1,          ///< PciDev
      5,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      1,          ///< PciDev
      6,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      1,          ///< PciDev
      7,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      2,          ///< PciDev
      1,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      3,          ///< PciDev
      1,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      3,          ///< PciDev
      2,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      3,          ///< PciDev
      3,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      3,          ///< PciDev
      4,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      3,          ///< PciDev
      5,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      3,          ///< PciDev
      6,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      3,          ///< PciDev
      7,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
    { 0xFF,       ///< PortAerEnable, 0=Disable 1=Enable 0xFF=do nothing
      4,          ///< PciDev
      1,          ///< PciFunc
      {0},        ///< CorrectableMask
      {0},        ///< UncorrectableMask
      {0},        ///< UncorrectableSeverity
    },
  }
}
  ;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AMD_NBIO_PCIE_AER_PROTOCOL mAmdNbioPcieAerProtocol = {
  NbioSetPcieAerFeature
};

/*----------------------------------------------------------------------------------------*/

/**
 *---------------------------------------------------------------------------------------
 *
 *  NbioPcieAerFeatureEnable
 *
 *  Description:
 *     Enable/Disable AER.
 *
 *  Parameters:
 *    @param[in]     GnbHandle
 *    @param[in]     PortAerConfig
 *
 *    @retval         VOID
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
NbioPcieAerFeatureEnable (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       NBIO_PCIe_PORT_AER_CONFIG  *PortAerConfig
  )
{
  UINT32                   Value;
  UINT32                   AddrIndex;
  UINT32                   CorrMaskAddr;
  UINT32                   UnCorrMaskAddr;
  UINT32                   UnCorrSeverityAddr;
  UINT32                   PcieErrCntlAddr;
  PCIe_ENGINE_CONFIG       *PcieEngine;
  PCIe_WRAPPER_CONFIG      *PcieWrapper;

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioPcieAerFeatureEnable Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    AddrIndex = (PcieWrapper->WrapId == 0) ? 0x00100000 : 0x00200000;
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated (PcieEngine)) {
        if ((PcieEngine->Type.Port.PortData.DeviceNumber == PortAerConfig->PciDev) &&
            (PcieEngine->Type.Port.PortData.FunctionNumber == PortAerConfig->PciFunc)) {
          IDS_HDT_CONSOLE (PCIE_MISC, "Set AER feature: Dev%d Func%d %x\n", PortAerConfig->PciDev, PortAerConfig->PciFunc, PortAerConfig->PortAerEnable);
          AddrIndex |= (PcieEngine->Type.Port.PortId % 8) << 12;
          PcieErrCntlAddr = 0x110401A8 | AddrIndex;
          CorrMaskAddr = 0x11000164 | AddrIndex;
          UnCorrMaskAddr = 0x11000158 | AddrIndex;
          UnCorrSeverityAddr = 0x1100015c | AddrIndex;

          if (PortAerConfig->PortAerEnable == 0) {
            NbioRegisterRead ( GnbHandle, TYPE_SMN, PcieErrCntlAddr, &Value, 0);
            Value |= BIT0;
            NbioRegisterWrite (GnbHandle, TYPE_SMN, PcieErrCntlAddr, &Value, GNB_REG_ACC_FLAG_S3SAVE);
          } else if (PortAerConfig->PortAerEnable == 1) {
            //
            // PCIe Correctable Error Mask
            //
            NbioRegisterRead ( GnbHandle, TYPE_SMN, CorrMaskAddr, &Value, 0);
            Value &= ~(BIT6 | BIT7 | BIT8 | BIT12 | BIT13);
            if (PortAerConfig->CorrectableMask.Field.BadTLPMask == 1) {
              Value |= BIT6;
            }
            if (PortAerConfig->CorrectableMask.Field.BadDLLPMask == 1) {
              Value |= BIT7;
            }
            if (PortAerConfig->CorrectableMask.Field.ReplayNumberRolloverMask == 1) {
              Value |= BIT8;
            }
            if (PortAerConfig->CorrectableMask.Field.ReplayTimerTimeoutMask == 1) {
              Value |= BIT12;
            }
            if (PortAerConfig->CorrectableMask.Field.AdvisoryNonFatalErrorMask == 1) {
              Value |= BIT13;
            }
            NbioRegisterWrite (GnbHandle, TYPE_SMN, CorrMaskAddr, &Value, GNB_REG_ACC_FLAG_S3SAVE);

            //
            // PCIe Unorrectable Error Mask
            //
            NbioRegisterRead ( GnbHandle, TYPE_SMN, UnCorrMaskAddr, &Value, 0);
            Value &= ~(BIT4 | BIT12 | BIT14 | BIT15 | BIT16 | BIT18 | BIT19 | BIT20 | BIT21);
            if (PortAerConfig->UncorrectableMask.Field.DataLinkProtocolErrorMask == 1) {
              Value |= BIT4;
            }
            if (PortAerConfig->UncorrectableMask.Field.PoisonedTLPMask == 1) {
              Value |= BIT12;
            }
            if (PortAerConfig->UncorrectableMask.Field.CompletionTimeoutMask == 1) {
              Value |= BIT14;
            }
            if (PortAerConfig->UncorrectableMask.Field.CompleterAbortMask == 1) {
              Value |= BIT15;
            }
            if (PortAerConfig->UncorrectableMask.Field.UnexpectedCompletionMask == 1) {
              Value |= BIT16;
            }
            if (PortAerConfig->UncorrectableMask.Field.MalTlpMask == 1) {
              Value |= BIT18;
            }
            if (PortAerConfig->UncorrectableMask.Field.ECRCErrorMask == 1) {
              Value |= BIT19;
            }
            if (PortAerConfig->UncorrectableMask.Field.UnsupportedRequestErrorMask == 1) {
              Value |= BIT20;
            }
            if (PortAerConfig->UncorrectableMask.Field.AcsViolationMask == 1) {
              Value |= BIT21;
            }
            NbioRegisterWrite (GnbHandle, TYPE_SMN, UnCorrMaskAddr, &Value, GNB_REG_ACC_FLAG_S3SAVE);

            //
            // PCIe Unorrectable Error Severity
            //
            NbioRegisterRead ( GnbHandle, TYPE_SMN, UnCorrSeverityAddr, &Value, 0);
            Value &= ~(BIT4 | BIT12 | BIT14 | BIT15 | BIT16 | BIT18 | BIT19 | BIT20 | BIT21);
            if (PortAerConfig->UncorrectableSeverity.Field.DataLinkProtocolErrorSeverity == 1) {
              Value |= BIT4;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.PoisonedTLPSeverity == 1) {
              Value |= BIT12;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.CompletionTimeoutSeverity == 1) {
              Value |= BIT14;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.CompleterAbortSeverity == 1) {
              Value |= BIT15;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.UnexpectedCompletionSeverity == 1) {
              Value |= BIT16;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.MalTlpSeverity == 1) {
              Value |= BIT18;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.ECRCErrorSeverity == 1) {
              Value |= BIT19;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.UnsupportedRequestErrorSeverity == 1) {
              Value |= BIT20;
            }
            if (PortAerConfig->UncorrectableSeverity.Field.AcsViolationSeverity == 1) {
              Value |= BIT21;
            }
            NbioRegisterWrite (GnbHandle, TYPE_SMN, UnCorrSeverityAddr, &Value, GNB_REG_ACC_FLAG_S3SAVE);
          }
        }
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioPcieAerFeatureEnable Exit\n");
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  DumpPcieAerSetting
 *
 *  Description:
 *     dump Nbio PCIe each port AER feature.
 *
 *  Parameters:
 *    @param[in]     PortAerConfig
 *
 *    @retval        VOID
 *
 *---------------------------------------------------------------------------------------
 **/
VOID DumpPcieAerSetting (
  IN       NBIO_PCIe_PORT_AER_CONFIG  *PortAerConfig
  )
{
  IDS_HDT_CONSOLE (PCIE_MISC, "Dev%d Func%d AER - %a\n", PortAerConfig->PciDev, PortAerConfig->PciFunc, PortAerConfig->PortAerEnable == 0? "Disable":"Enable");
  IDS_HDT_CONSOLE (PCIE_MISC, "  BadTLPMask - %d\n", PortAerConfig->CorrectableMask.Field.BadTLPMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  BadDLLPMask - %d\n", PortAerConfig->CorrectableMask.Field.BadDLLPMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  ReplayNumberRolloverMask - %d\n", PortAerConfig->CorrectableMask.Field.ReplayNumberRolloverMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  ReplayTimerTimeoutMask - %d\n", PortAerConfig->CorrectableMask.Field.ReplayTimerTimeoutMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  AdvisoryNonFatalErrorMask - %d\n", PortAerConfig->CorrectableMask.Field.AdvisoryNonFatalErrorMask);

  IDS_HDT_CONSOLE (PCIE_MISC, "  DataLinkProtocolErrorMask - %d\n", PortAerConfig->UncorrectableMask.Field.DataLinkProtocolErrorMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  PoisonedTLPMask - %d\n", PortAerConfig->UncorrectableMask.Field.PoisonedTLPMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  CompletionTimeoutMask - %d\n", PortAerConfig->UncorrectableMask.Field.CompletionTimeoutMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  CompleterAbortMask - %d\n", PortAerConfig->UncorrectableMask.Field.CompleterAbortMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  UnexpectedCompletionMask - %d\n", PortAerConfig->UncorrectableMask.Field.UnexpectedCompletionMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  MalTlpMask - %d\n", PortAerConfig->UncorrectableMask.Field.MalTlpMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  ECRCErrorMask - %d\n", PortAerConfig->UncorrectableMask.Field.ECRCErrorMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  UnsupportedRequestErrorMask - %d\n", PortAerConfig->UncorrectableMask.Field.UnsupportedRequestErrorMask);
  IDS_HDT_CONSOLE (PCIE_MISC, "  AcsViolationMask - %d\n", PortAerConfig->UncorrectableMask.Field.AcsViolationMask);

  IDS_HDT_CONSOLE (PCIE_MISC, "  DataLinkProtocolErrorSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.DataLinkProtocolErrorSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  PoisonedTLPSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.PoisonedTLPSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  CompletionTimeoutSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.CompletionTimeoutSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  CompleterAbortSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.CompleterAbortSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  UnexpectedCompletionSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.UnexpectedCompletionSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  MalTlpSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.MalTlpSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  ECRCErrorSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.ECRCErrorSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  UnsupportedRequestErrorSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.UnsupportedRequestErrorSeverity);
  IDS_HDT_CONSOLE (PCIE_MISC, "  AcsViolationSeverity - %d\n", PortAerConfig->UncorrectableSeverity.Field.AcsViolationSeverity);
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  NbioPcieAerInitZP
 *
 *  Description:
 *     Initializer Nbio PCIe each port AER feature.
 *
 *  Parameters:
 *    @param[in]     GnbHandle
 *
 *    @retval        VOID
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
NbioPcieAerInitZP (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  UINT32                   Index;

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioPcieAerInitZP Enter\n");
  NbioPcieAerConfig.NumberOfPorts = NUMBER_OF_PORTS;
  IDS_HDT_CONSOLE (GNB_TRACE, "Socket:%x, Die:%x, Instance:%x, Number of ports in NBIO: %d\n",  GnbHandle->SocketId, GnbHandle->DieNumber, GnbHandle->InstanceId, NbioPcieAerConfig.NumberOfPorts);
  IDS_HOOK(IDS_HOOK_NBIO_PCIE_AER_INIT, NULL, (void *)&NbioPcieAerConfig);

  for (Index = 0; Index < NbioPcieAerConfig.NumberOfPorts; Index++) {
    DumpPcieAerSetting ((NBIO_PCIe_PORT_AER_CONFIG *) &NbioPcieAerConfig.PortAerConfig[Index]);
    NbioPcieAerFeatureEnable (GnbHandle, (NBIO_PCIe_PORT_AER_CONFIG *) &NbioPcieAerConfig.PortAerConfig[Index]);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioPcieAerInitZP Exit\n");
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  SetPcieEcrcFeature
 *
 *  Description:
 *     Set Pcie AER ECRC feature enable.
 *
 *  Parameters:
 *    @param[in]     PciDevice
 *
 *    @retval        BOOLEAN
 *
 *---------------------------------------------------------------------------------------
 **/
BOOLEAN
SetPcieEcrcFeature (
  PCI_ADDR PciDevice
)
{
  UINT8     Bus, Dev, Fun;
  UINT16    ParentCapPtr;
  UINT16    ChildCapPtr;
  PCI_ADDR  Device;
  BOOLEAN   EcrcEnabled;
  UINT32    ParentCapFlag;      //Set Bit 5 ECRC Generation Capable, Bit 7 ECRC Check Capable
  UINT32    ChildCapFlag;
  UINT32    UceSeverity;

  ParentCapPtr = 0;
  ChildCapPtr = 0;
  Device.AddressValue = 0;
  EcrcEnabled = FALSE;
  ParentCapFlag = 0;
  ChildCapFlag = 0;
  UceSeverity = 0;

  // Check Bridge AER Capabilities
  ParentCapPtr = GnbLibFindPcieExtendedCapability (PciDevice.AddressValue, PCIE_EXT_AER_CAP_ID, NULL);
  if (ParentCapPtr != 0) {
    GnbLibPciRead (PciDevice.AddressValue | (ParentCapPtr + 0x18), AccessWidth32, &ParentCapFlag, NULL);
  }

  // Check Ecrc Capability support
  if ((ParentCapFlag & (BIT5 | BIT7)) != 0) {
    // Get Secondary Bug
    GnbLibPciRead (PciDevice.AddressValue | 0x19, AccessWidth8, &Bus, NULL);
    Device.Address.Bus = Bus;

    // Scan support ECRC endpoint device
    for (Dev = 0; Dev < 32; Dev++) {
      Device.Address.Device = Dev;

      for (Fun = 0; Fun <= 7; Fun++) {
        Device.Address.Function = Fun;

        if (GnbLibPciIsDevicePresent (Device.AddressValue, NULL)) {
          // Is Bridge ?
          if (GnbLibPciIsBridgeDevice (Device.AddressValue, NULL)) {
            EcrcEnabled = SetPcieEcrcFeature (Device);
          } else {
            // Is Endpoint Device
            ChildCapPtr = GnbLibFindPcieExtendedCapability (Device.AddressValue, PCIE_EXT_AER_CAP_ID, NULL);
            if (ChildCapPtr != 0) {
              GnbLibPciRead (Device.AddressValue | (ChildCapPtr + 0x18), AccessWidth32, &ChildCapFlag, NULL);

              // If Bridge & Endpoint device support than enable ECRC
              if ((ChildCapFlag & (BIT5 | BIT7)) != 0) {
                // Set Endpoint ECRC Generation & Check Enable
                ChildCapFlag |= (BIT6 | BIT8);
                GnbLibPciWrite (Device.AddressValue | (ChildCapPtr + 0x18), AccessS3SaveWidth32, &ChildCapFlag, NULL);
                IDS_HDT_CONSOLE (PCIE_MISC, "Set EndPoint ECRC Enable Bus=%02X, Dev=%02X, Fun=%02X, Reg=%03X\n", \
                    Device.Address.Bus, Device.Address.Device, Device.Address.Function, (Device.AddressValue | (ChildCapPtr + 0x18)));
                EcrcEnabled = TRUE;
                if (PcdGetBool (PcdPcieEcrcSeverityFatal)) {
                  // Set ECRC Severity as Fatal
                  GnbLibPciRead (Device.AddressValue | (ChildCapPtr + 0x0C), AccessWidth32, &UceSeverity, NULL);
                  UceSeverity |= BIT19;
                  GnbLibPciWrite (Device.AddressValue | (ChildCapPtr + 0x0C), AccessS3SaveWidth32, &UceSeverity, NULL);
                }
              }
            }
          }

          // If not multi function skip to next Device
          if (!GnbLibPciIsMultiFunctionDevice (Device.AddressValue, NULL)) {
            break;
          }
        }
      }  //Fun
    }  //Dev

    // Check if Endpoint device enable ECRC
    if (EcrcEnabled) {
      // Set Bridge ECRC Generation & Check Enable
      ParentCapFlag |= (BIT6 | BIT8);
      GnbLibPciWrite (PciDevice.AddressValue | (ParentCapPtr + 0x18), AccessS3SaveWidth32, &ParentCapFlag, NULL);
      IDS_HDT_CONSOLE (PCIE_MISC, "Set Bridget ECRC Enable Bus=%02X, Dev=%02X, Fun=%02X, Reg=%03X\n", \
          PciDevice.Address.Bus, PciDevice.Address.Device, PciDevice.Address.Function, (PciDevice.AddressValue | (ParentCapPtr + 0x18)));
      if (PcdGetBool (PcdPcieEcrcSeverityFatal)) {
        // Set ECRC Severity as Fatal
        GnbLibPciRead (PciDevice.AddressValue | (ParentCapPtr + 0x0C), AccessWidth32, &UceSeverity, NULL);
        UceSeverity |= BIT19;
        GnbLibPciWrite (PciDevice.AddressValue | (ParentCapPtr + 0x0C), AccessS3SaveWidth32, &UceSeverity, NULL);
      }
    }
  }
  return EcrcEnabled;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  NbioSetPcieAerFeature
 *
 *  Description:
 *     Enable/Disable AER feature.
 *
 *  Parameters:
 *    @param[in]     GnbHandle
 *    @param[in]     PortAerConfig
 *
 *    @retval        EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS
EFIAPI
NbioSetPcieAerFeature (
  IN       AMD_NBIO_PCIE_AER_PROTOCOL  *This,
  IN       PCIe_PORT_AER_CONFIG        *PcieAerSetting
)
{
  EFI_STATUS        Status;
  UINT16            PcieCapPtr;
  UINT32            Value;
  PCI_ADDR          Device;

  // IDS_HDT_CONSOLE (MAIN_FLOW, "NbioSetPcieAerFeature Entry\n");
  Status = EFI_UNSUPPORTED;
  Device.AddressValue = 0;

  if (PcieAerSetting->AerEnable == 1) {
    Device.Address.Bus = PcieAerSetting->PciBus;
    Device.Address.Device = PcieAerSetting->PciDev;
    Device.Address.Function = PcieAerSetting->PciFunc;

    PcieCapPtr = GnbLibFindPcieExtendedCapability (Device.AddressValue, PCIE_EXT_AER_CAP_ID, NULL);
    if (PcieCapPtr != 0) {
      // Uncorrectable Error Mask Register (Offset 08h)
      GnbLibPciRead (Device.AddressValue | (PcieCapPtr + 0x08), AccessWidth32, &Value, NULL);
      if (PcieAerSetting->UncorrectableMask.Field.DataLinkProtocolErrorMask == 1) {
        Value |= BIT4;
      }
      if (PcieAerSetting->UncorrectableMask.Field.PoisonedTLPMask == 1) {
        Value |= BIT12;
      }
      if (PcieAerSetting->UncorrectableMask.Field.CompletionTimeoutMask == 1) {
        Value |= BIT14;
      }
      if (PcieAerSetting->UncorrectableMask.Field.CompleterAbortMask == 1) {
        Value |= BIT15;
      }
      if (PcieAerSetting->UncorrectableMask.Field.UnexpectedCompletionMask == 1) {
        Value |= BIT16;
      }
      if (PcieAerSetting->UncorrectableMask.Field.MalTlpMask == 1) {
        Value |= BIT18;
      }
      if (PcieAerSetting->UncorrectableMask.Field.ECRCErrorMask == 1) {
        Value |= BIT19;
      }
      if (PcieAerSetting->UncorrectableMask.Field.UnsupportedRequestErrorMask == 1) {
        Value |= BIT20;
      }
      if (PcieAerSetting->UncorrectableMask.Field.AcsViolationMask == 1) {
        Value |= BIT21;
      }
      GnbLibPciWrite (Device.AddressValue | (PcieCapPtr + 0x08), AccessS3SaveWidth32, &Value, NULL);

      // Uncorrectable Error Severity Register (Offset 0Ch)
      GnbLibPciRead (Device.AddressValue | (PcieCapPtr + 0x0C), AccessWidth32, &Value, NULL);
      if (PcieAerSetting->UncorrectableSeverity.Field.DataLinkProtocolErrorSeverity == 1) {
        Value |= BIT4;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.PoisonedTLPSeverity == 1) {
        Value |= BIT12;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.CompletionTimeoutSeverity == 1) {
        Value |= BIT14;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.CompleterAbortSeverity == 1) {
        Value |= BIT15;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.UnexpectedCompletionSeverity == 1) {
        Value |= BIT16;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.MalTlpSeverity == 1) {
        Value |= BIT18;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.ECRCErrorSeverity == 1) {
        Value |= BIT19;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.UnsupportedRequestErrorSeverity == 1) {
        Value |= BIT20;
      }
      if (PcieAerSetting->UncorrectableSeverity.Field.AcsViolationSeverity == 1) {
        Value |= BIT21;
      }
      GnbLibPciWrite (Device.AddressValue | (PcieCapPtr + 0x0C), AccessS3SaveWidth32, &Value, NULL);

      // Correctable Error Mask Register (Offset 14h)
      GnbLibPciRead (Device.AddressValue | (PcieCapPtr + 0x14), AccessWidth32, &Value, NULL);
      if (PcieAerSetting->CorrectableMask.Field.BadTLPMask == 1) {
        Value |= BIT6;
      }
      if (PcieAerSetting->CorrectableMask.Field.BadDLLPMask == 1) {
        Value |= BIT7;
      }
      if (PcieAerSetting->CorrectableMask.Field.ReplayNumberRolloverMask == 1) {
        Value |= BIT8;
      }
      if (PcieAerSetting->CorrectableMask.Field.ReplayTimerTimeoutMask == 1) {
        Value |= BIT12;
      }
      if (PcieAerSetting->CorrectableMask.Field.AdvisoryNonFatalErrorMask == 1) {
        Value |= BIT13;
      }
      GnbLibPciWrite (Device.AddressValue | (PcieCapPtr + 0x14), AccessS3SaveWidth32, &Value, NULL);

      SetPcieEcrcFeature(Device);

      Status = EFI_SUCCESS;
    }
  }

  // IDS_HDT_CONSOLE (MAIN_FLOW, "NbioSetPcieAerFeature Exit(%x)\n", Status);
  return Status;
}
