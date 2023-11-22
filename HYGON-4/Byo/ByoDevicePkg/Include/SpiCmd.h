
/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/


#ifndef __SPI_CMD_H__
#define __SPI_CMD_H__


#define SF_INST_NOP             0x00
#define SF_INST_WRSR            0x01     // Write Status Register
#define SF_INST_WRSR2            0x31     // Write Status Register-2
#define SF_INST_WRSR3            0x11     // Write Status Register-3
#define SF_INST_PROG            0x02     // Byte Program    
#define SF_INST_READ            0x03     // Read
#define SF_INST_WRDI            0x04     // Write Disable
#define SF_INST_RDSR            0x05     // Read Status Register
#define SF_INST_RDSR2            0x35     // Read Status Register-2
#define SF_INST_RDSR3            0x15     // Read Status Register-3
#define SF_INST_RDCR            0x35     // Read Configuration Register
#define SF_INST_WREN            0x06     // Write Enable
#define SF_INST_HS_READ         0x0B     // High-speed Read 
#define SF_INST_SERASE          0x20     // Sector Erase (4KB)
#define SF_INST_SERASE_D8       0xD8
#define SF_INST_BERASE          0x52     // Block Erase (32KB)
#define SF_INST_64KB_ERASE      0xD8     // Block Erase (64KB)
#define SF_INST_64KB_ERASE_C7   0xC7     // Block Erase (64KB)
#define SF_INST_EWSR            0x50     // Enable Write Status Register
#define SF_INST_READ_ID         0xAB     // Read ID
#define SF_INST_JEDEC_READ_ID   0x9F     // JEDEC Read ID

#define SF_INST_RBPR            0x72     // Read Block-Protection Register
#define SF_INST_WBPR            0x42     // Write Block-Protection Register
#define SF_INST_LBPR            0x8D     // Lock down Block-protection Register
#define SF_INST_ULBPR           0x98     // Global Block Protection unlock

#define SF_SR_WPE               0x3C     // Enable write protection on all blocks for SST25 
#define SF_INST_256KB_ERASE      0xD8     // Sector Erase (256KB)








#define SF_VENDOR_ID_WINBOND        0xEF
#define SF_DEVICE_ID0_W25Q64B       0x40
#define SF_DEVICE_ID1_W25Q64B       0x17
#define WINBOND_W25Q64B_SIZE       	0x800000

#define SF_DEVICE_ID0_W25Q128B      0x40
#define SF_DEVICE_ID1_W25Q128B      0x18
#define WINBOND_W25Q128B_SIZE       0x1000000

#define SF_DEVICE_ID0_W25Q128JW     0x60
#define SF_DEVICE_ID1_W25Q128JW     0x18
#define WINBOND_W25Q128JW_SIZE      0x1000000

#define SF_DEVICE_ID0_W25Q256B      0x40
#define SF_DEVICE_ID1_W25Q256B      0x19
#define WINBOND_W25Q256B_SIZE       0x2000000

#define SF_DEVICE_ID0_W25Q256JW     0x60
#define SF_DEVICE_ID1_W25Q256JW     0x19
#define WINBOND_W25Q256JW_SIZE      0x2000000

#define SF_DEVICE_ID0_W25Q257FV     0x40
#define SF_DEVICE_ID1_W25Q257FV     0x19
#define WINBOND_W25Q257FV_SIZE      0x2000000

#define SF_DEVICE_ID0_W25M512       0x71
#define SF_DEVICE_ID1_W25M512       0x19
#define WINBOND_W25M512_SIZE        0x4000000


#define SF_VENDOR_ID_SST            0xBF
#define SF_DEVICE_ID0_26VF064B      0x26
#define SF_DEVICE_ID1_26VF064B      0x43
#define SST_26VF064B_SIZE           0x800000

#define SF_DEVICE_ID0_25VF064C      0x25
#define SF_DEVICE_ID1_25VF064C      0x4B
#define SST_25VF064C_SIZE           0x800000

#define SF_VENDOR_ID_ST             0x20
#define SF_DEVICE_ID0_STM25P64      0x20
#define SF_DEVICE_ID1_STM25P64      0x17
#define WINBOND_STM25P64_SIZE      	0x800000

#define	SF_VENDOR_ID_NUMONYX        0x20
#define SF_DEVICE_ID0_N25Q064       0xBA
#define SF_DEVICE_ID1_N25Q064       0x17
#define NUMONYX_N25Q064_SIZE        0x800000

#define SF_DEVICE_ID0_MX25P128      0x20
#define SF_DEVICE_ID1_MX25P128      0x18
#define NUMONYX_MX25P128_SIZE       0x1000000



#define SF_VENDOR_ID_MXIC           0xC2
#define SF_DEVICE_ID0_MX25L64       0x20
#define SF_DEVICE_ID1_MX25L64       0x17
#define MXIC_MX25L64_SIZE           0x800000


#define SF_DEVICE_ID0_MX25L25       0x20
#define SF_DEVICE_ID1_MX25L25       0x19
#define MXIC_MX225L25735F_SIZE     0x2000000


#define SF_DEVICE_ID0_MX25L128      0x20
#define SF_DEVICE_ID1_MX25L128      0x18
#define MXIC_MX25L128_SIZE          0x1000000

#define SF_DEVICE_ID0_MX25L256      0x20
#define SF_DEVICE_ID1_MX25L256      0x19
#define MXIC_MX25L256_SIZE          0x2000000

#define SF_DEVICE_ID0_MX25L512      0x20
#define SF_DEVICE_ID1_MX25L512      0x1A
#define MXIC_MX25L512_SIZE          0x4000000

#define SF_DEVICE_ID0_MX66L1G      0x20
#define SF_DEVICE_ID1_MX66L1G      0x1B
#define MXIC_MX66L1G_SIZE          0x8000000

#define SF_DEVICE_ID0_MX25U256      0x25
#define SF_DEVICE_ID1_MX25U256      0x39
#define MXIC_MX25U256_SIZE          0x2000000

#define SF_VENDOR_ID_MICRON         0x20
#define SF_DEVICE_ID0_M_N25Q064     0xBA
#define SF_DEVICE_ID1_M_N25Q064     0x17
#define MICRON_N25Q064_SIZE         0x800000

#define SF_DEVICE_ID0_N25Q128       0xBA
#define SF_DEVICE_ID1_N25Q128       0x18
#define MICRON_N25Q128_SIZE         0x1000000

#define SF_DEVICE_ID0_N25Q256       0xBA
#define SF_DEVICE_ID1_N25Q256       0x19
#define MICRON_N25Q256_SIZE         0x2000000

#define SF_DEVICE_ID0_MT25QL512     0xBA
#define SF_DEVICE_ID1_MT25QL512     0x20
#define MICRON_MT25QL512_SIZE       0x4000000

#define SF_VENDOR_ID_GD             0xC8
#define SF_DEVICE_ID0_25Q64C        0x40
#define SF_DEVICE_ID1_25Q64C        0x17
#define GDW25Q64C_SIZE              0x800000

#define SF_DEVICE_ID0_25LQ256D      0x60
#define SF_DEVICE_ID1_25LQ256D      0x19
#define GD25LQ256D_SIZE             0x2000000

#define SF_DEVICE_ID0_25B512        0x47
#define SF_DEVICE_ID1_25B512        0x1A
#define GDW25B512_SIZE              0x4000000

#define SF_DEVICE_ID0_25B256E       0x40
#define SF_DEVICE_ID1_25B256E       0x19
#define GD25B256E_SIZE              0x2000000

#define SF_DEVICE_ID0_25Q127C       0x40
#define SF_DEVICE_ID1_25Q127C       0x18
#define GD25Q127C_SIZE              0x1000000

#define SF_DEVICE_ID0_25LQ128E      0x60
#define SF_DEVICE_ID1_25LQ128E      0x18
#define GD25LQ128E_SIZE             0x1000000

#define SF_DEVICE_ID0_25B128E       0x40
#define SF_DEVICE_ID1_25B128E       0x18
#define GD25B128E_SIZE              0x1000000

#define SF_VENDOR_ID_XM              0x20
#define SF_DEVICE_ID0_XM_25QH64B     0x60
#define SF_DEVICE_ID1_XM_25QH64B     0x17
#define XM25QH64B_SIZE               0x800000

#define SF_DEVICE_ID0_XM_25QH64C     0x40
#define SF_DEVICE_ID1_XM_25QH64C     0x17
#define XM25QH64C_SIZE               0x800000

#define SF_DEVICE_ID0_XM_25P128     0x20
#define SF_DEVICE_ID1_XM_25P128     0x18
#define XM25P128_SIZE               0x1000000

#define SF_DEVICE_ID0_XM_25QH128C    0x40
#define SF_DEVICE_ID1_XM_25QH128C    0x18
#define XM25QH128C_SIZE              0x1000000

#define SF_DEVICE_ID0_XM_25QH128B    0x60
#define SF_DEVICE_ID1_XM_25QH128B    0x18
#define XM25QH128B_SIZE              0x1000000

#define SF_VENDOR_ID_SYP             0x01
#define SF_DEVICE_ID0_S25FL128P      0x20
#define SF_DEVICE_ID1_S25FL128P      0x18
#define S25FL128P_SIZE               0x1000000

#define SF_VENDOR_ID_XT              0x0B
#define SF_DEVICE_ID0_25BF128F       0x40
#define SF_DEVICE_ID1_25BF128F       0x18
#define XT25BF128F_SIZE              0x1000000

#define SF_DEVICE_ID0_25RF128F       0x40
#define SF_DEVICE_ID1_25RF128F       0x18
#define XT25RF128F_SIZE              0x1000000

#define SF_DEVICE_ID0_25BF256B       0x40
#define SF_DEVICE_ID1_25BF256B       0x19
#define XT25BF256B_SIZE              0x2000000

#define SF_DEVICE_ID0_25BQ64D        0x60
#define SF_DEVICE_ID1_25BQ64D        0x17
#define XT25BQ64D_SIZE               0x800000

#define SF_DEVICE_ID0_25BQ128D       0x60
#define SF_DEVICE_ID1_25BQ128D       0x18
#define XT25BQ128D_SIZE              0x1000000

#define SF_DEVICE_ID0_25F64F         0x40
#define SF_DEVICE_ID1_25F64F         0x17
#define XT25F64F_SIZE                0x800000

#define SF_DEVICE_ID0_25F128F        0x40
#define SF_DEVICE_ID1_25F128F        0x18
#define XT25F128F_SIZE               0x1000000

#define SF_DEVICE_ID0_25F256B        0x40
#define SF_DEVICE_ID1_25F256B        0x19
#define XT25F256B_SIZE               0x2000000

#define SF_DEVICE_ID0_25Q64D         0x60
#define SF_DEVICE_ID1_25Q64D         0x17
#define XT25Q64D_SIZE                0x800000

#define SF_DEVICE_ID0_25Q128D        0x60
#define SF_DEVICE_ID1_25Q128D        0x18
#define XT25Q128D_SIZE               0x1000000

#define SF_DEVICE_ID0_25RF256B       0x40
#define SF_DEVICE_ID1_25RF256B       0x19
#define XT25RF256B_SIZE              0x2000000

#define SF_SECTOR_4K_SIZE           0x1000
#define SF_SECTOR_256K_SIZE           0x40000



typedef enum {
    SPI_WREN = 0,             // Prefix Opcode 0: Write Enable
    SPI_EWSR = 1,             // Prefix Opcode 1: Enable Write Status Register
    SPI_NOP  = 1,             // Prefix Opcode 1: No Operation
} PREFIX_OPCODE_INDEX;

typedef enum {
    SPI_READ_ID = 0,           // Opcode 0: READ ID, Read cycle with address
    SPI_READ    = 1,
    SPI_UNLOCK  = 1,           // Opcode 1: Unlock Flash, No address
    SPI_RDSR    = 2,           // Opcode 2: Read Status Register, No address
    SPI_RDSR2    = 3,           // Opcode 2: Read Status Register2, No address
    SPI_RDSR3    = 3,           // Opcode 2: Read Status Register3, No address
    SPI_WRDI    = 3,           // Opcode 3: Write Disable, No address
    SPI_RBPR    = 3,           // Opcode 3: Read Block Protection Register,No address
    SPI_SERASE  = 4,           // Opcode 4: Sector Erase (4KB), Write cycle with address
    SPI_BERASE  = 5,           // Opcode 5: Block Erase (32KB), Write cycle with address
    SPI_WRSR2    = 5,           // Opcode 5: Write Status Register2, No address
    SPI_WRSR3    = 5,           // Opcode 5: Write Status Register3, No address
    SPI_PROG    = 6,           // Opcode 6: Page Program, Write cycle with address
    SPI_WRSR    = 7,           // Opcode 7: Write Status Register, No address
    SPI_ULBPR   = 7,           // Opcode 7: Global block protection unlock,No address
} SPI_OPCODE_INDEX;



#endif
