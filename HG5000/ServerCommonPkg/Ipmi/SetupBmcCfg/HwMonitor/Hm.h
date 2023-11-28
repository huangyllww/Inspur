/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _EFI_HM_H
#define _EFI_HM_H

#define SENOR_TEMP_KEY           0x4000
// degrees C
#define CPU0_Temp                0x74    //  42 degrees C
#define CPU1_Temp                0x75    //  37 degrees C
#define PCIe_Amb_Temp            0x40    //  28 degrees C
#define OCP_Amb_Temp             0x41    //  27 degrees C
#define SYS_Outlet_Temp          0x42    //  23 degrees C
#define CPU0_Inlet_Temp          0x43    //  27 degrees C
#define CPU1_Inlet_Temp          0x44    //  27 degrees C
#define PSU1_Inlet_Temp          0x45    //  26 degrees C
#define PSU2_Inlet_Temp          0x46    //  25 degrees C
#define PSU1_Hs_Temp             0x5C    //  31 degrees C
#define PSU2_Hs_Temp             0x5D    //  27 degrees C

// Volts
#define CPU0_VCORE               0x20    // 0.961 Volts
#define CPU1_VCORE               0x21    // 0.966 Volts
#define CPU0_VSOC                0x22    // 0.875 Volts
#define CPU1_VSOC                0x23    // 0.868 Volts
#define CPU0_ABCD_VDDQ           0x24    // 1.221 Volts
#define CPU0_EFGH_VDDQ           0x25    // 1.221 Volts
#define CPU1_IJKL_VDDQ           0x26    // 1.221 Volts
#define CPU1_MNOP_VDDQ           0x27    // 1.221 Volts
#define _5V_AUX                  0x28    // 5.123 Volts
#define _3V3_AUX                 0x29    // 3.317 Volts
#define _1V8_AUX_CPU1            0x2B    // 1.786 Volts
#define _12V                     0x2C    // 12.10 Volts
#define _5V                      0x2D    // 5.005 Volts
#define _3V3                     0x2E    // 3.335 Volts
#define VBAT                     0x2F    // 3.003 Volts

// Amps

// Watts
#define PSU1_Power_in            0x54     //  260 Watts
#define PSU2_Power_in            0x55     //  240 Watts

// RPM
#define FAN1_Speed               0x30     // 5100 RPM
#define FAN2_Speed               0x31     // 5200 RPM
#define FAN3_Speed               0x32     // 5200 RPM
#define FAN4_Speed               0x33     // 5100 RPM

#endif
