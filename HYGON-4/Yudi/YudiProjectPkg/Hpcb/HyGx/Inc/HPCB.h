/* $NoKeywords:$ */

/**
* @file
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
* ***************************************************************************
*
*/

#ifndef _HPCB_H_
#define _HPCB_H_


//
// DDR4
//
#define VOLT1_2_ENCODED_VAL           0
#define VOLT1_TBD1_ENCODED_VAL        2
#define VOLT1_TBD2_ENCODED_VAL        4
#define VOLT_DDR4_ENCODED_LIMIT       1    // Maximum Encoded value for DDR4
#define VOLT1_2_MILLIVOLTS            1200

//
// DDR5
//
#define VOLT1_1_ENCODED_VAL           1
#define VOLT_DDR5_ENCODED_LIMIT       3    // Maximum Encoded value for DDR5
#define VOLT1_1_MILLIVOLTS            1100

//
// Unsupport DDR
//
#define VOLT_STARTING_ENCODED_VAL     0    // Lowest value for all technologies
#define VOLT_UNSUPPORTED_ENCODED_VAL  0xFF

//
// Macro to determine the limit value for each DRAM type (for looping)
//
#define GET_ENCODED_VOLTAGE_LIMIT(TechType)  ( \
                                               (TechType == DDR4_TECHNOLOGY) ? \
                                               VOLT_DDR4_ENCODED_LIMIT : \
                                               (TechType == DDR5_TECHNOLOGY) ? \
                                               VOLT_DDR5_ENCODED_LIMIT : \
                                               VOLT_STARTING_ENCODED_VAL)
//
// Conversion Macro from Interface DDRVoltage value to internal index
//
#define CONVERT_VDDIO_TO_ENCODED(VddIo, TechType)  ( \
                                                     (TechType == DDR4_TECHNOLOGY) ? \
                                                     ((VddIo == VOLT1_2) ? VOLT1_2_ENCODED_VAL : \
                                                      VOLT_UNSUPPORTED_ENCODED_VAL) : \
                                                     (TechType == DDR5_TECHNOLOGY) ? \
                                                     ((VddIo == VOLT1_1) ? VOLT1_1_ENCODED_VAL) :\
                                                     VOLT_UNSUPPORTED_ENCODED_VAL)
//
// Conversion Macro from internal index to Interface DDRVoltage
//
#define CONVERT_ENCODED_TO_VDDIO(EncodedVal, TechType)  ( \
                                                          (TechType == DDR4_TECHNOLOGY) ? \
                                                          ((EncodedVal == VOLT1_2_ENCODED_VAL) ? VOLT1_2 : \
                                                           VOLT_UNSUPPORTED) : \
                                                          (TechType == DDR5_TECHNOLOGY) ? \
                                                          ((EncodedVal == VOLT1_1_ENCODED_VAL) ? VOLT1_1 : \
                                                           VOLT_UNSUPPORTED) : \
                                                          VOLT_UNSUPPORTED)

//
// Conversion Macro from internal index to Interface DDRVoltage in Millivolts
//
#define CONVERT_ENCODED_TO_VDDIO_MILLIVOLTS(EncodedVal, TechType)  ( \
                                                                     (TechType == DDR4_TECHNOLOGY) ? \
                                                                     ((EncodedVal == VOLT1_2_ENCODED_VAL) ? VOLT1_2_MILLIVOLTS : \
                                                                      0) : \
                                                                     (TechType == DDR5_TECHNOLOGY) ? \
                                                                     ((EncodedVal == VOLT1_1_ENCODED_VAL) ? VOLT1_1_MILLIVOLTS : \
                                                                      0) : \
                                                                     0)

/// Dram technology type
typedef enum {
  DDR2_TECHNOLOGY,        ///< DDR2 technology
  DDR3_TECHNOLOGY,        ///< DDR3 technology
  GDDR5_TECHNOLOGY,       ///< GDDR5 technology
  DDR4_TECHNOLOGY,        ///< DDR4 technology
  LPDDR3_TECHNOLOGY,      ///< LPDDR3 technology
  DDR5_TECHNOLOGY,        ///< DDR5 technology
  UNSUPPORTED_TECHNOLOGY, ///< Unsupported technology
} TECHNOLOGY_TYPE;

/// Dram technology type
typedef enum {
  SODIMM_PACKAGE,        ///< SODIMM package
  UDIMM_PACKAGE,         ///< UDIMM Package
  RDIMM_PACKAGE,         ///< RDIMM Package
  DRAMDOWN_PACKAGE,      ///< DRAM Down Package
  LRDIMM_PACKAGE,        ///< LRDIMM Package
  NVRDIMM_PACKAGE,       ///< NV-RDIMM PACKAGE
  UNSUPPORTED_PACKAGE,   ///< Unsupported package
} DRAM_PACKAGE_TYPE;

/// I2C_MUX_INFO for SPD
typedef struct _SPD_MUX_INFO {
  UINT8    SpdMuxPresent;     ///< Spd I2C MUX present or not, TRUE - Present, FALSE - Not Present
  UINT8    MuxSmbusAddress;   ///< I2C Mux Address
  UINT8    MuxChannel;        ///< I2C Mux Channel assocaited iwth this SPD
  UINT8    Reserved;          ///< Reserved
} SPD_MUX_INFO;

// -----------------------------------------------------------------------------
///
/// SPD Data for each DIMM.
///
typedef struct _SPD_DEF_STRUCT {
  BOOLEAN              SpdValid; ///< Indicates that the SPD is valid
  BOOLEAN              DimmPresent; ///< Indicates that the DIMM is present and Data is valid
  UINT8                PageAddress; ///< Indicates the 256 Byte EE Page the data belongs to
  // 0 = Lower Page
  // 1 = Upper Page
  BOOLEAN              NvDimmPresent; ///< Indicates this DIMM is NVDIMM
  UINT16               SPDManufacturerIDCode;    /// SPD Manufacturer ID
  UINT8                SPDDeviceType;
  UINT8                SPDRevisionNumber;
  UINT16               PMIC0ManufacturerIDCode;  /// PMIC 0 Manufacturer ID
  UINT8                PMIC0DeviceType;
  UINT8                PMIC0RevisionNumber;
  UINT16               PMIC1ManufacturerIDCode;  /// PMIC 1 Manufacturer ID
  UINT8                PMIC1DeviceType;
  UINT8                PMIC1RevisionNumber;
  UINT16               PMIC2ManufacturerIDCode;  /// PMIC 2 Manufacturer ID
  UINT8                PMIC2DeviceType;
  UINT8                PMIC2RevisionNumber;
  UINT16               ThermalSensorManufacturerIDCode;   /// Thermal Sensor Manufacturer ID
  UINT8                ThermalSensorDeviceType;
  UINT8                ThermalSensorRevisionNumber;
  UINT16               RCDManufacturerIDCode;             /// Registering CLock Driver Manufacturer ID
  UINT8                RCDDeviceType;
  UINT8                RCDRevisionNumber;
  UINT16               DataBufferManufacturerIDCode;      /// Data Buffer Manufacturer ID
  UINT8                DBDeviceType;
  UINT8                DBRevisionNumber;
  UINT16               ModuleManufacturersIDCode;         /// Module Manufacturer's ID
  UINT8                ModuleRevisionCode;
  UINT8                Reserved1;
  UINT16               DramManufacturersIDCode;           /// DRAM Manufacturer's ID
  UINT8                DramStepping;
  UINT8                Reserved0;
  UINT32               Address;                 ///< SMBus address of the DRAM
  SPD_MUX_INFO         SpdMuxInfo;              ///< Mux and Channel Number associated with this DIMM
  TECHNOLOGY_TYPE      Technology; ///< Indicates the type of Technology used in SPD
                                   ///< DDR3_TECHNOLOGY = Use DDR3 DIMMs
                                   ///< DDR4_TECHNOLOGY = Use DDR4 DIMMs
                                   ///< DDR5_TECHNOLOGY = Use DDR5 DIMMs
                                   ///< LPDDR3_TECHNOLOGY = Use LPDDR3
  DRAM_PACKAGE_TYPE    Package; ///< Indicates the package type
  // SODIMM_PACKAGE = SODIMM package
  // UDIMM_PACKAGE = UDIMM Package
  // RDIMM_PACKAGE = RDIMM Package
  // DRAMDOWN_PACKAGE = DRAM Down Package
  UINT8                SocketNumber;  ///< Indicates the socket number
  UINT8                ChannelNumber; ///< Indicates the channel number
  UINT8                DimmNumber;    ///< Indicates the channel number
  UINT8                Reserved2;     ///< Reserved
  UINT8                Data[1024];    ///< Buffer for 1024 Bytes of SPD data from DIMM
} SPD_DEF_STRUCT;

typedef struct _SPD_NODATA_DEF_STRUCT {
  BOOLEAN              SpdValid; ///< Indicates that the SPD is valid
  BOOLEAN              DimmPresent; ///< Indicates that the DIMM is present and Data is valid
  UINT8                PageAddress; ///< Indicates the 256 Byte EE Page the data belongs to
  // 0 = Lower Page
  // 1 = Upper Page
  BOOLEAN              NvDimmPresent; ///< Indicates this DIMM is NVDIMM
  UINT32               DramManufacturersIDCode; /// DRAM Manufacture ID
  UINT32               Address;                 ///< SMBus address of the DRAM
  SPD_MUX_INFO         SpdMuxInfo;              ///< Mux and Channel Number associated with this DIMM
  TECHNOLOGY_TYPE      Technology; ///< Indicates the type of Technology used in SPD
                                   ///< DDR3_TECHNOLOGY = Use DDR3 DIMMs
                                   ///< DDR5_TECHNOLOGY = Use DDR5 DIMMs
                                   ///< LPDDR3_TECHNOLOGY = Use LPDDR3
  DRAM_PACKAGE_TYPE    Package; ///< Indicates the package type
  // SODIMM_PACKAGE = SODIMM package
  // UDIMM_PACKAGE = UDIMM Package
  // RDIMM_PACKAGE = RDIMM Package
  // DRAMDOWN_PACKAGE = DRAM Down Package
  UINT8                SocketNumber;  ///< Indicates the socket number
  UINT8                ChannelNumber; ///< Indicates the channel number
  UINT8                DimmNumber;    ///< Indicates the channel number
  UINT8                Reserved2;     ///< Reserved
} SPD_NODATA_DEF_STRUCT;

/////Start PsMemoryCOnfigurations.h

/*----------------------------------------------------------------------------------------
*          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
*----------------------------------------------------------------------------------------
*/
#ifndef PSO_ENTRY
  #define PSO_ENTRY  UINT8
#endif

/*----------------------------------------------------------------------------------------
*                 D E F I N I T I O N S     A N D     M A C R O S
*----------------------------------------------------------------------------------------
*/
/*----------------------------------------------------------------------------------------
*               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
*----------------------------------------------------------------------------------------
*/
/*----------------------------------------------------------------------------------------
*                 PLATFORM SPECIFIC MEMORY DEFINITIONS
*----------------------------------------------------------------------------------------
*/
///
/// Memory Speed and DIMM Population Masks
///
///< DDR Speed Masks
///< Specifies the DDR Speed on a memory channel
///
#define ANY_SPEED  0xFFFFFFFFul
#define DDR400     ((UINT32)1 << (DDR400_FREQUENCY  / 66))
#define DDR533     ((UINT32)1 << (DDR533_FREQUENCY  / 66))
#define DDR667     ((UINT32)1 << (DDR667_FREQUENCY  / 66))
#define DDR800     ((UINT32)1 << (DDR800_FREQUENCY  / 66))
#define DDR1066    ((UINT32)1 << (DDR1066_FREQUENCY / 66))
#define DDR1333    ((UINT32)1 << (DDR1333_FREQUENCY / 66))
#define DDR1600    ((UINT32)1 << (DDR1600_FREQUENCY / 66))
#define DDR1866    ((UINT32)1 << (DDR1866_FREQUENCY / 66))
#define DDR2133    ((UINT32)1 << (DDR2133_FREQUENCY / 66))
#define DDR2400    ((UINT32)1 << (DDR2400_FREQUENCY / 66))
#define DDR2667    ((UINT32)1 << (DDR2667_FREQUENCY / 66))
#define DDR2933    ((UINT32)1 << (DDR2933_FREQUENCY / 66))
#define DDR3200    ((UINT32)1 << (DDR3200_FREQUENCY / 66))
#define DDR3600    ((UINT32)1 << (DDR3600_FREQUENCY / 200))
#define DDR4000    ((UINT32)1 << (DDR4000_FREQUENCY / 200))
#define DDR4400    ((UINT32)1 << (DDR4400_FREQUENCY / 200))
#define DDR4800    ((UINT32)1 << (DDR4800_FREQUENCY / 200))
#define DDR5200    ((UINT32)1 << (DDR5200_FREQUENCY / 200))
#define DDR5600    ((UINT32)1 << (DDR5600_FREQUENCY / 200))
#define DDR6000    ((UINT32)1 << (DDR6000_FREQUENCY / 200))
#define DDR6400    ((UINT32)1 << (DDR6400_FREQUENCY / 200))

///
///< DIMM POPULATION MASKS
///< Specifies the DIMM Population on a channel (can be added together to specify configuration).
///< ex. SR_DIMM0 + SR_DIMM1                       : Single Rank Dimm in slot 0 AND Slot 1
///< SR_DIMM0 + DR_DIMM0 + SR_DIMM1 +DR_DIMM1  : Single OR Dual rank in Slot 0 AND Single OR Dual rank in Slot 1
///
#define ANY_       0xFF       ///< Any dimm configuration the current channel
#define SR_DIMM0   0x0001     ///< Single rank dimm in slot 0 on the current channel
#define SR_DIMM1   0x0010     ///< Single rank dimm in slot 1 on the current channel
#define SR_DIMM2   0x0100     ///< Single rank dimm in slot 2 on the current channel
#define SR_DIMM3   0x1000     ///< Single rank dimm in slot 3 on the current channel
#define DR_DIMM0   0x0002     ///< Dual rank dimm in slot 0 on the current channel
#define DR_DIMM1   0x0020     ///< Dual rank dimm in slot 1 on the current channel
#define DR_DIMM2   0x0200     ///< Dual rank dimm in slot 2 on the current channel
#define DR_DIMM3   0x2000     ///< Dual rank dimm in slot 3 on the current channel
#define QR_DIMM0   0x0004     ///< Quad rank dimm in slot 0 on the current channel
#define QR_DIMM1   0x0040     ///< Quad rank dimm in slot 1 on the current channel
#define QR_DIMM2   0x0400     ///< Quad rank dimm in slot 2 on the current channel
#define QR_DIMM3   0x4000     ///< Quad rank dimm in slot 3 on the current channel
#define LR_DIMM0   0x0001     ///< Lrdimm in slot 0 on the current channel
#define LR_DIMM1   0x0010     ///< Lrdimm in slot 1 on the current channel
#define LR_DIMM2   0x0100     ///< Lrdimm in slot 2 on the current channel
#define LR_DIMM3   0x1000     ///< Lrdimm in slot 3 on the current channel
#define ANY_DIMM0  0x000F     ///< Any Dimm combination in slot 0 on the current channel
#define ANY_DIMM1  0x00F0     ///< Any Dimm combination in slot 1 on the current channel
#define ANY_DIMM2  0x0F00     ///< Any Dimm combination in slot 2 on the current channel
#define ANY_DIMM3  0xF000     ///< Any Dimm combination in slot 3 on the current channel
///
///< CS POPULATION MASKS
///< Specifies the CS Population on a channel (can be added together to specify configuration).
///< ex. CS0 + CS1                       : CS0 and CS1 apply to the setting
///
#define CS_ANY_  0xFF   ///< Any CS configuration
#define CS0_     0x01   ///< CS0 bit map mask
#define CS1_     0x02   ///< CS1 bit map mask
#define CS2_     0x04   ///< CS2 bit map mask
#define CS3_     0x08   ///< CS3 bit map mask
#define CS4_     0x10   ///< CS4 bit map mask
#define CS5_     0x20   ///< CS5 bit map mask
#define CS6_     0x40   ///< CS6 bit map mask
#define CS7_     0x80   ///< CS7 bit map mask
///
///< Number of Dimms on the current channel
///< This is a mask used to indicate the number of dimms in a channel
///< They can be added to indicate multiple conditions (i.e 1 OR 2 Dimms)
///
#define ANY_NUM     0xFF      ///< Any number of Dimms
#define NO_DIMM     0x00      ///< No Dimms present
#define ONE_DIMM    0x01      ///< One dimm Poulated on the current channel
#define TWO_DIMM    0x02      ///< Two dimms Poulated on the current channel
#define THREE_DIMM  0x04      ///< Three dimms Poulated on the current channel
#define FOUR_DIMM   0x08      ///< Four dimms Poulated on the current channel

///
///< DIMM VOLTAGE MASKS
///
#define VOLT_ANY_  0xFF       ///< Any voltage configuration
#define VOLT1_5_   0x01       ///< Voltage 1.5V bit map mask
#define VOLT1_35_  0x02       ///< Voltage 1.35V bit map mask
#define VOLT1_25_  0x04       ///< Voltage 1.25V bit map mask

//
// < Not applicable
//
#define NA_  0                ///< Not applicable

/*----------------------------------------------------------------------------------------
*
* Platform Specific Override Definitions for Socket, Channel and Dimm
* This indicates where a platform override will be applied.
*
*----------------------------------------------------------------------------------------
*/
///
///< SOCKET MASKS
///< Indicates associated processor sockets to apply override settings
///
#define ANY_SOCKET  0xFF       ///< Apply to all sockets
#define SOCKET0     0x01       ///< Apply to socket 0
#define SOCKET1     0x02       ///< Apply to socket 1
#define SOCKET2     0x04       ///< Apply to socket 2
#define SOCKET3     0x08       ///< Apply to socket 3
#define SOCKET4     0x10       ///< Apply to socket 4
#define SOCKET5     0x20       ///< Apply to socket 5
#define SOCKET6     0x40       ///< Apply to socket 6
#define SOCKET7     0x80       ///< Apply to socket 7
///
///< CHANNEL MASKS
///< Indicates Memory channels where override should be applied
///
#define ANY_CHANNEL  0xFF      ///< Apply to all Memory channels
#define CHANNEL_A    0x01      ///< Apply to Channel A
#define CHANNEL_B    0x02      ///< Apply to Channel B
#define CHANNEL_C    0x04      ///< Apply to Channel C
#define CHANNEL_D    0x08      ///< Apply to Channel D
#define CHANNEL_E    0x10      ///< Apply to Channel E
#define CHANNEL_F    0x20      ///< Apply to Channel F
#define CHANNEL_G    0x40      ///< Apply to Channel G
#define CHANNEL_H    0x80      ///< Apply to Channel H
///
/// DIMM MASKS
/// Indicates Dimm Slots where override should be applied
///
#define ALL_DIMMS  0xFF        ///< Apply to all dimm slots
#define DIMM0      0x01        ///< Apply to Dimm Slot 0
#define DIMM1      0x02        ///< Apply to Dimm Slot 1
#define DIMM2      0x04        ///< Apply to Dimm Slot 2
#define DIMM3      0x08        ///< Apply to Dimm Slot 3
///
/// REGISTER ACCESS MASKS
/// Not supported as an at this time
///
#define ACCESS_NB0     0x0
#define ACCESS_NB1     0x1
#define ACCESS_NB2     0x2
#define ACCESS_NB3     0x3
#define ACCESS_NB4     0x4
#define ACCESS_PHY     0x5
#define ACCESS_DCT_XT  0x6
///
/// MOTHER BOARD DESIGN LAYERS MASKS
/// Indicates the layer design of mother board
///
#define LAYERS_4  0x0
#define LAYERS_6  0x1

/*----------------------------------------------------------------------------------------
*
* Platform Specific Overriding Table Definitions
*
*----------------------------------------------------------------------------------------
*/

#define PSO_END                        0  ///< Table End
#define PSO_CKE_TRI                    1  ///< CKE Tristate Map
#define PSO_ODT_TRI                    2  ///< ODT Tristate Map
#define PSO_CS_TRI                     3  ///< CS Tristate Map
#define PSO_MAX_DIMMS                  4  ///< Max Dimms per channel
#define PSO_CLK_SPEED                  5  ///< Clock Speed
#define PSO_DIMM_TYPE                  6  ///< Dimm Type
#define PSO_MEMCLK_DIS                 7  ///< MEMCLK Disable Map
#define PSO_MAX_CHNLS                  8  ///< Max Channels per Socket
#define PSO_BUS_SPEED                  9  ///< Max Memory Bus Speed
#define PSO_MAX_CHIPSELS               10 ///< Max Chipsel per Channel
#define PSO_MEM_TECH                   11 ///< Channel Memory Type
#define PSO_WL_SEED                    12 ///< DDR3 Write Levelization Seed delay
#define PSO_RXEN_SEED                  13 ///< Hardwared based RxEn seed
#define PSO_NO_LRDIMM_CS67_ROUTING     14 ///< CS6 and CS7 are not Routed to all Memoy slots on a channel for LRDIMMs
#define PSO_SOLDERED_DOWN_SODIMM_TYPE  15 ///< Soldered down SODIMM type
#define PSO_LVDIMM_VOLT1_5_SUPPORT     16 ///< Force LvDimm voltage to 1.5V
#define PSO_MIN_RD_WR_DATAEYE_WIDTH    17 ///< Min RD/WR dataeye width
#define PSO_CPU_FAMILY_TO_OVERRIDE     18 ///< CPU family signature to tell following PSO macros are CPU family dependent
#define PSO_MAX_SOLDERED_DOWN_DIMMS    19 ///< Max Soldered-down Dimms per channel
#define PSO_MEMORY_POWER_POLICY        20 ///< Memory power policy override
#define PSO_MOTHER_BOARD_LAYERS        21 ///< Mother board layer design

/*----------------------------------
* CONDITIONAL PSO SPECIFIC ENTRIES
*---------------------------------*/
// Condition Types
#define CONDITIONAL_PSO_MIN  100      ///< Start of Conditional Entry Types
#define PSO_CONDITION_AND    100      ///< And Block - Start of Conditional block
#define PSO_CONDITION_LOC    101      ///< Location - Specify Socket, Channel, Dimms to be affected
#define PSO_CONDITION_SPD    102      ///< SPD - Specify a specific SPD value on a Dimm on the channel
#define PSO_CONDITION_REG    103      // Reserved
#define PSO_CONDITION_MAX    103      ///< End Of Condition Entry Types
// Action Types
#define PSO_ACTION_MIN         120    ///< Start of Action Entry Types
#define PSO_ACTION_ODT         120    ///< ODT values to override
#define PSO_ACTION_ADDRTMG     121    ///< Address/Timing values to override
#define PSO_ACTION_ODCCONTROL  122    ///< ODC Control values to override
#define PSO_ACTION_SLEWRATE    123    ///< Slew Rate value to override
#define PSO_ACTION_REG         124    // Reserved
#define PSO_ACTION_SPEEDLIMIT  125    ///< Memory Bus speed Limit based on configuration
#define PSO_ACTION_MAX         125    ///< End of Action Entry Types
#define CONDITIONAL_PSO_MAX    139    ///< End of Conditional Entry Types

/*----------------------------------
* TABLE DRIVEN PSO SPECIFIC ENTRIES
*---------------------------------*/
// Condition descriptor
#define PSO_TBLDRV_CONFIG  200        ///< Configuration Descriptor

// Overriding entry types
#define PSO_TBLDRV_START         210  ///< Start of Table Driven Overriding Entry Types
#define PSO_TBLDRV_SPEEDLIMIT    210  ///< Speed Limit
#define PSO_TBLDRV_ODT_RTTNOM    211  ///< RttNom
#define PSO_TBLDRV_ODT_RTTWR     212  ///< RttWr
#define PSO_TBLDRV_ODTPATTERN    213  ///< Odt Patterns
#define PSO_TBLDRV_ADDRTMG       214  ///< Address/Timing values
#define PSO_TBLDRV_ODCCTRL       215  ///< ODC Control values
#define PSO_TBLDRV_SLOWACCMODE   216  ///< Slow Access Mode
#define PSO_TBLDRV_MR0_CL        217  ///< MR0[CL]
#define PSO_TBLDRV_MR0_WR        218  ///< MR0[WR]
#define PSO_TBLDRV_RC2_IBT       219  ///< RC2[IBT]
#define PSO_TBLDRV_RC10_OPSPEED  220  ///< RC10[Opearting Speed]
#define PSO_TBLDRV_LRDIMM_IBT    221  ///< LrDIMM IBT
#define PSO_TBLDRV_2D_TRAINING   222  ///< 2D training
#define PSO_TBLDRV_INVALID_TYPE  223  ///< Invalid Type
#define PSO_TBLDRV_END           223  ///< End of Table Driven Overriding Entry Types

/*----------------------------------------------------------------------------------------
*                 CONDITIONAL OVERRIDE TABLE MACROS
*----------------------------------------------------------------------------------------
*/
#define CPU_FAMILY_TO_OVERRIDE(CpuFamilyRevision) \
  PSO_CPU_FAMILY_TO_OVERRIDE, 4, \
  ((CpuFamilyRevision) & 0x0FF), (((CpuFamilyRevision) >> 8)& 0x0FF), (((CpuFamilyRevision) >> 16)& 0x0FF), (((CpuFamilyRevision) >> 24)& 0x0FF)

#define MEMCLK_DIS_MAP(SocketID, ChannelID, Bit0Map, Bit1Map, Bit2Map, Bit3Map, Bit4Map, Bit5Map, Bit6Map, Bit7Map) \
  PSO_MEMCLK_DIS, 11, SocketID, ChannelID, ALL_DIMMS, Bit0Map, Bit1Map, Bit2Map, Bit3Map, Bit4Map, Bit5Map, Bit6Map \
  , Bit7Map

#define CKE_TRI_MAP(SocketID, ChannelID, Bit0Map, Bit1Map, Bit2Map, Bit3Map) \
  PSO_CKE_TRI, 7, SocketID, ChannelID, ALL_DIMMS, Bit0Map, Bit1Map, Bit2Map, Bit3Map

#define ODT_TRI_MAP(SocketID, ChannelID, Bit0Map, Bit1Map, Bit2Map, Bit3Map) \
  PSO_ODT_TRI, 7, SocketID, ChannelID, ALL_DIMMS, Bit0Map, Bit1Map, Bit2Map, Bit3Map

#define CS_TRI_MAP(SocketID, ChannelID, Bit0Map, Bit1Map, Bit2Map, Bit3Map, Bit4Map, Bit5Map, Bit6Map, Bit7Map) \
  PSO_CS_TRI, 11, SocketID, ChannelID, ALL_DIMMS, Bit0Map, Bit1Map, Bit2Map, Bit3Map, Bit4Map, Bit5Map, Bit6Map, Bit7Map

#define NUMBER_OF_DIMMS_SUPPORTED(SocketID, ChannelID, NumberOfDimmSlotsPerChannel) \
  PSO_MAX_DIMMS, 4, SocketID, ChannelID, ALL_DIMMS, NumberOfDimmSlotsPerChannel

#define NUMBER_OF_SOLDERED_DOWN_DIMMS_SUPPORTED(SocketID, ChannelID, NumberOfSolderedDownDimmsPerChannel) \
  PSO_MAX_SOLDERED_DOWN_DIMMS, 4, SocketID, ChannelID, ALL_DIMMS, NumberOfSolderedDownDimmsPerChannel

#define NUMBER_OF_CHIP_SELECTS_SUPPORTED(SocketID, ChannelID, NumberOfChipSelectsPerChannel) \
  PSO_MAX_CHIPSELS, 4, SocketID, ChannelID, ALL_DIMMS, NumberOfChipSelectsPerChannel

#define NUMBER_OF_CHANNELS_SUPPORTED(SocketID, NumberOfChannelsPerSocket) \
  PSO_MAX_CHNLS, 4, SocketID, ANY_CHANNEL, ALL_DIMMS, NumberOfChannelsPerSocket

#define OVERRIDE_DDR_BUS_SPEED(SocketID, ChannelID, TimingMode, BusSpeed) \
  PSO_BUS_SPEED, 11, SocketID, ChannelID, ALL_DIMMS, TimingMode, (TimingMode >> 8), (TimingMode >> 16), (TimingMode >> 24), \
  BusSpeed, (BusSpeed >> 8), (BusSpeed >> 16), (BusSpeed >> 24)

#define DRAM_TECHNOLOGY(SocketID, MemTechType) \
  PSO_MEM_TECH, 7, SocketID, ANY_CHANNEL, ALL_DIMMS, MemTechType, (MemTechType >> 8), (MemTechType >> 16), (MemTechType >> 24)

#define WRITE_LEVELING_SEED(SocketID, ChannelID, DimmID, Byte0Seed, Byte1Seed, Byte2Seed, Byte3Seed, Byte4Seed, Byte5Seed, \
                            Byte6Seed, Byte7Seed, ByteEccSeed) \
  PSO_WL_SEED, 12, SocketID, ChannelID, DimmID, Byte0Seed, Byte1Seed, Byte2Seed, Byte3Seed, Byte4Seed, Byte5Seed, \
  Byte6Seed, Byte7Seed, ByteEccSeed

#define HW_RXEN_SEED(SocketID, ChannelID, DimmID, Byte0Seed, Byte1Seed, Byte2Seed, Byte3Seed, Byte4Seed, Byte5Seed, \
                     Byte6Seed, Byte7Seed, ByteEccSeed) \
  PSO_RXEN_SEED, 21, SocketID, ChannelID, DimmID, Byte0Seed, (Byte0Seed >> 8), Byte1Seed, (Byte1Seed >> 8), Byte2Seed, (Byte2Seed >> 8), \
  Byte3Seed, (Byte3Seed >> 8), Byte4Seed, (Byte4Seed >> 8), Byte5Seed, (Byte5Seed >> 8), Byte6Seed, (Byte6Seed >> 8), \
  Byte7Seed, (Byte7Seed >> 8), ByteEccSeed, (ByteEccSeed >> 8)

#define NO_LRDIMM_CS67_ROUTING(SocketID, ChannelID) \
  PSO_NO_LRDIMM_CS67_ROUTING, 4, SocketID, ChannelID, ALL_DIMMS, TRUE

#define SOLDERED_DOWN_SODIMM_TYPE(SocketID, ChannelID) \
  PSO_SOLDERED_DOWN_SODIMM_TYPE, 4, SocketID, ChannelID, ALL_DIMMS, TRUE

#define LVDIMM_FORCE_VOLT1_5_FOR_D0 \
  PSO_LVDIMM_VOLT1_5_SUPPORT, 4, ANY_SOCKET, ANY_CHANNEL, ALL_DIMMS, TRUE

#define MIN_RD_WR_DATAEYE_WIDTH(SocketID, ChannelID, MinRdDataeyeWidth, MinWrDataeyeWidth) \
  PSO_MIN_RD_WR_DATAEYE_WIDTH, 5, SocketID, ChannelID, ALL_DIMMS, MinRdDataeyeWidth, MinWrDataeyeWidth

#define MEMORY_POWER_POLICY_OVERRIDE(PowerPolicy) \
  PSO_MEMORY_POWER_POLICY, 4, ANY_SOCKET, ANY_CHANNEL, ALL_DIMMS, PowerPolicy

#define MOTHER_BOARD_LAYERS(Layers) \
  PSO_MOTHER_BOARD_LAYERS, 4, ANY_SOCKET, ANY_CHANNEL, ALL_DIMMS, Layers

#define MAX_NUMBER_PSO_TABLES  13

/*----------------------------------------------------------------------------------------
*                 CONDITIONAL OVERRIDE TABLE MACROS
*----------------------------------------------------------------------------------------
*/
#define CONDITION_AND \
  PSO_CONDITION_AND, 0

#define COND_LOC(SocketMsk, ChannelMsk, DimmMsk) \
  PSO_CONDITION_LOC, 3, SocketMsk, ChannelMsk, DimmMsk

#define COND_SPD(Byte, Mask, Value) \
  PSO_CONDITION_SPD, 3, Byte, Mask, Value

#define COND_REG(Access, Offset, Mask, Value) \
  PSO_CONDITION_REG, 11, Access, (Offset & 0x0FF), (Offset >> 8), \
  ((Mask) & 0x0FF), (((Mask) >> 8) & 0x0FF), (((Mask) >> 16) & 0x0FF), (((Mask) >> 24) & 0x0FF), \
  ((Value) & 0x0FF), (((Value) >> 8) & 0x0FF), (((Value) >> 16) & 0x0FF), (((Value) >> 24) & 0x0FF)

#define ACTION_ODT(Frequency, Dimms, QrDimms, DramOdt, QrDramOdt, DramDynOdt) \
  PSO_ACTION_ODT, 9, \
  ((Frequency) & 0x0FF), (((Frequency) >> 8)& 0x0FF), (((Frequency) >> 16)& 0x0FF), ((Frequency >> 24)& 0x0FF), \
  Dimms, QrDimms, DramOdt, QrDramOdt, DramDynOdt

#define ACTION_ADDRTMG(Frequency, DimmConfig, AddrTmg) \
  PSO_ACTION_ADDRTMG, 10, \
  ((Frequency) & 0x0FF), (((Frequency) >> 8)& 0x0FF), (((Frequency) >> 16)& 0x0FF), (((Frequency) >> 24)& 0x0FF), \
  ((DimmConfig) & 0x0FF), (((DimmConfig) >> 8) & 0x0FF), \
  (AddrTmg & 0x0FF), ((AddrTmg >> 8)& 0x0FF), ((AddrTmg >> 16)& 0x0FF), ((AddrTmg >> 24)& 0x0FF)

#define ACTION_ODCCTRL(Frequency, DimmConfig, OdcCtrl) \
  PSO_ACTION_ODCCONTROL, 10, \
  ((Frequency) & 0x0FF), (((Frequency) >> 8)& 0x0FF), (((Frequency) >> 16)& 0x0FF), (((Frequency) >> 24)& 0x0FF), \
  ((DimmConfig) & 0x0FF), (((DimmConfig) >> 8) & 0x0FF), \
  (OdcCtrl & 0x0FF), ((OdcCtrl >> 8)& 0x0FF), ((OdcCtrl >> 16)& 0x0FF), ((OdcCtrl >> 24)& 0x0FF)

#define ACTION_SLEWRATE(Frequency, DimmConfig, SlewRate) \
  PSO_ACTION_SLEWRATE, 10, \
  ((Frequency) & 0x0FF), (((Frequency) >> 8)& 0x0FF), (((Frequency) >> 16)& 0x0FF), (((Frequency) >> 24)& 0x0FF), \
  ((DimmConfig) & 0x0FF), (((DimmConfig) >> 8) & 0x0FF), \
  (SlewRate & 0x0FF), ((SlewRate >> 8)& 0x0FF), ((SlewRate >> 16)& 0x0FF), ((SlewRate >> 24)& 0x0FF)

#define ACTION_SPEEDLIMIT(DimmConfig, Dimms, SpeedLimit15, SpeedLimit135, SpeedLimit125) \
  PSO_ACTION_SPEEDLIMIT, 9, \
  ((DimmConfig) & 0x0FF), (((DimmConfig) >> 8) & 0x0FF), Dimms, \
  (SpeedLimit15 & 0x0FF), ((SpeedLimit15 >> 8)& 0x0FF), \
  (SpeedLimit135 & 0x0FF), ((SpeedLimit135 >> 8)& 0x0FF), \
  (SpeedLimit125 & 0x0FF), ((SpeedLimit125 >> 8)& 0x0FF)

#define MEMCLK_DIS_MAP_SIZE                           13
#define CKE_TRI_MAP_SIZE                              9
#define ODT_TRI_MAP_SIZE                              9
#define CS_TRI_MAP_SIZE                               13
#define NUMBER_OF_DIMMS_SUPPORTED_SIZE                6
#define NUMBER_OF_SOLDERED_DOWN_DIMMS_SUPPORTED_SIZE  6
#define NUMBER_OF_CHIP_SELECTS_SUPPORTED_SIZE         6
#define NUMBER_OF_CHANNELS_SUPPORTED_SIZE             6
#define OVERRIDE_DDR_BUS_SPEED_SIZE                   13
#define DRAM_TECHNOLOGY_SIZE                          9
#define SOLDERED_DOWN_SODIMM_TYPE_SIZE                6
#define MEMORY_POWER_POLICY_OVERRIDE_SIZE             6
#define MOTHER_BOARD_LAYERS_SIZE                      6

/// End of PsMemoryConfigurations

/// Mp.h

/// Type of an entry for Dram Term table
typedef struct {
  _4BYTE_ALIGN UINT32    Speed; ///< BitMap for the supported speed
  _1BYTE_ALIGN UINT8     Dimms; ///< BitMap for supported number of dimm
  _1BYTE_ALIGN UINT8     QR_Dimms; ///< BitMap for supported number of QR dimm
  _1BYTE_ALIGN UINT8     DramTerm; ///< DramTerm value
  _1BYTE_ALIGN UINT8     QR_DramTerm; ///< DramTerm value for QR
  _1BYTE_ALIGN UINT8     DynamicDramTerm; ///< Dynamic DramTerm
} DRAM_TERM_ENTRY;

/// Type of an entry for POR speed limit table
typedef struct {
  _2BYTE_ALIGN UINT16    DIMMRankType; ///< Bitmap of Ranks
  _1BYTE_ALIGN UINT8     Dimms;        ///< Number of dimm
  _2BYTE_ALIGN UINT16    SpeedLimit_1_5V; ///< POR speed limit for 1.5V
  _2BYTE_ALIGN UINT16    SpeedLimit_1_35V; ///< POR speed limit for 1.35V
  _2BYTE_ALIGN UINT16    SpeedLimit_1_25V; ///< POR speed limit for 1.25V
} POR_SPEED_LIMIT;

/// UDIMM&RDIMM Max. Frequency
typedef union {
  struct {
    ///< PSCFG_MAXFREQ_ENTRY
    _1BYTE_ALIGN UINT8     DimmPerCh;       ///< Dimm slot per chanel
    _2BYTE_ALIGN UINT16    Dimms;           ///< Number of Dimms on a channel
    _2BYTE_ALIGN UINT16    SR;              ///< Number of single-rank Dimm
    _2BYTE_ALIGN UINT16    DR;              ///< Number of dual-rank Dimm
    _2BYTE_ALIGN UINT16    QR;              ///< Number of quad-rank Dimm
    _2BYTE_ALIGN UINT16    Speed1_5V;       ///< Speed limit with voltage 1.5V
    _2BYTE_ALIGN UINT16    Speed1_35V;      ///< Speed limit with voltage 1.35V
    _2BYTE_ALIGN UINT16    Speed1_25V;      ///< Speed limit with voltage 1.25V
  } _MAXFREQ_ENTRY;
  struct {
    _1BYTE_ALIGN UINT8     DimmSlotPerCh;
    _2BYTE_ALIGN UINT16    CDN;               ///< Condition
    _2BYTE_ALIGN UINT16    CDN1;              ///< Condition
    _2BYTE_ALIGN UINT16    CDN2;              ///< Condition
    _2BYTE_ALIGN UINT16    CDN3;              ///< Condition
    _2BYTE_ALIGN UINT16    Speed[3];          ///< Speed limit
  } MAXFREQ_ENTRY;
} PSCFG_MAXFREQ_ENTRY;

/// LRDIMM Max. Frequency
typedef union {
  struct {
    ///< PSCFG_LR_MAXFREQ_ENTRY
    _1BYTE_ALIGN UINT8     DimmPerCh;      ///< Dimm slot per chanel
    _2BYTE_ALIGN UINT16    Dimms;          ///< Number of Dimms on a channel
    _2BYTE_ALIGN UINT16    LR;             ///< Number of LR-DIMM
    _2BYTE_ALIGN UINT16    Speed1_5V;      ///< Speed limit with voltage 1.5V
    _2BYTE_ALIGN UINT16    Speed1_35V;     ///< Speed limit with voltage 1.35V
    _2BYTE_ALIGN UINT16    Speed1_25V;     ///< Speed limit with voltage 1.25V
  } _LR_MAXFREQ_ENTRY;
  struct {
    _1BYTE_ALIGN UINT8     DimmSlotPerCh;
    _2BYTE_ALIGN UINT16    CDN;
    _2BYTE_ALIGN UINT16    CDN1;               ///< Condition
    _2BYTE_ALIGN UINT16    CDN2;               ///< Condition
    _2BYTE_ALIGN UINT16    CDN3;               ///< Condition
    _2BYTE_ALIGN UINT16    Speed[3];
  } LR_MAXFREQ_ENTRY;
} PSCFG_LR_MAXFREQ_ENTRY;

/// UDIMM&RDIMM RttNom and RttWr
typedef struct {
  _8BYTE_ALIGN UINT64    DimmPerCh;      ///< Dimm slot per chanel
  _8BYTE_ALIGN UINT64    DDRrate;        ///< Bitmap of DDR rate
  _8BYTE_ALIGN UINT64    VDDIO;          ///< Bitmap of VDDIO
  _8BYTE_ALIGN UINT64    Dimm0;          ///< Bitmap of rank type of Dimm0
  _8BYTE_ALIGN UINT64    Dimm1;          ///< Bitmap of rank type of Dimm1
  _8BYTE_ALIGN UINT64    Dimm2;          ///< Bitmap of rank type of Dimm2
  _8BYTE_ALIGN UINT64    Dimm;           ///< Bitmap of rank type of Dimm
  _8BYTE_ALIGN UINT64    Rank;           ///< Bitmap of rank
  _1BYTE_ALIGN UINT8     RttNom;         ///< Dram term
  _1BYTE_ALIGN UINT8     RttWr;          ///< Dynamic dram term
} PSCFG_RTT_ENTRY;

/// LRDIMM RttNom and RttWr
typedef struct {
  _8BYTE_ALIGN UINT64    DimmPerCh;      ///< Dimm slot per chanel
  _8BYTE_ALIGN UINT64    DDRrate;        ///< Bitmap of DDR rate
  _8BYTE_ALIGN UINT64    VDDIO;          ///< Bitmap of VDDIO
  _8BYTE_ALIGN UINT64    Dimm0;          ///< Dimm0 population
  _8BYTE_ALIGN UINT64    Dimm1;          ///< Dimm1 population
  _8BYTE_ALIGN UINT64    Dimm2;          ///< Dimm2 population
  _1BYTE_ALIGN UINT8     RttNom;         ///< Dram term
  _1BYTE_ALIGN UINT8     RttWr;          ///< Dynamic dram term
} PSCFG_LR_RTT_ENTRY;

/// UDIMM&RDIMM&LRDIMM ODT pattern OF 1 DPC
typedef struct {
  _4BYTE_ALIGN UINT32    Dimm0;           ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  _4BYTE_ALIGN UINT32    ODTPatCS0;       ///< ODT Pattern for CS0
  _4BYTE_ALIGN UINT32    ODTPatCS1;       ///< ODT Pattern for CS1
  _4BYTE_ALIGN UINT32    ODTPatCS2;       ///< ODT Pattern for CS2
  _4BYTE_ALIGN UINT32    ODTPatCS3;       ///< ODT Pattern for CS3
} PSCFG_1D_ODTPAT_ENTRY;

/// UDIMM&RDIMM&LRDIMM ODT pattern OF 2 DPC
typedef struct {
  UINT32    Dimm0 : 4;       ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  UINT32    Dimm1 : 28;      ///< Bitmap of dimm1 rank type or dimm1 population of LRDIMM
  UINT32    ODTPatCS0;       ///< ODT Pattern for CS0
  UINT32    ODTPatCS1;       ///< ODT Pattern for CS1
  UINT32    ODTPatCS2;       ///< ODT Pattern for CS2
  UINT32    ODTPatCS3;       ///< ODT Pattern for CS3
} PSCFG_2D_ODTPAT_ENTRY;

/// UDIMM&RDIMM&LRDIMM ODT pattern OF 3 DPC
typedef struct {
  UINT32    Dimm0 : 4;       ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  UINT32    Dimm1 : 4;       ///< Bitmap of dimm1 rank type or dimm1 population of LRDIMM
  UINT32    Dimm2 : 24;      ///< Bitmap of dimm2 rank type or dimm2 population of LRDIMM
  UINT32    ODTPatCS0;       ///< ODT Pattern for CS0
  UINT32    ODTPatCS1;       ///< ODT Pattern for CS1
  UINT32    ODTPatCS2;       ///< ODT Pattern for CS2
  UINT32    ODTPatCS3;       ///< ODT Pattern for CS3
} PSCFG_3D_ODTPAT_ENTRY;

/// UDIMM&RDIMM&LRDIMM SlowMode, AddrTmgCtl and ODC
typedef struct {
  _8BYTE_ALIGN UINT64    DimmPerCh;      ///< Dimm slot per channel
  _8BYTE_ALIGN UINT64    DDRrate;        ///< Bitmap of DDR rate
  _8BYTE_ALIGN UINT64    VDDIO;          ///< Bitmap of VDDIO
  _8BYTE_ALIGN UINT64    Dimm0;          ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  _8BYTE_ALIGN UINT64    Dimm1;          ///< Bitmap of dimm1 rank type or dimm1 population of LRDIMM
  _8BYTE_ALIGN UINT64    Dimm2;          ///< Bitmap of dimm2 rank type or dimm2 population of LRDIMM
  _8BYTE_ALIGN UINT64    SlowMode;       ///< SlowMode
  _4BYTE_ALIGN UINT32    AddTmgCtl;      ///< AddTmgCtl
  _4BYTE_ALIGN UINT32    ODC;            ///< ODC
  _1BYTE_ALIGN UINT8     POdtOff;        ///< POdtOff
} PSCFG_SAO_ENTRY;

/// UDIMM&RDIMM&LRDIMM 2D training config entry
typedef struct {
  _8BYTE_ALIGN UINT64    DimmPerCh;      ///< Dimm per channel
  _8BYTE_ALIGN UINT64    DDRrate;        ///< Bitmap of DDR rate
  _8BYTE_ALIGN UINT64    VDDIO;          ///< Bitmap of VDDIO
  _8BYTE_ALIGN UINT64    Dimm0;          ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  _8BYTE_ALIGN UINT64    Dimm1;          ///< Bitmap of dimm1 rank type or dimm1 population of LRDIMM
  _8BYTE_ALIGN UINT64    Dimm2;          ///< Bitmap of dimm2 rank type or dimm2 population of LRDIMM
  _8BYTE_ALIGN UINT64    Enable2D;       ///< SlowMode
} PSCFG_S2D_ENTRY;

/// UDIMM&RDIMM MR0[WR]
typedef struct {
  _1BYTE_ALIGN UINT8    Timing;             ///< Fn2_22C_dct[1:0][Twr]
  _1BYTE_ALIGN UINT8    Value;              ///< MR0[WR] : bit0 - bit2 available
} PSCFG_MR0WR_ENTRY;

/// UDIMM&RDIMM MR0[CL]
typedef struct {
  _1BYTE_ALIGN UINT8    Timing;           ///< Fn2_200_dct[1:0][Tcl]
  _1BYTE_ALIGN UINT8    Value;            ///< MR0[CL] : bit0 - bit2 CL[3:1]
  _1BYTE_ALIGN UINT8    Value1;           ///< MR0[CL] : bit3 CL[0]
} PSCFG_MR0CL_ENTRY;

/// UDIMM&RDIMM MR2[IBT]
typedef struct {
  _8BYTE_ALIGN UINT64    DimmPerCh;      ///< Dimm per channel
  _8BYTE_ALIGN UINT64    DDRrate;        ///< Bitmap of DDR rate
  _8BYTE_ALIGN UINT64    VDDIO;          ///< Bitmap of VDDIO
  _8BYTE_ALIGN UINT64    Dimm0;          ///< Bitmap of dimm0 rank type
  _8BYTE_ALIGN UINT64    Dimm1;          ///< Bitmap of dimm1 rank type
  _8BYTE_ALIGN UINT64    Dimm2;          ///< Bitmap of dimm2 rank type
  _8BYTE_ALIGN UINT64    Dimm;           ///< Bitmap of rank type of Dimm
  _8BYTE_ALIGN UINT64    NumOfReg;       ///< Number of registers
  _8BYTE_ALIGN UINT64    IBT;            ///< MR2[IBT] value
} PSCFG_MR2IBT_ENTRY;

/// UDIMM&RDIMM&LRDIMM Operating Speed
typedef struct {
  _4BYTE_ALIGN UINT32    DDRrate;           ///< Bitmap of DDR rate
  _1BYTE_ALIGN UINT8     OPSPD;             ///< RC10[OperatingSpeed]
} PSCFG_OPSPD_ENTRY;

/// LRDIMM IBT
typedef struct {
  _8BYTE_ALIGN UINT64    DimmPerCh;      ///< Dimm per channel
  _8BYTE_ALIGN UINT64    DDRrate;        ///< Bitmap of DDR rate
  _8BYTE_ALIGN UINT64    VDDIO;          ///< Bitmap of VDDIO
  _8BYTE_ALIGN UINT64    Dimm0;          ///< Dimm0 population
  _8BYTE_ALIGN UINT64    Dimm1;          ///< Dimm1 population
  _8BYTE_ALIGN UINT64    Dimm2;          ///< Dimm2 population
  _8BYTE_ALIGN UINT64    F0RC8;          ///< F0RC8
  _8BYTE_ALIGN UINT64    F1RC0;          ///< F1RC0
  _8BYTE_ALIGN UINT64    F1RC1;          ///< F1RC1
  _8BYTE_ALIGN UINT64    F1RC2;          ///< F1RC2
} PSCFG_L_IBT_ENTRY;

/// LRDIMM F0RC13[NumPhysicalRanks]
typedef struct {
  _1BYTE_ALIGN UINT8    NumRanks    : 3;      ///< NumRanks
  _1BYTE_ALIGN UINT8    NumPhyRanks : 5;      ///< NumPhyRanks
} PSCFG_L_NPR_ENTRY;

/// LRDIMM F0RC13[NumLogicalRanks]
typedef struct {
  _2BYTE_ALIGN UINT16    NumPhyRanks;     ///< NumPhyRanks
  _2BYTE_ALIGN UINT16    DramCap;         ///< DramCap
  _2BYTE_ALIGN UINT16    NumDimmSlot;     ///< NumDimmSlot
  _1BYTE_ALIGN UINT8     NumLogRanks;     ///< NumLogRanks
} PSCFG_L_NLR_ENTRY;

/// UDIMM&RDIMM&LRDIMM pass1 seed entry
typedef struct {
  _1BYTE_ALIGN UINT8     DimmPerCh;         ///< Dimm per channel
  _1BYTE_ALIGN UINT8     Channel;           ///< Channel#
  _2BYTE_ALIGN UINT16    SeedVal;           ///< Seed value
} PSCFG_SEED_ENTRY;

/// Platform specific configuration types
typedef enum {
  PSCFG_MAXFREQ,               ///< PSCFG_MAXFREQ
  PSCFG_LR_MAXFREQ,            ///< PSCFG_LR_MAXFREQ
  PSCFG_RTT,                   ///< PSCFG_RTT
  PSCFG_LR_RTT,                ///< PSCFG_LR_RTT
  PSCFG_ODT_PAT_1D,            ///< PSCFG_ODT_PAT_1D
  PSCFG_ODT_PAT_2D,            ///< PSCFG_ODT_PAT_2D
  PSCFG_ODT_PAT_3D,            ///< PSCFG_ODT_PAT_3D
  PSCFG_LR_ODT_PAT_1D,         ///< PSCFG_LR_ODT_PAT_1D
  PSCFG_LR_ODT_PAT_2D,         ///< PSCFG_LR_ODT_PAT_2D
  PSCFG_LR_ODT_PAT_3D,         ///< PSCFG_LR_ODT_PAT_3D
  PSCFG_SAO,                   ///< PSCFG_SAO
  PSCFG_LR_SAO,                ///< PSCFG_LR_SAO
  PSCFG_MR0WR,                 ///< PSCFG_MR0WR
  PSCFG_MR0CL,                 ///< PSCFG_MR0CL
  PSCFG_RC2IBT,                ///< PSCFG_RC2IBT
  PSCFG_RC10OPSPD,             ///< PSCFG_RC10OPSPD
  PSCFG_LR_IBT,                ///< PSCFG_LR_IBT
  PSCFG_LR_NPR,                ///< PSCFG_LR_NPR
  PSCFG_LR_NLR,                ///< PSCFG_LR_NLR
  PSCFG_S2D,                   ///< PSCFG_S2D
  PSCFG_WL_PASS1_SEED,         ///< PSCFG_WL_PASS1_SEED
  PSCFG_HWRXEN_PASS1_SEED,     ///< PSCFG_HWRXEN_SEED
  PSCFG_CADBUS,                ///< PSCFG_CADBUS
  PSCFG_CADBUS_DRAMDN,         ///< PSCFG_CADBUS_DRAMDN
  PSCFG_DATABUS,               ///< PSCFG_DATABUS
  PSCFG_DATABUS_DRAMDN,        ///< PSCFG_DATABUS_DRAMDN

  // The type of general table entries could be added between
  // PSCFG_GEN_START and PSCFG_GEN_END so that the PSCGen routine
  // is able to look for the entries per the PSCType.
  PSCFG_GEN_START,             ///< PSCFG_GEN_START
  PSCFG_CLKDIS,                ///< PSCFG_CLKDIS
  PSCFG_CKETRI,                ///< PSCFG_CKETRI
  PSCFG_ODTTRI,                ///< PSCFG_ODTTRI
  PSCFG_CSTRI,                 ///< PSCFG_CSTRI
  PSCFG_GEN_END                ///< PSCFG_GEN_END
} PSCFG_TYPE;

/// Dimm types
typedef enum {
  UDIMM_TYPE        = 0x01,    ///< UDIMM_TYPE
  RDIMM_TYPE        = 0x02,    ///< RDIMM_TYPE
  SODIMM_TYPE       = 0x04,    ///< SODIMM_TYPE
  LRDIMM_TYPE       = 0x08,    ///< LRDIMM_TYPE
  SODWN_SODIMM_TYPE = 0x10,    ///< SODWN_SODIMM_TYPE
  DT_DONT_CARE      = 0xFF     ///< DT_DONT_CARE
} DIMM_TYPE;

/// Number of DRAM devices or DIMM slots
typedef enum {
  _1DIMM        = 0x01,        ///< _1DIMM
  _2DIMM        = 0x02,        ///< _2DIMM
  _3DIMM        = 0x04,        ///< _3DIMM
  _4DIMM        = 0x08,        ///< _4DIMM
  _DIMM_NONE    = 0xF0,        ///< _DIMM_NONE (no DIMM slot)
  NOD_DONT_CARE = 0xFF         ///< NOD_DONT_CARE
} NOD_SUPPORTED;

/// Motherboard layer type
typedef enum {
  _4LAYERS      = 0x01,        ///< 4 Layers
  _6LAYERS      = 0x02,        ///< 6 Layers
  MBL_DONT_CARE = 0xFF         ///< MBL_DONT_CARE
} MB_LAYER_TYPE;

/// Motherboard power type
typedef enum {
  LPM           = 0x01,        ///< Low power motherboard
  HPM           = 0x02,        ///< High power motherboard
  MBP_DONT_CARE = 0xFF         ///< MBP_DONT_CARE
} MB_POWER_TYPE;

/// Table header related definitions
typedef struct {
  PSCFG_TYPE            PSCType;      ///< PSC Type
  DIMM_TYPE             DimmType;     ///< Dimm Type
  NOD_SUPPORTED         NumOfDimm;    ///< Numbef of dimm
  SOC_LOGICAL_ID        LogicalCpuid; ///< Logical Cpuid
  _1BYTE_ALIGN UINT8    PackageType;  ///< Package Type
  TECHNOLOGY_TYPE       TechType;     ///< Technology type
  MB_LAYER_TYPE         MotherboardLayer; ///< Motherboard layer type
  MB_POWER_TYPE         MotherboardPower; ///< Motherboard power type
} PSC_TBL_HEADER;

/// Table entry
typedef struct {
  PSC_TBL_HEADER        Header;    ///< PSC_TBL_HEADER
  _1BYTE_ALIGN UINT8    TableSize; ///< Table size
  VOID                  *TBLPtr;   ///< Pointer of the table
} PSC_TBL_ENTRY;

#define PT_DONT_CARE  0xFF
#define NP            1
#define V1_5          1
#define V1_35         2
#define V1_25         4
#define V1_2          1
#define V_TBD1        2
#define V_TBD2        4

#define VOLT_ALL     (7)
#define DIMM_SR      2
#define DIMM_DR      4
#define DIMM_QR      8
#define DIMM_LR      2
#define R0           1
#define R1           2
#define R2           4
#define R3           8
#define CH_A         0x01
#define CH_B         0x02
#define CH_C         0x04
#define CH_D         0x08
#define CH_ALL       0x0F
#define DEVWIDTH_16  1
#define DEVWIDTH_32  2

/// CAD Bus configuration
typedef struct {
  _4BYTE_ALIGN UINT32    DimmPerCh; ///< Bitmap of Dimm slot per chanel
  _4BYTE_ALIGN UINT32    DDRrate;   ///< Bitmap of DDR rate
  _4BYTE_ALIGN UINT32    VDDIO;     ///< Bitmap of VDDIO
  _4BYTE_ALIGN UINT32    Dimm0;     ///< Bitmap of rank type of Dimm0
  _4BYTE_ALIGN UINT32    Dimm1;     ///< Bitmap of rank type of Dimm1

  _4BYTE_ALIGN UINT16    GearDownMode; ///< GearDownMode
  _4BYTE_ALIGN UINT16    SlowMode;     ///< SlowMode
  _4BYTE_ALIGN UINT32    AddrCmdCtl;   ///< AddrCmdCtl

  _1BYTE_ALIGN UINT8     CkeStrength;   ///< CKE drive strength
  _1BYTE_ALIGN UINT8     CsOdtStrength; ///< CS ODT drive strength
  _1BYTE_ALIGN UINT8     AddrCmdStrength; ///< Addr Cmd drive strength
  _1BYTE_ALIGN UINT8     ClkStrength;  ///< CLK drive strength
} PSCFG_CADBUS_ENTRY;

/// CAD Bus configuration for Soldered Down DRAM
typedef struct {
  _4BYTE_ALIGN UINT32    DimmPerCh;  ///< Bitmap of Dimm slot per chanel
  _4BYTE_ALIGN UINT32    DDRrate;    ///< Bitmap of DDR rate
  _4BYTE_ALIGN UINT32    VDDIO;      ///< Bitmap of VDDIO
  _4BYTE_ALIGN UINT32    Dimm0;      ///< Bitmap of rank type of Dimm0
  _4BYTE_ALIGN UINT32    DevWidth;   ///< Device Width

  _4BYTE_ALIGN UINT32    SlowMode;   ///< SlowMode
  _4BYTE_ALIGN UINT32    AddrCmdCtl; ///< AddrCmdCtl

  _1BYTE_ALIGN UINT8     CkeStrength;   ///< CKE drive strength
  _1BYTE_ALIGN UINT8     CsOdtStrength; ///< CS ODT drive strength
  _1BYTE_ALIGN UINT8     AddrCmdStrength; ///< Addr Cmd drive strength
  _1BYTE_ALIGN UINT8     ClkStrength;  ///< CLK drive strength
} PSCFG_DRAMDN_CADBUS_ENTRY;

/// On die termination encoding
typedef enum {
  ODT_OFF = 0,    ///< 0 On die termination disabled
  ODT_60  = 1,    ///< 1 60 ohms
  ODT_120 = 2,    ///< 2 120 ohms
  ODT_40  = 3,    ///< 3 40 ohms
  ODT_20  = 4,    ///< 4 20 ohms
  ODT_30  = 5,    ///< 5 30 ohms
} ODT_ENC;

/// DDR5 Rtt_Nom termination encoding
typedef enum {
  RTTNOM_OFF = 0, ///< 0 Rtt_Nom Disabled
  RTTNOM_60  = 1, ///< 1 60 ohms
  RTTNOM_120 = 2, ///< 2 120 ohms
  RTTNOM_40  = 3, ///< 3 40 ohms
  RTTNOM_240 = 4, ///< 4 240 ohms
  RTTNOM_48  = 5, ///< 5 48 ohms
  RTTNOM_80  = 6, ///< 5 80 ohms
  RTTNOM_34  = 7, ///< 5 34 ohms
} D4_RTT_NOM_ENC;

/// DDR5 Rtt_Wr termination encoding
typedef enum {
  RTTWR_OFF = 0, ///< 0 Rtt_Nom Disabled
  RTTWR_120 = 1, ///< 1 120 ohms
  RTTWR_240 = 2, ///< 2 24 ohms
  RTTWR_HIZ = 3, ///< 3 Hi-Z
  RTTWR_80  = 4, ///< 4 80 ohms
} D4_RTT_WR_ENC;

/// DDR5 Rtt_Park termination encoding
typedef enum {
  RTTPRK_OFF = 0, ///< 0 Rtt_Park Disabled
  RTTPRK_60  = 1, ///< 1 60 ohms
  RTTPRK_120 = 2, ///< 2 120 ohms
  RTTPRK_40  = 3, ///< 3 40 ohms
  RTTPRK_240 = 4, ///< 4 240 ohms
  RTTPRK_48  = 5, ///< 5 48 ohms
  RTTPRK_80  = 6, ///< 5 80 ohms
  RTTPRK_34  = 7, ///< 5 34 ohms
} D4_RTT_PRK_ENC;

/// DDR3 Data Bus configuration
typedef struct {
  _4BYTE_ALIGN UINT32    DimmPerCh; ///< Bitmap of Dimm slot per chanel
  _4BYTE_ALIGN UINT32    DDRrate;   ///< Bitmap of DDR rate
  _4BYTE_ALIGN UINT32    VDDIO;     ///< Bitmap of VDDIO
  _4BYTE_ALIGN UINT32    Dimm0;     ///< Bitmap of rank type of Dimm0
  _4BYTE_ALIGN UINT32    Dimm1;     ///< Bitmap of rank type of Dimm1

  _4BYTE_ALIGN UINT32    RttNom;     ///< Rtt_Nom
  _4BYTE_ALIGN UINT32    RttWr;      ///< Rtt_Wr
  _4BYTE_ALIGN UINT32    DqStrength; ///< Data drive strength
  _4BYTE_ALIGN UINT32    DqsStrength; ///< DQS drive strength
  _4BYTE_ALIGN UINT32    OdtStrength; ///< ODT drive strength
} PSCFG_DATABUS_ENTRY;

/// Data Bus configuration
typedef struct {
  _4BYTE_ALIGN UINT32    DimmPerCh; ///< Bitmap of Dimm slot per chanel
  _4BYTE_ALIGN UINT32    DDRrate;   ///< Bitmap of DDR rate
  _4BYTE_ALIGN UINT32    VDDIO;     ///< Bitmap of VDDIO
  _4BYTE_ALIGN UINT32    Dimm0;     ///< Bitmap of rank type of Dimm0
  _4BYTE_ALIGN UINT32    DevWidth;  ///< Device Width

  _4BYTE_ALIGN UINT32    RttNom;     ///< Rtt_Nom
  _4BYTE_ALIGN UINT32    RttWr;      ///< Rtt_Wr
  _4BYTE_ALIGN UINT32    DqStrength; ///< Data drive strength
  _4BYTE_ALIGN UINT32    DqsStrength; ///< DQS drive strength
  _4BYTE_ALIGN UINT32    OdtStrength; ///< ODT drive strength
} PSCFG_DRAMDN_DATABUS_ENTRY;

/// DDR5 Data Bus configuration
typedef struct {
  _4BYTE_ALIGN UINT8     DimmPerCh; ///< Bitmap of Dimm slot per chanel
  _4BYTE_ALIGN UINT32    DDRrate;   ///< Bitmap of DDR rate
  _4BYTE_ALIGN UINT32    VDDIO;     ///< Bitmap of VDDIO
  _4BYTE_ALIGN UINT32    Dimm0;     ///< Bitmap of rank type of Dimm0
  _4BYTE_ALIGN UINT32    Dimm1;     ///< Bitmap of rank type of Dimm1

  _4BYTE_ALIGN UINT32    RttNom;     ///< Rtt_Nom
  _4BYTE_ALIGN UINT32    RttWr;      ///< Rtt_Wr
  _4BYTE_ALIGN UINT32    RttPark;    ///< Rtt_Park
  _4BYTE_ALIGN UINT32    DqStrength; ///< Data drive strength
  _4BYTE_ALIGN UINT32    DqsStrength; ///< DQS drive strength
  _4BYTE_ALIGN UINT32    OdtStrength; ///< ODT drive strength
} PSCFG_DATABUS_ENTRY_D4;
// end of Mp.h
// End of IDS Stuff

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */
#define HPCB_HEADER_VERSION  0x0000002A     // HPCB Version

#define HPCB_MAX_SOCKETS_SUPPORTED   1 /// Max Sockets supproted by HPCB
#define HPCB_MAX_CHANNEL_PER_SOCKET  2
#define HPCB_MAX_DIMMS_PER_CHANNEL   2
#define HPCB_TOTAL_SYSTEM_DIMMS      (HPCB_MAX_SOCKETS_SUPPORTED * HPCB_MAX_CHANNEL_PER_SOCKET * HPCB_MAX_DIMMS_PER_CHANNEL)
#define HPCB_TYPE_DATA_START_SIGNATURE()  _16BYTE_ALIGN CHAR8 SigStartTypeData[16] = { '$', 'A', 'P', 'C', 'B', '_', 'T', 'Y', 'P', 'E', '_', 'S', 'T', 'A', 'R', 'T' }
#define HPCB_TYPE_DATA_END_SIGNATURE()    _4BYTE_ALIGN CHAR8 SigEndTypeData[16]    = { '$', 'A', 'P', 'C', 'B', '_', 'T', 'Y', 'P', 'E', '_', 'E', 'N', 'D', '_', '$' }

//
// Group Id and Type Id for Memory Specific Data
//

#define HPCB_GROUP_PSP     0x1701
#define HPCB_GROUP_CCX     0x1702
#define HPCB_GROUP_DF      0x1703
#define HPCB_GROUP_MEMORY  0x1704
#define HPCB_GROUP_GNB     0x1705
#define HPCB_GROUP_FCH     0x1706
#define HPCB_GROUP_CBS     0x1707

#define HPCB_ENABLED   1
#define HPCB_MANUAL    1
#define HPCB_AUTO_U8   0xFF
#define HPCB_AUTO_U16  0xFFFF
#define HPCB_AUTO_U32  0xFFFFFFFFul
#define HPCB_AUTO_U64  0xFFFFFFFFFFFFFFFFull

typedef enum {
  HPCB_PSP_TYPE_CONFIG_DEFAULT_PARAMETERS = 1,
  HPCB_PSP_TYPE_CONFIG_PARAMETERS,
  HPCB_CCX_TYPE_CONFIG_DEFAULT_PARAMETERS = 3,
  HPCB_CCX_TYPE_CONFIG_PARAMETERS,
  HPCB_DF_TYPE_CONFIG_DEFAULT_PARAMETERS = 5,
  HPCB_DF_TYPE_CONFIG_PARAMETERS,
  HPCB_MEM_TYPE_CONFIG_DEFAULT_PARAMETERS = 7,
  HPCB_MEM_TYPE_CONFIG_PARAMETERS,
  HPCB_GNB_TYPE_CONFIG_DEFAULT_PARAMETERS = 9,
  HPCB_GNB_TYPE_CONFIG_PARAMETERS,
  HPCB_FCH_TYPE_CONFIG_DEFAULT_PARAMETERS = 11,
  HPCB_FCH_TYPE_CONFIG_PARAMETERS,
  HPCB_TYPE_CBS_COMMON_PARAMETERS = 13,
  HPCB_TYPE_CBS_COMMON_DUMMY_PARAMETERS,
  HPCB_TYPE_CBS_DEBUG_PARAMETERS = 15,
  HPCB_TYPE_CBS_DEBUG_DUMMY_PARAMETERS,
  HPCB_FCH_ESPI_TYPE_CONFIG_PARAMETERS
} HPCB_PARAM_TYPE;


typedef struct _HPCB_FCH_ESPI_INIT_STRUCT {
  UINT8        GPIO_Iomux[7];             ///< eSPI Gpio Address
  UINT8        GPIO_Function[7];          ///< eSPI Gpio Function
  UINT8        IoRangeSize[4];            ///< eSPI IO range size
  UINT16       IoRangeBase[4];            ///< eSPI IO range base
  UINT16       MmioRangeSize[4];          ///< eSPI MMIO range size
  UINT32       MmioRangeBase[4];          ///< eSPI MMIO range base
  UINT32       IrqMask;                   ///< eSPI IRQ mask bitmap
  UINT32       IrqPolarity;               ///< eSPI IRQ polarity bitmap
  UINT8        ClockFreq;                 ///< eSPI operating clock frequency
  UINT8        IoMode;                    ///< eSPI IO mode
} HPCB_FCH_ESPI_INIT_STRUCT;

typedef enum {
  ESPI_CLOCKVAL_16MHZ,            ///< 16.6MHz
  ESPI_CLOCKVAL_33MHZ,            ///< 33MHz
  ESPI_CLOCKVAL_66MHZ,            ///< 66MHz
} ESPI_CLOCKVAL;

typedef enum {
  ESPI_IOMODEVAL_SINGLE,            ///< Single I/O
  ESPI_IOMODEVAL_DUAL,              ///< Dual I/O
  ESPI_IOMODEVAL_QUAD,              ///< Quad I/O
} ESPI_IOMODEVAL;

#define HPCB_MEM_TYPE_SPD_INFO              0x0030
#define HPCB_MEM_TYPE_DIMM_INFO_SMBUS       0x0031
#define HPCB_MEM_TYPE_DIMM_CONFIG_INFO_ID   0x0032
#define HPCB_MEM_TYPE_MEM_OVERCLOCK_CONFIG  0x0033

#define HPCB_MEM_TYPE_PSO_DATA  0x0040

#define HPCB_MEM_TYPE_PS_UDIMM_DDR5_ODT_PAT       0x0081
#define HPCB_MEM_TYPE_PS_UDIMM_DDR5_CAD_BUS       0x0082
#define HPCB_MEM_TYPE_PS_UDIMM_DDR5_DATA_BUS      0x0083
#define HPCB_MEM_TYPE_PS_UDIMM_DDR5_MAX_FREQ      0x0084
#define HPCB_MEM_TYPE_PS_UDIMM_DDR5_STRETCH_FREQ  0x0085

#define HPCB_MEM_TYPE_PS_RDIMM_DDR5_ODT_PAT       0x0086
#define HPCB_MEM_TYPE_PS_RDIMM_DDR5_CAD_BUS       0x0087
#define HPCB_MEM_TYPE_PS_RDIMM_DDR5_DATA_BUS      0x0088
#define HPCB_MEM_TYPE_PS_RDIMM_DDR5_MAX_FREQ      0x0089
#define HPCB_MEM_TYPE_PS_RDIMM_DDR5_STRETCH_FREQ  0x008A

#define HPCB_MEM_TYPE_CONSOLE_OUT_CONTROL      0x0050
#define HPCB_MEM_TYPE_EVENT_CONTROL            0x0051
#define HPCB_MEM_TYPE_ERROR_OUT_EVENT_CONTROL  0x0052
#define HPCB_MEM_TYPE_EXT_VOLTAGE_CONTROL      0x0053

#define HPCB_MEM_TYPE_PS_LRDIMM_DDR5_ODT_PAT       0x0094
#define HPCB_MEM_TYPE_PS_LRDIMM_DDR5_CAD_BUS       0x0095
#define HPCB_MEM_TYPE_PS_LRDIMM_DDR5_DATA_BUS      0x0096
#define HPCB_MEM_TYPE_PS_LRDIMM_DDR5_MAX_FREQ      0x0097
#define HPCB_MEM_TYPE_PS_LRDIMM_DDR5_STRETCH_FREQ  0x0098

#define HPCB_MEM_TYPE_PS_SODIMM_DDR5_ODT_PAT       0x0099
#define HPCB_MEM_TYPE_PS_SODIMM_DDR5_CAD_BUS       0x009A
#define HPCB_MEM_TYPE_PS_SODIMM_DDR5_DATA_BUS      0x009B
#define HPCB_MEM_TYPE_PS_SODIMM_DDR5_MAX_FREQ      0x009C
#define HPCB_MEM_TYPE_PS_SODIMM_DDR5_STRETCH_FREQ  0x009D

#define HPCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR        0x005E
#define HPCB_MEM_TYPE_PS_DRAMDOWN_DDR5_ODT_PAT       0x00A0
#define HPCB_MEM_TYPE_PS_DRAMDOWN_DDR5_CAD_BUS       0x00A1
#define HPCB_MEM_TYPE_PS_DRAMDOWN_DDR5_DATA_BUS      0x00A2
#define HPCB_MEM_TYPE_PS_DRAMDOWN_DDR5_MAX_FREQ      0x00A3
#define HPCB_MEM_TYPE_PS_DRAMDOWN_DDR5_STRETCH_FREQ  0x00A4

#define HPCB_PSP_TYPE_BOARD_ID_GETTING_METHOD  0x0060

/// CBS Items Start
#define BSU08(u8)   ((UINT8)(u8)  & 0xFF)
#define BSU16(u16)  ((UINT16)(u16) & 0xFF), (((UINT16)(u16) >> 8) & 0xFF)
#define BSU32(u32)  ((UINT32)(u32) & 0xFF), (((UINT32)(u32) >> 8) & 0xFF), (((UINT32)(u32) >> 16) & 0xFF), (((UINT32)(u32) >> 24) & 0xFF)
#define BSU64(u64)  ((UINT64)(u64) & 0xFF), (((UINT64)(u64) >> 8) & 0xFF), (((UINT64)(u64) >> 16) & 0xFF), (((UINT64)(u64) >> 24) & 0xFF), \
  (((UINT64)(u64) >> 32) & 0xFF), (((UINT64)(u64) >> 40) & 0xFF), (((UINT64)(u64) >> 48) & 0xFF), (((UINT64)(u64) >> 56) & 0xFF)
#define BSBLN(bl)   ((UINT8)(bl)  & 0xFF)

//
// DF Type ID
//
#define HPCB_DF_TYPE_XHMI_TX_EQ                        0x00D0
#define   HPCB_DF_TYPE_XHMI_INSTANCE_SPEED_COMBINED    0  // Instance ID 0
#define   HPCB_DF_TYPE_XHMI_INSTANCE_SPEED_COMBINED_1  1  // Instance ID 1

//
// The enumerator should always grow at the end of the list of each component
// Obsolete tokens should be preserved to maintain backward compatibility
//
typedef enum {
  HPCB_TOKEN_CONFIG_PSP_BEGIN = 0x0,
  HPCB_TOKEN_CONFIG_PSP_ENABLE_DEBUG_MODE,
  HPCB_TOKEN_CONFIG_PSP_END,
  HPCB_TOKEN_CONFIG_CCX_BEGIN = 0x100,
  HPCB_TOKEN_CONFIG_CCX_MIN_CSV_ASID,
  HPCB_TOKEN_CONFIG_CCX_END,
  HPCB_TOKEN_CONFIG_DF_BEGIN = 0x300,
  HPCB_TOKEN_CONFIG_DF_HMI_ENCRYPT,
  HPCB_TOKEN_CONFIG_DF_XHMI_ENCRYPT,
  HPCB_TOKEN_CONFIG_DF_SAVE_RESTORE_MEM_ENCRYPT,
  HPCB_TOKEN_CONFIG_DF_SYS_STORAGE_AT_TOP_OF_MEM,
  HPCB_TOKEN_CONFIG_DF_PROBE_FILTER_ENABLE,
  HPCB_TOKEN_CONFIG_DF_BOTTOMIO,
  HPCB_TOKEN_CONFIG_DF_MEM_INTERLEAVING,
  HPCB_TOKEN_CONFIG_DF_DRAM_INTERLEAVE_SIZE,
  HPCB_TOKEN_CONFIG_DF_ENABLE_CHAN_INTLV_HASH,
  HPCB_TOKEN_CONFIG_DF_PCI_MMIO_SIZE,
  HPCB_TOKEN_CONFIG_DF_CAKE_CRC_THRESH_PERF_BOUNDS,
  HPCB_TOKEN_CONFIG_DF_MEM_SCRUB_ENABLE,
  HPCB_TOKEN_CONFIG_DF_MEM_MIRROR_ENABLE,
  HPCB_TOKEN_CONFIG_DF_MEM_CLEAR_DISABLE,
  HPCB_TOKEN_CONFIG_DF_PCI_BUS_ASSIGN_NUMBER,
  HPCB_TOEKN_CONFIG_DF_EXT_PSP_PRIVATE_SPACE_SIZE,
  HPCB_TOEKN_CONFIG_DF_EXT_SMU_PRIVATE_SPACE_SIZE,
  HPCB_TOKEN_CONFIG_DF_RESERVE0,
  HPCB_TOKEN_CONFIG_DF_END,
  HPCB_TOKEN_CONFIG_MEM_BEGIN = 0x700,
  HPCB_TOKEN_CONFIG_BOTTOMIO,
  HPCB_TOKEN_CONFIG_MEMHOLEREMAPPING,
  HPCB_TOKEN_CONFIG_LIMITMEMORYTOBELOW1TB,
  HPCB_TOKEN_CONFIG_USERTIMINGMODE,
  HPCB_TOKEN_CONFIG_MEMCLOCKVALUE,
  HPCB_TOKEN_CONFIG_ENABLECHIPSELECTINTLV,
  HPCB_TOKEN_CONFIG_ENABLECHANNELINTLV,
  HPCB_TOKEN_CONFIG_ENABLEECCFEATURE,
  HPCB_TOKEN_CONFIG_ENABLEPOWERDOWN,
  HPCB_TOKEN_CONFIG_ENABLEPARITY,
  HPCB_TOKEN_CONFIG_ENABLEBANKSWIZZLE,
  HPCB_TOKEN_CONFIG_ENABLEMEMCLR,
  HPCB_TOKEN_CONFIG_UMAMODE,
  HPCB_TOKEN_CONFIG_UMASIZE,
  HPCB_TOKEN_CONFIG_MEMRESTORECTL,
  HPCB_TOKEN_CONFIG_SAVEMEMCONTEXTCTL,
  HPCB_TOKEN_CONFIG_ISCAPSULEMODE,
  HPCB_TOKEN_CONFIG_FORCETRAINMODE,
  HPCB_TOKEN_CONFIG_DIMMTYPEUSEDINMIXEDCONFIG,
  HPCB_TOKEN_CONFIG_AMPENABLE,
  HPCB_TOKEN_CONFIG_DRAMDOUBLEREFRESHRATE,
  HPCB_TOKEN_CONFIG_PMUTRAINMODE,
  HPCB_TOKEN_CONFIG_ECCREDIRECTION,
  HPCB_TOKEN_CONFIG_SCRUBDRAMRATE,
  HPCB_TOKEN_CONFIG_SCRUBL2RATE,
  HPCB_TOKEN_CONFIG_SCRUBL3RATE,
  HPCB_TOKEN_CONFIG_SCRUBICRATE,
  HPCB_TOKEN_CONFIG_SCRUBDCRATE,
  HPCB_TOKEN_CONFIG_ECCSYNCFLOOD,
  HPCB_TOKEN_CONFIG_ECCSYMBOLSIZE,
  HPCB_TOKEN_CONFIG_DQSTRAININGCONTROL,
  HPCB_TOKEN_CONFIG_UMAABOVE4G,
  HPCB_TOKEN_CONFIG_UMAALIGNMENT,
  HPCB_TOKEN_CONFIG_MEMORYALLCLOCKSON,
  HPCB_TOKEN_CONFIG_MEMORYBUSFREQUENCYLIMIT,
  HPCB_TOKEN_CONFIG_POWERDOWNMODE,
  HPCB_TOKEN_CONFIG_IGNORESPDCHECKSUM,
  HPCB_TOKEN_CONFIG_MEMORYMODEUNGANGED,
  HPCB_TOKEN_CONFIG_MEMORYQUADRANKCAPABLE,
  HPCB_TOKEN_CONFIG_MEMORYRDIMMCAPABLE,
  HPCB_TOKEN_CONFIG_MEMORYLRDIMMCAPABLE,
  HPCB_TOKEN_CONFIG_MEMORYUDIMMCAPABLE,
  HPCB_TOKEN_CONFIG_MEMORYSODIMMCAPABLE,
  HPCB_TOKEN_CONFIG_DRAMDOUBLEREFRESHRATEEN,
  HPCB_TOKEN_CONFIG_DIMMTYPEDDDR5CAPABLE,
  HPCB_TOKEN_CONFIG_DIMMTYPEDDDR4CAPABLE,
  HPCB_TOKEN_CONFIG_ENABLEZQRESET,
  HPCB_TOKEN_CONFIG_ENABLEBANKGROUPSWAP,
  HPCB_TOKEN_CONFIG_ODTSCMDTHROTEN,
  HPCB_TOKEN_CONFIG_SWCMDTHROTEN,
  HPCB_TOKEN_CONFIG_FORCEPWRDOWNTHROTEN,
  HPCB_TOKEN_CONFIG_ODTSCMDTHROTCYC,
  HPCB_TOKEN_CONFIG_SWCMDTHROTCYC,
  HPCB_TOKEN_CONFIG_DIMMSENSORCONF,
  HPCB_TOKEN_CONFIG_DIMMSENSORUPPER,
  HPCB_TOKEN_CONFIG_DIMMSENSORLOWER,
  HPCB_TOKEN_CONFIG_DIMMSENSORCRITICAL,
  HPCB_TOKEN_CONFIG_DIMMSENSORRESOLUTION,
  HPCB_TOKEN_CONFIG_AUTOREFFINEGRANMODE,
  HPCB_TOKEN_CONFIG_ENABLEMEMPSTATE,
  HPCB_TOKEN_CONFIG_SOLDERDOWNDRAM,
  HPCB_TOKEN_CONFIG_DDRROUTEBALANCEDTEE,
  HPCB_TOKEN_CONFIG_MEM_MBIST_TEST_ENABLE,
  HPCB_TOKEN_CONFIG_MEM_MBIST_SUBTEST_TYPE,
  HPCB_TOKEN_CONFIG_MEM_MBIST_AGGRESOR_ON,
  HPCB_TOKEN_CONFIG_MEM_MBIST_HALT_ON_ERROR,
  HPCB_TOKEN_CONFIG_MEM_CPU_VREF_RANGE,
  HPCB_TOKEN_CONFIG_MEM_DRAM_VREF_RANGE,
  HPCB_TOKEN_CONFIG_MEM_TSME_ENABLE,
  HPCB_TOKEN_CONFIG_MEM_NVDIMM_POWER_SOURCE,
  HPCB_TOKEN_CONFIG_MEM_DATA_POISON,
  HPCB_TOKEN_CONFIG_MEM_DATA_SCRAMBLE,
  HPCB_TOKEN_CONFIG_MEM_PS_ERROR_HANDLING,
  HPCB_TOKEN_CONFIG_MEM_TEMP_CONTROLLED_REFRESH_EN,
  HPCB_TOKEN_CONFIG_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT,
  HPCB_TOKEN_CONFIG_MEM_1SPC_2SPC_MIX_MODE_EN,
  HPCB_TOKEN_CONFIG_MEM_END,
  HPCB_TOKEN_CONFIG_GNB_BEGIN = 0x1800,
  HPCB_TOKEN_CONFIG_BMC_SOCKET_NUMBER,
  HPCB_TOKEN_CONFIG_BMC_START_LANE,
  HPCB_TOKEN_CONFIG_BMC_END_LANE,
  HPCB_TOKEN_CONFIG_BMC_DEVICE,
  HPCB_TOKEN_CONFIG_BMC_FUNCTION,
  HPCB_TOKEN_CONFIG_GNB_GOP_PHY_RATE_CFG,
  HPCB_TOKEN_CONFIG_GNB_HMI_PHY_RATE_CFG,
  HPCB_TOKEN_CONFIG_GNB_END,
  HPCB_TOKEN_CONFIG_FCH_BEGIN = 0x1C00,
  HPCB_TOKEN_CONFIG_FCH_CONSOLE_OUT_ENABLE,
  HPCB_TOKEN_CONFIG_FCH_CONSOLE_OUT_SERIAL_PORT,
  HPCB_TOKEN_CONFIG_FCH_CONSOLE_OUT_DIE_TYPE,
  HPCB_TOKEN_CONFIG_FCH_CONSOLE_OUT_SOCKET_ID,
  HPCB_TOKEN_CONFIG_FCH_CONSOLE_OUT_IOD_ID,
  HPCB_TOKEN_CONFIG_FCH_CONSOLE_OUT_CDD_ID,
  HPCB_TOKEN_CONFIG_FCH_END,
  HPCB_TOKEN_CONFIG_LIMIT = 0x1FFF
} HPCB_PARAM_CONFIG_TOKEN;

// Note: Ensure that the token name matches the variable name in CBS xml file/
// HpcbAutoGen.h file in Resource folder in CBS package.
//
// CBS CMN HPCB Tokens
//
#define HPCB_TOKEN_CBS_CMN_BEGIN                               0x0000
#define HPCB_TOKEN_CBS_DF_CMN_HMI_ENCRYPTION                   0x0001
#define HPCB_TOKEN_CBS_DF_CMN_X_HMI_ENCRYPTION                 0x0002
#define HPCB_TOKEN_CBS_DF_CMN_CC6_MEM_ENCRYPTION               0x0003
#define HPCB_TOKEN_CBS_DF_CMN_CC6_ALLOCATION_SCHEME            0x0004
#define HPCB_TOKEN_CBS_DF_CMN_SYS_PROBE_FILTER                 0x0005
#define HPCB_TOKEN_CBS_DF_CMN_MEM_INTLV                        0x0006
#define HPCB_TOKEN_CBS_DF_CMN_MEM_INTLV_SIZE                   0x0007
#define HPCB_TOKEN_CBS_DF_CMN_CHNL_INTLV_HASH                  0x0008
#define HPCB_TOKEN_CBS_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR5    0x0009
#define HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR5    0x000A
#define HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_BANK_SWIZZLE_DDR5       0x000B
#define HPCB_TOKEN_CBS_CMN_MEM_ADDRESS_HASHING_DDR5            0x000C
#define HPCB_TOKEN_CBS_CMN_MEM_SPEED_DDR5                      0x000D
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TCL_DDR5                 0x000E
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRCDRD_DDR5              0x000F
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRCDWR_DDR5              0x0010
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRP_DDR5                 0x0011
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRAS_DDR5                0x0012
#define HPCB_TOKEN_CBS_CMN_MEM_DATA_POISONING_DDR5             0x0013
#define HPCB_TOKEN_CBS_CMN_MEM_ADDRESS_HASH_BANK_DDR5          0x0014
#define HPCB_TOKEN_CBS_CMN_MEM_ADDRESS_HASH_CS_DDR5            0x0015
#define HPCB_TOKEN_CBS_CMN_MEM_TSME_DDR5                       0x0016
#define HPCB_TOKEN_CBS_CMN_MEM_OVERCLOCK_DDR5                  0x0017
#define HPCB_TOKEN_CBS_CMN_GNB_GFX_UMA_MODE                    0x0018
#define HPCB_TOKEN_CBS_CMN_GNB_GFX_UMA_VERSION                 0x0019
#define HPCB_TOKEN_CBS_CMN_GNB_GFX_DISPLAY_RESOLUTION          0x001A
#define HPCB_TOKEN_CBS_CMN_GNB_GFX_UMA_FRAME_BUFFER_SIZE       0x001B
#define HPCB_TOKEN_CBS_CMN_CPU_CSV_ASID_SPACE_LIMIT            0x001C
#define HPCB_TOKEN_CBS_CMN_MEM_SPD_READ_OPTIMIZATION_DDR5      0x001D
#define HPCB_TOKEN_CBS_CMN_MEM_MBIST_EN                        0x001E
#define HPCB_TOKEN_CBS_CMN_MEM_MBIST_SUB_TEST                  0x001F
#define HPCB_TOKEN_CBS_DF_CMN_MEM_CLEAR                        0x0020
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRC_CTRL_DDR5            0x0021
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRC_DDR5                 0x0022
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRRD_S_DDR5              0x0023
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRRD_L_DDR5              0x0024
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TFAW_CTRL_DDR5           0x0025
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TFAW_DDR5                0x0026
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TWTR_S_DDR5              0x0027
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TWTR_L_DDR5              0x0028
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TWR_CTRL_DDR5            0x0029
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TWR_DDR5                 0x002A
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRCPAGE_CTRL_DDR5        0x002B
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRCPAGE_DDR5             0x002C
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRDRD_SC_L_CTRL_DDR5     0x002D
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRDRD_SC_L_DDR5          0x002E
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TWRWR_SC_L_CTRL_DDR5     0x002F
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TWRWR_SC_L_DDR5          0x0030
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRFC_CTRL_DDR5           0x0031
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRFC_DDR5                0x0032
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRFC2_CTRL_DDR5          0x0033
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRFC2_DDR5               0x0034
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRFC4_CTRL_DDR5          0x0035
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRFC4_DDR5               0x0036
#define HPCB_TOKEN_CBS_CMN_MEM_OVERCLOCK_FAIL_CNT              0x0037
#define HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_PROC_ODT_DDR5           0x0038
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRDRD_SC_DDR5            0x0039
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRDRD_SD_DDR5            0x003A
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRDRD_DD_DDR5            0x003B
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TWRWR_SC_DDR5            0x003C
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TWRWR_SD_DDR5            0x003D
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TWRWR_DD_DDR5            0x003E
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRDWR_DDR5               0x003F
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TWRRD_DDR5               0x0040
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TRTP_DDR5                0x0041
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TCWL_DDR5                0x0042
#define HPCB_TOKEN_CBS_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR5        0x0043
#define HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_RTT_NOM_DDR5            0x0044
#define HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_RTT_WR_DDR5             0x0045
#define HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_RTT_PARK_DDR5           0x0046
#define HPCB_TOKEN_CBS_CMN_MEM_TIMING_TCKE_DDR5                0x0047
#define HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR5      0x0048
#define HPCB_TOKEN_CBS_CMN_CLDO_VDDP_CTL                       0x0049
#define HPCB_TOKEN_CBS_CMN_CLDOVDD_PVOLTAGE                    0x004A
#define HPCB_TOKEN_CBS_CMN_MEM_CTRLLER2_T_MODE_DDR5            0x004B
#define HPCB_TOKEN_CBS_CMN_MEM_GEAR_DOWN_MODE_DDR5             0x004C
#define HPCB_TOKEN_CBS_CMN_MEM_MBIST_AGGRESSORS                0x004D
#define HPCB_TOKEN_CBS_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT  0x004E
#define HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_PWR_DN_EN_DDR5          0x004F

#define HPCB_TOKEN_CBS_CMN_FCH_CONSOLE_OUT_DIE_TYPE            0x0050
#define HPCB_TOKEN_CBS_CMN_FCH_CONSOLE_OUT_IOD_ID              0x0051
#define HPCB_TOKEN_CBS_CMN_FCH_CONSOLE_OUT_CDD_ID              0x0052

// DF new added in Satori/Dhrama
#define HPCB_TOKEN_CBS_DF_CMN_MEM_MIRROR_ENABLE           0x0053
#define HPCB_TOKEN_CBS_DF_CMN_PCI_BUS_ASSIGN_NUMBER       0x0054
#define HPCB_TOEKN_CBS_DF_CMN_EXT_PSP_PRIVATE_SPACE_SIZE  0x0055
#define HPCB_TOEKN_CBS_DF_CMN_EXT_SMU_PRIVATE_SPACE_SIZE  0x0056

#define HPCB_TOKEN_CBS_CMN_FCH_CONSOLE_OUT_ENABLE         0x0057
#define HPCB_TOKEN_CBS_CMN_GNB_XHMI_PHY_RATE_CFG          0x0058
#define HPCB_TOKEN_CBS_CMN_GNB_HMI_PHY_RATE_CFG           0x0059
#define HPCB_TOKEN_CBS_CMN_SOCKET_NUMBER                  0x005A
#define HPCB_TOKEN_CBS_CMN_XHMI_CONNECT_TYPE              0x005B
#define HPCB_TOKEN_CBS_CMN_TPCM_CONTROL                   0x005C
#define HPCB_TOKEN_CBS_CMN_PSF_CONTROL                    0x005D
#define HPCB_TOKEN_CBS_CMN_CLDO_VDDP_IOD_HMI_VID          0x005E
#define HPCB_TOKEN_CBS_CMN_CLDO_VDDP_CDD_HMI_VID          0x005F
#define HPCB_TOKEN_CBS_CMN_CLDO_VDDP_CDD_DDR_VID          0x0060
#define HPCB_TOKEN_CBS_CMN_CLDO_VDDM_BYPASS_MODE          0x0061
#define HPCB_TOKEN_CBS_CMN_AVS_CTL                        0x0062
#define HPCB_TOKEN_CBS_CMN_CLDO_VDDM_CTL                  0x0063
#define HPCB_TOKEN_CBS_CMN_CLDOVDD_MVOLTAGE               0x0064
#define HPCB_TOKEN_CBS_CMN_POST_COMPLETE_PIN_CTRL         0x0065
#define HPCB_TOKEN_CBS_CMN_CONSOLE_OUT_SERIAL_PORT        0x0066
#define HPCB_TOKEN_CBS_CMN_PSP_LOG_CATEGORY               0x0067
#define HPCB_TOKEN_CBS_CMN_PSP_LOG_LEVEL                  0x0068
#define HPCB_TOKEN_CBS_CMN_MEM_EYE_TEST                   0x0069
#define HPCB_TOKEN_CBS_CMN_DJ_CLDO_VDDP_CTL               0x006A
#define HPCB_TOKEN_CBS_CMN_DJ_CLDOVDD_PVOLTAGE            0x006B
#define HPCB_TOKEN_CBS_CMN_EM_CLDO_VDDP_CTL               0x006C
#define HPCB_TOKEN_CBS_CMN_EM_CLDOVDD_PVOLTAGE            0x006D
#define HPCB_TOKEN_CBS_CMN_CDD_CLDO_VDDP_CTL              0x006E
#define HPCB_TOKEN_CBS_CMN_CDD_CLDOVDD_PVOLTAGE           0x006F
#define HPCB_TOKEN_CBS_CMN_AUTOREFFINEGRANMODE            0x0070
#define HPCB_TOKEN_CBS_CMN_QOS                            0x0071
#define HPCB_TOKEN_CBS_CMN_FCH_CONSOLE_OUT_SOCKET_ID      0x0072
#define HPCB_TOKEN_CBS_CMN_CLDO_VDDP_DJ_BYPASS_MODE       0x0073
#define HPCB_TOKEN_CBS_CMN_CLDO_VDDP_EM_BYPASS_MODE       0x0074
#define HPCB_TOKEN_CBS_CMN_CLDO_VDDP_CDD_BYPASS_MODE      0x0075
#define HPCB_TOKEN_CBS_CMN_SUBCHNL_INTLV_BIT              0x0076
#define HPCB_TOKEN_CBS_CMN_MCA_BANK_CTL                   0x0077
#define HPCB_TOKEN_CBS_CMN_FCLK_UCLK_SYNC_MODE            0x0078
#define HPCB_TOKEN_CBS_CMN_BIOS_SPI_SPEED_MODE            0x0079

#define HPCB_TOKEN_CBS_CMN_END                            0x0080
#define HPCB_TOKEN_CBS_CMN_LIMIT                          0x1FFF

//
// CBS DBG HPCB Tokens
//
#define HPCB_TOKEN_CBS_DBG_BEGIN                                           0x0000
#define HPCB_TOKEN_CBS_DF_DBG_CORE_MSTR_REQ_ORDERING                       0x0001
#define HPCB_TOKEN_CBS_DF_DBG_IO_MSTR_REQ_ORDERING                         0x0002
#define HPCB_TOKEN_CBS_DF_DBG_MEM_CANCEL_DIS                               0x0003
#define HPCB_TOKEN_CBS_DF_DBG_MIG_PRB_DIS                                  0x0004
#define HPCB_TOKEN_CBS_DF_DBG_STOP_ON_SPF_ERROR                            0x0005
#define HPCB_TOKEN_CBS_DF_DBG_TCDX_BYPASS_DIS                              0x0006
#define HPCB_TOKEN_CBS_DF_DBG_F1F_FETCH_PRB_RMT                            0x0007
#define HPCB_TOKEN_CBS_DF_DBG_F1F_FETCH_PRB_LCL                            0x0008
#define HPCB_TOKEN_CBS_DF_DBG_F1F_FETCH_PRB_TARGET                         0x0009
#define HPCB_TOKEN_CBS_DF_DBG_SPF_WAY_DIS                                  0x000A
#define HPCB_TOKEN_CBS_DF_DBG_IO_REQ_BUS_LOCK_CTRL                         0x000B
#define HPCB_TOKEN_CBS_DF_DBG_IOM_PERF_MON_PRB_RSP                         0x000C
#define HPCB_TOKEN_CBS_DF_DBG_IOM_PERF_MON_TGT_SEL                         0x000D
#define HPCB_TOKEN_CBS_DF_DBG_WDT_CTRL                                     0x000E
#define HPCB_TOKEN_CBS_DF_DBG_CAKE_REQ_ADDR_COMPRESSION                    0x000F
#define HPCB_TOKEN_CBS_DF_DBG_CAKE_DATA_COMPRESSION                        0x0010
#define HPCB_TOKEN_CBS_DF_DBG_CAKE_RSP_CMD_PACKING                         0x0011
#define HPCB_TOKEN_CBS_DF_DBG_CAKE_PRB_COMBINING                           0x0012
#define HPCB_TOKEN_CBS_DF_DBG_CAKE_IO_ADDR_COMP_BIT                        0x0013
#define HPCB_TOKEN_CBS_DF_DBG_IO_BUS_LOCK_INV_DIS                          0x0014
#define HPCB_TOKEN_CBS_DF_DBG_PIE_PRQ_BCST_LIMIT                           0x0015
#define HPCB_TOKEN_CBS_DF_DBG_LMT_MAX_IO_REQ_MASTER                        0x0016
#define HPCB_TOKEN_CBS_DF_DBG_LMT_MAX_IO_REQ_LIMIT                         0x0017
#define HPCB_TOKEN_CBS_DF_DBG_IOS_RSP_BYPASS_CTRL                          0x0018
#define HPCB_TOKEN_CBS_DF_DBG_CAKE_BYPASS_CTRL                             0x0019
#define HPCB_TOKEN_CBS_DF_DBG_CS_BYPASS_CTRL                               0x001A
#define HPCB_TOKEN_CBS_DF_DBG_TGT_START_CTRL                               0x001B
#define HPCB_TOKEN_CBS_DF_DBG_LINK_ERR_INJ                                 0x001C
#define HPCB_TOKEN_CBS_DF_DBG_VC8_TRACING_TOK_ALLOC                        0x001D
#define HPCB_TOKEN_CBS_DF_DBG_VC8_TRACING_REQ_CS                           0x001E
#define HPCB_TOKEN_CBS_DF_DBG_VC8_TRACING_REQ_IOMS                         0x001F
#define HPCB_TOKEN_CBS_DF_DBG_VC8_TRACING_CSQ                              0x0020
#define HPCB_TOKEN_CBS_DF_DBG_VC8_TRACING_RSP_IOMS                         0x0021
#define HPCB_TOKEN_CBS_DF_DBG_VC8_TRACING_RSP_TCDX                         0x0022
#define HPCB_TOKEN_CBS_DF_DBG_REDUCE_CREDITS_REQ                           0x0023
#define HPCB_TOKEN_CBS_DF_DBG_REDUCE_CREDITS_PRB                           0x0024
#define HPCB_TOKEN_CBS_DF_DBG_REDUCE_CREDITS_RSP                           0x0025
#define HPCB_TOKEN_CBS_DF_DBG_LOAD_STEP_CTRL_DIS                           0x0026
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_ENFORCE_FREQ_DDR5                    0x0027
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_CTL_DDR5                             0x0028
#define HPCB_TOKEN_CBS_DBG_MEM_SPEED_DDR5                                  0x0029
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_UCLK_DIV1_M0_DDR5                    0x002A
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_UCLK_DIV1_M1_DDR5                    0x002B
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_UCLK_DIV1_M2_DDR5                    0x002C
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_UCLK_DIV1_M3_DDR5                    0x002D
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TCL_DDR5                             0x002E
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRCDRD_DDR5                          0x002F
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRCDWR_DDR5                          0x0030
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRP_DDR5                             0x0031
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRTP_DDR5                            0x0032
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRAS_DDR5                            0x0033
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRRD_L_DDR5                          0x0034
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRRD_S_DDR5                          0x0035
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRC_CTL_DDR5                         0x0036
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRC_DDR5                             0x0037
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRDWR_DDR5                           0x0038
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TWRRD_DDR5                           0x0039
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TWTR_L_DDR5                          0x003A
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TWTR_S_DDR5                          0x003B
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TWRWR_SC_DDR5                        0x003C
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TWRWR_SD_DDR5                        0x003D
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TWRWR_DD_DDR5                        0x003E
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRDRD_SC_DDR5                        0x003F
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRDRD_SD_DDR5                        0x0040
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRDRD_DD_DDR5                        0x0041
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TREF_CTL_DDR5                        0x0042
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TREF_DDR5                            0x0043
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRWT_WB_DDR5                         0x0044
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRFC0_CTL_DDR5                       0x0045
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRFC0_DDR5                           0x0046
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRFC1_CTL_DDR5                       0x0047
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRFC1_DDR5                           0x0048
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TCKSRX_CTL_DDR5                      0x0049
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TCKSRX_DDR5                          0x004A
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TCKSRE_CTL_DDR5                      0x004B
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TCKSRE_DDR5                          0x004C
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TPD_CTL_DDR5                         0x004D
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TPD_DDR5                             0x004E
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_ADDR_CMD_TRI_EN_DDR5                0x004F
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER2_T_MODE_DDR5                        0x0050
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_DATA_SCRAMBLE_DDR5                  0x0051
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_ON_DIMM_TEMP_SENSOR_EN_DDR5         0x0052
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_CMD_THROTTLE_MODE_CTL_DDR5          0x0053
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_CMD_THROTTLE_MODE_DDR5              0x0054
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_WR_CRC_EN_DDR5                      0x0055
#define HPCB_TOKEN_CBS_DBG_MEM_GEAR_DOWN_MODE_DDR5                         0x0056
#define HPCB_TOKEN_CBS_DBG_MEM_HW_HISTORY_MECH_DDR5                        0x0057
#define HPCB_TOKEN_CBS_DBG_MEM_DRAM_PARITY_DDR5                            0x0058
#define HPCB_TOKEN_CBS_DBG_MEM_RCD_PARITY_DDR5                             0x0059
#define HPCB_TOKEN_CBS_DBG_MEM_DBI_DDR5                                    0x005A
#define HPCB_TOKEN_CBS_DBG_MEM_ADDR_TWEAK_EN_CTRL_DDR5                     0x005B
#define HPCB_TOKEN_CBS_DBG_MEM_ADDR_TWEAK_EN_DDR5                          0x005C
#define HPCB_TOKEN_CBS_DBG_MEM_DATA_ENCRYPTION_DDR5                        0x005D
#define HPCB_TOKEN_CBS_DBG_MEM_FORCE_ENCRYPTION_DDR5                       0x005E
#define HPCB_TOKEN_CBS_DBG_MEM_FINE_GRAN_REF_DDR5                          0x005F
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PWR_DN_EN_DDR5                      0x0060
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PWR_DN_MO_DDR5                      0x0061
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PD_DLY_CTL_DDR5                     0x0062
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PD_DLY_DDR5                         0x0063
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_AGGRESSIVE_PWR_DOWN_EN_DDR5         0x0064
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_AGGRE_PD_DLY_CTL_DDR5               0x0065
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_AGGRE_PD_DLY_DDR5                   0x0066
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PHY_POWER_SAVING_DDR5               0x0067
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PD_PHY_POWER_SAVE_DIS_DDR5          0x0068
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PHY_RECEIVER_POWER_MODE_DDR5        0x0069
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_MEM_PWR_POLICY_DDR5                 0x006A
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_ALL_MEM_CLKS_DDR5                   0x006B
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_ALL_CKE_DDR5                        0x006C
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_ALL_CS_DDR5                         0x006D
#define HPCB_TOKEN_CBS_DBG_MEM_DATA_POISONING_DDR5                         0x006E
#define HPCB_TOKEN_CBS_DBG_MEM_ADDR_CMD_PARITY_RETRY_DDR5                  0x006F
#define HPCB_TOKEN_CBS_DBG_MEM_ADDR_CMD_PARITY_ERROR_MAX_REPLAY_DDR5       0x0070
#define HPCB_TOKEN_CBS_DBG_MEM_WRITE_CRC_RETRY_DDR5                        0x0071
#define HPCB_TOKEN_CBS_DBG_MEM_WRITE_CRC_ERROR_MAX_REPLAY_DDR5             0x0072
#define HPCB_TOKEN_CBS_DBG_MEM_INJECT_TX_CMD_PARITY_ERRORS_DDR5            0x0073
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR5           0x0074
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_DRAM_ECC_EN_DDR5                    0x0075
#define HPCB_TOKEN_CBS_DBG_MEM_DATA_BUS_CONFIG_CTL_DDR5                    0x0076
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_DATA_DRV_STR_DDR5                   0x0077
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_RTT_NOM_DDR5                        0x0078
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_RTT_WR_DDR5                         0x0079
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_RTT_PARK_DDR5                       0x007A
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_A_RD_PTR_MP0_CTL_DDR5                0x007B
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_A_RD_PTR_MP0_DDR5                    0x007C
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_A_RD_PTR_MP1_CTL_DDR5                0x007D
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_A_RD_PTR_MP1_DDR5                    0x007E
#define HPCB_TOKEN_CBS_DBG_MEM_DFI_MRL_MARGIN_CTL_DDR5                     0x007F
#define HPCB_TOKEN_CBS_DBG_MEM_DFI_MRL_MARGIN_DDR5                         0x0080
#define HPCB_TOKEN_CBS_DBG_MEM_PER_RANK_TIMING_DDR5                        0x0081
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PMU_TRAIN_MODE_DDR5                 0x0082
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_MR6_VREF_DQ_CTRL_DDR5               0x0083
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_MR6_VREF_DQ_DDR5                    0x0084
#define HPCB_TOKEN_CBS_DBG_MEM_VREF_RANGE_DDR5                             0x0085
#define HPCB_TOKEN_CBS_DBG_CLDO_E12_GA                                     0x0086
#define HPCB_TOKEN_CBS_DBG_CLDO_DDR                                        0x0087
#define HPCB_TOKEN_CBS_DBG_CLDO_WAFL                                       0x0088
#define HPCB_TOKEN_CBS_DBG_CLDO_E12_GB                                     0x0089
#define HPCB_TOKEN_CBS_DBG_CLDO_VDDM0                                      0x008A
#define HPCB_TOKEN_CBS_DBG_CLDO_VDDM1                                      0x008B
#define HPCB_TOKEN_CBS_DBG_CLDO_VDDP_CTL                                   0x008C
#define HPCB_TOKEN_CBS_DBG_CLDOVDD_PVOLTAGE                                0x008D
#define HPCB_TOKEN_CBS_DBG_CLDO_VDDM_CTL                                   0x008E
#define HPCB_TOKEN_CBS_DBG_CLDOVDD_MVOLTAGE                                0x008F
#define HPCB_TOKEN_CBS_DBG_CPU_GEN_HBL_BP                                  0x0090
#define HPCB_TOKEN_CBS_DBG_CPU_GEN_HBL_CON_OUT                             0x0091
#define HPCB_TOKEN_CBS_DBG_MEM_UMC_KEY0_DDR5                               0x0092
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PMU_TRAIN_DFE_DDR5                  0x0093
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PMU_TRAIN_FFE_DDR5                  0x0094
#define HPCB_TOKEN_CBS_DBG_GNB_DBGU_NBIO_PORT80_TRACING                    0x0095
#define HPCB_TOKEN_CBS_DBG_GNB_DBGU_NBIO_PORT80_TIMESTAMP                  0x0096
#define HPCB_TOKEN_CBS_DBG_GNB_RESERVED1                                   0x0097
#define HPCB_TOKEN_CBS_DBG_GNB_RESERVED2                                   0x0098
#define HPCB_TOKEN_CBS_DF_DBG_CAKE_TKN_THRESH                              0x0099
#define HPCB_TOKEN_CBS_DF_DBG_OPT_CAKE_DAT_LAT                             0x009A
#define HPCB_TOKEN_CBS_DF_DBG_CPU_MSTR_RCV_PRB_RSP                         0x009B
#define HPCB_TOKEN_CBS_DF_DBG_IO_MSTR_RCV_PRB_RSP                          0x009C
#define HPCB_TOKEN_CBS_DBG_CLDO_VDDM                                       0x009D
#define HPCB_TOKEN_CBS_DBG_MEM_PSTATES_DDR5                                0x009E
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_POST_PACKAGE_REPAIR_EN_DDR5         0x009F
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_POST_PACKAGE_REPAIR_ALL_BANKS_DDR5  0x00A0
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_POST_PACKAGE_REPAIR_ALL_RANKS_DDR5  0x00A1
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TRDRD_SC_L_DDR5                      0x00A2
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TWRWR_SC_L_DDR5                      0x00A3
#define HPCB_TOKEN_CBS_DBG_MEM_TIMING_TCWL_DDR5                            0x00A4
#define HPCB_TOKEN_CBS_DBG_CPU_GEN_HBL_HDT_OUT                             0x00A5
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PMU_DELAY_WEIGHT2_D_CTRL_DDR5       0x00A6
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PMU_DELAY_WEIGHT2_D_DDR5            0x00A7
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PMU_VOLTAGE_WEIGHT2_D_CTRL_DDR5     0x00A8
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PMU_VOLTAGE_WEIGHT2_D_DDR5          0x00A9
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PMU_BITS_PER_POINT_CTRL_DDR5        0x00AA
#define HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_PMU_BITS_PER_POINT_DDR5             0x00AB
#define HPCB_TOKEN_CBS_DBG_CPU_PM_SC_THROTTLE                              0x00AC
#define HPCB_TOKEN_CBS_DBG_CPU_PM_SC_THROTTLE_CTRL                         0x00AD
#define HPCB_TOKEN_CBS_DBG_CPU_GEN_HBL_HDT_OUT_DIE_CTRL                    0x00AE
#define HPCB_TOKEN_CBS_DBG_CPU_GEN_HBL_HDT_OUT_DIE_BIT_MASK                0x00AF
#define HPCB_TOKEN_CBS_DBG_END                                             0x00B0
#define HPCB_TOKEN_CBS_DBG_LIMIT                                           0x1FFF

typedef enum {
  HPCB_TIME_POINT_NONE = 0,
  HPCB_TIME_POINT_ANY  = 1,
  HPCB_TIME_POINT_BEFORE_DRAM_INIT,
  HPCB_TIME_POINT_AFTER_DRAM_SPD_TIMING_INIT,
  HPCB_TIME_POINT_AFTER_DRAM_DATASHEET_TIMING_INIT,
  HPCB_TIME_POINT_AFTER_DRAM_PHY_INIT,
  HPCB_TIME_POINT_AFTER_DRAM_INIT,
  HPCB_TIME_POINT_BEFORE_PMU_TRAINING,
  HPCB_TIME_POINT_AFTER_PMU_TRAINING,
  HPCB_TIME_POINT_AFTER_DRAM_FINALIZATION,
  HPCB_TIME_POINT_BEFORE_TRAINING,
  HPCB_TIME_POINT_TRAINNING_STEP1,
  HPCB_TIME_POINT_TRAINNING_STEP2,
  HPCB_TIME_POINT_TRAINNING_STEP3,
  HPCB_TIME_POINT_TRAINNING_STEP4,
  HPCB_TIME_POINT_TRAINNING_STEP5,
  HPCB_TIME_POINT_TRAINNING_STEP6,
  HPCB_TIME_POINT_TRAINNING_STEP7,
  HPCB_TIME_POINT_TRAINNING_STEP8,
  HPCB_TIME_POINT_TRAINNING_STEP9,
  HPCB_TIME_POINT_TRAINNING_STEP10,
  HPCB_TIME_POINT_AFTER_TRAINING,
  HPCB_TIME_POINT_BEFORE_HMI_TRAINING,
  HPCB_TIME_POINT_AFTER_HMI_TRAINING,
  HPCB_TIME_POINT_AFTER_DF_FINAL_INIT,
  HPCB_TIME_POINT_BEFORE_DF_SPF_INIT,
  HPCB_TIME_POINT_BEFORE_DF_CREDIT_RELEASE,
  HPCB_TIME_POINT_END
} HPCB_TIME_POINT;

typedef struct {
  UINT32    TimePoint : 8;              ///< Specify when the parameter can be retrieved
  UINT32    Token     : 13;
  UINT32    Size      : 3;            ///< The actual size of the parameter - 1
  UINT32    Reserved  : 8;
} HPCB_PARAM_ATTRIBUTE;

typedef struct {
  UINT8    *BaseAddr[2];              ///< Pointers to the value stream
} HPCB_PARAM_BASE_ADDRESS;

typedef struct {
  UINT8     Valid    : 2;             ///< Indicator if the map entry is valid
  UINT8     Size     : 3;
  UINT8     Reserved : 3;
  UINT8     TimePoint;
  UINT16    Offset[2];                ///< Offset to the parameter
                                      ///< within the HPCB parameter block
} HPCB_PARAM_MAP;

typedef struct {
  UINT16    HpcbTokenConfig;                  ///< The HPCB Config Token to be linked
  UINT16    HpcbTokenCbs;                     ///< The HPCB CBS Token to be linked
} HPCB_LINKED_TOKENS;

/// DDR5 DIMM ID information
typedef struct {
  UINT8      Socket;                   ///< Indicates the socket number
  UINT8      Channel;                  ///< Indicates the channel number
  UINT8      Dimm;                     ///< Indicates the channel number
  BOOLEAN    DimmPresentInConfig;      ///< Indicates that the DIMM config is present
  UINT16     PrevPage1SpdChecksumLow;  ///< DIMM checksum for lower 128 bytes
  UINT16     PrevPage1SpdChecksumHigh; ///< DIMM checksum for Page 1 upper 128 bytes
  UINT16     PrevDimmManId;            ///< DIMM manufacturer ID
  UINT16     PrevDimmModId;            ///< DIMM Module ID
} DIMM_HPCB_INFO_CONFIG_ID;

///
/// Memory Board Layout Info
///
#define MEMORY_LAYOUT_TYPE_DIMM     1
#define MEMORY_LAYOUT_TYPE_ONBOARD  2

// -----------------------------------------------------------------------------
///
/// SPD Data for each DIMM.
///
typedef struct _DIMM_INFO {
  UINT8    DimmSlotPresent;   ///< Indicates that the DIMM is present and Data is valid
  UINT8    SocketId;          ///< Indicates the socket number
  UINT8    ChannelId;         ///< Indicates the channel number
  UINT8    DimmId;            ///< Indicates the channel number
  UINT8    DimmSmbusAdderess; ///< SMBus address of the DRAM
  UINT8    I2CMuxAddress;     ///< I2C Mux Address
  UINT8    MuxControlAddress;
  UINT8    MuxChannel;        ///< I2C Mux Channel assocaited iwth this SPD
} DIMM_INFO_SMBUS;

// -----

/// ===============================================================================
/// _PSP_PSO_STRUCT
/// This data structure is used to pass wrapper parameters to the memory configuration code
///
typedef struct _PSP_PSO_STRUCT {
  PSO_ENTRY    MemClkDisMap[MEMCLK_DIS_MAP_SIZE];
  PSO_ENTRY    CkeTriMap[CKE_TRI_MAP_SIZE];
  PSO_ENTRY    OdtTriMap[ODT_TRI_MAP_SIZE];
  PSO_ENTRY    CsTriMap[CS_TRI_MAP_SIZE];
  PSO_ENTRY    NumberDimmSupported[NUMBER_OF_DIMMS_SUPPORTED_SIZE];
  PSO_ENTRY    NumberSolderedDownDimmsSupported[NUMBER_OF_SOLDERED_DOWN_DIMMS_SUPPORTED_SIZE];
  PSO_ENTRY    NumberChipSelectsSupported[NUMBER_OF_CHIP_SELECTS_SUPPORTED_SIZE];
  PSO_ENTRY    NumberChannelsSupported[NUMBER_OF_CHANNELS_SUPPORTED_SIZE];
  PSO_ENTRY    OverrideBusSpeed[OVERRIDE_DDR_BUS_SPEED_SIZE];
  PSO_ENTRY    DramTechnology[DRAM_TECHNOLOGY_SIZE];
  PSO_ENTRY    SolderedDownSoDimmType[SOLDERED_DOWN_SODIMM_TYPE_SIZE];
  PSO_ENTRY    MemoryPowerPolicyOverride[MEMORY_POWER_POLICY_OVERRIDE_SIZE];
  PSO_ENTRY    MotherboardLayers[MOTHER_BOARD_LAYERS_SIZE];
} PSP_PSO_STRUCT;
/// ===============================================================================
/// _PSP_SPD_LPDDR3_STRUCT
/// This data structure is used to pass wrapper parameters to the memory configuration code
/// In presilion enviroment SPD_DEF_STRUCT only spd data is important, size 1024
///
typedef struct _PSP_SPD_STRUCT {
  UINT32            TotalDimms;
  SPD_DEF_STRUCT    SpdData[HPCB_TOTAL_SYSTEM_DIMMS];
} PSP_SPD_STRUCT;

#if 0
/// HDTOUT Header.
typedef struct _HDTOUT_HEADER {
  UINT32    Signature;            ///< 0xDB1099CC
  UINT16    Version;              ///< HDTOUT version.
  UINT16    BufferSize;           ///< Size in bytes.
  UINT16    DataIndex;            ///< Data Index.
  UINT8     PrintCtrl;            ///< 0 off no print  1 on print
  UINT8     NumBreakpointUnit;    ///< default 0 no bp unit others number of bp unit
  UINT32    FuncListAddr;         ///< 32 bit address to the list of functions that script can execute
  UINT8     ConsoleType;          ///< Console type - deprecated
  UINT8     Event;                ///< Event type. - deprecated
  UINT8     OutBufferMode;        ///< Off:stack mode, On: heap mode - deprecated
  UINT32    EnableMask;           ///< Bitmap to select which part should be streamed out
  UINT64    ConsoleFilter;        ///< Filter use to select which part should be streamed out
  UINT8     BspOnlyFlag;          ///< 1 Only Enable Bsp output, 0 enable On All cores
  UINT8     Reserved[56 - 32];    ///< Reserved for header expansion

  CHAR8     BreakpointList[300];  ///< Breakpoint list
  CHAR8     StatusStr[156];       ///< Shows current node, DCT, CS,...
  CHAR8     Data[2];              ///< HDTOUT content. Its size will be determined by BufferSize.
} HDTOUT_HEADER;
#endif
/// HBL Console Out Header.
typedef struct _HBL_CONSOLE_OUT_HEADER {
  BOOLEAN    HblConsoleEnable;                  ///< Enable HBL Console output
                                                ///< TRUE - Enable HBL console output  (Default)
                                                ///< FALSE - Disable HBL console output
  UINT32     HblConsolePort;                    ///< 32 bit HBL Console output port (Default - 0x80)
  BOOLEAN    HblConsoleFilterMemFlowEnable;     ///< HBL Console out filter for "MEM FLOW"    (Default - Enabled)
                                                ///< TRUE - Filter is enabled  (Default)
                                                ///< FALSE - Fitler is diesabed
  BOOLEAN    HblConsoleFilterMemSetRegEnable;   ///< HBL Console out filter to enable "MEM SETREG" (Default - Enabled)
                                                ///< TRUE - Filter is enabled  (Default)
                                                ///< FALSE - Fitler is diesabed
  BOOLEAN    HblConsoleFilterMemGetRegEnable;   ///< HBL Console out filter to enable "MEM GETREG"   (Default - disabled)
                                                ///< TRUE - Filter is enabled
                                                ///< FALSE - Fitler is diesabed (Default)
  BOOLEAN    HblConsoleFilterMemStatusEnable;   ///< HBL Console out filter to enable "MEM STATUS"   (Default - disabled)
                                                ///< TRUE - Filter is enabled
                                                ///< FALSE - Fitler is diesabed (Default)
  BOOLEAN    HblConsoleFilterMemPmuEnable;      ///< HBL Console out filter to enable "MEM PMU"   (Default - disabled)
                                                ///< TRUE - Filter is enabled
                                                ///< FALSE - Fitler is diesabed (Default)
  BOOLEAN    HblConsoleFilterMemPmuSramReadEnable; ///< HBL Console out filter to enable "MEM PMU SRAM READ" (Default - disabled)
                                                   ///< TRUE - Filter is enabled
                                                   ///< FALSE - Fitler is diesabed (Default)
  BOOLEAN    HblConsoleFilterMemPmuSramWriteEnable; ///< HBL Console out filter to enable "MEM PMU SRAM WRITE" (Default - disabled)
                                                    ///< TRUE - Filter is enabled
                                                    ///< FALSE - Fitler is diesabed (Default)
  BOOLEAN    HblConsoleFilterMemTestVerboseEnable; ///< HBL Console out filter to enable "MEM TEST VERBOSE" (Default - disabled)
                                                   ///< TRUE - Filter is enabled
                                                   ///< FALSE - Fitler is diesabed (Default)
} HBL_CONSOLE_OUT_HEADER;

/// Breakpoint control
typedef struct _HBL_BREAKPOINT_CONTROL {
  BOOLEAN    BreakPointEnable;   ///< Enable HBL Console output
                                 ///< TRUE - All dies will stop at the same breakpoints (Default)
                                 ///< FALSE - Master will only stop with breakpoints
  BOOLEAN    BreakOnAllDies;     ///< Enable HBL Console output
                                 ///< TRUE - All dies will stop at the same breakpoints  (Default)
                                 ///< FALSE - Master will only stop with breakpoints
} HBL_BREAKPOINT_CONTROL;

/// ===============================================================================
/// PSP_CONSOLE_OUT_STRUCT
/// This data structure is used to control console output support
///
typedef struct _PSP_CONSOLE_OUT_STRUCT {
  HBL_CONSOLE_OUT_HEADER    ConsoleOutControl;  // < Control structure for Console out
  HBL_BREAKPOINT_CONTROL    BreakPointControl;  // < Control structure for Breakpoints
} PSP_CONSOLE_OUT_STRUCT;

/// ===============================================================================
/// PSP_EVENT_OUT_CONTROL_STRUCT
/// This data structure is used to control sending HGPI Event info to the specified port
///
typedef struct _PSP_EVENT_OUT_CONTROL_STRUCT {
  IN
  BOOLEAN    EVENT_OUT_PORT_ENABLED; ///< Indicates if HGPI PSP will output Event Info to specificed port
  IN
  BOOLEAN    HALT_ON_CRITICAL_FATAL_EVENT_ENABLED; ///< Indicates if HGPI PSP will halt on critical/fatal event
  IN
  UINT64     EventInfoPort; // < Port to output HGPI Event Info
} PSP_EVENT_OUT_CONTROL_STRUCT;

/// ===============================================================================
/// ERROR_HBL_GPIO
/// This data structure is user-defined GPIO indicator of a failure
///
typedef struct _ERROR_HBL_GPIO {
  UINT8    Gpio;           ///< FCH GPIO number of failure indicator
  UINT8    GpioIoMUX;      ///< Value write to IOMUX to configure this GPIO pin
  UINT8    GpioBankCtl;    ///< Value write to GPIOBankCtl[23:16] to configure this GPIO pin
} ERROR_HBL_GPIO;

/// ===============================================================================
/// ERROR_BEEP_CODE_TABLE
/// This data structure can be customized/enhanced as per platform need
///
#define BEEP_ERROR_TYPE_MASK       0xF000
#define BEEP_ERROR_OPERATION_MASK  0x0FFF
#define BEEP_ERROR_TYPE_GENERAL    0x3000
#define BEEP_ERROR_TYPE_MEMORY     0x4000
#define BEEP_ERROR_TYPE_DF         0x5000
#define BEEP_ERROR_TYPE_CCX        0x6000
#define BEEP_ERROR_TYPE_GNB        0x7000
#define BEEP_ERROR_TYPE_PSP        0x8000
#define BEEP_ERROR_TYPE_SMU        0x9000
#define BEEP_ERROR_TYPE_UNKNOW     0xFFFF
#define BEEP_ERROR_TABLE_SIZE      8

// PEAK_ATTR attribute
typedef union {
  struct {
    UINT32    PeakCnt     : 5;  ///< number of valid bit, zeor based
    UINT32    PulseWidths : 3;  ///< pulse widths of positive peak, multiple of 0.1s
    UINT32    RepeatCnt   : 4;  ///< waves repeat count
    UINT32                : 20; ///< Reserved
  } Bits;
  UINT32    Uint32;
} PEAK_ATTR;

/// ===============================================================================
/// MEMORY OVERCLOCK ERROR TYPE
///
typedef enum {
  MEM_OVERCLOCK_ERROR_MEM_INIT     = 0,
  MEM_OVERCLOCK_ERROR_MEM_TEST     = 1,
  MEM_OVERCLOCK_ERROR_MEM_OTHER    = 2,
  MEM_OVERCLOCK_ERROR_PMU_TRAINING = 3,
  MEM_OVERCLOCK_ERROR_RRW_TEST     = 4,
  MEM_OVERCLOCK_ERROR_MAX          = 5
} MEMORY_OVERCLOCK_ERROR_TYPE;

typedef struct _ERROR_BEEP_CODE_TABLE {
  UINT16    ErrorType;     ///< Error type is specified for the beep codes
  UINT16    PeakMap;       ///< Peak bitmap value to indicated the error
  UINT32    WA32;          ///
} ERROR_BEEP_CODE_TABLE;

/// ===============================================================================
/// PSP_ERROR_OUT_CONTROL_STRUCT
/// This data structure is used to control sending HGPI Event info to the specified port
///
typedef struct _PSP_ERROR_OUT_CONTROL_STRUCT {
  IN
  BOOLEAN                  ErrorLogPortReportingEnable; ///< Indicates if HBL will report errors via a port
                                                        ///< TRUE - Error logging will be reported via a port
                                                        ///< FALSE - Error logging will not be reported via a port
  IN
  BOOLEAN                  ErrorReportErrorGpioEnable; ///< Indicates if HBL will report errors via GPIO
                                                       ///< TRUE - Error logging will be reported via a GPIO
                                                       ///< FALSE - Error logging will not be reported via a GPIO
                                                       ///< (only valid if ErrorLogPortReportingEnable = TRUE)
  IN
  BOOLEAN                  ErrorReportErrorBeepCodeEnable; ///< This flag indicates if the HBL will report error via beep codes
                                                           ///< TRUE - Error logging will be reported via FCH speaker
                                                           ///< FALSE - Error logging will not be reported via FCH speaker
                                                           ///< (only valid if ErrorLogPortReportingEnable = TRUE)
  IN
  BOOLEAN                  ErrorLogReportUsingHandshakeEnable; ///< This flag indicates if the HBL will use an handshake for the Error Log
                                                               ///< TRUE - Error log reported using a handshake with the "ErrorLogOutputPort" and "ErrorLogInputPort"
                                                               ///< FALSE - Error log reported using "ErrorLogOutputPort" only with each DWORD in log delayed by
                                                               ///< ErrorLogOutputDwordDelay
  IN
  UINT32                   ErrorLogInputPort;  ///< Input Port to receive HBL Error information
                                               ///< (only valid if ReportErrorLogUsingHandshakeEnable = TRUE)
  IN
  UINT32                   ErrorLogOutputDwordDelay; ///< Number of "10ns" to wait before sending the next Log Dword informaiton via "ErrorLogOutputPort"
                                                     ///< (only valid if ReportErrorLogUsingHandshakeEnable = FALSE)
  IN
  UINT32                   ErrorLogOutputPort; ///< Output Port for HBL Error information
  IN
  BOOLEAN                  ErrorStopOnFirstFatalErrorEnable; ///< Indicates that HBL will stop on the first fatal error
                                                             ///< TRUE - Stop and report the first FATAL error
                                                             ///< FALSE - Report all errors
  IN
  UINT32                   ErrorLogReportInputPortSize; ///< Indicates the sie of the input and outut port
                                                        ///< 1 - 8 bit port
                                                        ///< 2 - 16 bit port
                                                        ///< 4 - 32 bit port
  IN
  UINT32                   ErrorLogReportOutputPortSize; ///< Indicates the sie of the input and outut port
                                                         ///< 1 - 8 bit port
                                                         ///< 2 - 16 bit port
                                                         ///< 4 - 32 bit port
  IN
  UINT32                   ErrorLogReportInputPortType; ///< Indicates the type of Input Port or location of the port
                                                        ///< 0 - PCIE HT0
                                                        ///< 2 - PCIE HT1
                                                        ///< 5 - PCIE MMIO
                                                        ///< 6 - FCH_HT_IO (Default)
                                                        ///< 7 - FCH_MMIO
  IN
  UINT32                   ErrorLogReportOutputPortType; ///< Indicates the type of Output Port or location of the port
                                                         ///< 0 - PCIE HT0
                                                         ///< 2 - PCIE HT1
                                                         ///< 5 - PCIE MMIO
                                                         ///< 6 - FCH_HT_IO (Default)
                                                         ///< 7 - FCH_MMIO

  IN
  BOOLEAN                  ErrorLogReportClearAcknowledgement; ///< Indicates if the HBL will clear acknolgements during protocol
                                                               ///< TRUE - Clear acknowledgemetns
                                                               ///< FALSE - Do not clear acknologements

  ERROR_HBL_GPIO           ErrorLogReportGpioReport; ///< Structure for the GPIO definition
                                                     ///< (only valid if ErrorReportErrorGpioEnable = TRUE)

  ERROR_BEEP_CODE_TABLE    BeepCodeTable[BEEP_ERROR_TABLE_SIZE];      ///< Structure for definition of beep codes
                                                                      ///< (only valid if ErrorReportErrorBeepCodeEnable = TRUE)
  IN
  BOOLEAN                  ErrorLogHeartBeatEnable; ///< Indicates if HBL will provide periodic status to a port as a heart beat
                                                    ///< TRUE - Heartbeat Error log will be reported via a port
                                                    ///< FALSE - Heartbeat Error log will not be reported via a port
                                                    ///<
                                                    ///< Notes:
                                                    ///< 1) This feature is only valid if ErrorLogPortReportingEnable
                                                    ///< 2) This is can be mono-directional or bi-directional based on "ErrorLogReportUsingHandshakeEnable"
                                                    ///< "ErrorLogReportUsingHandshakeEnable" = TRUE - bi-directional
                                                    ///< "ErrorLogReportUsingHandshakeEnable" = FALSE - mono-directional
                                                    ///< 3) Requires the following to be defined:
                                                    ///< - "ErrorLogReportInputPortType", "ErrorLogInputPort, "ErrorLogReportInputPortSize"
                                                    ///< - "ErrorLogReportInputPortType", "ErrorLogInputPort, "ErrorLogReportInputPortSize"
} PSP_ERROR_OUT_CONTROL_STRUCT;

/// ===============================================================================
/// PSP_EXT_VOLTAGE_CONTROL_STRUCT
/// This data structure is used to control sending HGPI volatage info to the specified port
///
typedef struct _PSP_EXT_VOLTAGE_CONTROL_STRUCT {
  IN
  BOOLEAN    ExtVoltageControlEnable;          ///< Indicates if HBL will supprot external voltage changes
                                               ///< TRUE - External voltage change are supported
                                               ///< FALSE - No external voltage change are supported (Default)
  IN
  UINT32     ExtVoltageInputPort;              ///< Input Port to receive voltage information  (Default = 0x84)
  IN
  UINT32     ExtVoltageOutputPort;             ///< Output Port to send voltage (Default = 0x80)
  IN
  UINT32     ExtVoltageInputPortSize;          ///< Indicates the sie of the input and outut port
                                               ///< 1 - 8 bit port
                                               ///< 2 - 16 bit port
                                               ///< 4 - 32 bit port (Default)
  IN
  UINT32     ExtVoltageOutputPortSize;         ///< Indicates the sie of the input and outut port
                                               ///< 1 - 8 bit port
                                               ///< 2 - 16 bit port
                                               ///< 4 - 32 bit port (Default)
  IN
  UINT32     ExtVoltageInputPortType;           ///< Indicates the type of Input Port or location of the port
                                                ///< 0 - PCIE HT0
                                                ///< 2 - PCIE HT1
                                                ///< 5 - PCIE MMIO
                                                ///< 6 - FCH_HT_IO (Default)
                                                ///< 7 - FCH_MMIO
  IN
  UINT32     ExtVoltageOutputPortType;          ///< Indicates the type of Output Port or location of the port
                                                ///< 0 - PCIE HT0
                                                ///< 2 - PCIE HT1
                                                ///< 5 - PCIE MMIO
                                                ///< 6 - FCH_HT_IO (Default)
                                                ///< 7 - FCH_MMIO

  IN
  BOOLEAN    ExtVoltageClearAcknowledgement;   ///< Indicates if the HBL will clear acknolgements during protocol
                                               ///< TRUE - Clear acknowledgemetns (Default)
                                               ///< FALSE - Do not clear acknologements
} PSP_EXT_VOLTAGE_CONTROL_STRUCT;

// HPCB HEADER
// #define HPCB_HEADER_VERSION_MAJOR   2
// #define HPCB_HEADER_VERSION_MINOR   0
// #define HPCB_HEADER_VERSION         ((HPCB_HEADER_VERSION_MAJOR << 4) | HPCB_HEADER_VERSION_MINOR)

//
// HPCB Header Definition Version 2
//
typedef struct {
  UINT32    Signature;        // ASCII "HPCB", 'A' is LSB
  UINT16    SizeOfHeader;     // Size of header
  UINT16    Version;          // Version, BCD. Version 1.2 is 0x12
  UINT32    SizeOfHpcb;       // Size of HPCB
  UINT32    UniqueHpcbInstance; // UniqueHpcbInstance to ensure
                                // compatibitly for giveen flshed BIOS lifecycle
  UINT8     CheckSumByte;      // HPCB Checksum Byte
  UINT8     Reserved1[3];      // Reserved
  UINT32    Reserved2[3];      // Reserved
} HPCB_HEADER;

#define HPCB_INDEX_HEADER_VERSION_MAJOR  0
#define HPCB_INDEX_HEADER_VERSION_MINOR  1
#define HPCB_INDEX_HEADER_VERSION        ((HPCB_INDEX_HEADER_VERSION_MAJOR << 4) | HPCB_INDEX_HEADER_VERSION_MINOR)
//
// HPCB Index of Data Type Header Definition
//
typedef struct {
  UINT16    SizeOfHeader;    // Size of header
  UINT16    Version;         // Version, BCD. Version 1.2 is 0x12
  UINT16    Reserved;        // Reserved
  UINT32    SizeOfIndexData; // Size of index data, absolute address in PSP
                             // NOTE: Will we support it to be relative address to starging address of HPCB?
} HPCB_INDEX_HEADER;

//
// HPCB Index of Data Type  Definition
//
typedef struct {
  UINT16    GroupId;        // Group ID
  UINT16    TypeId;         // Type ID
  UINT32    Address;        // Data address
} HPCB_TYPE_INDEX;

//
// HPCB Data Type Header Definition
//
typedef struct {
  UINT16    GroupId;        // Group ID
  UINT16    TypeId;         // Type ID
  UINT16    SizeOfType;     // Size of type, in bytes
  UINT16    InstanceId;     // Instance ID
  UINT32    Reserved1;
  UINT32    Reserved2;
} HPCB_TYPE_HEADER;

#define ALIGN_SIZE_OF_TYPE  4

#define HPCB_GROUP_HEADER_VERSION_MAJOR  0
#define HPCB_GROUP_HEADER_VERSION_MINOR  1
#define HPCB_GROUP_HEADER_VERSION        ((HPCB_GROUP_HEADER_VERSION_MAJOR << 4) | HPCB_GROUP_HEADER_VERSION_MINOR)

#define INVALID_GROUP_ID  0xFFFF
#define UNKNOWN_GROUP_ID  0xFFFE

#define OVERRIDE_INSTANCE  0x10000
#define INSTANCE_ID_MASK   0xFFFF

//
// HPCB Data Group Header Definition
//
typedef struct {
  UINT32    Signature;      // ASCII Signature
  UINT16    GroupId;        // Group ID
  UINT16    SizeOfHeader;   // Size of header
  UINT16    Version;        // Version, BCD. Version 1.2 is 0x12
  UINT16    Reserved;       // Reserved
  UINT32    SizeOfGroup;    // Size of group
} HPCB_GROUP_HEADER;

/// ===============================================================================
// Get board ID retrieve method, then load the HPCB instance based on the board ID

#define BOARD_ID_METHOD_SMBUS   1
#define BOARD_ID_METHOD_EEPROM  2
#define BOARD_ID_METHOD_GPIO    3
#define METHOD_USER_CONFIG      0xF

#define ID_FEATURE_MASK  0x80            // This is Bit 7 of IdAndFeatureMask field

typedef struct {
  IN       UINT8    IdAndFeatureMask;  ///< Mask to the value read from board ID register
                                       ///< Bit 6:0 - Id Mask Bits
                                       ///< Bit7 - determines between normal and feature controlled Instance
                                       ///< Bit7,   1 = User Controlled Feature Enabled, 0 - Normal Mode
  IN       UINT8    IdAndFeatureValue; ///< IdValue after mask
  IN       UINT8    HpcbInstance;      ///< The corresponding HPCB instance
} ID_HPCB_MAPPING;

/// For EEPROM method
typedef struct _PSP_GET_BOARD_ID_FROM_EEPROM_STRUCT {
  IN       UINT16    AccessMethod;  ///< 0: EEPROM, 1: Smbus, 2: FCH GPIO. 0xF: USER_CONFIG
  IN       UINT16    I2cCtrlr;      ///< SAT I2C controller: 0: I2C_0, 1: I2C_1, 2: I2C_2, 3: I2C_3, 4: I2C_4, 5: I2C_5                                   ///
  IN       UINT16    SmbusAddr;     ///< Smbus address of the EEPROM
} PSP_GET_BOARD_ID_FROM_EEPROM_STRUCT;

/// For Smbus device method
typedef struct _PSP_GET_BOARD_ID_FROM_SMBUS_STRUCT {
  IN       UINT16             AccessMethod; ///< 0: EEPROM, 1: Smbus, 2: FCH GPIO F: None
  IN       UINT16             I2cCtrlr;     ///< SAT I2C controller: 0: I2C_0, 1: I2C_1, 2: I2C_2, 3: I2C_3, 4: I2C_4, 5: I2C_5                                   ///
  IN       UINT16             SmbusAddr;    ///< Smbus address of the device to get the board ID
  IN       UINT16             RegIndex;     ///< Register index of the Smbus device to get the board ID
  IN       ID_HPCB_MAPPING    IdHpcbMapping[];
} PSP_GET_BOARD_ID_FROM_SMBUS_STRUCT;

/// For FCH GPIO method
typedef struct _PSP_GET_BOARD_ID_FROM_GPIO_STRUCT {
  IN       UINT16             AccessMethod; ///< 0: EEPROM, 1: Smbus, 2: FCH GPIO, 0xF: USER_CONFIG
  IN       UINT8              Gpio0;        ///< FCH GPIO number of the board ID bit 0
  IN       UINT8              Gpio0IoMUX;   ///< Value write to IOMUX to configure this GPIO pin
  IN       UINT8              Gpio0BankCtl; ///< Value write to GPIOBankCtl[23:16] to configure this GPIO pin
  IN       UINT8              Gpio1;        ///< FCH GPIO number of the board ID bit 1
  IN       UINT8              Gpio1IoMUX;   ///< Value write to IOMUX to configure this GPIO pin
  IN       UINT8              Gpio1BankCtl; ///< Value write to GPIOBankCtl[23:16] to configure this GPIO pin
  IN       UINT8              Gpio2;        ///< FCH GPIO number of the board ID bit 2, put 0xff if does not exist
  IN       UINT8              Gpio2IoMUX;   ///< Value write to IOMUX to configure this GPIO pin
  IN       UINT8              Gpio2BankCtl; ///< Value write to GPIOBankCtl[23:16] to configure this GPIO pin
  IN       UINT8              Gpio3;        ///< FCH GPIO number of the board ID bit 3, put 0xff if does not exist
  IN       UINT8              Gpio3IoMUX;   ///< Value write to IOMUX to configure this GPIO pin
  IN       UINT8              Gpio3BankCtl; ///< Value write to GPIOBankCtl[23:16] to configure this GPIO pin
  IN       ID_HPCB_MAPPING    IdHpcbMapping[];
} PSP_GET_BOARD_ID_FROM_GPIO_STRUCT;

/// For User Controlled Method to Load Specific HPCB Instance
typedef struct _PSP_METHOD_USER_CONTROLLED_FEATURE_STRUCT {
  IN       UINT16             AccessMethod;    ///< 0: EEPROM, 1: Smbus, 2: FCH GPIO,  0xF: USER_CONFIG
  IN       UINT16             FeatureMaskData; ///< Data Pattern defined by user which can be used to
                                               ///< mask between normal and feature enabled Instance selection
  IN       ID_HPCB_MAPPING    IdHpcbMapping[];
} PSP_METHOD_USER_CONTROLLED_FEATURE_STRUCT;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

#define  DF_MEM_INTLV_SOCKET   3
#define  DF_MEM_INTLV_DIE      2
#define  DF_MEM_INTLV_CHANNEL  1
#define  DF_MEM_INTLV_NONE     0

#define  DF_DRAM_INTLV_SIZE_256   0
#define  DF_DRAM_INTLV_SIZE_512   1
#define  DF_DRAM_INTLV_SIZE_1024  2
#define  DF_DRAM_INTLV_SIZE_2048  3

// Note a is from 0 to 31
#define BP_SHIFT(a)  ((UINT64)1 << a)

// Breakpoint Definition
// UMC BPs
#define HPCB_BP_UMC_PHASE_1_END            BP_SHIFT (0)          // End of Phase 1 Initialization  - all addressing/UMC configs done
#define HPCB_BP_UMC_START_PMU_ON_IMAGE     BP_SHIFT (1)          // Start PMU on imagex  - PHY init/SRAM Msg Block done ; just staring 1D training
#define HPCB_BP_UMC_MISSION_MODE_CONFIG    BP_SHIFT (2)          // UMC mision mode configuration
#define HPCB_BP_UMC_PMU_FINISHED           BP_SHIFT (3)          // PMU Finish
#define HPCB_BP_UMC_2D_TRAINING_DONE       BP_SHIFT (4)          // PMU Based Training complete  - 2D Training done
#define HPCB_BP_UMC_MEM_PHASE_2_END        BP_SHIFT (5)          // End of Phase 2 Initialization  - Mission mode timings updated (turn around timings)
#define HPCB_BP_UMC_BEFORE_INITIAL_PSTATE  BP_SHIFT (6)          // Before initial P-state change
#define HPCB_BP_UMC_AFTER_INITIAL_PSTATE   BP_SHIFT (7)          // After initial P-state change
#define HPCB_BP_UMC_BEFORE_DRAM_READY      BP_SHIFT (8)          // Before set UMC DramReady = 1

// DF BPs
#define HPCB_BP_DF_INIT_START              BP_SHIFT (10)
#define HPCB_BP_DF_BEFORE_LINK_TRAINING    BP_SHIFT (11)
#define HPCB_BP_DF_LINK_TRAINING_COMPLETE  BP_SHIFT (12)
#define HPCB_BP_DF_BEFORE_CREDIT_RELEASE   BP_SHIFT (13)
#define HPCB_BP_DF_INIT_COMPLETE           BP_SHIFT (14)

// Other BPS
#define HPCB_BP_CUSTOM        BP_SHIFT (30)
#define HPCB_BREAKPOINT_DONE  BP_SHIFT (31)

// BP related Register definitions
#define UMC_CTRL_SMN_BASE              0x51000
#define ENV_CONTROL_UMC_SCRATCH_REG_0  (UMC_CTRL_SMN_BASE | 0x050)     // UserDataPattern0
#define ENV_CONTROL_UMC_SCRATCH_REG_1  (UMC_CTRL_SMN_BASE | 0x054)     // UserDataPattern1
#define ENV_CONTROL_UMC_SCRATCH_REG_2  (UMC_CTRL_SMN_BASE | 0x058)     // UserDataPattern2
#define ENV_CONTROL_UMC_SCRATCH_REG_3  (UMC_CTRL_SMN_BASE | 0x05C)     // UserDataPattern3

#define HPCB_BREAKPOINT_SMN_DATA_MBOX_0  ENV_CONTROL_UMC_SCRATCH_REG_2 // Data Mailbox
#define HPCB_BREAKPOINT_SMN_CMD_MBOX_1   ENV_CONTROL_UMC_SCRATCH_REG_3 // Command Mailbox

#define HPCB_BREAKPOINT_CMD_HBL_BP_FOUND     0x5000C001  // Command from HBL indicating that BP found
#define HPCB_BREAKPOINT_CMD_HBL_BP_DONE      0x5000DEAD  // Command from HBL indicating that All BPs are done
#define HPCB_BREAKPOINT_CMD_CONTINUE         0xC001C001  // Command from script to HBL indicating continue execution
#define HPCB_BREAKPOINT_CMD_EXTERNAL_ENABLE  0xC001BEE5  // Command to enable breakpoints
#define HPCB_BREAKPOINT_CMD_BP_LIST          0xC001BB11  // Command from script to indicating

//
// HPCB BreakPoint Definition
//
typedef struct {
  UINT32    BreakPointsEnabled;       // Indicates if BP are enabled
  UINT32    BreakPointValue;          // BreakPoint Value
} HPCB_BREAKPOINT_CONTROL_STRUCT;

//
// DDR Post Package Repair
//
#define MAX_DPPRCL_ENTRY     24
#define MAX_DPPRCL_SOCKETS   2
#define MAX_DPPRCL_CHANNELS  8
#define MAX_DPPRCL_ENTRIES   MAX_DPPRCL_SOCKETS *MAX_DPPRCL_CHANNELS  *MAX_DPPRCL_ENTRY

#define DPPR_SOFT_REPAIR     0
#define DPPR_HARD_REPAIR     1
#define DPPR_VALID_REPAIR    1
#define DPPR_INVALID_REPAIR  0

#define DPPR_DEVICE_X4   4
#define DPPR_DEVICE_X8   8
#define DPPR_DEVICE_X16  16

#define DPPR_NUM_CACHE_LINES  128
#define DPPR_BUBBLE_COUNT     256

typedef struct _HPCB_DPPRCL_REPAIR_ENTRY {
  UINT32    Bank           : 5;            ///< [4:0] Bank
  UINT32    RankMultiplier : 3;            ///< [7:5] Rank Multiplier
  UINT32    Device         : 5;            ///< [12:8] Device
  UINT32    ChipSelect     : 2;            ///< [14:13] ChipSelect
  UINT32    Column         : 10;           ///< [24:15] Column Address
  UINT32    RepairType     : 1;            ///< [25:25] Hard/soft repair (0=hard, 1=soft)
  UINT32    Valid          : 1;            ///< [26:26] Valid entry
  UINT32    TargetDevice   : 5;            ///< [31:27] Target Device (only valid if Device = 0x1F)
                                           ///< x4 (0-17 (with ECC)), x8 (0-8 (with ECC))
  UINT32    Row            : 18;           ///< [18:0] Row Address
  UINT32    Socket         : 3;            ///< [21:19] Socket
  UINT32    Channel        : 3;            ///< [21:19] Channel
  UINT32    Reserverd1     : 8;            ///< pad to 32 bit
} HPCB_DPPRCL_REPAIR_ENTRY;

#endif
