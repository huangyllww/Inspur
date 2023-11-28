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


#include "AmdCbsVariable.h"
#include "token.h"

VOID
CbsWriteDefalutValue (
  IN UINT8 *IfrData
  )
{
  CBS_CONFIG *Setup_Config;

  Setup_Config = (CBS_CONFIG *)IfrData;

  Setup_Config->CbsComboFlag = 2;                                       // Combo CBS
  Setup_Config->CbsCmnCpuGenWA05 = 1;                                   // RedirectForReturnDis
  Setup_Config->CbsCmnCpuL2TlbWayAlloc = 3;                             // L2 TLB Associativity
  //BYO Modify for RAS >>
  Setup_Config->CbsCmnCpuPfeh = 3;                                      // Platform First Error Handling
  //BYO Modify for RAS <<
  Setup_Config->CbsCmnCpuCpb = 1;                                       // Core Performance Boost
  Setup_Config->CbsCmnCpuEnIbs = 3;                                     // Enable IBS
  Setup_Config->CbsCmnCpuGlobalCstateCtrl = 0;                          // Global C-state Control
  Setup_Config->CbsCmnCpuOpcacheCtrl = 3;                               // Opcache Control
  Setup_Config->CbsCmnCpuOcMode = 0;                                    // OC Mode
  Setup_Config->CbsCmnCpuSevAsidSpaceLimit = 5;                         // SEV-ES ASID Space Limit
  Setup_Config->CbsCmnCpuStreamingStoresCtrl = 3;                       // Streaming Stores Control
  Setup_Config->CbsCmnCpuLoadUcodeCtrl = 1;                             // Enable CPU load ucode Feature, <PS-315+>
  Setup_Config->CbsCmnAvailableVqCount=0;
  Setup_Config->CbsCmnCpuRdseedRdrandCap = 1;                           // Display RdseedRdrand configuration item in CBS, <PS-625+>
  Setup_Config->CbsCpuLegalDisclaimer = 0;                              // Custom Core Pstates Legal Disclaimer
  Setup_Config->CbsCpuLegalDisclaimer1 = 0;                             // Custom Core Pstates Legal Disclaimer 1
  Setup_Config->CbsCpuPstCustomP0 = 2;                                  // Custom Pstate0
  Setup_Config->CbsCpuCofP0 = 0;                                        //   Frequency (MHz)
  Setup_Config->CbsCpuVoltageP0 = 0;                                    //   Voltage (uV)
  Setup_Config->CbsCpuPst0Fid = 16;                                     //   Pstate0 FID
  Setup_Config->CbsCpuPst0Did = 8;                                      //   Pstate0 DID
  Setup_Config->CbsCpuPst0Vid = 255;                                    //   Pstate0 VID
  Setup_Config->CbsCpuPstCustomP1 = 2;                                  // Custom Pstate1
  Setup_Config->CbsCpuCofP1 = 0;                                        //   Frequency (MHz)
  Setup_Config->CbsCpuVoltageP1 = 0;                                    //   Voltage (uV)
  Setup_Config->CbsCpuPst1Fid = 16;                                     //   Pstate1 FID
  Setup_Config->CbsCpuPst1Did = 8;                                      //   Pstate1 DID
  Setup_Config->CbsCpuPst1Vid = 255;                                    //   Pstate1 VID
  Setup_Config->CbsCpuPstCustomP2 = 2;                                  // Custom Pstate2
  Setup_Config->CbsCpuCofP2 = 0;                                        //   Frequency (MHz)
  Setup_Config->CbsCpuVoltageP2 = 0;                                    //   Voltage (uV)
  Setup_Config->CbsCpuPst2Fid = 16;                                     //   Pstate2 FID
  Setup_Config->CbsCpuPst2Did = 8;                                      //   Pstate2 DID
  Setup_Config->CbsCpuPst2Vid = 255;                                    //   Pstate2 VID
  Setup_Config->CbsCpuPstCustomP3 = 2;                                  // Custom Pstate3
  Setup_Config->CbsCpuCofP3 = 0;                                        //   Frequency (MHz)
  Setup_Config->CbsCpuVoltageP3 = 0;                                    //   Voltage (uV)
  Setup_Config->CbsCpuPst3Fid = 16;                                     //   Pstate3 FID
  Setup_Config->CbsCpuPst3Did = 8;                                      //   Pstate3 DID
  Setup_Config->CbsCpuPst3Vid = 255;                                    //   Pstate3 VID
  Setup_Config->CbsCpuPstCustomP4 = 2;                                  // Custom Pstate4
  Setup_Config->CbsCpuCofP4 = 0;                                        //   Frequency (MHz)
  Setup_Config->CbsCpuVoltageP4 = 0;                                    //   Voltage (uV)
  Setup_Config->CbsCpuPst4Fid = 16;                                     //   Pstate4 FID
  Setup_Config->CbsCpuPst4Did = 8;                                      //   Pstate4 DID
  Setup_Config->CbsCpuPst4Vid = 255;                                    //   Pstate4 VID
  Setup_Config->CbsCpuPstCustomP5 = 2;                                  // Custom Pstate5
  Setup_Config->CbsCpuCofP5 = 0;                                        //   Frequency (MHz)
  Setup_Config->CbsCpuVoltageP5 = 0;                                    //   Voltage (uV)
  Setup_Config->CbsCpuPst5Fid = 16;                                     //   Pstate5 FID
  Setup_Config->CbsCpuPst5Did = 8;                                      //   Pstate5 DID
  Setup_Config->CbsCpuPst5Vid = 255;                                    //   Pstate5 VID
  Setup_Config->CbsCpuPstCustomP6 = 2;                                  // Custom Pstate6
  Setup_Config->CbsCpuCofP6 = 0;                                        //   Frequency (MHz)
  Setup_Config->CbsCpuVoltageP6 = 0;                                    //   Voltage (uV)
  Setup_Config->CbsCpuPst6Fid = 16;                                     //   Pstate6 FID
  Setup_Config->CbsCpuPst6Did = 8;                                      //   Pstate6 DID
  Setup_Config->CbsCpuPst6Vid = 255;                                    //   Pstate6 VID
  Setup_Config->CbsCpuPstCustomP7 = 2;                                  // Custom Pstate7
  Setup_Config->CbsCpuCofP7 = 0;                                        //   Frequency (MHz)
  Setup_Config->CbsCpuVoltageP7 = 0;                                    //   Voltage (uV)
  Setup_Config->CbsCpuPst7Fid = 16;                                     //   Pstate7 FID
  Setup_Config->CbsCpuPst7Did = 8;                                      //   Pstate7 DID
  Setup_Config->CbsCpuPst7Vid = 255;                                    //   Pstate7 VID
  Setup_Config->CbsCpuCoreThreadLegalDisclaimer = 0;                    // Core/Thread Enablement Legal Disclaimer
  Setup_Config->CbsCmnCpuGenDowncoreCtrl = 0;                           // Downcore control
  Setup_Config->CbsDfCmnRedirScrubCtrl = 3;                             // Redirect scrubber control
  Setup_Config->CbsDfCmnSyncFloodProp = 3;                              // Disable DF sync flood propagation
  Setup_Config->CbsDfCmnFreezeQueueError = 3;                           // Freeze DF module queues on error
  Setup_Config->CbsDfCmnGmiEncryption = 3;                              // GMI encryption control
  Setup_Config->CbsDfCmnXGmiEncryption = 3;                             // xGMI encryption control
  Setup_Config->CbsDfCmnCc6MemEncryption = 3;                           // CC6 memory region encryption
  Setup_Config->CbsDfCmnCc6AllocationScheme = 3;                        // Location of private memory regions
  Setup_Config->CbsDfCmnSysProbeFilter = 3;                             // System probe filter
  Setup_Config->CbsDfCmnMemIntlvSize = 7;                               // Memory interleaving size
  Setup_Config->CbsDfCmnMemClear = 3;                                   // Memory Clear
  Setup_Config->CbsCmnMemTimingLegalDisclaimer = 0;                     // DRAM Timing Configuration Legal Disclaimer
  Setup_Config->CbsCmnMemTimingLegalDisclaimer1 = 0;                    // DRAM Timing Configuration Legal Disclaimer 1
  Setup_Config->CbsCmnMemOverclockDdr4 = 1;                          // Overclock
  Setup_Config->CbsCmnMemSpeedDdr4 = 0xFF;                              // Memory Clock Speed
  Setup_Config->CbsCmnMemTimingTclDdr4 = 0xFF;                          // Tcl
  Setup_Config->CbsCmnMemTimingTrcdrdDdr4 = 0xFF;                       // Trcdrd
  Setup_Config->CbsCmnMemTimingTrcdwrDdr4 = 0xFF;                       // Trcdwr
  Setup_Config->CbsCmnMemTimingTrpDdr4 = 0xFF;                          // Trp
  Setup_Config->CbsCmnMemTimingTrasDdr4 = 0xFF;                         // Tras
  Setup_Config->CbsCmnMemTimingTrcCtrlDdr4 = 0;                         // Trc Ctrl
  Setup_Config->CbsCmnMemTimingTrcDdr4 = 0x39;                          // Trc
  Setup_Config->CbsCmnMemTimingTrrdSDdr4 = 0xFF;                        // TrrdS
  Setup_Config->CbsCmnMemTimingTrrdLDdr4 = 0xFF;                        // TrrdL
  Setup_Config->CbsCmnMemTimingTfawCtrlDdr4 = 0;                        // Tfaw Ctrl
  Setup_Config->CbsCmnMemTimingTfawDdr4 = 26;                           // Tfaw
  Setup_Config->CbsCmnMemTimingTwtrSDdr4 = 0xFF;                        // TwtrS
  Setup_Config->CbsCmnMemTimingTwtrLDdr4 = 0xFF;                        // TwtrL
  Setup_Config->CbsCmnMemTimingTwrCtrlDdr4 = 0;                         // Twr Ctrl
  Setup_Config->CbsCmnMemTimingTwrDdr4 = 18;                            // Twr
  Setup_Config->CbsCmnMemTimingTrcpageCtrlDdr4 = 0;                     // Trcpage Ctrl
  Setup_Config->CbsCmnMemTimingTrcpageDdr4 = 0;                         // Trcpage
  Setup_Config->CbsCmnMemTimingTrdrdScLCtrlDdr4 = 0;                    // TrdrdScL Ctrl
  Setup_Config->CbsCmnMemTimingTrdrdScLDdr4 = 3;                        // TrdrdScL
  Setup_Config->CbsCmnMemTimingTwrwrScLCtrlDdr4 = 0;                    // TwrwrScL Ctrl
  Setup_Config->CbsCmnMemTimingTwrwrScLDdr4 = 0x3;                      // TwrwrScL
  Setup_Config->CbsCmnMemTimingTrfcCtrlDdr4 = 0;                        // Trfc Ctrl
  Setup_Config->CbsCmnMemTimingTrfcDdr4 = 0x138;                        // Trfc
  Setup_Config->CbsCmnMemTimingTrfc2CtrlDdr4 = 0;                       // Trfc2 Ctrl
  Setup_Config->CbsCmnMemTimingTrfc2Ddr4 = 0xC0;                        // Trfc2
  Setup_Config->CbsCmnMemTimingTrfc4CtrlDdr4 = 0;                       // Trfc4 Ctrl
  Setup_Config->CbsCmnMemTimingTrfc4Ddr4 = 0x84;                        // Trfc4
  Setup_Config->CbsCmnMemCtrllerProcOdtDdr4 = 0xFF;                     // ProcODT
  Setup_Config->CbsCmnMemTimingTcwlDdr4 = 0xFF;                         // Tcwl
  Setup_Config->CbsCmnMemTimingTrtpDdr4 = 0xFF;                         // Trtp
  Setup_Config->CbsCmnMemTimingTrdwrDdr4 = 0xFF;                        // Trdwr
  Setup_Config->CbsCmnMemTimingTwrrdDdr4 = 0xFF;                        // Twrrd
  Setup_Config->CbsCmnMemTimingTwrwrScDdr4 = 0xFF;                      // TwrwrSc
  Setup_Config->CbsCmnMemTimingTwrwrSdDdr4 = 0xFF;                      // TwrwrSd
  Setup_Config->CbsCmnMemTimingTwrwrDdDdr4 = 0xFF;                      // TwrwrDd
  Setup_Config->CbsCmnMemTimingTrdrdScDdr4 = 0xFF;                      // TrdrdSc
  Setup_Config->CbsCmnMemTimingTrdrdSdDdr4 = 0xFF;                      // TrdrdSd
  Setup_Config->CbsCmnMemTimingTrdrdDdDdr4 = 0xFF;                      // TrdrdDd
  Setup_Config->CbsCmnMemTimingTckeDdr4 = 0xFF;                         // Tcke
  Setup_Config->CbsCmnMemCtrller2TModeDdr4 = 0xFF;                      // Cmd2T
  Setup_Config->CbsCmnMemGearDownModeDdr4 = 0xFF;                       // Gear Down Mode
  Setup_Config->CbsCmnMemGearDownModeDdr4 = 0;                          // Auto Refresh Rate
  Setup_Config->CbsCmnMemCtrllerPwrDnEnDdr4 = 0xFF;                     // Power Down Enable
  Setup_Config->CbsCmnMemDataBusConfigCtlDdr4 = 0xFF;                   // Data Bus Configuration User Controls
  Setup_Config->CbsCmnMemCtrllerRttNomDdr4 = 0xFF;                      // RttNom
  Setup_Config->CbsCmnMemCtrllerRttWrDdr4 = 0xFF;                       // RttWr
  Setup_Config->CbsCmnMemCtrllerRttParkDdr4 = 0xFF;                     // RttPark
  Setup_Config->CbsCmnMemDataPoisoningDdr4 = 0xFF;                      // Data Poisoning
  Setup_Config->CbsCmnMemCtrllerDramEccSymbolSizeDdr4 = 0xFF;           // DRAM ECC Symbol Size
  Setup_Config->CbsCmnMemCtrllerDramEccEnDdr4 = 0xFF;                   // DRAM ECC Enable
  Setup_Config->CbsCmnMemTsmeDdr4 = 0xFF;                               // TSME
  Setup_Config->CbsCmnMemCtrllerDataScrambleDdr4 = 0xFF;                // Data Scramble
  Setup_Config->CbsCmnMemMappingBankInterleaveDdr4 = 0xFF;              // Chipselect Interleaving
  Setup_Config->CbsCmnMemCtrllerBankGroupSwapDdr4 = 0xFF;               // BankGroupSwap
  Setup_Config->CbsCmnMemCtrllerBankGroupSwapAltDdr4 = 0xFF;            // BankGroupSwapAlt
  Setup_Config->CbsCmnMemAddressHashBankDdr4 = 0xFF;                    // Address Hash Bank
  Setup_Config->CbsCmnMemAddressHashCsDdr4 = 0xFF;                      // Address Hash CS
  Setup_Config->CbsCmnMemMbistEn = 0;                                   // MBIST Enable
  Setup_Config->CbsCmnMemMbistTestmode = 0;                             // MBIST Test Mode
  Setup_Config->CbsCmnMemMbistAggressors = 0xff;                        // MBIST Aggressors
  Setup_Config->CbsCmnMemMbistPerBitSlaveDieReport = 0xff;              // MBIST Per Bit Slave Die Reporting
  Setup_Config->CbsCmnGnbSMUSystemConfig = 0xf;                         // System Configuration
  Setup_Config->CbsDbgPoisonConsumption = 0xF;                          // NBIO Internal Poison Consumption
  Setup_Config->CbsDbgRASControl = 0xF;                                 // NBIO RAS Control
  Setup_Config->CbsCmncTDPCtl = 0;                                      // cTDP Control
  Setup_Config->CbsCmncTDPLimit = 0;                                    // cTDP
  Setup_Config->CbsCmnNbioEfficiencyOptimizedMode = 0xF;                // Efficiency Optimized Mode
  Setup_Config->CbsCmnNbioPSIDisable = 0xf;                             // PSI
  Setup_Config->CbsDbgGnbDbgACSEnable = 0xF;                            // ACS Enable
  Setup_Config->CbsGnbDbgPcieAriSupport = 0xf;                          // PCIe ARI Support
  Setup_Config->CbsCmnCLDO_VDDPCtl = 0;                                 // CLDO_VDDP Control
  Setup_Config->CbsCmnCLDOVDDPvoltage = 0;                              // CLDO_VDDP voltage
  Setup_Config->CbsCmnGnbHdAudioEn = 0xF;                               // HD Audio Enable
  Setup_Config->CbsCfgPcieLoopbackMode = 2;                             // Block PCIe Loopback
  Setup_Config->CbsCfgPcieCrsDelay = 6;                                 // CRS Delay
  Setup_Config->CbsCfgPcieCrsLimit = 6;                                 // CRS Limit
  Setup_Config->CbsCmnGnbNbIOMMU = 1;                                   // IOMMU
  Setup_Config->CbsPcieSyncReset = 0xf;                                 // Concurrent Training
  Setup_Config->CbsDbgFanCtl = 0;                                       // Fan Control
  Setup_Config->CbsDbgForcePwmCtl = 0;                                  // Force PWM Control
  Setup_Config->CbsDbgForcePwm = 0;                                     // Force PWM
  Setup_Config->CbsDbgFanTableCtl = 0;                                  // Fan Table Control
  Setup_Config->CbsDbgFanTableTempLow = 0;                              // Low Temperature
  Setup_Config->CbsDbgFanTableTempMed = 0;                              // Medium Temperature
  Setup_Config->CbsDbgFanTableTempHigh = 0;                             // High Temperature
  Setup_Config->CbsDbgFanTableTempCritical = 0;                         // Critical Temperature
  Setup_Config->CbsDbgFanTableTPwmLow = 0;                              // Low Pwm
  Setup_Config->CbsDbgFanTablePwmMed = 0;                               // Medium Pwm
  Setup_Config->CbsDbgFanTablePwmHigh = 0;                              // High Pwm
  Setup_Config->CbsDbgFanTableHyst = 0;                                 // Temperature Hysteresis
  Setup_Config->CbsDbgFanTablePwmFreq = 0;                              // Pwm Frequency
  Setup_Config->CbsDbgFanPolarity = 0;                                  // Fan Polarity
  Setup_Config->CbsCmnDisableSideband = 0;                              // Ignore sideband
  Setup_Config->CbsCmnDisableL1wa = 0;                                  // Disable L1 w/a
  Setup_Config->CbsCmnDisableBridgeDis = 0;                             // Disable BridgeDis
  Setup_Config->CbsCmnDisableIrqPoll = 0;                               // Disable irq polling
  Setup_Config->CbsCmnIrqSetsBridgeDis = 0;                             // IRQ sets BridgeDis
  Setup_Config->CbsPcieHotplugSupport = 1;                              // PCIE hotplug
  Setup_Config->CbsCmnFchSataEnable = 0xf;                              // SATA Controller
  Setup_Config->CbsCmnFchSataClass = 2;                                 // SATA Mode
  Setup_Config->CbsCmnFchSataRasSupport = 0xf;                          // Sata RAS Support
  Setup_Config->CbsCmnFchSataAhciDisPrefetchFunction = 0xf;             // Sata Disabled AHCI Prefetch Function
  Setup_Config->CbsDbgFchSataAggresiveDevSlpP0 = 0xF;                   // Aggresive SATA Device Sleep Port 0
  Setup_Config->CbsDbgFchSataDevSlpPort0Num = 0;                        // DevSleep0 Port Number
  Setup_Config->CbsDbgFchSataAggresiveDevSlpP1 = 0xF;                   // Aggresive SATA Device Sleep Port 1
  Setup_Config->CbsDbgFchSataDevSlpPort1Num = 0;                        // DevSleep1 Port Number
  Setup_Config->CbsCmnFchUsbXHCI0Enable = 0xf;                          // XHCI controller enable
  Setup_Config->CbsCmnFchUsbXHCI1Enable = 0xf;                          // XHCI Controller1 enable (Die1)
  Setup_Config->CbsCmnFchUsbXHCI2Enable = 0xf;                          // XHCI2 enable (MCM1/Die0)
  Setup_Config->CbsCmnFchUsbXHCI3Enable = 0xf;                          // XHCI3 enable (MCM1/Die1)
  Setup_Config->CbsCmnFchSdConfig = 0;                                  // SD Configuration Mode
  Setup_Config->CbsCmnFchSystemPwrFailShadow = 0;                       // Ac Loss Control
  Setup_Config->CbsCmnFchI2C0Config = 0xf;                              // I2C 0 Enable
  Setup_Config->CbsCmnFchI2C1Config = 0xf;                              // I2C 1 Enable
  Setup_Config->CbsCmnFchI2C2Config = 0xf;                              // I2C 2 Enable
  Setup_Config->CbsCmnFchI2C3Config = 0xf;                              // I2C 3 Enable
  Setup_Config->CbsCmnFchI2C4Config = 0xf;                              // I2C 4 Enable
  Setup_Config->CbsCmnFchI2C5Config = 0xf;                              // I2C 5 Enable
  Setup_Config->CbsCmnFchUart0Config = 0xf;                             // Uart 0 Enable
  Setup_Config->CbsCmnFchUart0LegacyConfig = 0;                         // Uart 0 Legacy Options
  Setup_Config->CbsCmnFchUart1Config = 0xf;                             // Uart 1 Enable
  Setup_Config->CbsCmnFchUart1LegacyConfig = 0;                         // Uart 1 Legacy Options
  Setup_Config->CbsCmnFchUart2Config = 0xf;                             // Uart 2 Enable (no HW FC)
  Setup_Config->CbsCmnFchUart2LegacyConfig = 0;                         // Uart 2 Legacy Options
  Setup_Config->CbsCmnFchUart3Config = 0xf;                             // Uart 3 Enable (no HW FC)
  Setup_Config->CbsCmnFchUart3LegacyConfig = 0;                         // Uart 3 Legacy Options
  Setup_Config->CbsCmnFchEspiConfig = 0xf;                              // ESPI Enable
  Setup_Config->CbsCmnPTSataPortEnable = 0xf;                           // PT SATA Port Enable
  Setup_Config->CbsCmnPTSataClass = 0xf;                                // PT SATA Mode
  Setup_Config->CbsDbgPTSataAggresiveDevSlpP0 = 0xF;                    // PT Aggresive SATA Device Sleep Port 0
  Setup_Config->CbsDbgPTSataAggresiveDevSlpP1 = 0xF;                    // PT Aggresive SATA Device Sleep Port 1
  Setup_Config->CbsDbgPTXhciGen1 = 0xF;                                 // PT XHCI GEN1
  Setup_Config->CbsDbgPTXhciGen2 = 0xF;                                 // PT XHCI GEN2
  Setup_Config->CbsDbgPTUsbEqualization4 = 0xF;                         // PT USB Equalization4
  Setup_Config->CbsDbgPTUsbRedriver = 0xF;                              // PT USB Redriver
  Setup_Config->CbsDbgPTUsb31P0 = 0xF;                                  // PT USB31 PORT0
  Setup_Config->CbsDbgPTUsb31P1 = 0xF;                                  // PT USB31 PORT1
  Setup_Config->CbsDbgPTUsb30P0 = 0xF;                                  // PT USB30 PORT0
  Setup_Config->CbsDbgPTUsb30P1 = 0xF;                                  // PT USB30 PORT1
  Setup_Config->CbsDbgPTUsb30P2 = 0xF;                                  // PT USB30 PORT2
  Setup_Config->CbsDbgPTUsb30P3 = 0xF;                                  // PT USB30 PORT3
  Setup_Config->CbsDbgPTUsb30P4 = 0xF;                                  // PT USB30 PORT4
  Setup_Config->CbsDbgPTUsb30P5 = 0xF;                                  // PT USB30 PORT5
  Setup_Config->CbsDbgPTUsb20P0 = 0xF;                                  // PT USB20 PORT0
  Setup_Config->CbsDbgPTUsb20P1 = 0xF;                                  // PT USB20 PORT1
  Setup_Config->CbsDbgPTUsb20P2 = 0xF;                                  // PT USB20 PORT2
  Setup_Config->CbsDbgPTUsb20P3 = 0xF;                                  // PT USB20 PORT3
  Setup_Config->CbsDbgPTUsb20P4 = 0xF;                                  // PT USB20 PORT4
  Setup_Config->CbsDbgPTUsb20P5 = 0xF;                                  // PT USB20 PORT5
  Setup_Config->CbsDbgPTProm2Usb31P0 = 0xF;                             // PT USB31 PORT0
  Setup_Config->CbsDbgPTProm2Usb31P1 = 0xF;                             // PT USB31 PORT1
  Setup_Config->CbsDbgPTProm2Usb30P0 = 0xF;                             // PT USB30 PORT0
  Setup_Config->CbsDbgPTProm2Usb30P1 = 0xF;                             // PT USB30 PORT1
  Setup_Config->CbsDbgPTProm2Usb20P0 = 0xF;                             // PT USB20 PORT0
  Setup_Config->CbsDbgPTProm2Usb20P1 = 0xF;                             // PT USB20 PORT1
  Setup_Config->CbsDbgPTProm2Usb20P2 = 0xF;                             // PT USB20 PORT2
  Setup_Config->CbsDbgPTProm2Usb20P3 = 0xF;                             // PT USB20 PORT3
  Setup_Config->CbsDbgPTProm2Usb20P4 = 0xF;                             // PT USB20 PORT4
  Setup_Config->CbsDbgPTProm2Usb20P5 = 0xF;                             // PT USB20 PORT5
  Setup_Config->CbsDbgPTProm1Usb31P0 = 0xF;                             // PT USB31 PORT0
  Setup_Config->CbsDbgPTProm1Usb31P1 = 0xF;                             // PT USB31 PORT1
  Setup_Config->CbsDbgPTProm1Usb30P0 = 0xF;                             // PT USB30 PORT0
  Setup_Config->CbsDbgPTProm1Usb20P0 = 0xF;                             // PT USB20 PORT0
  Setup_Config->CbsDbgPTProm1Usb20P1 = 0xF;                             // PT USB20 PORT1
  Setup_Config->CbsDbgPTProm1Usb20P2 = 0xF;                             // PT USB20 PORT2
  Setup_Config->CbsDbgPTProm1Usb20P3 = 0xF;                             // PT USB20 PORT3
  Setup_Config->CbsDbgPTProm1Usb20P4 = 0xF;                             // PT USB20 PORT4
  Setup_Config->CbsDbgPTProm1Usb20P5 = 0xF;                             // PT USB20 PORT5
  Setup_Config->CbsDbgSP3NtbEnable = 0;                                 // NTB Enable
  Setup_Config->CbsDbgSP3NtbLocation = 0xFF;                            // NTB Location
  Setup_Config->CbsDbgSP3NtbPCIeCore = 0xf;                             // NTB active on PCIeCore
  Setup_Config->CbsDbgSP3NtbMode = 0xf;                                 // NTB Mode
  Setup_Config->CbsDbgSP3NtbLinkSpeed = 0xf;                            //   Link Speed
  Setup_Config->CbsDbgSP3NtbBAR1Window = 2;                            //   bar1 window size
  Setup_Config->CbsDbgSP3NtbBAR23Window = 2;                            //   bar1 window size
  Setup_Config->CbsDbgSP3NtbBAR45Window = 2;                            //   bar1 window size

  #if BC_TENCENT
  Setup_Config->CbsCmnDeterminismSlider = 1;                          // Determinism Slider
  Setup_Config->CbsDfCmnMemIntlv = 2;                                 // Memory interleaving
  Setup_Config->CbsDfCmnDramScrubTime = 5;                              // DRAM scrub time
  Setup_Config->CbsCmnCpuRdseedRdrandCtrl = 3;                          // Enable CPU RdseedRdrand Feature, hzp20170921+ //PS-625
  Setup_Config->CbsCmnSmeeCtrl = 0;                                     // Enable/Disable SMEE
  Setup_Config->CbsCmnCpuSmeeCap = 0;                                   // Enable/Disable SMEE
  Setup_Config->CbsCmnSVMCtrl = 1;                                      // Enable/Disable SVM
  Setup_Config->CbsDfCmnChnlIntlvHash = 1;                              // Channel interleaving hash
  Setup_Config->CbsCmnCpuL1StreamHwPrefetcher = 3;                      // L1 Stream HW Prefetcher
  Setup_Config->CbsCmnCpuL2StreamHwPrefetcher = 3;                      // L2 Stream HW Prefetcher  
  #else 
  Setup_Config->CbsCmnDeterminismSlider = 0xf;                        // Determinism Slider
  Setup_Config->CbsDfCmnMemIntlv = 7;                                 // Memory interleaving
  Setup_Config->CbsDfCmnDramScrubTime = 7;                              // DRAM scrub time
  Setup_Config->CbsCmnCpuRdseedRdrandCtrl = 0;                          // Enable CPU RdseedRdrand Feature, hzp20170921+ //PS-625
  Setup_Config->CbsCmnSmeeCtrl = 1;                                     // Enable/Disable SMEE
  Setup_Config->CbsCmnCpuSmeeCap = 1;                                   // Enable/Disable SMEE
  Setup_Config->CbsCmnSVMCtrl = 0;                                      // Enable/Disable SVM
  Setup_Config->CbsDfCmnChnlIntlvHash = 3;                              // Channel interleaving hash  
  Setup_Config->CbsCmnCpuL1StreamHwPrefetcher = 3;                      // L1 Stream HW Prefetcher
  Setup_Config->CbsCmnCpuL2StreamHwPrefetcher = 3;                      // L2 Stream HW Prefetcher  
  #endif
 
}
