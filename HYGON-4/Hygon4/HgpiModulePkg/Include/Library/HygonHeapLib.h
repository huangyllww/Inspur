/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Heap related functions.
 *
 * Contains code that initialize, maintain, and allocate the heap space.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  LIB
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _HYGON_HEAP_LIB_H_
#define _HYGON_HEAP_LIB_H_

/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */
/// Heap Manager Life cycle (Persist)
/// For compatile still support previous Persist definition, but will keep the data through boot time for all persist type
/// And keep runtime persist for HEAP_RUNTIME_SYSTEM_MEM type.
/// Suggest use HEAP_BOOTTIME_SYSTEM_MEM for boot time required, and HEAP_RUNTIME_SYSTEM_MEM for runtime required
#define HEAP_DO_NOT_EXIST_YET      1
#define HEAP_LOCAL_CACHE           2
#define HEAP_TEMP_MEM              3
#define HEAP_SYSTEM_MEM            4
#define HEAP_DO_NOT_EXIST_ANYMORE  5
#define HEAP_S3_RESUME             6
#define HEAP_BOOTTIME_SYSTEM_MEM   7
#define HEAP_RUNTIME_SYSTEM_MEM    8

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
#pragma pack (push, 1)
/// Allocate Heap Parameters
typedef struct _ALLOCATE_HEAP_PARAMS {
  UINT32    RequestedBufferSize;                  ///< Size of buffer.
  UINT32    BufferHandle;                         ///< An unique ID of buffer.
  UINT8     Persist;                              ///< Persist Flag, refer to the Macro HEAP_X definition above
  UINT8     *BufferPtr;                           ///< Pointer to buffer.
} ALLOCATE_HEAP_PARAMS;

/// Locate Heap Parameters
typedef struct _LOCATE_HEAP_PTR {
  UINT32    BufferHandle;                         ///< An unique ID of buffer.
  UINT32    BufferSize;                           ///< Data buffer size.
  UINT8     *BufferPtr;                           ///< Pointer to buffer.
} LOCATE_HEAP_PTR;
#pragma pack (pop)

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */

// HGPI_STATUS
// HeapManagerInit (
// IN       HYGON_CONFIG_PARAMS *StdHeader
// );

HGPI_STATUS
HeapAllocateBuffer (
  IN OUT   ALLOCATE_HEAP_PARAMS *AllocateHeapParams,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

HGPI_STATUS
HeapDeallocateBuffer (
  IN       UINT32 BufferHandle,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  );

HGPI_STATUS
HeapLocateBuffer (
  IN OUT   LOCATE_HEAP_PTR *LocateHeap,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  );

/// HGPI Buffer Handles (These are reserved)
typedef enum {
  HYGON_INIT_RESET_HANDLE = 0x000A000,              ///< Assign 0x000A000 buffer handle to HygonInitReset routine.
  HYGON_INIT_EARLY_HANDLE,                          ///< Assign 0x000A001 buffer handle to HygonInitEarly routine.
  HYGON_INIT_POST_HANDLE,                           ///< Assign 0x000A002 buffer handle to HygonInitPost routine.
  HYGON_INIT_ENV_HANDLE,                            ///< Assign 0x000A003 buffer handle to HygonInitEnv routine.
  HYGON_INIT_MID_HANDLE,                            ///< Assign 0x000A004 buffer handle to HygonInitMid routine.
  HYGON_INIT_LATE_HANDLE,                           ///< Assign 0x000A005 buffer handle to HygonInitLate routine.
  HYGON_INIT_RESUME_HANDLE,                         ///< Assign 0x000A006 buffer handle to HygonInitResume routine.
  HYGON_LATE_RUN_AP_TASK_HANDLE,                    ///< Assign 0x000A007 buffer handle to HygonLateRunApTask routine.
  HYGON_INIT_RTB_HANDLE,                            ///< Assign 0x000A008 buffer handle to HygonInitRtb routine.
  HYGON_S3_LATE_RESTORE_HANDLE,                     ///< Assign 0x000A009 buffer handle to HygonS3LateRestore routine.
  HYGON_S3_SCRIPT_SAVE_TABLE_HANDLE,                ///< Assign 0x000A00A buffer handle to be used for S3 save table
  HYGON_S3_SCRIPT_TEMP_BUFFER_HANDLE,               ///< Assign 0x000A00B buffer handle to be used for S3 save table
  HYGON_CPU_AP_TASKING_HANDLE,                      ///< Assign 0x000A00C buffer handle to AP tasking input parameters.
  HYGON_REC_MEM_SOCKET_HANDLE,                      ///< Assign 0x000A00D buffer handle to save socket with memory in memory recovery mode.
  HYGON_MEM_AUTO_HANDLE,                            ///< Assign 0x000A00E buffer handle to HygonMemAuto routine.
  HYGON_MEM_SPD_HANDLE,                             ///< Assign 0x000A00F buffer handle to HygonMemSpd routine.
  HYGON_MEM_DATA_HANDLE,                            ///< Assign 0x000A010 buffer handle to MemData
  HYGON_MEM_TRAIN_BUFFER_HANDLE,                    ///< Assign 0x000A011 buffer handle to allocate buffer for training
  HYGON_MEM_S3_DATA_HANDLE,                         ///< Assign 0x000A012 buffer handle to special case register for S3
  HYGON_MEM_S3_NB_HANDLE,                           ///< Assign 0x000A013 buffer handle to NB block for S3
  HYGON_MEM_S3_MR0_DATA_HANDLE,                     ///< Assign 0x000A014 buffer handle to MR0 data block for S3
  HYGON_UMA_INFO_HANDLE,                            ///< Assign 0x000A015 buffer handle to be used for Uma information
  HYGON_DMI_MEM_DEV_INFO_HANDLE,                    ///< Assign 0x000A016 buffer handle to DMI Type16 17 19 20 information
  EVENT_LOG_BUFFER_HANDLE,                        ///< Assign 0x000A017 buffer handle to Event Log
  IDS_CONTROL_HANDLE,                             ///< Assign 0x000A018 buffer handle to HygonIds routine.
  IDS_HDT_OUT_BUFFER_HANDLE,                      ///< Assign 0x000A019 buffer handle to be used for HDTOUT support.
  IDS_CHECK_POINT_PERF_HANDLE,                    ///< Assign 0x000A01A buffer handle to Performance analysis
  HYGON_PCIE_COMPLEX_DATA_HANDLE,                   ///< Assign 0x000A01B buffer handle to be used for PCIe support
  HYGON_MEM_SYS_DATA_HANDLE,                        ///< Assign 0x000A01C buffer handle to be used for memory data structure
  HYGON_GNB_SMU_CONFIG_HANDLE,                      ///< Assign 0x000A01D buffer handle to be used for GNB SMU configuration
  HYGON_PP_FUSE_TABLE_HANDLE,                       ///< Assign 0x000A01E buffer handle to be used for TT fuse table
  HYGON_GFX_PLATFORM_CONFIG_HANDLE,                 ///< Assign 0x000A01F buffer handle to be used for Gfx platform configuration
  HYGON_GNB_TEMP_DATA_HANDLE,                       ///< Assign 0x000A020 buffer handle for GNB general purpose data block
  HYGON_MEM_2D_RDQS_HANDLE,                         ///< Assign 0x000A021 buffer handle for 2D training
  HYGON_MEM_2D_RD_WR_HANDLE,                        ///< Assign 0x000A022 buffer handle for 2D Read/Write training
  HYGON_GNB_IOMMU_SCRATCH_MEM_HANDLE,               ///< Assign 0x000A023 buffer handle to be used for GNB IOMMU scratch memory
  HYGON_MEM_S3_SAVE_HANDLE,                         ///< Assign 0x000A024 buffer handle for memory data saved right after memory init
  HYGON_MEM_2D_RDQS_RIM_HANDLE,                     ///< Assign 0x000A025 buffer handle for 2D training Eye RIM Search
  HYGON_MEM_2D_RD_WR_RIM_HANDLE,                    ///< Assign 0x000A026 buffer handle for 2D Read/Write training Eye RIM Search
  HYGON_CPU_NB_PSTATE_FIXUP_HANDLE,                 ///< Assign 0x000A027 buffer handle for an NB P-state workaround
  HYGON_MEM_CRAT_INFO_BUFFER_HANDLE,                ///< Assign 0x000A028 buffer handle for CRAT Memory affinity component structure
  HYGON_SKIP_MEM_S3_SAVE,                           ///< Assign 0x000A029 buffer handle for the flag to skip memory S3 save
  HYGON_IS_FEATURE_ENABLED,                         ///< Assign 0x000A02A buffer handle for keeping the result of IsFeatureEnabled
  HYGON_MEM_DATAEYE_WORK_AREA_HANDLE,               ///< Assign 0x000A02B buffer handle for Composite Data Eye Compression Work Area
  HYGON_GNB_SAMU_PATCH_HANDLE,                      ///< Assign 0x000A02C buffer handle for Samu patch buffer
  HYGON_GNB_SAMU_BOOT_CONTROL_HANDLE,               ///< Assign 0x000A02D buffer handle for Samu boot control buffer
  HYGON_GNB_ACP_ENGINE_HANDLE,                      ///< Assign 0x000A02E buffer handle for GNB ACP engine buffer
  HYGON_MEM_PMU_SRAM_MSG_BLOCK_HANDLE,              ///< Assign 0x000A02F buffer handle for PMU SRAM Message Block buffer
  HYGON_MEM_DRAM_CAD_BUS_CONFIG_HANDLE,             ///< Assign 0x000A030 buffer handle for DRAM CAD Bus Configuration
  HYGON_GNB_SMU_TABLE_HANDLE,                       ///< Assign 0x000A031 buffer handle for GNB SMU table buffer
  HYGON_GNB_CRAT_HSA_TABLE_HANDLE,                  ///< Assign 0x000A032 buffer handle for GNB CRAT HSA unit table
  HYGON_GNB_BUILD_OPTIONS_HANDLE,                   ///< Assign 0x000A033 buffer handle for GNB build options
  HYGON_S3_FINAL_RESTORE_HANDLE,                    ///< Assign 0x000A034 buffer handle to HygonS3FinalRestore routine.
  HYGON_GNB_PCIE_AER_CONFIG_HANDLE,                 ///< Assign 0x000A035 buffer handle for GNB PCIE AER configuration.
  HYGON_BTC_SCRATCH_HANDLE,                         ///< Assign 0x000A036 buffer handle for boot time calibration workloads
  HYGON_BTC_XMM_SAVE_HANDLE,                        ///< Assign 0x000A037 buffer handle for boot time calibration xmm register save
  HYGON_S3_SAVE_HANDLE,                             ///< Assign 0x000A038 buffer handle to memory context data
  HYGON_MEM_DDR_MAX_RATE_HANDLE,                    ///< Assign 0x000A039 buffer handle to memory DDR max rate
  HYGON_GNB_TDP_HANDLE,                             ///< Assign 0x000A03A buffer handle to GNB TDP
  HYGON_FABRIC_PSTATE_LIB_HANDLE,                   ///< Assign 0x000A03B buffer handle to Fabric Pstate lib
  HYGON_ACPI_TABLE_BUFFER_HANDLE,                   ///< Assign 0x000A03C buffer handle to ACPI Tables
  HYGON_MMIO_MANAGER,                               ///< Assign 0x000A03D buffer handle to DF MMIO manager
  HYGON_IO_MANAGER,                                 ///< Assign 0x000A03E buffer handle to DF IO manager
  HYGON_PFEH_HANDLE,                                ///< Assign 0x000A03F buffer handle to PFEH mode
  HYGON_RBPRESENT_HANDLE,                           ///< Assign 0x000A040 buffer handle to RB Present
  HYGON_CXL_MANAGER,                                ///< Assign 0x000A041 buffer handle to DF CXL manager
  HYGON_ACPI_CHBS_BUFFER_HANDLE,                    ///< Assign 0x000A042 buffer handle to CEDT table
  HYGON_MEM_MISC_HANDLES_START      = 0x1000000,    ///< Reserve 0x1000000 to 0x1FFFFFF buffer handle
  HYGON_MEM_MISC_HANDLES_END        = 0x1FFFFFF,    ///< miscellaneous memory init tasks' buffers.
  HYGON_HEAP_IN_MAIN_MEMORY_HANDLE  = 0x8000000,    ///< Assign 0x8000000 to HYGON_HEAP_IN_MAIN_MEMORY_HANDLE.
  SOCKET_DIE_MAP_HANDLE           = 0x534F4B54,   ///< 'sokt'
  NODE_ID_MAP_HANDLE              = 0x4E4F4445,   ///< 'node'
  HOP_COUNT_TABLE_HANDLE          = 0x484F5053,   ///< 'hops'
  HYGON_FCH_RESET_DATA_BLOCK_HANDLE = 0x46434852,   ///< 'FCHR' Buffer handle for FCH private data block at InitReset
  HYGON_FCH_DATA_BLOCK_HANDLE       = 0x46434845,   ///< 'FCHE' Buffer handle for FCH private data block at InitEnv
  IDS_TRAP_TABLE_HANDLE           = 0x49524547,   ///< 'IREG' Handle for IDS register table
  IDS_SAVE_IDTR_HANDLE            = 0x49445452,   ///< 'IDTR'
  IDS_BSC_IDT_HANDLE              = 0x42534349,   ///< 'BSCI' BSC Idt table
  IDS_NV_TO_CMOS_HANDLE           = 0x534D4349,   ///< 'ICMS' Handle for IDS CMOS save
  IDS_GRA_HANDLE                  = 0x41524749,   ///< 'IGRA' Handle for IDS GRA save
  IDS_EXTEND_HANDLE               = 0x54584549,   ///< 'IEXT' Handle for IDS extend module
  IDS_TEMP_DATA_HANDLE            = 0x504D5459,   ///< 'ITMP' Handle for IDS temp data
  IDS_NV_TABLE_DATA_HANDLE        = 0x4254564E,   ///< 'NVTB' Handle for IDS NV table data
} HGPI_BUFFER_HANDLE;

#endif // _HYGON_HEAP_LIB_H_
