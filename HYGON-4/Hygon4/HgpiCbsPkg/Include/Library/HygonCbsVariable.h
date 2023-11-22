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

#ifndef _HYGON_CBS_VARIABLE_H_
#define _HYGON_CBS_VARIABLE_H_

#pragma pack(push,1)

typedef struct _CBS_CONFIG {
  UINT8     CbsCmnCpuPfeh;                                    ///< Platform First Error Handling
  UINT8     CbsCmnCpuMcaErrThreshEn;                          ///< MCA error thresh enable
  UINT16    CbsCmnCpuMcaErrThreshCount;                       ///< MCA error thresh count
  UINT8     CbsCmnCpuCpb;                                     ///< Core Performance Boost
  UINT8     CbsCmnCpuEnIbs;                                   ///< Enable IBS
  UINT8     CbsCmnCpuCstateCtrl;                              ///< CPU C-state Control
  UINT8     CbsCmnSmtMode;                                    ///< SMT Mode Control
  UINT8     CbsCmnCpuOpcacheCtrl;                             ///< Opcache Control
  UINT8     CbsCmnCpuOcMode;                                  ///< OC Mode
  UINT8     CbsCpuModel;                                      ///< One of CPU Model
  UINT8     CbsCpuPkgType;                                    ///< Cpu Pkg Type
  UINT32    CbsCmnCpuCsvAsidSpaceLimit;                       ///< CSV-ES ASID Space Limit
  UINT8     CbsCmnCpuStreamingStoresCtrl;                     ///< Streaming Stores Control
  UINT8     CbsCmnCpuLApicMode;                               ///< Local APIC Mode
  UINT8     CbsCpuPstCustomP0;                                ///< Custom Pstate0
  UINT32    CbsCpuCofP0;                                      ///< Frequency (MHz)
  UINT32    CbsCpuVoltageP0;                                  ///< Voltage (uV)
  UINT8     CbsCpuPst0Fid;                                    ///< Pstate0 FID
  UINT8     CbsCpuPst0Did;                                    ///< Pstate0 DID
  UINT8     CbsCpuPst0Vid;                                    ///< Pstate0 VID
  UINT8     CbsCpuPstCustomP1;                                ///< Custom Pstate1
  UINT32    CbsCpuCofP1;                                      ///< Frequency (MHz)
  UINT32    CbsCpuVoltageP1;                                  ///< Voltage (uV)
  UINT8     CbsCpuPst1Fid;                                    ///< Pstate1 FID
  UINT8     CbsCpuPst1Did;                                    ///< Pstate1 DID
  UINT8     CbsCpuPst1Vid;                                    ///< Pstate1 VID
  UINT8     CbsCpuPstCustomP2;                                ///< Custom Pstate2
  UINT32    CbsCpuCofP2;                                      ///< Frequency (MHz)
  UINT32    CbsCpuVoltageP2;                                  ///< Voltage (uV)
  UINT8     CbsCpuPst2Fid;                                    ///< Pstate2 FID
  UINT8     CbsCpuPst2Did;                                    ///< Pstate2 DID
  UINT8     CbsCpuPst2Vid;                                    ///< Pstate2 VID
  UINT8     CbsCmnCpuGenDowncoreCtrl;                         ///< Downcore control
  UINT8     CbsCmnCpuL1StreamHwPrefetcher;                    ///< L1 Stream HW Prefetcher
  UINT8     CbsCmnCpuL2StreamHwPrefetcher;                    ///< L2 Stream HW Prefetcher
  UINT8     CbsCmnCoreWatchdogCtrl;                           ///< Core Watchdog Control
  UINT8     CbsDfCmnDramScrubTime;                            ///< DRAM scrub time
  UINT8     CbsDfCmnRedirScrubCtrl;                           ///< Redirect scrubber control
  UINT8     CbsDfCmnSyncFloodProp;                            ///< Disable DF sync flood propagation
  UINT8     CbsDfCmnFreezeQueueError;                         ///< Freeze DF module queues on error
  UINT8     CbsDfCmnHmiEncryption;                            ///< HMI encryption control
  UINT8     CbsDfCmnXHmiEncryption;                           ///< xHMI encryption control
  UINT8     CbsDfCmnCc6MemEncryption;                         ///< CC6 memory region encryption
  UINT8     CbsDfCmnCc6AllocationScheme;                      ///< Location of private memory regions
  UINT8     CbsDfCmnSysProbeFilter;                           ///< System probe filter
  UINT8     CbsDfCmnMemIntlv;                                 ///< Memory interleaving
  UINT8     CbsDfCmnMemIntlvSize;                             ///< Memory interleaving size
  UINT8     CbsDfCmnChnlIntlvHash;                            ///< Channel interleaving hash
  UINT8     CbsDfCmnSubchannelIntlvBit;                       ///< Subchannel interleaving bit
  UINT8     CbsDfCmnMemClear;                                 ///< Memory Clear
  UINT8     CbsFclkUclkSyncMode;                              ///< FclkUclkSyncMode
  UINT8     CbsDfNumaNodePerSocket;                           ///< NUMA node per socket
  UINT8     CbsDfCmnCstateCtrl;                               ///< DF C-state Control
  UINT8     CbsCmnMemOverclockDdr5;                           ///< Overclock
  UINT8     CbsCmnMemSpeedDdr5;                               ///< Memory Clock Speed
  UINT8     CbsCmnMemCtrller2TModeDdr5;                       ///< Cmd2T
  UINT8     CbsCmnMemGearDownModeDdr5;                        ///< Gear Down Mode
  UINT8     CbsCmnMemCtrllerPwrDnEnDdr5;                      ///< Power Down Enable
  UINT8     CbsCmnMemDataBusConfigCtlDdr5;                    ///< Data Bus Configuration User Controls
  UINT8     CbsCmnMemCtrllerPhyDrvStrenDqdqs;                 ///< phydrvstrenQqdqs
  UINT8     CbsCmnMemCtrllerPhyOdtStrenDqDqs;                 ///< PhyOdtStrenDqDqs
  UINT8     CbsCmnMemCtrllerRttNomRDDdr5;                     ///< RttNom
  UINT8     CbsCmnMemCtrllerRttWrDdr5;                        ///< RttWr
  UINT8     CbsCmnMemCtrllerRttNomWrDdr5;                     ///< RttNomWr
  UINT8     CbsCmnMemCtrllerRttParkDdr5;                      ///< RttPark
  UINT8     CbsCmnMemDataPoisoningDdr5;                       ///< Data Poisoning
  UINT8     CbsCmnMemCtrllerDramEccSymbolSizeDdr5;            ///< DRAM ECC Symbol Size
  UINT8     CbsCmnMemCtrllerDramEccEnDdr5;                    ///< DRAM ECC Enable
  UINT8     CbsCmnMemTsmeDdr5;                                ///< TSME
  UINT8     CbsCmnMemCtrllerDataScrambleDdr5;                 ///< Data Scramble
  UINT8     CbsCmnMemMappingBankInterleaveDdr5;               ///< Chipselect Interleaving
  UINT8     CbsCmnMemCtrllerBankGroupSwapDdr5;                ///< BankGroupSwap
  UINT8     CbsCmnMemAddressHashBankDdr5;                     ///< Address Hash Bank
  UINT8     CbsCmnMemAddressHashCsDdr5;                       ///< Address Hash CS
  UINT8     CbsCmnMemMbistEn;                                 ///< MBIST Enable
  UINT8     CbsCmnMemMbistTestmode;                           ///< MBIST Test Mode
  UINT8     CbsCmnMemMbistAggressors;                         ///< MBIST Aggressors
  UINT8     CbsCmnMemMbistPerBitSlaveDieReport;               ///< MBIST Per Bit Slave Die Reporting
  UINT8     CbsDbgPoisonConsumption;                          ///< NBIO Internal Poison Consumption
  UINT8     CbsNbioRASControl;                                ///< NBIO RAS Control
  UINT8     CbsCmncTDPCtl;                                    ///< cTDP Control
  UINT32    CbsCmncTDPLimit;                                  ///< cTDP
  UINT8     CbsCmnTDPBoost;                                   ///< TDP Boost
  UINT8     CbsCmnNbioEfficiencyOptimizedMode;                ///< Efficiency Optimized Mode
  UINT8     CbsCmnNbioPSIDisable;                             ///< PSI
  UINT8     CbsDbgGnbDbgACSEnable;                            ///< ACS Enable
  UINT8     CbsGnbDbgPcieAriSupport;                          ///< PCIe ARI Support
  UINT8     CbsCmnCLDO_VDDMCtl;                               ///< CLDO_VDDM Control
  UINT32    CbsCmnCLDOVDDMvoltage;                            ///< CLDO_VDDM voltage
  // HYGON_EX_CPU
  UINT8     CbsCmnCLDO_VDDPCtl;                               ///< CLDO_VDDP Control
  UINT32    CbsCmnCLDOVDDPvoltage;                            ///< CLDO_VDDP voltage
  UINT16    CbsCmnCLDOVDDPvoltageIODHmiPHY;                   ///< CLDO_VDDP voltage IOD HMI PHY
  UINT16    CbsCmnCLDOVDDPvoltageCDDDDRPHY;                   ///< CLDO_VDDP voltage CDD DDR PHY
  UINT16    CbsCmnCLDOVDDPvoltageCDDHmiCFOPPHY;               ///< CLDO_VDDP voltage CDD HMI CFOP PHY
  UINT8     CbsCmnCldoVDDPBypass;                             ///< CLDO_VDDP Bypass Control
  UINT8     CbsCmnCldoVDDPBypassIODHMI_MODE;                  ///< CLDO_VDDP BypassIODHMI
  UINT8     CbsCmnCldoVDDPBypassCDDHMI_MODE;                  ///< CLDO_VDDP BypassCDDHMI
  UINT8     CbsCmnCldoVDDPBypassCDDDDR_MODE;                  ///< CLDO_VDDP BypassCDDDDR

  // HYGON_GX_CPU
  UINT8     CbsCmnDjCLDO_VDDPCtl;                             ///< DJ CLDO_VDDP Control
  UINT16    CbsCmnDjCLDOVDDPvoltageHmi;                       ///< DJ HMI VID
  UINT16    CbsCmnDjCLDOVDDPvoltageS5;                        ///< DJ S5 VID
  UINT8     CbsCmnEmCLDO_VDDPCtl;                             ///< EM CLDO_VDDP Control
  UINT16    CbsCmnEmCLDOVDDPvoltageHmi;                       ///< EM HMI VID
  UINT16    CbsCmnEmCLDOVDDPvoltageCfop;                      ///< EM CFOP VID
  UINT8     CbsCmnCddCLDO_VDDPCtl;                            ///< CDD CLDO_VDDP Control
  UINT16    CbsCmnCddCLDOVDDPvoltageHmi;                      ///< CDD HMI VID
  UINT16    CbsCmnCddCLDOVDDPvoltageDDR;                      ///< CDD DDR VID
  UINT8     CbsCmnCldoVDDPBypassCtl;                          ///< CLDO_VDDP Bypass Control
  UINT8     CbsCmnCldoVDDPDjHmiBypassMode;                    ///< CLDO_VDDP Bypass DJ HMI
  UINT8     CbsCmnCldoVDDPDjS5BypassMode;                     ///< CLDO_VDDP Bypass DJ S5
  UINT8     CbsCmnCldoVDDPEmHmiBypassMode;                    ///< CLDO_VDDP Bypass EM HMI
  UINT8     CbsCmnCldoVDDPEmCfopBypassMode;                   ///< CLDO_VDDP Bypass EM CFOP
  UINT8     CbsCmnCldoVDDPCddHmiBypassMode;                   ///< CLDO_VDDP Bypass CDD HMI
  UINT8     CbsCmnCldoVDDPCddDdrBypassMode;                   ///< CLDO_VDDP Bypass CDD DDR
  
  UINT8     CbsCmnCldoVDDMBypass_MODE;                        ///< CLDO_VDDM Bypass
  UINT8     CbsCmnAvsCtl;                                     ///< AVS Control
  UINT8     CbsDldoPsmMarginCtl;                              ///< DldoPsmMargin Control
  UINT8     CbsDldoPsmMargin;                                 ///< DldoPsmMarginVoltage
  UINT8     CbsCmnGnbHdAudioEn;                               ///< HD Audio Enable
  UINT8     CbsCfgPcieLoopbackMode;                           ///< Block PCIe Loopback
  UINT16    CbsCfgPcieCrsDelay;                               ///< CRS Delay
  UINT16    CbsCfgPcieCrsLimit;                               ///< CRS Limit
  UINT8     CbsCmnGnbNbIOMMU;                                 ///< IOMMU
  UINT8     CbsXhmiLinkWidth;                                 ///< xHMI Link Width Control
  UINT8     CbsPcieAspmControl;                               ///< PCIe Root Port ASPM Control
  UINT8     CbsPcieEqMode;                                    ///< PCIe Eq Mode Control
  UINT8     CbsPcie4Symbol;                                   ///< PCIe 4 Symbol Control
  UINT8     CbsPcieGen5Precoding;                             ///< PCIe Gen5 Precoding
  UINT8     CbsPcieMaxpayloadSize;                            ///< PCIe Max Payload Size
  UINT8     CbsTcdxRouting;                                   ///< TCDX Routing Method
  UINT8     CbsCmnDisableSideband;                            ///< Ignore sideband
  UINT8     CbsCmnDisableL1wa;                                ///< Disable L1 w/a
  UINT8     CbsCmnDisableBridgeDis;                           ///< Disable BridgeDis
  UINT8     CbsCmnDisableIrqPoll;                             ///< Disable irq polling
  UINT8     CbsCmnIrqSetsBridgeDis;                           ///< IRQ sets BridgeDis
  UINT8     CbsPcieHotplugSupport;                            ///< PCIE hotplug support
  UINT8     CbsPcieSITestMode;                                ///< PCIE SI Test mode
  UINT8     CbsCmnFchSataClass;                               ///< Sata Class
  UINT8     CbsCmnFchSataEnable0;                             ///< SATA Controller 0
  UINT8     CbsCmnFchSataEnable1;                             ///< SATA Controller 1
  UINT8     CbsCmnFchSataEnable2;                             ///< SATA Controller 2
  UINT8     CbsCmnFchSataEnable3;                             ///< SATA Controller 3
  UINT8     CbsCmnFchSataRasSupport;                          ///< Sata RAS Support
  UINT8     CbsCmnFchSataAhciDisPrefetchFunction;             ///< Sata Disabled AHCI Prefetch Function
  UINT8     CbsDbgFchSataAggresiveDevSlpP0;                   ///< Aggresive SATA Device Sleep Port 0
  UINT8     CbsDbgFchSataDevSlpPort0Num;                      ///< DevSleep0 Port Number
  UINT8     CbsDbgFchSataAggresiveDevSlpP1;                   ///< Aggresive SATA Device Sleep Port 1
  UINT8     CbsDbgFchSataDevSlpPort1Num;                      ///< DevSleep1 Port Number
  UINT8     CbsCmnFchUsbXHCI0Enable;                          ///< XHCI controller0 enable
  UINT8     CbsCmnFchUsbXHCI1Enable;                          ///< XHCI controller1 enable
  UINT8     CbsCmnFchUsbXHCI2Enable;                          ///< XHCI controller2 enable
  UINT8     CbsCmnFchUsbXHCI3Enable;                          ///< XHCI controller3 enable
  UINT8     CbsCmnFchUsbXHCI4Enable;                          ///< XHCI controller4 enable
  UINT8     CbsCmnFchUsbXHCI5Enable;                          ///< XHCI controller5 enable
  UINT8     CbsCmnFchUsbXHCI6Enable;                          ///< XHCI controller6 enable
  UINT8     CbsCmnFchUsbXHCI7Enable;                          ///< XHCI controller7 enable
  UINT8     CbsCmnFchSystemPwrFailShadow;                     ///< Ac Loss Control
  UINT8     CbsCmnFchI2C0Config;                              ///< I2C 0 Enable
  UINT8     CbsCmnFchI2C1Config;                              ///< I2C 1 Enable
  UINT8     CbsCmnFchI2C2Config;                              ///< I2C 2 Enable
  UINT8     CbsCmnFchI2C3Config;                              ///< I2C 3 Enable
  UINT8     CbsCmnFchI2C4Config;                              ///< I2C 4 Enable
  UINT8     CbsCmnFchI2C5Config;                              ///< I2C 5 Enable
  UINT8     CbsCmnFchI3C0Config;                              ///< I3C 0 Enable
  UINT8     CbsCmnFchI3C1Config;                              ///< I3C 1 Enable
  UINT8     CbsCmnFchUart0Config;                             ///< Uart 0 Enable
  UINT8     CbsCmnFchUart0LegacyConfig;                       ///< Uart 0 Legacy Options
  UINT8     CbsCmnFchUart1Config;                             ///< Uart 1 Enable
  UINT8     CbsCmnFchUart1LegacyConfig;                       ///< Uart 1 Legacy Options
  UINT8     CbsCmnFchUart2Config;                             ///< Uart 2 Enable (no HW FC)
  UINT8     CbsCmnFchUart2LegacyConfig;                       ///< Uart 2 Legacy Options
  UINT8     CbsCmnFchUart3Config;                             ///< Uart 3 Enable (no HW FC)
  UINT8     CbsCmnFchUart3LegacyConfig;                       ///< Uart 3 Legacy Options
  UINT8     CbsCmnNtbEnable;                                  ///< NTB Enable
  UINT8     CbsCmnNtbLocation;                                ///< NTB Location
  UINT8     CbsCmnNtbPCIeCore;                                ///< NTB active on PCIeCore
  UINT8     CbsCmnNtbMode;                                    ///< NTB Mode
  UINT8     CbsCmnNtbLinkSpeed;                               ///< Link Speed
  UINT8     CbsCmnNtbBAR1Window;                              ///< BAR1 winddow size
  UINT8     CbsCmnNtbBAR23Window;                             ///< BAR23 winddow size
  UINT8     CbsCmnNtbBAR45Window;                             ///< BAR45 winddow size
  UINT8     CbsCmnCpuRdseedRdrandCtrl;                        ///< Enable CPU RdseedRdrand Feature
  UINT8     CbsCmnCpuLoadUcodeCtrl;                           ///< Enable/Disable Loading Microcode Patch
  UINT8     CbsCmnCpuRdseedRdrandCap;                         ///< Enable/Disable Rdrand/Rdseed Feature
  UINT8     CbsCmnSmeeCtrl;                                   /// Enable/Disable SMEE
  UINT8     CbsAbove4GMmioLimitBit;                           /// Set Above 64 MMIO limit bit
  UINT8     CbsCmnCpuSmeeCap;                                 /// CPU SMEE cap
  UINT8     CbsCmnSVMCtrl;                                    /// Enable/Disable SMEE
  UINT8     CbsDbgLevel;                                      /// Serail output level flag. //Hygon Debug Level
  UINT8     CbsCmnCpuForceVddcrCpuVidEn;                      /// Fource vddcr cpu enable
  UINT16    CbsCmnCpuForceVddcrCpuVid;                        /// Fource vddcr cpu vid value
  UINT16    CbsCmnVddcrCpuVoltageMargin;                      /// cpu voltage margin
  UINT8     CbsCmnForceCclkFrequencyEn;                       /// Fource cclk enable
  UINT16    CbsCmnForceCclkFrequency;                         /// Fource cclk value
  UINT8     CbsCmnDldoBypass;                                 /// Bypass dldo
  UINT8     CbsValidationDdrParity;                           /// Ddr Parity for Validation
  UINT8     CbsValidationPspCcpVqCount;                       /// PSPCCP VQ Count Configuration fro Validation
  UINT8     CbsGnbxHmiPHYRateCFG;                             /// GNB xHMI PHY Rate
  UINT8     CbsGnbHmiPHYRateCFG;                              /// GNB HMI PHY Rate
  UINT8     CbsDfCmnMcaBankCtrl;                              /// MCA Bank Control
  UINT8     CbsHBLCntl;                                       /// HBL Log Control
  UINT8     CbsHBLDieType;                                    /// HBL Log Die Select
  UINT8     CbsHBLSOCKETID;                                   /// HBL Log SOCKET ID
  UINT8     CbsHBLIODID;                                      /// HBL Log IOD ID
  UINT8     CbsHBLCDDID;                                      /// HBL Log CDD ID
  UINT8     CbsPspLogCategory;                                /// psp log category
  UINT8     CbsPspLogLevel;                                   /// psp log level
  UINT8     CbsMemEyeTest;                                    /// mem eye test
  UINT8     AutoRefFineGranMode;                              /// Refresh Mode
  UINT8     CbsTpcmControl;                                   /// Tpcm Control
  UINT8     CbsPsfControl;                                    /// CSV Control
  UINT8     CbsSerialPortCtl;                                 /// Serial Port Select
  UINT8     CbsQoSControl;                                    /// QoS Control
  UINT32    CbsPostCompletePin;                               /// Post complete pin
  UINT8     CbsCmnCxlControl;                                 /// CXL control
  UINT8     CbsCmnCxlMemAttr;                                 /// CXL memory attribute
  UINT8     CbsCmnSPISpeedCtrl;                               ///< SPISpeed
  UINT8     Reserved[32];                                     // byo230914 +
} CBS_CONFIG;

#pragma pack(pop)

#endif // _HYGON_CBS_VARIABLE_H_
