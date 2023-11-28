
#ifndef __SPI_CMD_H__
#define __SPI_CMD_H__


#define SF_INST_NOP             0x00
#define SF_INST_WRSR            0x01     // Write Status Register
#define SF_INST_PROG            0x02     // Byte Program    
#define SF_INST_READ            0x03     // Read
#define SF_INST_WRDI            0x04     // Write Disable
#define SF_INST_RDSR            0x05     // Read Status Register
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








#define SF_VENDOR_ID_WINBOND        0xEF
#define SF_DEVICE_ID0_W25Q64B       0x40
#define SF_DEVICE_ID1_W25Q64B       0x17
#define WINBOND_W25Q64B_SIZE       	0x800000

#define SF_DEVICE_ID0_W25Q128B      0x40
#define SF_DEVICE_ID1_W25Q128B      0x18
#define WINBOND_W25Q128B_SIZE       0x1000000

#define SF_DEVICE_ID0_W25Q256B      0x40
#define SF_DEVICE_ID1_W25Q256B      0x19
#define WINBOND_W25Q256B_SIZE       0x2000000

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
#define SF_DEVICE_ID0_N25Q064       0xBB
#define SF_DEVICE_ID1_N25Q064       0x17
#define NUMONYX_N25Q064_SIZE        0x800000

#define SF_VENDOR_ID_MXIC           0xC2
#define SF_DEVICE_ID0_MX25L64       0x20
#define SF_DEVICE_ID1_MX25L64       0x17
#define MXIC_MX25L64_SIZE           0x800000

#define SF_DEVICE_ID0_MX25L128      0x20
#define SF_DEVICE_ID1_MX25L128      0x18
#define MXIC_MX25L128_SIZE          0x1000000

#define SF_DEVICE_ID0_MX25L256      0x20
#define SF_DEVICE_ID1_MX25L256      0x19
#define MXIC_MX25L256_SIZE          0x2000000

#define SF_DEVICE_ID0_MX25L512      0x20
#define SF_DEVICE_ID1_MX25L512      0x1A
#define MXIC_MX25L512_SIZE          0x4000000

#define SF_VENDOR_ID_MICRON         0x20
#define SF_DEVICE_ID0_M_N25Q064     0xBA
#define SF_DEVICE_ID1_M_N25Q064     0x17
#define MICRON_N25Q064_SIZE         0x800000

#define SF_DEVICE_ID0_N25Q128       0xBA
#define SF_DEVICE_ID1_N25Q128       0x18
#define MICRON_N25Q128_SIZE         0x1000000

#define SF_DEVICE_ID0_N25Q256       0xBA
#define SF_DEVICE_ID1_N25Q256       0x18
#define MICRON_N25Q256_SIZE         0x2000000

#define SF_VENDOR_ID_GD             0xC8
#define SF_DEVICE_ID0_25Q64C        0x40
#define SF_DEVICE_ID1_25Q64C        0x17
#define SF_DEVICE_ID0_25B256D       0x40
#define SF_DEVICE_ID1_25B256D       0x19
#define SF_DEVICE_ID0_25B512ME      0x47
#define SF_DEVICE_ID1_25B512ME      0x1A
#define GD25B256D_SIZE              0x2000000
#define GD25B512ME_SIZE             0x2000000


#define GDW25Q64C_SIZE              0x800000

#define SF_VENDOR_ID_XM              0x20
#define SF_DEVICE_ID0_XM_25QH64B     0x60
#define SF_DEVICE_ID1_XM_25QH64B     0x17
#define XM25QH64B_SIZE               0x800000

#define SF_DEVICE_ID0_XM_25QH128B    0x60
#define SF_DEVICE_ID1_XM_25QH128B    0x18
#define XM25QH128B_SIZE              0x1000000


#define SF_SECTOR_4K_SIZE           0x1000



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
    SPI_WRDI    = 3,           // Opcode 3: Write Disable, No address
    SPI_RBPR    = 3,           // Opcode 3: Read Block Protection Register,No address
    SPI_SERASE  = 4,           // Opcode 4: Sector Erase (4KB), Write cycle with address
    SPI_BERASE  = 5,           // Opcode 5: Block Erase (32KB), Write cycle with address
    SPI_PROG    = 6,           // Opcode 6: Page Program, Write cycle with address
    SPI_WRSR    = 7,           // Opcode 7: Write Status Register, No address
} SPI_OPCODE_INDEX;

#endif
