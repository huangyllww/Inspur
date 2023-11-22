/* $NoKeywords:$ */

/**
 * @file
 *
 * Nbio PCIe AER Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Nbio
 */
/*****************************************************************************
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
 *
 ***************************************************************************/

#ifndef _NBIO_PCIE_AER_PROTOCOL_H_
#define _NBIO_PCIE_AER_PROTOCOL_H_

/// PCIe Correctable Error Mask
typedef union {
  struct {
    ///<
    UINT32    BadTLPMask                : 1;                          ///<
    UINT32    BadDLLPMask               : 1;                          ///<
    UINT32    ReplayNumberRolloverMask  : 1;                          ///<
    UINT32    ReplayTimerTimeoutMask    : 1;                          ///<
    UINT32    AdvisoryNonFatalErrorMask : 1;                          ///<
  } Field;                                                            ///<
  UINT32    Value;                                                    ///<
} PCIe_AER_CORRECTABLE_MASK;

/// PCIe Unorrectable Error Mask
typedef union {
  struct {
    ///<
    UINT32    DataLinkProtocolErrorMask   : 1;                        ///<
    UINT32    PoisonedTLPMask             : 1;                        ///<
    UINT32    CompletionTimeoutMask       : 1;                        ///<
    UINT32    CompleterAbortMask          : 1;                        ///<
    UINT32    UnexpectedCompletionMask    : 1;                        ///<
    UINT32    MalTlpMask                  : 1;                        ///<
    UINT32    ECRCErrorMask               : 1;                        ///<
    UINT32    UnsupportedRequestErrorMask : 1;                        ///<
    UINT32    AcsViolationMask            : 1;                        ///<
  } Field;                                                            ///<
  UINT32    Value;                                                    ///<
} PCIe_AER_UNCORRECTABLE_MASK;

/// PCIe Unorrectable Error Severity
typedef union {
  struct {
    ///<
    UINT32    DataLinkProtocolErrorSeverity   : 1;                    ///<
    UINT32    PoisonedTLPSeverity             : 1;                    ///<
    UINT32    CompletionTimeoutSeverity       : 1;                    ///<
    UINT32    CompleterAbortSeverity          : 1;                    ///<
    UINT32    UnexpectedCompletionSeverity    : 1;                    ///<
    UINT32    MalTlpSeverity                  : 1;                    ///<
    UINT32    ECRCErrorSeverity               : 1;                    ///<
    UINT32    UnsupportedRequestErrorSeverity : 1;                    ///<
    UINT32    AcsViolationSeverity            : 1;                    ///<
  } Field;                                                            ///<
  UINT32    Value;                                                    ///<
} PCIe_AER_UNCORRECTABLE_SEVERITY;

/// PCIe AER Port Configuration
typedef struct {
  UINT8                              AerEnable;                       ///< General per-port enable, 0=disable 1=enable
  UINT8                              PciBus;                          ///< PCI Bus Number
  UINT8                              PciDev;                          ///< PCI Device Number
  UINT8                              PciFunc;                         ///< PCI Function Number
  PCIe_AER_CORRECTABLE_MASK          CorrectableMask;                 ///< Per-port mask for correctable errors
  PCIe_AER_UNCORRECTABLE_MASK        UncorrectableMask;               ///< Per-port mask for uncorrectable errors
  PCIe_AER_UNCORRECTABLE_SEVERITY    UncorrectableSeverity;           ///< Per-port severity configuration for uncorrectable errors
} PCIe_PORT_AER_CONFIG;

///
/// Forward declaration for the HYGON_NBIO_PCIE_AER_PROTOCOL
///
typedef struct _HYGON_NBIO_PCIE_AER_PROTOCOL HYGON_NBIO_PCIE_AER_PROTOCOL;

//
// Protocol Definitions
//

/**
  Enable or disable Advanced Error Reporting feature
  This
    A pointer to the HYGON_NBIO_PCIE_AER_PROTOCOL instance.
  PcieAerSetting
    A pointer to store the address of the PCIe port AER config structure
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_NBIO_PCIE_AER_FEATURE)(
  IN  HYGON_NBIO_PCIE_AER_PROTOCOL  *This,
  IN  PCIe_PORT_AER_CONFIG        *PcieAerSetting
  );

///
/// The Dxe of PCIE AER Services
///
struct _HYGON_NBIO_PCIE_AER_PROTOCOL {
  HYGON_NBIO_PCIE_AER_FEATURE    SetPcieAerFeature;  ///<
};

extern EFI_GUID  gHygonNbioPcieAerProtocolGuid;

#endif
