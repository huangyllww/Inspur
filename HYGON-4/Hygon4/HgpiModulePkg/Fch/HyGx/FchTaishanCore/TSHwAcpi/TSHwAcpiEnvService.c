/* $NoKeywords:$ */

/**
 * @file
 *
 * Config Fch HwAcpi controller
 *
 * Init HwAcpi Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: FCH
 *
 */
/*
*****************************************************************************
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "FchCommonCfg.h"
#include "Filecode.h"
#define FILECODE  PROC_FCH_TAISHAN_TSHWACPI_TSHWACPIENVSERVICE_FILECODE

#define HYGON_CPUID_APICID_LPC_BID  0x00000001ul    // Local APIC ID, Logical Processor Count, Brand ID

ACPI_REG_WRITE FchTaishanInitEnvSpecificHwAcpiMmioTable[] =
{
  { 00,             00,                  0xB0, 0xAC        },
  { PMIO_BASE >> 8, FCH_PMIOA_REG00 + 2, 0xFB, BIT2        },                       // Set ASF SMBUS master function enabled here (temporary)
 #ifdef ACPI_SLEEP_TRAP
  { SMI_BASE  >> 8, FCH_SMI_REGB0,       0xF3, BIT2        },                       // Set SLP_TYPE as SMI event
  { PMIO_BASE >> 8, FCH_PMIOA_REGBE,     0xDF, 0x00        },                       // Disabled SLP function for S1/S3/S4/S5
  { PMIO_BASE >> 8, FCH_PMIOA_REG08 + 3, 0xFC, BIT1        },                       // Set S state transition disabled (BIT0) force ACPI to
                                                                                    // send SMI message when writing to SLP_TYP Acpi register. (BIT1)
  { SMI_BASE  >> 8, FCH_SMI_REG98 + 3,   0x7F, 0x00        },                       // Enabled Global Smi ( BIT7 clear as 0 to enable )
 #endif
  { PMIO_BASE >> 8, FCH_PMIOA_REG80 + 1, 0xE7, BIT3 + BIT4 },
  { 0xFF,           0xFF,                0xFF, 0xFF        },
};

/**
 * FchInitEnvHwAcpiMmioTable - Fch ACPI MMIO initial
 * during POST.
 *
 */
ACPI_REG_WRITE  FchTaishanInitEnvHwAcpiMmioTable[] =
{
  { 00,             00,                     0xB0, 0xAC                      }, /// Signature

  { PMIO_BASE >> 8, FCH_PMIOA_REGCC,        0xF8, 0x03                      },
  { PMIO_BASE >> 8, FCH_PMIOA_REG74,        0x00, BIT0 + BIT1 + BIT2 + BIT4 },
  { PMIO_BASE >> 8, FCH_PMIOA_REG74 + 3,    0xDF, 0                         },
  { PMIO_BASE >> 8, FCH_PMIOA_REGBA,        0xF7, BIT3                      },
  { PMIO_BASE >> 8, FCH_PMIOA_REGBC,        0xFD, BIT1                      },

  { SMI_BASE >> 8,  FCH_SMI_Gevent1,        0,    1                         },
  { SMI_BASE >> 8,  FCH_SMI_Gevent3,        0,    3                         },
  { SMI_BASE >> 8,  FCH_SMI_Gevent4,        0,    4                         },
  { SMI_BASE >> 8,  FCH_SMI_Gevent5,        0,    5                         },
  { SMI_BASE >> 8,  FCH_SMI_Gevent6,        0,    6                         },
  { SMI_BASE >> 8,  FCH_SMI_Gevent23,       0,    23                        },
  // {SMI_BASE >> 8, FCH_SMI_xHC0Pme, 0, 11},
  // {SMI_BASE >> 8, FCH_SMI_xHC1Pme, 0, 11},
  // {SMI_BASE >> 8, FCH_SMI_Usbwakup0, 0, 11},
  // {SMI_BASE >> 8, FCH_SMI_Usbwakup1, 0, 11},
  // {SMI_BASE >> 8, FCH_SMI_Usbwakup2, 0, 11},
  // {SMI_BASE >> 8, FCH_SMI_Usbwakup3, 0, 11},
  // {SMI_BASE >> 8, FCH_SMI_IMCGevent0, 0, 12},
  // {SMI_BASE >> 8, FCH_SMI_FanThGevent, 0, 13},
  // {SMI_BASE >> 8, FCH_SMI_SBGppPme0, 0, 15},
  // {SMI_BASE >> 8, FCH_SMI_SBGppPme1, 0, 16},
  // {SMI_BASE >> 8, FCH_SMI_SBGppPme2, 0, 17},
  // {SMI_BASE >> 8, FCH_SMI_SBGppPme3, 0, 18},
  // {SMI_BASE >> 8, FCH_SMI_GecPme, 0, 19},
  // {SMI_BASE >> 8, FCH_SMI_CIRPme, 0, 28},
  { SMI_BASE >> 8,  FCH_SMI_Gevent8,        0,    24                        },
  // {SMI_BASE >> 8, FCH_SMI_AzaliaPme, 0, 27},
  { SMI_BASE >> 8,  FCH_SMI_SataGevent0,    0,    30                        },
  { SMI_BASE >> 8,  FCH_SMI_SataGevent1,    0,    31                        },
  { SMI_BASE >> 8,  FCH_SMI_REG08,          0xE7, 0                         },
  { SMI_BASE >> 8,  FCH_SMI_REG0C + 2,      0xF7, BIT3                      },
  { SMI_BASE >> 8,  FCH_SMI_TS_TWRN,        0,    9                         },
  { SMI_BASE >> 8,  FCH_SMI_TS_XhcWake,     0,    11                        },
  { SMI_BASE >> 8,  FCH_SMI_TS_FanThGevent, 0,    13                        },
  { SMI_BASE >> 8,  FCH_SMI_TS_NbGppPme,    0,    16                        },
  { SMI_BASE >> 8,  FCH_SMI_TS_NbGppHp,     0,    17                        },
  { 0xFF,           0xFF,                   0xFF, 0xFF                      },
};

/**
 * FchTaishanInitEnvHwAcpiPciTable - PCI device registers
 * initial during early POST.
 *
 */
REG8_MASK  FchTaishanInitEnvHwAcpiPciTable[] =
{
  //
  // SMBUS Device (Bus 0, Dev 20, Func 0)
  //
  { 0x00,          SMBUS_BUS_DEV_FUN, 0                    },
  { FCH_CFG_REG10, 0X00,              (FCH_VERSION & 0xFF) }, /// Program the version information
  { FCH_CFG_REG11, 0X00,              (FCH_VERSION >> 8)   },
  { 0xFF,          0xFF,              0xFF                 },
};

///
/// PCI_IRQ_REG_BLOCK- FCH PCI IRQ registers block
///
typedef struct _PCI_IRQ_REG_BLOCK {
  UINT8    PciIrqIndex;                   // PciIrqIndex - selects which PCI interrupt to map
  UINT8    PciIrqData;                    // PciIrqData  - Interrupt #
} PCI_IRQ_REG_BLOCK;

STATIC PCI_IRQ_REG_BLOCK  FchInternalDeviceIrqForApicMode[] = {
  { (FCH_TS_IRQ_INTA | FCH_IRQ_IOAPIC),  0x10 },
  { (FCH_TS_IRQ_INTB | FCH_IRQ_IOAPIC),  0x11 },
  { (FCH_TS_IRQ_INTC | FCH_IRQ_IOAPIC),  0x12 },
  { (FCH_TS_IRQ_INTD | FCH_IRQ_IOAPIC),  0x13 },
  { (FCH_TS_IRQ_SD | FCH_IRQ_IOAPIC),    0x10 },
  { (FCH_TS_IRQ_SDIO | FCH_IRQ_IOAPIC),  0x10 },
  { (FCH_TS_IRQ_EMMC | FCH_IRQ_IOAPIC),  0x5  },
  { (FCH_TS_IRQ_GPIO | FCH_IRQ_IOAPIC),  0x7  },
  { (FCH_TS_IRQ_UART0 | FCH_IRQ_IOAPIC), 0x4  },
  { (FCH_TS_IRQ_UART1 | FCH_IRQ_IOAPIC), 0x3  },
  { (FCH_TS_IRQ_UART2 | FCH_IRQ_IOAPIC), 0x4  },
  { (FCH_TS_IRQ_UART3 | FCH_IRQ_IOAPIC), 0x3  },
  { (FCH_TS_IRQ_I2C0 | FCH_IRQ_IOAPIC),  0xa  },
  { (FCH_TS_IRQ_I2C1 | FCH_IRQ_IOAPIC),  0xb  },
  { (FCH_TS_IRQ_I2C2 | FCH_IRQ_IOAPIC),  0xc  },
  { (FCH_TS_IRQ_I2C3 | FCH_IRQ_IOAPIC),  0x6  }, // Note:If there still have issue with IRQ 6,please try changing it to 5
  { (FCH_TS_IRQ_I2C4 | FCH_IRQ_IOAPIC),  0xe  },
  { (FCH_TS_IRQ_I2C5 | FCH_IRQ_IOAPIC),  0xf  },
};

STATIC PCI_IRQ_REG_BLOCK  FchInternalDeviceIrqForVWMode[] = {
  { (FCH_TS_IRQ_INTA | FCH_IRQ_IOAPIC),  0x10 },
  { (FCH_TS_IRQ_INTB | FCH_IRQ_IOAPIC),  0x11 },
  { (FCH_TS_IRQ_INTC | FCH_IRQ_IOAPIC),  0x12 },
  { (FCH_TS_IRQ_INTD | FCH_IRQ_IOAPIC),  0x13 },
  { (FCH_TS_IRQ_SD | FCH_IRQ_IOAPIC),    0x10 },
  { (FCH_TS_IRQ_SDIO | FCH_IRQ_IOAPIC),  0x10 },
  { (FCH_TS_IRQ_EMMC | FCH_IRQ_IOAPIC),  0x5  },
  { (FCH_TS_IRQ_GPIO | FCH_IRQ_IOAPIC),  0x7  },
  { (FCH_TS_IRQ_UART0 | FCH_IRQ_IOAPIC), 0x4  },
  { (FCH_TS_IRQ_UART1 | FCH_IRQ_IOAPIC), 0x3  },
  { (FCH_TS_IRQ_UART2 | FCH_IRQ_IOAPIC), 0x4  },
  { (FCH_TS_IRQ_UART3 | FCH_IRQ_IOAPIC), 0x3  },
  { (FCH_TS_IRQ_I2C0 | FCH_IRQ_IOAPIC),  0xa  },
  { (FCH_TS_IRQ_I2C1 | FCH_IRQ_IOAPIC),  0xb  },
  { (FCH_TS_IRQ_I2C2 | FCH_IRQ_IOAPIC),  0x4  },
  { (FCH_TS_IRQ_I2C3 | FCH_IRQ_IOAPIC),  0x6  },
  { (FCH_TS_IRQ_I2C4 | FCH_IRQ_IOAPIC),  0xe  },
  { (FCH_TS_IRQ_I2C5 | FCH_IRQ_IOAPIC),  0xf  },
};
#define NUM_OF_DEVICE_FOR_APICIRQ  sizeof (FchInternalDeviceIrqForApicMode) / sizeof (PCI_IRQ_REG_BLOCK)
#define VW_SRCACTIVELOW            0xFFCEF8ul

/**
 * ProgramEnvPFchAcpiMmio - Config HwAcpi MMIO registers
 *   Acpi S3 resume won't execute this procedure (POST only)
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramEnvPFchAcpiMmio (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;
  ProgramFchAcpiMmioTbl ((ACPI_REG_WRITE *)(&FchTaishanInitEnvHwAcpiMmioTable[0]), StdHeader);
}

/**
 * ProgramFchEnvHwAcpiPciReg - Config HwAcpi PCI controller
 * before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramFchEnvHwAcpiPciReg (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  //
  // Early post initialization of pci config space
  //
  ProgramPciByteTable ((REG8_MASK *)(&FchTaishanInitEnvHwAcpiPciTable[0]), sizeof (FchTaishanInitEnvHwAcpiPciTable) / sizeof (REG8_MASK), StdHeader);

  if ( LocalCfgPtr->Smbus.SmbusSsid != NULL ) {
    PciAndThenOr32 ((SMBUS_BUS_DEV_FUN << 12) + FCH_CFG_REG2C, 0x00, LocalCfgPtr->Smbus.SmbusSsid);
  }

  if ( LocalCfgPtr->Misc.NoneSioKbcSupport ) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGED, AccessWidth8, ~(UINT32)(BIT1), BIT1);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGED, AccessWidth8, ~(UINT32)(BIT1), 0);
  }

  ProgramPcieNativeMode (FchDataPtr);
}

/**
 * FchVgaInit - Config VGA CODEC
 *
 * @param[in] VOID empty
 *
 */
VOID
FchVgaInit (
  OUT VOID
  )
{
}

/**
 * ProgramSpecificFchInitEnvAcpiMmio - Config HwAcpi MMIO before
 * PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramSpecificFchInitEnvAcpiMmio (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;
  ProgramFchAcpiMmioTbl ((ACPI_REG_WRITE *)(&FchTaishanInitEnvSpecificHwAcpiMmioTable[0]), StdHeader);

  //
  // Set LDTSTP# duration
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG94, AccessWidth8, 0, 0x01);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG80 + 3, AccessWidth8, 0xFE, 0x20);
  //
  // LpcClk0DrivenZero & RTC Wake Mode Control
  //
  RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG6C + 2, AccessWidth8, 0x5F, 0xA0);
  //
  // Ac Loss Control
  //
  AcLossControl ((UINT8)LocalCfgPtr->HwAcpi.PwrFailShadow);
  //
  // FCH VGA Init
  //
  FchVgaInit ();

  //
  // Set ACPIMMIO by OEM Input table
  //
  ProgramFchAcpiMmioTbl ((ACPI_REG_WRITE *)(LocalCfgPtr->HwAcpi.OemProgrammingTablePtr), StdHeader);
}

/**
 * ValidateFchVariant - Validate FCH Variant
 *
 *
 *
 * @param[in] FchDataPtr
 *
 */
VOID
ValidateFchVariant (
  IN  VOID     *FchDataPtr
  )
{
}

/**
 * IsExternalClockMode - Is External Clock Mode?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsExternalClockMode (
  IN  VOID     *FchDataPtr
  )
{
  UINT8  MISC80;

  ReadMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG80, AccessWidth8, &MISC80);
  return ( (BOOLEAN) ((MISC80 & BIT3) == BIT3) );
}

/**
 * ProgramFchEnvSpreadSpectrum - Config SpreadSpectrum before
 * PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramFchEnvSpreadSpectrum (
  IN  VOID     *FchDataPtr
  )
{
  UINT8  FchSpreadSpectrum;

  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;
  FchSpreadSpectrum = LocalCfgPtr->HwAcpi.SpreadSpectrum;

  //
  // new sequence for SSC programming
  //
  if (IsExternalClockMode (FchDataPtr)) {
    //Step 1 - If it is  Master die (SL2/SL2r2 socket 0 master die and DHM1): (100M refclk)
    //  Misc_Reg x10[1:0] (refclk_div[1:0]) value from 2'b00 to 2'b01 (no change)
    //  Misc_Reg x10[12:4] (FCW0_int[8:0]) value from 9'h000 to 9'h004 (no change)
    //  Misc_Reg x34[31:23] (FCW1_int[8:0]) value from 9'h000 to 9'h003C (no change)
    //  Misc_Reg x14[23:8] (FCW1_frac[8:0]) value from 16'h0000 to 16'h00003D7 (no change)
    //  Misc_Reg x18[31:16] (FCW_slew_frac[15:0]) value from 16'h0000 to 16'h0000 (no change)
    //  Misc_Reg x1C[16:13] (gp_coarse_mant[3:0]) value from 16'h0000 to 16'h0001
    //  Misc_Reg x1C[20:17](gp_coarse_exp[3:0]) value from 2'b00 to 2'b00
    //  Misc_Reg x1C[27:26] (gi_coarse_mant[1:0]) value from 2'b00 to 2'b10
    //  Misc_Reg x1C[12:9] (gi_coarse_exp[3:0]) value from 4'b0000 to  4'b0010
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x10, AccessWidth32, ~(UINT32) (0x3 << 0), (UINT32) (1 << 0));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x10, AccessWidth32, ~(UINT32) (0x1FF << 4), (UINT32) (4 << 4));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x34, AccessWidth32, ~(UINT32) (0x1FF << 23), (UINT32) (0x3C << 23));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x14, AccessWidth32, ~(UINT32) (0xFFFF << 8), (UINT32) (0x3D7 << 8));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x18, AccessWidth32, ~(UINT32) (0xFFFF << 16), (UINT32) (0x3E << 16));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32) (0xF << 13), (UINT32) (1 << 13));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32) (0xF << 17), (UINT32) (0 << 17));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32) (0x3 << 26), (UINT32) (2 << 26));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32) (0xF << 9), (UINT32) (2 << 9));
  } else {
    //Step 1 - If it is  Master die (SL2/SL2r2 socket 0 master die and DHM1): (48M refclk)
    //  Misc_Reg x10[1:0] (refclk_div[1:0]) value from 2'b00 to 2'b00 (no change)
    //  Misc_Reg x10[12:4] (FCW0_int[8:0]) value from 9'h000 to 9'h000 (no change)
    //  Misc_Reg x34[31:23] (FCW1_int[8:0]) value from 9'h000 to 9'h0000 (no change)
    //  Misc_Reg x14[23:8] (FCW1_frac[8:0]) value from 16'h0000 to 16'h0000 (no change)
    //  Misc_Reg x18[31:16] (FCW_slew_frac[15:0]) value from 16'h0000 to 16'h0000 (no change)
    //  Misc_Reg x1C[16:13] (gp_coarse_mant[3:0]) value from 16'h0000 to 16'h0004
    //  Misc_Reg x1C[20:17](gp_coarse_exp[3:0]) value from 2'b00 to 2'b11
    //  Misc_Reg x1C[27:26] (gi_coarse_mant[1:0]) value from 2'b00 to 2'b11
    //  Misc_Reg x1C[12:9] (gi_coarse_exp[3:0]) value from 4'b0000 to  4'b0001
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x10, AccessWidth32, ~(UINT32) (0x3 << 0), (UINT32) (0 << 0));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x10, AccessWidth32, ~(UINT32) (0x1FF << 4), (UINT32) (0 << 4));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x34, AccessWidth32, ~(UINT32) (0x1FF << 23), (UINT32) (0 << 23));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x14, AccessWidth32, ~(UINT32) (0xFFFF << 8), (UINT32) (0 << 8));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x18, AccessWidth32, ~(UINT32) (0xFFFF << 16), (UINT32) (0 << 16));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32) (0xF << 13), (UINT32) (4 << 13));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32) (0xF << 17), (UINT32) (3 << 17));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32) (0x3 << 26), (UINT32) (3 << 26));
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32) (0xF << 9), (UINT32) (1 << 9));
  }

  //
  // Step 2 - Misc_Reg x40[25] (FBDIV_LoadEN for loading register) value from 1'b0 to 1'b1
  //
  RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x40, AccessWidth32, ~(UINT32)BIT25, BIT25);

  //
  // Step 3 - Misc_ Reg x40[30]=1 (cfg_update_req)
  //
  RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x40, AccessWidth32, ~(UINT32)BIT30, BIT30);

  //
  // Step 4 - 6 are for Slave Dies which will be done in MultiFchPei
  //

  if ( FchSpreadSpectrum ) {
    //
    // Step 7 - If SSC needs to be enabled (only for SL2/SL2r2 socket 0 master die and DHM1-single die, 48M refclk)
    // For SSC enable: (follow-up after CGPLL setting changes)
    // Misc_x40[29]=1 (don't need to do this)
    // Misc_ Reg x10[29]=1 (CORE_CGPLL_fracn_en)
    // Misc_ Reg x08[0]=1 (CORE_CGPLL_ssc_en)
    // Misc_ Reg x40[30]=1 (cfg_update_req)
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x10, AccessWidth32, ~(UINT32)BIT29, BIT29);
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x08, AccessWidth32, ~(UINT32)BIT0, BIT0);
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x40, AccessWidth32, ~(UINT32)BIT30, BIT30);
  } else {
    //
    // Step 8 - If SSC needs to be disabled
    // For SSC disable:
    // Misc_x40[29]=1 (don't need to do this)
    // Misc_ Reg x10[29]=1 (CORE_CGPLL_fracn_en)
    // Misc_ Reg x08[0]=0 (CORE_CGPLL_ssc_en)
    // Misc_ Reg x40[30]=1 (cfg_update_req)
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x10, AccessWidth32, ~(UINT32)BIT29, BIT29);
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x08, AccessWidth32, ~(UINT32)BIT0, 0x00);
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x40, AccessWidth32, ~(UINT32)BIT30, BIT30);
  }
}

/**
 * TurnOffCG2
 *
 *
 * @retval  VOID
 *
 */
VOID
TurnOffCG2 (
  OUT VOID
  )
{
}

/**
 * BackUpCG2
 *
 *
 * @retval  VOID
 *
 */
VOID
BackUpCG2 (
  OUT VOID
  )
{
}

/**
 * HpetInit - Program Fch HPET function
 *
 *
 *
 * @param[in] FchDataPtr         Fch configuration structure pointer.
 *
 */
VOID
HpetInit (
  IN  VOID     *FchDataPtr
  )
{
  DESCRIPTION_HEADER  *HpetTable;
  BOOLEAN             FchHpetTimer;
  BOOLEAN             FchHpetMsiDis;
  FCH_DATA_BLOCK      *LocalCfgPtr;

  LocalCfgPtr   = (FCH_DATA_BLOCK *)FchDataPtr;
  FchHpetTimer  = LocalCfgPtr->Hpet.HpetEnable;
  FchHpetMsiDis = LocalCfgPtr->Hpet.HpetMsiDis;

  HpetTable = NULL;
  if ( FchHpetTimer == TRUE ) {
    //
    // Program the HPET BAR address
    //
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG00, AccessWidth8, 0xFF, BIT6);

    //
    // Enabling decoding of HPET MMIO
    // Enable HPET MSI support
    // Enable High Precision Event Timer (also called Multimedia Timer) interrupt
    //
    if ( FchHpetMsiDis == FALSE ) {
      RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG00, AccessWidth32, ~(UINT32)BIT29, BIT29);
 #ifdef FCH_TIMER_TICK_INTERVAL_WA
        RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG00, AccessWidth32, ~(UINT32)BIT29, 0);
 #endif
    } else {
      RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG00, AccessWidth32, ~(UINT32)BIT29, 0);
    }
  } else {
    if ( !(LocalCfgPtr->Misc.S3Resume)) {
      HpetTable = (DESCRIPTION_HEADER *)AcpiLocateTable ('TEPH');
    }

    if ( HpetTable != NULL ) {
      HpetTable->Signature = 'HPET';
    } else {
    }
  }
}

/**
 * ProgramPcieNativeMode - Config Pcie Native Mode
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramPcieNativeMode (
  IN  VOID     *FchDataPtr
  )
{
  UINT8           FchNativepciesupport;
  FCH_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  FchNativepciesupport = (UINT8)LocalCfgPtr->Misc.NativePcieSupport;

  //
  // PCIE Native setting
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGBA + 1, AccessWidth8, ~(UINT32)BIT6, 0);
  if ( FchNativepciesupport == 1) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG74 + 3, AccessWidth8, ~(UINT32)(BIT3 + BIT1 + BIT0), BIT3 + BIT0);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG74 + 3, AccessWidth8, ~(UINT32)(BIT3 + BIT1 + BIT0), BIT3);
  }
}

/**
 * PciIntVwInit - Config Pci Interrupt routing for VW mode
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
PciIntVwInit (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;
  UINT8                i;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  for (i = 0; i < NUM_OF_DEVICE_FOR_APICIRQ; i++) {
    LibHygonIoWrite (AccessWidth8, FCH_IOMAP_REGC00, &FchInternalDeviceIrqForVWMode[i].PciIrqIndex, StdHeader);
    LibHygonIoWrite (AccessWidth8, FCH_IOMAP_REGC01, &FchInternalDeviceIrqForVWMode[i].PciIrqData, StdHeader);
  }

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGA8, AccessWidth32, 0, BIT31 + VW_SRCACTIVELOW);
}

/**
 * FchInternalDeviceIrqInit - Config Fch internal Device Interrupt routing
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInternalDeviceIrqInit (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;
  UINT8                i;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  for (i = 0; i < NUM_OF_DEVICE_FOR_APICIRQ; i++) {
    LibHygonIoWrite (AccessWidth8, FCH_IOMAP_REGC00, &FchInternalDeviceIrqForApicMode[i].PciIrqIndex, StdHeader);
    LibHygonIoWrite (AccessWidth8, FCH_IOMAP_REGC01, &FchInternalDeviceIrqForApicMode[i].PciIrqData, StdHeader);
  }
}

/**
 * FchAl2ahbInit - Config Fch AL2AHB init
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchAl2ahbInit (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  //
  // Enable Clock Gating
  //
  RwMem (AH2ALB_BASE + 0x10, AccessWidth8, 0xFF, BIT1);
  RwMem (AH2ALB_BASE + 0x30, AccessWidth8, 0xFF, BIT1);
}

/**
 * FchI2cUartInit - Config Fch AMBA I2C Uart init
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchI2cUartInit (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;
  UINT32               FchDeviceEnMap;

  LocalCfgPtr    = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader      = LocalCfgPtr->StdHeader;
  FchDeviceEnMap = LocalCfgPtr->FchRunTime.FchDeviceEnableMap;

  //
  // I2C0
  //
  if ( FchDeviceEnMap & BIT5 ) {
    FchAoacPowerOnDev (FCH_AOAC_I2C0, 1);
    // IOMUX
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x91, AccessWidth8, 0, 0x0);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x92, AccessWidth8, 0, 0x0);
  } else {
    // FchAoacPowerOnDev (FCH_AOAC_I2C0, 0);
  }

  //
  // I2C1
  //
  if ( FchDeviceEnMap & BIT6 ) {
    FchAoacPowerOnDev (FCH_AOAC_I2C1, 1);
    // IOMUX
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x93, AccessWidth8, 0, 0x0);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x94, AccessWidth8, 0, 0x0);
  } else {
    // FchAoacPowerOnDev (FCH_AOAC_I2C1, 0);
  }

  //
  // I2C2
  //
  if ( FchDeviceEnMap & BIT7 ) {
    FchAoacPowerOnDev (FCH_AOAC_I2C2, 1);
    // IOMUX
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x71, AccessWidth8, 0, 0x1);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x72, AccessWidth8, 0, 0x1);
  } else {
    // FchAoacPowerOnDev (FCH_AOAC_I2C2, 0);
  }

  //
  // I2C3
  //
  if ( FchDeviceEnMap & BIT8 ) {
    FchAoacPowerOnDev (FCH_AOAC_I2C3, 1);
    // IOMUX
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x13, AccessWidth8, 0, 0x1);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x14, AccessWidth8, 0, 0x1);
  } else {
    // FchAoacPowerOnDev (FCH_AOAC_I2C3, 0);
  }

  //
  // I3C0
  //
  if ( FchDeviceEnMap & BIT9 ) {
    FchAoacPowerOnDev (FCH_AOAC_I3C0, 1);
    // IOMUX
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x95, AccessWidth8, 0, 0x0);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x96, AccessWidth8, 0, 0x0);
  } else {
    // FchAoacPowerOnDev (FCH_AOAC_I3C0, 0);
  }

  //
  // I3C1
  //
  if ( FchDeviceEnMap & BIT10 ) {
    FchAoacPowerOnDev (FCH_AOAC_I3C1, 1);
    // IOMUX
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x97, AccessWidth8, 0, 0x0);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x98, AccessWidth8, 0, 0x0);
  } else {
    // FchAoacPowerOnDev (FCH_AOAC_I3C1, 0);
  }

  //
  // UART0
  //
  if ( FchDeviceEnMap & BIT11 ) {
    FchAoacPowerOnDev (FCH_AOAC_UART0, 1);
    // IOMUX
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x87, AccessWidth8, 0, 0x0);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x88, AccessWidth8, 0, 0x0);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x89, AccessWidth8, 0, 0x0);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x8A, AccessWidth8, 0, 0x0);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x8B, AccessWidth8, 0, 0x0);
  } else {
    //FchAoacPowerOnDev (FCH_AOAC_UART0, 0);
  }

  //
  // UART1
  //
  if ( FchDeviceEnMap & BIT12 ) {
    FchAoacPowerOnDev (FCH_AOAC_UART1, 1);
    // IOMUX
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x8C, AccessWidth8, 0, 0x0);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x8D, AccessWidth8, 0, 0x0);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x8E, AccessWidth8, 0, 0x0);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x8F, AccessWidth8, 0, 0x0);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x90, AccessWidth8, 0, 0x0);
  } else {
    //FchAoacPowerOnDev (FCH_AOAC_UART1, 0);
  }

  //
  // UART2
  //
  if ( FchDeviceEnMap & BIT16 ) {
    FchAoacPowerOnDev (FCH_AOAC_UART2, 1);
    // IOMUX
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x87, AccessWidth8, 0, 0x1);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x89, AccessWidth8, 0, 0x1);
  } else {
    // FchAoacPowerOnDev (FCH_AOAC_UART2, 0);
  }

  //
  // UART3
  //
  if ( FchDeviceEnMap & BIT26 ) {
    FchAoacPowerOnDev (FCH_AOAC_UART3, 1);
    // IOMUX
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x8C, AccessWidth8, 0, 0x1);
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x8E, AccessWidth8, 0, 0x1);
  } else {
    // FchAoacPowerOnDev (FCH_AOAC_UART3, 0);
  }

  if (FchReadSleepType () == 0x3 &&
      (FchDeviceEnMap & BIT11 || FchDeviceEnMap & BIT12 || FchDeviceEnMap & BIT16 || FchDeviceEnMap & BIT26)
      ) {
    SerialPortInitialize ();
  }

  //
  // PSP_INTR0
  //
  // IOMUX
  RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x59, AccessWidth8, 0, 0x1);
  // UART Legacy IO Enable Support
  if ( LocalCfgPtr->FchRunTime.Al2AhbLegacyUartIoEnable ) {
    ACPIMMIO16 (FCH_AL2AHBx20_LEGACY_UART_IO_ENABLE) = LocalCfgPtr->FchRunTime.Al2AhbLegacyUartIoEnable;
  }
}
