/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH Function Support Definition
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: FCH
 *
 */
/*;********************************************************************************
;
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
;*********************************************************************************/

#ifndef _FCH_COMMON_CFG_H_
#define _FCH_COMMON_CFG_H_

#pragma pack (push, 1)

// -----------------------------------------------------------------------------
// FCH DEFINITIONS AND MACROS
// -----------------------------------------------------------------------------

//
// FCH Component Data Structure Definitions
//

#define NBIO_COUNT_PER_SATORI 4

#include <FchImc.h>
#include <FchXgbe.h>
#include <HGPI.h>
#include <HYGON.h>
#include <TSFch.h>

///
/// PCIE Reset Block
///
typedef enum {
  NbBlock,                                              ///< Reset for NB PCIE
  FchBlock                                              ///< Reset for FCH GPP
} RESET_BLOCK;

///
/// PCIE Reset Operation
///
typedef enum {
  DeassertReset,                                        ///< DeassertRese - Deassert reset
  AssertReset                                           ///< AssertReset - Assert reset
} RESET_OP;

///
/// Fch Run Time Parameters
///
typedef struct {
  UINT32    PcieMmioBase;                                          ///< PcieMmioBase
  UINT32    FchDeviceEnableMap;                                    ///< FchDeviceEnableMap
  UINT32    FchDeviceD3ColdMap;                                    ///< FchDeviceD3ColdMap
  UINT16    XHCI_PMx04_XhciFwRomAddr_Rom;                          ///< XHCI_PMx04_XhciFwRomAddr_Rom
  UINT32    XHCI_PMx08_xHCI_Firmware_Addr_1_Rom;                   ///< XHCI_PMx08_xHCI_Firmware_Addr_1_Ram
  UINT16    XHCI_PMx04_XhciFwRomAddr_Ram;                          ///< XHCI_PMx04_XhciFwRomAddr_Rom
  UINT32    XHCI_PMx08_xHCI_Firmware_Addr_1_Ram;                   ///< XHCI_PMx08_xHCI_Firmware_Addr_1_Ram
  UINT8     SataDevSlpPort0S5Pin;                                  ///< SataDevSlpPort0S5Pin - Reserved
  UINT8     SataDevSlpPort1S5Pin;                                  ///< SataDevSlpPort1S5Pin - Reserved
  UINT16    Al2AhbLegacyUartIoEnable;                              ///< Al2Ahb Legacy Uart Io Enable
} FCH_RUNTIME;

///
/// _SPI_DEVICE_PROFILE Spi Device Profile structure
///
typedef struct _SPI_DEVICE_PROFILE {
  UINT32    JEDEC_ID;                                   /// JEDEC ID
  UINT32    RomSize;                                    /// ROM Size
  UINT32    SectorSize;                                 /// Sector Size
  UINT16    MaxNormalSpeed;                             /// Max Normal Speed
  UINT16    MaxFastSpeed;                               /// Max Fast Speed
  UINT16    MaxDualSpeed;                               /// Max Dual Speed
  UINT16    MaxQuadSpeed;                               /// Max Quad Speed
  UINT8     QeReadRegister;                             /// QE Read Register
  UINT8     QeWriteRegister;                            /// QE Write Register
  UINT8     QeOperateSize;                              /// QE Operate Size 1byte/2bytes
  UINT16    QeLocation;                                 // QE Location in the register
} SPI_DEVICE_PROFILE;

///
/// _SPI_CONTROLLER_PROFILE Spi Device Profile structure
///
typedef struct _SPI_CONTROLLER_PROFILE {
  // UINT32                SPI_CONTROLLER_ID;                       /// SPI Controller ID
  UINT16    FifoSize;                                   /// FIFO Size
  UINT16    MaxNormalSpeed;                             /// Max Normal Speed
  UINT16    MaxFastSpeed;                               /// Max Fast Speed
  UINT16    MaxDualSpeed;                               /// Max Dual Speed
  UINT16    MaxQuadSpeed;                               /// Max Quad Speed
} SPI_CONTROLLER_PROFILE;

///
/// SPI structure
///
typedef struct {
  BOOLEAN    LpcMsiEnable;                              ///< LPC MSI capability
  UINT32     LpcSsid;                                   ///< LPC Subsystem ID
  UINT32     RomBaseAddress;                            ///< SpiRomBaseAddress
                                                        /// @par
                                                        /// SPI ROM BASE Address
                                                        ///
  UINT8      SpiSpeed;                                  ///< SpiSpeed - Spi Frequency
                                                        /// @par
                                                        /// SPI Speed [1.0] - the clock speed for non-fast read command
                                                        /// @li <b>00</b> - 66Mhz
                                                        /// @li <b>01</b> - 33Mhz
                                                        /// @li <b>10</b> - 22Mhz
                                                        /// @li <b>11</b> - 16.5Mhz
                                                        ///
  UINT8      SpiFastSpeed;                              ///< FastSpeed  - Spi Fast Speed feature
                                                        /// SPIFastSpeed [1.0] - the clock speed for Fast Speed Feature
                                                        /// @li <b>00</b> - 66Mhz
                                                        /// @li <b>01</b> - 33Mhz
                                                        /// @li <b>10</b> - 22Mhz
                                                        /// @li <b>11</b> - 16.5Mhz
                                                        ///
  UINT8      WriteSpeed;                                ///< WriteSpeed - Spi Write Speed
                                                        /// @par
                                                        /// WriteSpeed [1.0] - the clock speed for Spi write command
                                                        /// @li <b>00</b> - 66Mhz
                                                        /// @li <b>01</b> - 33Mhz
                                                        /// @li <b>10</b> - 22Mhz
                                                        /// @li <b>11</b> - 16.5Mhz
                                                        ///
  UINT8      SpiMode;                                   ///< SpiMode    - Spi Mode Setting
                                                        /// @par
                                                        /// @li <b>101</b> - Qual-io 1-4-4
                                                        /// @li <b>100</b> - Dual-io 1-2-2
                                                        /// @li <b>011</b> - Qual-io 1-1-4
                                                        /// @li <b>010</b> - Dual-io 1-1-2
                                                        /// @li <b>111</b> - FastRead
                                                        /// @li <b>110</b> - Normal
                                                        ///
  UINT8                 AutoMode;                       ///< AutoMode   - Spi Auto Mode
                                                        /// @par
                                                        /// SPI Auto Mode
                                                        /// @li <b>0</b> - Disabled
                                                        /// @li <b>1</b> - Enabled
                                                        ///
  UINT8                 SpiBurstWrite;                  ///< SpiBurstWrite - Spi Burst Write Mode
                                                        /// @par
                                                        /// SPI Burst Write
                                                        /// @li <b>0</b> - Disabled
                                                        /// @li <b>1</b> - Enabled
  BOOLEAN               LpcClk0;                        ///< Lclk0En - LPCCLK0
                                                        /// @par
                                                        /// LPC Clock 0 mode
                                                        /// @li <b>0</b> - forced to stop
                                                        /// @li <b>1</b> - functioning with CLKRUN protocol
  BOOLEAN               LpcClk1;                        ///< Lclk1En - LPCCLK1
                                                        /// @par
                                                        /// LPC Clock 1 mode
                                                        /// @li <b>0</b> - forced to stop
                                                        /// @li <b>1</b> - functioning with CLKRUN protocol
  // UINT32                SPI100_RX_Timing_Config_Register_38;                 ///< SPI100_RX_Timing_Config_Register_38
  // UINT16                SPI100_RX_Timing_Config_Register_3C;                 ///< SPI100_RX_Timing_Config_Register_3C
  // UINT8                 SpiProtectEn0_1d_34;                  ///
  UINT8                 SPI100_Enable;                  ///< Spi 100 Enable
  SPI_DEVICE_PROFILE    SpiDeviceProfile;               ///< Spi Device Profile
} FCH_SPI;

///
/// SPI structure
///
typedef struct {
  BOOLEAN                EspiIo80Enable;                ///< ESPI 80h Port - 0:disable, 1:enable
  BOOLEAN                EspiKbc6064Enable;             ///< ESPI 60h/65h  - 0:disable, 1:enable
  BOOLEAN                EspiEc0Enable;                 ///< ESPI EC  - 0:disable, 1:enable
} FCH_ESPI;

///
/// IDE structure
///
typedef struct {
  BOOLEAN    IdeEnable;                                 ///< IDE function switch
  BOOLEAN    IdeMsiEnable;                              ///< IDE MSI capability
  UINT32     IdeSsid;                                   ///< IDE controller Subsystem ID
} FCH_IDE;

///
/// IR Structure
///
typedef struct {
  IR_CONFIG    IrConfig;                                ///< IrConfig
  UINT8        IrPinControl;                            ///< IrPinControl
} FCH_IR;

///
/// PCI Bridge Structure
///
typedef struct {
  BOOLEAN    PcibMsiEnable;                             ///< PCI-PCI Bridge MSI capability
  UINT32     PcibSsid;                                  ///< PCI-PCI Bridge Subsystem ID
  UINT8      PciClks;                                   ///< 33MHz PCICLK0/1/2/3 Enable, bits [0:3] used
                                                        /// @li <b>0</b> - disable
                                                        /// @li <b>1</b> - enable
                                                        ///
  UINT16     PcibClkStopOverride;                       ///< PCIB_CLK_Stop Override
  BOOLEAN    PcibClockRun;                              ///< Enable the auto clkrun functionality
                                                        /// @li <b>0</b> - disable
                                                        /// @li <b>1</b> - enable
                                                        ///
} FCH_PCIB;

///
/// - SATA Phy setting structure
///
typedef struct _SATA_PHY_SETTING {
  UINT16    PhyCoreControlWord;                         /// PhyCoreControlWord - Reserved
  UINT32    PhyFineTuneDword;                           /// PhyFineTuneDword - Reserved
} SATA_PHY_SETTING;

///
/// SATA main setting structure
///
typedef struct _SATA_ST {
  UINT8     SataModeReg;                                ///< SataModeReg - Sata Controller Mode
  UINT32    SataEnable;                                 ///< SATA controller function
                                                        ///<   bit0~bit3 - Socket0 IOHC0~3, bit4~bit7 - Socket1 IOHC0~3 ... bit27~bit31 - Socket7 IOHC0~3
                                                        ///<   @li <b>FALSE</b> - SATA controller is disabled
                                                        ///<   @li <b>TRUE</b> - SATA controller is enabled
                                                        ///
  UINT8      Sata6AhciCap;                              ///< Sata6AhciCap - Reserved */
  BOOLEAN    SataSetMaxGen2;                            ///< SataSetMaxGen2 - Set Sata Max Gen2 mode
                                                        /// @par
                                                        /// Sata Controller Set to Max Gen2 mode
                                                        /// @li <b>0</b> - disable
                                                        /// @li <b>1</b> - enable
                                                        ///
  BOOLEAN    IdeEnable;                                 ///< IdeEnable - Hidden IDE
                                                        /// @par
                                                        /// Sata IDE Controller Combined Mode
                                                        /// Enable -  SATA controller has control over Port0 through Port3,
                                                        /// IDE controller has control over Port4 and Port7.
                                                        /// Disable - SATA controller has full control of all 8 Ports
                                                        /// when operating in non-IDE mode.
                                                        /// @li <b>0</b> - enable
                                                        /// @li <b>1</b> - disable
                                                        ///
  UINT8    SataClkMode;                                 /// SataClkMode - Reserved
} SATA_ST;

///
/// SATA_PORT_ST - SATA PORT structure
///
typedef struct _SATA_PORT_ST {
  UINT8                 SataPortReg[NBIO_COUNT_PER_SATORI];    ///< Total 4 RB, SATA Port bit map - bits[0:7] for ports 0 ~ 7
                                                            ///  @li <b>0</b> - disable
                                                            ///  @li <b>1</b> - enable
                                                            ///
  BOOLEAN               Port[NBIO_COUNT_PER_SATORI][TAISHAN_SATA_PORT_NUM];          ///< PORT - 0:disable, 1:enable
} SATA_PORT_ST;

///
///< _SATA_PORT_MD - Force Each PORT to GEN1/GEN2 mode
///
typedef struct _SATA_PORT_MD {
  UINT16                SataPortMode[NBIO_COUNT_PER_SATORI];                 ///< SATA Port GEN1/GEN2 mode bit map - bits [0:15] for ports 0 ~ 7
  UINT8                 Port[NBIO_COUNT_PER_SATORI][TAISHAN_SATA_PORT_NUM];  ///< PORT0 - set BIT0 to GEN1, BIT1 - PORT0 set to GEN2
} SATA_PORT_MD;
///
/// SATA structure
///
typedef struct {
  BOOLEAN         SataMsiEnable;                        ///< SATA MSI capability
  UINT32          SataIdeSsid;                          ///< SATA IDE mode SSID
  UINT32          SataRaidSsid;                         ///< SATA RAID mode SSID
  UINT32          SataRaid5Ssid;                        ///< SATA RAID 5 mode SSID
  UINT32          SataAhciSsid;                         ///< SATA AHCI mode SSID

  SATA_ST         SataMode;                             /// SataMode - Reserved
  SATA_CLASS      SataClass;                            ///< SataClass - SATA Controller mode [2:0]
  UINT8           SataIdeMode;                          ///< SataIdeMode - Sata IDE Controller mode
                                                        /// @par
                                                        /// @li <b>0</b> - Legacy IDE mode
                                                        /// @li <b>1</b> - Native IDE mode
                                                        ///
  UINT8           SataDisUnusedIdePChannel;             ///< SataDisUnusedIdePChannel-Disable Unused IDE Primary Channel
                                                        /// @par
                                                        /// @li <b>0</b> - Channel Enable
                                                        /// @li <b>1</b> - Channel Disable
                                                        ///
  UINT8           SataDisUnusedIdeSChannel;             ///< SataDisUnusedIdeSChannel - Disable Unused IDE Secondary Channel
                                                        /// @par
                                                        /// @li <b>0</b> - Channel Enable
                                                        /// @li <b>1</b> - Channel Disable
                                                        ///
  UINT8           IdeDisUnusedIdePChannel;              ///< IdeDisUnusedIdePChannel-Disable Unused IDE Primary Channel
                                                        /// @par
                                                        /// @li <b>0</b> - Channel Enable
                                                        /// @li <b>1</b> - Channel Disable
                                                        ///
  UINT8           IdeDisUnusedIdeSChannel;              ///< IdeDisUnusedIdeSChannel-Disable Unused IDE Secondary Channel
                                                        /// @par
                                                        /// @li <b>0</b> - Channel Enable
                                                        /// @li <b>1</b> - Channel Disable
                                                        ///
  UINT8           SataOptionReserved;                   /// SataOptionReserved - Reserved

  SATA_PORT_ST    SataEspPort;                          ///< SataEspPort - SATA port is external accessible on a signal only connector (eSATA:)

  SATA_PORT_ST    SataPortPower;                        ///< SataPortPower - Port Power configuration

  SATA_PORT_MD    SataPortMd;                           ///< SataPortMd - Port Mode

  UINT8           SataAggrLinkPmCap;                    /// SataAggrLinkPmCap - 0:OFF   1:ON
  UINT8           SataPortMultCap;                      /// SataPortMultCap - 0:OFF   1:ON
  UINT8           SataClkAutoOff;                       /// SataClkAutoOff - AutoClockOff 0:Disabled, 1:Enabled
  UINT8           SataPscCap;                           /// SataPscCap 1:Enable PSC, 0:Disable PSC capability
  UINT8           BiosOsHandOff;                        /// BiosOsHandOff - Reserved
  UINT8           SataFisBasedSwitching;                /// SataFisBasedSwitching - Reserved
  UINT8           SataCccSupport;                       /// SataCccSupport - Reserved
  UINT8           SataSscCap;                           /// SataSscCap - 1:Enable, 0:Disable SSC capability
  UINT8           SataMsiCapability;                    /// SataMsiCapability 0:Hidden 1:Visible
  UINT8           SataForceRaid;                        /// SataForceRaid   0:No function 1:Force RAID
  UINT8           SataInternal100Spread;                /// SataInternal100Spread - Reserved
  UINT8           SataDebugDummy;                       /// SataDebugDummy - Reserved
  UINT8           SataTargetSupport8Device;             /// SataTargetSupport8Device - Reserved
  UINT8           SataDisableGenericMode;               /// SataDisableGenericMode - Reserved
  BOOLEAN         SataAhciEnclosureManagement;          /// SataAhciEnclosureManagement - Reserved
  UINT8           SataSgpio;                            /// SataSgpio - Reserved
  UINT8           SataPhyPllShutDown;                   /// SataPhyPllShutDown - Reserved
  BOOLEAN         SataHotRemovalEnh;                    /// SataHotRemovalEnh - Reserved

  SATA_PORT_ST    SataHotRemovalEnhPort;                ///< SataHotRemovalEnhPort - Hot Remove

  BOOLEAN         SataOobDetectionEnh;                  /// SataOobDetectionEnh - TRUE
  BOOLEAN         SataPowerSavingEnh;                   /// SataPowerSavingEnh - TRUE
  UINT8           SataMemoryPowerSaving;                /// SataMemoryPowerSaving - 0-3 Default [3]
  BOOLEAN         SataRasSupport;                       /// SataRasSupport - Support RAS function TRUE: Enable FALSE: Disable
  BOOLEAN         SataAhciDisPrefetchFunction;          /// SataAhciDisPrefetchFunction - Disable AHCI Prefetch Function Support
  BOOLEAN         SataDevSlpPort0;                      /// SataDevSlpPort0 - Reserved
  BOOLEAN         SataDevSlpPort1;                      /// SataDevSlpPort1 - Reserved
  // UINT8                 SataDevSlpPort0S5Pin;           /// SataDevSlpPort0S5Pin - Reserved
  // UINT8                 SataDevSlpPort1S5Pin;           /// SataDevSlpPort1S5Pin - Reserved
  UINT32          TempMmio;                             /// TempMmio - Reserved
  UINT8           SataDevSlpPort0Num;                   /// SataDevSlpPort0Num - Reserved
  UINT8           SataDevSlpPort1Num;                   /// SataDevSlpPort1Num - Reserved
} FCH_SATA;

///
/// Hpet structure
///
typedef struct {
  BOOLEAN    HpetEnable;                                ///< HPET function switch

  BOOLEAN    HpetMsiDis;                                ///< HpetMsiDis - South Bridge HPET MSI Configuration
                                                        /// @par
                                                        /// @li <b>1</b> - disable
                                                        /// @li <b>0</b> - enable

  UINT32     HpetBase;                                  ///< HpetBase
                                                        /// @par
                                                        /// HPET Base address
} FCH_HPET;

///
/// GCPU related parameters
///
typedef struct {
  UINT8    AcDcMsg;                                     ///< Send a message to CPU to indicate the power mode (AC vs battery)
                                                        /// @li <b>1</b> - disable
                                                        /// @li <b>0</b> - enable

  UINT8    TimerTickTrack;                              ///< Send a message to CPU to indicate the latest periodic timer interval
                                                        /// @li <b>1</b> - disable
                                                        /// @li <b>0</b> - enable

  UINT8    ClockInterruptTag;                           ///< Mark the periodic timer interrupt
                                                        /// @li <b>1</b> - disable
                                                        /// @li <b>0</b> - enable

  UINT8    OhciTrafficHanding;                          ///< Cause CPU to break out from C state when USB OHCI has pending traffic
                                                        /// @li <b>1</b> - disable
                                                        /// @li <b>0</b> - enable

  UINT8    EhciTrafficHanding;                          ///< Cause CPU to break out from C state when USB EHCI has pending traffic
                                                        /// @li <b>1</b> - disable
                                                        /// @li <b>0</b> - enable

  UINT8    GcpuMsgCMultiCore;                           ///< Track of CPU C state by monitoring each core's C state message
                                                        /// @li <b>1</b> - disable
                                                        /// @li <b>0</b> - enable

  UINT8    GcpuMsgCStage;                               ///< Enable the FCH C state coordination logic
                                                        /// @li <b>1</b> - disable
                                                        /// @li <b>0</b> - enable
} FCH_GCPU;

///
/// Timer
///
typedef struct {
  BOOLEAN    Enable;                                    ///< Whether to register timer SMI in POST
  BOOLEAN    StartNow;                                  ///< Whether to start the SMI immediately during registration
  UINT16     CycleDuration;                             ///< [14:0] - Actual cycle duration = CycleDuration + 1
} TIMER_SMI;

///
/// CS support
///
typedef struct {
  BOOLEAN    FchCsD3Cold;                               ///< FCH Cs D3 Cold function
  BOOLEAN    FchCsHwReduced;                            ///< FCH Cs hardware reduced ACPI flag
  BOOLEAN    FchCsPwrBtn;                               ///< FCH Cs Power Button function
  BOOLEAN    FchCsAcDc;                                 ///< FCH Cs AcDc function
  BOOLEAN    AsfNfcEnable;                              ///< FCH Cs NFC function
  UINT8      AsfNfcInterruptPin;                        ///< NFC Interrupt pin define
  UINT8      AsfNfcRegPuPin;                            ///< NFC RegPu pin define
  UINT8      AsfNfcWakePin;                             ///< NFC Wake Pin define
  UINT8      PowerButtonGpe;                            ///< GPE# used by Power Button device
  UINT8      AcDcTimerGpe;                              ///< GPE# used by Timer device
} FCH_CS;

///
/// MISC structure
///
typedef struct {
  BOOLEAN      NativePcieSupport;                       /// PCIe NativePcieSupport - Debug function. 1:Enabled, 0:Disabled
  BOOLEAN      S3Resume;                                /// S3Resume - Flag of ACPI S3 Resume.
  BOOLEAN      RebootRequired;                          /// RebootRequired - Flag of Reboot system is required.
  UINT8        FchVariant;                              /// FchVariant - FCH Variant value.
  UINT8        Cg2Pll;                                  ///< CG2 PLL - 0:disable, 1:enable
  TIMER_SMI    LongTimer;                               ///< Long Timer SMI
  TIMER_SMI    ShortTimer;                              ///< Short Timer SMI
  UINT32       FchCpuId;                                ///< Saving CpuId for FCH Module.
  BOOLEAN      NoneSioKbcSupport;                       ///< NoneSioKbcSupport - No KBC/SIO controller ( Turn on Inchip KBC emulation function )
  FCH_CS       FchCsSupport;                            ///< FCH Cs function structure
} FCH_MISC;

///
/// MISC2 structure
///
typedef struct {
  UINT8                  Cg2Pll;                        ///< CG2 PLL - 0:disable, 1:enable
  BOOLEAN                EcKbd;                         ///< EC KBD - 0:disable, 1:enable
  BOOLEAN                LegacyFree;                    ///< Legacy Free - 0:disable, 1:enable
  BOOLEAN                SataInternal100Spread;         ///< SATA internal 100MHz spread ON/OFF
  BOOLEAN                EcChannel0;                    ///< Enable EC channel 0
  BOOLEAN                QeEnabled;                     /// Quad Mode Enabled
  BOOLEAN                FchOscout1ClkContinous;        ///< FCH OSCOUT1_CLK Continous
  UINT8                  LpcClockDriveStrength;         ///< Lpc Clock Drive Strength
  VOID                   *EarlyOemGpioTable;            /// Pointer of Early OEM GPIO table
  BOOLEAN                WdtEnable;                     ///< enable FCH WatchDog Timer (WDT)
  VOID                   *OemResetProgrammingTablePtr;  /// Pointer of ACPI OEM table
  UINT32                 FchAcpiMmioBase;               ///< FCH ACPI MMIO Base
  BOOLEAN                FchSerialIrqSupport;           ///< SerialIrqEnable
} FCH_MISC2;

///
/// SMBus structure
///
typedef struct {
  UINT32    SmbusSsid;                                  ///< SMBUS controller Subsystem ID
} FCH_SMBUS;

///
/// Acpi structure
///
typedef struct {
  UINT16        Smbus0BaseAddress;                      ///< Smbus0BaseAddress
                                                        /// @par
                                                        /// Smbus BASE Address
                                                        ///
  UINT16        Smbus1BaseAddress;                      ///< Smbus1BaseAddress
                                                        /// @par
                                                        /// Smbus1 (ASF) BASE Address
                                                        ///
  UINT16        SioPmeBaseAddress;                      ///< SioPmeBaseAddress
                                                        /// @par
                                                        /// SIO PME BASE Address
                                                        ///
  UINT32        WatchDogTimerBase;                      ///< WatchDogTimerBase
                                                        /// @par
                                                        /// Watch Dog Timer Address
                                                        ///
  UINT16        AcpiPm1EvtBlkAddr;                      ///< AcpiPm1EvtBlkAddr
                                                        /// @par
                                                        /// ACPI PM1 event block Address
                                                        ///
  UINT16        AcpiPm1CntBlkAddr;                      ///< AcpiPm1CntBlkAddr
                                                        /// @par
                                                        /// ACPI PM1 Control block Address
                                                        ///
  UINT16        AcpiPmTmrBlkAddr;                       ///< AcpiPmTmrBlkAddr
                                                        /// @par
                                                        /// ACPI PM timer block Address
                                                        ///
  UINT16        CpuControlBlkAddr;                      ///< CpuControlBlkAddr
                                                        /// @par
                                                        /// ACPI CPU control block Address
                                                        ///
  UINT16        AcpiGpe0BlkAddr;                        ///< AcpiGpe0BlkAddr
                                                        /// @par
                                                        /// ACPI GPE0 block Address
                                                        ///
  UINT16        SmiCmdPortAddr;                         ///< SmiCmdPortAddr
                                                        /// @par
                                                        /// SMI command port Address
                                                        ///
  UINT16        AcpiPmaCntBlkAddr;                      ///< AcpiPmaCntBlkAddr
                                                        /// @par
                                                        /// ACPI PMA Control block Address
                                                        ///
  BOOLEAN       AnyHt200MhzLink;                        ///< AnyHt200MhzLink
                                                        /// @par
                                                        /// HT Link Speed on 200MHz option for each CPU specific LDTSTP# (Force enable)
                                                        ///
  BOOLEAN       SpreadSpectrum;                         ///< SpreadSpectrum
                                                        /// @par
                                                        /// Spread Spectrum function
                                                        /// @li <b>0</b> - disable
                                                        /// @li <b>1</b> - enable
                                                        ///
  POWER_FAIL    PwrFailShadow;                          ///< PwrFailShadow = PM_Reg: 5Bh [3:0]
                                                        /// @par
                                                        /// @li  <b>00</b> - Always off
                                                        /// @li  <b>01</b> - Always on
                                                        /// @li  <b>11</b> - Use previous
                                                        ///
  UINT8         StressResetMode;                        ///< StressResetMode 01-10
                                                        /// @li  <b>00</b> - Disabed
                                                        /// @li  <b>01</b> - Io Write 0x64 with 0xfe
                                                        /// @li  <b>10</b> - Io Write 0xcf9 with 0x06
                                                        /// @li  <b>11</b> - Io Write 0xcf9 with 0x0e
                                                        ///
  BOOLEAN       MtC1eEnable;                            /// MtC1eEnable - Enable MtC1e
  VOID          *OemProgrammingTablePtr;                /// Pointer of ACPI OEM table
  UINT8         SpreadSpectrumOptions;                  /// SpreadSpectrumOptions - Spread Spectrum Option
  BOOLEAN       PwrDownDisp2ClkPcieR;                   /// Power down DISP2_CLK and PCIE_RCLK_Output for power savings
  BOOLEAN       NoClearThermalTripSts;                  /// Skip clearing ThermalTrip status
  UINT32        FchAcpiMmioBase;                        ///< FCH ACPI MMIO Base
} FCH_ACPI;

///
/// HWM temp parameter structure
///
typedef struct _FCH_HWM_TEMP_PAR {
  UINT16    At;                                         ///< At
  UINT16    Ct;                                         ///< Ct
  UINT8     Mode;                                       ///< Mode BIT0:HiRatio BIT1:HiCurrent
} FCH_HWM_TEMP_PAR;

///
/// HWM Current structure
///
typedef struct _FCH_HWM_CUR {
  UINT16    FanSpeed[5];                                ///< FanSpeed    - fan Speed
  UINT16    Temperature[5];                             ///< Temperature - temperature
  UINT16    Voltage[8];                                 ///< Voltage     - voltage
} FCH_HWM_CUR;

///
/// HWM fan control structure
///
typedef struct _FCH_HWM_FAN_CTR {
  UINT8     InputControlReg00;                          /// Fan Input Control register, PM2 offset [0:4]0
  UINT8     ControlReg01;                               /// Fan control register, PM2 offset [0:4]1
  UINT8     FreqReg02;                                  /// Fan frequency register, PM2 offset [0:4]2
  UINT8     LowDutyReg03;                               /// Low Duty register, PM2 offset [0:4]3
  UINT8     MedDutyReg04;                               /// Med Duty register, PM2 offset [0:4]4
  UINT8     MultiplierReg05;                            /// Multiplier register, PM2 offset [0:4]5
  UINT16    LowTempReg06;                               /// Low Temp register, PM2 offset [0:4]6
  UINT16    MedTempReg08;                               /// Med Temp register, PM2 offset [0:4]8
  UINT16    HighTempReg0A;                              /// High Temp register, PM2 offset [0:4]A
  UINT8     LinearRangeReg0C;                           /// Linear Range register, PM2 offset [0:4]C
  UINT8     LinearHoldCountReg0D;                       /// Linear Hold Count register, PM2 offset [0:4]D
} FCH_HWM_FAN_CTR;

///
/// Hwm structure
///
typedef struct _FCH_HWM {
  UINT8               HwMonitorEnable;                  ///< HwMonitorEnable
  UINT32              HwmControl;                       ///< hwmControl
                                                        /// @par
                                                        /// HWM control configuration
                                                        /// @li <b>0</b> - HWM is Enabled
                                                        /// @li <b>1</b> - IMC is Enabled
                                                        ///
  UINT8               FanSampleFreqDiv;                 ///< Sampling rate of Fan Speed
                                                        /// @li <b>00</b> - Base(22.5KHz)
                                                        /// @li <b>01</b> - Base(22.5KHz)/2
                                                        /// @li <b>10</b> - Base(22.5KHz)/4
                                                        /// @li <b>11</b> - Base(22.5KHz)/8
                                                        ///
  UINT8               HwmFchtsiAutoPoll;                ///< TSI Auto Polling
                                                        /// @li <b>0</b> - disable
                                                        /// @li <b>1</b> - enable
                                                        ///
  UINT8               HwmFchtsiAutoPollStarted;         ///< HwmSbtsiAutoPollStarted
  UINT8               FanLinearEnhanceEn;               ///< FanLinearEnhanceEn
  UINT8               FanLinearHoldFix;                 ///< FanLinearHoldFix
  UINT8               FanLinearRangeOutLimit;           ///< FanLinearRangeOutLimit
  UINT16              HwmCalibrationFactor;             /// Calibration Factor
  FCH_HWM_CUR         HwmCurrent;                       /// HWM Current structure
  FCH_HWM_CUR         HwmCurrentRaw;                    /// HWM Current Raw structure
  FCH_HWM_TEMP_PAR    HwmTempPar[5];                    /// HWM Temp parameter structure
  FCH_HWM_FAN_CTR     HwmFanControl[5];                 /// HWM Fan Control structure
  FCH_HWM_FAN_CTR     HwmFanControlCooked[5];           /// HWM Fan Control structure
} FCH_HWM;

///
/// Gec structure
///
typedef struct {
  BOOLEAN    GecEnable;                                 ///< GecEnable - GEC function switch
  UINT8      GecPhyStatus;                              /// GEC PHY Status
  UINT8      GecPowerPolicy;                            /// GEC Power Policy
                                                        /// @li <b>00</b> - GEC is powered down in S3 and S5
                                                        /// @li <b>01</b> - GEC is powered down only in S5
                                                        /// @li <b>10</b> - GEC is powered down only in S3
                                                        /// @li <b>11</b> - GEC is never powered down
                                                        ///
  UINT8      GecDebugBus;                               /// GEC Debug Bus
                                                        /// @li <b>0</b> - disable
                                                        /// @li <b>1</b> - enable
                                                        ///
  UINT32     GecShadowRomBase;                          ///< GecShadowRomBase
                                                        /// @par
                                                        /// GEC (NIC) SHADOWROM BASE Address
                                                        ///
  VOID       *PtrDynamicGecRomAddress;                  /// Pointer of Dynamic GEC ROM Address
} FCH_GEC;

///
/// _ABTblEntry - AB link register table R/W structure
///
typedef struct _AB_TBL_ENTRY {
  UINT8     RegType;                                    /// RegType  : AB Register Type (ABCFG, AXCFG and so on)
  UINT32    RegIndex;                                   /// RegIndex : AB Register Index
  UINT32    RegMask;                                    /// RegMask  : AB Register Mask
  UINT32    RegData;                                    /// RegData  : AB Register Data
} AB_TBL_ENTRY;

///
/// AB structure
///
typedef struct {
  BOOLEAN    AbMsiEnable;                               ///< ABlink MSI capability
  UINT8      ALinkClkGateOff;                           /// Alink Clock Gate-Off function - 0:disable, 1:enable *KR
  UINT8      BLinkClkGateOff;                           /// Blink Clock Gate-Off function - 0:disable, 1:enable *KR
  UINT8      GppClockRequest0;                          /// GPP Clock Request.
  UINT8      GppClockRequest1;                          /// GPP Clock Request.
  UINT8      GppClockRequest2;                          /// GPP Clock Request.
  UINT8      GppClockRequest3;                          /// GPP Clock Request.
  UINT8      GfxClockRequest;                           /// GPP Clock Request.
  UINT8      AbClockGating;                             /// AB Clock Gating - 0:disable, 1:enable *KR *CZ
  UINT8      GppClockGating;                            /// GPP Clock Gating - 0:disable, 1:enable
  UINT8      UmiL1TimerOverride;                        /// UMI L1 inactivity timer overwrite value
  UINT8      UmiLinkWidth;                              /// UMI Link Width
  UINT8      UmiDynamicSpeedChange;                     /// UMI Dynamic Speed Change - 0:disable, 1:enable
  UINT8      PcieRefClockOverClocking;                  /// PCIe Ref Clock OverClocking value
  UINT8      UmiGppTxDriverStrength;                    /// UMI GPP TX Driver Strength
  BOOLEAN    NbSbGen2;                                  /// UMI link Gen2 - 0:Gen1, 1:Gen2
  UINT8      PcieOrderRule;                             /// PCIe Order Rule - 0:disable, 1:enable *KR AB Posted Pass Non-Posted
  UINT8      SlowSpeedAbLinkClock;                      /// Slow Speed AB Link Clock - 0:disable, 1:enable *KR
  BOOLEAN    ResetCpuOnSyncFlood;                       /// Reset Cpu On Sync Flood - 0:disable, 1:enable *KR
  BOOLEAN    AbDmaMemoryWrtie3264B;                     /// AB DMA Memory Write 32/64 BYTE Support *KR only
  BOOLEAN    AbMemoryPowerSaving;                       /// AB Memory Power Saving *KR *CZ
  BOOLEAN    SbgDmaMemoryWrtie3264ByteCount;            /// SBG DMA Memory Write 32/64 BYTE Count Support *KR only
  BOOLEAN    SbgMemoryPowerSaving;                      /// SBG Memory Power Saving *KR *CZ
  BOOLEAN    SbgClockGating;                            /// SBG Clock Gate *CZ
  BOOLEAN    XdmaDmaWrite16ByteMode;                    /// XDMA DMA Write 16 byte mode *CZ
  BOOLEAN    XdmaMemoryPowerSaving;                     /// XDMA memory power saving *CZ
  UINT8      XdmaPendingNprThreshold;                   /// XDMA PENDING NPR THRESHOLD *CZ
  BOOLEAN    XdmaDncplOrderDis;                         /// XDMA DNCPL ORDER DIS *CZ
  UINT8      SltGfxClockRequest0;                       /// GPP Clock Request.
  UINT8      SltGfxClockRequest1;                       /// GPP Clock Request.
  BOOLEAN    SdphostBypassDataPack;                     /// SdphostBypassDataPack
  BOOLEAN    SdphostDisNpmwrProtect;                    /// Disable NPMWR interleaving protection
} FCH_AB;

/**
 * PCIE_CAP_ID - PCIe Cap ID
 *
 */
#define  PCIE_CAP_ID  0x10

///
/// FCH_GPP_PORT_CONFIG - Fch GPP port config structure
///
typedef struct {
  BOOLEAN    PortPresent;                               ///< Port connection
                                                        /// @par
                                                        /// @li <b>0</b> - Port doesn't have slot. No need to train the link
                                                        /// @li <b>1</b> - Port connection defined and needs to be trained
                                                        ///
  BOOLEAN    PortDetected;                              ///< Link training status
                                                        /// @par
                                                        /// @li <b>0</b> - EP not detected
                                                        /// @li <b>1</b> - EP detected
                                                        ///
  BOOLEAN    PortIsGen2;                                ///< Port link speed configuration
                                                        /// @par
                                                        /// @li <b>00</b> - Auto
                                                        /// @li <b>01</b> - Forced GEN1
                                                        /// @li <b>10</b> - Forced GEN2
                                                        /// @li <b>11</b> - Reserved
                                                        ///
  BOOLEAN    PortHotPlug;                               ///< Support hot plug?
                                                        /// @par
                                                        /// @li <b>0</b> - No support
                                                        /// @li <b>1</b> - support
                                                        ///
  UINT8      PortMisc;                                  /// PortMisc - Reserved
} FCH_GPP_PORT_CONFIG;

///
/// GPP structure
///
typedef struct {
  FCH_GPP_PORT_CONFIG    PortCfg[4];                    /// GPP port configuration structure
  GPP_LINKMODE           GppLinkConfig;                 ///< GppLinkConfig - PCIE_GPP_Enable[3:0]
                                                        /// @li  <b>0000</b> - Port ABCD -> 4:0:0:0
                                                        /// @li  <b>0010</b> - Port ABCD -> 2:2:0:0
                                                        /// @li  <b>0011</b> - Port ABCD -> 2:1:1:0
                                                        /// @li  <b>0100</b> - Port ABCD -> 1:1:1:1
                                                        ///
  BOOLEAN                GppFunctionEnable;             ///< GPP Function - 0:disable, 1:enable
  BOOLEAN                GppToggleReset;                ///< Toggle GPP core reset
  UINT8                  GppHotPlugGeventNum;           ///< Hotplug GEVENT # - valid value 0-31
  UINT8                  GppFoundGfxDev;                ///< Gpp Found Gfx Device
                                                        /// @li   <b>0</b> - Not found
                                                        /// @li   <b>1</b> - Found
                                                        ///
  BOOLEAN                GppGen2;                       ///< GPP Gen2 - 0:disable, 1:enable
  UINT8                  GppGen2Strap;                  ///< GPP Gen2 Strap - 0:disable, 1:enable, FCH itself uses this
  BOOLEAN                GppMemWrImprove;               ///< GPP Memory Write Improve - 0:disable, 1:enable
  BOOLEAN                GppUnhidePorts;                ///< GPP Unhide Ports - 0:disable, 1:enable
  UINT8                  GppPortAspm;                   ///< GppPortAspm - ASPM state for all GPP ports
                                                        /// @li   <b>01</b> - Disabled
                                                        /// @li   <b>01</b> - L0s
                                                        /// @li   <b>10</b> - L1
                                                        /// @li   <b>11</b> - L0s + L1
                                                        ///
  BOOLEAN                GppLaneReversal;               ///< GPP Lane Reversal - 0:disable, 1:enable
  BOOLEAN                GppPhyPllPowerDown;            ///< GPP PHY PLL Power Down - 0:disable, 1:enable
  BOOLEAN                GppDynamicPowerSaving;         ///< GPP Dynamic Power Saving - 0:disable, 1:enable
  BOOLEAN                PcieAer;                       ///< PcieAer - Advanced Error Report: 0/1-disable/enable
  BOOLEAN                PcieRas;                       ///< PCIe RAS - 0:disable, 1:enable
  BOOLEAN                PcieCompliance;                ///< PCIe Compliance - 0:disable, 1:enable
  BOOLEAN                PcieSoftwareDownGrade;         ///< PCIe Software Down Grade
  BOOLEAN                UmiPhyPllPowerDown;            ///< UMI PHY PLL Power Down - 0:disable, 1:enable
  BOOLEAN                SerialDebugBusEnable;          ///< Serial Debug Bus Enable
  UINT8                  GppHardwareDownGrade;          ///< GppHardwareDownGrade - Gpp HW Down Grade function 0:Disable, 1-4: portA-D
  UINT8                  GppL1ImmediateAck;             ///< GppL1ImmediateAck - Gpp L1 Immediate ACK 0: enable, 1: disable
  BOOLEAN                NewGppAlgorithm;               ///< NewGppAlgorithm - New GPP procedure
  UINT8                  HotPlugPortsStatus;            ///< HotPlugPortsStatus - Save Hot-Plug Ports Status
  UINT8                  FailPortsStatus;               ///< FailPortsStatus - Save Failure Ports Status
  UINT8                  GppPortMinPollingTime;         ///< GppPortMinPollingTime - Min. Polling time for Gpp Port Training
  BOOLEAN                IsCapsuleMode;                 ///< IsCapsuleMode - Support Capsule Mode in FCH
} FCH_GPP;

///
/// FCH USB3 Debug Sturcture
///
typedef struct {
  BOOLEAN    ServiceIntervalEnable;                 ///< Service Interval Enable
  BOOLEAN    BandwidthExpandEnable;                 ///< Bandwidth Expand Enable
  BOOLEAN    AoacEnable;                            ///< Aoac Enable
  BOOLEAN    HwLpmEnable;                           ///< HwLpm Enable
  BOOLEAN    DbcEnable;                             ///< DBC Enable
  BOOLEAN    MiscPlusEnable;                        ///< Misc Plus Enable
  BOOLEAN    EcoFixEnable;                          ///< Eco Fix Enable
  BOOLEAN    SsifEnable;                            ///< SSIF Enable
  BOOLEAN    U2ifEnable;                            ///< U2IF Enable
  BOOLEAN    FseEnable;                             ///< FSE Enable
  BOOLEAN    XhcPmeEnable;                          ///< Xhc Pme Enable
} USB3_DEBUG;

///
/// FCH IoMux Sturcture
///
typedef struct {
  UINT8    CbsDbgFchSmbusI2c2Egpio;                               ///< SMBUS/I2C_2/EGPIO_113_114
  UINT8    CbsDbgFchAsfI2c3Egpio;                                 ///< ASF/I2C_3/EGPIO_019_020
} FCH_IOMUX;

///
/// FCH USB sturcture
///
typedef struct {
  BOOLEAN    Ohci1Enable;                               ///< OHCI1 controller enable
  BOOLEAN    Ohci2Enable;                               ///< OHCI2 controller enable
  BOOLEAN    Ohci3Enable;                               ///< OHCI3 controller enable
  BOOLEAN    Ohci4Enable;                               ///< OHCI4 controller enable
  BOOLEAN    Ehci1Enable;                               ///< EHCI1 controller enable
  BOOLEAN    Ehci2Enable;                               ///< EHCI2 controller enable
  BOOLEAN    Ehci3Enable;                               ///< EHCI3 controller enable
  BOOLEAN    Xhci0Enable;                               ///< XHCI0 controller enable
  BOOLEAN    Xhci1Enable;                               ///< XHCI1 controller enable
  BOOLEAN    UsbMsiEnable;                              ///< USB MSI capability
  UINT32     OhciSsid;                                  ///< OHCI SSID
  UINT32     Ohci4Ssid;                                 ///< OHCI 4 SSID
  UINT32     EhciSsid;                                  ///< EHCI SSID
  UINT32     XhciSsid;                                  ///< XHCI SSID
  BOOLEAN    UsbPhyPowerDown;                           ///< USB PHY Power Down - 0:disable, 1:enable
  UINT32     UserDefineXhciRomAddr;                     ///< XHCI ROM address define by platform BIOS
  UINT8      Ehci1Phy[5];                               ///< EHCI1 USB PHY Driving Strength value table
  UINT8      Ehci2Phy[5];                               ///< EHCI2 USB PHY Driving Strength value table
  UINT8      Ehci3Phy[4];                               ///< EHCI3 USB PHY Driving Strength value table
  UINT8      Xhci20Phy[4];                              ///< XHCI USB 2.0 PHY Driving Strength value table
  UINT8      Ehci1DebugPortSel;                         ///< DebugPortSel for Ehci1 Hub
                                                        /// @li   <b>000</b> - Disable
                                                        /// @li   <b>001</b> - HubDownStreamPort0
                                                        /// @li   <b>010</b> - HubDownStreamPort1
                                                        /// @li   <b>011</b> - HubDownStreamPort2
                                                        /// @li   <b>100</b> - HubDownStreamPort3
  UINT8      Ehci2DebugPortSel;                         ///< DebugPortSel for Ehci2 Hub
                                                        /// @li   <b>000</b> - Disable
                                                        /// @li   <b>001</b> - HubDownStreamPort0
                                                        /// @li   <b>010</b> - HubDownStreamPort1
                                                        /// @li   <b>011</b> - HubDownStreamPort2
                                                        /// @li   <b>100</b> - HubDownStreamPort3
  UINT8      Ehci3DebugPortSel;                         ///< DebugPortSel for Ehci3 Hub
                                                        /// @li   <b>000</b> - Disable
                                                        /// @li   <b>001</b> - HubDownStreamPort0
                                                        /// @li   <b>010</b> - HubDownStreamPort1
                                                        /// @li   <b>011</b> - HubDownStreamPort2
                                                        /// @li   <b>100</b> - HubDownStreamPort3
  BOOLEAN    SsicEnable;                                ///< SSIC controller enable
  UINT32     SsicSsid;                                  ///< SSIC SSID
} FCH_USB;

///
/// FCH XHCI sturcture
///
typedef struct {
  UINT8                  XhciECCDedErrRptEn;            /// USB3 ECC SMI control
  UINT8                  Xhci0PortNum;                  /// USB3 Controller0 Port Num - [7:4] USB3, [3:0] USB2
  UINT8                  Xhci1PortNum;                  /// USB3 Controller1 Port Num - [7:4] USB3, [3:0] USB2
  UINT32                 XhciLaneParaCtl0;              /// USB3 PHY
} FCH_XHCI; 

///
/// FCH EMMC sturcture
///
typedef struct {
  BOOLEAN    EmmcEnable;                                ///< eMMC controller enable/disable
  UINT8      BusSpeedMode;                              ///< BusSpeedMode to select
                                                        /// @li   <b>000</b> - SDR12
                                                        /// @li   <b>001</b> - SDR25
                                                        /// @li   <b>010</b> - SDR50
                                                        /// @li   <b>011</b> - HS200
                                                        /// @li   <b>100</b> - DDR50
                                                        /// @li   <b>101</b> - HS400
  UINT8      BusWidth;                                  ///< BusWidth to select
                                                        /// @li   <b>000</b> - 1-bit
                                                        /// @li   <b>001</b> - 4-bit
                                                        /// @li   <b>010</b> - 8-bit
  UINT8      RetuneMode;                                ///< RetuneMode to select
                                                        /// @li   <b>000</b> - Mode 1
                                                        /// @li   <b>001</b> - Mode 2
                                                        /// @li   <b>010</b> - Mode 3
  BOOLEAN    ClockMultiplier;                           ///< ClockMultiplier enable/disable
} FCH_EMMC;

///
/// FCH GBE sturcture
///
typedef struct {
  BOOLEAN    Gbe0Enable;                                ///< Gbe controller0 enable/disable
  BOOLEAN    Gbe1Enable;                                ///< Gbe controller1 enable/disable
  UINT32     Gbe0MacAddress0Low;                        ///< Gbe0 MAC Address0 Low
  UINT32     Gbe0MacAddress0High;                       ///< Gbe0 MAC Address0 High
  UINT32     Gbe0MacAddress1Low;                        ///< Gbe0 MAC Address1 Low
  UINT32     Gbe0MacAddress1High;                       ///< Gbe0 MAC Address1 High
  UINT32     Gbe1MacAddress0Low;                        ///< Gbe1 MAC Address0 Low
  UINT32     Gbe1MacAddress0High;                       ///< Gbe1 MAC Address0 High
  UINT32     Gbe1MacAddress1Low;                        ///< Gbe1 MAC Address1 Low
  UINT32     Gbe1MacAddress1High;                       ///< Gbe1 MAC Address1 High
} FCH_GBE;

///
/// FCH Component Data Structure in InitReset stage
///
typedef struct {
  IN       BOOLEAN    UmiGen2;               ///< Enable Gen2 data rate of UMI
                                             ///< @li <b>FALSE</b> - Disable Gen2
                                             ///< @li <b>TRUE</b>  - Enable Gen2

  IN       UINT32     SataEnable;            ///< SATA controller function
                                             ///< bit0~bit3   - Socket0 IOHC0~3
                                             ///< bit4~bit7   - Socket1 IOHC0~3
                                             ///< bit8~bit11  - Socket2 IOHC0~3
                                             ///< bit12~bit15 - Socket3 IOHC0~3
                                             ///< bit16~bit19 - Socket4 IOHC0~3
                                             ///< bit20~bit23 - Socket5 IOHC0~3
                                             ///< bit24~bit27 - Socket6 IOHC0~3
                                             ///< bit28~bit31 - Socket7 IOHC0~3
                                             ///< @li <b>FALSE</b> - SATA controller is disabled
                                             ///< @li <b>TRUE</b> - SATA controller is enabled

  IN       BOOLEAN    IdeEnable;             ///< SATA IDE controller mode enabled/disabled
                                             ///< @li <b>FALSE</b> - IDE controller is disabled
                                             ///< @li <b>TRUE</b> - IDE controller is enabled

  IN       BOOLEAN    GppEnable;             ///< Master switch of GPP function
                                             ///< @li <b>FALSE</b> - GPP disabled
                                             ///< @li <b>TRUE</b> - GPP enabled

  IN       BOOLEAN    Xhci0Enable;           ///< XHCI0 controller function
                                             ///<   @li <b>FALSE</b> - XHCI0 controller disabled
                                             ///<   @li <b>TRUE</b> - XHCI0 controller enabled

  IN       BOOLEAN    Xhci1Enable;           ///< XHCI1 controller function
                                             ///< @li <b>FALSE</b> - XHCI0 controller disabled
                                             ///< @li <b>TRUE</b> - XHCI0 controller enabled
} FCH_RESET_INTERFACE;

/// Private: FCH_DATA_BLOCK_RESET
typedef struct _FCH_RESET_DATA_BLOCK {
  HYGON_CONFIG_PARAMS    *StdHeader;                    ///< Header structure
  FCH_RESET_INTERFACE    FchReset;                      ///< Reset interface
  SATA_ST                SataMode;                      ///< SataMode
  FCH_GPP                Gpp;                           ///< GPP subsystem
  FCH_SPI                Spi;                           ///< SPI subsystem
  FCH_ESPI               Espi;                          ///< ESPI subsystem
  FCH_PLATFORM_POLICY    FchBldCfg;                     /// Build Options
  FCH_XHCI               Xhci;
  FCH_MISC2              Misc2;                         ///< MISC2 structure
} FCH_RESET_DATA_BLOCK;

/// Private: FCH_DATA_BLOCK
typedef struct _FCH_DATA_BLOCK {
  FCH_RUNTIME             FchRunTime;                   ///< FCH Run Time Parameters
  HYGON_CONFIG_PARAMS     *StdHeader;                   ///< Header structure

  FCH_ACPI                HwAcpi;                       ///< ACPI structure
  FCH_AB                  Ab;                           ///< AB structure
  FCH_GPP                 Gpp;                          ///< GPP structure
  FCH_USB                 Usb;                          ///< USB structure
  FCH_SATA                Sata;                         ///< SATA structure
  FCH_SMBUS               Smbus;                        ///< SMBus structure
  FCH_IDE                 Ide;                          ///< IDE structure
  FCH_SPI                 Spi;                          ///< SPI structure
  FCH_PCIB                Pcib;                         ///< PCIB structure
  FCH_GEC                 Gec;                          ///< GEC structure
  FCH_HWM                 Hwm;                          ///< Hardware Moniter structure
  FCH_IR                  Ir;                           ///< IR structure
  FCH_HPET                Hpet;                         ///< HPET structure
  FCH_GCPU                Gcpu;                         ///< GCPU structure
  FCH_IMC                 Imc;                          ///< IMC structure
  FCH_MISC                Misc;                         ///< MISC structure
  FCH_IOMUX               IoMux;                        ///< IOMUX structure
  VOID                    *PostOemGpioTable;            /// Pointer of Post OEM GPIO table
  FCH_EMMC                Emmc;                         ///< EMMC structure
  FCH_XGBE                Xgbe;                         ///< XGBE structure
  FCH_RESET_DATA_BLOCK    *FchResetDataBlock;           ///< Reset data structure
} FCH_DATA_BLOCK;

typedef struct _HYGON_USB_DISABLE_MAP {
	UINT8  SocketNum ;
	UINT8  RootBridgeNum ;
	UINT8  UsbDisableMap;
} HYGON_USB_DISABLE_MAP;

HGPI_STATUS
FchInitReset (
  IN FCH_RESET_DATA_BLOCK *FchParams
  );

HGPI_STATUS
FchInitEnv (
  IN FCH_DATA_BLOCK *FchDataBlock
  );

HGPI_STATUS
FchInitMid (
  IN FCH_DATA_BLOCK *FchDataBlock
  );

HGPI_STATUS
FchInitLate (
  IN FCH_DATA_BLOCK *FchDataBlock
  );

HGPI_STATUS
FchSpiTransfer (
  IN       UINT8    PrefixCode,
  IN       UINT8    Opcode,
  IN  OUT  UINT8    *DataPtr,
  IN       UINT8    *AddressPtr,
  IN       UINT8    Length,
  IN       BOOLEAN  WriteFlag,
  IN       BOOLEAN  AddressFlag,
  IN       BOOLEAN  DataFlag,
  IN       BOOLEAN  FinishedFlag
  );

VOID
SataEnableWriteAccessTS (
  IN  UINT8               PhysicalDieId,
  IN  UINT8               SataIndex,
  IN  UINT32              DieBusNum,
  IN  VOID                *FchDataPtr
  );

VOID
FchTSSataInitPortActive (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  UINT32   PortNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchTSSataInitRsmuCtrl (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN VOID      *FchDataPtr
  );

VOID
FchTSSataInitCtrlReg (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN VOID      *FchDataPtr
  );

VOID
FchTSSataInitEsata (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN VOID      *FchDataPtr
  );

VOID
FchTSSataInitDevSlp (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchTSSataInitMpssMap (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchTSSataInitEnableErr (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchTSSataSetPortGenMode (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchTSSataGpioInitial (
  IN  UINT8      PhysicalDieId,
  IN  UINT8      SataIndex,
  IN  UINT32     DieBusNum,
  IN  VOID       *FchDataPtr
  );

VOID
FchInitEnvSataAhciTS (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchInitEnvSataRaidTS  (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );


VOID
SataDisableWriteAccessTS (
  IN  UINT8               PhysicalDieId,
  IN  UINT8               SataIndex,
  IN  UINT32              DieBusNum,
  IN  VOID                *FchDataPtr
  );

VOID
FchTsXgbePortPlatformConfig (
  IN UINT8        PhysicalDieId,
  IN UINT32       DieBusNum,
  IN UINT8        PortNum,
  IN UINT32       SystemPort,
  IN VOID         *PortData
  );

VOID
FchTsXgbePortMacAddress (
  IN UINT8        PhysicalDieId,
  IN UINT32       DieBusNum,
  IN UINT8        PortNum,
  IN VOID         *MacData
  );

VOID
FchTsXgbePortPtpInit (
  IN UINT8        PhysicalDieId,
  IN UINT32       DieBusNum,
  IN UINT8        PortNum,
  IN VOID         *PortData
  );

VOID
FchTsXgbePortClockGate (
  IN UINT8        PhysicalDieId,
  IN UINT32       DieBusNum,
  IN UINT8        Port
  );

VOID
FchInitLateSataAhci (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchInitLateSataRaid (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  );

VOID
FchTSSataShutdownUnconnectedSataPortClock (
  IN  UINT8      PhysicalDieId,
  IN  UINT8      SataIndex,
  IN  UINT32     DieBusNum,
  IN  VOID       *FchDataPtr
  );

VOID
FchTSSataInitMMC (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN VOID      *FchDataPtr
  );




#pragma pack (pop)

#endif
