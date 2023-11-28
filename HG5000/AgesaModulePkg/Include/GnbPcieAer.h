/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe AER definitions.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: NBIO
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
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

#ifndef _GNB_PCIE_AER_H_
#define _GNB_PCIE_AER_H_

#pragma pack (push, 1)
/// PCIe Correctable Error Mask
typedef union {
  struct {                                                            ///<
    UINT32                  Reserved_5_0 :6;                          ///<
    UINT32                  BadTLPMask :1;                            ///< [6] BadTlpMask
    UINT32                  BadDLLPMask :1;                           ///< [7] BadDllpMask
    UINT32                  ReplayNumberRolloverMask :1;              ///< [8] ReplayNumRolloverMask
    UINT32                  Reserved_11_9 :3;                         ///<
    UINT32                  ReplayTimerTimeoutMask :1;                ///< [12] ReplayTimerTimeoutMask
    UINT32                  AdvisoryNonFatalErrorMask :1;             ///< [13] AdvisoryNonfatalErrMask
    UINT32                  Reserved_31_14 :18;                       ///<
  } Field;                                                            ///<
  UINT32 Value;                                                       ///<
} NBIO_PCIe_AER_CORRECTABLE_MASK;

/// PCIe Unorrectable Error Mask
typedef union {
  struct {                                                            ///<
    UINT32                  Reserved_3_0 :4;                          ///<
    UINT32                  DataLinkProtocolErrorMask :1;             ///< [4] DlpErrMask
    UINT32                  Reserved_11_5 :7;                         ///<
    UINT32                  PoisonedTLPMask :1;                       ///< [12] PsnErrMask
    UINT32                  Reserved_13_13 :1;                        ///<
    UINT32                  CompletionTimeoutMask :1;                 ///< [14] CplTimeoutMask
    UINT32                  CompleterAbortMask :1;                    ///< [15] CplAbortErrMask
    UINT32                  UnexpectedCompletionMask :1;              ///< [16] UnexpCplMask
    UINT32                  Reserved_17_17:1 ;                        ///<
    UINT32                  MalTlpMask:1 ;                            ///< [18] MalTlpMask
    UINT32                  ECRCErrorMask :1;                         ///< [19] EcrcErrMask
    UINT32                  UnsupportedRequestErrorMask :1;           ///< [20] UnsuppReqErrMask
    UINT32                  AcsViolationMask:1 ;                      ///< [21] AcsViolationMask
    UINT32                  Reserved_31_22 :10;                       ///<
  } Field;                                                            ///<
  UINT32 Value;                                                       ///<
} NBIO_PCIe_AER_UNCORRECTABLE_MASK;

/// PCIe Unorrectable Error Severity
typedef union {
  struct {                                                            ///<
    UINT32                  Reserved_3_0 :4;                          ///< [3:0]
    UINT32                  DataLinkProtocolErrorSeverity :1;         ///< [4] DlpErrSeverity
    UINT32                  Reserved_11_5 :7;                         ///< [11:5]
    UINT32                  PoisonedTLPSeverity :1;                   ///< [12] PsnErrSeverity
    UINT32                  Reserved_13_13 :1;                        ///< [13]
    UINT32                  CompletionTimeoutSeverity :1;             ///< [14] CplTimeoutSeverity
    UINT32                  CompleterAbortSeverity :1;                ///< [15] CplAbortErrSeverity
    UINT32                  UnexpectedCompletionSeverity :1;          ///< [16] UnexpCplSeverity
    UINT32                  Reserved_17_17:1 ;                        ///< [17]
    UINT32                  MalTlpSeverity:1 ;                        ///< [18] MalTlpSeverity
    UINT32                  ECRCErrorSeverity :1;                     ///< [19] EcrcErrSeverity
    UINT32                  UnsupportedRequestErrorSeverity :1;       ///< [20] UnsuppReqErrSeverity
    UINT32                  AcsViolationSeverity:1 ;                  ///< [21] AcsViolationSeverity
    UINT32                  Reserved_31_22 :10;                       ///< [31:22]
  } Field;                                                            ///<
  UINT32 Value;                                                       ///<
} NBIO_PCIe_AER_UNCORRECTABLE_SEVERITY;
#pragma pack (pop)

/// PCIe AER Port Configuration
typedef struct {
  UINT8                                PortAerEnable;                 ///< General per-port enable
  UINT8                                PciDev;                        ///< PCI Device Number
  UINT8                                PciFunc;                       ///< PCI Function Number
  NBIO_PCIe_AER_CORRECTABLE_MASK       CorrectableMask;               ///< Per-port mask for correctable errors
  NBIO_PCIe_AER_UNCORRECTABLE_MASK     UncorrectableMask;             ///< Per-port mask for uncorrectable errors
  NBIO_PCIe_AER_UNCORRECTABLE_SEVERITY UncorrectableSeverity;         ///< Per-port severity configuration for uncorrectable errors
} NBIO_PCIe_PORT_AER_CONFIG;

/// PCIe AER Configuration
typedef struct {
  UINT8                      NumberOfPorts;                           ///< The amount of ports of GNB
  NBIO_PCIe_PORT_AER_CONFIG  PortAerConfig[];                         ///< Per-port AER configuration
} AMD_NBIO_PCIe_AER_CONFIG;

#endif
