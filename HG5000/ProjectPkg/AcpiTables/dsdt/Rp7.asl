        Device (S1D3)
        {
            Name (_HID, EisaId ("PNP0A08"))  // _HID: Hardware ID
            Name (_CID, EisaId ("PNP0A03"))  // _CID: Compatible ID
            Name (_ADR, Zero)  // _ADR: Address
            Method (^BN07, 0, NotSerialized)
            {
                Return (0x07)
            }

            Method (_BBN, 0, NotSerialized)  // _BBN: BIOS Bus Number
            {
                Return (BN07 ())
            }

            Name (_UID, 0x07)  // _UID: Unique ID
            Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
            {
                If (PICM)
                {
                    Return (AR07)
                }

                Return (PR07)
            }

//-         Name (LVGA, 0x00)

            Name (CRS2, ResourceTemplate ()
            {
                WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
                    0x0000,             // Granularity
                    0x0080,             // Range Minimum
                    0x00FF,             // Range Maximum
                    0x0000,             // Translation Offset
                    0x0080,             // Length
                    ,, _Y39)
                WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                    0x0000,             // Granularity
                    0x0000,             // Range Minimum
                    0x0000,             // Range Maximum
                    0x0000,             // Translation Offset
                    0x0000,             // Length
                    ,, _Y3B, TypeStatic)
                WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                    0x0000,             // Granularity
                    0x0000,             // Range Minimum
                    0x0000,             // Range Maximum
                    0x0000,             // Translation Offset
                    0x0000,             // Length
                    ,, _Y3A, TypeStatic)
                DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                    0x00000000,         // Granularity
                    0x00000000,         // Range Minimum
                    0x00000000,         // Range Maximum
                    0x00000000,         // Translation Offset
                    0x00000000,         // Length
                    ,, _Y3C, AddressRangeMemory, TypeStatic)
                DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                    0x00000000,         // Granularity
                    0x80000000,         // Range Minimum
                    0xFFFFFFFF,         // Range Maximum
                    0x00000000,         // Translation Offset
                    0x80000000,         // Length
                    ,, _Y3D, AddressRangeMemory, TypeStatic)
                   
                QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                    0x0000000000000000, // Granularity
                    0x0000000000000000, // Range Minimum
                    0x0000000000000000, // Range Maximum
                    0x0000000000000000, // Translation Offset
                    0x0000000000000000, // Length
                    ,, _Y3E, AddressRangeMemory, TypeStatic)
                    
            })
            Method (_STA, 0, NotSerialized)  // _STA: Status
            {
              If(LGreater(\RPCT, 7)){
                Return(0xF)
              }
              Return(0) 
            }

            Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
            {
                CreateWordField (CRS2, \_SB.S1D3._Y39._MIN, MIN2)  // _MIN: Minimum Base Address
                CreateWordField (CRS2, \_SB.S1D3._Y39._MAX, MAX2)  // _MAX: Maximum Base Address
                CreateWordField (CRS2, \_SB.S1D3._Y39._LEN, LEN2)  // _LEN: Length
                Store(\R7BB, MIN2)
                Store(\R7BL, MAX2)
                Subtract(MAX2, MIN2, Local0)
                Add(Local0, 1, LEN2)
                
                CreateWordField (CRS2, \_SB.S1D3._Y3A._MIN, MIN4)  // _MIN: Minimum Base Address
                CreateWordField (CRS2, \_SB.S1D3._Y3A._MAX, MAX4)  // _MAX: Maximum Base Address
                CreateWordField (CRS2, \_SB.S1D3._Y3A._LEN, LEN4)  // _LEN: Length
                Store (\R7IB, MIN4)
                Store (\R7IL, LEN4)
                Store (LEN4, Local1)
                Add (MIN4, Decrement (Local1), MAX4)
                If (\GFX7)
                {
                    CreateWordField (CRS2, \_SB.S1D3._Y3B._MIN, IMN2)  // _MIN: Minimum Base Address
                    CreateWordField (CRS2, \_SB.S1D3._Y3B._MAX, IMX2)  // _MAX: Maximum Base Address
                    CreateWordField (CRS2, \_SB.S1D3._Y3B._LEN, ILN2)  // _LEN: Length
                    Store (0x03B0, IMN2)
                    Store (0x03DF, IMX2)
                    Store (0x30, ILN2)
                    CreateDWordField (CRS2, \_SB.S1D3._Y3C._MIN, VMN2)  // _MIN: Minimum Base Address
                    CreateDWordField (CRS2, \_SB.S1D3._Y3C._MAX, VMX2)  // _MAX: Maximum Base Address
                    CreateDWordField (CRS2, \_SB.S1D3._Y3C._LEN, VLN2)  // _LEN: Length
                    Store (0x000A0000, VMN2)
                    Store (0x000BFFFF, VMX2)
                    Store (0x00020000, VLN2)
                }

                CreateDWordField (CRS2, \_SB.S1D3._Y3D._MIN, MIN5)  // _MIN: Minimum Base Address
                CreateDWordField (CRS2, \_SB.S1D3._Y3D._MAX, MAX5)  // _MAX: Maximum Base Address
                CreateDWordField (CRS2, \_SB.S1D3._Y3D._LEN, LEN5)  // _LEN: Length
                Store (\RP7B, MIN5)
                Store (\RP7L, LEN5)
                Store (LEN5, Local1)
                Add (MIN5, Decrement (Local1), MAX5)
                
                CreateQWordField (CRS2, \_SB.S1D3._Y3E._MIN, MIN9)  // _MIN: Minimum Base Address
                CreateQWordField (CRS2, \_SB.S1D3._Y3E._MAX, MAX9)  // _MAX: Maximum Base Address
                CreateQWordField (CRS2, \_SB.S1D3._Y3E._LEN, LEN9)  // _LEN: Length
                Store (\R7HB, MIN9)
                Store (\R7HL, LEN9)
                Store (LEN9, Local0)
                Add (MIN9, Decrement (Local0), MAX9)
                
                Return (CRS2)
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
                            /* 0000 */   0x5B, 0x4D, 0xDB, 0x33, 0xF7, 0x1F, 0x1C, 0x40,
                            /* 0008 */   0x96, 0x57, 0x74, 0x41, 0xC0, 0x3D, 0xD7, 0x66
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

            Device (D7B0)
            {
                Name (_ADR, 0x00030001)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG96)
                    }

                    Return (PG96)
                }

                Device (D0EA)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (D7B1)
            {
                Name (_ADR, 0x00030002)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG97)
                    }

                    Return (PG97)
                }

                Device (D0EB)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (D7B2)
            {
                Name (_ADR, 0x00030003)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG98)
                    }

                    Return (PG98)
                }

                Device (D0EC)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (D7B3)
            {
                Name (_ADR, 0x00030004)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG99)
                    }

                    Return (PG99)
                }

                Device (D0ED)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (D7B4)
            {
                Name (_ADR, 0x00030005)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG9A)
                    }

                    Return (PG9A)
                }

                Device (D0EE)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (D7B5)
            {
                Name (_ADR, 0x00030006)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG9B)
                    }

                    Return (PG9B)
                }

                Device (D0EF)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (D7B6)
            {
                Name (_ADR, 0x00030007)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG9C)
                    }

                    Return (PG9C)
                }

                Device (D0F0)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (D7B7)
            {
                Name (_ADR, 0x00040001)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG9D)
                    }

                    Return (PG9D)
                }

                Device (D0F1)
                {
                    Name (_ADR, 0xFFFF)  // _ADR: Address
                }
            }

            Device (S1D3)
            {
                Name (_ADR, 0x00070001)  // _ADR: Address
                Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
                {
                    Return (GPRW (0x0B, 0x04))
                }

                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AG9E)
                    }

                    Return (PG9E)
                }

                Device (D09F)
                {
                    Name (_ADR, One)  // _ADR: Address
                }

                Device (D0A0)
                {
                    Name (_ADR, 0x02)  // _ADR: Address
                }

                Device (XHC0)
                {
                    Name (_ADR, 0x03)  // _ADR: Address
                }
            }

            Device (BRA2)
            {
                Name (_ADR, 0x00080001)  // _ADR: Address
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AGA2)
                    }

                    Return (PGA2)
                }

                Device (D0A3)
                {
                    Name (_ADR, One)  // _ADR: Address
                }

                Device (SATA)
                {
                    Name (_ADR, 0x02)  // _ADR: Address
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

                        Method (_ON, 0, NotSerialized)  // _ON_: Power On
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

                        Method (_ON, 0, NotSerialized)  // _ON_: Power On
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

                Device (D0A5)
                {
                    Name (_ADR, 0x03)  // _ADR: Address
                }

                Device (D0A6)
                {
                    Name (_ADR, 0x04)  // _ADR: Address
                }

                Device (D0A7)
                {
                    Name (_ADR, 0x05)  // _ADR: Address
                }

                Device (D0A8)
                {
                    Name (_ADR, 0x06)  // _ADR: Address
                }

                Device (D0A9)
                {
                    Name (_ADR, 0x07)  // _ADR: Address
                }
            }
        }