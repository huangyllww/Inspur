/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR> 
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced, 
stored in a retrieval system, or transmitted in any form or by any 
means without the express written consent of Byosoft Corporation.

File Name:
  spiflashdevice.c

Abstract: 

Revision History:

**/
#include <PiSmm.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/PcdLib.h>
#include <SpiCmd.h>


#define SIZEOF_4K                        12
#define RECORD_ENTRY_4K(Count)           { SIZEOF_4K, Count }
#define RECORD_ENTRY_64M_OF_4K_BLOCKS    RECORD_ENTRY_4K(128 * 16)
#define RECORD_ENTRY_128M_OF_4K_BLOCKS   RECORD_ENTRY_4K(256 * 16)
#define RECORD_ENTRY_256M_OF_4K_BLOCKS   RECORD_ENTRY_4K(512 * 16)
#define RECORD_ENTRY_512M_OF_4K_BLOCKS   RECORD_ENTRY_4K(1024 * 16)
#define RECORD_MAP_END                   { 0, 0 }


STATIC MEDIA_BLOCK_MAP gFlashMap64M_4K[] = {
    RECORD_ENTRY_64M_OF_4K_BLOCKS,
    RECORD_MAP_END
};

STATIC MEDIA_BLOCK_MAP gFlashMap128M_4K[] = {
    RECORD_ENTRY_128M_OF_4K_BLOCKS,
    RECORD_MAP_END
};

STATIC MEDIA_BLOCK_MAP gFlashMap256M_4K[] = {
    RECORD_ENTRY_256M_OF_4K_BLOCKS,
    RECORD_MAP_END
};

STATIC MEDIA_BLOCK_MAP gFlashMap512M_4K[] = {
    RECORD_ENTRY_512M_OF_4K_BLOCKS,
    RECORD_MAP_END
};

STATIC NV_DEVICE_INSTANCE   mNvDevice = {
  NV_DEVICE_DATA_SIGNATURE,
  NULL,                           // Handle
  {
    device_info,
    device_sense,
    devcie_read,
    device_write,
    device_erase,
    device_lock
  },
  NULL,                           // SpiProtocol
  NULL,                           // PlatformAccessProtocol
  NULL,                           // BlockMap
  NULL,                           // InitTable
  1,                              // Number
  0,                              // FlashSize
  0                               // SectorSize
};


typedef struct {
  SPI_INIT_TABLE   InitTable;
  UINTN            SectorSize;  
  MEDIA_BLOCK_MAP  *BlockMap;
} MY_SPI_INIT_TABLE;

STATIC MY_SPI_INIT_TABLE gMySpiInitTableList[] = {
// MX25L512
  {
    {
      SF_VENDOR_ID_MXIC,
      SF_DEVICE_ID0_MX25L512,
      SF_DEVICE_ID1_MX25L512,
      {
        SF_INST_WREN,
        SF_INST_EWSR
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle50MHz,  EnumSpiOperationJedecId},             // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle33MHz,  EnumSpiOperationReadData},            // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle50MHz,  EnumSpiOperationReadStatus},          // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle50MHz,  EnumSpiOperationWriteDisable},        // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle50MHz,  EnumSpiOperationErase_4K_Byte},       // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle50MHz,  EnumSpiOperationErase_64K_Byte},      // Opcode 5: Block Erase (64KB)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle50MHz,  EnumSpiOperationProgramData_1_Byte},  // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle50MHz,  EnumSpiOperationWriteStatus},         // Opcode 7: Write Status Register
      },
      0,
      0,
      MXIC_MX25L512_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap512M_4K
  },

// W25Q256B
  {
    {
      SF_VENDOR_ID_WINBOND,
      SF_DEVICE_ID0_W25Q256B,
      SF_DEVICE_ID1_W25Q256B,
      {
        SF_INST_WREN,
        SF_INST_EWSR
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle50MHz,  EnumSpiOperationJedecId},             // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle50MHz,  EnumSpiOperationReadData},            // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle50MHz,  EnumSpiOperationReadStatus},          // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle50MHz,  EnumSpiOperationWriteDisable},        // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle50MHz,  EnumSpiOperationErase_4K_Byte},       // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle50MHz,  EnumSpiOperationErase_64K_Byte},      // Opcode 5: Block Erase (64KB)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle50MHz,  EnumSpiOperationProgramData_1_Byte},  // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle50MHz,  EnumSpiOperationWriteStatus},         // Opcode 7: Write Status Register
      },
      0,
      0,
      WINBOND_W25Q256B_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap256M_4K
  },

// MX25L256
  {
    {
      SF_VENDOR_ID_MXIC,
      SF_DEVICE_ID0_MX25L256,
      SF_DEVICE_ID1_MX25L256,
      {
        SF_INST_WREN,
        SF_INST_EWSR
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle50MHz,  EnumSpiOperationJedecId},             // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle33MHz,  EnumSpiOperationReadData},            // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle50MHz,  EnumSpiOperationReadStatus},          // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle50MHz,  EnumSpiOperationWriteDisable},        // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle50MHz,  EnumSpiOperationErase_4K_Byte},       // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle50MHz,  EnumSpiOperationErase_64K_Byte},      // Opcode 5: Block Erase (64KB)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle50MHz,  EnumSpiOperationProgramData_1_Byte},  // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle50MHz,  EnumSpiOperationWriteStatus},         // Opcode 7: Write Status Register
      },
      0,
      0,
      MXIC_MX25L256_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap256M_4K
  },

// MICRON_N25Q0256
  {
    {
      SF_VENDOR_ID_MICRON,
      SF_DEVICE_ID0_N25Q256,
      SF_DEVICE_ID1_N25Q256,
      {
        SF_INST_WREN,
        SF_INST_WREN
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle20MHz, EnumSpiOperationJedecId      },      // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle20MHz, EnumSpiOperationReadData      },      // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle20MHz, EnumSpiOperationReadStatus    },      // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle20MHz, EnumSpiOperationWriteDisable    },      // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle20MHz, EnumSpiOperationErase_4K_Byte  },      // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle20MHz, EnumSpiOperationErase_64K_Byte  },      // Opcode 5: Block Erase (64KB
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle20MHz, EnumSpiOperationProgramData_1_Byte},     // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle20MHz, EnumSpiOperationWriteStatus    },      // Opcode 7: Write Status Register
      },
      0,
      0,
      MICRON_N25Q256_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap256M_4K
  },

// W25Q64B  
  {
    {
      SF_VENDOR_ID_WINBOND,
      SF_DEVICE_ID0_W25Q64B,
      SF_DEVICE_ID1_W25Q64B,
      {
        SF_INST_WREN,
        SF_INST_EWSR
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle50MHz,  EnumSpiOperationJedecId},             // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle50MHz,  EnumSpiOperationReadData},            // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle50MHz,  EnumSpiOperationReadStatus},          // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle50MHz,  EnumSpiOperationWriteDisable},        // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle50MHz,  EnumSpiOperationErase_4K_Byte},       // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle50MHz,  EnumSpiOperationErase_64K_Byte},      // Opcode 5: Block Erase (64KB)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle50MHz,  EnumSpiOperationProgramData_1_Byte},  // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle50MHz,  EnumSpiOperationWriteStatus},         // Opcode 7: Write Status Register
      },
      0,                        // BiosStartOffset
      0,                        // BiosSize
      WINBOND_W25Q64B_SIZE      // FlashSize
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap64M_4K
  },


// SST_26VF064B
  {
    {
      SF_VENDOR_ID_SST,
      SF_DEVICE_ID0_26VF064B,
      SF_DEVICE_ID1_26VF064B,
      {
        SF_INST_WREN,       // Prefix Opcode 0: Write Enable
        SF_INST_NOP         // Prefix Opcode 1: no operation
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle104MHz,  EnumSpiOperationJedecId},                // Opcode 0: Read ID
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_ULBPR,          EnumSpiCycle104MHz,  EnumSpiOperationGlobalBlockProtectionUnlock},   // Opcode 1: unlock
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle104MHz,  EnumSpiOperationReadStatus},             // Opcode 2: Read Status Register 
        {EnumSpiOpcodeReadNoAddr,     SF_INST_WRDI,           EnumSpiCycle104MHz,  EnumSpiOperationWriteDisable},           // Opcode 3: Read block protection Register for replace WRDI
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle104MHz,  EnumSpiOperationErase_4K_Byte},          // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle104MHz,  EnumSpiOperationErase_64K_Byte},         // Opcode 5: Block Erase (8KB/32KB/64KB depending on address)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle104MHz,  EnumSpiOperationProgramData_1_Byte},     // Opcode 6: Page Program 
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle104MHz,  EnumSpiOperationWriteStatus},            // Opcode 7: Write Status Register
      },
      0,
      0,
      SST_26VF064B_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap64M_4K
  },


// SST_25VF064C
  {
    {
      SF_VENDOR_ID_SST,
      SF_DEVICE_ID0_25VF064C,
      SF_DEVICE_ID1_25VF064C,
      {
        SF_INST_WREN,
        SF_INST_EWSR
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle50MHz,  EnumSpiOperationJedecId},             // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle33MHz,  EnumSpiOperationReadData},            // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle50MHz,  EnumSpiOperationReadStatus},          // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle50MHz,  EnumSpiOperationWriteDisable},        // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle50MHz,  EnumSpiOperationErase_4K_Byte},       // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle50MHz,  EnumSpiOperationErase_64K_Byte},      // Opcode 5: Block Erase (64KB)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle50MHz,  EnumSpiOperationProgramData_64_Byte}, // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle50MHz,  EnumSpiOperationWriteStatus},         // Opcode 7: Write Status Register
      },
      0,
      0,
      SST_25VF064C_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap64M_4K
  },



// STM25P64
  {
    {
      SF_VENDOR_ID_ST,
      SF_DEVICE_ID0_STM25P64,
      SF_DEVICE_ID1_STM25P64,
      {
        SF_INST_WREN,
        SF_INST_NOP
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle50MHz,  EnumSpiOperationJedecId},             // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle50MHz,  EnumSpiOperationReadData},            // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle50MHz,  EnumSpiOperationReadStatus},          // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle50MHz,  EnumSpiOperationWriteDisable},        // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE_D8,      EnumSpiCycle50MHz,  EnumSpiOperationErase_4K_Byte},       // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE_C7,  EnumSpiCycle50MHz,  EnumSpiOperationErase_64K_Byte},      // Opcode 5: Block Erase (64KB)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle50MHz,  EnumSpiOperationProgramData_1_Byte},  // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle50MHz,  EnumSpiOperationWriteStatus},         // Opcode 7: Write Status Register
      },
      0,
      0,
      WINBOND_STM25P64_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap64M_4K
  },   


// NUMONYX_N25Q064
  {
    {
      SF_VENDOR_ID_NUMONYX,
      SF_DEVICE_ID0_N25Q064,
      SF_DEVICE_ID1_N25Q064,
      {
        SF_INST_WREN,
        SF_INST_WREN
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle20MHz, EnumSpiOperationJedecId      },      // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle20MHz, EnumSpiOperationReadData      },      // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle20MHz, EnumSpiOperationReadStatus    },      // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle20MHz, EnumSpiOperationWriteDisable    },      // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle20MHz, EnumSpiOperationErase_4K_Byte  },      // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle20MHz, EnumSpiOperationErase_64K_Byte  },      // Opcode 5: Block Erase (64KB
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle20MHz, EnumSpiOperationProgramData_1_Byte},     // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle20MHz, EnumSpiOperationWriteStatus    },      // Opcode 7: Write Status Register
      },
      0,
      0,
      NUMONYX_N25Q064_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap64M_4K
  },   


// MX25L64
  {
    {
      SF_VENDOR_ID_MXIC,
      SF_DEVICE_ID0_MX25L64,
      SF_DEVICE_ID1_MX25L64,
      {
        SF_INST_WREN,
        SF_INST_EWSR
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle50MHz,  EnumSpiOperationJedecId},             // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle33MHz,  EnumSpiOperationReadData},            // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle50MHz,  EnumSpiOperationReadStatus},          // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle50MHz,  EnumSpiOperationWriteDisable},        // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle50MHz,  EnumSpiOperationErase_4K_Byte},       // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle50MHz,  EnumSpiOperationErase_64K_Byte},      // Opcode 5: Block Erase (64KB)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle50MHz,  EnumSpiOperationProgramData_1_Byte},  // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle50MHz,  EnumSpiOperationWriteStatus},         // Opcode 7: Write Status Register
      },
      0,
      0,
      MXIC_MX25L64_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap64M_4K
  },           



// MICRON_N25Q064
  {
    {
      SF_VENDOR_ID_MICRON,
      SF_DEVICE_ID0_M_N25Q064,
      SF_DEVICE_ID1_M_N25Q064,
      {
        SF_INST_WREN,
        SF_INST_WREN
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle20MHz, EnumSpiOperationJedecId      },      // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle20MHz, EnumSpiOperationReadData      },      // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle20MHz, EnumSpiOperationReadStatus    },      // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle20MHz, EnumSpiOperationWriteDisable    },      // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle20MHz, EnumSpiOperationErase_4K_Byte  },      // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle20MHz, EnumSpiOperationErase_64K_Byte  },      // Opcode 5: Block Erase (64KB
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle20MHz, EnumSpiOperationProgramData_1_Byte},     // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle20MHz, EnumSpiOperationWriteStatus    },      // Opcode 7: Write Status Register
      },
      0,
      0,
      MICRON_N25Q064_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap64M_4K
  },  



// W25Q128B
  {
    {
      SF_VENDOR_ID_WINBOND,
      SF_DEVICE_ID0_W25Q128B,
      SF_DEVICE_ID1_W25Q128B,
      {
        SF_INST_WREN,
        SF_INST_EWSR
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle50MHz,  EnumSpiOperationJedecId},             // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle50MHz,  EnumSpiOperationReadData},            // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle50MHz,  EnumSpiOperationReadStatus},          // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle50MHz,  EnumSpiOperationWriteDisable},        // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle50MHz,  EnumSpiOperationErase_4K_Byte},       // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle50MHz,  EnumSpiOperationErase_64K_Byte},      // Opcode 5: Block Erase (64KB)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle50MHz,  EnumSpiOperationProgramData_1_Byte},  // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle50MHz,  EnumSpiOperationWriteStatus},         // Opcode 7: Write Status Register
      },
      0,
      0,
      WINBOND_W25Q128B_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap128M_4K
  },  



// MX25L128
  {
    {
      SF_VENDOR_ID_MXIC,
      SF_DEVICE_ID0_MX25L128,
      SF_DEVICE_ID1_MX25L128,
      {
        SF_INST_WREN,
        SF_INST_EWSR
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle50MHz,  EnumSpiOperationJedecId},             // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle33MHz,  EnumSpiOperationReadData},            // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle50MHz,  EnumSpiOperationReadStatus},          // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle50MHz,  EnumSpiOperationWriteDisable},        // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle50MHz,  EnumSpiOperationErase_4K_Byte},       // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle50MHz,  EnumSpiOperationErase_64K_Byte},      // Opcode 5: Block Erase (64KB)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle50MHz,  EnumSpiOperationProgramData_1_Byte},  // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle50MHz,  EnumSpiOperationWriteStatus},         // Opcode 7: Write Status Register
      },
      0,
      0,
      MXIC_MX25L128_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap128M_4K
  },    



// MICRON_N25Q0128
  {
    {
      SF_VENDOR_ID_MICRON,
      SF_DEVICE_ID0_N25Q128,
      SF_DEVICE_ID1_N25Q128,
      {
        SF_INST_WREN,
        SF_INST_WREN
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle20MHz, EnumSpiOperationJedecId      },      // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle20MHz, EnumSpiOperationReadData      },      // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle20MHz, EnumSpiOperationReadStatus    },      // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle20MHz, EnumSpiOperationWriteDisable    },      // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle20MHz, EnumSpiOperationErase_4K_Byte  },      // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle20MHz, EnumSpiOperationErase_64K_Byte  },      // Opcode 5: Block Erase (64KB
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle20MHz, EnumSpiOperationProgramData_1_Byte},     // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle20MHz, EnumSpiOperationWriteStatus    },      // Opcode 7: Write Status Register
      },
      0,
      0,
      MICRON_N25Q128_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap128M_4K
  }, 


// GD25Q64C  
  {
    {
      SF_VENDOR_ID_GD,
      SF_DEVICE_ID0_25Q64C,
      SF_DEVICE_ID1_25Q64C,
      {
        SF_INST_WREN,
        SF_INST_EWSR
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle50MHz,  EnumSpiOperationJedecId},             // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle50MHz,  EnumSpiOperationReadData},            // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle50MHz,  EnumSpiOperationReadStatus},          // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle50MHz,  EnumSpiOperationWriteDisable},        // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle50MHz,  EnumSpiOperationErase_4K_Byte},       // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle50MHz,  EnumSpiOperationErase_64K_Byte},      // Opcode 5: Block Erase (64KB)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle50MHz,  EnumSpiOperationProgramData_1_Byte},  // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle50MHz,  EnumSpiOperationWriteStatus},         // Opcode 7: Write Status Register
      },
      0,
      0,
      GDW25Q64C_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap64M_4K    
  },

// GD25B256D  
  {
    {
      SF_VENDOR_ID_GD,
      SF_DEVICE_ID0_25B256D,
      SF_DEVICE_ID1_25B256D,
      {
        SF_INST_WREN,
        SF_INST_EWSR
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle50MHz,  EnumSpiOperationJedecId},             // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle50MHz,  EnumSpiOperationReadData},            // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle50MHz,  EnumSpiOperationReadStatus},          // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle50MHz,  EnumSpiOperationWriteDisable},        // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle50MHz,  EnumSpiOperationErase_4K_Byte},       // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle50MHz,  EnumSpiOperationErase_64K_Byte},      // Opcode 5: Block Erase (64KB)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle50MHz,  EnumSpiOperationProgramData_1_Byte},  // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle50MHz,  EnumSpiOperationWriteStatus},         // Opcode 7: Write Status Register
      },
      0,
      0,
      GD25B256D_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap256M_4K    
  },


// GD25B512ME 
/*  {
    {
      SF_VENDOR_ID_GD,
      SF_DEVICE_ID0_25Q64C,
      SF_DEVICE_ID1_25Q64C,
      {
        SF_INST_WREN,
        SF_INST_EWSR
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle50MHz,  EnumSpiOperationJedecId},             // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle50MHz,  EnumSpiOperationReadData},            // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle50MHz,  EnumSpiOperationReadStatus},          // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle50MHz,  EnumSpiOperationWriteDisable},        // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle50MHz,  EnumSpiOperationErase_4K_Byte},       // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle50MHz,  EnumSpiOperationErase_64K_Byte},      // Opcode 5: Block Erase (64KB)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle50MHz,  EnumSpiOperationProgramData_1_Byte},  // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle50MHz,  EnumSpiOperationWriteStatus},         // Opcode 7: Write Status Register
      },
      0,
      0,
      GD25B512ME_SIZE
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap512M_4K    
  },*/


// XM25QH64B  
  {
    {
      SF_VENDOR_ID_XM,
      SF_DEVICE_ID0_XM_25QH64B,
      SF_DEVICE_ID1_XM_25QH64B,
      {
        SF_INST_WREN,
        SF_INST_NOP
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle50MHz,  EnumSpiOperationJedecId},             // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle50MHz,  EnumSpiOperationReadData},            // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle50MHz,  EnumSpiOperationReadStatus},          // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle50MHz,  EnumSpiOperationWriteDisable},        // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle50MHz,  EnumSpiOperationErase_4K_Byte},       // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle50MHz,  EnumSpiOperationErase_64K_Byte},      // Opcode 5: Block Erase (64KB)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle50MHz,  EnumSpiOperationProgramData_1_Byte},  // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle50MHz,  EnumSpiOperationWriteStatus},         // Opcode 7: Write Status Register
      },
      0,                        // BiosStartOffset
      0,                        // BiosSize
      XM25QH64B_SIZE            // FlashSize
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap64M_4K
  },


// XM25QH128B  
  {
    {
      SF_VENDOR_ID_XM,
      SF_DEVICE_ID0_XM_25QH128B,
      SF_DEVICE_ID1_XM_25QH128B,
      {
        SF_INST_WREN,
        SF_INST_NOP
      },
      {
        {EnumSpiOpcodeReadNoAddr,     SF_INST_JEDEC_READ_ID,  EnumSpiCycle50MHz,  EnumSpiOperationJedecId},             // Opcode 0: Read ID
        {EnumSpiOpcodeRead,           SF_INST_READ,           EnumSpiCycle50MHz,  EnumSpiOperationReadData},            // Opcode 1: Read
        {EnumSpiOpcodeReadNoAddr,     SF_INST_RDSR,           EnumSpiCycle50MHz,  EnumSpiOperationReadStatus},          // Opcode 2: Read Status Register
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRDI,           EnumSpiCycle50MHz,  EnumSpiOperationWriteDisable},        // Opcode 3: Write Disable
        {EnumSpiOpcodeWrite,          SF_INST_SERASE,         EnumSpiCycle50MHz,  EnumSpiOperationErase_4K_Byte},       // Opcode 4: Sector Erase (4KB)
        {EnumSpiOpcodeWrite,          SF_INST_64KB_ERASE,     EnumSpiCycle50MHz,  EnumSpiOperationErase_64K_Byte},      // Opcode 5: Block Erase (64KB)
        {EnumSpiOpcodeWrite,          SF_INST_PROG,           EnumSpiCycle50MHz,  EnumSpiOperationProgramData_1_Byte},  // Opcode 6: Byte Program
        {EnumSpiOpcodeWriteNoAddr,    SF_INST_WRSR,           EnumSpiCycle50MHz,  EnumSpiOperationWriteStatus},         // Opcode 7: Write Status Register
      },
      0,                        // BiosStartOffset
      0,                        // BiosSize
      XM25QH128B_SIZE           // FlashSize
    },
    SF_SECTOR_4K_SIZE,
    gFlashMap64M_4K
  },
};

#define MY_SPI_TABLE_SUPPORT_COUNT  (sizeof(gMySpiInitTableList)/sizeof(gMySpiInitTableList[0]))



/**
  SPI Flash device Driver entry point.

  Register SPI flash device access method to NVMediaAccess Protocol.

  @param[in] ImageHandle       The firmware allocated handle for the EFI image.
  @param[in] SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS          The entry point is executed successfully.
  @retval EFI_OUT_OF_RESOURCES Fails to allocate memory for device.
  @retval other                Some error occurs when executing this entry point.

**/
EFI_STATUS
FlashDeviceSmmEntry (
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS                Status;
  VOID                      *Interface;
  NV_DEVICE_INSTANCE        *NvDev;
  UINTN                     Index;
  MY_SPI_INIT_TABLE         *MyInitTable;
  EFI_HANDLE                Handle = NULL;
  NV_MEDIA_ACCESS_PROTOCOL  *NvAcc;


  Status = gBS->LocateProtocol (&gEfiSmmNvMediaDeviceProtocolGuid, NULL, &Interface);
  if(!EFI_ERROR(Status)){
    Status = EFI_ALREADY_STARTED;
    goto ProcExit;
  }

  NvDev = &mNvDevice;
  NvDev->Handle = ImageHandle;

  Status = gSmst->SmmLocateProtocol (
                  &gEfiSmmSpiProtocolGuid, 
                  NULL, 
                  &NvDev->SpiProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol(
                  &gEfiSmmPlatformAccessProtocolGuid, 
                  NULL, 
                  &NvDev->PlatformAccessProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol(&gEfiSmmNvMediaAccessProtocolGuid, NULL, &NvAcc);
  ASSERT_EFI_ERROR (Status);

  NvDev->PlatformAccessProtocol->Enable(NvDev->PlatformAccessProtocol);

  MyInitTable = gMySpiInitTableList;
  for(Index=0;Index<MY_SPI_TABLE_SUPPORT_COUNT;Index++){
    NvDev->InitTable = &MyInitTable[Index].InitTable;
    if(NvDev->InitTable->FlashSize < PcdGet32(PcdFlashAreaSize)){
      continue;
    }
    
    NvDev->BlockMap  = MyInitTable[Index].BlockMap;
    NvDev->InitTable->BiosSize = PcdGet32(PcdFlashAreaSize);
    NvDev->InitTable->BiosStartOffset = MyInitTable[Index].InitTable.FlashSize - NvDev->InitTable->BiosSize;
    NvDev->FlashSize = MyInitTable[Index].InitTable.FlashSize;
    NvDev->SectorSize = MyInitTable[Index].SectorSize;
    Status = NvDev->SpiProtocol->Init(NvDev->SpiProtocol, NvDev->InitTable);
    if(!EFI_ERROR(Status)){
      SpiFlashPatch(NvDev);
      break;
    }
  }

  NvDev->PlatformAccessProtocol->Disable(NvDev->PlatformAccessProtocol);

  if (Index < MY_SPI_TABLE_SUPPORT_COUNT) {
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiSmmNvMediaDeviceProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &NvDev->DeviceProtocol
                    );
    ASSERT_EFI_ERROR (Status);

    Status = NvAcc->Init(NvAcc, &NvDev->DeviceProtocol, SPI_MEDIA_TYPE);
    ASSERT_EFI_ERROR (Status);
    
  } else {
    Status = EFI_UNSUPPORTED;
  }

ProcExit:		
  return Status;
}



