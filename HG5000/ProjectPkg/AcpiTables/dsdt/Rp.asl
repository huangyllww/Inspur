
        Device (PCI0)
        {
            Name (_HID, EisaId ("PNP0A08"))  // PCI Express Bus
            Name (_CID, EisaId ("PNP0A03"))  // PCI Bus
            Name (_ADR, Zero)                //
            Method (^BN00, 0, NotSerialized){
                Return (Zero)
            }

            Method (_BBN, 0, NotSerialized)  // _BBN: BIOS Bus Number
            {
                Return (BN00 ())
            }

            Name (_UID, Zero)                // _UID: Unique ID
            Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
            {
                If (PICM){
                    Return (AR00)
                }

                Return (PR00)
            }

            OperationRegion (NAPC, PCI_Config, 0xB4, 0x0C)
            Field (NAPC, DWordAcc, NoLock, Preserve)
            {
                NAEX,   32, 
                NAPX,   32, 
                NAPD,   32
            }

            Mutex (NAPM, 0x00)
            Method (NAPE, 0, NotSerialized)
            {
                Acquire (NAPM, 0xFFFF)
                Store (Zero, NAEX)
                Store (0x0001C604, NAPX)
                Store (NAPD, Local0)
                And (Local0, 0x0F, Local0)
                While (LGreaterEqual (Local0, One))
                {
                    Subtract (Local0, One, Local1)
                    Store (Local1, NAEX)
                    Store (0x14300000, NAPX)
                    Store (NAPD, Local2)
                    And (Local2, 0xFFFFFFEF, Local2)
                    Store (Local2, NAPD)
                    Decrement (Local0)
                }

                Store (Zero, NAEX)
                Release (NAPM)
            }

            Device (AMDN)
            {
                Name (_HID, EisaId ("PNP0C01")) // System Board
                Name (_UID, 0xC8)
                Name (_STA, 0x0F)
                Name (NPTR, ResourceTemplate ()
                {
                    Memory32Fixed (ReadWrite,
                        0x00000000,         // Address Base
                        0x00000000,         // Address Length
                        _Y00)
                })
                Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
                {
                    CreateDWordField (NPTR, \_SB.PCI0.AMDN._Y00._LEN, PL)
                    CreateDWordField (NPTR, \_SB.PCI0.AMDN._Y00._BAS, PB)
                    Store (PEBS, PB)
                    Store (PEBL, PL)
                    Return (NPTR)
                }
            }

            Method (NPTS, 1, NotSerialized)
            {
//-                APTS (Arg0)
            }

            Method (NWAK, 1, NotSerialized)
            {
//-                AWAK (Arg0)
            }

    
// bus: [0, 7F]     -> [0, 10]
// Io : [0, 3AF]
// Io : [3B0, 3DF]  -> VGA IO
// Io : [400, CF7]
// Io : [CF8, CFF]
// Io : [D00, ?]
// mem: [A0000, BFFFF]
// mem: [C0000, DFFFF]
// mem: [0xEC000000, 0x04000000]                    <- need update
// mem: [0x000001C390000000, 0x0000003C70000000]
            
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
                    0x03AF,             // Range Maximum
                    0x0000,             // Translation Offset
                    0x03B0,             // Length
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
            
           
            Method (_STA, 0, NotSerialized)  // _STA: Status
            {
                Return (0x0F)
            }

            Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
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
              
              If (\GFX0)
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

            Method (_OSC, 4, Serialized)  // _OSC: Operating System Capabilities
            {
                Name (SUPP, Zero)
                Name (CTRL, Zero)
                CreateDWordField (Arg3, Zero, CDW1)
                CreateDWordField (Arg3, 0x04, CDW2)
                CreateDWordField (Arg3, 0x08, CDW3)
                If (LEqual (Arg0, Buffer (0x10)
                        {
                            0x5B, 0x4D, 0xDB, 0x33, 0xF7, 0x1F, 0x1C, 0x40,
                            0x96, 0x57, 0x74, 0x41, 0xC0, 0x3D, 0xD7, 0x66
                        }))
                {
                    Store (CDW2, SUPP)
                    Store (CDW3, CTRL)
                    If (LNotEqual (And (SUPP, 0x16), 0x16))
                    {
                        And (CTRL, 0x1E, CTRL)
                    }

                    If (LNot (PEHP))
                    {
                        And (CTRL, 0x1E, CTRL)
                    }

                    If (LNot (SHPC))
                    {
                        And (CTRL, 0x1D, CTRL)
                    }

                    If (LNot (PEPM))
                    {
                        And (CTRL, 0x1B, CTRL)
                    }

                    If (LNot (PEER))
                    {
                        And (CTRL, 0x15, CTRL)
                    }

                    If (LNot (PECS))
                    {
                        And (CTRL, 0x0F, CTRL)
                    }

                    If (LNotEqual (Arg1, One))
                    {
                        Or (CDW1, 0x08, CDW1)
                    }

                    If (LNotEqual (CDW3, CTRL))
                    {
                        Or (CDW1, 0x10, CDW1)
                    }

                    Store (CTRL, CDW3)
                    Return (Arg3)
                }
                Else
                {
                    Or (CDW1, 0x04, CDW1)
                    Return (Arg3)
                }
            }

            
            
            
//------------------------------------------------------------------------------            
            Device (D0A0)                        // (0, 1, 1)
            {
                Name (_ADR, 0x00010001)          // bridge
                Method (_PRT, 0, NotSerialized)  // PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG03)
                    }

                    Return (PG03)
                }

                Device (BRB0)
                {
                    Name (_ADR, 0xFFFF)              // bridge
                    Method (_PRT, 0, NotSerialized)  // PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AGB0)
                        }

                        Return (PGB0)
                    }

                    Device (VGFX)                    // VGA
                    {
                        Name (_ADR, 0xFFFF)
                    }
                }
            }

            Device (D0A1)
            {
                Name (_ADR, 0x00010002)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG04)
                    }

                    Return (PG04)
                }

                Device (D0B4)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (D0A2)
            {
                Name (_ADR, 0x00010003)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG05)
                    }

                    Return (PG05)
                }

                Device (D0B5)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (D0A3)
            {
                Name (_ADR, 0x00010004)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG06)
                    }

                    Return (PG06)
                }

                Device (D0B6)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (D0A4)
            {
                Name (_ADR, 0x00010005)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG07)
                    }

                    Return (PG07)
                }

                Device (D0B7)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (D0A5)
            {
                Name (_ADR, 0x00010006)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG08)
                    }

                    Return (PG08)
                }

                Device (D0B8)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (D0A6)
            {
                Name (_ADR, 0x00010007)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG09)
                    }

                    Return (PG09)
                }

                Device (D0B9)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (D0A7)
            {
                Name (_ADR, 0x00020001)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG0A)
                    }

                    Return (PG0A)
                }

                Device (D0BA)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (S0D0)                         // bridge (0,7,1)
            {
                Name (_ADR, 0x00070001)  // _ADR: Address
               /* Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
                {
                    Return (GPRW (0x0B, 0x04))
                }*/

                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG0B)
                    }

                    Return (PG0B)
                }

                Device (D00C)
                {
                    Name (_ADR, One)  // _ADR: Address
                }

                Device (D00D)
                {
                    Name (_ADR, 0x02)  // _ADR: Address
                }

                Device (XHC0)
                {
                    Name (_ADR, 0x03)  // _ADR: Address
                }
            }

            Device (BR0F)                         // bridge (0,8,1)
            {
                Name (_ADR, 0x00080001)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG0F)
                    }

                    Return (PG0F)
                }

                Device (D010)
                {
                    Name (_ADR, One)  // _ADR: Address
                }

                Device (SATA)
                {
                    Name (_ADR, 0x02)  // (4,0,2)
                    
                    Name (B5EN, Zero)
                    Name (BA_5, Zero)
                    Name (SBAR, 0xF0B6F000)
                    Name (OSVR, Zero)
                    Name (SAD0, Zero)
                    Name (SAD3, Zero)
                    PowerResource (P0SA, 0x00, 0x0000)
                    {
                        Method (_STA, 0, NotSerialized)  // _STA: Status
                        {
                            Return (SAD0)
                        }

                        Method (_ON, 0, NotSerialized)   // _ON_: Power On
                        {
                            Store (0xA1, DBG8)
                            Store (One, SAD0)
                        }

                        Method (_OFF, 0, NotSerialized)  // _OFF: Power Off
                        {
                            Store (0xA2, DBG8)
                            Store (Zero, SAD0)
                        }
                    }

                    PowerResource (P3SA, 0x00, 0x0000)
                    {
                        Method (_STA, 0, NotSerialized)  // _STA: Status
                        {
                            Return (SAD3)
                        }

                        Method (_ON, 0, NotSerialized)   // _ON_: Power On
                        {
                            Store (0xA4, DBG8)
                            Store (One, SAD3)
                        }

                        Method (_OFF, 0, NotSerialized)  // _OFF: Power Off
                        {
                            Store (0xA5, DBG8)
                            Store (Zero, SAD3)
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
                        Store (BA05, BA_5)
                        If (LOr (LEqual (BA_5, Ones), LNotEqual (STCL, 0x0101)))
                        {
                            Store (Zero, B5EN)
                            Return (SBAR)
                        }
                        Else
                        {
                            Store (One, B5EN)
                            Return (BA_5)
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
                            /* 0000 */   0x78, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
                            /* 0008 */   0x78, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
                            /* 0010 */   0x1F, 0x00, 0x00, 0x00
                        })
                        Method (_GTM, 0, NotSerialized)  // _GTM: Get Timing Mode
                        {
                            Return (SPTM)
                        }

                        Method (_STM, 3, NotSerialized)  // _STM: Set Timing Mode
                        {
                            Store (Arg0, SPTM)
                        }

                        Method (_PS0, 0, NotSerialized)  // _PS0: Power State 0
                        {
                            GBAA ()
                            If (LAnd (LOr (LGreaterEqual (OSVR, 0x0C), LEqual (OSVR, Zero)), 
                                B5EN))
                            {
                                If (IPM2)
                                {
                                    Store (0x32, Local0)
                                    While (LAnd (LEqual (BSY2, One), Local0))
                                    {
                                        Sleep (0xFA)
                                        Decrement (Local0)
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
                                If (LEqual (B5EN, Zero))
                                {
                                    Return (Zero)
                                }

                                If (LEqual (DET0, 0x03))
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
                                If (LAnd (LAnd (LLess (OSVR, 0x0C), LNotEqual (OSVR, Zero)), 
                                    B5EN))
                                {
                                    Store (0x32, Local0)
                                    While (LAnd (LEqual (BSY0, One), Local0))
                                    {
                                        Sleep (0xFA)
                                        Decrement (Local0)
                                    }
                                }
                            }

                            Method (_PS3, 0, NotSerialized)  // _PS3: Power State 3
                            {
                            }

                            Method (_GTF, 0, NotSerialized)  // _GTF: Get Task File
                            {
                                Store (Buffer (0x07)
                                    {
                                         0x03, 0x46, 0x00, 0x00, 0x00, 0xA0, 0xEF
                                    }, Local0)
                                Return (Local0)
                            }
                        }

                        Device (P_D1)
                        {
                            Name (_ADR, One)  // _ADR: Address
                            Method (_STA, 0, NotSerialized)  // _STA: Status
                            {
                                GBAA ()
                                If (LEqual (B5EN, Zero))
                                {
                                    Return (Zero)
                                }

                                If (LEqual (DET2, 0x03))
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
                                If (LAnd (LAnd (LLess (OSVR, 0x0C), LNotEqual (OSVR, Zero)), 
                                    B5EN))
                                {
                                    Store (0x32, Local0)
                                    While (LAnd (LEqual (BSY2, One), Local0))
                                    {
                                        Sleep (0xFA)
                                        Decrement (Local0)
                                    }
                                }
                            }

                            Method (_PS3, 0, NotSerialized)  // _PS3: Power State 3
                            {
                            }

                            Method (_GTF, 0, NotSerialized)  // _GTF: Get Task File
                            {
                                Store (Buffer (0x07)
                                    {
                                         0x03, 0x46, 0x00, 0x00, 0x00, 0xA0, 0xEF
                                    }, Local0)
                                Return (Local0)
                            }
                        }
                    }

                    Device (SECD)
                    {
                        Name (_ADR, 0x02)  // _ADR: Address
                        Name (SPTM, Buffer (0x14)
                        {
                            /* 0000 */   0x78, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
                            /* 0008 */   0x78, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
                            /* 0010 */   0x1F, 0x00, 0x00, 0x00
                        })
                        Method (_GTM, 0, NotSerialized)  // _GTM: Get Timing Mode
                        {
                            Return (SPTM)
                        }

                        Method (_STM, 3, NotSerialized)  // _STM: Set Timing Mode
                        {
                            Store (Arg0, SPTM)
                        }

                        Method (_PS0, 0, NotSerialized)  // _PS0: Power State 0
                        {
                            GBAA ()
                            If (LAnd (LOr (LGreaterEqual (OSVR, 0x0C), LEqual (OSVR, Zero)), 
                                B5EN))
                            {
                                If (IPM1)
                                {
                                    Store (0x32, Local0)
                                    While (LAnd (LEqual (BSY1, One), Local0))
                                    {
                                        Sleep (0xFA)
                                        Decrement (Local0)
                                    }
                                }

                                If (IPM3)
                                {
                                    Store (0x32, Local0)
                                    While (LAnd (LEqual (BSY3, One), Local0))
                                    {
                                        Sleep (0xFA)
                                        Decrement (Local0)
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
                                If (LEqual (B5EN, Zero))
                                {
                                    Return (Zero)
                                }

                                If (LEqual (DET1, 0x03))
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
                                If (LAnd (LAnd (LLess (OSVR, 0x0C), LNotEqual (OSVR, Zero)), 
                                    B5EN))
                                {
                                    Store (0x32, Local0)
                                    While (LAnd (LEqual (BSY1, One), Local0))
                                    {
                                        Sleep (0xFA)
                                        Decrement (Local0)
                                    }
                                }
                            }

                            Method (_PS3, 0, NotSerialized)  // _PS3: Power State 3
                            {
                            }

                            Method (_GTF, 0, NotSerialized)  // _GTF: Get Task File
                            {
                                Store (Buffer (0x07)
                                    {
                                         0x03, 0x46, 0x00, 0x00, 0x00, 0xA0, 0xEF
                                    }, Local0)
                                Return (Local0)
                            }
                        }

                        Device (S_D1)
                        {
                            Name (_ADR, One)  // _ADR: Address
                            Method (_STA, 0, NotSerialized)  // _STA: Status
                            {
                                GBAA ()
                                If (LEqual (B5EN, Zero))
                                {
                                    Return (Zero)
                                }

                                If (LEqual (DET3, 0x03))
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
                                If (LAnd (LAnd (LLess (OSVR, 0x0C), LNotEqual (OSVR, Zero)), 
                                    B5EN))
                                {
                                    Store (0x32, Local0)
                                    While (LAnd (LEqual (BSY3, One), Local0))
                                    {
                                        Sleep (0xFA)
                                        Decrement (Local0)
                                    }
                                }
                            }

                            Method (_PS3, 0, NotSerialized)  // _PS3: Power State 3
                            {
                            }

                            Method (_GTF, 0, NotSerialized)  // _GTF: Get Task File
                            {
                                Store (Buffer (0x07)
                                    {
                                         0x03, 0x46, 0x00, 0x00, 0x00, 0xA0, 0xEF
                                    }, Local0)
                                Return (Local0)
                            }
                        }
                    }

                    Method (ENP, 2, NotSerialized)
                    {
                        If (LEqual (Arg0, Zero))
                        {
                            Store (Not (Arg1), DIS0)
                        }
                        Else
                        {
                            If (LEqual (Arg0, One))
                            {
                                Store (Not (Arg1), DIS1)
                            }
                            Else
                            {
                                If (LEqual (Arg0, 0x02))
                                {
                                    Store (Not (Arg1), DIS2)
                                }
                                Else
                                {
                                    If (LEqual (Arg0, 0x03))
                                    {
                                        Store (Not (Arg1), DIS3)
                                    }
                                    Else
                                    {
                                        If (LEqual (Arg0, 0x04))
                                        {
                                            Store (Not (Arg1), DIS4)
                                        }
                                        Else
                                        {
                                            If (LEqual (Arg0, 0x05))
                                            {
                                                Store (Not (Arg1), DIS5)
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        Store (One, WTEN)
                        If (LEqual (Arg0, Zero))
                        {
                            Store (Arg1, PTI0)
                        }
                        Else
                        {
                            If (LEqual (Arg0, One))
                            {
                                Store (Arg1, PTI1)
                            }
                            Else
                            {
                                If (LEqual (Arg0, 0x02))
                                {
                                    Store (Arg1, PTI2)
                                }
                                Else
                                {
                                    If (LEqual (Arg0, 0x03))
                                    {
                                        Store (Arg1, PTI3)
                                    }
                                    Else
                                    {
                                        If (LEqual (Arg0, 0x04))
                                        {
                                            Store (Arg1, PTI4)
                                        }
                                        Else
                                        {
                                            If (LEqual (Arg0, 0x05))
                                            {
                                                Store (Arg1, PTI5)
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        If (LEqual (DISP, 0x3F))
                        {
                            Store (One, PTI0)
                        }
                        Else
                        {
                            If (LAnd (DIS0, XOr (And (DISP, 0x3E), 0x3E)))
                            {
                                Store (Zero, PTI0)
                            }
                        }

                        Store (PTI, Local0)
                        Store (Zero, Local1)
                        While (Local0)
                        {
                            If (And (Local0, One))
                            {
                                Increment (Local1)
                            }

                            ShiftRight (Local0, One, Local0)
                        }

                        Store (Decrement (Local1), NOPT)
                        Store (Zero, WTEN)
                    }
                }

                Device (D012)
                {
                    Name (_ADR, 0x03)  // _ADR: Address
                }

                Device (D013)
                {
                    Name (_ADR, 0x04)  // _ADR: Address
                }

                Device (D014)
                {
                    Name (_ADR, 0x05)  // _ADR: Address
                }

                Device (D015)
                {
                    Name (_ADR, 0x06)  // _ADR: Address
                }

                Device (D016)
                {
                    Name (_ADR, 0x07)  // _ADR: Address
                }
            }

            Device (D0AC)
            {
                Name (_ADR, 0x00140000)  // _ADR: Address
            }

            Device (SBRG)
            {
                Name (_ADR, 0x00140003)  // ISA
                Device (PIC)
                {
                    Name (_HID, EisaId ("PNP0000"))  // _HID: Hardware ID
                    Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
                    {
                        IO (Decode16,
                            0x0020,             // Range Minimum
                            0x0020,             // Range Maximum
                            0x00,               // Alignment
                            0x02,               // Length
                            )
                        IO (Decode16,
                            0x00A0,             // Range Minimum
                            0x00A0,             // Range Maximum
                            0x00,               // Alignment
                            0x02,               // Length
                            )
                        IRQNoFlags ()
                            {2}
                    })
                }

                Device (DMAD)
                {
                    Name (_HID, EisaId ("PNP0200"))  // _HID: Hardware ID
                    Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
                    {
                        DMA (Compatibility, BusMaster, Transfer8, )
                            {4}
                        IO (Decode16,
                            0x0000,             // Range Minimum
                            0x0000,             // Range Maximum
                            0x00,               // Alignment
                            0x10,               // Length
                            )
                        IO (Decode16,
                            0x0081,             // Range Minimum
                            0x0081,             // Range Maximum
                            0x00,               // Alignment
                            0x03,               // Length
                            )
                        IO (Decode16,
                            0x0087,             // Range Minimum
                            0x0087,             // Range Maximum
                            0x00,               // Alignment
                            0x01,               // Length
                            )
                        IO (Decode16,
                            0x0089,             // Range Minimum
                            0x0089,             // Range Maximum
                            0x00,               // Alignment
                            0x03,               // Length
                            )
                        IO (Decode16,
                            0x008F,             // Range Minimum
                            0x008F,             // Range Maximum
                            0x00,               // Alignment
                            0x01,               // Length
                            )
                        IO (Decode16,
                            0x00C0,             // Range Minimum
                            0x00C0,             // Range Maximum
                            0x00,               // Alignment
                            0x20,               // Length
                            )
                    })
                }

                Device (TMR)
                {
                    Name (_HID, EisaId ("PNP0100"))  // _HID: Hardware ID
                    Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
                    {
                        IO (Decode16,
                            0x0040,             // Range Minimum
                            0x0040,             // Range Maximum
                            0x00,               // Alignment
                            0x04,               // Length
                            )
                        IRQNoFlags ()
                            {0}
                    })
                }

                Device (RTC0)
                {
                    Name (_HID, EisaId ("PNP0B00"))  // _HID: Hardware ID
                    Name (BUF0, ResourceTemplate ()
                    {
                        IO (Decode16,
                            0x0070,             // Range Minimum
                            0x0070,             // Range Maximum
                            0x00,               // Alignment
                            0x02,               // Length
                            )
                    })
                    Name (BUF1, ResourceTemplate ()
                    {
                        IO (Decode16,
                            0x0070,             // Range Minimum
                            0x0070,             // Range Maximum
                            0x00,               // Alignment
                            0x02,               // Length
                            )
                        IRQNoFlags ()
                            {8}
                    })
                    Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
                    {
                        If (LEqual (HPEN, One))
                        {
                            Return (BUF0)
                        }

                        Return (BUF1)
                    }
                }

                Device (SPKR)
                {
                    Name (_HID, EisaId ("PNP0800"))  // _HID: Hardware ID
                    Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
                    {
                        IO (Decode16,
                            0x0061,             // Range Minimum
                            0x0061,             // Range Maximum
                            0x00,               // Alignment
                            0x01,               // Length
                            )
                    })
                }

                Method (SPTS, 1, NotSerialized)
                {
                }

                Method (SWAK, 1, NotSerialized)
                {
                    If (PICM)
                    {
                        IRQC ()
                    }
                }

                Scope (\_SB)
                {
                    Name (SLPS, Zero)
                }

                OperationRegion (SMI0, SystemIO, SMIO, One)
                Field (SMI0, ByteAcc, NoLock, Preserve)
                {
                    SMIC,   8
                }

                Scope (\_SB)
                {
                    Scope (PCI0)
                    {
                        Device (S900)
                        {
                            Name (_HID, EisaId ("PNP0C02"))  // _HID: Hardware ID
                            Name (_UID, 0x0700)  // _UID: Unique ID
                            Name (_STA, 0x0F)  // _STA: Status
                            Name (CRS, ResourceTemplate ()
                            {
                                IO (Decode16,
                                    0x0010,             // Range Minimum
                                    0x0010,             // Range Maximum
                                    0x00,               // Alignment
                                    0x10,               // Length
                                    )
                                IO (Decode16,
                                    0x0022,             // Range Minimum
                                    0x0022,             // Range Maximum
                                    0x00,               // Alignment
                                    0x1E,               // Length
                                    )
                                IO (Decode16,
                                    0x0063,             // Range Minimum
                                    0x0063,             // Range Maximum
                                    0x00,               // Alignment
                                    0x01,               // Length
                                    )
                                IO (Decode16,
                                    0x0065,             // Range Minimum
                                    0x0065,             // Range Maximum
                                    0x00,               // Alignment
                                    0x01,               // Length
                                    )
                                IO (Decode16,
                                    0x0067,             // Range Minimum
                                    0x0067,             // Range Maximum
                                    0x00,               // Alignment
                                    0x09,               // Length
                                    )
                                IO (Decode16,
                                    0x0072,             // Range Minimum
                                    0x0072,             // Range Maximum
                                    0x00,               // Alignment
                                    0x0E,               // Length
                                    )
                                IO (Decode16,
                                    0x0080,             // Range Minimum
                                    0x0080,             // Range Maximum
                                    0x00,               // Alignment
                                    0x01,               // Length
                                    )
                                IO (Decode16,
                                    0x0084,             // Range Minimum
                                    0x0084,             // Range Maximum
                                    0x00,               // Alignment
                                    0x03,               // Length
                                    )
                                IO (Decode16,
                                    0x0088,             // Range Minimum
                                    0x0088,             // Range Maximum
                                    0x00,               // Alignment
                                    0x01,               // Length
                                    )
                                IO (Decode16,
                                    0x008C,             // Range Minimum
                                    0x008C,             // Range Maximum
                                    0x00,               // Alignment
                                    0x03,               // Length
                                    )
                                IO (Decode16,
                                    0x0090,             // Range Minimum
                                    0x0090,             // Range Maximum
                                    0x00,               // Alignment
                                    0x10,               // Length
                                    )
                                IO (Decode16,
                                    0x00A2,             // Range Minimum
                                    0x00A2,             // Range Maximum
                                    0x00,               // Alignment
                                    0x1E,               // Length
                                    )
                                IO (Decode16,
                                    0x00B1,             // Range Minimum
                                    0x00B1,             // Range Maximum
                                    0x00,               // Alignment
                                    0x01,               // Length
                                    )
                                IO (Decode16,
                                    0x00E0,             // Range Minimum
                                    0x00E0,             // Range Maximum
                                    0x00,               // Alignment
                                    0x10,               // Length
                                    )
                                IO (Decode16,
                                    0x04D0,             // Range Minimum
                                    0x04D0,             // Range Maximum
                                    0x00,               // Alignment
                                    0x02,               // Length
                                    )
                                IO (Decode16,
                                    0x040B,             // Range Minimum
                                    0x040B,             // Range Maximum
                                    0x00,               // Alignment
                                    0x01,               // Length
                                    )
                                IO (Decode16,
                                    0x04D6,             // Range Minimum
                                    0x04D6,             // Range Maximum
                                    0x00,               // Alignment
                                    0x01,               // Length
                                    )
                                IO (Decode16,
                                    0x0C00,             // Range Minimum
                                    0x0C00,             // Range Maximum
                                    0x00,               // Alignment
                                    0x02,               // Length
                                    )
                                IO (Decode16,
                                    0x0C14,             // Range Minimum
                                    0x0C14,             // Range Maximum
                                    0x00,               // Alignment
                                    0x01,               // Length
                                    )
                                IO (Decode16,
                                    0x0C50,             // Range Minimum
                                    0x0C50,             // Range Maximum
                                    0x00,               // Alignment
                                    0x02,               // Length
                                    )
                                IO (Decode16,
                                    0x0C52,             // Range Minimum
                                    0x0C52,             // Range Maximum
                                    0x00,               // Alignment
                                    0x01,               // Length
                                    )
                                IO (Decode16,
                                    0x0C6C,             // Range Minimum
                                    0x0C6C,             // Range Maximum
                                    0x00,               // Alignment
                                    0x01,               // Length
                                    )
                                IO (Decode16,
                                    0x0C6F,             // Range Minimum
                                    0x0C6F,             // Range Maximum
                                    0x00,               // Alignment
                                    0x01,               // Length
                                    )
                                IO (Decode16,
                                    0x0CD0,             // Range Minimum
                                    0x0CD0,             // Range Maximum
                                    0x00,               // Alignment
                                    0x02,               // Length
                                    )
                                IO (Decode16,
                                    0x0CD2,             // Range Minimum
                                    0x0CD2,             // Range Maximum
                                    0x00,               // Alignment
                                    0x02,               // Length
                                    )
                                IO (Decode16,
                                    0x0CD4,             // Range Minimum
                                    0x0CD4,             // Range Maximum
                                    0x00,               // Alignment
                                    0x02,               // Length
                                    )
                                IO (Decode16,
                                    0x0CD6,             // Range Minimum
                                    0x0CD6,             // Range Maximum
                                    0x00,               // Alignment
                                    0x02,               // Length
                                    )
                                IO (Decode16,
                                    0x0CD8,             // Range Minimum
                                    0x0CD8,             // Range Maximum
                                    0x00,               // Alignment
                                    0x08,               // Length
                                    )
                                IO (Decode16,
                                    0x0000,             // Range Minimum
                                    0x0000,             // Range Maximum
                                    0x00,               // Alignment
                                    0x00,               // Length
                                    _Y0D)
                                IO (Decode16,
                                    0x0000,             // Range Minimum
                                    0x0000,             // Range Maximum
                                    0x00,               // Alignment
                                    0x00,               // Length
                                    _Y10)
                                IO (Decode16,
                                    0x0000,             // Range Minimum
                                    0x0000,             // Range Maximum
                                    0x00,               // Alignment
                                    0x00,               // Length
                                    _Y0F)
                                IO (Decode16,
                                    0x0000,             // Range Minimum
                                    0x0000,             // Range Maximum
                                    0x00,               // Alignment
                                    0x00,               // Length
                                    _Y0E)
                                IO (Decode16,
                                    0x0900,             // Range Minimum
                                    0x0900,             // Range Maximum
                                    0x00,               // Alignment
                                    0x10,               // Length
                                    )
                                IO (Decode16,
                                    0x0910,             // Range Minimum
                                    0x0910,             // Range Maximum
                                    0x00,               // Alignment
                                    0x10,               // Length
                                    )
                                IO (Decode16,
                                    0xFE00,             // Range Minimum
                                    0xFE00,             // Range Maximum
                                    0x00,               // Alignment
                                    0xFF,               // Length
                                    )
                                IO (Decode16,
                                    0x0060,             // Range Minimum
                                    0x0060,             // Range Maximum
                                    0x00,               // Alignment
                                    0x00,               // Length
                                    )
                                IO (Decode16,
                                    0x0064,             // Range Minimum
                                    0x0064,             // Range Maximum
                                    0x00,               // Alignment
                                    0x00,               // Length
                                    )
                                Memory32Fixed (ReadWrite,
                                    0x00000000,         // Address Base
                                    0x00000000,         // Address Length
                                    _Y11)
                                Memory32Fixed (ReadWrite,
                                    0xFEC01000,         // Address Base
                                    0x00001000,         // Address Length
                                    )
                                Memory32Fixed (ReadWrite,
                                    0xFEDC0000,         // Address Base
                                    0x00001000,         // Address Length
                                    )
                                Memory32Fixed (ReadWrite,
                                    0xFEE00000,         // Address Base
                                    0x00001000,         // Address Length
                                    )
                                Memory32Fixed (ReadWrite,
                                    0xFED80000,         // Address Base
                                    0x00010000,         // Address Length
                                    )
                                Memory32Fixed (ReadWrite,
                                    0xFED61000,         // Address Base
                                    0x00010000,         // Address Length
                                    )
                                Memory32Fixed (ReadWrite,
                                    0x00000000,         // Address Base
                                    0x00000000,         // Address Length
                                    _Y12)
                                Memory32Fixed (ReadWrite,
                                    0x00000000,         // Address Base
                                    0x00000000,         // Address Length
                                    _Y13)
                                Memory32Fixed (ReadWrite,
                                    0x00000000,         // Address Base
                                    0x00000000,         // Address Length
                                    _Y14)
                            })
                            Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
                            {
                                CreateWordField (CRS, \_SB.PCI0.S900._Y0D._MIN, PBB)  // _MIN: Minimum Base Address
                                CreateWordField (CRS, \_SB.PCI0.S900._Y0D._MAX, PBH)  // _MAX: Maximum Base Address
                                CreateByteField (CRS, \_SB.PCI0.S900._Y0D._LEN, PML)  // _LEN: Length
                                Store (PMBS, PBB)
                                Store (PMBS, PBH)
                                Store (PMLN, PML)
                                If (SMBB)
                                {
                                    CreateWordField (CRS, \_SB.PCI0.S900._Y0E._MIN, SMB1)  // _MIN: Minimum Base Address
                                    CreateWordField (CRS, \_SB.PCI0.S900._Y0E._MAX, SMH1)  // _MAX: Maximum Base Address
                                    CreateByteField (CRS, \_SB.PCI0.S900._Y0E._LEN, SML1)  // _LEN: Length
                                    Store (SMBB, SMB1)
                                    Store (SMBB, SMH1)
                                    Store (SMBL, SML1)
                                    CreateWordField (CRS, \_SB.PCI0.S900._Y0F._MIN, SMBZ)  // _MIN: Minimum Base Address
                                    CreateWordField (CRS, \_SB.PCI0.S900._Y0F._MAX, SMH0)  // _MAX: Maximum Base Address
                                    CreateByteField (CRS, \_SB.PCI0.S900._Y0F._LEN, SML0)  // _LEN: Length
                                    Store (SMB0, SMBZ)
                                    Store (SMB0, SMH0)
                                    Store (SMBM, SML0)
                                }

                                If (GPBS)
                                {
                                    CreateWordField (CRS, \_SB.PCI0.S900._Y10._MIN, IGB)  // _MIN: Minimum Base Address
                                    CreateWordField (CRS, \_SB.PCI0.S900._Y10._MAX, IGH)  // _MAX: Maximum Base Address
                                    CreateByteField (CRS, \_SB.PCI0.S900._Y10._LEN, IGL)  // _LEN: Length
                                    Store (GPBS, IGB)
                                    Store (GPBS, IGH)
                                    Store (GPLN, IGL)
                                }

                                If (APCB)
                                {
                                    CreateDWordField (CRS, \_SB.PCI0.S900._Y11._BAS, APB)  // _BAS: Base Address
                                    CreateDWordField (CRS, \_SB.PCI0.S900._Y11._LEN, APL)  // _LEN: Length
                                    Store (APCB, APB)
                                    Store (APCL, APL)
                                }

                                If (SPIB)
                                {
                                    CreateDWordField (CRS, \_SB.PCI0.S900._Y12._BAS, SPIB)  // _BAS: Base Address
                                    CreateDWordField (CRS, \_SB.PCI0.S900._Y12._LEN, SPIL)  // _LEN: Length
                                    Store (\SPIB, SPIB)
                                    Store (\SPIL, SPIL)
                                }

                                If (WDTB)
                                {
                                    CreateDWordField (CRS, \_SB.PCI0.S900._Y13._BAS, WDTB)  // _BAS: Base Address
                                    CreateDWordField (CRS, \_SB.PCI0.S900._Y13._LEN, WDTL)  // _LEN: Length
                                    Store (\WDTB, WDTB)
                                    Store (\WDTL, WDTL)
                                }

                                CreateDWordField (CRS, \_SB.PCI0.S900._Y14._BAS, ROMB)  // _BAS: Base Address
                                CreateDWordField (CRS, \_SB.PCI0.S900._Y14._LEN, ROML)  // _LEN: Length
                                Store (0xFF000000, ROMB)
                                Store (0x01000000, ROML)
                                Return (CRS)
                            }
                        }
                    }
                }

/*                
                Scope (\_SB)
                {
                    Scope (PCI0)
                    {
                        Scope (SBRG)
                        {
                            Method (RRIO, 4, NotSerialized)
                            {
                                Store ("RRIO", Debug)
                            }

                            Method (RDMA, 3, NotSerialized)
                            {
                                Store ("rDMA", Debug)
                            }
                        }
                    }
                }
*/

                Scope (\_SB)
                {
                    OperationRegion(LUIE, SystemMemory, 0xFEDC0020, 0x04)
                    Field(LUIE, AnyAcc, NoLock, Preserve)
                    {
                      IER0, 1,
                      IER1, 1,
                      IER2, 1,
                      IER3, 1,
                      RESV, 4,
                      WUR0, 2,
                      WUR1, 2,
                      WUR2, 2,
                      WUR3, 2,
                    }   
                    
                    OperationRegion(FPIC, SystemIO, 0x0C00, 0x02)
                    Field(FPIC, AnyAcc, NoLock, Preserve)
                    {
                      FPII, 8,
                      FPID, 8,
                    }
                    IndexField(FPII, FPID, ByteAcc, NoLock, Preserve)
                    {
                      Offset(0xF4),	//Offset(244),
                      IUA0, 8,
                      IUA1, 8,
                      Offset(0xF8),	//Offset(248),
                      IUA2, 8,
                      IUA3, 8,
                    }     
                    
                    Method(FRUI, 1, Serialized)
                    {
                      If(LEqual(Arg0, Zero))
                      {
                        Return(IUA0)
                      }
                      Else
                      {
                        If(LEqual(Arg0, One))
                        {
                          Return(IUA1)
                        }
                        Else
                        {
                          If(LEqual(Arg0, 0x02))
                          {
                            Return(IUA2)
                          }
                          Else
                          {
                            If(LEqual(Arg0, 0x03))
                            {
                              Return(IUA3)
                            }
                            Else
                            {
                              Return(0x03)
                            }
                          }
                        }
                      }
                    }       
                    
                    Device(COM1)
                    {
                      Name(_HID, EISAID("PNP0501"))
                      Name(_DDN, "COM1")
                      Name(_UID, One)
                      Name(TIRQ, Zero)
                      Method(_STA, 0, NotSerialized)
                      {
                        If(IER3)
                        {
                          Return(0x0F)
                        }
                        Else
                        {
                          Return(Zero)
                        }
                      }
                      Method(_CRS, 0, NotSerialized)
                      {
                        Name(BUF0, ResourceTemplate()
                        {
                          IO(Decode16, 0x03F8, 0x03F8, 0x01, 0x08)
                          IRQNoFlags() {4}
                        })
                        CreateWordField(BUF0, 0x09, IRQL)
                        ShiftLeft(One, FRUI(WUR3), IRQL)
                        Return(BUF0)
                      }
                    }
                
                    Device (FUR0)
                    {
                        Name (_HID, "AMDI0020")  // _HID: Hardware ID
                        Name (_UID, Zero)  // _UID: Unique ID
                        Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
                        {
                            IRQ (Edge, ActiveHigh, Exclusive, )
                                {3}
                            Memory32Fixed (ReadWrite,
                                0xFEDC9000,         // Address Base
                                0x00001000,         // Address Length
                                )
                        })
                        Method (_STA, 0, NotSerialized)  // _STA: Status
                        {
                            Return (UT0E)
                        }
                    }

                    Device (FUR1)
                    {
                        Name (_HID, "AMDI0020")  // _HID: Hardware ID
                        Name (_UID, One)  // _UID: Unique ID
                        Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
                        {
                            IRQ (Edge, ActiveHigh, Exclusive, )
                                {4}
                            Memory32Fixed (ReadWrite,
                                0xFEDCA000,         // Address Base
                                0x00001000,         // Address Length
                                )
                        })
                        Method (_STA, 0, NotSerialized)  // _STA: Status
                        {
                            Return (UT1E)
                        }
                    }
                }

                Scope (\_SB)
                {
                    Device (I2CA)
                    {
                        Name (_HID, "AMDI0010")  // _HID: Hardware ID
                        Name (_UID, Zero)  // _UID: Unique ID
                        Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
                        {
                            IRQ (Edge, ActiveHigh, Exclusive, )
                                {10}
                            Memory32Fixed (ReadWrite,
                                0xFEDC2000,         // Address Base
                                0x00001000,         // Address Length
                                )
                        })
                        Method (_STA, 0, NotSerialized)  // _STA: Status
                        {
                            Return (ICAE)
                        }
                    }

                    Device (I2CB)
                    {
                        Name (_HID, "AMDI0010")  // _HID: Hardware ID
                        Name (_UID, One)  // _UID: Unique ID
                        Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
                        {
                            IRQ (Edge, ActiveHigh, Exclusive, )
                                {11}
                            Memory32Fixed (ReadWrite,
                                0xFEDC3000,         // Address Base
                                0x00001000,         // Address Length
                                )
                        })
                        Method (_STA, 0, NotSerialized)  // _STA: Status
                        {
                            Return (ICBE)
                        }
                    }

                    Device (I2CC)
                    {
                        Name (_HID, "AMDI0010")  // _HID: Hardware ID
                        Name (_UID, 0x02)  // _UID: Unique ID
                        Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
                        {
                            IRQ (Edge, ActiveHigh, Exclusive, )
                                {12}
                            Memory32Fixed (ReadWrite,
                                0xFEDC4000,         // Address Base
                                0x00001000,         // Address Length
                                )
                        })
                        Method (_STA, 0, NotSerialized)  // _STA: Status
                        {
                            Return (ICCE)
                        }
                    }

                    Device (I2CD)
                    {
                        Name (_HID, "AMDI0010")  // _HID: Hardware ID
                        Name (_UID, 0x03)  // _UID: Unique ID
                        Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
                        {
                            IRQ (Edge, ActiveHigh, Exclusive, )
                                {13}
                            Memory32Fixed (ReadWrite,
                                0xFEDC5000,         // Address Base
                                0x00001000,         // Address Length
                                )
                        })
                        Method (_STA, 0, NotSerialized)  // _STA: Status
                        {
                            Return (ICDE)
                        }
                    }

                    Device (I2CE)
                    {
                        Name (_HID, "AMDI0010")  // _HID: Hardware ID
                        Name (_UID, 0x04)  // _UID: Unique ID
                        Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
                        {
                            IRQ (Edge, ActiveHigh, Exclusive, )
                                {14}
                            Memory32Fixed (ReadWrite,
                                0xFEDC6000,         // Address Base
                                0x00001000,         // Address Length
                                )
                        })
                        Method (_STA, 0, NotSerialized)  // _STA: Status
                        {
                            Return (ICEE)
                        }
                    }

                    Device (I2CF)
                    {
                        Name (_HID, "AMDI0010")  // _HID: Hardware ID
                        Name (_UID, 0x05)  // _UID: Unique ID
                        Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
                        {
                            IRQ (Edge, ActiveHigh, Exclusive, )
                                {15}
                            Memory32Fixed (ReadWrite,
                                0xFEDCB000,         // Address Base
                                0x00001000,         // Address Length
                                )
                        })
                        Method (_STA, 0, NotSerialized)  // _STA: Status
                        {
                            Return (ICFE)
                        }
                    }
                }

                Scope (\_SB)
                {
                    Device (GPIO)
                    {
                        Name (_HID, "AMDI0030")  // _HID: Hardware ID
                        Name (_CID, "AMDI0030")  // _CID: Compatible ID
                        Name (_UID, Zero)  // _UID: Unique ID
                        Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
                        {
                            Name (RBUF, ResourceTemplate ()
                            {
                                Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, )
                                {
                                    0x00000007,
                                }
                                Memory32Fixed (ReadWrite,
                                    0xFED81500,         // Address Base
                                    0x00000300,         // Address Length
                                    )
                            })
                            Return (RBUF)
                        }

                        Method (_STA, 0, NotSerialized)  // _STA: Status
                        {
                            Return (0x0F)
                        }
                    }
                }
            }

            Device (D0AE)
            {
                Name (_ADR, 0x00140006)  // _ADR: Address
            }
        }