#ifndef SMU_BIOS_IF_H_
#define SMU_BIOS_IF_H_

// *** IMPORTANT ***
// SMU TEAM: Always increment the interface version if
// any structure is changed in this file
#define BIOS_IF_VERSION  0x48000001

typedef struct {
  // BLDCFG
  UINT32    MaxSusPwr; // Power [W]
  UINT32    TDC; // Current [A]
  UINT32    EDC; // Current [A]
  UINT8     DeterminismPercentageEn; // [1 means use the value in DeterminismSlider; 0 means use the default value]
  UINT8     AgeVoltageMargin_Dis;
  UINT8     TDPBoostEn;
  UINT8     spare3;
  UINT32    DeterminismPercentage; // [0 means full non-determinism; 100 means full determinism; range is between 0-100]

  // BLDCFG/CBS fan table
  UINT8     FanTable_Override; // [1 means use the full set of data specified below; 0 means use default fan table]
  UINT8     FanTable_Hysteresis;
  UINT8     FanTable_TempLow;
  UINT8     FanTable_TempMed;
  UINT8     FanTable_TempHigh;
  UINT8     FanTable_TempCritical;
  UINT8     FanTable_PwmLow;
  UINT8     FanTable_PwmMed;
  UINT8     FanTable_PwmHigh;
  UINT8     FanTable_PwmFreq;  // [0 = 25kHz; 1 = 100Hz]
  UINT8     FanTable_Polarity; // [0 = negative; 1 = positive]
  UINT8     FanTable_spare;

  // CBS debug options
  UINT8     DldoPsmMargin;        // [PSM count 1 ~ 1.8mV]
  UINT8     ForceFanPwmEn;        // [1 means use the ForceFanPwm value below]
  UINT8     ForceFanPwm;          // [% 0-100]
  UINT8     CoreStretcherDis;     // [1 means disable]
  UINT8     L3StretcherDis;       // [1 means disable]
  UINT8     CoreStretchThreshEn;  // [1 means use the CoreStretchThresh value below]
  UINT8     CoreStretchThresh;    // [1 = 2.5%]
  UINT8     L3StretchThreshEn;    // [1 means use the L3StretchThresh value below]
  UINT8     L3StretchThresh;      // [1 = 2.5%]
  UINT8     DldoBypass;           // [1 means bypass]
  UINT8     XiSeparationEn;       // [1 means use the XiSeparation value below]
  UINT8     XiSeparationHigh;     // [0 = async mode; 3 = 2 cycle; 4 = 2.5 cycle; 5 = 3 cycle; 6 = 3.5 cycle; 7 = 4 cycle]
  UINT8     XiSeparationLow;      // [0 = async mode; 3 = 2 cycle; 4 = 2.5 cycle; 5 = 3 cycle; 6 = 3.5 cycle; 7 = 4 cycle]
  UINT8     AvfsCoeffTable_Override; // [1 = override; 0 = use defaults]
  UINT8     ForceVddcrCpuVidEn;    // [1 = use the value in ForceVddcrCpuVid; 0 = use default algorithm]
  UINT8     ForceVddcrCpuVid;      // [VID code]
  UINT32    VddcrCpuVoltageMargin; // [mV]
  UINT16    FcwSlewFrac_L3FidTotalSteps;
  UINT8     FcwSlewFrac_L3FidTotalStepsEn;
  UINT8     ForceCclkFrequencyEn; // [1 = force CCLK frequency to ForceCclkFrequency; 0 = use default CCLK controller algorithm]
  UINT32    ForceCclkFrequency;   // [MHz]
  UINT8     UseTelemetryData;     // [1 = use telemetry instead of CAC power estimates for algorithms; 0 = use default]
  UINT8     WaitVidCompDis;       // [0 = use VOTF feedback from VR; 1 = use internal timer]
  UINT8     xHMIForceLinkWidthEn; // [1 = force to the value in xHMIForceLinkWidth; 0 = do not force]
  UINT8     xHMIForceLinkWidth;   // [0 = x2; 1 = x8; 2 = x16]
  UINT8     xHMIMaxLinkWidthEn;   // [1 = use the max width defined in xHMIMaxLinkWidth; 0 = use the default max width of x16]
  UINT8     xHMIMaxLinkWidth;     // [0 = x2; 1 = x8; 2 = x16]
  UINT8     spare[2];

  // CBS debug options [AVFS]
  UINT32    CorePsfFreq;
  UINT32    CorePsfTemp;
  UINT32    CorePsfSidd;
  UINT32    CorePsfCac;
  UINT32    CorePsfNumActiveCores;
  UINT32    CorePsfOtherDie;
  UINT32    CorePsfSigma;
  UINT32    L3PsfFreq;
  UINT32    L3PsfTemp;
  UINT32    L3PsfSidd;
  UINT32    L3PsfCac;
  UINT32    L3PsfNumActiveCores;
  UINT32    L3PsfOtherDie;
  UINT32    L3PsfSigma;
} PPTable_t;
typedef PPTable_t PP_TABLE;

// --------------------------------------------------------------------------------------------------
// PCIE_HOTPLUG_MAPPING
// Each MAPPING descriptor contains information that identifies how a given hot-plug slot maps onto the APU device capabilities.
// Depending on whether the port is active, the <slotNum> MAPPING descriptor specifies a mapping to a particular PCIe function/port
// in the socket. Additionally, the MAPPING descriptor also identifies the level of hot-plug capability, as well as the applicability
// of the RESET and GPIO descriptors. The MAPPING descriptor is always valid for an active hot-plug slot.
//

/// PCIe Hotplug Mapping Descriptor
typedef struct {
  UINT32    HotPlugFormat        :   3;    /**< Hotplug Format
                                              *  @li @b 0 - Simple Presence Detect
                                              *  @li @b 1 - PCIe Express Module
                                              *  @li @b 2 - Enterprise SSD
                                              */
  UINT32    Revision             :   1;    ///< GPIO Descriptor revision
  UINT32    GpioDescriptorValid  :   1;    /**< GPIO Descriptor Valid
                                              *  @li @b 0 - This descriptor should be ignored
                                              *  @li @b 1 - This descriptor is valid
                                              */
  UINT32    ResetDescriptorValid :   1;    /**< Reset Descriptor Valid
                                              *  @li @b 0 - Reset is not supported from the I2C subsystem
                                              *  @li @b 1 - Reset is supported from the I2C subsystem
                                              */
  UINT32    PortActive           :   1;    /**< This bit is essentially a valid marker indicating whether the hot-plug slot is
                                              *   implemented in the system and whether the information in the descriptor register
                                              *   is valid.
                                              *  @li @b 0 - This descriptor should be ignored
                                              *  @li @b 1 - This descriptor is valid
                                              */
  UINT32    MasterSlaveAPU       :   1;    /**< this bit indicates the APU (Master or Slave die) the PCIe device, that is
                                              *   mapped onto the hot-plug slot, resides. Note, the "Master" APU is the APU
                                              *   that hosts the I2C subsystem. The "Slave" APU communicates with the "Master"
                                              *   APU through a die-to-die communication link (DDCL).
                                              *  @li @b 0 - Connected to Master APU
                                              *  @li @b 1 - Connected to Slave APU
                                              */
  UINT32    DieID                :   4;    ///< Indicates the Satori Die number in the system
  UINT32    ApertureID           :  12;    /**< Indicates the Aperture ID assigned to the PCIe tile on the SMN network.
                                              *   This is used by the SMU to route the "SMU Status Transfer" to the correct PCIe tile.
                                              *   This can also be seen as the PCIe "core number", that contains PCIe device that is
                                              *   mapped onto the hot-plug slot.
                                              */
  UINT32    AlternateSlotNumber  :   6;    /**< When the descriptor is used for an Enterprise SSD form-factor, and the slot
                                              *   supports two PCIe links, this field serves as a pointer to the PCIE_HOTPLUG_MAPPING<slotNum>
                                              *   descriptor associated with the 'other' PCIe link. The primary descriptor shall point to
                                              *   the secondary descriptor, and the secondary descriptor must point to the primary descriptor.
                                              *   For a slot that only supports a single PCIe link, the primary descriptor must set this field to zero.
                                              */
  UINT32    PrimarySecondaryLink :   1;    /**< When the descriptor is used for an Enterprise SSD form-factor, this bit specifies whether the
                                              *   descriptor is for the primary PCIe device, or the secondary PCIe device associated with the slot.
                                              *  @li @b 0 - This is the primary PCIe device
                                              *  @li @b 1 - This is the secondary PCIe device
                                              */
  UINT32    Reserved1            :   1;    ///< Reserved for future use
} PcieHotPlugMappingDescriptor_t;
typedef PcieHotPlugMappingDescriptor_t PCIE_HOTPLUG_MAPPING;

// --------------------------------------------------------------------------------------------------
// PCIE_HOTPLUG_FUNCTION Descriptor
// Associated with each MAPPING descriptor (refer to section 5.5.1), a FUNCTION descriptor is provided that indicates where the associated hot-plug functionality
// is mapped within the I2C subsystem. Through the FUNCTION descriptor, the exact pin (or group of pins) on a specific IO byte of a specific I2C GPIO device.
// In addition, where a group of pins is allocated to the hot-plug functionality (based on the "HotPlugFormat" field of the corresponding MAPPING descriptor),
// a bitwise mask is provided to opt-out of specific functional elements of the specified hot-plug behaviour, freeing the associated pin for other potential uses.
//

/// PCIe Hotplug Function Descriptor
typedef struct {
  UINT32    I2CGpioBitSelector      :   2; /**< I2C GPIO Bit Select : for a simple presense detect usage model, this field indicates which
                                              *   bit of which IO byte is used for the presence detect signal associated with this descriptor.
                                              *   For an Enterprise SSD form-factor, bits 0 and 1 must be 0.
                                              */
  UINT32    I2CGpioNibbleSelector   :   1; /**< I2C GPIO Nibble Select : for a simple presense detect usage model, this field indicates which
                                              *   bit of which IO byte is used for the presence detect signal associated with this descriptor.
                                              *   bit 0 selects which nibble of the IO byte is used for the hot plug signals associated with
                                              *   this descriptor.
                                              */
  UINT32    I2CGpioByteMapping      :   3; /**< I2C GPIO IO Byte Mapping : indicates which IO byte of the I2C GPIO device is used for the
                                              *   hot-plug signals associated with this descriptor.
                                              */
  UINT32    I2CGpioDeviceMappingExt :   2; /**< I2C GPIO Device Mapping Ext: indicates bits 4:3 of the I2C address for the
                                              *   I2C GPIO device that is used to host the hot-plug signals associated with this descriptor.
                                              */
  UINT32    I2CGpioDeviceMapping    :   3; /**< I2C GPIO Device Mapping : indicates the lower three bits of the I2C address for the
                                              *   I2C GPIO device that is used to host the hot-plug signals associated with this descriptor.
                                              */
  UINT32    I2CDeviceType           :   2; /**< I2C Device Type : indicates which of three I2C GPIO devices is used to host the hot-plug
                                              *   signals associated with this descriptor.
                                              *  @li @b 00 - PCA9539 address and register map; in support of legacy hot-plug implementations
                                              *  @li @b 01 - PCA9535 address and register map; in support of newer hot-plug implementations
                                              *  @li @b 10 - PCA9506 address and register map; in support of high slot-capacity implementations
                                              *  @li @b 11 - reserved
                                              *   Note, PCA9535 and PCA9506 devices may be intermixed on the I2C interface.
                                              *   However, due to common addressing, a combined total of eight devices may be populated.
                                              */
  UINT32    I2CBusSegment :   3;           /**< I2C Bus Segment :  in the context of an I2C subsystem constructed with PCA9545 I2C switches,
                                              *   this field specifies the downstream I2C bus segment, on one of two PCA9545 switches, on which
                                              *   the target I2C GPIO device is located. However, for I2C topologies that are not constructed
                                              *   with any PCA9545 devices, one value is reserved to identify that the I2C GPIO device does
                                              *   not reside behind any PCA9454 I2C switch.
                                              *  @li @b 000 - PCA9545, address=00, downstream I2C bus=00
                                              *  @li @b 001 - PCA9545, address=00, downstream I2C bus=01
                                              *  @li @b 010 - PCA9545, address=00, downstream I2C bus=10
                                              *  @li @b 011 - PCA9545, address=00, downstream I2C bus=11
                                              *  @li @b 100 - PCA9545, address=01, downstream I2C bus=00
                                              *  @li @b 101 - PCA9545, address=01, downstream I2C bus=01
                                              *  @li @b 110 - PCA9545, address=01, downstream I2C bus=10
                                              *  @li @b 111 - no PCA9545; I2C bus directly connected to APU socket
                                              */
  UINT32    FunctionMask :   8;            /**< Function Mask : when the descriptor is used for a multi-bit hot-plug functional format,
                                              *   this field can be used to opt-out of specific pin functionality. When an 8bit GPIO group
                                              *   is allocated to the hot-plug function, all eight bits of this field are valid. When a
                                              *   4bit GPIO group is allocated to the hot-plug function, only the lower four bits of this
                                              *   field are valid. When valid, if bitN of this field is set, the sub-element of the hot-plug
                                              *   function that is mapped to the Nth IO bit of the byte/nibble is disabled. Once disabled
                                              *   via this field, the associated pin is made available for other potential purposes such
                                              *   as a RESET or GPIO capability.
                                              */
  UINT32    PortMapping      :   5;        ///< Logical Bridge Id for port mapping
  UINT32    Reserved         :   1;        ///< Reserved for future use
  UINT32    I2CBusSegmentExt :   2;        /**< I2C Bus Segment Ext : optional upper bits for additional I2C switches
                                              *  This field is populated with bits 4:3 of I2CBusSegment (above) if present.
                                              *  @li @b 01000 - PCA9545, address=01, downstream I2C bus=11
                                              *  @li @b 01001 - PCA9545, address=02, downstream I2C bus=00
                                              *  @li @b 01010 - PCA9545, address=02, downstream I2C bus=01
                                              *  @li @b 01011 - PCA9545, address=02, downstream I2C bus=10
                                              *  @li @b 01100 - PCA9545, address=02, downstream I2C bus=11
                                              *  @li @b 01101 - PCA9545, address=03, downstream I2C bus=00
                                              *  @li @b 01110 - PCA9545, address=03, downstream I2C bus=01
                                              *  @li @b 01111 - PCA9545, address=03, downstream I2C bus=10
                                              *  @li @b 10000 - PCA9545, address=03, downstream I2C bus=11
                                              */
} PcieHotPlugFunctionDescriptor_t;
typedef PcieHotPlugFunctionDescriptor_t PCIE_HOTPLUG_FUNCTION;

// --------------------------------------------------------------------------------------------------
// PCIE_HOTPLUG_RESET Descriptor
// When PCIE_HOTPLUG_MAPPING<slotNum>.ResetDescriptorVld is set to 0, reset  functionality is not tied to the hot-plug slot and must be furnished in
// some other manner. However, when the ResetDescriptorVld bit-field is set to 1, the RESET descriptor for the <slotNum> link specifies the exact pin
// within the hot-plug I2C subsystem onto which the reset functionality is mapped, enabling control of the reset functionality through a BIOS
// accessible API.

/// PCIe Hotplug Reset Descriptor
typedef struct {
  UINT32    Reserved0            :   3;    ///< Reserved for future use
  UINT32    I2CGpioByteMapping   :   3;    /**< I2C GPIO IO Byte Mapping : indicates which IO byte of the I2C GPIO device is used for the
                                              *   hot-plug signals associated with this descriptor.
                                              */
  UINT32    Reserved1            :   2;    ///< Reserved for future use
  UINT32    I2CGpioDeviceMapping :   3;    /**< I2C GPIO Device Mapping : indicates the lower three bits of the I2C address for the
                                              *   I2C GPIO device that is used to host the hot-plug signals associated with this descriptor.
                                              */
  UINT32    I2CDeviceType        :   2;    /**< I2C Device Type : indicates which of three I2C GPIO devices is used to host the hot-plug
                                              *   signals associated with this descriptor.
                                              *  @li @b 00 - PCA9539 address and register map; in support of legacy hot-plug implementations
                                              *  @li @b 01 - PCA9535 address and register map; in support of newer hot-plug implementations
                                              *  @li @b 10 - PCA9506 address and register map; in support of high slot-capacity implementations
                                              *  @li @b 11 - reserved
                                              *   Note, PCA9535 and PCA9506 devices may be intermixed on the I2C interface.
                                              *   However, due to common addressing, a combined total of eight devices may be populated.
                                              */
  UINT32    I2CBusSegment :   3;           /**< I2C Bus Segment :  in the context of an I2C subsystem constructed with PCA9545 I2C switches,
                                              *   this field specifies the downstream I2C bus segment, on one of two PCA9545 switches, on which
                                              *   the target I2C GPIO device is located. However, for I2C topologies that are not constructed
                                              *   with any PCA9545 devices, one value is reserved to identify that the I2C GPIO device does
                                              *   not reside behind any PCA9454 I2C switch.
                                              *  @li @b 000 - PCA9545, address=00, downstream I2C bus=00
                                              *  @li @b 001 - PCA9545, address=00, downstream I2C bus=01
                                              *  @li @b 010 - PCA9545, address=00, downstream I2C bus=10
                                              *  @li @b 011 - PCA9545, address=00, downstream I2C bus=11
                                              *  @li @b 100 - PCA9545, address=01, downstream I2C bus=00
                                              *  @li @b 101 - PCA9545, address=01, downstream I2C bus=01
                                              *  @li @b 110 - PCA9545, address=01, downstream I2C bus=10
                                              *  @li @b 111 - no PCA9545; I2C bus directly connected to APU socket
                                              */
  UINT32    ResetSelect :   8;             /**< Reset Select : this bit-field is used to specify which pin within the selected I2C IO byte
                                              *   (identified by the other fields in this descriptor) is used for the reset function associated
                                              *   with the <slotNum> PCIe link. Since reset is a singular function, only one bit in this field
                                              *   may be set to one. When the Nth bit of this field is set to 1, the Nth bit of the selected I2C
                                              *   IO byte serves as the platform reset signal for the associated PCIe link.
                                              *   Note, although this descriptor identifies a single bit, this field has been defined in this
                                              *   manner so as to be consistent with both the "FunctionMask" field in the FUNCTION descriptor
                                              *   and the "GPIOSelect" field in the GPIO descriptor.
                                              */
  UINT32    Reserved2        :   6;        ///< Reserved for future use
  UINT32    I2CBusSegmentExt :   2;        ///< I2C Bus Segment Ext
} PcieHotPlugResetDescriptor_t;
typedef PcieHotPlugResetDescriptor_t PCIE_HOTPLUG_RESET;

// --------------------------------------------------------------------------------------------------
// SMU_GENERIC_GPIO Descriptor
// In order to provide HYGON's customers with flexibility to customize and differentiate their own hot-plug deployments, the hot-plug a facility incorporates
// generalized GPIO extensibility through the SMU-managed I2C subsystem. Through the use of GPIO descriptors, generalized GPIO pins can be allocated within
// the hot-plug I2C subsystem, and are exposed to the platform software through a BIOS accessible API. For each I2C IO bit that is mapped into the generalized
// GPIO facility, the bit is capable of operating as either:
// - an output, used to drive a platform level signal
// - an input, used to ascertain platform-specific information
// - a level-sensitive interrupt input, used to signal platform events to the platform BIOS or other system software
// Within this GPIO facility, the I2C GPIOs can be conceptually tied to the per-slot functionality, or can even be stand-alone GPIO functions for purposes
// unrelated to PCIe hot-plug. The former association is accomplished by setting PCIE_HOTPLUG_MAPPING<slotNum>.GPIODescriptorVld = 1, in which case the
// SMU_GENERIC_GPIO<slotNum> descriptor is used for GPIO functionality associated with the <slotNum> hot-plug slot.
// Otherwise, those SMU_GENERIC_GPIO<descNum> descriptors not allocated to the MAPPING registers can be used for potentially orthogonal purposes.

/// General Purpose GPIO Descriptor
typedef struct {
  UINT32    Reserved0            :   3;    ///< Reserved for future use
  UINT32    I2CGpioByteMapping   :   3;    /**< I2C GPIO IO Byte Mapping : indicates which IO byte of the I2C GPIO device is used for the
                                              *   hot-plug signals associated with this descriptor.
                                              */
  UINT32    Reserved1            :   2;    ///< Reserved for future use
  UINT32    I2CGpioDeviceMapping :   3;    /**< I2C GPIO Device Mapping : indicates the lower three bits of the I2C address for the
                                              *   I2C GPIO device that is used to host the hot-plug signals associated with this descriptor.
                                              */
  UINT32    I2CDeviceType        :   2;    /**< I2C Device Type : indicates which of three I2C GPIO devices is used to host the hot-plug
                                              *   signals associated with this descriptor.
                                              *  @li @b 00 - PCA9539 address and register map; in support of legacy hot-plug implementations
                                              *  @li @b 01 - PCA9535 address and register map; in support of newer hot-plug implementations
                                              *  @li @b 10 - PCA9506 address and register map; in support of high slot-capacity implementations
                                              *  @li @b 11 - reserved
                                              *   Note, PCA9535 and PCA9506 devices may be intermixed on the I2C interface.
                                              *   However, due to common addressing, a combined total of eight devices may be populated.
                                              */
  UINT32    I2CBusSegment :   3;           /**< I2C Bus Segment :  in the context of an I2C subsystem constructed with PCA9545 I2C switches,
                                              *   this field specifies the downstream I2C bus segment, on one of two PCA9545 switches, on which
                                              *   the target I2C GPIO device is located. However, for I2C topologies that are not constructed
                                              *   with any PCA9545 devices, one value is reserved to identify that the I2C GPIO device does
                                              *   not reside behind any PCA9454 I2C switch.
                                              *  @li @b 000 - PCA9545, address=00, downstream I2C bus=00
                                              *  @li @b 001 - PCA9545, address=00, downstream I2C bus=01
                                              *  @li @b 010 - PCA9545, address=00, downstream I2C bus=10
                                              *  @li @b 011 - PCA9545, address=00, downstream I2C bus=11
                                              *  @li @b 100 - PCA9545, address=01, downstream I2C bus=00
                                              *  @li @b 101 - PCA9545, address=01, downstream I2C bus=01
                                              *  @li @b 110 - PCA9545, address=01, downstream I2C bus=10
                                              *  @li @b 111 - no PCA9545; I2C bus directly connected to APU socket
                                              */
  UINT32    GpioSelect          :   8;     /**< GPIO Select : this bit-field is used to specify which pin within the selected I2C IO byte
                                              *   (identified by the other fields in this descriptor) is used for GPIO functionality. For each
                                              *   bit that is set to 1, the corresponding pin on the I2C IO Byte (selected by the remaining
                                              *   fields in this descriptor) are assigned to GPIO functionality. Any population of bits, from
                                              *   one to eight, can be selected using this field.
                                              */
  UINT32    GpioInterruptEnable :   8;     /**< GPIO Interrupt Enable : each bit of this field acts as an interrupt enable for the
                                              *   corresponding bit in the I2C IO Byte pointed to by this descriptor. When the bit in the
                                              *   IO Byte is used as a GPIO, and it used as an input, the corresponding enable bit determines
                                              *   whether an SMI/SCI interrupt will be generated in response to a change in the input value.
                                              *   When the corresponding mask bit is '1', an SCI/SMI interrupt will be generated as a consequence of
                                              *   a change on the I2C GPIO bit. When the corresponding mask bit is '0', no interrupt will be generated.
                                              */
} SmuGenericGpioDescriptor_t;
typedef SmuGenericGpioDescriptor_t PCIE_HOTPLUG_GPIO;

typedef struct {
  UINT32  PhyPortId                 :   5;
  UINT32  StartLane                 :   4;
  UINT32  EndLane                   :   4;
  UINT32  NbioId                    :   3;
  UINT32  reserved                  :   16;
} PcieHotPlugPatchDescriptor_t;

typedef struct {
  PcieHotPlugMappingDescriptor_t     MappingDescriptor[64];
  PcieHotPlugFunctionDescriptor_t    FunctionDescriptor[64];
  PcieHotPlugResetDescriptor_t       ResetDescriptor[64];
  SmuGenericGpioDescriptor_t         GpioDescriptor[64];
  PcieHotPlugPatchDescriptor_t       HotPlugPatch[64];
} PcieHotPlugConfigTable_t;
typedef PcieHotPlugConfigTable_t PCIE_HOTPLUG_CONFIG_TABLE;

#endif
