

Device (PCI0)
{
    Name (_HID, EisaId ("PNP0A08") /* PCI Express Bus */)  // _HID: Hardware ID
    Name (_CID, EisaId ("PNP0A03") /* PCI Bus */)  // _CID: Compatible ID
    Name (_UID, Zero)  // _UID: Unique ID
    Name (_BBN, Zero)  // _BBN: BIOS Bus Number
    Name (SUPP, Zero)
    Name (CTRL, Zero)
    Name (VGAF, One)
    Method (_PXM, 0, NotSerialized)  // _PXM: Device Proximity
    {
        Return (\PXM0)
    }

    Method (_OSC, 4, NotSerialized)  // _OSC: Operating System Capabilities
    {
        CreateDWordField (Arg3, Zero, CDW1)
        CreateDWordField (Arg3, 0x04, CDW2)
        CreateDWordField (Arg3, 0x08, CDW3)
        If ((Arg0 == ToUUID ("33db4d5b-1ff7-401c-9657-7441c03dd766") /* PCI Host Bridge Device */))
        {
            SUPP = CDW2 /* \_SB_.PCI0._OSC.CDW2 */
            CTRL = CDW3 /* \_SB_.PCI0._OSC.CDW3 */
            If (((SUPP & 0x16) != 0x16))
            {
                CTRL &= 0x1E
            }

            If ((PFEH == One))
            {
                CTRL &= 0x15
            }
            Else
            {
                CTRL &= 0x1D
            }

            If (~(CDW1 & One))
            {
                If ((CTRL & One)){}
                If ((CTRL & 0x04)){}
                If ((CTRL & 0x10)){}
            }

            If ((Arg1 != One))
            {
                CDW1 |= 0x08
            }

            If ((CDW3 != CTRL))
            {
                CDW1 |= 0x10
            }

            CDW3 = CTRL /* \_SB_.PCI0.CTRL */
            Return (Arg3)
        }
        Else
        {
            CDW1 |= 0x04
            Return (Arg3)
        }
    }

    Method (TOM, 0, NotSerialized)
    {
        Local0 = (TOML * 0x00010000)
        Local1 = (TOMH * 0x01000000)
        Local0 += Local1
        Return (Local0)
    }

    Name (CRS1, ResourceTemplate ()
    {
        WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
            0x0000,             // Granularity
            0x0000,             // Range Minimum
            0x007F,             // Range Maximum
            0x0000,             // Translation Offset
            0x0080,             // Length
            ,, _Y01
            )

        IO (Decode16,
            0x0CF8,             // Range Minimum
            0x0CF8,             // Range Maximum
            0x01,               // Alignment
            0x08,               // Length
            )

        WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
            0x0000,             // Granularity
            0x0000,             // Range Minimum
            0x02F7,             // Range Maximum
            0x0000,             // Translation Offset
            0x02f8,             // Length
            ,, , TypeStatic
            )
        WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
            0x0000,             // Granularity
            0x0300,             // Range Minimum
            0x03AF,             // Range Maximum
            0x0000,             // Translation Offset
            0x00b0,             // Length
            ,, , TypeStatic
            )                    
            
        WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
            0x0000,             // Granularity
            0x0400,             // Range Minimum
            0x0CF7,             // Range Maximum
            0x0000,             // Translation Offset
            0x08F8,             // Length
            ,, , TypeStatic)
        WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
            0x0000,             // Granularity
            0x0000,             // Range Minimum
            0x0000,             // Range Maximum
            0x0000,             // Translation Offset
            0x0000,             // Length
            ,, _Y03, TypeStatic)
        WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
            0x0000,             // Granularity
            0x0D00,             // Range Minimum
            0x0FFF,             // Range Maximum
            0x0000,             // Translation Offset
            0x0300,             // Length
            ,, _Y02, TypeStatic)
        DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
            0x00000000,         // Granularity
            0x00000000,         // Range Minimum
            0x00000000,         // Range Maximum
            0x00000000,         // Translation Offset
            0x00000000,         // Length
            ,, _Y04, AddressRangeMemory, TypeStatic)
        DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
            0x00000000,         // Granularity
            0x000C0000,         // Range Minimum
            0x000DFFFF,         // Range Maximum
            0x00000000,         // Translation Offset
            0x00020000,         // Length
            ,, , AddressRangeMemory, TypeStatic)
        DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
            0x00000000,         // Granularity
            0x02000000,         // Range Minimum
            0xFFDFFFFF,         // Range Maximum
            0x00000000,         // Translation Offset
            0xFDE00000,         // Length
            ,, _Y05, AddressRangeMemory, TypeStatic)

        QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
            0x0000000000000000, // Granularity
            0x0000000000000000, // Range Minimum
            0x0000000000000000, // Range Maximum
            0x0000000000000000, // Translation Offset
            0x0000000000000000, // Length
            ,, _Y06, AddressRangeMemory, TypeStatic)

    })

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
      CreateWordField (CRS1, \_SB.PCI0._Y01._MIN, MIN0)  // _MIN: Minimum Base Address
      CreateWordField (CRS1, \_SB.PCI0._Y01._MAX, MAX0)  // _MAX: Maximum Base Address
      CreateWordField (CRS1, \_SB.PCI0._Y01._LEN, LEN0)  // _LEN: Length
      Store(\R0BB, MIN0)
      Store(\R0BL, MAX0)
      Subtract(MAX0, MIN0, Local0)
      Add(Local0, 1, LEN0)

      CreateWordField (CRS1, \_SB.PCI0._Y02._MIN, MIN1)  // _MIN: Minimum Base Address
      CreateWordField (CRS1, \_SB.PCI0._Y02._MAX, MAX1)  // _MAX: Maximum Base Address
      CreateWordField (CRS1, \_SB.PCI0._Y02._LEN, LEN1)  // _LEN: Length
      Store (\R0IB, MIN1)
      Store (\R0IL, LEN1)
      Store (LEN1, Local0)
      Add (MIN1, Decrement (Local0), MAX1)

  //              ASL_COM((2, "MIN1 ", MIN1))
  //              ASL_COM((2, "MAX1 ", MAX1))
  //              ASL_COM((2, "LEN1 ", LEN1))
  //              ASL_COM((2, "GFX0 ", \GFX0))

      If (\VEDI == 0)
      {
          If (VGAF)
          {
              CreateWordField (CRS1, \_SB.PCI0._Y03._MIN, IMN1)  // _MIN: Minimum Base Address
              CreateWordField (CRS1, \_SB.PCI0._Y03._MAX, IMX1)  // _MAX: Maximum Base Address
              CreateWordField (CRS1, \_SB.PCI0._Y03._LEN, ILN1)  // _LEN: Length
              Store (0x03B0, IMN1)
              Store (0x03DF, IMX1)
              Store (0x30, ILN1)

              CreateDWordField (CRS1, \_SB.PCI0._Y04._MIN, VMN1)  // _MIN: Minimum Base Address
              CreateDWordField (CRS1, \_SB.PCI0._Y04._MAX, VMX1)  // _MAX: Maximum Base Address
              CreateDWordField (CRS1, \_SB.PCI0._Y04._LEN, VLN1)  // _LEN: Length
              Store (0x000A0000, VMN1)
              Store (0x000BFFFF, VMX1)
              Store (0x00020000, VLN1)
              Store (Zero, VGAF)
          }
      }

      CreateDWordField (CRS1, \_SB.PCI0._Y05._MIN, MIN3)  // _MIN: Minimum Base Address
      CreateDWordField (CRS1, \_SB.PCI0._Y05._MAX, MAX3)  // _MAX: Maximum Base Address
      CreateDWordField (CRS1, \_SB.PCI0._Y05._LEN, LEN3)  // _LEN: Length
      Store (\RP0B, MIN3)
      Store (\RP0L, LEN3)
      Store (LEN3, Local0)
      Add (MIN3, Decrement (Local0), MAX3)

      CreateQWordField (CRS1, \_SB.PCI0._Y06._MIN, MIN8)  // _MIN: Minimum Base Address
      CreateQWordField (CRS1, \_SB.PCI0._Y06._MAX, MAX8)  // _MAX: Maximum Base Address
      CreateQWordField (CRS1, \_SB.PCI0._Y06._LEN, LEN8)  // _LEN: Length
      Store (\R0HB, MIN8)
      Store (\R0HL, LEN8)
      Store (LEN8, Local0)
      Add (MIN8, Decrement (Local0), MAX8)

      Return (CRS1)
    }

    Device (MEMR)
    {
        Name (_HID, EisaId ("PNP0C02") /* PNP Motherboard Resources */)  // _HID: Hardware ID
        Name (BAR3, 0xFFF00000)
        Name (MEM1, ResourceTemplate ()
        {
            Memory32Fixed (ReadWrite,
                0x00000000,         // Address Base
                0x00000000,         // Address Length
                _Y0E)
            Memory32Fixed (ReadWrite,
                0x00000000,         // Address Base
                0x00000000,         // Address Length
                _Y0F)
            Memory32Fixed (ReadWrite,
                0x00000000,         // Address Base
                0x00000000,         // Address Length
                _Y10)
        })
        Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
        {
            CreateDWordField (MEM1, \_SB.PCI0.MEMR._Y0E._BAS, MB01)  // _BAS: Base Address
            CreateDWordField (MEM1, \_SB.PCI0.MEMR._Y0E._LEN, ML01)  // _LEN: Length
            CreateDWordField (MEM1, \_SB.PCI0.MEMR._Y0F._BAS, MB02)  // _BAS: Base Address
            CreateDWordField (MEM1, \_SB.PCI0.MEMR._Y0F._LEN, ML02)  // _LEN: Length
            CreateDWordField (MEM1, \_SB.PCI0.MEMR._Y10._BAS, MB03)  // _BAS: Base Address
            CreateDWordField (MEM1, \_SB.PCI0.MEMR._Y10._LEN, ML03)  // _LEN: Length
            If (GPIC)
            {
                MB01 = 0xFEC00000
                MB02 = 0xFEE00000
                ML01 = 0x1000
                ML01 += 0x1000
                ML02 = 0x1000
            }

            MB03 = 0xFF000000
            ML03 = 0x01000000
            Return (MEM1) /* \_SB_.PCI0.MEMR.MEM1 */
        }
    }

    OperationRegion (NAPC, PCI_Config, 0xB4, 0x0C)
    Field (NAPC, DWordAcc, NoLock, Preserve)
    {
        NAEX,   32, 
        NAPX,   32, 
        NAPD,   32
    }

    Name (PKGN, One)
    Mutex (NAPM, 0x00)
    Method (NAPE, 0, NotSerialized)
    {
        Acquire (NAPM, 0xFFFF)
        NAEX = Zero
        NAPX = 0x0001C604
        Local0 = NAPD /* \_SB_.PCI0.NAPD */
        Local0 &= 0xFF000000
        Local0 >>= 0x18
        NAEX = Zero
        NAPX = 0x0001C600
        PKGN = NAPD /* \_SB_.PCI0.NAPD */
        PKGN &= 0xFF
        FindSetLeftBit (PKGN, Local3)
        Local4 = (Local0 / Local3)
        Local5 = Zero
        While ((Local5 < Local3))
        {
            Local1 = 0x14300000
            Local6 = Zero
            While ((Local6 < Local4))
            {
                NAEX = Local5
                NAPX = Local1
                Local2 = NAPD /* \_SB_.PCI0.NAPD */
                Local2 &= 0xFFFFFFEF
                NAPD = Local2
                Local6++
                Local1 += 0x00100000
            }

            Local5++
        }

        NAEX = Zero
        Release (NAPM)
    }

    Name (PR00, Package (0x18)
    {
        Package (0x04)
        {
            0x0001FFFF, 
            Zero, 
            LNKA, 
            Zero
        }, 

        Package (0x04)
        {
            0x0001FFFF, 
            One, 
            LNKB, 
            Zero
        }, 

        Package (0x04)
        {
            0x0001FFFF, 
            0x02, 
            LNKC, 
            Zero
        }, 

        Package (0x04)
        {
            0x0001FFFF, 
            0x03, 
            LNKD, 
            Zero
        }, 

        Package (0x04)
        {
            0x0002FFFF, 
            Zero, 
            LNKE, 
            Zero
        }, 

        Package (0x04)
        {
            0x0002FFFF, 
            One, 
            LNKF, 
            Zero
        }, 

        Package (0x04)
        {
            0x0002FFFF, 
            0x02, 
            LNKG, 
            Zero
        }, 

        Package (0x04)
        {
            0x0002FFFF, 
            0x03, 
            LNKH, 
            Zero
        }, 

        Package (0x04)
        {
            0x0003FFFF, 
            Zero, 
            LNKG, 
            Zero
        }, 

        Package (0x04)
        {
            0x0003FFFF, 
            One, 
            LNKH, 
            Zero
        }, 

        Package (0x04)
        {
            0x0003FFFF, 
            0x02, 
            LNKE, 
            Zero
        }, 

        Package (0x04)
        {
            0x0003FFFF, 
            0x03, 
            LNKF, 
            Zero
        }, 

        Package (0x04)
        {
            0x0004FFFF, 
            Zero, 
            LNKC, 
            Zero
        }, 

        Package (0x04)
        {
            0x0004FFFF, 
            One, 
            LNKD, 
            Zero
        }, 

        Package (0x04)
        {
            0x0004FFFF, 
            0x02, 
            LNKA, 
            Zero
        }, 

        Package (0x04)
        {
            0x0004FFFF, 
            0x03, 
            LNKB, 
            Zero
        }, 

        Package (0x04)
        {
            0x0007FFFF, 
            Zero, 
            LNKH, 
            Zero
        }, 

        Package (0x04)
        {
            0x0007FFFF, 
            One, 
            LNKE, 
            Zero
        }, 

        Package (0x04)
        {
            0x0007FFFF, 
            0x02, 
            LNKF, 
            Zero
        }, 

        Package (0x04)
        {
            0x0007FFFF, 
            0x03, 
            LNKG, 
            Zero
        }, 

        Package (0x04)
        {
            0x0008FFFF, 
            Zero, 
            LNKB, 
            Zero
        }, 

        Package (0x04)
        {
            0x0008FFFF, 
            One, 
            LNKC, 
            Zero
        }, 

        Package (0x04)
        {
            0x0008FFFF, 
            0x02, 
            LNKD, 
            Zero
        }, 

        Package (0x04)
        {
            0x0008FFFF, 
            0x03, 
            LNKA, 
            Zero
        }
    })
    Name (AR00, Package (0x18)
    {
        Package (0x04)
        {
            0x0001FFFF, 
            Zero, 
            Zero, 
            0x18
        }, 

        Package (0x04)
        {
            0x0001FFFF, 
            One, 
            Zero, 
            0x19
        }, 

        Package (0x04)
        {
            0x0001FFFF, 
            0x02, 
            Zero, 
            0x1A
        }, 

        Package (0x04)
        {
            0x0001FFFF, 
            0x03, 
            Zero, 
            0x1B
        }, 

        Package (0x04)
        {
            0x0002FFFF, 
            Zero, 
            Zero, 
            0x34
        }, 

        Package (0x04)
        {
            0x0002FFFF, 
            One, 
            Zero, 
            0x35
        }, 

        Package (0x04)
        {
            0x0002FFFF, 
            0x02, 
            Zero, 
            0x36
        }, 

        Package (0x04)
        {
            0x0002FFFF, 
            0x03, 
            Zero, 
            0x37
        }, 

        Package (0x04)
        {
            0x0003FFFF, 
            Zero, 
            Zero, 
            0x36
        }, 

        Package (0x04)
        {
            0x0003FFFF, 
            One, 
            Zero, 
            0x37
        }, 

        Package (0x04)
        {
            0x0003FFFF, 
            0x02, 
            Zero, 
            0x34
        }, 

        Package (0x04)
        {
            0x0003FFFF, 
            0x03, 
            Zero, 
            0x35
        }, 

        Package (0x04)
        {
            0x0004FFFF, 
            Zero, 
            Zero, 
            0x1A
        }, 

        Package (0x04)
        {
            0x0004FFFF, 
            One, 
            Zero, 
            0x1B
        }, 

        Package (0x04)
        {
            0x0004FFFF, 
            0x02, 
            Zero, 
            0x18
        }, 

        Package (0x04)
        {
            0x0004FFFF, 
            0x03, 
            Zero, 
            0x19
        }, 

        Package (0x04)
        {
            0x0007FFFF, 
            Zero, 
            Zero, 
            0x2F
        }, 

        Package (0x04)
        {
            0x0007FFFF, 
            Zero, 
            One, 
            0x2C
        }, 

        Package (0x04)
        {
            0x0007FFFF, 
            Zero, 
            0x02, 
            0x2D
        }, 

        Package (0x04)
        {
            0x0007FFFF, 
            Zero, 
            0x03, 
            0x2E
        }, 

        Package (0x04)
        {
            0x0008FFFF, 
            Zero, 
            Zero, 
            0x29
        }, 

        Package (0x04)
        {
            0x0008FFFF, 
            Zero, 
            One, 
            0x2A
        }, 

        Package (0x04)
        {
            0x0008FFFF, 
            Zero, 
            0x02, 
            0x2B
        }, 

        Package (0x04)
        {
            0x0008FFFF, 
            Zero, 
            0x03, 
            0x28
        }
    })
    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
    {
        If (PICM)
        {
            Return (AR00) /* \_SB_.PCI0.AR00 */
        }
        Else
        {
            Return (PR00) /* \_SB_.PCI0.PR00 */
        }
    }

    Device (GPP0)
    {
        Name (_ADR, 0x00010001)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR01, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKA, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKB, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKC, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKD, 
                Zero
            }
        })
        Name (AR01, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x18
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x19
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x1A
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x1B
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR01) /* \_SB_.PCI0.GPP0.AR01 */
            }
            Else
            {
                Return (PR01) /* \_SB_.PCI0.GPP0.PR01 */
            }
        }
    }

    Device (GPP1)
    {
        Name (_ADR, 0x00010002)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR02, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKE, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKF, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKG, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKH, 
                Zero
            }
        })
        Name (AR02, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x1C
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x1D
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x1E
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x1F
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR02) /* \_SB_.PCI0.GPP1.AR02 */
            }
            Else
            {
                Return (PR02) /* \_SB_.PCI0.GPP1.PR02 */
            }
        }
    }

    Device (GPP2)
    {
        Name (_ADR, 0x00010003)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR03, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKA, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKB, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKC, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKD, 
                Zero
            }
        })
        Name (AR03, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x20
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x21
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x22
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x23
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR03) /* \_SB_.PCI0.GPP2.AR03 */
            }
            Else
            {
                Return (PR03) /* \_SB_.PCI0.GPP2.PR03 */
            }
        }
    }

    Device (GPP3)
    {
        Name (_ADR, 0x00010004)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR04, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKE, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKF, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKG, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKH, 
                Zero
            }
        })
        Name (AR04, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x24
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x25
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x26
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x27
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR04) /* \_SB_.PCI0.GPP3.AR04 */
            }
            Else
            {
                Return (PR04) /* \_SB_.PCI0.GPP3.PR04 */
            }
        }
    }

    Device (GPP4)
    {
        Name (_ADR, 0x00010005)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR05, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKA, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKB, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKC, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKD, 
                Zero
            }
        })
        Name (AR05, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x28
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x29
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x2A
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x2B
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR05) /* \_SB_.PCI0.GPP4.AR05 */
            }
            Else
            {
                Return (PR05) /* \_SB_.PCI0.GPP4.PR05 */
            }
        }
    }

    Device (GPP5)
    {
        Name (_ADR, 0x00010006)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR06, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKE, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKF, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKG, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKH, 
                Zero
            }
        })
        Name (AR06, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x2C
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x2D
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x2E
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x2F
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR06) /* \_SB_.PCI0.GPP5.AR06 */
            }
            Else
            {
                Return (PR06) /* \_SB_.PCI0.GPP5.PR06 */
            }
        }
    }

    Device (GPP6)
    {
        Name (_ADR, 0x00010007)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR07, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKA, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKB, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKC, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKD, 
                Zero
            }
        })
        Name (AR07, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x30
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x31
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x32
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x33
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR07) /* \_SB_.PCI0.GPP6.AR07 */
            }
            Else
            {
                Return (PR07) /* \_SB_.PCI0.GPP6.PR07 */
            }
        }
    }

    Device (GPP7)
    {
        Name (_ADR, 0x00020001)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR08, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKE, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKF, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKG, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKH, 
                Zero
            }
        })
        Name (AR08, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x34
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x35
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x36
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x37
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR08) /* \_SB_.PCI0.GPP7.AR08 */
            }
            Else
            {
                Return (PR08) /* \_SB_.PCI0.GPP7.PR08 */
            }
        }
    }

    Device (GPP8)
    {
        Name (_ADR, 0x00030001)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR09, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKG, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKH, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKE, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKF, 
                Zero
            }
        })
        Name (AR09, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x36
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x37
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x34
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x35
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR09) /* \_SB_.PCI0.GPP8.AR09 */
            }
            Else
            {
                Return (PR09) /* \_SB_.PCI0.GPP8.PR09 */
            }
        }
    }

    Device (GPP9)
    {
        Name (_ADR, 0x00030002)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR0A, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKC, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKD, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKA, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKB, 
                Zero
            }
        })
        Name (AR0A, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x32
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x33
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x30
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x31
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR0A) /* \_SB_.PCI0.GPP9.AR0A */
            }
            Else
            {
                Return (PR0A) /* \_SB_.PCI0.GPP9.PR0A */
            }
        }
    }

    Device (GPPA)
    {
        Name (_ADR, 0x00030003)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR0B, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKG, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKH, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKE, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKF, 
                Zero
            }
        })
        Name (AR0B, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x2E
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x2F
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x2C
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x2D
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR0B) /* \_SB_.PCI0.GPPA.AR0B */
            }
            Else
            {
                Return (PR0B) /* \_SB_.PCI0.GPPA.PR0B */
            }
        }
    }

    Device (GPPB)
    {
        Name (_ADR, 0x00030004)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR0C, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKC, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKD, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKA, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKB, 
                Zero
            }
        })
        Name (AR0C, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x2A
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x2B
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x28
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x29
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR0C) /* \_SB_.PCI0.GPPB.AR0C */
            }
            Else
            {
                Return (PR0C) /* \_SB_.PCI0.GPPB.PR0C */
            }
        }
    }

    Device (GPPC)
    {
        Name (_ADR, 0x00030005)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR0D, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKG, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKH, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKE, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKF, 
                Zero
            }
        })
        Name (AR0D, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x26
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x27
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x24
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x25
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR0D) /* \_SB_.PCI0.GPPC.AR0D */
            }
            Else
            {
                Return (PR0D) /* \_SB_.PCI0.GPPC.PR0D */
            }
        }
    }

    Device (GPPD)
    {
        Name (_ADR, 0x00030006)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR0E, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKC, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKD, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKA, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKB, 
                Zero
            }
        })
        Name (AR0E, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x22
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x23
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x20
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x21
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR0E) /* \_SB_.PCI0.GPPD.AR0E */
            }
            Else
            {
                Return (PR0E) /* \_SB_.PCI0.GPPD.PR0E */
            }
        }
    }

    Device (GPPE)
    {
        Name (_ADR, 0x00030007)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR0F, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKG, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKH, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKE, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKF, 
                Zero
            }
        })
        Name (AR0F, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x1E
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x1F
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x1C
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x1D
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR0F) /* \_SB_.PCI0.GPPE.AR0F */
            }
            Else
            {
                Return (PR0F) /* \_SB_.PCI0.GPPE.PR0F */
            }
        }
    }

    Device (GPPF)
    {
        Name (_ADR, 0x00040001)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            If ((WKPM == One))
            {
                Return (GPRW (0x08, 0x04))
            }
            Else
            {
                Return (GPRW (0x08, Zero))
            }
        }

        Name (PR0G, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKC, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKD, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKA, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKB, 
                Zero
            }
        })
        Name (AR0G, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x1A
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x1B
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x18
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x19
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR0G) /* \_SB_.PCI0.GPPF.AR0G */
            }
            Else
            {
                Return (PR0G) /* \_SB_.PCI0.GPPF.PR0G */
            }
        }
    }

    Device (R0BB)
    {
        Name (_ADR, 0x00070001)  // _ADR: Address
        Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
        {
            Return (GPRW (0x19, 0x04))
        }

        Name (PR0H, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKH, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKE, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKF, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKG, 
                Zero
            }
        })
        Name (AR0H, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x2F
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x2C
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x2D
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x2E
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR0H) /* \_SB_.PCI0.R0BB.AR0H */
            }
            Else
            {
                Return (PR0H) /* \_SB_.PCI0.R0BB.PR0H */
            }
        }

        Device (XHCI)
        {
            Name (_ADR, 0x03)  // _ADR: Address
        }
    }

    Device (R0BC)
    {
        Name (_ADR, 0x00080001)  // _ADR: Address
        Name (PR0I, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                LNKB, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                LNKC, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                LNKD, 
                Zero
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                LNKA, 
                Zero
            }
        })
        Name (AR0I, Package (0x04)
        {
            Package (0x04)
            {
                0xFFFF, 
                Zero, 
                Zero, 
                0x29
            }, 

            Package (0x04)
            {
                0xFFFF, 
                One, 
                Zero, 
                0x2A
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x02, 
                Zero, 
                0x2B
            }, 

            Package (0x04)
            {
                0xFFFF, 
                0x03, 
                Zero, 
                0x28
            }
        })
        Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
        {
            If (PICM)
            {
                Return (AR0I) /* \_SB_.PCI0.R0BC.AR0I */
            }
            Else
            {
                Return (PR0I) /* \_SB_.PCI0.R0BC.PR0I */
            }
        }

        Device (SATA)
        {
            Name (_ADR, 0x02)  // _ADR: Address
            Name (B5EN, Zero)
            Name (BA_5, Zero)
            Name (SBAR, 0xF5202000)
            Name (SAD0, Zero)
            Name (SAD3, Zero)
            PowerResource (P0SA, 0x00, 0x0000)
            {
                Method (_STA, 0, NotSerialized)  // _STA: Status
                {
                    Return (SAD0) /* \_SB_.PCI0.R0BC.SATA.SAD0 */
                }

                Method (_ON, 0, NotSerialized)  // _ON_: Power On
                {
                    IO80 = 0xA1
                    SAD0 = One
                }

                Method (_OFF, 0, NotSerialized)  // _OFF: Power Off
                {
                    IO80 = 0xA2
                    SAD0 = Zero
                }
            }

            PowerResource (P3SA, 0x00, 0x0000)
            {
                Method (_STA, 0, NotSerialized)  // _STA: Status
                {
                    Return (SAD3) /* \_SB_.PCI0.R0BC.SATA.SAD3 */
                }

                Method (_ON, 0, NotSerialized)  // _ON_: Power On
                {
                    IO80 = 0xA4
                    SAD3 = One
                }

                Method (_OFF, 0, NotSerialized)  // _OFF: Power Off
                {
                    IO80 = 0xA5
                    SAD3 = Zero
                }
            }

            OperationRegion (SATX, PCI_Config, Zero, 0x44)
            Field (SATX, AnyAcc, NoLock, Preserve)
            {
                VIDI,   32, 
                Offset (0x0A), 
                STCL,   16, 
                Offset (0x24), 
                BA05,   32, 
                Offset (0x40), 
                WTEN,   1, 
                Offset (0x42), 
                DIS0,   1, 
                DIS1,   1, 
                DIS2,   1, 
                DIS3,   1, 
                DIS4,   1, 
                DIS5,   1
            }

            Field (SATX, AnyAcc, NoLock, Preserve)
            {
                Offset (0x42), 
                DISP,   6
            }

            Method (GBAA, 0, Serialized)
            {
                BA_5 = BA05 /* \_SB_.PCI0.R0BC.SATA.BA05 */
                If (((BA_5 == 0xFFFFFFFF) || (STCL != 0x0101)))
                {
                    B5EN = Zero
                    Return (SBAR) /* \_SB_.PCI0.R0BC.SATA.SBAR */
                }
                Else
                {
                    B5EN = One
                    Return (BA_5) /* \_SB_.PCI0.R0BC.SATA.BA_5 */
                }
            }

            OperationRegion (BAR5, SystemMemory, GBAA (), 0x1000)
            Field (BAR5, AnyAcc, NoLock, Preserve)
            {
                NOPT,   5, 
                Offset (0x0C), 
                PTI0,   1, 
                PTI1,   1, 
                PTI2,   1, 
                PTI3,   1, 
                PTI4,   1, 
                PTI5,   1, 
                PTI6,   1, 
                PTI7,   1, 
                Offset (0x118), 
                CST0,   1, 
                Offset (0x120), 
                    ,   7, 
                BSY0,   1, 
                Offset (0x128), 
                DET0,   4, 
                Offset (0x129), 
                IPM0,   4, 
                Offset (0x12C), 
                DDI0,   4, 
                Offset (0x198), 
                CST1,   1, 
                Offset (0x1A0), 
                    ,   7, 
                BSY1,   1, 
                Offset (0x1A8), 
                DET1,   4, 
                Offset (0x1A9), 
                IPM1,   4, 
                Offset (0x1AC), 
                DDI1,   4, 
                Offset (0x218), 
                CST2,   1, 
                Offset (0x220), 
                    ,   7, 
                BSY2,   1, 
                Offset (0x228), 
                DET2,   4, 
                Offset (0x229), 
                IPM2,   4, 
                Offset (0x22C), 
                DDI2,   4, 
                Offset (0x298), 
                CST3,   1, 
                Offset (0x2A0), 
                    ,   7, 
                BSY3,   1, 
                Offset (0x2A8), 
                DET3,   4, 
                Offset (0x2A9), 
                IPM3,   4, 
                Offset (0x2AC), 
                DDI3,   4, 
                Offset (0x318), 
                CST4,   1, 
                Offset (0x320), 
                    ,   7, 
                BSY4,   1, 
                Offset (0x328), 
                DET4,   4, 
                Offset (0x329), 
                IPM4,   4, 
                Offset (0x32C), 
                DDI4,   4, 
                Offset (0x398), 
                CST5,   1, 
                Offset (0x3A0), 
                    ,   7, 
                BSY5,   1, 
                Offset (0x3A8), 
                DET5,   4, 
                Offset (0x3A9), 
                IPM5,   4, 
                Offset (0x3AC), 
                DDI5,   4
            }

            Field (BAR5, AnyAcc, NoLock, Preserve)
            {
                Offset (0x0C), 
                PTI,    6
            }

            Method (_INI, 0, NotSerialized)  // _INI: Initialize
            {
                GBAA ()
            }

            Device (PRID)
            {
                Name (_ADR, Zero)  // _ADR: Address
                Name (SPTM, Buffer (0x14)
                {
                    /* 0000 */  0x78, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,  // x.......
                    /* 0008 */  0x78, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,  // x.......
                    /* 0010 */  0x1F, 0x00, 0x00, 0x00                           // ....
                })
                Method (_GTM, 0, NotSerialized)  // _GTM: Get Timing Mode
                {
                    Return (SPTM) /* \_SB_.PCI0.R0BC.SATA.PRID.SPTM */
                }

                Method (_STM, 3, NotSerialized)  // _STM: Set Timing Mode
                {
                    SPTM = Arg0
                }

                Method (_PS0, 0, NotSerialized)  // _PS0: Power State 0
                {
                    GBAA ()
                    If (((OSYS >= 0x07D5) && B5EN))
                    {
                        If (IPM2)
                        {
                            Local0 = 0x32
                            While (((BSY2 == One) && Local0))
                            {
                                Sleep (0xFA)
                                Local0--
                            }
                        }
                    }
                }

                Method (_PS3, 0, NotSerialized)  // _PS3: Power State 3
                {
                }

                Device (P_D0)
                {
                    Name (_ADR, Zero)  // _ADR: Address
                    Method (_STA, 0, NotSerialized)  // _STA: Status
                    {
                        GBAA ()
                        If ((B5EN == Zero))
                        {
                            Return (Zero)
                        }

                        If ((DET0 == 0x03))
                        {
                            Return (0x0F)
                        }
                        Else
                        {
                            Return (Zero)
                        }
                    }

                    Name (S12P, Zero)
                    Method (_PS0, 0, NotSerialized)  // _PS0: Power State 0
                    {
                        GBAA ()
                        If (((OSYS < 0x07D5) && B5EN))
                        {
                            Local0 = 0x32
                            While (((BSY0 == One) && Local0))
                            {
                                Sleep (0xFA)
                                Local0--
                            }
                        }
                    }

                    Method (_PS3, 0, NotSerialized)  // _PS3: Power State 3
                    {
                    }

                    Method (_GTF, 0, NotSerialized)  // _GTF: Get Task File
                    {
                        Local0 = Buffer (0x07)
                            {
                                 0x03, 0x46, 0x00, 0x00, 0x00, 0xA0, 0xEF         // .F.....
                            }
                        Return (Local0)
                    }
                }

                Device (P_D1)
                {
                    Name (_ADR, One)  // _ADR: Address
                    Method (_STA, 0, NotSerialized)  // _STA: Status
                    {
                        GBAA ()
                        If ((B5EN == Zero))
                        {
                            Return (Zero)
                        }

                        If ((DET2 == 0x03))
                        {
                            Return (0x0F)
                        }
                        Else
                        {
                            Return (Zero)
                        }
                    }

                    Name (S12P, Zero)
                    Method (_PS0, 0, NotSerialized)  // _PS0: Power State 0
                    {
                        GBAA ()
                        If (((OSYS < 0x07D5) && B5EN))
                        {
                            Local0 = 0x32
                            While (((BSY2 == One) && Local0))
                            {
                                Sleep (0xFA)
                                Local0--
                            }
                        }
                    }

                    Method (_PS3, 0, NotSerialized)  // _PS3: Power State 3
                    {
                    }

                    Method (_GTF, 0, NotSerialized)  // _GTF: Get Task File
                    {
                        Local0 = Buffer (0x07)
                            {
                                 0x03, 0x46, 0x00, 0x00, 0x00, 0xA0, 0xEF         // .F.....
                            }
                        Return (Local0)
                    }
                }
            }

            Device (SECD)
            {
                Name (_ADR, 0x02)  // _ADR: Address
                Name (SPTM, Buffer (0x14)
                {
                    /* 0000 */  0x78, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,  // x.......
                    /* 0008 */  0x78, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,  // x.......
                    /* 0010 */  0x1F, 0x00, 0x00, 0x00                           // ....
                })
                Method (_GTM, 0, NotSerialized)  // _GTM: Get Timing Mode
                {
                    Return (SPTM) /* \_SB_.PCI0.R0BC.SATA.SECD.SPTM */
                }

                Method (_STM, 3, NotSerialized)  // _STM: Set Timing Mode
                {
                    SPTM = Arg0
                }

                Method (_PS0, 0, NotSerialized)  // _PS0: Power State 0
                {
                    GBAA ()
                    If (((OSYS >= 0x07D5) && B5EN))
                    {
                        If (IPM1)
                        {
                            Local0 = 0x32
                            While (((BSY1 == One) && Local0))
                            {
                                Sleep (0xFA)
                                Local0--
                            }
                        }

                        If (IPM3)
                        {
                            Local0 = 0x32
                            While (((BSY3 == One) && Local0))
                            {
                                Sleep (0xFA)
                                Local0--
                            }
                        }
                    }
                }

                Method (_PS3, 0, NotSerialized)  // _PS3: Power State 3
                {
                }

                Device (S_D0)
                {
                    Name (_ADR, Zero)  // _ADR: Address
                    Method (_STA, 0, NotSerialized)  // _STA: Status
                    {
                        GBAA ()
                        If ((B5EN == Zero))
                        {
                            Return (Zero)
                        }

                        If ((DET1 == 0x03))
                        {
                            Return (0x0F)
                        }
                        Else
                        {
                            Return (Zero)
                        }
                    }

                    Method (_PS0, 0, NotSerialized)  // _PS0: Power State 0
                    {
                        GBAA ()
                        If (((OSYS < 0x07D5) && B5EN))
                        {
                            Local0 = 0x32
                            While (((BSY1 == One) && Local0))
                            {
                                Sleep (0xFA)
                                Local0--
                            }
                        }
                    }

                    Method (_PS3, 0, NotSerialized)  // _PS3: Power State 3
                    {
                    }

                    Method (_GTF, 0, NotSerialized)  // _GTF: Get Task File
                    {
                        Local0 = Buffer (0x07)
                            {
                                 0x03, 0x46, 0x00, 0x00, 0x00, 0xA0, 0xEF         // .F.....
                            }
                        Return (Local0)
                    }
                }

                Device (S_D1)
                {
                    Name (_ADR, One)  // _ADR: Address
                    Method (_STA, 0, NotSerialized)  // _STA: Status
                    {
                        GBAA ()
                        If ((B5EN == Zero))
                        {
                            Return (Zero)
                        }

                        If ((DET3 == 0x03))
                        {
                            Return (0x0F)
                        }
                        Else
                        {
                            Return (Zero)
                        }
                    }

                    Method (_PS0, 0, NotSerialized)  // _PS0: Power State 0
                    {
                        GBAA ()
                        If (((OSYS < 0x07D5) && B5EN))
                        {
                            Local0 = 0x32
                            While (((BSY3 == One) && Local0))
                            {
                                Sleep (0xFA)
                                Local0--
                            }
                        }
                    }

                    Method (_PS3, 0, NotSerialized)  // _PS3: Power State 3
                    {
                    }

                    Method (_GTF, 0, NotSerialized)  // _GTF: Get Task File
                    {
                        Local0 = Buffer (0x07)
                            {
                                 0x03, 0x46, 0x00, 0x00, 0x00, 0xA0, 0xEF         // .F.....
                            }
                        Return (Local0)
                    }
                }
            }

            Method (ENP, 2, NotSerialized)
            {
                If ((Arg0 == Zero))
                {
                    DIS0 = ~Arg1
                }
                ElseIf ((Arg0 == One))
                {
                    DIS1 = ~Arg1
                }
                ElseIf ((Arg0 == 0x02))
                {
                    DIS2 = ~Arg1
                }
                ElseIf ((Arg0 == 0x03))
                {
                    DIS3 = ~Arg1
                }
                ElseIf ((Arg0 == 0x04))
                {
                    DIS4 = ~Arg1
                }
                ElseIf ((Arg0 == 0x05))
                {
                    DIS5 = ~Arg1
                }

                WTEN = One
                If ((Arg0 == Zero))
                {
                    PTI0 = Arg1
                }
                ElseIf ((Arg0 == One))
                {
                    PTI1 = Arg1
                }
                ElseIf ((Arg0 == 0x02))
                {
                    PTI2 = Arg1
                }
                ElseIf ((Arg0 == 0x03))
                {
                    PTI3 = Arg1
                }
                ElseIf ((Arg0 == 0x04))
                {
                    PTI4 = Arg1
                }
                ElseIf ((Arg0 == 0x05))
                {
                    PTI5 = Arg1
                }

                If ((DISP == 0x3F))
                {
                    PTI0 = One
                }
                ElseIf ((DIS0 && ((DISP & 0x3E) ^ 0x3E)))
                {
                    PTI0 = Zero
                }

                Local0 = PTI /* \_SB_.PCI0.R0BC.SATA.PTI_ */
                Local1 = Zero
                While (Local0)
                {
                    If ((Local0 & One))
                    {
                        Local1++
                    }

                    Local0 >>= One
                }

                NOPT = Local1--
                WTEN = Zero
            }
        }

        Device (AZAL)
        {
            Name (_ADR, 0x03)  // _ADR: Address
        }
    }

    Device (HPET)
    {
        Name (_HID, EisaId ("PNP0103") /* HPET System Timer */)  // _HID: Hardware ID
        Method (_STA, 0, NotSerialized)  // _STA: Status
        {
            If ((HPEN == One))
            {
                If ((OSYS >= 0x07D5))
                {
                    Return (0x0F)
                }

                HPEN = Zero
                Return (One)
            }

            Return (One)
        }

        Name (HPER, ResourceTemplate ()
        {
            IRQNoFlags ()
                {0}
            IRQNoFlags ()
                {8}
            Memory32Fixed (ReadOnly,
                0xFED00000,         // Address Base
                0x00000400,         // Address Length
                _Y11)
        })
        Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
        {
            CreateDWordField (HPER, \_SB.PCI0.HPET._Y11._BAS, HPEB)  // _BAS: Base Address
            Local0 = 0xFED00000
            HPEB = (Local0 & 0xFFFFFC00)
            Return (HPER) /* \_SB_.PCI0.HPET.HPER */
        }
    }

    Device (SMBS)
    {
        Name (_ADR, 0x000B0000)  // _ADR: Address
    }

    Device (LPCB)
    {
        Name (_ADR, 0x000B0003)  // _ADR: Address
        Device (DMAC)
        {
            Name (_HID, EisaId ("PNP0200") /* PC-class DMA Controller */)  // _HID: Hardware ID
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                IO (Decode16,
                    0x0000,             // Range Minimum
                    0x0000,             // Range Maximum
                    0x01,               // Alignment
                    0x10,               // Length
                    )
                IO (Decode16,
                    0x0081,             // Range Minimum
                    0x0081,             // Range Maximum
                    0x01,               // Alignment
                    0x0F,               // Length
                    )
                IO (Decode16,
                    0x00C0,             // Range Minimum
                    0x00C0,             // Range Maximum
                    0x01,               // Alignment
                    0x20,               // Length
                    )
                DMA (Compatibility, NotBusMaster, Transfer8_16, )
                    {4}
            })
        }

        Device (COPR)
        {
            Name (_HID, EisaId ("PNP0C04") /* x87-compatible Floating Point Processing Unit */)  // _HID: Hardware ID
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                IO (Decode16,
                    0x00F0,             // Range Minimum
                    0x00F0,             // Range Maximum
                    0x01,               // Alignment
                    0x0F,               // Length
                    )
                IRQNoFlags ()
                    {13}
            })
        }

        Device (PIC)
        {
            Name (_HID, EisaId ("PNP0000") /* 8259-compatible Programmable Interrupt Controller */)  // _HID: Hardware ID
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                IO (Decode16,
                    0x0020,             // Range Minimum
                    0x0020,             // Range Maximum
                    0x01,               // Alignment
                    0x02,               // Length
                    )
                IO (Decode16,
                    0x00A0,             // Range Minimum
                    0x00A0,             // Range Maximum
                    0x01,               // Alignment
                    0x02,               // Length
                    )
                IRQNoFlags ()
                    {2}
            })
        }

        Device (RTC)
        {
            Name (_HID, EisaId ("PNP0B00") /* AT Real-Time Clock */)  // _HID: Hardware ID
            Name (BUF0, ResourceTemplate ()
            {
                IO (Decode16,
                    0x0070,             // Range Minimum
                    0x0070,             // Range Maximum
                    0x01,               // Alignment
                    0x02,               // Length
                    )
            })
            Name (BUF1, ResourceTemplate ()
            {
                IO (Decode16,
                    0x0070,             // Range Minimum
                    0x0070,             // Range Maximum
                    0x01,               // Alignment
                    0x02,               // Length
                    )
                IRQNoFlags ()
                    {8}
            })
            Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
            {
                If ((HPEN == One))
                {
                    Return (BUF0) /* \_SB_.PCI0.LPCB.RTC_.BUF0 */
                }

                Return (BUF1) /* \_SB_.PCI0.LPCB.RTC_.BUF1 */
            }
        }

        Device (SPKR)
        {
            Name (_HID, EisaId ("PNP0800") /* Microsoft Sound System Compatible Device */)  // _HID: Hardware ID
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                IO (Decode16,
                    0x0061,             // Range Minimum
                    0x0061,             // Range Maximum
                    0x01,               // Alignment
                    0x01,               // Length
                    )
            })
        }

        Device (TMR)
        {
            Name (_HID, EisaId ("PNP0100") /* PC-class System Timer */)  // _HID: Hardware ID
            Name (BUF0, ResourceTemplate ()
            {
                IO (Decode16,
                    0x0040,             // Range Minimum
                    0x0040,             // Range Maximum
                    0x01,               // Alignment
                    0x04,               // Length
                    )
            })
            Name (BUF1, ResourceTemplate ()
            {
                IO (Decode16,
                    0x0040,             // Range Minimum
                    0x0040,             // Range Maximum
                    0x01,               // Alignment
                    0x04,               // Length
                    )
                IRQNoFlags ()
                    {0}
            })
            Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
            {
                If ((HPEN == One))
                {
                    Return (BUF0) /* \_SB_.PCI0.LPCB.TMR_.BUF0 */
                }

                Return (BUF1) /* \_SB_.PCI0.LPCB.TMR_.BUF1 */
            }
        }

        Device (SYSR)
        {
            Name (_HID, EisaId ("PNP0C02") /* PNP Motherboard Resources */)  // _HID: Hardware ID
            Name (_UID, One)  // _UID: Unique ID
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                IO (Decode16,
                    0x0010,             // Range Minimum
                    0x0010,             // Range Maximum
                    0x01,               // Alignment
                    0x10,               // Length
                    )
                IO (Decode16,
                    0x0072,             // Range Minimum
                    0x0072,             // Range Maximum
                    0x01,               // Alignment
                    0x02,               // Length
                    )
                IO (Decode16,
                    0x0080,             // Range Minimum
                    0x0080,             // Range Maximum
                    0x01,               // Alignment
                    0x01,               // Length
                    )
                IO (Decode16,
                    0x00B0,             // Range Minimum
                    0x00B0,             // Range Maximum
                    0x01,               // Alignment
                    0x02,               // Length
                    )
                IO (Decode16,
                    0x0092,             // Range Minimum
                    0x0092,             // Range Maximum
                    0x01,               // Alignment
                    0x01,               // Length
                    )
                IO (Decode16,
                    0x0400,             // Range Minimum
                    0x0400,             // Range Maximum
                    0x01,               // Alignment
                    0xD0,               // Length
                    )
                IO (Decode16,
                    0x04D0,             // Range Minimum
                    0x04D0,             // Range Maximum
                    0x01,               // Alignment
                    0x02,               // Length
                    )
                IO (Decode16,
                    0x04D6,             // Range Minimum
                    0x04D6,             // Range Maximum
                    0x01,               // Alignment
                    0x01,               // Length
                    )
                IO (Decode16,
                    0x0C00,             // Range Minimum
                    0x0C00,             // Range Maximum
                    0x01,               // Alignment
                    0x02,               // Length
                    )
                IO (Decode16,
                    0x0C14,             // Range Minimum
                    0x0C14,             // Range Maximum
                    0x01,               // Alignment
                    0x01,               // Length
                    )
                IO (Decode16,
                    0x0C50,             // Range Minimum
                    0x0C50,             // Range Maximum
                    0x01,               // Alignment
                    0x03,               // Length
                    )
                IO (Decode16,
                    0x0C6C,             // Range Minimum
                    0x0C6C,             // Range Maximum
                    0x01,               // Alignment
                    0x01,               // Length
                    )
                IO (Decode16,
                    0x0C6F,             // Range Minimum
                    0x0C6F,             // Range Maximum
                    0x01,               // Alignment
                    0x01,               // Length
                    )
                IO (Decode16,
                    0x0CD0,             // Range Minimum
                    0x0CD0,             // Range Maximum
                    0x01,               // Alignment
                    0x0C,               // Length
                    )
            })
        }

        Device (MEM)
        {
            Name (_HID, EisaId ("PNP0C01") /* System Board */)  // _HID: Hardware ID
            Name (MSRC, ResourceTemplate ()
            {
                Memory32Fixed (ReadOnly,
                    0x000E0000,         // Address Base
                    0x00020000,         // Address Length
                    )
                Memory32Fixed (ReadWrite,
                    0xFFF00000,         // Address Base
                    0x00100000,         // Address Length
                    _Y12)
            })
            Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
            {
                CreateDWordField (MSRC, \_SB.PCI0.LPCB.MEM._Y12._LEN, PSIZ)  // _LEN: Length
                CreateDWordField (MSRC, \_SB.PCI0.LPCB.MEM._Y12._BAS, PBAS)  // _BAS: Base Address
                PSIZ = ROMS /* \ROMS */
                Local0 = (ROMS - One)
                PBAS = (0xFFFFFFFF - Local0)
                Return (MSRC) /* \_SB_.PCI0.LPCB.MEM_.MSRC */
            }
        }

        Mutex (PSMX, 0x00)
    }
}