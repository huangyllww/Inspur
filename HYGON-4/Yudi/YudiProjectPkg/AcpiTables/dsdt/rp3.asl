
        Scope (\_SB)
        {
            Device (RRB3)
            {
                Name (_HID, EisaId ("PNP0A08") /* PCI Express Bus */)  // _HID: Hardware ID
                Name (_CID, EisaId ("PNP0A03") /* PCI Bus */)  // _CID: Compatible ID
                Name (_UID, 0x03)  // _UID: Unique ID
                Name (SUPP, Zero)
                Name (CTRL, Zero)
                Method (_BBN, 0, NotSerialized)  // _BBN: BIOS Bus Number
                {
                    Return (0x03)
                }

                Method (_PXM, 0, NotSerialized)  // _PXM: Device Proximity
                {
                    Return (\PXM3)
                }

                Name (CRS2, ResourceTemplate ()
                {
                    WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
                        0x0000,             // Granularity
                        0x0080,             // Range Minimum
                        0x00FF,             // Range Maximum
                        0x0000,             // Translation Offset
                        0x0080,             // Length
                        ,, _Y21)
                    WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                        0x0000,             // Granularity
                        0x0000,             // Range Minimum
                        0x0000,             // Range Maximum
                        0x0000,             // Translation Offset
                        0x0000,             // Length
                        ,, _Y23, TypeStatic)
                    WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                        0x0000,             // Granularity
                        0x0000,             // Range Minimum
                        0x0000,             // Range Maximum
                        0x0000,             // Translation Offset
                        0x0000,             // Length
                        ,, _Y22, TypeStatic)
                    DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                        0x00000000,         // Granularity
                        0x00000000,         // Range Minimum
                        0x00000000,         // Range Maximum
                        0x00000000,         // Translation Offset
                        0x00000000,         // Length
                        ,, _Y24, AddressRangeMemory, TypeStatic)
                    DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                        0x00000000,         // Granularity
                        0x80000000,         // Range Minimum
                        0xFFFFFFFF,         // Range Maximum
                        0x00000000,         // Translation Offset
                        0x80000000,         // Length
                        ,, _Y25, AddressRangeMemory, TypeStatic)

                    QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                        0x0000000000000000, // Granularity
                        0x0000000000000000, // Range Minimum
                        0x0000000000000000, // Range Maximum
                        0x0000000000000000, // Translation Offset
                        0x0000000000000000, // Length
                        ,, _Y26, AddressRangeMemory, TypeStatic)

                })

                Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
                {
                    CreateWordField (CRS2, \_SB.RRB3._Y21._MIN, MIN2)  // _MIN: Minimum Base Address
                    CreateWordField (CRS2, \_SB.RRB3._Y21._MAX, MAX2)  // _MAX: Maximum Base Address
                    CreateWordField (CRS2, \_SB.RRB3._Y21._LEN, LEN2)  // _LEN: Length
                    Store(\R3BB, MIN2)
                    Store(\R3BL, MAX2)
                    Subtract(MAX2, MIN2, Local0)
                    Add(Local0, 1, LEN2)

                    If (LNotEqual(\R3IL, Zero)){
                      CreateWordField (CRS2, \_SB.RRB3._Y22._MIN, MIN4)  // _MIN: Minimum Base Address
                      CreateWordField (CRS2, \_SB.RRB3._Y22._MAX, MAX4)  // _MAX: Maximum Base Address
                      CreateWordField (CRS2, \_SB.RRB3._Y22._LEN, LEN4)  // _LEN: Length
                      Store (\R3IB, MIN4)
                      Store (\R3IL, LEN4)
                      Store (LEN4, Local1)
                      Add (MIN4, Decrement (Local1), MAX4)
                    }
                    
                    If (\VEDI == 3)
                    {
                        CreateWordField (CRS2, \_SB.RRB3._Y23._MIN, IMN2)  // _MIN: Minimum Base Address
                        CreateWordField (CRS2, \_SB.RRB3._Y23._MAX, IMX2)  // _MAX: Maximum Base Address
                        CreateWordField (CRS2, \_SB.RRB3._Y23._LEN, ILN2)  // _LEN: Length
                        Store (0x03B0, IMN2)
                        Store (0x03DF, IMX2)
                        Store (0x30, ILN2)
                        CreateDWordField (CRS2, \_SB.RRB3._Y24._MIN, VMN2)  // _MIN: Minimum Base Address
                        CreateDWordField (CRS2, \_SB.RRB3._Y24._MAX, VMX2)  // _MAX: Maximum Base Address
                        CreateDWordField (CRS2, \_SB.RRB3._Y24._LEN, VLN2)  // _LEN: Length
                        Store (0x000A0000, VMN2)
                        Store (0x000BFFFF, VMX2)
                        Store (0x00020000, VLN2)
                    }

                    CreateDWordField (CRS2, \_SB.RRB3._Y25._MIN, MIN5)  // _MIN: Minimum Base Address
                    CreateDWordField (CRS2, \_SB.RRB3._Y25._MAX, MAX5)  // _MAX: Maximum Base Address
                    CreateDWordField (CRS2, \_SB.RRB3._Y25._LEN, LEN5)  // _LEN: Length
                    Store (\RP3B, MIN5)
                    Store (\RP3L, LEN5)
                    Store (LEN5, Local1)
                    Add (MIN5, Decrement (Local1), MAX5)

                    CreateQWordField (CRS2, \_SB.RRB3._Y26._MIN, MIN9)  // _MIN: Minimum Base Address
                    CreateQWordField (CRS2, \_SB.RRB3._Y26._MAX, MAX9)  // _MAX: Maximum Base Address
                    CreateQWordField (CRS2, \_SB.RRB3._Y26._LEN, LEN9)  // _LEN: Length
                    Store (\R3HB, MIN9)
                    Store (\R3HL, LEN9)
                    Store (LEN9, Local0)
                    Add (MIN9, Decrement (Local0), MAX9)

                    Return (CRS2)
                }

                Method (_STA, 0, NotSerialized)  // _STA: Status
                {
                    If (\R3BL == 0)
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
                        SUPP = CDW2 /* \_SB_.RRB3._OSC.CDW2 */
                        CTRL = CDW3 /* \_SB_.RRB3._OSC.CDW3 */
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

                        CDW3 = CTRL /* \_SB_.RRB3.CTRL */
                        Return (Arg3)
                    }
                    Else
                    {
                        CDW1 |= 0x04
                        Return (Arg3)
                    }
                }

                Name (PR30, Package (0x18)
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
                Name (AR30, Package (0x18)
                {
                    Package (0x04)
                    {
                        0x0001FFFF, 
                        Zero, 
                        Zero, 
                        0x78
                    }, 

                    Package (0x04)
                    {
                        0x0001FFFF, 
                        One, 
                        Zero, 
                        0x79
                    }, 

                    Package (0x04)
                    {
                        0x0001FFFF, 
                        0x02, 
                        Zero, 
                        0x7A
                    }, 

                    Package (0x04)
                    {
                        0x0001FFFF, 
                        0x03, 
                        Zero, 
                        0x7B
                    }, 

                    Package (0x04)
                    {
                        0x0002FFFF, 
                        Zero, 
                        Zero, 
                        0x94
                    }, 

                    Package (0x04)
                    {
                        0x0002FFFF, 
                        One, 
                        Zero, 
                        0x95
                    }, 

                    Package (0x04)
                    {
                        0x0002FFFF, 
                        0x02, 
                        Zero, 
                        0x96
                    }, 

                    Package (0x04)
                    {
                        0x0002FFFF, 
                        0x03, 
                        Zero, 
                        0x97
                    }, 

                    Package (0x04)
                    {
                        0x0003FFFF, 
                        Zero, 
                        Zero, 
                        0x96
                    }, 

                    Package (0x04)
                    {
                        0x0003FFFF, 
                        One, 
                        Zero, 
                        0x97
                    }, 

                    Package (0x04)
                    {
                        0x0003FFFF, 
                        0x02, 
                        Zero, 
                        0x94
                    }, 

                    Package (0x04)
                    {
                        0x0003FFFF, 
                        0x03, 
                        Zero, 
                        0x95
                    }, 

                    Package (0x04)
                    {
                        0x0004FFFF, 
                        Zero, 
                        Zero, 
                        0x7A
                    }, 

                    Package (0x04)
                    {
                        0x0004FFFF, 
                        One, 
                        Zero, 
                        0x7B
                    }, 

                    Package (0x04)
                    {
                        0x0004FFFF, 
                        0x02, 
                        Zero, 
                        0x78
                    }, 

                    Package (0x04)
                    {
                        0x0004FFFF, 
                        0x03, 
                        Zero, 
                        0x79
                    }, 

                    Package (0x04)
                    {
                        0x0007FFFF, 
                        Zero, 
                        Zero, 
                        0x8F
                    }, 

                    Package (0x04)
                    {
                        0x0007FFFF, 
                        Zero, 
                        One, 
                        0x8C
                    }, 

                    Package (0x04)
                    {
                        0x0007FFFF, 
                        Zero, 
                        0x02, 
                        0x8D
                    }, 

                    Package (0x04)
                    {
                        0x0007FFFF, 
                        Zero, 
                        0x03, 
                        0x8E
                    }, 

                    Package (0x04)
                    {
                        0x0008FFFF, 
                        Zero, 
                        Zero, 
                        0x89
                    }, 

                    Package (0x04)
                    {
                        0x0008FFFF, 
                        Zero, 
                        One, 
                        0x8A
                    }, 

                    Package (0x04)
                    {
                        0x0008FFFF, 
                        Zero, 
                        0x02, 
                        0x8B
                    }, 

                    Package (0x04)
                    {
                        0x0008FFFF, 
                        Zero, 
                        0x03, 
                        0x88
                    }
                })
                Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                {
                    If (PICM)
                    {
                        Return (AR30) /* \_SB_.RRB3.AR30 */
                    }
                    Else
                    {
                        Return (PR30) /* \_SB_.RRB3.PR30 */
                    }
                }

                Device (R3G0)
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

                    Name (PR31, Package (0x04)
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
                    Name (AR31, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x78
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x79
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x7A
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x7B
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR31) /* \_SB_.RRB3.R3G0.AR31 */
                        }
                        Else
                        {
                            Return (PR31) /* \_SB_.RRB3.R3G0.PR31 */
                        }
                    }
                }

                Device (R3G1)
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

                    Name (PR32, Package (0x04)
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
                    Name (AR32, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x7C
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x7D
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x7E
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x7F
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR32) /* \_SB_.RRB3.R3G1.AR32 */
                        }
                        Else
                        {
                            Return (PR32) /* \_SB_.RRB3.R3G1.PR32 */
                        }
                    }
                }

                Device (R3G2)
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

                    Name (PR33, Package (0x04)
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
                    Name (AR33, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x80
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x81
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x82
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x83
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR33) /* \_SB_.RRB3.R3G2.AR33 */
                        }
                        Else
                        {
                            Return (PR33) /* \_SB_.RRB3.R3G2.PR33 */
                        }
                    }
                }

                Device (R3G3)
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

                    Name (PR34, Package (0x04)
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
                    Name (AR34, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x84
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x85
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x86
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x87
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR34) /* \_SB_.RRB3.R3G3.AR34 */
                        }
                        Else
                        {
                            Return (PR34) /* \_SB_.RRB3.R3G3.PR34 */
                        }
                    }
                }

                Device (R3G4)
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

                    Name (PR35, Package (0x04)
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
                    Name (AR35, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x88
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x89
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x8A
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x8B
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR35) /* \_SB_.RRB3.R3G4.AR35 */
                        }
                        Else
                        {
                            Return (PR35) /* \_SB_.RRB3.R3G4.PR35 */
                        }
                    }
                }

                Device (R3G5)
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

                    Name (PR36, Package (0x04)
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
                    Name (AR36, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x8C
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x8D
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x8E
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x8F
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR36) /* \_SB_.RRB3.R3G5.AR36 */
                        }
                        Else
                        {
                            Return (PR36) /* \_SB_.RRB3.R3G5.PR36 */
                        }
                    }
                }

                Device (R3G6)
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

                    Name (PR37, Package (0x04)
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
                    Name (AR37, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x90
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x91
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x92
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x93
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR37) /* \_SB_.RRB3.R3G6.AR37 */
                        }
                        Else
                        {
                            Return (PR37) /* \_SB_.RRB3.R3G6.PR37 */
                        }
                    }
                }

                Device (R3G7)
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

                    Name (PR38, Package (0x04)
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
                    Name (AR38, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x94
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x95
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x96
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x97
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR38) /* \_SB_.RRB3.R3G7.AR38 */
                        }
                        Else
                        {
                            Return (PR38) /* \_SB_.RRB3.R3G7.PR38 */
                        }
                    }
                }

                Device (R3G8)
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

                    Name (PR39, Package (0x04)
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
                    Name (AR39, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x96
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x97
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x94
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x95
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR39) /* \_SB_.RRB3.R3G8.AR39 */
                        }
                        Else
                        {
                            Return (PR39) /* \_SB_.RRB3.R3G8.PR39 */
                        }
                    }
                }

                Device (R3G9)
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

                    Name (PR3A, Package (0x04)
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
                    Name (AR3A, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x92
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x93
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x90
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x91
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR3A) /* \_SB_.RRB3.R3G9.AR3A */
                        }
                        Else
                        {
                            Return (PR3A) /* \_SB_.RRB3.R3G9.PR3A */
                        }
                    }
                }

                Device (R3GA)
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

                    Name (PR3B, Package (0x04)
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
                    Name (AR3B, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x8E
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x8F
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x8C
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x8D
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR3B) /* \_SB_.RRB3.R3GA.AR3B */
                        }
                        Else
                        {
                            Return (PR3B) /* \_SB_.RRB3.R3GA.PR3B */
                        }
                    }
                }

                Device (R3GB)
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

                    Name (PR3C, Package (0x04)
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
                    Name (AR3C, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x8A
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x8B
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x88
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x89
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR3C) /* \_SB_.RRB3.R3GB.AR3C */
                        }
                        Else
                        {
                            Return (PR3C) /* \_SB_.RRB3.R3GB.PR3C */
                        }
                    }
                }

                Device (R3GC)
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

                    Name (PR3D, Package (0x04)
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
                    Name (AR3D, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x86
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x87
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x84
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x85
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR3D) /* \_SB_.RRB3.R3GC.AR3D */
                        }
                        Else
                        {
                            Return (PR3D) /* \_SB_.RRB3.R3GC.PR3D */
                        }
                    }
                }

                Device (R3GD)
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

                    Name (PR3E, Package (0x04)
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
                    Name (AR3E, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x82
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x83
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x80
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x81
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR3E) /* \_SB_.RRB3.R3GD.AR3E */
                        }
                        Else
                        {
                            Return (PR3E) /* \_SB_.RRB3.R3GD.PR3E */
                        }
                    }
                }

                Device (R3GE)
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

                    Name (PR3F, Package (0x04)
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
                    Name (AR3F, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x7E
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x7F
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x7C
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x7D
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR3F) /* \_SB_.RRB3.R3GE.AR3F */
                        }
                        Else
                        {
                            Return (PR3F) /* \_SB_.RRB3.R3GE.PR3F */
                        }
                    }
                }

                Device (R3GF)
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

                    Name (PR3G, Package (0x04)
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
                    Name (AR3G, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x7A
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x7B
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x78
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x79
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR3G) /* \_SB_.RRB3.R3GF.AR3G */
                        }
                        Else
                        {
                            Return (PR3G) /* \_SB_.RRB3.R3GF.PR3G */
                        }
                    }
                }

                Device (R3BB)
                {
                    Name (_ADR, 0x00070001)  // _ADR: Address
                    Name (PR3H, Package (0x04)
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
                    Name (AR3H, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x8F
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x8C
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x8D
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x8E
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR3H) /* \_SB_.RRB3.R3BB.AR3H */
                        }
                        Else
                        {
                            Return (PR3H) /* \_SB_.RRB3.R3BB.PR3H */
                        }
                    }
                }

                Device (R3BC)
                {
                    Name (_ADR, 0x00080001)  // _ADR: Address
                    Name (PR3I, Package (0x04)
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
                    Name (AR3I, Package (0x04)
                    {
                        Package (0x04)
                        {
                            0xFFFF, 
                            Zero, 
                            Zero, 
                            0x89
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            One, 
                            Zero, 
                            0x8A
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x02, 
                            Zero, 
                            0x8B
                        }, 

                        Package (0x04)
                        {
                            0xFFFF, 
                            0x03, 
                            Zero, 
                            0x88
                        }
                    })
                    Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
                    {
                        If (PICM)
                        {
                            Return (AR3I) /* \_SB_.RRB3.R3BC.AR3I */
                        }
                        Else
                        {
                            Return (PR3I) /* \_SB_.RRB3.R3BC.PR3I */
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
                                Return (SAD0) /* \_SB_.RRB3.R3BC.SATA.SAD0 */
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
                                Return (SAD3) /* \_SB_.RRB3.R3BC.SATA.SAD3 */
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
                            BA_5 = BA05 /* \_SB_.RRB3.R3BC.SATA.BA05 */
                            If (((BA_5 == 0xFFFFFFFF) || (STCL != 0x0101)))
                            {
                                B5EN = Zero
                                Return (SBAR) /* \_SB_.RRB3.R3BC.SATA.SBAR */
                            }
                            Else
                            {
                                B5EN = One
                                Return (BA_5) /* \_SB_.RRB3.R3BC.SATA.BA_5 */
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
                                Return (SPTM) /* \_SB_.RRB3.R3BC.SATA.PRID.SPTM */
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
                                Return (SPTM) /* \_SB_.RRB3.R3BC.SATA.SECD.SPTM */
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

                            Local0 = PTI /* \_SB_.RRB3.R3BC.SATA.PTI_ */
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