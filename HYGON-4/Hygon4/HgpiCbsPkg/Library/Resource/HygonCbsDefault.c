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

#include "HygonCbsVariable.h"

VOID
CbsWriteDefalutValue (
  IN UINT8 *IfrData
  )
{
  CBS_CONFIG  *Setup_Config;

  Setup_Config = (CBS_CONFIG *)IfrData;

  Setup_Config->CbsCmnCpuPfeh  = 3;                                     // Platform First Error Handling  // byo231109 -
  Setup_Config->CbsCmnCpuMcaErrThreshEn = 0xFF;                         // MCA error thresh enable
  Setup_Config->CbsCmnCpuMcaErrThreshCount = 0xA;                       // MCA error thresh count
  Setup_Config->CbsCmnCpuCpb   = 1;                                     // Core Performance Boost
  Setup_Config->CbsCmnCpuEnIbs = 3;                                     // Enable IBS
  Setup_Config->CbsCmnCpuCstateCtrl = 0xFF;                             // CPU C-state Control
  Setup_Config->CbsCmnSmtMode = 0x1;                                    // SMT Mode Control
  Setup_Config->CbsCmnCpuOpcacheCtrl = 3;                               // Opcache Control
  Setup_Config->CbsCmnCpuOcMode = 0;                                    // OC Mode
  Setup_Config->CbsCpuModel     = 4;                                    // Cpu model
  Setup_Config->CbsCpuPkgType   = 1;                                    // Cpu PkgType
  Setup_Config->CbsCmnCpuCsvAsidSpaceLimit   = 1;                       // CSV-ES ASID Space Limit
  Setup_Config->CbsCmnCpuStreamingStoresCtrl = 3;                       // Streaming Stores Control
  Setup_Config->CbsCmnCpuLApicMode = 0xFF;                              // Local APIC Mode
  Setup_Config->CbsCmnCpuRdseedRdrandCtrl   = 3;                        // Enable CPU RdseedRdrand Feature
  Setup_Config->CbsCmnCpuLoadUcodeCtrl      = 1;                        // Enable CPU load ucode Feature
  Setup_Config->CbsCmnCoreWatchdogCtrl      = 3;                        // Core Watchdog Control
  Setup_Config->CbsCmnCpuForceVddcrCpuVidEn = 0;                        // Fource vddcr cpu enable
  Setup_Config->CbsCmnCpuForceVddcrCpuVid   = 0;                        // Fource vddcr cpu vid value
  Setup_Config->CbsCmnVddcrCpuVoltageMargin = 0;                        // cpu voltage margin
  Setup_Config->CbsCmnForceCclkFrequencyEn  = 0;                        // Fource cclk enable
  Setup_Config->CbsCmnForceCclkFrequency    = 2000;                     // Fource cclk value
  Setup_Config->CbsCmnDldoBypass = 0;                                   // Bypass dldo
  Setup_Config->CbsCmnCpuRdseedRdrandCap = 1;                           // Display RdseedRdrand configuration item in CBS        // byo231012 -
  Setup_Config->CbsCmnSmeeCtrl = 0;                                     // Enable/Disable SMEE
  Setup_Config->CbsAbove4GMmioLimitBit = 43;                            // Above 64 MMIO limit bit 
  Setup_Config->CbsCmnCpuSmeeCap = 1;                                   // Enable/Disable SMEE
  Setup_Config->CbsCmnSVMCtrl    = 0;                                   // Enable/Disable SVM
  Setup_Config->CbsCpuPstCustomP0 = 2;                                  // Custom Pstate0
  Setup_Config->CbsCpuCofP0       = 0;                                  // Frequency (MHz)
  Setup_Config->CbsCpuVoltageP0   = 0;                                  // Voltage (uV)
  Setup_Config->CbsCpuPst0Fid     = 16;                                 // Pstate0 FID
  Setup_Config->CbsCpuPst0Did     = 8;                                  // Pstate0 DID
  Setup_Config->CbsCpuPst0Vid     = 255;                                // Pstate0 VID
  Setup_Config->CbsCpuPstCustomP1 = 2;                                  // Custom Pstate1
  Setup_Config->CbsCpuCofP1       = 0;                                  // Frequency (MHz)
  Setup_Config->CbsCpuVoltageP1   = 0;                                  // Voltage (uV)
  Setup_Config->CbsCpuPst1Fid     = 16;                                 // Pstate1 FID
  Setup_Config->CbsCpuPst1Did     = 8;                                  // Pstate1 DID
  Setup_Config->CbsCpuPst1Vid     = 255;                                // Pstate1 VID
  Setup_Config->CbsCpuPstCustomP2 = 2;                                  // Custom Pstate2
  Setup_Config->CbsCpuCofP2       = 0;                                  // Frequency (MHz)
  Setup_Config->CbsCpuVoltageP2   = 0;                                  // Voltage (uV)
  Setup_Config->CbsCpuPst2Fid     = 16;                                 // Pstate2 FID
  Setup_Config->CbsCpuPst2Did     = 8;                                  // Pstate2 DID
  Setup_Config->CbsCpuPst2Vid     = 255;                                // Pstate2 VID
  Setup_Config->CbsCmnCpuGenDowncoreCtrl = 0;                           // Downcore control
  Setup_Config->CbsCmnCpuL1StreamHwPrefetcher = 3;                      // L1 Stream HW Prefetcher
  Setup_Config->CbsCmnCpuL2StreamHwPrefetcher = 3;                      // L2 Stream HW Prefetcher
  Setup_Config->CbsDfCmnDramScrubTime       = 7;                        // DRAM scrub time
  Setup_Config->CbsDfCmnRedirScrubCtrl      = 3;                        // Redirect scrubber control
  Setup_Config->CbsDfCmnSyncFloodProp       = 1;                        // Disable DF sync flood propagation
  Setup_Config->CbsDfCmnFreezeQueueError    = 1;                        // Freeze DF module queues on error
  Setup_Config->CbsDfCmnHmiEncryption       = 3;                        // HMI encryption control
  Setup_Config->CbsDfCmnXHmiEncryption      = 3;                        // xHMI encryption control
  Setup_Config->CbsDfCmnCc6MemEncryption    = 3;                        // CC6 memory region encryption
  Setup_Config->CbsDfCmnCc6AllocationScheme = 3;                        // Location of private memory regions
  Setup_Config->CbsDfCmnSysProbeFilter      = 3;                        // System probe filter
  Setup_Config->CbsDfCmnMemIntlv       = 7;                             // Memory interleaving
  Setup_Config->CbsDfCmnMemIntlvSize   = 7;                             // Memory interleaving size
  Setup_Config->CbsDfCmnChnlIntlvHash  = 3;                             // Channel interleaving hash
  Setup_Config->CbsDfCmnSubchannelIntlvBit = 0;                         // Subchannel interleaving bit
  Setup_Config->CbsDfCmnMemClear       = 3;                             // Memory Clear
  Setup_Config->CbsFclkUclkSyncMode    = 1;                             // FclkUclkSyncMode
  Setup_Config->CbsDfNumaNodePerSocket = 0xFF;                          // NUMA Nodes number per socket
  Setup_Config->CbsDfCmnCstateCtrl     = 0xFF;                          // DF C-state Control
  Setup_Config->CbsCmnMemOverclockDdr5        = 0xFF;                   // Overclock
  Setup_Config->CbsCmnMemSpeedDdr5            = 0xFF;                   // Memory Clock Speed
  Setup_Config->CbsCmnMemCtrller2TModeDdr5    = 0xFF;                   // Cmd2T
  Setup_Config->CbsCmnMemGearDownModeDdr5     = 0xFF;                   // Gear Down Mode
  Setup_Config->CbsCmnMemCtrllerPwrDnEnDdr5   = 0xFF;                   // Power Down Enable
  Setup_Config->CbsCmnMemDataBusConfigCtlDdr5 = 0xFF;                   // Data Bus Configuration User Controls
  Setup_Config->CbsCmnMemCtrllerPhyDrvStrenDqdqs = 0xFF;                // phydrvstrenQqdqs
  Setup_Config->CbsCmnMemCtrllerPhyOdtStrenDqDqs = 0xFF;                // PhyOdtStrenDqDqs
  Setup_Config->CbsCmnMemCtrllerRttNomRDDdr5 = 0xFF;                    // RttNom
  Setup_Config->CbsCmnMemCtrllerRttWrDdr5 = 0xFF;                       // RttWr
  Setup_Config->CbsCmnMemCtrllerRttNomWrDdr5 = 0xFF;                    // RttNomWr
  Setup_Config->CbsCmnMemCtrllerRttParkDdr5   = 0xFF;                   // RttPark
  Setup_Config->CbsCmnMemDataPoisoningDdr5    = 0xFF;                   // Data Poisoning
  Setup_Config->CbsCmnMemCtrllerDramEccSymbolSizeDdr5 = 0xFF;           // DRAM ECC Symbol Size
  Setup_Config->CbsCmnMemCtrllerDramEccEnDdr5 = 0xFF;                   // DRAM ECC Enable
  Setup_Config->CbsCmnMemTsmeDdr5 = 0xFF;                               // TSME
  Setup_Config->CbsCmnMemCtrllerDataScrambleDdr5     = 0xFF;            // Data Scramble
  Setup_Config->CbsCmnMemMappingBankInterleaveDdr5   = 0xFF;            // Chipselect Interleaving
  Setup_Config->CbsCmnMemCtrllerBankGroupSwapDdr5    = 0xFF;            // BankGroupSwap
  Setup_Config->CbsCmnMemAddressHashBankDdr5 = 0xFF;                    // Address Hash Bank
  Setup_Config->CbsCmnMemAddressHashCsDdr5   = 0xFF;                    // Address Hash CS
  Setup_Config->CbsCmnMemMbistEn = 0;                                   // MBIST Enable
  Setup_Config->CbsCmnMemMbistTestmode   = 0;                           // MBIST Test Mode
  Setup_Config->CbsCmnMemMbistAggressors = 0xff;                        // MBIST Aggressors
  Setup_Config->CbsCmnMemMbistPerBitSlaveDieReport = 0xff;              // MBIST Per Bit Slave Die Reporting
  Setup_Config->CbsDbgPoisonConsumption  = 0xF;                         // NBIO Internal Poison Consumption
  Setup_Config->CbsNbioRASControl = 0;                                  // NBIO RAS Control
  Setup_Config->CbsCmncTDPCtl     = 0;                                  // cTDP Control
  Setup_Config->CbsCmncTDPLimit   = 0;                                  // cTDP
  Setup_Config->CbsCmnTDPBoost    = 0xF;                                // TDP Boost
  Setup_Config->CbsCmnNbioEfficiencyOptimizedMode = 0xF;                // Efficiency Optimized Mode
  Setup_Config->CbsCmnNbioPSIDisable    = 0xf;                          // PSI
  Setup_Config->CbsDbgGnbDbgACSEnable   = 0xF;                          // ACS Enable
  Setup_Config->CbsGnbDbgPcieAriSupport = 0xf;                          // PCIe ARI Support
  Setup_Config->CbsCmnCLDO_VDDMCtl = 0;                                 // CLDO_VDDM Control
  Setup_Config->CbsCmnCLDOVDDMvoltage = 0;                              // CLDO_VDDM voltage
  // HYGON_EX_CPU
  Setup_Config->CbsCmnCLDO_VDDPCtl = 0;                                 // CLDO_VDDP Control
  Setup_Config->CbsCmnCLDOVDDPvoltage = 0;                              // CLDO_VDDP voltage
  Setup_Config->CbsCmnCLDOVDDPvoltageIODHmiPHY      = 241;              // CLDO_VDDP voltage IOD HMI PHY
  Setup_Config->CbsCmnCLDOVDDPvoltageCDDDDRPHY      = 241;              // CLDO_VDDP voltage CDD DDR PHY
  Setup_Config->CbsCmnCLDOVDDPvoltageCDDHmiCFOPPHY  = 241;              // CLDO_VDDP voltage CDD HMI CFOP PHY
  Setup_Config->CbsCmnCldoVDDPBypass = 0;                               // CLDO_VDDP Bypass Control
  Setup_Config->CbsCmnCldoVDDPBypassIODHMI_MODE = 0;                    // CLDO_VDDP Bypass IOD HMI Mode
  Setup_Config->CbsCmnCldoVDDPBypassCDDHMI_MODE = 0;                    // CLDO_VDDP Bypass CDD HMI Mode
  Setup_Config->CbsCmnCldoVDDPBypassCDDDDR_MODE = 0;                    // CLDO_VDDP Bypass CDD DDR Mode
  // HYGON_GX_CPU
  Setup_Config->CbsCmnDjCLDO_VDDPCtl = 0;                               // DJ CLDO_VDDP Control
  Setup_Config->CbsCmnDjCLDOVDDPvoltageHmi = 241;                       // DJ HMI CLDO_VDDP voltage
  Setup_Config->CbsCmnDjCLDOVDDPvoltageS5 = 241;                        // DJ S5 CLDO_VDDP voltage
  Setup_Config->CbsCmnEmCLDO_VDDPCtl = 0;                               // EM CLDO_VDDP Control
  Setup_Config->CbsCmnEmCLDOVDDPvoltageHmi = 241;                       // EM HMI CLDO_VDDP voltage
  Setup_Config->CbsCmnEmCLDOVDDPvoltageCfop = 241;                      // EM CFOP CLDO_VDDP voltage
  Setup_Config->CbsCmnCddCLDO_VDDPCtl = 0;                              // CDD CLDO_VDDP Control
  Setup_Config->CbsCmnCddCLDOVDDPvoltageHmi = 241;                      // CDD HMI CLDO_VDDP voltage
  Setup_Config->CbsCmnCddCLDOVDDPvoltageDDR = 241;                      // CDD DDR CLDO_VDDP voltage
  Setup_Config->CbsCmnCldoVDDPBypassCtl = 0;                            // CLDO_VDDP Bypass Control
  Setup_Config->CbsCmnCldoVDDPDjHmiBypassMode = 0;                      // CLDO_VDDP Bypass DJ HMI Mode
  Setup_Config->CbsCmnCldoVDDPDjS5BypassMode = 0;                       // CLDO_VDDP Bypass DJ S5 Mode
  Setup_Config->CbsCmnCldoVDDPEmHmiBypassMode = 0;                      // CLDO_VDDP Bypass EM HMI Mode
  Setup_Config->CbsCmnCldoVDDPEmCfopBypassMode = 0;                     // CLDO_VDDP Bypass EM CFOP Mode
  Setup_Config->CbsCmnCldoVDDPCddHmiBypassMode = 0;                     // CLDO_VDDP Bypass CDD HMI Mode
  Setup_Config->CbsCmnCldoVDDPCddDdrBypassMode = 0;                     // CLDO_VDDP Bypass CDD DDR Mode
  
  Setup_Config->CbsCmnCldoVDDMBypass_MODE = 0;                          // CLDO_VDDM Bypass Mode
//Setup_Config->CbsCmnAvsCtl = 0;                                       // AVS Control                                           // byo231012 -
  Setup_Config->CbsDldoPsmMarginCtl = 0;                                // DldoPsmMargin Control
  Setup_Config->CbsDldoPsmMargin = 12;                                  // DldoPsmMarginVoltage
  Setup_Config->CbsCmnGnbHdAudioEn     = 0xF;                           // HD Audio Enable
  Setup_Config->CbsCfgPcieLoopbackMode = 2;                             // Block PCIe Loopback
  Setup_Config->CbsCfgPcieCrsDelay     = 6;                             // CRS Delay
  Setup_Config->CbsCfgPcieCrsLimit     = 6;                             // CRS Limit
  Setup_Config->CbsCmnGnbNbIOMMU = 0xf;                                 // IOMMU
  Setup_Config->CbsXhmiLinkWidth = 0;                                   // xHMI Link Width
  Setup_Config->CbsPcieAspmControl = 0x0;                               // PCIe Root Port ASPM Control
  Setup_Config->CbsPcieEqMode    = 0x0;                                 // PCIe Eq Mode Control
  Setup_Config->CbsPcie4Symbol   = 0xff;                                // PCIe 4Symbol
  Setup_Config->CbsPcieGen5Precoding   = 0xff;                          // PCIe Gen5 Precoding
  Setup_Config->CbsPcieMaxpayloadSize   = 0xff;                         // PCIe Max Payload Size
  Setup_Config->CbsTcdxRouting   = 0;                                   // TCDX Routing Method
  Setup_Config->CbsCmnDisableSideband   = 0;                            // Ignore sideband
  Setup_Config->CbsCmnDisableL1wa       = 0;                            // Disable L1 w/a
  Setup_Config->CbsCmnDisableBridgeDis  = 0;                            // Disable BridgeDis
  Setup_Config->CbsCmnDisableIrqPoll    = 0;                            // Disable irq polling
  Setup_Config->CbsCmnIrqSetsBridgeDis  = 0;                            // IRQ sets BridgeDis
  Setup_Config->CbsPcieHotplugSupport   = 0;                            // PCIE hotplug
  Setup_Config->CbsCmnFchSataEnable0 = 0xf;                             // SATA Controller 0
  Setup_Config->CbsCmnFchSataEnable1 = 0xf;                             // SATA Controller 1
  Setup_Config->CbsCmnFchSataEnable2 = 0xf;                             // SATA Controller 2
  Setup_Config->CbsCmnFchSataEnable3 = 0xf;                             // SATA Controller 3
  Setup_Config->CbsCmnFchSataClass      = 2;                            // SATA Mode
  Setup_Config->CbsCmnFchSataRasSupport = 0xf;                          // Sata RAS Support
  Setup_Config->CbsCmnFchSataAhciDisPrefetchFunction = 0xf;             // Sata Disabled AHCI Prefetch Function
  Setup_Config->CbsDbgFchSataAggresiveDevSlpP0 = 0xF;                   // Aggresive SATA Device Sleep Port 0
  Setup_Config->CbsDbgFchSataDevSlpPort0Num    = 0;                     // DevSleep0 Port Number
  Setup_Config->CbsDbgFchSataAggresiveDevSlpP1 = 0xF;                   // Aggresive SATA Device Sleep Port 1
  Setup_Config->CbsDbgFchSataDevSlpPort1Num = 0;                        // DevSleep1 Port Number
  Setup_Config->CbsCmnFchUsbXHCI0Enable = 0xf;                          // XHCI controller0 enable
  Setup_Config->CbsCmnFchUsbXHCI1Enable = 0xf;                          // XHCI Controller1 enable 
  Setup_Config->CbsCmnFchUsbXHCI2Enable = 0xf;                          // XHCI controller2 enable
  Setup_Config->CbsCmnFchUsbXHCI3Enable = 0xf;                          // XHCI controller3 enable
  Setup_Config->CbsCmnFchUsbXHCI4Enable = 0xf;                          // XHCI controller4 enable
  Setup_Config->CbsCmnFchUsbXHCI5Enable = 0xf;                          // XHCI controller5 enable
  Setup_Config->CbsCmnFchUsbXHCI6Enable = 0xf;                          // XHCI controller6 enable
  Setup_Config->CbsCmnFchUsbXHCI7Enable = 0xf;                          // XHCI controller7 enable
  Setup_Config->CbsCmnFchSystemPwrFailShadow = 0;                       // Ac Loss Control
  Setup_Config->CbsCmnFchI2C0Config  = 0xf;                             // I2C 0 Enable
  Setup_Config->CbsCmnFchI2C1Config  = 0xf;                             // I2C 1 Enable
  Setup_Config->CbsCmnFchI2C2Config  = 0xf;                             // I2C 2 Enable
  Setup_Config->CbsCmnFchI2C3Config  = 0xf;                             // I2C 3 Enable
  Setup_Config->CbsCmnFchI2C4Config = 0xf;                              // I2C 4 Enable
  Setup_Config->CbsCmnFchI2C5Config = 0xf;                              // I2C 5 Enable
  Setup_Config->CbsCmnFchI3C0Config  = 0xf;                             // I3C 0 Enable
  Setup_Config->CbsCmnFchI3C1Config  = 0xf;                             // I3C 1 Enable
  Setup_Config->CbsCmnFchUart0Config = 1;                               // Uart 0 Enable
  Setup_Config->CbsCmnFchUart0LegacyConfig = 4;                         // Uart 0 Legacy Options
  Setup_Config->CbsCmnFchUart1Config = 0xf;                             // Uart 1 Enable
  Setup_Config->CbsCmnFchUart1LegacyConfig = 0;                         // Uart 1 Legacy Options
  Setup_Config->CbsCmnFchUart2Config = 0xf;                             // Uart 2 Enable (no HW FC)
  Setup_Config->CbsCmnFchUart2LegacyConfig = 0;                         // Uart 2 Legacy Options
  Setup_Config->CbsCmnFchUart3Config = 0xf;                             // Uart 3 Enable (no HW FC)
  Setup_Config->CbsCmnFchUart3LegacyConfig = 0;                         // Uart 3 Legacy Options
  Setup_Config->CbsCmnNtbEnable     = 0;                                // NTB Enable
  Setup_Config->CbsCmnNtbLocation   = 0xFF;                             // NTB Location
  Setup_Config->CbsCmnNtbPCIeCore   = 0xf;                              // NTB active on PCIeCore
  Setup_Config->CbsCmnNtbMode = 0xf;                                    // NTB Mode
  Setup_Config->CbsCmnNtbLinkSpeed         = 0xf;                       // Link Speed
  Setup_Config->CbsCmnNtbBAR1Window        = 0xff;                      // bar1 window size
  Setup_Config->CbsCmnNtbBAR23Window       = 0xff;                      // bar1 window size
  Setup_Config->CbsCmnNtbBAR45Window       = 0xff;                      // bar1 window size
  Setup_Config->CbsValidationDdrParity     = 0xF;                       // Ddr Parity Control Only Used for Validation
  Setup_Config->CbsValidationPspCcpVqCount = 0;                         // PSPCCP VQ count for validation
  Setup_Config->CbsGnbxHmiPHYRateCFG       = 0xff;                      // xHMI PHY Rate
  Setup_Config->CbsGnbHmiPHYRateCFG        = 0xff;                      // HMI PHY Rate
  Setup_Config->CbsDfCmnMcaBankCtrl        = 3;                         // MCA Bank Control
  Setup_Config->CbsHBLCntl     = 0x0;                                   // HBL Log Control                                       // byo231012 -
  Setup_Config->CbsHBLDieType  = 0x2;                                   // HBL Die Type
  Setup_Config->CbsHBLSOCKETID = 0x0;                                   // SOCKET ID
  Setup_Config->CbsHBLIODID    = 0x0;                                   // IOD ID
  Setup_Config->CbsHBLCDDID    = 0x0;                                   // CDD ID
  Setup_Config->CbsPspLogCategory = 1;                                  /// psp log category
  Setup_Config->CbsPspLogLevel    = 3;                                  /// psp log level
  Setup_Config->CbsMemEyeTest     = 0;                                  /// mem eye test
  Setup_Config->AutoRefFineGranMode = 1;                                /// Refresh Mode
  Setup_Config->CbsTpcmControl  = 0x0;                                  // Tpcm Control
  Setup_Config->CbsPsfControl   = 0x0;                                  // Psf Control
  Setup_Config->CbsSerialPortCtl   = 0x1;                               // Serial Port Select
  Setup_Config->CbsCmnAvsCtl   = 0x1;                                   // AVS Control
  Setup_Config->CbsQoSControl   = 0x0;                                  // QoS Control
  Setup_Config->CbsPostCompletePin = 0x1D00;                            // Post complete pin
  Setup_Config->CbsCmnCxlControl  = 0xFF;                               // CXL Control
  Setup_Config->CbsCmnCxlMemAttr  = 0x01;                               // CXL memory attribute
  Setup_Config->CbsCmnSPISpeedCtrl = 0xf;                               // SPI Speed Value of HPCB_TOKEN_CBS_CMN_BIOS_SPI_SPEED_MODE_VALUE
}
