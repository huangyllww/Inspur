/** @file OemHgpiCustomizePei.h
    OemHgpiCustomizePei definitions

**/

#ifndef __HGPI_MEM_PEI_H_
#define __HGPI_MEM_PEI_H_
#ifdef __cplusplus
  extern "C" {
#endif

EFI_STATUS
EFIAPI
PlatformDimmSpdRead (
  IN      EFI_PEI_SERVICES                    **PeiServices, ///< Pointer to PeiServices
  IN struct _PEI_HYGON_PLATFORM_DIMM_SPD_PPI    *This,       ///< Pointer to the PPI structure
  IN OUT  HGPI_READ_SPD_PARAMS               *SpdData        ///< SPD Data Buffer
  );

#pragma pack (push, 1)
typedef struct {
  UINT8    SocketId;
  UINT8    ChannelId;
  UINT8    DimmId;
  UINT8    SmbusAddress;
} HGPI_DIMM_INFO_SMBUS;
#pragma pack (pop)

#define DIMM_DEVICE_SLOT_COUNT  DIMM_SLOTS_PER_SCKT           // Dimm Slot depends on platform

#define DIMM_SPD_SERIAL_NO_LENGTH  4        // 4   Bytes Serial Number
#define DIMM_SPD_PART_NO_LENGTH    18       // 18  Bytes Part Number
#define DIMM_SPD_DATA_LENGTH       512      // 512 Bytes SPD Data

// 16  Bytes Serial no Data
#define DIMM_HOB_SERIAL_NO_LENGTH  DIMM_SPD_SERIAL_NO_LENGTH *DIMM_DEVICE_SLOT_COUNT
// 512 Bytes * DIMM number Hob Data
#define DIMM_HOB_DATA_LENGTH  DIMM_SPD_DATA_LENGTH *DIMM_DEVICE_SLOT_COUNT

#define DIMM_SPD_SERIAL_NO_OFFSET_DDR4  0x145   // DDR4 SPD Serial Data Offset 325

#define DIMM_SPD_DATA_256  256                  // SPD data 256 bytes

#define OEM_HGPI_PCI_CFG_ADDRESS(bus, dev, func, reg) \
  (UINT64)((((UINT8)(bus) << 24) + ((UINT8)(dev) << 16) + \
            ((UINT8)(func) << 8) + ((UINT8)(reg))) & 0xffffffff)

#define HYGON_BUS_0             0x00              // Bus 0
#define HYGON_DEV_18            0x18              // Device Number
#define HYGON_MISC_FUNC         3                 // Misc Config bits
#define HYGON_F3_PWR_CTRL_MISC  0xA0              // Power Control Miscellaneous

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
  }
#endif
#endif
