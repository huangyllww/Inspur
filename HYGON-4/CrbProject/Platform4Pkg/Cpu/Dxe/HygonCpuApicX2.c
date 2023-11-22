

#include <Uefi.h>
#include <Register/Intel/Cpuid.h>
#include <Register/Amd/Cpuid.h>
#include <Register/Intel/LocalApic.h>
#include <Library/LocalApicLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/TimerLib.h>


#define CPUID_SIGNATURE_AUTHENTIC_HYGON_EBX  0x6F677948
#define CPUID_SIGNATURE_AUTHENTIC_HYGON_EDX  0x6E65476E
#define CPUID_SIGNATURE_AUTHENTIC_HYGON_ECX  0x656E6975



BOOLEAN
EFIAPI
StandardSignatureIsHygon (
  VOID
  )
{
  UINT32  RegEbx;
  UINT32  RegEcx;
  UINT32  RegEdx;

  AsmCpuid (CPUID_SIGNATURE, NULL, &RegEbx, &RegEcx, &RegEdx);
  return (RegEbx == CPUID_SIGNATURE_AUTHENTIC_HYGON_EBX &&
          RegEcx == CPUID_SIGNATURE_AUTHENTIC_HYGON_ECX &&
          RegEdx == CPUID_SIGNATURE_AUTHENTIC_HYGON_EDX);
}






VOID
SendIpi (
  IN UINT32  IcrLow,
  IN UINT32  ApicId
  );

VOID
SendNmiIpi (
  IN UINT32  ApicId
  )
{
  LOCAL_APIC_ICR_LOW  IcrLow;

//DEBUG((EFI_D_INFO, "SendNmiIpi(%d)\n", ApicId));

  IcrLow.Uint32            = 0;
  IcrLow.Bits.DeliveryMode = LOCAL_APIC_DELIVERY_MODE_NMI;
  SendIpi (IcrLow.Uint32, ApicId);
}


#if 0
VOID
_SendInitSipiSipiAllExcludingSelf (
  IN UINT32  StartupRoutine
  )
{
  LOCAL_APIC_ICR_LOW  IcrLow;

  ASSERT (StartupRoutine < 0x100000);
  ASSERT ((StartupRoutine & 0xfff) == 0);

  SendInitIpiAllExcludingSelf ();
  MicroSecondDelay (PcdGet32 (PcdCpuInitIpiDelayInMicroSeconds));
  IcrLow.Uint32                    = 0;
  IcrLow.Bits.Vector               = (StartupRoutine >> 12);
  IcrLow.Bits.DeliveryMode         = LOCAL_APIC_DELIVERY_MODE_STARTUP;
  IcrLow.Bits.Level                = 1;
  IcrLow.Bits.DestinationShorthand = LOCAL_APIC_DESTINATION_SHORTHAND_ALL_EXCLUDING_SELF;
  SendIpi (IcrLow.Uint32, 0);

  MicroSecondDelay (200);
  SendIpi (IcrLow.Uint32, 0);
}
#endif

