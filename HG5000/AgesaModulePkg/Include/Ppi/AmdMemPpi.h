/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD MEM PPI prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
 */
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
#ifndef _AMD_MEM_PPI_H_
#define _AMD_MEM_PPI_H_

#include "AGESA.h"
#include "PlatformMemoryConfiguration.h"
#include "Library/AmdCalloutLib.h"

//----------------------------------------------------
//
//  Amd Memory Service Functions
//
//-------------------------------------------------------

/**
  Reads Service for the Memory Above 4GB

  Parameters:
  PeiServices
    A pointer to the PEI services
  AmountOfMemory
    Return data pointer for the amount of memory

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
typedef EFI_STATUS (EFIAPI * PEI_AMD_SOC_GET_MEMORY_ABOVE_4GB_INTERFACE  ) (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32            *AmountOfMemory
  );
/**
  Read Service for the Memory Below 4GB

  Parameters:
  PeiServices
    A pointer to the PEI services
  AmountOfMemory
    Return data pointer for the amount of memory

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
typedef EFI_STATUS (EFIAPI * PEI_AMD_SOC_GET_MEMORY_BELOW_4GB_INTERFACE  ) (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32            *AmountOfMemory
  );
/**
  Read Service for the memory Below 1TB

  Parameters:
  PeiServices
    A pointer to the PEI services
  AmountOfMemory
    Return data pointer for the amount of memory

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
typedef EFI_STATUS (EFIAPI * PEI_AMD_SOC_GET_MEMORY_BELOW_1TB_INTERFACE  ) (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32            *AmountOfMemory
  );
/**
  Read Service for the total amount of memory in the system

  Parameters:
  PeiServices
    A pointer to the PEI services
  AmountOfMemory
    Return data pointer for the amount of memory

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
typedef EFI_STATUS (EFIAPI * PEI_AMD_SOC_GET_TOTAL_MEMORY_SIZE_INTERFACE) (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32            *AmountOfMemory
  );
/**
  Read Service for the Bottom of MMIO

  Parameters:
  PeiServices
    A pointer to the PEI services
  AmountOfMemory
    Return data pointer for the amount of memory

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
typedef EFI_STATUS (EFIAPI * PEI_AMD_SOC_GET_MEMORY_BOTTOM_IO_INTERFACE) (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32            *AmountOfMemory
  );
/**
  Read Service for the Memory Frequency of a channel

  Parameters:
  PeiServices
    A pointer to the PEI services
  Channel
    A pointer to the channel to read
  Frequency
    Return data pointer for a channel frequency

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
typedef EFI_STATUS (EFIAPI * PEI_AMD_SOC_GET_MEMORY_FREQUENCY_INTERFACE) (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32            Channel,
     OUT  UINT32            *Frequency
  );
/**
  Read Service for the Memory Frequency of a channel

  Parameters:
  PeiServices
    A pointer to the PEI services
  UmaInfo
    Return data pointer for the UmaInfo

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
typedef EFI_STATUS (EFIAPI * PEI_AMD_SOC_GET_UMA_INFO_INTERFACE) (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32            *UmaInfo
  );



/**
  Get the Memory Map Interface

  Parameters:
  PeiServices

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
typedef EFI_STATUS (EFIAPI * PEI_GET_SYSTEM_MEMORY_MAP) (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN UINT32                     *NumberOfHoles,
  IN UINT64                     *TopOfSystemMemory,
  IN VOID                       **MemHoleDescPtr
 );
 

/**
  Get DDR Post Package Repair Interface

  Parameters:
  PeiServices

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
typedef EFI_STATUS (EFIAPI * PEI_GET_DDR_POST_PACKAGE_REPAIR) (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN VOID                       **MemDdrPostPackageRepairPtr
 );



//----------------------------------------------------
//
//  Amd Memory Init Complete Ppi
//
//-------------------------------------------------------
extern EFI_GUID gAmdMemoryInitCompletePpiGuid;

//----------------------------------------------------
//
//  Amd Memory UMA Info
//
//-------------------------------------------------------
typedef struct _AMD_MEMORY_UMA_INFO {
  UMA_MODE UmaMode; ///<  Uma Mode
                       ///<  0 = None
                       ///<  1 = Specified
                       ///<  2 = Auto
  UINT32 UmaSize; ///<  The size of shared graphics dram (16-bits)
                     ///<  NV_UMA_Size[31:0]=Addr[47:16]
                     ///<
  UINT32 UmaBase;
} AMD_MEMORY_UMA_INFO;

#define AMD_MEM_PPI_MAX_SOCKETS_SUPPORTED   2  ///< Max number of sockets in system
#define AMD_MEM_PPI_MAX_DIES_PER_SOCKET     4   ///< Max dies per socket
#define AMD_MEM_PPI_MAX_CHANNELS_PER_SOCKET 8   ///< Max Channels per sockets
#define AMD_MEM_PPI_MAX_CHANNELS_PER_DIE    2   ///< Max channels per die
#define AMD_MEM_PPI_MAX_DIMMS_PER_CHANNEL   2   ///< Max dimms per die
//-----------------------------------------------------------------------------
///
/// APOB SPD Data for each DIMM.
///
typedef struct _AMD_MEM_SPD_STRUCT {
  UINT8  SocketNumber;          ///< Indicates the socket number
  UINT8  ChannelNumber;          ///< Indicates the channel number
  UINT8  DimmNumber;          ///< Indicates the channel number
  UINT8  PageAddress;
  BOOLEAN DimmPresent;   //< Indicates if the DIMM is present
  UINT8  MuxPresent;      ///< SpdMux Present or not. if 1, then yes otherwise no
  UINT8  MuxI2CAddress;      ///< MuxI2cAddress
  UINT8  MuxChannel;         ///< MuxChannel no.
  UINT32 Address;         ///< SMBus address of the DRAM
  UINT32 SerialNumber;   ///< DIMM Serial Number
  UINT8 Data[512];      ///< Buffer for 512 Bytes of SPD data from DIMM
} AMD_MEM_SPD_STRUCT;
/// AMD_MEM_DIMM_SPD_DATA_STRUCT
typedef struct {
  BOOLEAN  AmdDimmSpdDataSupported;   // Flag indicating that Apob DIMM spd data is supported
  UINT8   Reserved[3];
  AMD_MEM_SPD_STRUCT   DimmSpdInfo[AMD_MEM_PPI_MAX_SOCKETS_SUPPORTED * AMD_MEM_PPI_MAX_CHANNELS_PER_SOCKET * AMD_MEM_PPI_MAX_DIMMS_PER_CHANNEL];     ///<
} AMD_MEM_DIMM_SPD_DATA_STRUCT;
//----------------------------------------------------
//
//  Amd Memory Init Complete Ppi
//
//-------------------------------------------------------
typedef struct _AMD_MEMORY_INIT_COMPLETE_PPI {
  UINT32                 Revision;                                              ///< revision
  UINT16                 AmdBottomIo;                                           ///< Bottom IO
  UINT32                 AmdMemoryBelow4gb;                                     ///< Memory below 4G
  UINT32                 AmdMemoryAbove4gb;                                     ///< Memory above 4G
  UINT32                 AmdMemoryBelow1Tb;                                     ///< Memory below 1T
  UINT32                 AmdTotalMemorySize;                                    ///< Total Memory Size
  UINT32                 HoleBase;                                              ///< Hole Base
  UINT32                 Sub4GCacheTop;                                         ///< Sub 4G Cache Top
  UINT32                 Sub1THoleBase;                                         ///< Sub 1T Hole Base
  UINT32                 SysLimit;                                              ///< System Limit
  UINT32                 AmdMemoryFrequency;                                    ///< Memory Frequency
  DIMM_VOLTAGE           AmdMemoryVddIo;                                        ///< Memory Vddio
  VDDP_VDDR_VOLTAGE      AmdMemoryVddpVddr;                                     ///< Memory Vddp Vddr
  AMD_MEMORY_UMA_INFO    AmdGetUmaInfo;                                         ///< AMD UMA Info
  UINT32                 DdrMaxRate;                                            ///< DdrMaxRate
  PEI_GET_SYSTEM_MEMORY_MAP        GetSystemMemoryMap;                          ///< System Memory Map
  PEI_GET_DDR_POST_PACKAGE_REPAIR        GetDdrPostPackageRepairInfo;           ///< Ddr Post Package Repair
  AMD_MEM_DIMM_SPD_DATA_STRUCT    AmdDimmSpInfo;                                ///< Memory SPD info
} AMD_MEMORY_INIT_COMPLETE_PPI;

#define AMD_MEMORY_INIT_COMPLETE_REVISION   0x01

//----------------------------------------------------
//
//  Amd Memory Channel Translation Table Ppi
//
//-------------------------------------------------------
typedef struct _AMD_MEMORY_CHANNEL_XLAT_PPI {
  UINT32                          Revision;                                              ///< revision
  VOID                            *XLatTab;                                              ///< Translation Table
} AMD_MEMORY_CHANNEL_XLAT_PPI;

#define AMD_MEMORY_CHANNEL_XLAT_REVISION   0x01

//----------------------------------------------------
//
//  Amd Memory Platform Configuration Ppi
//
//-------------------------------------------------------
typedef struct _AMD_AGESA_MEM_PLATFORM_CONFIGURATION_PPI {
  UINT32                          Revision;                                              ///< revision
  PSO_ENTRY                       *PlatformMemoryConfiguration;                          ///< Platform Memory Configurattion
} AMD_AGESA_MEM_PLATFORM_CONFIGURATION_PPI;

//----------------------------------------------------
//
// PEI_AMD_PLATFORM_DIMM_SPD_PPI data structure definition.
//
//-------------------------------------------------------
typedef struct _PEI_AMD_PLATFORM_DIMM_SPD_PPI PEI_AMD_PLATFORM_DIMM_SPD_PPI;

//----------------------------------------------------
//
// Defines function prototype for PlatformDimmSpdRead
//
//-------------------------------------------------------
typedef
EFI_STATUS
(EFIAPI *PEI_AMD_DIMM_SPD_READ) (
  IN      EFI_PEI_SERVICES                    **PeiServices,  ///< Pointer to PeiServices
  IN struct _PEI_AMD_PLATFORM_DIMM_SPD_PPI    *This,          ///< Pointer to the PPI structure
  IN OUT  AGESA_READ_SPD_PARAMS               *SpdData        ///< SPD Data Buffer
  );

//----------------------------------------------------
//
// PEI_AMD_PLATFORM_DIMM_SPD_PPI data structure definition.
//
//-------------------------------------------------------
struct _PEI_AMD_PLATFORM_DIMM_SPD_PPI {          // See the Forward Declaration above
  UINT8                                       TotalNumberOfSocket;    ///< Total Number of Physical Socket.
  UINT8                                       TotalNumberOfDimms;     ///< Total Number of DIMMS
  PEI_AMD_DIMM_SPD_READ                       PlatformDimmSpdRead;    ///< Function to be called
};

extern EFI_GUID gAmdMemoryBeforeDramInitPpiGuid;

/**
  Memory Before DRAM Init Interface

  Parameters:
  PeiServices

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
typedef EFI_STATUS (EFIAPI * PEI_AMD_MEMORY_BEFORE_DRAM_INIT_INTERFACE  ) (
  IN      EFI_PEI_SERVICES  **PeiServices
  );

//----------------------------------------------------
//
//  Amd Memory Before Dram Init Ppi
//
//-------------------------------------------------------
typedef struct _PEI_AMD_MEMORY_BEFORE_DRAM_INIT_PPI {
  UINT32                          Revision;                                              ///< revision
} PEI_BEFORE_DRAM_INIT_COMPLETE_PPI;

#define AMD_MEMORY_BEFORE_DRAM_INIT_REVISION   0x00

extern EFI_GUID gAmdMemoryAfterDramInitPpiGuid;

//----------------------------------------------------
//
//  Amd Memory After Dram Init Ppi
//
//-------------------------------------------------------
typedef struct _PEI_AMD_MEMORY_AFTER_DRAM_INIT_PPI {
  UINT32                          Revision;                                              ///< revision
} PEI_AFTER_DRAM_INIT_COMPLETE_PPI;

#define AMD_MEMORY_AFTER_DRAM_INIT_REVISION   0x00

extern EFI_GUID gAmdMemoryInitializeAgesaMemoryPpiGuid;

//----------------------------------------------------
//
//  Amd Memory Init Ppi
//
//-------------------------------------------------------
typedef struct _PEI_INITIALIZE_AGESA_MEMORY_PPI {
  UINT32                          Revision;                                              ///< revision
} PEI_INITIALIZE_AGESA_MEMORY_PPI;

#define AMD_MEMORY_INITIALIZE_AGESA_MEMORY_REVISION   0x00

extern EFI_GUID gAmdMemoryFamilyServicesPpiGuid;

//----------------------------------------------------
//
//  Amd Memory Family Services PPI
//
//-------------------------------------------------------
typedef struct _PEI_MEMORY_FAMILY_SERVICE_PPI {
  UINT32                          Revision;                                              ///< revision
  PEI_AMD_SOC_GET_MEMORY_ABOVE_4GB_INTERFACE   AmdGetMemoryAbove4gbFamilyService;        ///< Service to get memory above 4G
  PEI_AMD_SOC_GET_MEMORY_BELOW_4GB_INTERFACE   AmdGetMemoryBelow4gbFamilyService;        ///< Service to get memory below 4G
  PEI_AMD_SOC_GET_MEMORY_BELOW_1TB_INTERFACE    AmdGetMemoryBelow1TbFamilyService;       ///< Service to get memory below 1T
  PEI_AMD_SOC_GET_MEMORY_BOTTOM_IO_INTERFACE    AmdGetBottomIo1TbFamilyService;          ///< Service to get BottomIo
  PEI_AMD_SOC_GET_TOTAL_MEMORY_SIZE_INTERFACE   AmdGetTotalMemorySize4gbFamilyService;   ///< Service to get total memory
} PEI_MEMORY_FAMILY_SERVICE_PPI;

#define AMD_MEMORY_FAMILY_SERVICE_REVISION   0x00

//
// GUID definition
//

extern EFI_GUID gAmdMemoryPoolPointerTablePpiGuid;

//----------------------------------------------------
//
//  Definition for AGESA Buffer Handles
//
//-------------------------------------------------------
typedef enum {
  AMD_INIT_HANDLE = 0,                        ///< First Handle pointer
  AMD_CRAT_STRUCTURE_POOL_PTR,                ///< Pointer to CRAT Table structure
  AMD_DATA_EYES_POOL_PTR,                     ///< Pointer to Data eyes table
  AMD_DATA_EYE_WORK_AREA_POOL_PTR,            ///< Pointer to Data eye work area
  AMD_DMI_DEV_INFO_STRUCTURE_POOL_PTR,        ///< Pointer to Dmi Dev Info Struncture
  AMD_DMI_DEV_INFO_DDR4_STRUCTURE_POOL_PTR,   ///< Pointer to Dmi Dev Info Struncture for DDR4
  AMD_NB_STRUCTURE_POOL_PTR,                  ///< Pointer to NB Structure
  AMD_2D_RD_WR_RIM_HANDLES_POOL_PTR,          ///< Pointer to 2D RD WR RIM Handle
  AMD_2D_RD_WR_POOL_PTR,                      ///< Pointer to 2D RD WR structure
  AMD_DEVICE_BLOCK_HEADER_POOL_PTR,           ///< Pointer to Device Block Header
  AMD_SPECIAL_CASE_REG_HEADER_POOL_PTR,       ///< Pointer to Special Case Register Heater
  AMD_MEM_DATA_STRUCT_POOL_PTR,               ///< Pointer to Mem Data Struct
  AMD_NB_BLOCK_POOL_PTR,                      ///< Pointer to NB Block
  AMD_S3_MEM_NB_BLOCK_POOL_PTR,               ///< Pointer to S3 Mem NB Block
  AMD_MEM_DIE_STRUCT_POOL_PTR,                ///< Pointer to Mem die struct
  AMD_S3_DATA_HANDLE_STRUCT_POOL_PTR,         ///< Pointer to S3 Data handle struct
  AMD_S3_SAVE_HANDLE_STRUCT_POOL_PTR,         ///< Pointer to S3 Save handle struct
  AMD_SKIP_MEM_S3_SAVE_STRUCT_POOL_PTR,       ///< Pointer to S3 Save struct
  AMD_SKIP_MEM_S3_NB_HANDLE_POOL_PTR,         ///< Pointer to S3 NB Handle
  AMD_SPD_DAT_STRUCTURE_POOL_PTR,             ///< Pointer to SPD Dat structure
  AMD_UMA_INFO_HANDLE_POOL_PTR,               ///< Pointer to UMA Info Handle
  AMD_MEM_TRAIN_BUFFER_POOL_PTR,              ///< Pointer to Mem train buffer
  AMD_NB_REG_TABLE_POOL_PTR,                  ///< Pointer to NB reg table
  AMD_UMA_INFO_POOL_PTR,                      ///< Pointer to UMC Info
  AMD_GNB_ACP_ENGINE_POOL_PTR,                ///< Pointer to GNB ACP Engine
  AMD_MEM_PMU_SRAM_MSG_BLOCK_POOL_PTR,        ///< Pointer to PMU SRAM MSG Block
  AMD_TRN_DATA_POOL_PTR,                      ///< Pointer to TRN Data
  AMD_MEM_2D_RDDQS_RIM_POOL_PTR,              ///< Pointer to  2D RDDQS RIM
  AMD_MEM_AUTO_HANDLER_POOL_PTR,              ///< Pointer to Mem Auto Handler
  AMD_FINAL_POOL_PTR                          ///< Last Handle Pointer
} AGESA_BUFFER_HANDLE;

//----------------------------------------------------
//
//  Pool Pointer Table PPI
//
//-------------------------------------------------------
typedef struct _AMD_MEM_POOL_PTR_TABLE_PPI {
  UINTN               Revision;               ///< Revision Number
  UINT32              *PtrTable[AMD_FINAL_POOL_PTR];             ///< The Point of Pointer Table List
} AMD_MEM_POOL_PTR_TABLE_PPI;

// Current PPI revision
#define AMD_MEM_POOL_PTR_TABLE_REVISION   0x00

extern EFI_GUID gAmdMemoryTechServicesPpiGuid;

//----------------------------------------------------
//
//  AMD memory tech Services PPI
//
//-------------------------------------------------------
typedef EFI_STATUS (EFIAPI * PEI_AMD_MEM_TECH_SERVICES_INTERFACE  ) (
  );
typedef struct _AMD_MEM_TECH_SERVICES_PPI {
  UINTN               Revision;                                   ///< Revision Number
  PEI_AMD_MEM_TECH_SERVICES_INTERFACE   AmdDimmPresenceService;   ///< Service to detect DIMM presence
} AMD_MEM_TECH_SERVICES_PPI;

// Current PPI revision
#define AMD_MEM_TECH_SERVICES_REVISION   0x00

extern EFI_GUID gAmdMemoryTecnologyPpiGuid;

//----------------------------------------------------
//
//  AMD memory tech PPI
//
//-------------------------------------------------------
typedef struct _PEI_MEMORY_TECHNOLOGY_PPI {
  UINTN               Revision;               ///< Revision Number
} PEI_MEMORY_TECHNOLOGY_PPI;

// Current PPI revision
#define AMD_MEMORY_TECHNOLOGY_REVISION   0x00

extern EFI_GUID gAmdMemoryAgesaReadSpdPpiGuid;

typedef EFI_STATUS (EFIAPI * PEI_AMD_MEMORY_AGESA_READ_SPD_INTERFACE  ) (
  );
//----------------------------------------------------
//
//  AMD memory Read SPD PPI
//
//-------------------------------------------------------
typedef struct _PEI_MEMORY_AGESA_READ_SPD_PPI {
  UINTN               Revision;                             ///< Revision Number
  PEI_AMD_MEMORY_AGESA_READ_SPD_INTERFACE   AgesaReadSpd;   ///< Service to read SPD
} PEI_MEMORY_AGESA_READ_SPD_PPI;

// Current PPI revision
#define AMD_MEMORY_AGESA_READ_SPD_REVISION   0x00

//----------------------------------------------------
//
// Defines function prototype to install Memory feature block
//
//-------------------------------------------------------
typedef
EFI_STATUS (*AMD_MEM_FEAT_ISNTALL_INTERFACE) (
  IN OUT     VOID*      MemFeatBlock                      ///< Memory feature block
  );
//----------------------------------------------------
//
//  AMD memory Install Feature PPI
//
//-------------------------------------------------------
typedef struct _AMD_AGESA_MEM_FEAT_INSTALL_PPI {
  UINT32                                Revision;                   ///< revision
  AMD_MEM_FEAT_ISNTALL_INTERFACE        Install;                    ///< Service to install memory feature block
} AMD_AGESA_MEM_FEAT_INSTALL_PPI;

// Current PPI revision
#define AMD_MEMORY_FEAT_INSTALL_REVISION   0x00

#endif
