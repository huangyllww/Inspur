//# @file/
//#
//# Copyright (c) 2006 - 2018, Byosoft Corporation.<BR>
//# All rights reserved.This software and associated documentation (if any)
//# is furnished under a license and may only be used or copied in
//# accordance with the terms of the license. Except as permitted by such
//# license, no part of this software or documentation may be reproduced,
//# stored in a retrieval system, or transmitted in any form or by any
//# means without the express written consent of Byosoft Corporation.
//#
//# File Name:
//#   TscTimerLib.c
//#
//# Abstract:
//#   CPU Timer File
//#
//# Revision History:
//#
//##

/** @file TscTimerLib.c
    This file contains TscTimerLib library instance

**/

#include <Base.h>
//#include <token.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
//#include <Cpu.h>
#include <Library/IoLib.h>

//UINT32          gNumBoostStates = 0xFF;


/**
    Stalls the CPU for at least the given number of ticks.

    @param Delay A period of time to delay in ticks.

**/

VOID
InternalX86Delay (
  IN UINT64         Delay
  )
{
  UINT64            Ticks;

  //
  // The target timer count is calculated here
  //
  Ticks = AsmReadTsc() + Delay;

  //
  // Wait until time out
  // Timer wrap-arounds are NOT handled correctly by this function.
  // Thus, this function must be called within 10 years of reset since
  // Intel guarantees a minimum of 10 years before the TSC wraps.
  //
  while (AsmReadTsc() <= Ticks) CpuPause();
}

/**
    Read the PCI Register

    @param Address :(UINT32) ( (((UINT32)bus) << 24) + (((UINT32)dev) << 19) \
        + (((UINT32)func) << 16) + ((UINT32)reg) )
    @param ExtAccess : TRUE for extended access, FALSE is for normal access

    @retval UINT32 return value from the PCI Register.

**/
UINT32 TscReadPCI32(
    IN UINT32 Address,
    IN BOOLEAN  ExtAccess
)
{
    UINT32  Value32;
    UINT32  AddrCf8 = (1 << 31) + (Address & 0xFFFF00) + (Address & 0xFC);

    if (ExtAccess) //If extended config Access requested then add the extended config address  bit (24 to 27)
        AddrCf8 = AddrCf8 | 0x1000000;

    IoWrite32(0xCF8, AddrCf8);
    Value32 = IoRead32(0xCFC);
    return Value32;
}

/**
    Get CPU frequency

    @param VOID

    @retval UINT64 The number of TSC counts per second.

**/

UINT64
InternalGetTscFrequency (
  VOID
  )
{
  UINT64    TscFrequency;
  
  UINT32    RegValue;
  UINT32    CurFID;
  UINT32    CurDID;
  UINT32    CurrPstate = 0;

  CurrPstate = (UINT32)(AsmReadMsr64(0xC0010063) & 0x7);
  RegValue = (UINT32)AsmReadMsr64(0xC0010064 + CurrPstate);

  //Extract the FID and DID.
  CurFID = (UINT8)RegValue & 0xFF;
  CurDID = (UINT8)(RegValue >> 8) & 0x3F;
  // to avoid divide by zero
  if(CurDID == 0)   CurDID = 1;
  // Freq = (FID/DID)*200 MHz
  TscFrequency = MultU64x64(DivU64x32(CurFID, CurDID), 200 * 1000000);

  return TscFrequency;
}

/**
    This function delays for the number of micro seconds passed in

    @param MicroSeconds Number of microseconds(us) to delay

    @retval UINTN Value passed in for microseconds delay

**/

UINTN
EFIAPI
MicroSecondDelay (
  IN UINTN          MicroSeconds
  )
{
  InternalX86Delay (
    DivU64x32 (
      MultU64x64 (
        InternalGetTscFrequency (),
        MicroSeconds
        ),
      1000000u
    )
  );
  return MicroSeconds;
}

/**
    This function delays for the number of nano seconds passed in

    @param MicroSeconds Number of nanoseconds(ns) to delay

    @retval UINTN Value passed in for nanoseconds delay

**/

UINTN
EFIAPI
NanoSecondDelay (
  IN UINTN          NanoSeconds
  )
{ 
  InternalX86Delay (
    DivU64x32 (
      MultU64x32 (
        InternalGetTscFrequency (),
        (UINT32)NanoSeconds
      ),
      1000000000u
    )
  );
  return NanoSeconds;  
}

/**
    Retrieves the current value of a 64-bit free running
    performance counter.

    @param VOID

    @retval UINT64 The current value of the free running performance counter.

**/

UINT64
EFIAPI
GetPerformanceCounter (
  VOID
  )
{
  return AsmReadTsc ();
}

/**
    Retrieves the 64-bit frequency in Hz and the range of
    performance counter values.

    @param StartValue The value the performance counter starts with when
        it rolls over.
    @param EndValue The value that the performance counter ends with
        before it rolls over.

    @retval UINT64 The frequency in Hz.

**/

UINT64
EFIAPI
GetPerformanceCounterProperties (
  OUT      UINT64                    *StartValue,  OPTIONAL
  OUT      UINT64                    *EndValue     OPTIONAL
  )
{
    if (StartValue != NULL) {
      *StartValue = 0;
    }
    if (EndValue != NULL) {
      *EndValue = 0xFFFFFFFFFFFFFFFFull;
    }

    return InternalGetTscFrequency ();
}

/**
    This function converts the elapsed ticks of running performance
    counter to time value in unit of nanoseconds.

    @param Ticks The number of elapsed ticks of running performance
        counter.

    @retval UINT64 The elapsed time in nanoseconds.

**/

UINT64
EFIAPI
GetTimeInNanoSecond (
  IN      UINT64                     Ticks
  )
{
  UINT64  Frequency;
  UINT64  NanoSeconds;
  UINT64  Remainder;
  INTN    Shift;

  Frequency = GetPerformanceCounterProperties (NULL, NULL);

  //
  //          Ticks
  // Time = --------- x 1,000,000,000
  //        Frequency
  //
  NanoSeconds = MultU64x32 (DivU64x64Remainder (Ticks, Frequency, &Remainder), 1000000000u);

  //
  // Ensure (Remainder * 1,000,000,000) will not overflow 64-bit.
  // Since 2^29 < 1,000,000,000 = 0x3B9ACA00 < 2^30, Remainder should < 2^(64-30) = 2^34,
  // i.e. highest bit set in Remainder should <= 33.
  //
  Shift = MAX (0, HighBitSet64 (Remainder) - 33);
  Remainder = RShiftU64 (Remainder, (UINTN) Shift);
  Frequency = RShiftU64 (Frequency, (UINTN) Shift);
  NanoSeconds += DivU64x64Remainder (MultU64x32 (Remainder, 1000000000u), Frequency, NULL);

  return NanoSeconds;
}

