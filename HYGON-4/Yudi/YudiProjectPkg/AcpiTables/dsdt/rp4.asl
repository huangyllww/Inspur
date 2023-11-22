
        Scope (\_SB)
        {
            Device (RRB4)
            {
                Name (_HID, EisaId ("PNP0A08") /* PCI Express Bus */)  // _HID: Hardware ID
                Name (_CID, EisaId ("PNP0A03") /* PCI Bus */)  // _CID: Compatible ID
                Name (_UID, 0x04)  // _UID: Unique ID
                Name (SUPP, Zero)
                Name (CTRL, Zero)
                Method (_BBN, 0, NotSerialized)  // _BBN: BIOS Bus Number
                {
                    Return (4)
                }

                Method (_PXM, 0, NotSerialized)  // _PXM: Device Proximity
                {
                    Return (\PXM4)
                }

                Name (CRS2, ResourceTemplate ()
                {
                    WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
                        0x0000,             // Granularity
                        0x0080,             // Range Minimum
                        0x00FF,             // Range Maximum
                        0x0000,             // Translation Offset
                        0x0080,             // Length
                        ,, _Y27)
                    WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                        0x0000,             // Granularity
                        0x0000,             // Range Minimum
                        0x0000,             // Range Maximum
                        0x0000,             // Translation Offset
                        0x0000,             // Length
                        ,, _Y29, TypeStatic)
                    WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                        0x0000,             // Granularity
                        0x0000,             // Range Minimum
                        0x0000,             // Range Maximum
                        0x0000,             // Translation Offset
                        0x0000,             // Length
                        ,, _Y28, TypeStatic)
                    DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                        0x00000000,         // Granularity
                        0x00000000,         // Range Minimum
                        0x00000000,         // Range Maximum
                        0x00000000,         // Translation Offset
                        0x00000000,         // Length
                        ,, _Y2A, AddressRangeMemory, TypeStatic)
                    DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                        0x00000000,         // Granularity
                        0x80000000,         // Range Minimum
                        0xFFFFFFFF,         // Range Maximum
                        0x00000000,         // Translation Offset
                        0x80000000,         // Length
                        ,, _Y2B, AddressRangeMemory, TypeStatic)

                    QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                        0x0000000000000000, // Granularity
                        0x0000000000000000, // Range Minimum
                        0x0000000000000000, // Range Maximum
                        0x0000000000000000, // Translation Offset
                        0x0000000000000000, // Length
                        ,, _Y2C, AddressRangeMemory, TypeStatic)

                })

                Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
                {
                    CreateWordField (CRS2, \_SB.RRB4._Y27._MIN, MIN2)  // _MIN: Minimum Base Address
                    CreateWordField (CRS2, \_SB.RRB4._Y27._MAX, MAX2)  // _MAX: Maximum Base Address
                    CreateWordField (CRS2, \_SB.RRB4._Y27._LEN, LEN2)  // _LEN: Length
                    Store(\R4BB, MIN2)
                    Store(\R4BL, MAX2)
                    Subtract(MAX2, MIN2, Local0)
                    Add(Local0, 1, LEN2)

                    If (LNotEqual(\R4IL, Zero)){
                      CreateWordField (CRS2, \_SB.RRB4._Y28._MIN, MIN4)  // _MIN: Minimum Base Address
                      CreateWordField (CRS2, \_SB.RRB4._Y28._MAX, MAX4)  // _MAX: Maximum Base Address
                      CreateWordField (CRS2, \_SB.RRB4._Y28._LEN, LEN4)  // _LEN: Length
                      Store (\R4IB, MIN4)
                      Store (\R4IL, LEN4)
                      Store (LEN4, Local1)
                      Add (MIN4, Decrement (Local1), MAX4)
                    }
                    
                    If (\VEDI == 4)
                    {
                        CreateWordField (CRS2, \_SB.RRB4._Y29._MIN, IMN2)  // _MIN: Minimum Base Address
                        CreateWordField (CRS2, \_SB.RRB4._Y29._MAX, IMX2)  // _MAX: Maximum Base Address
                        CreateWordField (CRS2, \_SB.RRB4._Y29._LEN, ILN2)  // _LEN: Length
                        Store (0x03B0, IMN2)
                        Store (0x03DF, IMX2)
                        Store (0x30, ILN2)
                        CreateDWordField (CRS2, \_SB.RRB4._Y2A._MIN, VMN2)  // _MIN: Minimum Base Address
                        CreateDWordField (CRS2, \_SB.RRB4._Y2A._MAX, VMX2)  // _MAX: Maximum Base Address
                        CreateDWordField (CRS2, \_SB.RRB4._Y2A._LEN, VLN2)  // _LEN: Length
                        Store (0x000A0000, VMN2)
                        Store (0x000BFFFF, VMX2)
                        Store (0x00020000, VLN2)
                    }

                    CreateDWordField (CRS2, \_SB.RRB4._Y2B._MIN, MIN5)  // _MIN: Minimum Base Address
                    CreateDWordField (CRS2, \_SB.RRB4._Y2B._MAX, MAX5)  // _MAX: Maximum Base Address
                    CreateDWordField (CRS2, \_SB.RRB4._Y2B._LEN, LEN5)  // _LEN: Length
                    Store (\RP4B, MIN5)
                    Store (\RP4L, LEN5)
                    Store (LEN5, Local1)
                    Add (MIN5, Decrement (Local1), MAX5)

                    CreateQWordField (CRS2, \_SB.RRB4._Y2C._MIN, MIN9)  // _MIN: Minimum Base Address
                    CreateQWordField (CRS2, \_SB.RRB4._Y2C._MAX, MAX9)  // _MAX: Maximum Base Address
                    CreateQWordField (CRS2, \_SB.RRB4._Y2C._LEN, LEN9)  // _LEN: Length
                    Store (\R4HB, MIN9)
                    Store (\R4HL, LEN9)
                    Store (LEN9, Local0)
                    Add (MIN9, Decrement (Local0), MAX9)

                    Return (CRS2)
                }

                Method (_STA, 0, NotSerialized)  // _STA: Status
                {
                    If (\R4BL == 0)
                    {
                        Return (Zero)
                    }
                    Else
                    {
                        Return (0x0F)
                    }
                }

                Method (_OSC, 4, Serialized)  // _OSC: Operating System Capabilities
                {
                    CreateDWordField (Arg3, Zero, CDW1)
                    CreateDWordField (Arg3, 0x04, CDW2)
                    CreateDWordField (Arg3, 0x08, CDW3)
                    If ((Arg0 == ToUUID ("33db4d5b-1ff7-401c-9657-7441c03dd766") /* PCI Host Bridge Device */))
                    {
                        SUPP = CDW2 /* \_SB_.RRB4._OSC.CDW2 */
                        CTRL = CDW3 /* \_SB_.RRB4._OSC.CDW3 */
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

                        CDW3 = CTRL /* \_SB_.RRB4.CTRL */
                        Return (Arg3)
                    }
                    Else
                    {
                        CDW1 |= 0x04
                        Return (Arg3)
                    }
                }

                Name (PR40, Package (0x18)
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
                Name (AR40, Package (0x18)
                {
                    Package (0x04)
                    {
                        0x0001FFFF, 
                        Zero, 
                        Zero, 
                        0x98
                    }, 

                    Package (0x04)
                    {
                        0x0001FFFF, 
                        One, 
                        Zero, 
                        0x99
                    }, 

                    Package (0x04)
                    {
                        0x0001FFFF, 
                        0x02, 
                        Zero, 
                        0x9A
                    }, 

                    Package (0x04)
                    {
                        0x0001FFFF, 
                        0x03, 
                        Zero, 
                        0x9B
                    }, 

                    Package (0x04)
                    {
                        0x0002FFFF, 
                        Zero, 
                        Zero, 
                        0xB4
                    }, 

                    Package (0x04)
                    {
                        0x0002FFFF, 
                        One, 
                        Zero, 
                        0xB5
                    }, 

                    Package (0x04)
                    {
                        0x0002FFFF, 
                        0x02, 
                        Zero, 
                        0xB6
                    }, 

                    Package (0x04)
                    {
                        0x0002FFFF, 
                        0x03, 
                        Zero, 
                        0xB7
                    }, 

                    Package (0x04)
                    {
                        0x0003FFFF, 
                        Zero, 
                        Zero, 
                        0xB6
                    }, 

                    Package (0x04)
                    {
                        0x0003FFFF, 
                        One, 
                        Zero, 
                        0xB7
                    }, 

                    Package (0x04)
                    {
                        0x0003FFFF, 
                        0x02, 
                        Zero, 
                        0xB4
                    }, 

                    Package (0x04)
                    {
                        0x0003FFFF, 
                        0x03, 
                        Zero, 
                        0xB5
                    }, 

                    Package (0x04)
                    {
                        0x0004FFFF, 
                        Zero, 
                        Zero, 
                        0x9A
                    }, 

                    Package (0x04)
                    {
                        0x0004FFFF, 
                        One, 
                        Zero, 
                        0x9B
                    }, 

                    Package (0x04)
                    {
                        0x0004FFFF, 
                        0x02, 
                        Zero, 
                        0x98
                    }, 

                    Package (0x04)
                    {
                        0x0004FFFF, 
                        0x03, 
                        Zero, 
                        0x99
                    }, 

                    Package (0x04)
                    {
                        0x0007FFFF, 
                        Zero, 
                        Zero, 
                        0xAF
                    }, 

                    Package (0x04)
                    {
                        0x0007FFFF, 
                        Zero, 
                        One, 
                        0xAC
                    }, 

                    Package (0x04)
                    {
                        0x0007FFFF, 
                        Zero, 
                        0x02, 
                        0xAD
                    }, 

                    Package (0x04)
                    {
                        0x0007FFFF, 
                        Zero, 
                        0x03, 
                        0xAE
                    }, 

                    Package (0x04)
                    {
                        0x0008FFFF, 
                        Zero, 
                        Zero, 
                        0xA9
                    }, 

                    Package (0x04)
                    {
                        0x0008FFFF, 
                        Zero, 
                        One, 
                        0xAA
                    }, 

                    Package (0x04)
                    {
                        0x0008FFFF, 
                        Zero, 
                        0x02, 
                        0xAB
                    }, 

                    Package (0x04)
                    {
                        0x0008FFFF, 
                        Zero, 
                        0x03, 
                        0xA8
                    }
                })
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AR40) /* \_SB_.RRB4.AR40 */
                    }
                    Else
                    {
                        Return (PR40) /* \_SB_.RRB4.PR40 */
                    }
                }

                Device (R4G0)
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

                    Name (PR41, Package (0x04)
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
                    Name (AR41, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x98
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x99
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x9A
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x9B
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR41) /* \_SB_.RRB4.R4G0.AR41 */
                        }
                        Else
                        {
                            Return (PR41) /* \_SB_.RRB4.R4G0.PR41 */
                        }
                    }
                }

                Device (R4G1)
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

                    Name (PR42, Package (0x04)
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
                    Name (AR42, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x9C
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x9D
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x9E
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x9F
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR42) /* \_SB_.RRB4.R4G1.AR42 */
                        }
                        Else
                        {
                            Return (PR42) /* \_SB_.RRB4.R4G1.PR42 */
                        }
                    }
                }

                Device (R4G2)
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

                    Name (PR43, Package (0x04)
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
                    Name (AR43, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0xA0
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0xA1
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0xA2
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0xA3
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR43) /* \_SB_.RRB4.R4G2.AR43 */
                        }
                        Else
                        {
                            Return (PR43) /* \_SB_.RRB4.R4G2.PR43 */
                        }
                    }
                }

                Device (R4G3)
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

                    Name (PR44, Package (0x04)
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
                    Name (AR44, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0xA4
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0xA5
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0xA6
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0xA7
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR44) /* \_SB_.RRB4.R4G3.AR44 */
                        }
                        Else
                        {
                            Return (PR44) /* \_SB_.RRB4.R4G3.PR44 */
                        }
                    }
                }

                Device (R4G4)
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

                    Name (PR45, Package (0x04)
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
                    Name (AR45, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0xA8
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0xA9
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0xAA
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0xAB
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR45) /* \_SB_.RRB4.R4G4.AR45 */
                        }
                        Else
                        {
                            Return (PR45) /* \_SB_.RRB4.R4G4.PR45 */
                        }
                    }
                }

                Device (R4G5)
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

                    Name (PR46, Package (0x04)
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
                    Name (AR46, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0xAC
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0xAD
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0xAE
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0xAF
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR46) /* \_SB_.RRB4.R4G5.AR46 */
                        }
                        Else
                        {
                            Return (PR46) /* \_SB_.RRB4.R4G5.PR46 */
                        }
                    }
                }

                Device (R4G6)
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

                    Name (PR47, Package (0x04)
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
                    Name (AR47, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0xB0
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0xB1
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0xB2
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0xB3
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR47) /* \_SB_.RRB4.R4G6.AR47 */
                        }
                        Else
                        {
                            Return (PR47) /* \_SB_.RRB4.R4G6.PR47 */
                        }
                    }
                }

                Device (R4G7)
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

                    Name (PR48, Package (0x04)
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
                    Name (AR48, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0xB4
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0xB5
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0xB6
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0xB7
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR48) /* \_SB_.RRB4.R4G7.AR48 */
                        }
                        Else
                        {
                            Return (PR48) /* \_SB_.RRB4.R4G7.PR48 */
                        }
                    }
                }

                Device (R4G8)
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

                    Name (PR49, Package (0x04)
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
                    Name (AR49, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0xB6
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0xB7
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0xB4
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0xB5
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR49) /* \_SB_.RRB4.R4G8.AR49 */
                        }
                        Else
                        {
                            Return (PR49) /* \_SB_.RRB4.R4G8.PR49 */
                        }
                    }
                }

                Device (R4G9)
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

                    Name (PR4A, Package (0x04)
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
                    Name (AR4A, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0xB2
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0xB3
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0xB0
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0xB1
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR4A) /* \_SB_.RRB4.R4G9.AR4A */
                        }
                        Else
                        {
                            Return (PR4A) /* \_SB_.RRB4.R4G9.PR4A */
                        }
                    }
                }

                Device (R4GA)
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

                    Name (PR4B, Package (0x04)
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
                    Name (AR4B, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0xAE
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0xAF
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0xAC
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0xAD
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR4B) /* \_SB_.RRB4.R4GA.AR4B */
                        }
                        Else
                        {
                            Return (PR4B) /* \_SB_.RRB4.R4GA.PR4B */
                        }
                    }
                }

                Device (R4GB)
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

                    Name (PR4C, Package (0x04)
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
                    Name (AR4C, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0xAA
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0xAB
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0xA8
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0xA9
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR4C) /* \_SB_.RRB4.R4GB.AR4C */
                        }
                        Else
                        {
                            Return (PR4C) /* \_SB_.RRB4.R4GB.PR4C */
                        }
                    }
                }

                Device (R4GC)
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

                    Name (PR4D, Package (0x04)
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
                    Name (AR4D, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0xA6
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0xA7
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0xA4
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0xA5
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR4D) /* \_SB_.RRB4.R4GC.AR4D */
                        }
                        Else
                        {
                            Return (PR4D) /* \_SB_.RRB4.R4GC.PR4D */
                        }
                    }
                }

                Device (R4GD)
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

                    Name (PR4E, Package (0x04)
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
                    Name (AR4E, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0xA2
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0xA3
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0xA0
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0xA1
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR4E) /* \_SB_.RRB4.R4GD.AR4E */
                        }
                        Else
                        {
                            Return (PR4E) /* \_SB_.RRB4.R4GD.PR4E */
                        }
                    }
                }

                Device (R4GE)
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

                    Name (PR4F, Package (0x04)
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
                    Name (AR4F, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x9E
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x9F
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x9C
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x9D
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR4F) /* \_SB_.RRB4.R4GE.AR4F */
                        }
                        Else
                        {
                            Return (PR4F) /* \_SB_.RRB4.R4GE.PR4F */
                        }
                    }
                }

                Device (R4GF)
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

                    Name (PR4G, Package (0x04)
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
                    Name (AR4G, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x9A
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x9B
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x98
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x99
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR4G) /* \_SB_.RRB4.R4GF.AR4G */
                        }
                        Else
                        {
                            Return (PR4G) /* \_SB_.RRB4.R4GF.PR4G */
                        }
                    }
                }

                Device (R4BB)
                {
                    Name (_ADR, 0x00070001)  // _ADR: Address
                    Method (_PRW, 0, NotSerialized)  // _PRW: Power Resources for Wake
                    {
                        Return (GPRW (0x19, 0x04))
                    }

                    Name (PR4H, Package (0x04)
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
                    Name (AR4H, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0xAF
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0xAC
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0xAD
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0xAE
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR4H) /* \_SB_.RRB4.R4BB.AR4H */
                        }
                        Else
                        {
                            Return (PR4H) /* \_SB_.RRB4.R4BB.PR4H */
                        }
                    }

                    Device (XHCI)
                    {
                        Name (_ADR, 0x03)  // _ADR: Address
                    }
                }

                Device (R4BC)
                {
                    Name (_ADR, 0x00080001)  // _ADR: Address
                    Name (PR4I, Package (0x04)
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
                    Name (AR4I, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0xA9
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0xAA
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0xAB
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0xA8
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR4I) /* \_SB_.RRB4.R4BC.AR4I */
                        }
                        Else
                        {
                            Return (PR4I) /* \_SB_.RRB4.R4BC.PR4I */
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
                                Return (SAD0) /* \_SB_.RRB4.R4BC.SATA.SAD0 */
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
                                Return (SAD3) /* \_SB_.RRB4.R4BC.SATA.SAD3 */
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
                            BA_5 = BA05 /* \_SB_.RRB4.R4BC.SATA.BA05 */
                            If (((BA_5 == 0xFFFFFFFF) || (STCL != 0x0101)))
                            {
                                B5EN = Zero
                                Return (SBAR) /* \_SB_.RRB4.R4BC.SATA.SBAR */
                            }
                            Else
                            {
                                B5EN = One
                                Return (BA_5) /* \_SB_.RRB4.R4BC.SATA.BA_5 */
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
                                Return (SPTM) /* \_SB_.RRB4.R4BC.SATA.PRID.SPTM */
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
                                Return (SPTM) /* \_SB_.RRB4.R4BC.SATA.SECD.SPTM */
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

                            Local0 = PTI /* \_SB_.RRB4.R4BC.SATA.PTI_ */
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
                }
            }
        }