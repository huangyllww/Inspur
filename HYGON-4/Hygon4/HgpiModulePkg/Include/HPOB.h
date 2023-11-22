/* $NoKeywords:$ */

/**
 * @file
 *
 * HPCB.h
 *
 * HGPI PSP Output Block
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: HGPI
 * @e sub-project: (Mem)
 *
 **/
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

#ifndef _HPOB_H_
#define _HPOB_H_

// #include "PlatformMemoryConfiguration.h"
// #include "mnreg.h"
// #include "mempmu.h"
#include "HGPI.h"

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */
#define HPOB_SIGNATURE                      0x424F5041    //HPOB
#define HPOB_VERSION                        0x04221101    //Major:Year:Month:Minor

#define HPOB_MAX_SOCKETS_SUPPORTED   	      8   ///< Max number of sockets in system
#define HPOB_MAX_DIMMS_PER_CHANNEL   	      2   ///< Max DIMMs on a memory channel

#define HPOB_MAX_CDDS_PER_SOCKET_HYEX     	  4   ///< Max CDD dies per socket
#define HPOB_MAX_CHANNELS_PER_CDD_HYEX    	  3   ///< Max Channels per CDD
#define HPOB_MAX_CHANNELS_PER_SOCKET_HYEX 	  (HPOB_MAX_CDDS_PER_SOCKET_HYEX  * HPOB_MAX_CHANNELS_PER_CDD_HYEX)

#define HPOB_MAX_CDDS_PER_SOCKET_HYGX     	  8   ///< Max CDD dies per socket
#define HPOB_MAX_CHANNELS_PER_CDD_HYGX    	  2   ///< Max Channels per CDD
#define HPOB_MAX_CHANNELS_PER_SOCKET_HYGX 	  (HPOB_MAX_CDDS_PER_SOCKET_HYGX  * HPOB_MAX_CHANNELS_PER_CDD_HYGX)

/*
 *   HPOB die_type_t
 *   cannot use die_type_t in this header files, this files linked with HPOB
 *    use macro replace the enum structure in mcm.h
 *       #define IO_DIE_TYPE 0       IO_DIE
 *       #define CORE_DIE_TYPE 1     CORE_DIE
 */
#define HYEX_IO_DIE_TYPE      (0)
#define HYEX_CORE_DIE_TYPE    (1)

#define HYGX_DJ_DIE_TYPE      (0)
#define HYGX_EMEI_DIE_TYPE    (1)
#define HYGX_CORE_DIE_TYPE    (2)

// HPOB Group Definitons
#define HPOB_GROUP_MEM       1
#define HPOB_GROUP_DF        2
#define HPOB_GROUP_CCX       3
#define HPOB_GROUP_GNB       4
#define HPOB_GROUP_FCH       5
#define HPOB_GROUP_PSP       6
#define HPOB_GROUP_GEN       7
#define HPOB_GROUP_SMBIOS    8
#define HPOB_GROUP_FABRIC    9
#define HPOB_GROUP_FLOW      10

//////////////////////////////// HPOB Type Define ////////////////////////////////

/// HPOB MEM Type Definitions
#define HPOB_MEM_GENERAL_ERRORS_TYPE                         1
#define HPOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE             2	//used
#define HPOB_MEM_NVDIMM_INFO_TYPE                            15	//used
#define HPOB_HPCB_BOOT_INFO_TYPE                             16	//used
#define HPOB_MEM_DIMM_SPD_DATA_TYPE                          17	//used
#define HPOB_MEM_MBIST_RESULT_INFO_TYPE                      18	//used

/// HPOB DF Type Definitions

/// HPOB CCX Type Definitions
#define HPOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE       1	//used

/// HPOB GNB Type Definitions

/// HPOB FCH Type Definitions

/// HPOB PSP Type Definitions
#define HPOB_CDD_LOGICAL_TO_PHYSICAL_MAP_TYPE       1	//used

/// HPOB General Type Definitions
#define HPOB_GEN_CONFIGURATION_INFO_TYPE            3 //used
#define HPOB_GEN_S3_REPLAY_BUFFER_INFO_TYPE         4

/// HPOB SMBIOS Type Definitions
#define HPOB_MEM_SMBIOS_TYPE                        8	// used

/// HPOB Fabric Type Definitions
#define HPOB_SYS_MAP_INFO_TYPE                      9	// used

/// HPOB Flow Type Define
#define HPOB_FLOW_NVSAVE_HEADER_TYPE                1 //used
#define HPOB_FLOW_MEM_ENV_INFO_TYPE                 2 //used
#define HPOB_FLOW_S3_REPLAY_BUFFER_INFO_TYPE        3 //used


#define HMAC_HASH_SIZE                              32

//////////////////////////////// Common Structure Define ////////////////////////////////
/// HPOB_HMAC
typedef struct
{
	UINT8 HpobHmac[HMAC_HASH_SIZE];		///< HPOB HMAC for secutity check of each type
} HPOB_HMAC;

/// HPOB_TYPE Header
typedef struct
{
  UINT32       GroupID;                     ///< Group ID
  UINT32       DataTypeID;                  ///< Data Type ID
  UINT32       InstanceID;                  ///< Instance ID
                                            ///< - Bit 16-23 - SocketNumbers
                                            ///< - Bit 15:8 - DieType
                                            ///< - Bit  7:0 - CDD Numbers
  UINT32       TypeSize;                    ///< Type Size (including header)
  HPOB_HMAC    HpobTypeHmac;                ///< Hmac location for type
} HPOB_TYPE_HEADER;

//////////////////////////////////////// HPOB Entry Define Start //////////////////////////////////////////////
typedef struct
{
  HPOB_TYPE_HEADER    HpobTypeHeader;                                 ///< HPOB Type Header
  UINT32              BootMediaTypeInfo;                              ///< BootMediaTypeInfo
  UINT16              MemClkFreq;                                     ///< Data from MemClkFreq: 667/800/933/1067/1200/1333/1467/1600 Mhz
  UINT16              DdrMaxRate;                                     ///< UMC DdrMaxRate
  BOOLEAN             EccEnable[HPOB_MAX_CHANNELS_PER_CDD_HYEX];       ///< ECC enabled or Disabled per channel. ie. Channel 0 or Channel 1
  BOOLEAN             NvDimmInstalled[HPOB_MAX_CHANNELS_PER_CDD_HYEX]; ///< NVDIMM presence per channel. ie. Channel 0 or Channel 1
  BOOLEAN             ChannelIntlvEn;                                 ///< DIMM channel interleave status
  UINT8               Reserved1[3];
} HPOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT_HYEX;

typedef struct
{
  HPOB_TYPE_HEADER    HpobTypeHeader;                                 ///< HPOB Type Header
  UINT32              BootMediaTypeInfo;                              ///< BootMediaTypeInfo
  UINT16              MemClkFreq;                                     ///< Data from MemClkFreq: 667/800/933/1067/1200/1333/1467/1600 Mhz
  UINT16              DdrMaxRate;                                     ///< UMC DdrMaxRate
  BOOLEAN             EccEnable[HPOB_MAX_CHANNELS_PER_CDD_HYGX];       ///< ECC enabled or Disabled per channel. ie. Channel 0 or Channel 1
  BOOLEAN             NvDimmInstalled[HPOB_MAX_CHANNELS_PER_CDD_HYGX]; ///< NVDIMM presence per channel. ie. Channel 0 or Channel 1
  BOOLEAN             ChannelIntlvEn;                                 ///< DIMM channel interleave status
  UINT8               Reserved1[3];
} HPOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT_HYGX;

#define CCX_MAX_SOCKETS                HPOB_MAX_SOCKETS_SUPPORTED

#define CCX_MAX_DIES_PER_SOCKET_HYEX    HPOB_MAX_CDDS_PER_SOCKET_HYEX
#define CCX_MAX_COMPLEXES_PER_DIE_HYEX  4
#define CCX_MAX_CORES_PER_COMPLEX_HYEX  4

#define CCX_MAX_DIES_PER_SOCKET_HYGX    HPOB_MAX_CDDS_PER_SOCKET_HYGX
#define CCX_MAX_COMPLEXES_PER_DIE_HYGX  2
#define CCX_MAX_CORES_PER_COMPLEX_HYGX  8

#define CCX_MAX_THREADS_PER_CORE       2  
#define CCX_NOT_PRESENT                (0xFF)

typedef struct
{
  UINT8      PhysCoreNumber;
  BOOLEAN    IsThreadEnabled[CCX_MAX_THREADS_PER_CORE];
} LOGICAL_CORE_INFO;

typedef struct
{
  UINT8                PhysComplexNumber;
  LOGICAL_CORE_INFO    CoreInfo[CCX_MAX_CORES_PER_COMPLEX_HYEX];
} LOGICAL_COMPLEX_INFO_HYEX;

typedef struct
{
  HPOB_TYPE_HEADER            HpobTypeHeader;  ///< HPOB Type Header
  LOGICAL_COMPLEX_INFO_HYEX    ComplexMap[CCX_MAX_COMPLEXES_PER_DIE_HYEX];
  UINT8                       padding[4];
} HPOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE_STRUCT_HYEX;

typedef struct
{
  UINT8                       PhysComplexNumber;
  LOGICAL_CORE_INFO           CoreInfo[CCX_MAX_CORES_PER_COMPLEX_HYGX];
} LOGICAL_COMPLEX_INFO_HYGX;

typedef struct
{
  HPOB_TYPE_HEADER            HpobTypeHeader;  ///< HPOB Type Header
  LOGICAL_COMPLEX_INFO_HYGX    ComplexMap[CCX_MAX_COMPLEXES_PER_DIE_HYGX];
  UINT8                       padding[6];
} HPOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE_STRUCT_HYGX;

/// HPOB SPD Data for each DIMM.
typedef struct _HPOB_SPD_STRUCT
{
  UINT8              SocketNumber;  ///< Indicates the socket number
  UINT8              ChannelNumber; ///< Indicates the channel number
  UINT8              DimmNumber;    ///< Indicates the channel number
  UINT8              PageAddress;   ///< Indicates the 256 Byte EE Page the data belongs to
  // 0 = Lower Page
  // 1 = Upper Page
  TECHNOLOGY_TYPE    Technology; ///< Indicates the type of Technology used in SPD
                                 ///< DDR3_TECHNOLOGY = Use DDR3 DIMMs
                                 ///< DDR4_TECHNOLOGY = Use DDR4 DIMMs
								///<   DDR5_TECHNOLOGY = Use DDR5 DIMMs
                                 ///< LPDDR3_TECHNOLOGY = Use LPDDR3

  BOOLEAN            DimmPresent;   // < Indicates if the DIMM is present
  UINT8              MuxPresent;    ///< SpdMux Present or not. if 1, then yes otherwise no
  UINT8              MuxI2CAddress; ///< MuxI2cAddress
  UINT8              MuxChannel;    ///< MuxChannel no.
  UINT32             Address;       ///< SMBus address of the DRAM
  UINT32             SerialNumber;  ///< DIMM Serial Number
  UINT8              Data[1024];    ///< Buffer for 1024 Bytes of SPD data from DIMM
} HPOB_SPD_STRUCT;

typedef struct
{
  HPOB_TYPE_HEADER    HpobTypeHeader;
  UINT8               MaxDimmsPerChannel;
  UINT8               MaxChannelsPerCdd;
  UINT8               padding0[2];
  HPOB_SPD_STRUCT     DimmSmbusInfo[HPOB_MAX_CHANNELS_PER_CDD_HYEX * HPOB_MAX_DIMMS_PER_CHANNEL];   ///<
  UINT8               padding1[4];
} HPOB_MEM_DIMM_SPD_DATA_STRUCT_HYEX;

typedef struct
{
  HPOB_TYPE_HEADER    HpobTypeHeader;
  UINT8               MaxDimmsPerChannel;
  UINT8               MaxChannelsPerCdd;
  UINT8               padding0[2];
  HPOB_SPD_STRUCT     DimmSmbusInfo[HPOB_MAX_CHANNELS_PER_CDD_HYGX * HPOB_MAX_DIMMS_PER_CHANNEL];   ///<
  UINT8               padding1[4];
} HPOB_MEM_DIMM_SPD_DATA_STRUCT_HYGX;

typedef enum
{
  UMA,                        ///< UC DRAM cycles.  I don't think this is needed in SoC15 since HBM is not part of the system map
  MMIO,                       ///< Cycles are sent out to IO.  Only expect the 1 below 4GB
  PrivilegedDRAM,             ///< Read-only zero.  No special cache considerations are needed.  Map out of E820
  ReservedCxl,                ///< CXL memory is NOT available until later in POST
  MaxMemoryHoleTypes,         ///< Not a valid type.  Used for validating the others.
} MEMORY_HOLE_TYPES;

typedef struct
{
  UINT64               Base;  ///< Full 64 bit base address of the hole
  UINT64               Size;  ///< Size in bytes of the hole
  MEMORY_HOLE_TYPES    Type;  ///< Hole type
  UINT8                padding[4];
} MEMORY_HOLE_DESCRIPTOR;

typedef struct
{
  UINT64                    TopOfSystemMemory; ///< Final DRAM byte address in the system + 1
  UINT32                    NumberOfHoles;     ///< SoC15 systems will always have at least one
  UINT8                     padding[4];
  MEMORY_HOLE_DESCRIPTOR    HoleInfo[1];       ///< Open ended array of descriptors
} SYSTEM_MEMORY_MAP;

typedef struct
{
  HPOB_TYPE_HEADER          HpobTypeHeader; ///< HPOB Type Header
  SYSTEM_MEMORY_MAP         HpobSystemMap; ///< HPOB event log
  MEMORY_HOLE_DESCRIPTOR    AdditionalHoleInfo[33];
} HPOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT_HYEX;

typedef struct
{
  HPOB_TYPE_HEADER          HpobTypeHeader; ///< HPOB Type Header
  SYSTEM_MEMORY_MAP         HpobSystemMap; ///< HPOB event log
  MEMORY_HOLE_DESCRIPTOR    AdditionalHoleInfo[97]; //MMIO(1) + CDD_NUMBER(8X8)+cxl(8X4)
} HPOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT_HYGX;

/// Memory DMI Type 17 - for memory use
typedef struct
{
  OUT UINT8     Socket;                                 ///< Socket ID
  OUT UINT8     Channel;                                ///< Channel ID
  OUT UINT8     Dimm;                                   ///< DIMM ID
  OUT UINT8     DimmPresent;                            ///< Dimm Present
  OUT UINT8     Reserved[5];
  OUT UINT8     SpdAddr;                                ///< SPD Address
  OUT UINT16    Handle;                                 ///< The temporary handle, or instance number, associated with the structure
  OUT UINT16    ConfigSpeed;                            ///< Configured memory clock speed
  OUT UINT16    ConfigVoltage;                          ///< Configured voltage for this device, in millivolt
} HPOB_MEM_DMI_PHYSICAL_DIMM;

/// Memory DMI Type 20 - for memory use
typedef struct
{
  OUT UINT8     Socket;                                 ///< Socket ID
  OUT UINT8     Channel;                                ///< Channel ID
  OUT UINT8     Dimm;                                   ///< DIMM ID
  OUT UINT8     DimmPresent;                            ///< Dimm Present
  OUT UINT8     Interleaved;                            ///< Interleaving enabled
  OUT UINT8     Reserved;
  OUT UINT16    MemoryDeviceHandle;                     ///< The handle, or instance number, associated with
                                                        ///< the Memory Device structure to which this address
                                                        ///< range is mapped.
  OUT UINT32    StartingAddr;                           ///< The physical address, in kilobytes, of a range
                                                        ///< of memory mapped to the referenced Memory Device.
  OUT UINT32    EndingAddr;                             ///< The handle, or instance number, associated with
                                                        ///< the Memory Device structure to which this address
								                                        ///< range is mapped.
	union
	{
    OUT UINT64    ExtStartingAddr;                      ///< The physical address, in bytes, of a range of
                                                        ///< memory mapped to the referenced Memory Device.
		struct
    {
      OUT UINT32    ExtStartingAddrLow;
      OUT UINT32    ExtStartingAddrHigh;
    } ExtStartingAddrForPsp;
  } UnifiedExtStartingAddr;
	union
	{
    OUT UINT64    ExtEndingAddr;                        ///< The physical ending address, in bytes, of the last of
                                                        ///< a range of addresses mapped to the referenced Memory Device.
		struct
    {
      OUT UINT32    ExtEndingAddrLow;
      OUT UINT32    ExtEndingAddrHigh;
    } ExtEndingAddrForPsp;
  } UnifiedExtEndingAddr;
} HPOB_MEM_DMI_LOGICAL_DIMM;

// FIXME: Move HpobTypeHeader to HPOB_MEM_DMI_INFO_STRUCT
typedef struct {
  HPOB_TYPE_HEADER    HpobTypeHeader;                  ///< HPOB Type header
  UINT8               MemoryType : 7;                  ///< Memory Type
  UINT8               EccCapable : 1;                  ///< ECC Capable
  UINT8               MaxPhysicalDimms;                ///< Maximum physical DIMMs
  UINT8               MaxLogicalDimms;                 ///< Maximum logical DIMMs
  UINT8               Reserved;
} HPOB_MEM_DMI_HEADER;

// FIXME: Field Update according to BIOS Requirement?
typedef struct
{
  HPOB_TYPE_HEADER    HpobTypeHeader;
  UINT32              NvdimmInfo[HPOB_MAX_SOCKETS_SUPPORTED * HPOB_MAX_CHANNELS_PER_SOCKET_HYEX * HPOB_MAX_DIMMS_PER_CHANNEL][2];
  BOOLEAN             NvdimmPresentInSystem;
  UINT8               padding[7];
  UINT64              NvdimmPresentInChannel;
} HPOB_MEM_NVDIMM_INFO_STRUCT_HYEX;

typedef struct
{
  HPOB_TYPE_HEADER    HpobTypeHeader;
  UINT32              NvdimmInfo[HPOB_MAX_SOCKETS_SUPPORTED * HPOB_MAX_CHANNELS_PER_SOCKET_HYGX * HPOB_MAX_DIMMS_PER_CHANNEL][2];
  BOOLEAN             NvdimmPresentInSystem;
  UINT8               padding[7];
  UINT64              NvdimmPresentInChannel;
} HPOB_MEM_NVDIMM_INFO_STRUCT_HYGX;

/// DDR4 DIMM ID information
typedef struct
{
  UINT16              DimmManId;            ///< DIMM manufacturer ID
  UINT16              DimmModId;            ///< DIMM Module ID
  UINT16              Page1SpdChecksumLow;  ///< DIMM checksum for lower 128 bytes
  UINT16              Page1SpdChecksumHigh; ///< DIMM checksum for upper 128 bytes
  BOOLEAN             DimmPresentInConfig;  ///< Indicates that the DIMM config is present
  UINT8               Reserved[3];
} DIMM_HPOB_OPT_ID;

// Defintion for HPOB Boot Info to pass RecoveryFlag and HpcbInstance
// FIXME: Remove CurrentBootDimmIdInfo and DimmConfigurationUpdated
typedef struct
{
  HPOB_TYPE_HEADER    HpobTypeHeader;
  UINT32              HpcbActiveInstance;                                                                                                      ///< HPCB Active Insatnce of the boot
  DIMM_HPOB_OPT_ID    CurrentBootDimmIdInfo[HPOB_MAX_SOCKETS_SUPPORTED][HPOB_MAX_CHANNELS_PER_SOCKET_HYGX][HPOB_MAX_DIMMS_PER_CHANNEL]; ///< DIMM Identification informaion for the current boot
  BOOLEAN             DimmConfigurationUpdated;                                                                                                // < FALSE - DIMM Configuration not updated,
										 //< TRUE - DIMM Configuration updated
  UINT8               HpcbRecoveryFlag;                                                                                                        ///< HPCB Recovery Flag
  UINT8               HpcbActiveType;          ///< HPCB Active Type
  UINT8               reserved;
  BOOLEAN             EmulationEnv; ///< Emulation Environment detected by PSP
  BOOLEAN             SimulationEnv; ///< Simulation Environment detected by PSP
  BOOLEAN             FpgaEnv;	///< FPGA Environment detected by PSP
  BOOLEAN             AsicEnv;	///< ASIC Environment detected by PSP
  UINT32              Bootmode; ///< Bootmode detected by psp
  UINT32              HpobUniqueHpcbInstance; ///< UniuweHpcbInstance to ensure: compatibitly for giveen flshed BIOS lifecycle
  UINT8               padding[4];
} HPOB_HPCB_BOOT_INFO_STRUCT;

//////////////////////////////////////// HPOB Entry Define End //////////////////////////////////////////////

//////////////////////////////////////// HPOB Structure Define Start //////////////////////////////////////////////
typedef struct
{
  UINT32                 Signature;               ///< HPOB signature
  UINT32                 Version;                 ///< Version
  UINT32                 Size;                    ///< HPOB Size
  UINT32                 OffsetOfFirstEntry;      ///< HPOB Header Size
  UINT32                 NVSaveOffset;            ///< Offset Save to Flash(BIOS Entry: 0x63)
  UINT32                 NVSaveSize;             	///< Size Save to Flash(BIOS Entry: 0x63)
} HPOB_HEADER;

//////////////////////////////////////// HPOB Structure Define End //////////////////////////////////////////////

#endif /* _HPOB_H_ */
