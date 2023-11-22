/* $NoKeywords:$ */

/**
 * @file
 *
 * HblPostCode.h
 *
 * Contains definition needed for HBL Post Codes
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  PSP
 *
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
 ******************************************************************************
 */

#ifndef _HBLPOSTCODE_H_
#define _HBLPOSTCODE_H_

/// <PostCodePrefix> 0xEA000000

/**
 *  HGPI HBL Test Points
 *
 *  These are the values displayed to the user to indicate progress through boot.
 *
 */
typedef enum {
  StartProcessorTestPoints = 0xE000,                   ///< Entry used for range testing for @b Processor related TPs

  // Memory test points
  TpProcMemBeforeMemDataInit      = 0xE001,            ///< Memory structure initialization (Public interface)
  TpProcMemBeforeSpdProcessing    = 0xE002,            ///< SPD Data processing  (Public interface)
  TpProcMemHygonMemAutoPhase1     = 0xE003,            ///< Memory configuration  (Public interface) Phase 1
  TpProcMemDramInit               = 0xE004,            ///< DRAM initialization
  TpProcMemSPDChecking            = 0xE005,            ///< ProcMemSPDChecking
  TpProcMemModeChecking           = 0xE006,            ///< ProcMemModeChecking
  TpProcMemSpeedTclConfig         = 0xE007,            ///< Speed and TCL configuration
  TpProcMemSpdTiming              = 0xE008,            ///< ProcMemSpdTiming
  TpProcMemDramMapping            = 0xE009,            ///< ProcMemDramMapping
  TpProcMemPlatformSpecificConfig = 0xE00A,            ///< ProcMemPlatformSpecificConfig
  TPProcMemPhyCompensation        = 0xE00B,            ///< ProcMemPhyCompensation
  TpProcMemStartDcts              = 0xE00C,            ///< ProcMemStartDcts
  TpProcMemBeforeDramInit         = 0xE00D,            ///< ProcMemBeforeDramInit (Public interface)
  TpProcMemPhyFenceTraining       = 0xE00E,            ///< ProcMemPhyFenceTraining
  TpProcMemSynchronizeDcts        = 0xE00F,            ///< ProcMemSynchronizeDcts
  TpProcMemSystemMemoryMapping    = 0xE010,            ///< ProcMemSystemMemoryMapping
  TpProcMemMtrrConfiguration      = 0xE011,            ///< ProcMemMtrrConfiguration
  TpProcMemDramTraining           = 0xE012,            ///< ProcMemDramTraining
  TpProcMemBeforeAnyTraining      = 0xE013,            ///< ProcMemBeforeAnyTraining(Public interface)
  // PMU Test Points
  TpProcMemPmuBeforeFirmwareLoad    = 0xE014,          ///< HBL Mem - PMU - Before PMU Firmware load
  TpProcMemPmuAfterFirmwareLoad     = 0xE015,          ///< HBL Mem - PMU - After PMU Firmware load
  TpProcMemPmuPopulateSramTimings   = 0xE016,          ///< HBL Mem - PMU Populate SRAM Timing
  TpProcMemPmuPopulateSramConfig    = 0xE017,          ///< HBL Mem - PMU Populate SRAM Config
  TpProcMemPmuWriteSramMsgBlock     = 0xE018,          ///< HBL Mem - PMU Write SRAM Msg Block
  TpProcMemPmuWaitForPhyCalComplete = 0xE019,          ///< HBL Mem - Wait for Phy Cal Complete
  TpProcMemPmuPhyCalComplete        = 0xE01A,          ///< HBL Mem - Phy Cal Complete
  TpProcMemPmuStart                 = 0xE01B,          ///< HBL Mem - PMU Start
  TpProcMemPmuStarted               = 0xE01C,          ///< HBL Mem - PMU Started
  TpProcMemPmuWaitingForComplete    = 0xE01D,          ///< HBL Mem - PMU Waiting for Complete
  TpProcMemPmuStageDevInit          = 0xE01E,          ///< HBL Mem - PMU Stage Dec Init
  TpProcMemPmuStageTrainWrLvl       = 0xE01F,          ///< HBL Mem - PMU Stage Training Wr Lvl
  TpProcMemPmuStageTrainRxEn        = 0xE020,          ///< HBL Mem - PMU Stage Training Rx En
  TpProcMemPmuStageTrainRdDqs1D     = 0xE021,          ///< HBL Mem - PMU Stage Training Rd Dqs
  TpProcMemPmuStageTrainRd2D        = 0xE022,          ///< HBL Mem - PMU Stage Traning Rd 2D
  TpProcMemPmuStageTrainWr2D        = 0xE023,          ///< HBL Mem - PMU Stage Training Wr 2D
  TpProcMemPmuStagePMUQEmpty        = 0xE024,          ///< HBL Mem - PMU Queue Empty
  TpProcMemPmuUSMsgStart            = 0xE025,          ///< HBL Mem - PMU US message Start
  TpProcMemPmuUSMsgEnd              = 0xE026,          ///< HBL Mem - PMU US message End
  TpProcMemPmuComplete              = 0xE027,          ///< HBL Mem - PMU Complete
  TpProcMemAfterPmuTraining         = 0xE028,          ///< HBL Mem - PMU - After PMU Training
  TpProcMemBeforeDisablePmu         = 0xE029,          ///< HBL Mem - PMU - Before Disable PMU
  // Original Post code
  TpProcMemTransmitDqsTraining             = 0xE02A,  ///< HBL Mem - ProcMemTransmitDqsTraining
  TpProcMemTxDqStartSweep                  = 0xE02B,  ///< HBL Mem - Start write sweep
  TpProcMemTxDqSetDelay                    = 0xE02C,  ///< HBL Mem - Set Transmit DQ delay
  TpProcMemTxDqWritePattern                = 0xE02D,  ///< HBL Mem - Write test pattern
  TpProcMemTxDqReadPattern                 = 0xE02E,  ///< HBL Mem - Read Test pattern
  TpProcMemTxDqTestPattern                 = 0xE02F,  ///< HBL Mem - Compare Test pattern
  TpProcMemTxDqResults                     = 0xE030,  ///< HBL Mem - Update results
  TpProcMemTxDqFindWindow                  = 0xE031,  ///< HBL Mem - Start Find passing window
  TpProcMemMaxRdLatencyTraining            = 0xE032,  ///< HBL Mem - ProcMemMaxRdLatencyTraining
  TpProcMemMaxRdLatStartSweep              = 0xE033,  ///< HBL Mem - Start sweep
  TpProcMemMaxRdLatSetDelay                = 0xE034,  ///< HBL Mem - Set delay
  TpProcMemMaxRdLatWritePattern            = 0xE035,  ///< HBL Mem - Write test pattern
  TpProcMemMaxRdLatReadPattern             = 0xE036,  ///< HBL Mem - Read Test pattern
  TpProcMemMaxRdLatTestPattern             = 0xE037,  ///< HBL Mem - Compare Test pattern
  TpProcMemOnlineSpareInit                 = 0xE038,  ///< HBL Mem - Online Spare init
  TpProcMemChipSelectInterleaveInit        = 0xE039,  ///< HBL Mem - Chip select Interleave Init
  TpProcMemNodeInterleaveInit              = 0xE03A,  ///< HBL Mem - Node Interleave Init
  TpProcMemChannelInterleaveInit           = 0xE03B,  ///< HBL Mem - Channel Interleave Init
  TpProcMemEccInitialization               = 0xE03C,  ///< HBL Mem - ECC initialization
  TpProcMemPlatformSpecificInit            = 0xE03D,  ///< HBL Mem - Platform Specific Init
  TpProcMemBeforeHgpiReadSpd               = 0xE03E,  ///< HBL Mem - Before callout for "HgpiReadSpd"
  TpProcMemAfterHgpiReadSpd                = 0xE03F,  ///< HBL Mem - After callout for "HgpiReadSpd"
  TpProcMemBeforeHgpiHookBeforeDramInit    = 0xE040,  ///< HBL Mem - Before optional callout "HgpiHookBeforeDramInit"
  TpProcMemAfterHgpiHookBeforeDramInit     = 0xE041,  ///< HBL Mem - After optional callout "HgpiHookBeforeDramInit"
  TpProcMemBeforeHgpiHookBeforeDQSTraining = 0xE042,  ///< HBL Mem - Before optional callout "HgpiHookBeforeDQSTraining"
  TpProcMemAfterHgpiHookBeforeDQSTraining  = 0xE043,  ///< HBL Mem - After optional callout "HgpiHookBeforeDQSTraining"
  TpProcMemBeforeHgpiHookBeforeExitSelfRef = 0xE044,  ///< HBL Mem - Before optional callout "HgpiHookBeforeDramInit"
  TpProcMemAfterHgpiHookBeforeExitSelfRef  = 0xE045,  ///< HBL Mem - After optional callout "HgpiHookBeforeDramInit"
  TpProcMemAfterMemDataInit                = 0xE046,  ///< HBL Mem - After MemDataInit
  TpProcMemInitializeMCT                   = 0xE047,  ///< HBL Mem - Before InitializeMCT
  TpProcMemLvDdr3                          = 0xE048,  ///< HBL Mem - Before LV DDR3
  TpProcMemInitMCT                         = 0xE049,  ///< HBL Mem - Before InitMCT
  TpProcMemOtherTiming                     = 0xE04A,  ///< HBL Mem - Before OtherTiming
  TpProcMemUMAMemTyping                    = 0xE04B,  ///< HBL Mem - Before UMAMemTyping
  TpProcMemSetDqsEccTmgs                   = 0xE04C,  ///< HBL Mem - Before SetDqsEccTmgs
  TpProcMemMemClr                          = 0xE04D,  ///< HBL Mem - Before MemClr
  TpProcMemOnDimmThermal                   = 0xE04E,  ///< HBL Mem - Before On DIMM Thermal
  TpProcMemDmi                             = 0xE04F,  ///< HBL Mem - Before DMI
  TpProcMemEnd                             = 0xE050,  ///< HBL MEM - End of phase 3 memory code

  // CPU test points
  TpProcCpuInitAfterTrainingStart = 0xE051,            ///< Entry point CPU init after training
  TpProcCpuInitAfterTrainingEnd   = 0xE052,            ///< Exit point CPU init after training
  TpProcCpuHpobCcxMapInitStart    = 0xE053,            ///< Entry point CPU HPOB CCX map init
  TpProcCpuHpobCcxMapInitEnd      = 0xE054,            ///< Exit point CPU HPOB CCX map init
  TpProcCpuOptimizedBootStart     = 0xE055,            ///< Entry point CPU Optimized boot init
  TpProcCpuOptimizedBootEnd       = 0xE056,            ///< Exit point CPU Optimized boot init
  TpProcCpuHpobCcxEdcInitStart    = 0xE057,            ///< Entry point CPU HPOB EDC info init
  TpProcCpuHpobCcxEdcInitEnd      = 0xE058,            ///< Exit point CPU HPOB EDC info init

  // Topology test points
  TpProcTopologyEntry = 0xE071,                        ///< ProcTopologyEntry
  TpProcTopologyDone  = 0xE07C,                        ///< ProcTopologyDone

  // Extended memory test point
  TpProcMemSendMRS2                    = 0xE080,       ///< ProcMemSendMRS2
  TpProcMemSendMRS3                    = 0xE081,       ///< Sedding MRS3
  TpProcMemSendMRS1                    = 0xE082,       ///< Sending MRS1
  TpProcMemSendMRS0                    = 0xE083,       ///< Sending MRS0
  TpProcMemContinPatternGenRead        = 0xE084,       ///< Continuous Pattern Read
  TpProcMemContinPatternGenWrite       = 0xE085,       ///< Continuous Pattern Write
  TpProcMem2dRdDqsTraining             = 0xE086,       ///< Mem: 2d RdDqs Training begin
  TpProcMemBefore2dTrainExtVrefChange  = 0xE087,       ///< Mem: Before optional callout to platform BIOS to change External Vref during 2d Training
  TpProcMemAfter2dTrainExtVrefChange   = 0xE088,       ///< Mem: After optional callout to platform BIOS to change External Vref during 2d Training
  TpProcMemConfigureDCTForGeneral      = 0xE089,       ///< Configure DCT For General use begin
  TpProcMemProcConfigureDCTForTraining = 0xE08A,       ///< Configure DCT For training begin
  TpProcMemConfigureDCTNonExplicitSeq  = 0xE08B,       ///< Configure DCT For Non-Explicit
  TpProcMemSynchronizeChannels         = 0xE08C,       ///< Configure to Sync channels
  TpProcMemC6StorageAllocation         = 0xE08D,       ///< Allocate C6 Storage
  TpProcMemLvDdr4                      = 0xE08E,       ///< Before LV DDR4
  TpProcMemLvLpddr3                    = 0xE08F,       ///< Before LV DDR3

  // Gnb Earlier init
  TP0x90 = 0xE090,                                     ///< TP0x90
  TP0x91 = 0xE091,                                     ///< GNB earlier interface
  TP0x92 = 0xE092,                                     ///< GNB internal debug code
  TP0x93 = 0xE093,                                     ///< GNB internal debug code
  TP0x94 = 0xE094,                                     ///< GNB internal debug code
  TP0x95 = 0xE095,                                     ///< GNB internal debug code
  TP0x96 = 0xE096,                                     ///< GNB internal debug code
  TP0x97 = 0xE097,                                     ///< GNB internal debug code
  TP0x98 = 0xE098,                                     ///< GNB internal debug code
  TP0x99 = 0xE099,                                     ///< GNB internal debug code
  TP0x9A = 0xE09A,                                     ///< GNB internal debug code
  TP0x9B = 0xE09B,                                     ///< GNB internal debug code
  TP0x9C = 0xE09C,                                     ///< GNB internal debug code
  TP0x9D = 0xE09D,                                     ///< GNB internal debug code
  TP0x9E = 0xE09E,                                     ///< GNB internal debug code
  TP0x9F = 0xE09F,                                     ///< GNB internal debug code
  TP0xA0 = 0xE0A0,                                     ///< TP0xA0
  TP0xA1 = 0xE0A1,                                     ///< GNB internal debug code
  TP0xA2 = 0xE0A2,                                     ///< GNB internal debug code
  TP0xA3 = 0xE0A3,                                     ///< GNB internal debug code
  TP0xA4 = 0xE0A4,                                     ///< GNB internal debug code
  TP0xA5 = 0xE0A5,                                     ///< GNB internal debug code
  TP0xA6 = 0xE0A6,                                     ///< GNB internal debug code
  TP0xA7 = 0xE0A7,                                     ///< GNB internal debug code
  TP0xA8 = 0xE0A8,                                     ///< GNB internal debug code
  TP0xA9 = 0xE0A9,                                     ///< GNB internal debug code
  TP0xAA = 0xE0AA,                                     ///< GNB internal debug code
  TP0xAB = 0xE0AB,                                     ///< GNB internal debug code
  TP0xAC = 0xE0AC,                                     ///< GNB internal debug code
  TP0xAD = 0xE0AD,                                     ///< GNB internal debug code
  TP0xAE = 0xE0AE,                                     ///< GNB internal debug code
  TP0xAF = 0xE0AF,                                     ///< GNB internal debug code

  TpHbl1Begin                          = 0xE0B0,       ///< Hbl1Begin
  TpHbl1Initialization                 = 0xE0B1,       ///< HBL 1 Initialization
  TpHbl1DfEarly                        = 0xE0B2,       ///< HBL 1 DF Early
  TpHbl1DfPreTraining                  = 0xE0B3,       ///< HBL 1 DF Pre Training
  TpHbl1DebugSync                      = 0xE0B4,       ///< HBL 1 Debug Synchronization
  TpHbl1ErrorDetected                  = 0xE0B5,       ///< HBL 1 Error Detected
  TpHbl1GlobalMemErrorDetected         = 0xE0B6,       ///< HBL 1 Global memory error detected
  TpHbl1End                            = 0xE0B7,       ///< HBL 1 End
  TpHbl2Begin                          = 0xE0B8,       ///< HBL 2 Begin
  TpHbl2Initialization                 = 0xE0B9,       ///< HBL 2 Initialization
  TpHbl2DfAfterTraining                = 0xE0BA,       ///< HBL 2 After Training
  TpHbl2DebugSync                      = 0xE0BB,       ///< HBL 2 Debug Synchronization
  TpHbl2ErrorDetected                  = 0xE0BC,       ///< HBL 2 Error detected
  TpHbl2GlobalMemErrorDetected         = 0xE0BD,       ///< HBL 2 Global memory error detected
  TpHbl2End                            = 0xE0BE,       ///< HBL 2 End
  TpHbl3Begin                          = 0xE0BF,       ///< HBL 3 Begin
  TpHbl3Initialization                 = 0xE0C0,       ///< HBL 3 Initialziation
  TpHbl3HmiGopInitStage1               = 0xE1C0,       ///< HBL 3 HMI/xHMI Initialization Stage 1
  TpHbl3HmiGopInitStage1Warning        = 0xB1C0,       ///< HBL 3 HMI/xHMI Initialization Stage 1 Warning
  TpHbl3HmiGopInitState1Error          = 0xF1C0,       ///< HBL 3 HMI/xHMI Initialization Stage 2 Error
  TpHbl3HmiGopInitStage2               = 0xE2C0,       ///< HBL 3 HMI/xHMI Initialization Stage 2
  TpHbl3HmiGopInitStage2Warning        = 0xB2C0,       ///< HBL 3 HMI/xHMI Initialization Stage 2 Warning
  TpHbl3HmiGopInitState2Error          = 0xF2C0,       ///< HBL 3 HMI/xHMI Initialization Stage 2 Error
  TpHbl3HmiGopInitStage3               = 0xE3C0,       ///< HBL 3 HMI/xHMI Initialization Stage 3
  TpHbl3HmiGopInitStage3Warning        = 0xB3C0,       ///< HBL 3 HMI/xHMI Initialization Stage 3 Warning
  TpHbl3HmiGopInitState3Error          = 0xF3C0,       ///< HBL 3 HMI/xHMI Initialization Stage 3 Error
  TpHbl3HmiGopInitStage4               = 0xE4C0,       ///< HBL 3 HMI/xHMI Initialization Stage 4
  TpHbl3HmiGopInitStage4Warning        = 0xB4C0,       ///< HBL 3 HMI/xHMI Initialization Stage 4 Warning
  TpHbl3HmiGopInitState4Error          = 0xF4C0,       ///< HBL 3 HMI/xHMI Initialization Stage 4 Error
  TpHbl3HmiGopInitStage5               = 0xE5C0,       ///< HBL 3 HMI/xHMI Initialization Stage 5
  TpHbl3HmiGopInitStage5Warning        = 0xB5C0,       ///< HBL 3 HMI/xHMI Initialization Stage 5 Warning
  TpHbl3HmiGopInitState5Error          = 0xF5C0,       ///< HBL 3 HMI/xHMI Initialization Stage 5 Error
  TpHbl3HmiGopInitStage6               = 0xE6C0,       ///< HBL 3 HMI/xHMI Initialization Stage 6
  TpHbl3HmiGopInitStage6Warning        = 0xB6C0,       ///< HBL 3 HMI/xHMI Initialization Stage 6 Warning
  TpHbl3HmiGopInitState6Error          = 0xF6C0,       ///< HBL 3 HMI/xHMI Initialization Stage 6 Error
  TpHbl3HmiGopInitStage7               = 0xE7C0,       ///< HBL 3 HMI/xHMI Initialization Stage 7
  TpHbl3HmiGopInitStage8               = 0xE8C0,       ///< HBL 3 HMI/xHMI Initialization Stage 8
  TpHbl3HmiGopInitStage9               = 0xE9C0,       ///< HBL 3 HMI/xHMI Initialization Stage 9
  TpHbl3HmiGopInitStage9Error          = 0xF9C0,       ///< HBL 3 HMI/xHMI Initialization Stage 9 Error
  TpHbl3HmiGopInitStage10              = 0xEAC0,       ///< HBL 3 HMI/xHMI Initialization Stage 10
  TpHbl3HmiGopInitStage10Error         = 0xFAC0,       ///< HBL 3 HMI/xHMI Initialization Stage 10 Error
  TpHbl3ProgramUmcKeys                 = 0xE0C1,       ///< Hbl3ProgramUmcKeys
  TpHbl3DfFinalInitialization          = 0xE0C2,       ///< HBL 3 DF Finial Initalization
  TpHbl3ExecuteSyncFunction            = 0xE0C3,       ///< HBL 3 Execute Synchronization Function
  TpHbl3DebugSync                      = 0xE0C4,       ///< HBL 3 Debug Synchronization Function
  TpHbl3ErrorDetected                  = 0xE0C5,       ///< HBL 3 Error Detected
  TpHbl3GlobalMemErrorDetected         = 0xE0C6,       ///< HBL 3 Global memroy error detected
  TpHbl4ColdInitialization             = 0xE0C7,       ///< HBL 4 Initialiation - cold boot
  TpHbl4MemTest                        = 0xE0C8,       ///< HBL 4 Memory test - cold boot
  TpHbl4Hpob                           = 0xE0C9,       ///< HBL 4 HPOB Initialzation - cold boot
  TpHbl4Finalize                       = 0xE0CA,       ///< HBL 4 Finalize memory settings - cold boot
  TpHbl4CpuInizialOptimizedBoot        = 0xE0CB,       ///< HBL 4 CPU Initialize Optimized Boot - cold boot
  TpHbl4HmicieTraining                 = 0xE0CC,       ///< HBL 4 Hmi Pcie Training - cold boot
  TpHbl4ColdEnd                        = 0xE0CD,       ///< HBL 4 Cold boot End
  TpHbl4ResumeInitialization           = 0xE0CE,       ///< HBL 4 Initialization - Resume boot
  TpHbl4ResumeEnd                      = 0xE0CF,       ///< HBL 4 Resume End
  TpHbl4End                            = 0xE0D0,       ///< HBL 4 End Cold/Resume boot
  TpProcMemHygonMemAutoPhase2          = 0xE0D1,       ///< HBL 2 memory initialization
  TpProcMemHygonMemAutoPhase3          = 0xE0D2,       ///< HBL 3 memory initialization
  TpHbl3End                            = 0xE0D3,       ///< HBL 3 End
  TpHbl1EnterMemFlow                   = 0xE0D4,       ///< HBL 1 Enter Memory Flow
  TpHbl1MemFlowMemClkSync              = 0xE0D5,       ///< Memorry flow memory clock synchronization
  TpIfHygonReadEventLogEntry           = 0xE0D6,       ///< IfHygonReadEventLogEntry
  TpIfHygonReadEventLogExit            = 0xE0D7,       ///< Exiting from HygonReadEventLog
  TpIfHygonGetApicIdEntry              = 0xE0D8,       ///< Entry to HygonGetApicId
  TpIfHygonGetApicIdExit               = 0xE0D9,       ///< Exiting from HygonGetApicId
  TpIfHygonGetPciAddressEntry          = 0xE0DA,       ///< Entry to HygonGetPciAddress
  TpIfHygonGetPciAddressExit           = 0xE0DB,       ///< Exiting from HygonGetPciAddress
  TpIfHygonIdentifyCoreEntry           = 0xE0DC,       ///< Entry to HygonIdentifyCore
  TpIfHygonIdentifyCoreExit            = 0xE0DD,       ///< Exiting from HygonIdentifyCore
  TpIfBeforeRunApFromIds               = 0xE0DE,       ///< After IDS calls out to run code on an AP
  TpIfAfterRunApFromIds                = 0xE0DF,       ///< After IDS calls out to run code on an AP
  TpIfBeforeGetIdsData                 = 0xE0E0,       ///< Before IDS calls out to get IDS data
  TpIfAfterGetIdsData                  = 0xE0E1,       ///< After IDS calls out to get IDS data
  TpIfBeforeAllocateHeapBuffer         = 0xE0E2,       ///< Before the heap manager calls out to allocate a buffer
  TpIfAfterAllocateHeapBuffer          = 0xE0E3,       ///< After the heap manager calls out to allocate a buffer
  TpIfBeforeDeallocateHeapBuffer       = 0xE0E4,       ///< Before the heap manager calls out to deallocate a buffer
  TpIfAfterDeallocateHeapBuffer        = 0xE0E5,       ///< After the heap manager calls out to deallocate a buffer
  TpIfBeforeLocateHeapBuffer           = 0xE0E6,       ///< Before the heap manager calls out to locate a buffer
  TpIfAfterLocateHeapBuffer            = 0xE0E7,       ///< After the heap manager calls out to locate a buffer
  TpHbl1MemFlowPStateSync              = 0xE0E8,       ///< Memory flow P-State synchronization
  TpIfAfterRunApFromAllAps             = 0xE0E9,       ///< After the BSP calls out to run code on an AP
  TpIfBeforeRunApFromAllCore0s         = 0xE0EA,       ///< Before the BSP calls out to run code on an AP
  TpIfAfterRunApFromAllCore0s          = 0xE0EB,       ///< After the BSP calls out to run code on an AP
  TpIfBeforeAllocateS3SaveBuffer       = 0xE0EC,       ///< Before the S3 save code calls out to allocate a buffer
  TpIfAfterAllocateS3SaveBuffer        = 0xE0ED,       ///< After the S3 save code calls out to allocate a buffer
  TpIfBeforeAllocateMemoryS3SaveBuffer = 0xE0EE,       ///< Before the memory S3 save code calls out to allocate a buffer
  TpIfAfterAllocateMemoryS3SaveBuffer  = 0xE0EF,       ///< After the memory S3 save code calls out to allocate a buffer
  TpIfBeforeLocateS3PciBuffer          = 0xE0F0,       ///< Before the memory code calls out to locate a buffer
  TpIfAfterLocateS3PciBuffer           = 0xE0F1,       ///< After the memory code calls out to locate a buffer
  TpIfBeforeLocateS3CPciBuffer         = 0xE0F2,       ///< Before the memory code calls out to locate a buffer
  TpIfAfterLocateS3CPciBuffer          = 0xE0F3,       ///< After the memory code calls out to locate a buffer
  TpIfBeforeLocateS3MsrBuffer          = 0xE0F4,       ///< Before the memory code calls out to locate a buffer
  TpIfAfterLocateS3MsrBuffer           = 0xE0F5,       ///< After the memory code calls out to locate a buffer
  TpIfBeforeLocateS3CMsrBuffer         = 0xE0F6,       ///< Before the memory code calls out to locate a buffer
  TpIfAfterLocateS3CMsrBuffer          = 0xE0F7,       ///< After the memory code calls out to locate a buffer
  TpReadyToBoot                        = 0xE0F8,       ///< Ready to boot event
  // PMU test points
  TpProcMemPmuFailed = 0xE0F9,                         ///< Failed PMU training.
  TpProcMemPhase1End = 0xE0FA,                         ///< End of phase 1 memory code
  TpProcMemPhase2End = 0xE0FB,                         ///< End of phase 2 memory code

  // HBL0 test points
  TpHbl0Begin = 0xE0FC,                                ///< Hbl0Begin
  TpHbl0End   = 0xE0FD,                                ///< HBL 0 End

  // HBL5 test points
  TpHbl5End                    = 0xE100,               ///< HBL 5 End
  TpHbl5ResumeInitialization   = 0xE101,               ///< HBL 5 Resume boot
  TpHbl6End                    = 0xE102,               ///< HBL 6 End
  TpHbl6Initialization         = 0xE103,               ///< HBL 6 Initialization
  TpProcMemPhase1bEnd          = 0xE104,               ///< End of phase 1b memory code
  TpProcMemHygonMemAutoPhase1b = 0xE105,               ///< HBL 1b memory initialization
  TpHbl6GlobalMemErrorDetected = 0xE106,               ///< HBL 6 Global memroy error detected
  TpHbl1bDebugSync             = 0xE107,               ///< HBL 1b Debug Synchronization Function
  TpHbl4bDebugSync             = 0xE108,               ///< HBL 4b Debug Synchronization Function
  TpHbl1bBegin                 = 0xE109,               ///< HblbBegin
  TpHbl4bBegin                 = 0xE10A,               ///< Ab4bBegin

  TpProcHpobHmacFailOnS3                             = 0xE10B,  ///< BSP encountered HMAC fail on HPOB Header
  TpHblErroGeneralAssert                             = 0xE2A0,  ///< HBL Eroor General ASSERT
  TpHblErrorUnknown                                  = 0xE2A1,  ///< Unknown Error
  TpHblErrorLogInitError                             = 0xE2A3,  ///< HBL Error Log Inig Error
  TpHblErrorOdtHeap                                  = 0xE2A4,  ///< HBL Error for On DIMM thermal Heap allocation error
  TpHblErrorMemoryTest                               = 0xE2A5,  ///< HBL Error for memory test error
  TpHblErrorExecutingMemoryTest                      = 0xE2A6,  ///< HBL Error while executing memory test error
  TpHblErrorDpprMemAutoHeapAlocError                 = 0xE2A7,  ///< HBL Error DDR Post Packge Repair Mem Auto Heap Alloc error
  TpHblErrorDpprNoHpobHeapAlocError                  = 0xE2A8,  ///< HBL Error for DDR Post Package repair Hpob Heap Alloc error
  TpHblErrorDpprNoPprTblHeapAlocError                = 0xE2A9,  ///< HBL Error for DDR Post Package Repair No PPR Table Heap Aloc error
  TpHblErrorEccMemAutoHeapAlocError                  = 0xE2AA,  ///< HBL Error for Ecc Mem Auto Aloc Error error
  TpHblErrorSocScanHeapAlocError                     = 0xE2AB,  ///< HBL Error for Soc Scan Heap Aloc error
  TpHblErrorSocScanNoDieError                        = 0xE2AC,  ///< HBL Error for Soc Scan No Die error
  TpHblErrorNbTecHeapAlocError                       = 0xE2AD,  ///< HBL Error for Nb Tech Heap Aloc error
  TpHblErrorNoNbConstError                           = 0xE2AE,  ///< HBL Error for No Nb Constructor error
  TpHblErrorNoTechConstError                         = 0xE2B0,  ///< HBL Error for No Tech Constructor error
  TpHblErrorHbl1bAutoAloc                            = 0xE2B1,  ///< HBL Error for HBL1b Auto Alocation error
  TpHblErrorHbl1bNoNbConst                           = 0xE2B2,  ///< HBL Error for HBL1b No NB Constructor error
  TpHblErrorHbl2NoNbConst                            = 0xE2B3,  ///< HBL Error for HBL2 No Nb Constructor error
  TpHblErrorHbl3AutoAloc                             = 0xE2B4,  ///< HBL Error for HBL3 Auto Allocation error
  TpHblErrorHbl3NoNbConst                            = 0xE2B5,  ///< HBL Error for HBL3 No Nb Constructor error
  TpHblErrorHbl1bGen                                 = 0xE2B6,  ///< HBL Error for HBL1b General error
  TpHblErrorHbl2Gen                                  = 0xE2B7,  ///< HBL Error for HBL2 General error
  TpHblErrorHbl3Gen                                  = 0xE2B8,  ///< HBL Error for HBL3 General error
  TpHblErrorGetTargetSpeed                           = 0xE2B9,  ///< HBL Error for Get Target Speed error
  TpHblErrorFlowP1FamilySupport                      = 0xE2BA,  ///< HBL Error for Flow P1 Family Support error
  TpHblErrorNoValidDdr4Dimms                         = 0xE2BB,  ///< HBL Error for No Valid Ddr4 Dimms error
  TpHblErrorNoDimmPresent                            = 0xE2BC,  ///< HBL Error for No Dimm Present error
  TpHblErrorFlowP2FamilySupport                      = 0xE2BD,  ///< HBL Error for Flow P2 Family Supprot error
  TpHblErrorHeapDealocForPmuSramMsgBlock             = 0xE2BE,  ///< HBL Error for Heap Deallocation for PMU Sram Msg Block error
  TpHblErrorDdrRecovery                              = 0xE2BF,  ///< HBL Error for DDR Recovery error
  TpHblErrorRrwTest                                  = 0xEBC0,  ///< HBL Error for RRW Test error
  TpHblErrorOdtInit                                  = 0xE2C1,  ///< HBL Error for On Die Thermal error
  TpHblErrorHeapAllocForDctStructAndChDefStruct      = 0xE2C2,  ///< HBL Error for Heap Allocation For Dct Struct Hygon Ch Def structure error
  TpHblErrorHeapAlocForPmuSramMsgBlock               = 0xE2C3,  ///< HBL Error for Heap Allocation for PMU SRAM Msg block error
  TpHblErrorHeapPhyPllLockFailure                    = 0xE2C4,  ///< HBL Error for Heap Phy PLL lock Flure error
  TpHblErrorPmuTraining                              = 0xE2C5,  ///< HBL Error for Pmu Training error
  TpHblErrorFailureToLoadOrVerifyPmuFw               = 0xE2C6,  ///< HBL Error for Failure to Load or Verify PMU FW error
  TpHblErrorAllocateForPmuSramMsgBlockNoInit         = 0xE2C7,  ///< HBL Error for Allocate for PMU SRAM Msg Block No Init error
  TpHblErrorFailureBiosPmuFwMismatchHgpiPmuFwVersion = 0xE2C8,  ///< HBL Error for Failure BIOS PMU FW Mismatch HGPI PMU FW version error
  TpHblErrorDeallocateForPmuSramMsgBlock             = 0xE2CA,  ///< HBL Error for Deallocate for PMU SRAM Msg Block error
  TpHblErrorModuleTypeMismatchRDimm                  = 0xE2CB,  ///< HBL Error for Module Type Mismatch RDIMM error
  TpHblErrorModuleTypeMismatchLRDimm                 = 0xE2CC,  ///< HBL Error for Module type Mismatch LRDIMM error
  TpHblErrorMemAutoNvdimm                            = 0xE2CD,  ///< HBL Error for MEm Auto NVDIM error
  TpHblErrorUnknownResponse                          = 0xE2CE,  ///< HBL Error for Unknowm Responce error
  TpHblErrorMemOverclockErrorRrwTestResults          = 0xE2CF,  ///< HBL Error for Over Clock Error RRW Test Results Error
  TpHblErrorOverClockErrorPmuTraining                = 0xE2D0,  ///< HBL Error for Over Clock Error PMU Training Error
  TpHblErrorHbl1GenError                             = 0xE2D1,  ///< HBL Error for HBL1 General Error
  TpHblErrorHbl2GenError                             = 0xE2D2,  ///< HBL Error for HBL2 General Error
  TpHblErrorHbl3GenError                             = 0xE2D3,  ///< HBL Error for HBL3 General Error
  TpHblErrorHbl5GenError                             = 0xE2D4,  ///< HBL Error for HBL4 General Error
  TpHblErrorOverClockMemInit                         = 0xE2D5,  ///< HBL Error over clock Mem Init Error
  TpHblErrorOverClockMemOther                        = 0xE2D6,  ///< HBL Error over clock Mem Other Error
  TpHblErrorHbl6GenError                             = 0xE2D7,  ///< HBL Error for HBL6 General Error
  TpEventLogInit                                     = 0xE2D8,  ///< HBL Error Event Log Error
  TpHblErrorHbl1FatalError                           = 0xE2D9,  ///< HBL Error FATAL HBL1 Log Error
  TpHblErrorHbl2FatalError                           = 0xE2DA,  ///< HBL Error FATAL HBL2 Log Error
  TpHblErrorHbl3FatalError                           = 0xE2DB,  ///< HBL Error FATAL HBL3 Log Error
  TpHblErrorHbl4FatalError                           = 0xE2DC,  ///< HBL Error FATAL HBL4 Log Error
  TpHblErrorSlaveSyncFunctionExecutionError          = 0xE2DD,  ///< HBL Error Slave Sync function execution Error
  TpHblErrorSlaveSyncCommWithDataSentToMasterError   = 0xE2DE,  ///< HBL Error Slave Sync communicaton with data set to master Error
  TpHblErrorSlaveBroadcastCommFromMasterToSlaveError = 0xE2DF,  ///< HBL Error Slave broadcast communication from master to slave Error
  TpHblErrorHbl6FatalError                           = 0xE2E0,  ///< HBL Error FATAL HBL6 Log Error
  TpHblErrorSlaveOfflineMsgError                     = 0xE2E1,  ///< HBL Error Slave Offline Error
  TpHblErrorSlaveInformsMasterErrorInoError          = 0xE2E2,  ///< HBL Error Slave Informs Master Error Info Error
  TpHblErrorHeapLocateForPmuSramMsgBlock             = 0xE2E3,  ///< HBL Error Error Heap Locate for PMU SRAM Msg Block Error
  TpHblErrorHbl2AutoAloc                             = 0xE2E4,  ///< HBL Error HBL2 Auto Error
  TpHblErrorFlowP3FamilySupport                      = 0xE2E5,  ///< HBL Error Flow P3 Family support Error
  TpHblErrorHbl4GenError                             = 0xE2E5,  ///< HBL Error Hbl 4 Gen Error
  TpHblErrorMbistHeapAlloc                           = 0xE2EB,  ///< HBL Error MBIST Heap Allocation Error
  TpHblErrorMbistResultsError                        = 0xE2EC,  ///< HBL Error MBIST Results Error
  TpHblErrorNoDimmSmbusInfoError                     = 0xE2ED,  ///< HBL Error NO Dimm Smcus Info Error
  TpHblErrorPorMaxFreqTblError                       = 0xE2EE,  ///< HBL Error Por Max Freq Table Error
  TpHblErrorUnsupportedDimmConfuglError              = 0xE2EF,  ///< HBL Error Unsupproted DIMM Config Error
  TpHblErrorNoPsTableError                           = 0xE2F0,  ///< HBL Error No Ps Table Error
  TpHblErrorCadBusTmgNoFoundError                    = 0xE2F1,  ///< HBL Error Cad Bus Timing Not Found Error
  TpHblErrorDataBusTmgNoFoundError                   = 0xE2F2,  ///< HBL Error Data Bus Timing Not Found Error
  TpHblErrorLrIbtNotFoundError                       = 0xE2F3,  ///< HBL Error LrDIMM IBT Not Found Error
  TpHblErrorUnsupportedDimmConfigMaxFreqError        = 0xE2F4,  ///< HBL Error Unsupprote Dimm Config Max Freq Error Error
  TpHblErrorMr0NotFoundError                         = 0xE2F5,  ///< HBL Error Mr0 Not Found Error
  TpHblErrorOdtPAtNotFoundError                      = 0xE2F6,  ///< HBL Error Obt Pattern Not found Error
  TpHblErrorRc10OpSpeedNotFoundError                 = 0xE2F7,  ///< HBL Error Rc10 Op Speed Not FOund Error
  TpHblErrorRc2IbtNotFoundError                      = 0xE2F8,  ///< HBL Error Rc2 Ibt Not Found Error
  TpHblErrorRttNotFoundError                         = 0xE2F9,  ///< HBL Error Rtt Not Found Error
  TpHblErrorChecksumReStrtError                      = 0xE2FA,  ///< HBL Error Checksum ReStrt Results Error
  TpHblErrorNoChipselectError                        = 0xE2FB,  ///< HBL Error No Chipselect Results Error
  TpHblErrorNoCommonCasLAtError                      = 0xE2FC,  ///< HBL Error No Common Cas Latency Results Error
  TpHblErrorCasLatXceedsTaaMaxError                  = 0xE2FD,  ///< HBL Error Cas Latecncy exceeds Taa Max Error
  TpHblErrorNvdimmArmMissmatcPowerPolicyError        = 0xE2FE,  ///< HBL Error Nvdimm Arm Missmatch Power Policy Error
  TpHblErrorNvdimmArmMissmatchPowerSouceError        = 0xE2FF,  ///< HBL Error Nvdimm Arm Missmatch Power Source Error
  TpHblErrorHbl1MemInitError                         = 0xE300,  ///< HBL Error HBL 1 Mem Init Error
  TpHblErrorHbl2MemInitError                         = 0xE301,  ///< HBL Error HBL 2 Mem Init Error
  TpHblErrorHbl4MemInitError                         = 0xE302,  ///< HBL Error HBL 4 Mem Init Error
  TpHblErrorHbl6MemInitError                         = 0xE303,  ///< HBL Error HBL 6 Mem Init Error
  TpHblErrorHbl1ErrorReportError                     = 0xE304,  ///< HBL Error HBL 1 error repor Error
  TpHblErrorHbl2ErrorReportError                     = 0xE305,  ///< HBL Error HBL 2 error repor Error
  TpHblErrorHbl3ErrorReportError                     = 0xE306,  ///< HBL Error HBL 3 error repor Error
  TpHblErrorHbl4ErrorReportError                     = 0xE307,  ///< HBL Error HBL 4 error repor Error
  TpHblErrorHbl6ErrorReportError                     = 0xE308,  ///< HBL Error HBL 6 error repor Error
  TpHblErrorMsgSlaveSyncFunctionExecutionError       = 0xE30A,  ///< HBL Error message slave sync function execution Error
  TpHblErrorSlaveOfflineError                        = 0xE30B,  ///< HBL Error slave offline Error
  TpHblErrorSyncMasterError                          = 0xE30C,  ///< HBL Error Sync Master Error
  TpHblErrorSlaveInformsMasterInfoMsgError           = 0xE30D,  ///< HBL Error Slave Informs Master Info Message Error
  TpHblErrorGenAssertError                           = 0xE30F,  ///< HBL Error General Assert Error
  TpHblErrorNoDimmOnAnyChannelInSystem               = 0xE310,  ///< HBL Error No Dimms On Any Channel in sysem
  TpHblMemAlertPmuMajorMsg                           = 0xE311,  ///< HBL Alert PMU Major Message captured
  TpHblMemAlertPmuResultsRxTmg                       = 0xE312,  ///< HBL Alert PMU REsults Rx Timing captured
  TpHblMemAlertPmuResultsTxTmg                       = 0xE313,  ///< HBL Alert PMU REsults Tx Timing captured
  TpHblMemAlertPmuResultsRxVref                      = 0xE314,  ///< HBL Alert PMU REsults Rx Vref captured
  TpHblMemAlertPmuResultsTxVref                      = 0xE315,  ///< HBL Alert PMU REsults Tx Vref captured
  TpHblErrorNoMemoryAvailableInSystem                = 0xE316,  ///< HBL Error No memory available in system
  TpHblHpcbLoadError                                 = 0xEACB,  ///< HBL Alert HPCB Failed to Load
  EndHgpiTps                                         = 0xEFFF,  ///< EndHgpis
} HGPI_TP;

#endif
