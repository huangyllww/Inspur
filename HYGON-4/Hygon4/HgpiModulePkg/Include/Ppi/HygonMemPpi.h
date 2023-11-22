/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON MEM PPI prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  PSP
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
 *
 ***************************************************************************/

#ifndef _HYGON_MEM_PPI_H_
#define _HYGON_MEM_PPI_H_

#include "HGPI.h"
#include "HPCB.h"
#include "Library/HygonCalloutLib.h"

// ----------------------------------------------------
//
// Hygon Memory Service Functions
//
// -------------------------------------------------------

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
typedef EFI_STATUS (EFIAPI *PEI_HYGON_SOC_GET_MEMORY_ABOVE_4GB_INTERFACE)(
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
typedef EFI_STATUS (EFIAPI *PEI_HYGON_SOC_GET_MEMORY_BELOW_4GB_INTERFACE)(
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
typedef EFI_STATUS (EFIAPI *PEI_HYGON_SOC_GET_MEMORY_BELOW_1TB_INTERFACE)(
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
typedef EFI_STATUS (EFIAPI *PEI_HYGON_SOC_GET_TOTAL_MEMORY_SIZE_INTERFACE)(
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
typedef EFI_STATUS (EFIAPI *PEI_HYGON_SOC_GET_MEMORY_BOTTOM_IO_INTERFACE)(
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
typedef EFI_STATUS (EFIAPI *PEI_HYGON_SOC_GET_MEMORY_FREQUENCY_INTERFACE)(
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
typedef EFI_STATUS (EFIAPI *PEI_HYGON_SOC_GET_UMA_INFO_INTERFACE)(
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
typedef EFI_STATUS (EFIAPI *PEI_GET_SYSTEM_MEMORY_MAP)(
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN UINT32                     *NumberOfHoles,
  IN UINT64                     *TopOfSystemMemory,
  IN VOID                       **MemHoleDescPtr
  );

// ----------------------------------------------------
//
// Hygon Memory Init Complete Ppi
//
// -------------------------------------------------------
extern EFI_GUID  gHygonMemoryInitCompletePpiGuid;

// ----------------------------------------------------
//
// Hygon Memory UMA Info
//
// -------------------------------------------------------
typedef struct _HYGON_MEMORY_UMA_INFO {
  UMA_MODE    UmaMode; ///< Uma Mode
                       ///< 0 = None
                       ///< 1 = Specified
                       ///< 2 = Auto
  UINT32      UmaSize; ///< The size of shared graphics dram (16-bits)
                       ///< NV_UMA_Size[31:0]=Addr[47:16]
                       ///<
  UINT32      UmaBase;
} HYGON_MEMORY_UMA_INFO;

#define HYGON_MEM_PPI_MAX_SOCKETS_SUPPORTED    4  ///< Max number of sockets in system
#define HYGON_MEM_PPI_MAX_CDDS_PER_SOCKET      8  ///< Max dies per socket
#define HYGON_MEM_PPI_MAX_CHANNELS_PER_SOCKET  12  ///< Max Channels per sockets
#define HYGON_MEM_PPI_MAX_CHANNELS_PER_CDD     3  ///< Max channels per die
#define HYGON_MEM_PPI_MAX_DIMMS_PER_CHANNEL    2  ///< Max dimms per die
// -----------------------------------------------------------------------------
///
/// HPOB SPD Data for each DIMM.
///
#define DDR4_SPD_SIZE  512
#define DDR5_SPD_SIZE  1024

typedef struct _HYGON_MEM_SPD_STRUCT {
  UINT8      SocketNumber;    ///< Indicates the socket number
  UINT8      ChannelNumber;   ///< Indicates the channel number
  UINT8      DimmNumber;      ///< Indicates the channel number
  UINT8      PageAddress;
  BOOLEAN    DimmPresent;     ///< Indicates if the DIMM is present
  UINT8      MuxPresent;      ///< SpdMux Present or not. if 1, then yes otherwise no
  UINT8      MuxI2CAddress;   ///< MuxI2cAddress
  UINT8      MuxChannel;      ///< MuxChannel no.
  UINT32     Address;         ///< SMBus address of the DRAM
  UINT32     SerialNumber;    ///< DIMM Serial Number
  UINT8      *SpdDataPtr;     /// Buffer pointer to SPD Data from DIMM
} HYGON_MEM_SPD_STRUCT;
/// HYGON_MEM_DIMM_SPD_DATA_STRUCT

typedef struct {
  BOOLEAN                 HygonDimmSpdDataSupported; // Flag indicating that Hpob DIMM spd data is supported
  UINT8                   Reserved[3];
  HYGON_MEM_SPD_STRUCT    DimmSpdInfo[HYGON_MEM_PPI_MAX_SOCKETS_SUPPORTED][HYGON_MEM_PPI_MAX_CHANNELS_PER_SOCKET][HYGON_MEM_PPI_MAX_DIMMS_PER_CHANNEL];
} HYGON_MEM_DIMM_SPD_DATA_STRUCT;
// ----------------------------------------------------
//
// Hygon Memory Init Complete Ppi
//
// -------------------------------------------------------
typedef struct _HYGON_MEMORY_INIT_COMPLETE_PPI {
  UINT32                             Revision;                                  ///< revision
  UINT16                             HygonBottomIo;                             ///< Bottom IO
  UINT32                             HygonMemoryBelow4gb;                       ///< Memory below 4G
  UINT32                             HygonMemoryAbove4gb;                       ///< Memory above 4G
  UINT32                             HygonMemoryBelow1Tb;                       ///< Memory below 1T
  UINT32                             HygonTotalMemorySize;                      ///< Total Memory Size
  UINT32                             HoleBase;                                  ///< Hole Base
  UINT32                             Sub4GCacheTop;                             ///< Sub 4G Cache Top
  UINT32                             Sub1THoleBase;                             ///< Sub 1T Hole Base
  UINT32                             SysLimit;                                  ///< System Limit
  UINT32                             HygonMemoryFrequency;                      ///< Memory Frequency
  DIMM_VOLTAGE                       HygonMemoryVddIo;                          ///< Memory Vddio
  VDDP_VDDR_VOLTAGE                  HygonMemoryVddpVddr;                       ///< Memory Vddp Vddr
  HYGON_MEMORY_UMA_INFO              HygonGetUmaInfo;                           ///< HYGON UMA Info
  UINT32                             DdrMaxRate;                                ///< DdrMaxRate
  PEI_GET_SYSTEM_MEMORY_MAP          GetSystemMemoryMap;                        ///< System Memory Map
  HYGON_MEM_DIMM_SPD_DATA_STRUCT     HygonDimmSpInfo;                           ///< Memory SPD info
} HYGON_MEMORY_INIT_COMPLETE_PPI;

#define HYGON_MEMORY_INIT_COMPLETE_REVISION  0x01

// ----------------------------------------------------
//
// Hygon Memory Channel Translation Table Ppi
//
// -------------------------------------------------------
typedef struct _HYGON_MEMORY_CHANNEL_XLAT_PPI {
  UINT32    Revision;                                                                    ///< revision
  VOID      *XLatTab;                                                                    ///< Translation Table
} HYGON_MEMORY_CHANNEL_XLAT_PPI;

#define HYGON_MEMORY_CHANNEL_XLAT_REVISION  0x01

// ----------------------------------------------------
//
// Hygon Memory Platform Configuration Ppi
//
// -------------------------------------------------------
typedef struct _HYGON_HGPI_MEM_PLATFORM_CONFIGURATION_PPI {
  UINT32       Revision;                                                                 ///< revision
  PSO_ENTRY    *PlatformMemoryConfiguration;                                             ///< Platform Memory Configurattion
} HYGON_HGPI_MEM_PLATFORM_CONFIGURATION_PPI;

// ----------------------------------------------------
//
// PEI_HYGON_PLATFORM_DIMM_SPD_PPI data structure definition.
//
// -------------------------------------------------------
typedef struct _PEI_HYGON_PLATFORM_DIMM_SPD_PPI PEI_HYGON_PLATFORM_DIMM_SPD_PPI;

// ----------------------------------------------------
//
// Defines function prototype for PlatformDimmSpdRead
//
// -------------------------------------------------------
typedef
EFI_STATUS
(EFIAPI *PEI_HYGON_DIMM_SPD_READ)(
  IN      EFI_PEI_SERVICES                    **PeiServices, ///< Pointer to PeiServices
  IN struct _PEI_HYGON_PLATFORM_DIMM_SPD_PPI    *This,       ///< Pointer to the PPI structure
  IN OUT  HGPI_READ_SPD_PARAMS               *SpdData        ///< SPD Data Buffer
  );

// ----------------------------------------------------
//
// PEI_HYGON_PLATFORM_DIMM_SPD_PPI data structure definition.
//
// -------------------------------------------------------
struct _PEI_HYGON_PLATFORM_DIMM_SPD_PPI {
  // See the Forward Declaration above
  UINT8                      TotalNumberOfSocket;                     ///< Total Number of Physical Socket.
  UINT8                      TotalNumberOfDimms;                      ///< Total Number of DIMMS
  PEI_HYGON_DIMM_SPD_READ    PlatformDimmSpdRead;                     ///< Function to be called
};

extern EFI_GUID  gHygonMemoryBeforeDramInitPpiGuid;

/**
  Memory Before DRAM Init Interface

  Parameters:
  PeiServices

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
typedef EFI_STATUS (EFIAPI *PEI_HYGON_MEMORY_BEFORE_DRAM_INIT_INTERFACE)(
  IN      EFI_PEI_SERVICES  **PeiServices
  );

// ----------------------------------------------------
//
// Hygon Memory Before Dram Init Ppi
//
// -------------------------------------------------------
typedef struct _PEI_HYGON_MEMORY_BEFORE_DRAM_INIT_PPI {
  UINT32    Revision;                                                                    ///< revision
} PEI_BEFORE_DRAM_INIT_COMPLETE_PPI;

#define HYGON_MEMORY_BEFORE_DRAM_INIT_REVISION  0x00

extern EFI_GUID  gHygonMemoryAfterDramInitPpiGuid;

// ----------------------------------------------------
//
// Hygon Memory After Dram Init Ppi
//
// -------------------------------------------------------
typedef struct _PEI_HYGON_MEMORY_AFTER_DRAM_INIT_PPI {
  UINT32    Revision;                                                                    ///< revision
} PEI_AFTER_DRAM_INIT_COMPLETE_PPI;

#define HYGON_MEMORY_AFTER_DRAM_INIT_REVISION  0x00

extern EFI_GUID  gHygonMemoryInitializeHgpiMemoryPpiGuid;

// ----------------------------------------------------
//
// Hygon Memory Init Ppi
//
// -------------------------------------------------------
typedef struct _PEI_INITIALIZE_HGPI_MEMORY_PPI {
  UINT32    Revision;                                                                    ///< revision
} PEI_INITIALIZE_HGPI_MEMORY_PPI;

#define HYGON_MEMORY_INITIALIZE_HGPI_MEMORY_REVISION  0x00

extern EFI_GUID  gHygonMemoryFamilyServicesPpiGuid;

// ----------------------------------------------------
//
// Hygon Memory Family Services PPI
//
// -------------------------------------------------------
typedef struct _PEI_MEMORY_FAMILY_SERVICE_PPI {
  UINT32                                           Revision;                                ///< revision
  PEI_HYGON_SOC_GET_MEMORY_ABOVE_4GB_INTERFACE     HygonGetMemoryAbove4gbFamilyService;     ///< Service to get memory above 4G
  PEI_HYGON_SOC_GET_MEMORY_BELOW_4GB_INTERFACE     HygonGetMemoryBelow4gbFamilyService;     ///< Service to get memory below 4G
  PEI_HYGON_SOC_GET_MEMORY_BELOW_1TB_INTERFACE     HygonGetMemoryBelow1TbFamilyService;     ///< Service to get memory below 1T
  PEI_HYGON_SOC_GET_MEMORY_BOTTOM_IO_INTERFACE     HygonGetBottomIo1TbFamilyService;        ///< Service to get BottomIo
  PEI_HYGON_SOC_GET_TOTAL_MEMORY_SIZE_INTERFACE    HygonGetTotalMemorySize4gbFamilyService; ///< Service to get total memory
} PEI_MEMORY_FAMILY_SERVICE_PPI;

#define HYGON_MEMORY_FAMILY_SERVICE_REVISION  0x00

//
// GUID definition
//

extern EFI_GUID  gHygonMemoryPoolPointerTablePpiGuid;

// Current PPI revision
#define HYGON_MEM_POOL_PTR_TABLE_REVISION  0x00

extern EFI_GUID  gHygonMemoryTechServicesPpiGuid;

// ----------------------------------------------------
//
// HYGON memory tech Services PPI
//
// -------------------------------------------------------
typedef EFI_STATUS (EFIAPI *PEI_HYGON_MEM_TECH_SERVICES_INTERFACE)(
  );
typedef struct _HYGON_MEM_TECH_SERVICES_PPI {
  UINTN                                    Revision;                 ///< Revision Number
  PEI_HYGON_MEM_TECH_SERVICES_INTERFACE    HygonDimmPresenceService; ///< Service to detect DIMM presence
} HYGON_MEM_TECH_SERVICES_PPI;

// Current PPI revision
#define HYGON_MEM_TECH_SERVICES_REVISION  0x00

extern EFI_GUID  gHygonMemoryTecnologyPpiGuid;

// ----------------------------------------------------
//
// HYGON memory tech PPI
//
// -------------------------------------------------------
typedef struct _PEI_MEMORY_TECHNOLOGY_PPI {
  UINTN    Revision;                          ///< Revision Number
} PEI_MEMORY_TECHNOLOGY_PPI;

// Current PPI revision
#define HYGON_MEMORY_TECHNOLOGY_REVISION  0x00

extern EFI_GUID  gHygonMemoryHgpiReadSpdPpiGuid;

typedef EFI_STATUS (EFIAPI *PEI_HYGON_MEMORY_HGPI_READ_SPD_INTERFACE)(
  );
// ----------------------------------------------------
//
// HYGON memory Read SPD PPI
//
// -------------------------------------------------------
typedef struct _PEI_MEMORY_HGPI_READ_SPD_PPI {
  UINTN                                       Revision;     ///< Revision Number
  PEI_HYGON_MEMORY_HGPI_READ_SPD_INTERFACE    HgpiReadSpd;  ///< Service to read SPD
} PEI_MEMORY_HGPI_READ_SPD_PPI;

// Current PPI revision
#define HYGON_MEMORY_HGPI_READ_SPD_REVISION  0x00

// ----------------------------------------------------
//
// Defines function prototype to install Memory feature block
//
// -------------------------------------------------------
typedef
EFI_STATUS(*HYGON_MEM_FEAT_ISNTALL_INTERFACE) (
  IN OUT     VOID *MemFeatBlock                           ///< Memory feature block
  );
// ----------------------------------------------------
//
// HYGON memory Install Feature PPI
//
// -------------------------------------------------------
typedef struct _HYGON_HGPI_MEM_FEAT_INSTALL_PPI {
  UINT32                              Revision;                     ///< revision
  HYGON_MEM_FEAT_ISNTALL_INTERFACE    Install;                      ///< Service to install memory feature block
} HYGON_HGPI_MEM_FEAT_INSTALL_PPI;

// Current PPI revision
#define HYGON_MEMORY_FEAT_INSTALL_REVISION  0x00

#endif
