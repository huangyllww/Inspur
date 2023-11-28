#ifndef _AMD_SATA_WORKAROUND_H_
#define _AMD_SATA_WORKAROUND_H_

/// Device and function number of PCI device
typedef struct {
  AMD_PLATFORM_ID       PlatformID;                ///< Platform Identification
  UINT16                SataInstanceBitMap;        ///< Sata instance bit map
} SATA_WA_PLATFORM;

//                    Socket 0                    |                     Socket1
//    Die0        Die1        Die2        Die3    |     Die4        Die5        Die6        Die7
//PHYP0 PHYP1 PHYP0 PHYP1 PHYP0 PHYP1 PHYP0 PHYP1 | PHYP0 PHYP1 PHYP0 PHYP1 PHYP0 PHYP1 PHYP0 PHYP1
//Bit0  Bit1  Bit2  Bit3  Bit4  Bit5  Bit6  Bit7  | Bit8  Bit9  Bit10 Bit11 Bit12 Bit13 Bit14 Bit15

#endif //_AMD_SATA_WORKAROUND_H_
