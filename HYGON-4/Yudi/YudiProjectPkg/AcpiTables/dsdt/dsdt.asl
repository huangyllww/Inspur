

DefinitionBlock ("", "DSDT", 2, "HYGON ", "Hygon   ", 0x00000002)
{
  External (_SB_.APTS, MethodObj)    // 1 Arguments
  External (_SB_.AWAK, MethodObj)    // 1 Arguments

#include <../JedecNvdimm/JedecNvdimm.asl>
#include <AcpiRam.asi>

  OperationRegion (DBG0, SystemIO, 0x80, One)
  Field (DBG0, ByteAcc, NoLock, Preserve)
  {
      IO80,   8
  }

  OperationRegion (PSMI, SystemIO, 0xB0, 0x02)
  Field (PSMI, ByteAcc, NoLock, Preserve)
  {
      APMC,   8, 
      APMD,   8
  }

  OperationRegion (PMRG, SystemIO, 0x0CD6, 0x02)
  Field (PMRG, ByteAcc, NoLock, Preserve)
  {
      PMRI,   8, 
      PMRD,   8
  }
  IndexField (PMRI, PMRD, ByteAcc, NoLock, Preserve)
  {
          ,   6, 
      HPEN,   1, 
      Offset (0x60), 
      P1EB,   16, 
      Offset (0xF0), 
          ,   3, 
      RSTU,   1
  }

  OperationRegion (GSMM, SystemMemory, 0xFED80000, 0x1000)
  Field (GSMM, AnyAcc, NoLock, Preserve)
  {
      Offset (0x288), 
          ,   1, 
      CLPS,   1, 
      Offset (0x2B0), 
          ,   2, 
      SLPS,   2, 
      Offset (0x3BB), 
          ,   6, 
      PWDE,   1, 
      Offset (0x3E4), 
      BLNK,   2
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

  OperationRegion (IOCC, SystemIO, 0x0400, 0x80)
  Field (IOCC, ByteAcc, NoLock, Preserve)
  {
      Offset (0x01), 
          ,   2, 
      RTCS,   1
  }

  Name (PRWP, Package (0x02)
  {
      Zero, 
      Zero
  })
  Method (GPRW, 2, NotSerialized)
  {
      PRWP [Zero] = Arg0
      PRWP [One] = Arg1
      If ((DAS3 == Zero))
      {
          If ((Arg1 <= 0x03))
          {
              PRWP [One] = Zero
          }
      }

      If ((DAS4 == Zero))
      {
          If ((Arg1 <= 0x04))
          {
              PRWP [One] = Zero
          }
      }

      Return (PRWP) /* \PRWP */
  }

  Method (SPTS, 1, NotSerialized)
  {
      If ((Arg0 == 0x03))
      {
          BLNK = One
      }

      If (((Arg0 == 0x04) || (Arg0 == 0x05)))
      {
          BLNK = Zero
      }

      If ((Arg0 == 0x03))
      {
          RSTU = Zero
      }

      CLPS = One
      SLPS = One
      PEWS = One
      PEWD = Zero
      PWDE = One
  }

  Method (SWAK, 1, NotSerialized)
  {
      BLNK = 0x03
      If ((Arg0 == 0x03))
      {
          RSTU = One
      }

      PEWS = One
      PEWD = One
      PWDE = Zero
  }

  Name (_S0, Package (0x04)  // _S0_: S0 System State
  {
      Zero, 
      Zero, 
      Zero, 
      Zero
  })
  If ((DAS4 == One))
  {
      Name (_S4, Package (0x04)  // _S4_: S4 System State
      {
          0x04, 
          Zero, 
          Zero, 
          Zero
      })
  }

  Name (_S5, Package (0x04)  // _S5_: S5 System State
  {
      0x05, 
      Zero, 
      Zero, 
      Zero
  })
  Name (PICM, Zero)
  Name (GPIC, Zero)
  Method (_PIC, 1, NotSerialized)  // _PIC: Interrupt Model
  {
      PICM = Arg0
      GPIC = Arg0
      If (PICM)
      {
          \_SB.DSPI ()
          \_SB.PCI0.NAPE ()
      }
  }

  Method (_PTS, 1, NotSerialized)  // _PTS: Prepare To Sleep
  {
      SPTS (Arg0)
      If ((Arg0 == One))
      {
          \_SB.S80H (0x51)
      }

      If ((Arg0 == 0x03))
      {
          \_SB.S80H (0x53)
      }

      If ((Arg0 == 0x04))
      {
          \_SB.S80H (0x54)
      }

      If ((Arg0 == 0x05))
      {
          \_SB.S80H (0x55)
          \_SB.BSMI (Zero)
          \_SB.GSMI (0x03)
      }

      \_SB.APTS (Arg0)
  }

  Method (_WAK, 1, NotSerialized)  // _WAK: Wake
  {
      SWAK (Arg0)
      \_SB.AWAK (Arg0)
      If (((Arg0 == 0x03) || (Arg0 == 0x04)))
      {
          If ((GPIC != Zero))
          {
              \_SB.DSPI ()
              \_SB.PCI0.NAPE ()
          }
      }

      If ((Arg0 == 0x03))
      {
          \_SB.S80H (0xE3)
      }

      If ((Arg0 == 0x04))
      {
          \_SB.S80H (0xE4)
      }

      Return (Package (0x02)
      {
          Zero, 
          Zero
      })
  }

  Scope (_SB)
  {
      Name (OSYS, Zero)
      Method (_INI, 0, NotSerialized)  // _INI: Initialize
      {
          If ((GPIC != Zero))
          {
              DSPI ()
              ^PCI0.NAPE ()
          }

          OSYS = 0x07D0
          If (CondRefOf (\_OSI))
          {
              If (_OSI ("Windows 2000"))
              {
                  OSYS = 0x07D0
              }

              If (_OSI ("Windows 2001"))
              {
                  OSYS = 0x07D1
              }

              If (_OSI ("Windows 2001 SP1"))
              {
                  OSYS = 0x07D1
              }

              If (_OSI ("Windows 2001 SP2"))
              {
                  OSYS = 0x07D2
              }

              If (_OSI ("Windows 2001.1"))
              {
                  OSYS = 0x07D3
              }

              If (_OSI ("Windows 2001.1 SP1"))
              {
                  OSYS = 0x07D4
              }

              If (_OSI ("Windows 2006"))
              {
                  OSYS = 0x07D5
              }

              If (_OSI ("Windows 2006 SP1"))
              {
                  OSYS = 0x07D6
              }

              If (_OSI ("Windows 2006.1"))
              {
                  OSYS = 0x07D8
              }

              If (_OSI ("Windows 2009"))
              {
                  OSYS = 0x07D9
              }

              If (_OSI ("Windows 2012"))
              {
                  OSYS = 0x07DC
              }

              If (_OSI ("Windows 2013"))
              {
                  OSYS = 0x07DD
              }

              If (_OSI ("Windows 2015"))
              {
                  OSYS = 0x07DF
              }

              If (_OSI ("Windows 2016"))
              {
                  OSYS = 0x07E0
              }

              If (_OSI ("Windows 2017"))
              {
                  OSYS = 0x07E1
              }

              If (_OSI ("Windows 2017.2"))
              {
                  OSYS = 0x07E1
              }

              If (_OSI ("Windows 2018"))
              {
                  OSYS = 0x07E2
              }

              If (_OSI ("Windows 2018.2"))
              {
                  OSYS = 0x07E2
              }

              If (_OSI ("Windows 2019"))
              {
                  OSYS = 0x07E3
              }

              If (_OSI ("Windows 2020"))
              {
                  OSYS = 0x07E4
              }
          }
      }

      Device (PWRB)
      {
          Name (_HID, EisaId ("PNP0C0C") /* Power Button Device */)  // _HID: Hardware ID
          Method (_STA, 0, NotSerialized)  // _STA: Status
          {
              Return (0x0B)
          }
      }
      
      Method (_OSC, 4, Serialized)  // _OSC: Operating System Capabilities
      {
          CreateDWordField (Arg3, Zero, CDW1)
          CreateDWordField (Arg3, 0x04, CDW2)
          If ((Arg0 == ToUUID ("0811b06e-4a27-44f9-8d60-3cbbc22e7b48") /* Platform-wide Capabilities */))
          {
              If ((Arg1 != One))
              {
                  CDW1 |= 0x08
              }

              CDW2 |= 0x10
              Return (Arg3)
          }
          Else
          {
              CDW1 |= 0x04
              Return (Arg3)
          }
      }        

      Method (GSMI, 1, NotSerialized)
      {
          APMD = Arg0
          APMC = 0xE4
          Sleep (0x02)
      }

      Method (S80H, 1, NotSerialized)
      {
          IO80 = Arg0
      }

      Method (BSMI, 1, NotSerialized)
      {
          APMD = Arg0
          APMC = 0xBE
          Sleep (One)
      }      
  }                                 // _SB
  
  Scope (\_SB){
    include ("link.asl") 
    include ("rp.asl")
  }
  include ("rp1.asl")
  include ("rp2.asl")
  include ("rp3.asl")
  include ("rp4.asl")
  include ("rp5.asl")
  include ("rp6.asl")
  include ("rp7.asl") 
  include ("fch.asl")  
}