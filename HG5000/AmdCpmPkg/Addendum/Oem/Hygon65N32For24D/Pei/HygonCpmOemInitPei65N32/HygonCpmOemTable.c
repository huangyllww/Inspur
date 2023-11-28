#include <AmdCpmPei.h>
#include <AmdCpmBaseIo.h>
#include "HygonCpmOemInitPeim.h"
#include <Token.h>

//
// OEM CPM Table Definition
//
#define LANE16  16
#define LANE40  40
#define LANE48  48
#define LANE80  80
#define LANE104 104
#define LANE112 112

//
// Platform Id Table: Get Board Id from SMBUS
//

// Read SMBUS to detect board ID
//BOARD_ID_BIT2  BOARD_ID_BIT1  BOARD_ID_BIT0    PCBA
//    0                      0                          0             Normal  REVA
//    0                      1                          0             SLT REVA
//    1                      0                          0             DAP REVA
AMD_CPM_PLATFORM_ID_TABLE2          gCpmPlatformIdTable = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GET_PLATFORM_ID2, sizeof (gCpmPlatformIdTable) / sizeof (UINT8), 0, 0, 0, 1},
  {
//   UINT8  SmbusSelect;      ///< SMBUS Number
//   UINT8  SmbusAddress;     ///< SMBUS Address
//   UINT8  SmbusOffset;      ///< SMBUS Offset
//   UINT8  SmbusBit;         ///< SMBUS Bit
    0xFF,
  }
};

//
// Convert Table from Board Id to Platform Id
//

AMD_CPM_PLATFORM_ID_CONVERT_TABLE   gCpmPlatformIdConvertTable = {
  {CPM_SIGNATURE_GET_PLATFORM_ID_CONVERT, sizeof (gCpmPlatformIdConvertTable) / sizeof (UINT8), 0, 0, 0, 1},
  {
    0xFFFF,
  }
};


//
// Pre-Init Table
//
AMD_CPM_PRE_INIT_TABLE              gCpmPreInitTable = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_PRE_INIT, sizeof (gCpmPreInitTable) / sizeof (UINT8), 0, 0, 0, 0x01},
  {
//  {UINT8  Type;     // Register type. 0: FCH MMIO. 1: PCI
//   UINT8  Select;   // Register sub-type
//   UINT8  Offset;   // Register offset
//   UINT8  AndMask;  // AND mask
//   UINT8  OrMask;   // OR mask
//   UINT8  Stage;    // Stage number},
    0xFF,
  }
};

//
// GPIO Init Table for Hygon65N32
//
AMD_CPM_GPIO_INIT_TABLE             gCpmGpioInitTableHygon65N32 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table content
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_INIT, sizeof (gCpmGpioInitTableHygon65N32) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {//        (socket,die,gpio,function,         output,           pullup)
    GPIO_DEF_V2 (0, 0, 89,  GPIO_FUNCTION_1,  GPIO_NA,       GPIO_PU_PD_DIS),   //  PSP_INTR_0
//    GPIO_DEF_V2 (0, 0, 90,  GPIO_FUNCTION_0,  GPIO_INPUT,       GPIO_PU_EN    ),   // TPM_INT#
//    GPIO_DEF_V2 (0, 0, 130, GPIO_FUNCTION_0,  GPIO_NA,          GPIO_PU_PD_DIS),   // SATA_LED
//    GPIO_DEF_V2 (0, 0, 131, GPIO_FUNCTION_3,  GPIO_OUTPUT_HIGH, GPIO_PU_EN    ),   // 0: SATA, 1: PCIE
    0xFF,
  }
};

//
// GEVENT Init Table
//
AMD_CPM_GEVENT_INIT_TABLE           gCpmGeventInitTableHygon65N32 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GEVENT_INIT, sizeof (gCpmGeventInitTableHygon65N32) / sizeof (UINT8), 0, 0, 0, 0x00000001},
  {//                  GEvent EventEnable   SciTrigE      SciLevl         SmiSciEn        SciS0En         SciMap      SmiTrig       SmiControl
//    GEVENT_DEFINITION (0x03,  EVENT_ENABLE, SCITRIG_HI,   SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_03,  SMITRIG_HI,   SMICONTROL_DISABLE), // GEVENT03: PM_INT_IN
//    GEVENT_DEFINITION (0x05,  EVENT_ENABLE, SCITRIG_HI,   SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_05,  SMITRIG_HI,   SMICONTROL_DISABLE), // GEVENT05: LAN_MEDIA_SENSE
//    GEVENT_DEFINITION (0x08,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_08,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT08: PCIE_WAKE_UP#
//    GEVENT_DEFINITION (0x0C,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_12,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT12: USB_OC#
//    GEVENT_DEFINITION (0x0D,  EVENT_ENABLE, SCITRIG_HI,   SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_13,  SMITRIG_HI,   SMICONTROL_DISABLE), // GEVENT13: LAN_LOW_POWER
//    GEVENT_DEFINITION (0x0E,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_14,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT14: LAN_SMART#
//    GEVENT_DEFINITION (0x0F,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_15,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT15: EVALCARD_ALERT#
    0xFF,
  }
};

//
// CPM GPIO Module
//

AMD_CPM_GPIO_DEVICE_CONFIG_TABLE    gCpmGpioDeviceConfigTableHygon65N32 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_CONFIG, sizeof (gCpmGpioDeviceConfigTableHygon65N32) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {//                       DeviceId          Enable            Assert  Deassert  Hotplugs
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_GBE,    CPM_DEVICE_ON,    0,      0,        0), // GBE
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_BT,     CPM_DEVICE_ON,    0,      0,        0), // BT
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_WLAN,   CPM_DEVICE_ON,    0,      0,        0), // WLAN
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_PCIE_X16_SWITCH,   CPM_DEVICE_ON,    0,      0,        0), // PCIe 1x16/2x8 Switch
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_SATAE_M2_SWITCH,   CPM_DEVICE_ON,    0,      0,        0), // SataExpress/M.2 Switch
    0xFF,
  }
};

//
// CPM Device Detection Table for Hygon65N32
//
AMD_CPM_GPIO_DEVICE_DETECTION_TABLE gCpmGpioDeviceDetectionTableHygon65N32 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_DETECTION, sizeof (gCpmGpioDeviceDetectionTableHygon65N32) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
//  {UINT8  DeviceId; // Device Id
//   UINT8  Type;     // Detection type. 0: One GPIO pin. 1: Two GPIO pins. 2: Special Pin
//   UINT16 PinNum1;  // Pin number of GPIO 1
//   UINT8  Value1;   // Value of GPIO 1
//   UINT16 PinNum2;  // Pin number of GPIO 2
//   UINT8  Value2;   // Value of GPIO 2
//   UINT16 PinNum3;  // Pin number of GPIO 3
//   UINT8  Value3;   // Value of GPIO 3},
    0xFF,
  }
};

//
// CPM Device Reset Table
//
AMD_CPM_GPIO_DEVICE_RESET_TABLE gCpmGpioDeviceResetTableHygon65N32 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_RESET, sizeof (gCpmGpioDeviceResetTableHygon65N32) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
//  {UINT8  DeviceId;       // Device Id
//   UINT8  Mode;           // Reset mode     // 0: Reset Assert. // 1: Reset De-assert             // 2: Delay between Assert and Deassert
//   UINT8  Type;           // Register type  // 0: GPIO.         // 1: Special pin.if Mode = 0 or 1
//   UINT32 ((UINT16)Pin + ((UINT8)Value << 16));                 // GPIO pin value
//   UINT8  InitFlag;       // Init flag in post},
    0xFF,
  }
};

//
// CPM GPIO Device Init Table (Power On/Off)
//
AMD_CPM_GPIO_DEVICE_POWER_TABLE gCpmGpioDevicePowerTableHygon65N32 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_POWER, sizeof (gCpmGpioDevicePowerTableHygon65N32) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
//  {UINT8  DeviceId;       // Device Id
//   UINT8  Mode;           // Device Power Mode. 1: Power On. 0: Power Off
//   UINT8  Type;           // Device Power Item. 0: Set GPIO. 1: Wait GPIO. 2: Add Delay
//   UINT32 ((UINT16)Pin + ((UINT8)Value << 16));                 // GPIO pin value or delay timer
//   UINT8  InitFlag;       // Init flag in post},
//    {DEVICE_ID_GBE,    CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (GBE_POWER_CONTROL_REVA, 1),  0},
//    {DEVICE_ID_BT,     CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (BT_POWER_CONTROL_REVA, 0),   0},
//    {DEVICE_ID_WLAN,   CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (WLAN_POWER_CONTROL_REVA, 0), 0},
//    {DEVICE_ID_PCIE_X16_SWITCH,   CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (PCIE_X16_SWITCH, 0), 0},
//    {DEVICE_ID_SATAE_M2_SWITCH,   CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (SATAE_M2_SWITCH, 0), 0},

//    {DEVICE_ID_GBE,    CPM_POWER_ON,  CPM_POWER_SET,   CPM_GPIO_PIN (GBE_POWER_CONTROL_REVA, 0),  0},
//    {DEVICE_ID_BT,     CPM_POWER_ON,  CPM_POWER_SET,   CPM_GPIO_PIN (BT_POWER_CONTROL_REVA, 1),   0},
//    {DEVICE_ID_WLAN,   CPM_POWER_ON,  CPM_POWER_SET,   CPM_GPIO_PIN (WLAN_POWER_CONTROL_REVA, 1), 0},
//    {DEVICE_ID_PCIE_X16_SWITCH,   CPM_POWER_ON, CPM_POWER_SET,   CPM_GPIO_PIN (PCIE_X16_SWITCH, 1), 0},
//    {DEVICE_ID_SATAE_M2_SWITCH,   CPM_POWER_ON, CPM_POWER_SET,   CPM_GPIO_PIN (SATAE_M2_SWITCH, 1), 0},
    0xFF,
  }
};

//
// PCIE Clock Table
//
AMD_CPM_PCIE_CLOCK_TABLE    gCpmPcieClockTableHygon65N32 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table content
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_PCIE_CLOCK, sizeof (gCpmPcieClockTableHygon65N32) / sizeof (UINT8), 0, 0, 0, 0x0F},
  {
//  {UINT8  ClkId;              // FCH PCIe Clock
//   UINT8  ClkReq;             // FCH PCIe ClkReq
//   UINT8  ClkIdExt;           // External Clock Source
//   UINT8  ClkReqExt;          // External ClkReq
//   UINT8  DeviceId;           // Device Id. No Device Id if 0xFF
//   UINT8  Device;             // Device Number of PCIe bridge
//   UINT8  Function;           // Function Number of PCIe bridge
//   UINT8  SlotCheck;          // Slot Check Flag: // BIT0: Check PCI Space // BIT1: Check GPIO pin // BIT2: Check Clock Power Management Enable // BIT3~6: Reserved // BIT7: Change PCIe Clock in ACPI method
//   UINT32 SpecialFunctionId;  // Id of Special Function}
    // GPP_CLK0 clock for PCIE X8 slot is controlled by CLK_REQ0_L. For Bristol, this slot is not used
//    {GPP_CLK0,            CLK_ENABLE,  SRC_CLK4,  CLK_ENABLE,  DEVICE_ID_PCIE_X8_SLOT,  0, 0, NON_SLOT_CHECK,            0},
    // GPP_CLK1 clock for SSD is controlled by CLK_REQ1_L
//    {GPP_CLK1,            CLK_REQ1,    SRC_CLK8,  CLK_REQ8,    DEVICE_ID_M2_CONNECT,    0, 0, NON_SLOT_CHECK,            0},
    // GPP_CLK2 clock for Promontory chip is always on
//    {GPP_CLK2,            CLK_ENABLE,  SRC_CLK6,  CLK_ENABLE,  DEVICE_ID_PROMONTORY,    0, 0, NON_SLOT_CHECK,            0},
    // GFX_CLK clock for PCIE X16 slot is controlled by CLK_REQG_L. For Bristol, this slot is used as x8
//    {GPP_CLK9,            CLK_REQGFX,  SRC_CLK2,  CLK_REQ2,    DEVICE_ID_PCIE_X16_SLOT, 0, 0, NON_SLOT_CHECK,            0},
    // GPP_CLK3 clock for PCIE X4 slot is always on
//    {GPP_CLK3,            CLK_ENABLE,  SRC_CLK10, CLK_ENABLE,  DEVICE_ID_PCIE_X4_SLOT,  0, 0, NON_SLOT_CHECK,            0},
//    {CPM_CLKID_PCIE_RCLK, CLK_DISABLE, SRC_SKIP,  0,           0,                       0, 0, NON_SLOT_CHECK,            0},
    0xFF,
  }
};


//
// CPM DXIO Topology Table Socket 0 with field card in slot1
//
AMD_CPM_DXIO_TOPOLOGY_TABLE gCpmDxioTopologyTableHygon65N32S0 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_DXIO_TOPOLOGY, sizeof (gCpmDxioTopologyTableHygon65N32S0) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  0,  // SocketId
  {   // DXIO_PORT_DESCRIPTOR
       { // S0D0P0 Type A (PCIe) - SATA - M.2_0 (J11) and ODD (J21)
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 1, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_SATA (
              DxioPortEnabled                       // Port Present
            )
       },
        { // S0D0P0 Type A: unused lane 2 and 3. Set to PCIE and disable them according to DXIO programming guide.
          0,
          DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 2, 3, DxioHotplugDisabled, 1),
          DXIO_PORT_DATA_INITIALIZER_PCIE (
            DxioPortDisabled,                     // Port Present
            0,                                    // Requested Device
            0,                                    // Requested Function
            DxioHotplugDisabled,                  // Hotplug
            DxioGenMaxSupported,                  // Max Link Speed  
            DxioGenMaxSupported,                  // Max Link Capability  
            DxioAspmL0sL1,                        // ASPM
            DxioAspmDisabled,                     // ASPM L1.1 disabled
            DxioAspmDisabled,                     // ASPM L1.2 disabled
            DxioClkPmSupportDisabled              // Clock PM
          )
        },
        { // S0D0P0 Type A (PCIe) - x1 PCIe Gen3 - BMC BGA
          0,
          DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 4, 7, DxioHotplugDisabled, 1),
          DXIO_PORT_DATA_INITIALIZER_PCIE (
            DxioPortEnabled,                     // Port Present
            0,                                    // Requested Device
            0,                                    // Requested Function
            DxioHotplugDisabled,                  // Hotplug
            DxioGenMaxSupported,                  // Max Link Speed  
            DxioGenMaxSupported,                  // Max Link Capability  
            DxioAspmL0sL1,                        // ASPM
            DxioAspmDisabled,                     // ASPM L1.1 disabled
            DxioAspmDisabled,                     // ASPM L1.2 disabled
            DxioClkPmSupportDisabled              // Clock PM
          )
        },
    
    
        { // P0 [8:11] - M.2 J11
              0,
              DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 11, DxioHotplugDisabled, 1),
              DXIO_PORT_DATA_INITIALIZER_PCIE (
                     DxioPortEnabled,                      // Port Present
                     0,                                    // Requested Device
                     0,                                    // Requested Function
                     DxioHotplugDisabled,                  // Hotplug
                     DxioGenMaxSupported,                  // Max Link Speed
                     DxioGenMaxSupported,                  // Max Link Capability
                     DxioAspmL0sL1,                        // ASPM
                     DxioAspmDisabled,                     // ASPM L1.1 disabled
                     DxioAspmDisabled,                     // ASPM L1.2 disabled
                     DxioClkPmSupportDisabled              // Clock PM
                )
              },
        { // P0 [12:15] - M.2 J12
              0,
              DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 12, 15, DxioHotplugDisabled, 1),
              DXIO_PORT_DATA_INITIALIZER_PCIE (
                DxioPortEnabled,                    // Port Present
                0,                                  // Requested Device
                0,                                  // Requested Function
                DxioHotplugDisabled,                // Hotplug
                DxioGenMaxSupported,                            // Max Link Speed
                DxioGenMaxSupported,                            // Max Link Capability
                DxioAspmL0sL1,                      // ASPM
                DxioAspmDisabled,                   // ASPM L1.1 disabled
                DxioAspmDisabled,                   // ASPM L1.2 disabled
                DxioClkPmSupportDisabled            // Clock PM
                )
        },
              
#if 1
          { // P1 [0:3]- MiniSAS HD0 J13
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 32, 35, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                     DxioPortEnabled                       // Port Present
                   )
                 },
#else
                 { //P1 [0:3]- MiniSAS HD0 J13
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 32, 35, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                     DxioPortEnabled,                      // Port Present
                     0,                                    // Requested Device
                     0,                                    // Requested Function
                     DxioHotplugDisabled,             // Hotplug
                     DxioGenMaxSupported,                  // Max Link Speed
                     DxioGenMaxSupported,                  // Max Link Capability
                     DxioAspmL0sL1,                        // ASPM
                     DxioAspmDisabled,                     // ASPM L1.1 disabled
                     DxioAspmDisabled,                     // ASPM L1.2 disabled
                     DxioClkPmSupportDisabled              // Clock PM
                   )                 // Port Present
    
                 },              
#endif
#if 1
          { // P1 [4:7] - MiniSAS HD1 J14
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 36, 39, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                     DxioPortEnabled                       // Port Present
                   )
                 },
    
#else
           { // P1 [4:7] - MiniSAS HD1 J14
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 36, 39, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                     DxioPortEnabled,                      // Port Present
                     0,                                    // Requested Device
                     0,                                    // Requested Function
                     DxioHotplugDisabled,             // Hotplug
                     DxioGenMaxSupported,                  // Max Link Speed
                     DxioGenMaxSupported,                  // Max Link Capability
                     DxioAspmL0sL1,                        // ASPM
                     DxioAspmDisabled,                     // ASPM L1.1 disabled
                     DxioAspmDisabled,                     // ASPM L1.2 disabled
                     DxioClkPmSupportDisabled              // Clock PM
                   )                 // Port Present
                 },             
#endif
#if 1
          { // P1[8:15] - PCIE x8 slot J10 x8
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE40, LANE40+7, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                  DxioPortEnabled,                      // Port Present
                  0,                                    // Requested Device
                  0,                                    // Requested Function
                  DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                            // Max Link Speed
                    DxioGenMaxSupported,                            // Max Link Capability
                    DxioAspmL0sL1,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                  )
          },
#else
          { // P1[8:15] - PCIE x8 slot J10 x4 
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE40, LANE40+3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                  DxioPortEnabled,                      // Port Present
                  0,                                    // Requested Device
                  0,                                    // Requested Function
                  DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                            // Max Link Speed
                    DxioGenMaxSupported,                            // Max Link Capability
                    DxioAspmL0sL1,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                  )
          },
          { // P1[8:15] - PCIE x8 slot J10 x4
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE40+4, LANE40+7, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                  DxioPortEnabled,                      // Port Present
                  0,                                    // Requested Device
                  0,                                    // Requested Function
                  DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                            // Max Link Speed
                    DxioGenMaxSupported,                            // Max Link Capability
                    DxioAspmL0sL1,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                  )
          },
#endif
#if 1
          { // P2 [0:15] - PCIE x16 slot J7A - x16
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE80, LANE80+15, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port Present
                   0,                                    // Requested Device
                   0,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                  // Max Link Speed
                   DxioGenMaxSupported,                  // Max Link Capability
                   DxioAspmL0sL1,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
         },
#else
#if 1
          { // P2 [0:7] - PCIE x8 slot J7A - x8
                     0,
                     DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE80, LANE80+7, DxioHotplugDisabled, 1),
                     DXIO_PORT_DATA_INITIALIZER_PCIE (
                       DxioPortEnabled,                      // Port Present
                       0,                                    // Requested Device
                       0,                                    // Requested Function
                       DxioHotplugDisabled,                  // Hotplug
                       DxioGenMaxSupported,                  // Max Link Speed
                       DxioGenMaxSupported,                  // Max Link Capability
                       DxioAspmL0sL1,                        // ASPM
                       DxioAspmDisabled,                     // ASPM L1.1 disabled
                       DxioAspmDisabled,                     // ASPM L1.2 disabled
                       DxioClkPmSupportDisabled              // Clock PM
                     )
                   },
          { // P2 [8:15] - PCIE x8 slot J7A -x8
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE80+8, LANE80+15, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                        DxioPortEnabled,                      // Port Present
                           0,                                    // Requested Device
                           0,                                    // Requested Function
                           DxioHotplugDisabled,                  // Hotplug
                           DxioGenMaxSupported,                  // Max Link Speed
                           DxioGenMaxSupported,                  // Max Link Capability
                           DxioAspmL0sL1,                        // ASPM
                           DxioAspmDisabled,                     // ASPM L1.1 disabled
                           DxioAspmDisabled,                     // ASPM L1.2 disabled
                           DxioClkPmSupportDisabled              // Clock PM
                   )
          }, 
#else
          { // P2 [8:15] - PCIE x8 slot J7A -x4
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE80, LANE80+3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                        DxioPortEnabled,                      // Port Present
                           0,                                    // Requested Device
                           0,                                    // Requested Function
                           DxioHotplugDisabled,                  // Hotplug
                           DxioGenMaxSupported,                  // Max Link Speed
                           DxioGenMaxSupported,                  // Max Link Capability
                           DxioAspmL0sL1,                        // ASPM
                           DxioAspmDisabled,                     // ASPM L1.1 disabled
                           DxioAspmDisabled,                     // ASPM L1.2 disabled
                           DxioClkPmSupportDisabled              // Clock PM
                   )
          },
          { // P2 [8:15] - PCIE x8 slot J7A -x4
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE80+4, LANE80+7, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                        DxioPortEnabled,                      // Port Present
                           0,                                    // Requested Device
                           0,                                    // Requested Function
                           DxioHotplugDisabled,                  // Hotplug
                           DxioGenMaxSupported,                  // Max Link Speed
                           DxioGenMaxSupported,                  // Max Link Capability
                           DxioAspmL0sL1,                        // ASPM
                           DxioAspmDisabled,                     // ASPM L1.1 disabled
                           DxioAspmDisabled,                     // ASPM L1.2 disabled
                           DxioClkPmSupportDisabled              // Clock PM
                   )
          }, 
          { // P2 [8:15] - PCIE x8 slot J7A -x4
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE80+8, LANE80+11, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                        DxioPortEnabled,                      // Port Present
                           0,                                    // Requested Device
                           0,                                    // Requested Function
                           DxioHotplugDisabled,                  // Hotplug
                           DxioGenMaxSupported,                  // Max Link Speed
                           DxioGenMaxSupported,                  // Max Link Capability
                           DxioAspmL0sL1,                        // ASPM
                           DxioAspmDisabled,                     // ASPM L1.1 disabled
                           DxioAspmDisabled,                     // ASPM L1.2 disabled
                           DxioClkPmSupportDisabled              // Clock PM
                   )
          },
          { // P2 [8:15] - PCIE x8 slot J7A -x4
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE80+12, LANE80+15, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                        DxioPortEnabled,                      // Port Present
                           0,                                    // Requested Device
                           0,                                    // Requested Function
                           DxioHotplugDisabled,                  // Hotplug
                           DxioGenMaxSupported,                  // Max Link Speed
                           DxioGenMaxSupported,                  // Max Link Capability
                           DxioAspmL0sL1,                        // ASPM
                           DxioAspmDisabled,                     // ASPM L1.1 disabled
                           DxioAspmDisabled,                     // ASPM L1.2 disabled
                           DxioClkPmSupportDisabled              // Clock PM
                   )
          },
#endif
#endif
    
         { // S0P3 lane0-7 J6_1 X8
           0,
           DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 112, 119, DxioHotplugDisabled, 1),
           DXIO_PORT_DATA_INITIALIZER_PCIE (
             DxioPortEnabled,                      // Port Present
             0,                                    // Requested Device
             0,                                    // Requested Function
             DxioHotplugDisabled,                  // Hotplug
             DxioGenMaxSupported,                  // Max Link Speed
             DxioGenMaxSupported,                  // Max Link Capability
             DxioAspmL0sL1,                        // ASPM
             DxioAspmDisabled,                     // ASPM L1.1 disabled
             DxioAspmDisabled,                     // ASPM L1.2 disabled
             DxioClkPmSupportDisabled              // Clock PM
           )
         },
         
         { // S0P3 lane8-11, J7B
           0,
           DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 120, 123, DxioHotplugDisabled, 1),
           DXIO_PORT_DATA_INITIALIZER_PCIE (
             DxioPortEnabled,                      // Port Present
             0,                                    // Requested Device
             0,                                    // Requested Function
             DxioHotplugDisabled,                  // Hotplug
             DxioGenMaxSupported,                  // Max Link Speed
             DxioGenMaxSupported,                  // Max Link Capability
             DxioAspmL0sL1,                        // ASPM
             DxioAspmDisabled,                     // ASPM L1.1 disabled
             DxioAspmDisabled,                     // ASPM L1.2 disabled
             DxioClkPmSupportDisabled              // Clock PM
           )
         },
         
         { // S0P3 lane12-15, I350 AM2
           DESCRIPTOR_TERMINATE_LIST,
           DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 124, 127, DxioHotplugDisabled, 1),
           DXIO_PORT_DATA_INITIALIZER_PCIE (
             DxioPortEnabled,                      // Port Present
             0,                                    // Requested Device
             0,                                    // Requested Function
             DxioHotplugDisabled,                  // Hotplug
             DxioGenMaxSupported,                  // Max Link Speed
             DxioGenMaxSupported,                  // Max Link Capability
             DxioAspmL0sL1,                        // ASPM
             DxioAspmDisabled,                     // ASPM L1.1 disabled
             DxioAspmDisabled,                     // ASPM L1.2 disabled
             DxioClkPmSupportDisabled              // Clock PM
           )
         }                    
      }   // End of DXIO_PORT_DESCRIPTOR
};

//
// CPM DXIO Topology Table
//
AMD_CPM_DXIO_TOPOLOGY_TABLE gCpmDxioTopologyTableHygon65N32S1 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_DXIO_TOPOLOGY_S1, sizeof (gCpmDxioTopologyTableHygon65N32S1) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  1,  // SocketId
  {   // DXIO_PORT_DESCRIPTOR
//  { UINT32  Flags;                    /**< Descriptor flags
//    #define DXIO_ENGINE_DATA_INITIALIZER(mType, mStartLane, mEndLane, mHotplug, mGpioGroupId) {mType, mHotplug, 0, mStartLane, mEndLane, mGpioGroupId, 0, 0, 0}
//    #define DXIO_PORT_DATA_INITIALIZER_PCIE(mPortPresent, mDevAddress, mDevFunction, mHotplug, mMaxLinkSpeed, mMaxLinkCap, mAspm, mAspmL1_1, mAspmL1_2,  mClkPmSupport) \
//            {mPortPresent, mDevAddress, mDevFunction, mMaxLinkSpeed, mAspm, mAspmL1_1, mAspmL1_2, 0, mHotplug, {0, mMaxLinkCap, 0, mClkPmSupport}, {0, 0, 0}, DxioEndpointDetect}

#if 0
          { // P0 [0:3]- MiniSAS HD2 J15
                0,
                DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                )
              },
#else
          { // P0[0:3] - MiniSAS HD2 J15
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 0, 3, DxioHotplugServerExpress, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugServerExpress,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
#endif
#if 0
          { // P0[4:7] - MiniSAS HD3 J16
                0,
                DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 4, 7, DxioHotplugDisabled, 1),
                DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                )
              },
#else
          { // P0[4:7] - MiniSAS HD3 J16
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 4, 7, DxioHotplugServerExpress, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugServerExpress,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
#endif
          { // P0[8:11] - MiniSAS J19
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 11, DxioHotplugServerExpress, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugServerExpress,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
          { // P0[12:15] - MiniSAS J20
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 12, 15, DxioHotplugServerExpress, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugServerExpress,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
#if 1
          { // P1 [0:3]- MiniSAS HD4 J17
                0,
                DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 32, 35, DxioHotplugDisabled, 1),
                DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                )
              },
#else
          { // P1 [0:3]- MiniSAS HD4 J17
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 32, 35, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
#endif
#if 1
          { // P1[4:7] - MiniSAS HD4 J18
                0,
                DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 36, 39, DxioHotplugDisabled, 1),
                DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                )
              },
#else
          { // P1[4:7] - MiniSAS HD4 J18
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 36, 39, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
#endif
          { // P1[8:15] - PCIE x8 slot J8B
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 40, 47, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
#if 1
          { // P2[0:15] - PCIE x16 slot J8A
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE80, LANE80+15, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
#else
#if 1
          { // P2[0:7] - PCIE x16 slot J8A -x8
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE80, LANE80+7, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
          { // P2[8:15] - PCIE x16 slot J8A -x8
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE80+8, LANE80+15, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
#else
          { // P2[8:15] - PCIE x16 slot J8A -x4
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE80, LANE80+3, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
          { // P2[8:15] - PCIE x16 slot J8A -x4
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE80+4, LANE80+7, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
          { // P2[8:15] - PCIE x16 slot J8A -x4
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE80+8, LANE80+11, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
          { // P2[8:15] - PCIE x16 slot J8A -x4
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE80+12, LANE80+15, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
#endif
#endif
#if 1
          { // P3[0:15] - PCIE x16 slot J9
            DESCRIPTOR_TERMINATE_LIST,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE112, LANE112+15, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
#else
#if 1
          { // P3[0:15] - PCIE x16 slot J9 - x8
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE112, LANE112+7, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
          { // P3[0:15] - PCIE x16 slot J9 -x8
            DESCRIPTOR_TERMINATE_LIST,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE112+8, LANE112+15, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
#else
          { // P3[0:15] - PCIE x16 slot J9 - x4
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE112, LANE112+3, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
          { // P3[0:15] - PCIE x16 slot J9 - x4
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE112+4, LANE112+7, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
          { // P3[0:15] - PCIE x16 slot J9 - x4
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE112+8, LANE112+11, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },
          
          { // P3[0:15] - PCIE x16 slot J9 - x4
            DESCRIPTOR_TERMINATE_LIST,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, LANE112+12, LANE112+15, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGenMaxSupported,                  // Max Link Speed
              DxioGenMaxSupported,                  // Max Link Capability
              DxioAspmL0sL1,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          }
#endif
#endif
  }   // End of DXIO_PORT_DESCRIPTOR
};

/*----------------------------------------------------------------------------------------*/
/**
 * Detect PCIe Devices Automatically
 *
 * This function patches PCIe Topology Override Table by detecting PCIe devices
 * automatically.
 *
 * @param[in]     AmdCpmTablePpi                Pointer to AMD CPM Table PPI
 * @param[in]     PcieTopologyTablePtr          Pointer to PCIe Topology Table
 * @param[in]     BoardId                       Current Board ID
 *
 */
VOID
DetectPcieDevices (
  IN       AMD_CPM_TABLE_PPI            *AmdCpmTablePpi,
  IN       AMD_CPM_PCIE_TOPOLOGY_TABLE  *PcieTopologyTablePtr,
  IN       UINT16                       BoardId
  )
{

}

/*----------------------------------------------------------------------------------------*/
/**
 * CPM Override Function After AMD CPM Table PPI
 *
 * This function updates CPM OEM Tables according to setup options or the value to be detected
 * on run time after AMD CPM Table PPI is installed.
 *
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Function initialized successfully
 * @retval        EFI_ERROR      Function failed (see error for more details)
 *
 */
EFI_STATUS
EFIAPI
AmdCpmTableOverride (
  IN       EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_STATUS                        Status = EFI_SUCCESS;

  return Status;
}
