

#ifndef MDEPKG_NDEBUG      // debug mode
#define ASL_COM(x)   DBGC x
#else
#define ASL_COM(x)
#endif

DefinitionBlock ("dsdt.aml", "DSDT", 2, "HIGON", "Higon ", 0x00000001)
{
//    External (\_SB_.ALIB, MethodObj)    // 2 Arguments
//    External (APTS, MethodObj)          // 1 Arguments
//    External (AWAK, MethodObj)          // 1 Arguments

    include ("AcpiRam.asl")

    Name (PEBS, 0xF0000000)
    Name (PEBL, 0x08000000)
    Name (SMBB, 0x0B20)
    Name (SMBL, 0x20)
    Name (SMB0, 0x0B00)
    Name (SMBM, 0x10)
    Name (PMBS, 0x0800)
    Name (PMLN, 0xA0)
    Name (SMIO, 0xB2)
    Name (GPBS, Zero)
    Name (GPLN, Zero)
    Name (APCB, 0xFEC00000)
    Name (APCL, 0x1000)
    Name (SPIB, 0xFEC10000)
    Name (SPIL, 0x1000)
    Name (WDTB, Zero)
    Name (WDTL, Zero)
    Name (ICAE, 0x00)
    Name (ICBE, 0x00)
    Name (ICCE, 0x00)
    Name (ICDE, 0x00)
    Name (ICEE, 0x00)
    Name (ICFE, 0x00)
    Name (UT0E, 0x00)
    Name (UT1E, 0x00)
    Name (PEHP, One)
    Name (SHPC, Zero)
    Name (PEPM, One)
    Name (PEER, One)
    Name (PECS, One)
    Name (PICM, Zero)
    Name (OSVR, Ones)    
    Name (SS1, Zero)
    Name (SS2, Zero)
    Name (SS3, One)
    Name (SS4, Zero)
    Name (VGAF, One)

    include ("misc.asl")    
    
    OperationRegion (ACMS, SystemIO, 0x72, 0x10)
    Field (ACMS, ByteAcc, NoLock, Preserve){
        Offset (0x0E), 
        P80,    8
    }

    OperationRegion (CMPT, SystemIO, 0x0C50, 0x03)
    Field (CMPT, ByteAcc, NoLock, Preserve)
    {
        CMID,   8, 
            ,   6, 
        GPCT,   2, 
        GP0I,   1, 
        GP1I,   1, 
        GP2I,   1, 
        GP3I,   1, 
        GP4I,   1, 
        GP5I,   1, 
        GP6I,   1, 
        GP7I,   1
    }

    OperationRegion (PCFG, SystemMemory, PEBS, 0x02000000)
    Field (PCFG, AnyAcc, NoLock, Preserve)
    {
        Offset (0x20078), 
        Offset (0x2007A), 
        PMSA,   1, 
        Offset (0x28078), 
        Offset (0x2807A), 
        PMSB,   1, 
        Offset (0x48078), 
        Offset (0x4807A), 
        PMSC,   1, 
        Offset (0x8800A), 
        STCL,   16, 
        Offset (0x880FC), 
        RMBA,   32, 
        Offset (0xA0004), 
        SMIE,   1, 
        SMME,   1, 
        Offset (0xA0008), 
        RVID,   8, 
        Offset (0xA0014), 
        SMB1,   32, 
        Offset (0xA0082), 
            ,   2, 
        G31O,   1, 
        Offset (0xA00E1), 
            ,   6, 
        ACIR,   1, 
        Offset (0xA4004), 
        PCMD,   2, 
        Offset (0xA807A), 
        PMS0,   1, 
        Offset (0xA8088), 
        TLS0,   4, 
        Offset (0xA907A), 
        PMS1,   1, 
        Offset (0xA9088), 
        TLS1,   4, 
        Offset (0xAA07A), 
        PMS2,   1, 
        Offset (0xAA088), 
        TLS2,   4, 
        Offset (0xAB07A), 
        PMS3,   1, 
        Offset (0xAB088), 
        TLS3,   4
    }

    OperationRegion (PMIO, SystemIO, 0x0CD6, 0x02)
    Field (PMIO, ByteAcc, NoLock, Preserve){
        INPM,   8, 
        DAPM,   8
    }

    IndexField (INPM, DAPM, ByteAcc, NoLock, Preserve){
            ,   6, 
        HPEN,   1, 
        Offset (0x60), 
        P1EB,   16, 
        Offset (0xC8), 
            ,   2, 
        SPRE,   1, 
        TPDE,   1, 
        Offset (0xF0), 
            ,   3, 
        RSTU,   1
    }

    OperationRegion (ERMM, SystemMemory, 0xFED80000, 0x1800)
    Field (ERMM, AnyAcc, NoLock, Preserve){
        Offset (0x200), 
            ,   9, 
        GESA,   1, 
        Offset (0x208), 
            ,   5, 
        E05C,   1, 
        Offset (0x209), 
        E08C,   1, 
        E09C,   1, 
        E10C,   1, 
        Offset (0x20B), 
            ,   5, 
        E1DC,   1, 
        Offset (0x296), 
            ,   7, 
        TMSE,   1, 
        Offset (0x2B0), 
            ,   2, 
        SLPS,   2, 
        Offset (0x377), 
        EPNM,   1, 
        DPPF,   1, 
        Offset (0x3BB), 
            ,   6, 
        PWDE,   1, 
        Offset (0x3BE), 
            ,   5, 
        ALLS,   1, 
        Offset (0x3F0), 
        PHYD,   1, 
            ,   1, 
            ,   1, 
        US5R,   1, 
        Offset (0x3F6), 
        GECD,   1, 
        Offset (0xD06), 
        G06M,   8, 
        G07M,   8, 
        Offset (0xD0B), 
        G0BM,   8, 
        Offset (0xD0E), 
        G0EM,   8, 
        Offset (0xD14), 
        G14M,   8, 
        Offset (0xD1C), 
        G1CM,   8, 
        Offset (0xD20), 
        G20M,   8, 
        Offset (0xD23), 
        G23M,   8, 
        Offset (0xD2D), 
        G2DM,   8, 
        Offset (0xD37), 
        G37M,   8, 
        Offset (0xD70), 
        G70M,   8, 
        Offset (0xD76), 
        G76M,   8, 
        Offset (0xDAF), 
        GAFM,   8, 
        GB0M,   8, 
        Offset (0xE80), 
            ,   2, 
        IMCS,   1, 
        Offset (0x1518), 
        Offset (0x151A), 
        GE10,   1, 
        Offset (0x1524), 
        Offset (0x1526), 
        GE22,   1, 
        Offset (0x1538), 
        Offset (0x153A), 
        G66I,   1, 
            ,   5, 
        G66O,   1, 
        G66E,   1, 
        Offset (0x1560), 
        Offset (0x1562), 
        G6FI,   1, 
            ,   5, 
        G6FO,   1, 
        G6FE,   1, 
        Offset (0x1604), 
        Offset (0x1606), 
        G50I,   1, 
            ,   5, 
        G50O,   1, 
        G50E,   1, 
        Offset (0x1608), 
        Offset (0x160A), 
        G51I,   1, 
            ,   5, 
        G51O,   1, 
        G51E,   1, 
        Offset (0x1610), 
        Offset (0x1612), 
        G57I,   1, 
            ,   5, 
        G57O,   1, 
        G57E,   1, 
        Offset (0x1618), 
        Offset (0x161A), 
        G59I,   1, 
            ,   5, 
        G59O,   1, 
        G59E,   1
    }

    OperationRegion (ABIO, SystemIO, 0x0CD8, 0x08)
    Field (ABIO, DWordAcc, NoLock, Preserve){
        INAB,   32, 
        DAAB,   32
    }

    Method (RDAB, 1, NotSerialized)
    {
        Store (Arg0, INAB)
        Return (DAAB)
    }

    Method (WTAB, 2, NotSerialized)
    {
        Store (Arg0, INAB)
        Store (Arg1, DAAB)
    }

    Method (RWAB, 3, NotSerialized)
    {
        And (RDAB (Arg0), Arg1, Local0)
        Or (Local0, Arg2, Local1)
        WTAB (Arg0, Local1)
    }

    Method (CABR, 3, NotSerialized)
    {
        ShiftLeft (Arg0, 0x05, Local0)
        Add (Local0, Arg1, Local1)
        ShiftLeft (Local1, 0x18, Local2)
        Add (Local2, Arg2, Local3)
        Return (Local3)
    }



    OperationRegion (P1E0, SystemIO, P1EB, 0x04)
    Field (P1E0, ByteAcc, NoLock, Preserve)
    {
            ,   14, 
        PEWS,   1, 
        WSTA,   1, 
            ,   14, 
        PEWD,   1
    }

    Method (SPTS, 1, NotSerialized)
    {
        Store (One, PCMD)
        Store (Arg0, P80)
        If (IMCS)
        {
            IBSE ()
        }

        CPMS ()
        CPMS ()
        Store (One, PEWS)
    }

    Method (SWAK, 1, NotSerialized)
    {
        If (PICM)
        {
            \_SB.IRQC ()
        }

        If (TMSE)
        {
            Store (Zero, TMSE)
        }

        CPMS ()
        CPMS ()
        Store (One, PEWS)
        If (IMCS)
        {
            IBWE ()
        }
    }

    Method (CPMS, 0, NotSerialized)
    {
        If (LEqual (EPNM, Zero))
        {
            Store (One, PMSA)
            Store (One, PMSB)
            Store (One, PMSC)
            Store (One, PMS0)
            Store (One, PMS1)
            Store (One, PMS2)
            Store (One, PMS3)
        }
    }

    OperationRegion (IMIO, SystemIO, 0x3E, 0x02)
    Field (IMIO, ByteAcc, NoLock, Preserve){
        IMCX,   8, 
        IMCA,   8
    }

    IndexField (IMCX, IMCA, ByteAcc, NoLock, Preserve){
        Offset (0x80), 
        MSTI,   8, 
        MITS,   8, 
        MRG0,   8, 
        MRG1,   8, 
        MRG2,   8, 
        MRG3,   8
    }

    Method (WACK, 0, NotSerialized){
        Store (Zero, Local0)
        While (LNotEqual (Local0, 0xFA))
        {
            Store (MRG0, Local0)
            Sleep (0x0A)
        }
    }

    Method (IBSE, 0, NotSerialized){
        Store (Zero, MRG0)
        Store (0xB5, MRG1)
        Store (Zero, MRG2)
        Store (0x96, MSTI)
        WACK ()
        Store (Zero, MRG0)
        Store (Zero, MRG1)
        Store (Zero, MRG2)
        Store (0x80, MSTI)
        WACK ()
        Store (MRG2, Local0)
        And (Local0, 0xFE, Local0)
        Store (Zero, MRG0)
        Store (Zero, MRG1)
        Store (Local0, MRG2)
        Store (0x81, MSTI)
        WACK ()
        Store (Zero, MRG0)
        Store (One, MRG1)
        Store (Zero, MRG2)
        Store (0x98, MSTI)
        WACK ()
        Store (Zero, MRG0)
        Store (0xB4, MRG1)
        Store (Zero, MRG2)
        Store (0x96, MSTI)
        WACK ()
    }

    Method (IBWE, 0, NotSerialized){
        Store (Zero, MRG0)
        Store (0xB5, MRG1)
        Store (Zero, MRG2)
        Store (0x96, MSTI)
        WACK ()
        Store (Zero, MRG0)
        Store (Zero, MRG1)
        Store (Zero, MRG2)
        Store (0x80, MSTI)
        WACK ()
        Store (MRG2, Local0)
        Or (Local0, One, Local0)
        Store (Zero, MRG0)
        Store (Zero, MRG1)
        Store (Local0, MRG2)
        Store (0x81, MSTI)
        WACK ()
    }

    Scope (_SB)
    {
        Method (_OSC, 4, NotSerialized)  // Operating System Capabilities
        {
            Name (SUPP, Zero)
            Name (CTRL, Zero)
            CreateDWordField (Arg3, Zero, CDW1)
            If (LEqual (Arg0, Buffer (0x10)
                    {
                       0x6E, 0xB0, 0x11, 0x08, 0x27, 0x4A, 0xF9, 0x44,
                       0x8D, 0x60, 0x3C, 0xBB, 0xC2, 0x2E, 0x7B, 0x48
                    }))
            {
                CreateDWordField (Arg3, 0x04, CDW2)
                If (LNotEqual (Arg1, One))
                {
                    Or (CDW1, 0x08, CDW1)
                }

                Or (CDW2, 0x10, CDW2)
                Return (Arg3)
            }
            Else
            {
                Or (CDW1, 0x04, CDW1)
                Return (Arg3)
            }
        }
    }



//------------------------------------------------------------------------------
/*

6.2.13 _PXM (Proximity)
This optional object is used to describe proximity domains within a machine. _PXM evaluates to an
integer that identifies the device as belonging to a specific proximity domain. OSPM assumes that
two devices in the same proximity domain are tightly coupled. OSPM could choose to optimize its
behavior based on this. For example, in a system with four processors and six memory devices, there
might be two separate proximity domains (0 and 1), each with two processors and three memory
devices. In this case, the OS may decide to run some software threads on the processors in proximity
domain 0 and others on the processors in proximity domain 1. Furthermore, for performance
reasons, it could choose to allocate memory for those threads from the memory devices inside the
proximity domain common to the processor and the memory device rather than from a memory
device outside of the processor’s proximity domain. _PXM can be used to identify any device
belonging to a proximity domain. Children of a device belong to the same proximity domain as their
parent unless they contain an overriding _PXM. Proximity domains do not imply any ejection
relationships.
An OS makes no assumptions about the proximity or nearness of different proximity domains. The
difference between two integers representing separate proximity domains does not imply distance
between the proximity domains (in other words, proximity domain 1 is not assumed to be closer to
proximity domain 0 than proximity domain 6).
If the Local APIC ID / Local SAPIC ID / Local x2APIC ID of a dynamically added processor is not
present in the System Resource Affinity Table (SRAT), a _PXM object must exist for the
processor’s device or one of its ancestors in the ACPI Namespace.

6.5.5 _BBN (Base Bus Number)
For multi-root PCI platforms, the _BBN object evaluates to the PCI bus number that the BIOS
assigns. This is needed to access a PCI_Config operation region for the specific bus. The _BBN
object is located under a PCI host bridge and must be unique for every host bridge within a segment
since it is the PCI bus number.

*/

    include ("pcitree.asl")
    include ("Link.asl")

    Scope(_SB){
      include ("Rp.asl")
      include ("Rp1.asl")
      include ("Rp2.asl")
      include ("Rp3.asl")
      include ("Rp4.asl")
      include ("Rp5.asl")
      include ("Rp6.asl")
      include ("Rp7.asl")       
    }

    include ("Sleep.asl")
    include ("Gpe.asl")

    Scope (_SB.PCI0)
    {
        Method (BN00, 0, NotSerialized)
        {
            Return (\R0BB)
        }

        Method (_PXM, 0, NotSerialized)  // _PXM: Device Proximity
        {
            Return (Zero)
        }
    }

    Scope (_SB.S0D1)
    {
        Method (BN01, 0, NotSerialized)
        {
            Return (\R1BB)
        }

        Method (_PXM, 0, NotSerialized)  // _PXM: Device Proximity
        {
            Return (One)
        }
    }

    Scope (_SB.S0D2)
    {
        Method (BN02, 0, NotSerialized)
        {
            Return (\R2BB)
        }

        Method (_PXM, 0, NotSerialized)  // _PXM: Device Proximity
        {
            Return (0x02)
        }
    }

    Scope (_SB.S0D3)
    {
        Method (BN03, 0, NotSerialized)
        {
            Return (\R3BB)
        }

        Method (_PXM, 0, NotSerialized)  // _PXM: Device Proximity
        {
            Return (0x03)
        }
    }

    Scope (_SB.S1D0)
    {
        Method (BN04, 0, NotSerialized)
        {
            Return (\R4BB)
        }

        Method (_PXM, 0, NotSerialized)  // _PXM: Device Proximity
        {
            Return (0x04)
        }
    }

    Scope (_SB.S1D1)
    {
        Method (BN05, 0, NotSerialized)
        {
            Return (\R5BB)
        }

        Method (_PXM, 0, NotSerialized)  // _PXM: Device Proximity
        {
            Return (0x05)
        }
    }

    Scope (_SB.S1D2)
    {
        Method (BN06, 0, NotSerialized)
        {
            Return (\R6BB)
        }

        Method (_PXM, 0, NotSerialized)  // _PXM: Device Proximity
        {
            Return (0x06)
        }
    }

    Scope (_SB.S1D3)
    {
        Method (BN07, 0, NotSerialized)
        {
            Return (\R7BB)
        }

        Method (_PXM, 0, NotSerialized)  // _PXM: Device Proximity
        {
            Return (0x07)
        }
    }

#if defined TCM_ENABLE && TCM_ENABLE == 1
#include "Tcm.asl"
#endif    
      Device (\_SB.FTPM)
  {
      Name(_HID, "HYGT0101")
      Name(_STR, Unicode("Hygon fTPM"))
   
      Method(_STA, 0){
        if(LEqual(\FTPP, 0)){
          Return(0)
        }
        Return (0x0f)
      }
  }

    
}

