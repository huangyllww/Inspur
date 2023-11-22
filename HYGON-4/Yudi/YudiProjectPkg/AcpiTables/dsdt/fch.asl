
Scope (_SB)
{
    OperationRegion (LUIE, SystemMemory, 0xFEDC0020, 0x04)
    Field (LUIE, AnyAcc, NoLock, Preserve)
    {
        IER0,   1, 
        IER1,   1, 
        IER2,   1, 
        IER3,   1, 
        UOL0,   1, 
        UOL1,   1, 
        UOL2,   1, 
        UOL3,   1, 
        WUR0,   2, 
        WUR1,   2, 
        WUR2,   2, 
        WUR3,   2
    }

    OperationRegion (FPIC, SystemIO, 0x0C00, 0x02)
    Field (FPIC, AnyAcc, NoLock, Preserve)
    {
        FPII,   8, 
        FPID,   8
    }

    IndexField (FPII, FPID, ByteAcc, NoLock, Preserve)
    {
        Offset (0xF4), 
        IUA0,   8, 
        IUA1,   8, 
        Offset (0xF8), 
        IUA2,   8, 
        IUA3,   8
    }

    Method (FRUI, 1, Serialized)
    {
        Local0 = Zero
        If ((Arg0 == Zero))
        {
            Local0 = IUA0 /* \_SB_.IUA0 */
        }

        If ((Arg0 == One))
        {
            Local0 = IUA1 /* \_SB_.IUA1 */
        }

        If ((Arg0 == 0x02))
        {
            Local0 = IUA2 /* \_SB_.IUA2 */
        }

        If ((Arg0 == 0x03))
        {
            Local0 = IUA3 /* \_SB_.IUA3 */
        }

        Return (Local0)
    }

    Method (CKST, 1, Serialized)
    {
        If (((IER0 == One) && (WUR0 == Arg0)))
        {
            Return (Zero)
        }
        ElseIf (((IER1 == One) && (WUR1 == Arg0)))
        {
            Return (Zero)
        }
        ElseIf (((IER2 == One) && (WUR2 == Arg0)))
        {
            Return (Zero)
        }
        ElseIf (((IER3 == One) && (WUR3 == Arg0)))
        {
            Return (Zero)
        }
        Else
        {
            Return (One)
        }
    }

    Method (SRAD, 2, Serialized)
    {
        Local0 = (Arg0 << One)
        Local0 += 0xFED81E40
        OperationRegion (ADCR, SystemMemory, Local0, 0x02)
        Field (ADCR, ByteAcc, NoLock, Preserve)
        {
            ADTD,   2, 
            ADPS,   1, 
            ADPD,   1, 
            ADSO,   1, 
            ADSC,   1, 
            ADSR,   1, 
            ADIS,   1, 
            ADDS,   3
        }

        ADIS = One
        ADSR = Zero
        Stall (Arg1)
        ADSR = One
        ADIS = Zero
        Stall (Arg1)
    }

    Device (GPIO)
    {
        Name (_HID, "HYGO0030")  // _HID: Hardware ID
        Name (_CID, "HYGO0030")  // _CID: Compatible ID
        Name (_UID, Zero)  // _UID: Unique ID
        Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
        {
            Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, )
            {
                0x00000007,
            }
            Memory32Fixed (ReadWrite,
                0xFED81500,         // Address Base
                0x00000400,         // Address Length
                )
        })
        Method (_STA, 0, NotSerialized)  // _STA: Status
        {
            If ((OSYS >= 0x07DF))
            {
                Return (0x0F)
            }
            Else
            {
                Return (Zero)
            }
        }
    }

    Device (FUR0)
    {
        Name (_HID, "HYGO0020")  // _HID: Hardware ID
        Name (_UID, Zero)  // _UID: Unique ID
        Name (F0CR, ResourceTemplate ()
        {
            IRQ (Edge, ActiveHigh, Exclusive, )
                {3}
            Memory32Fixed (ReadWrite,
                0xFEDC9000,         // Address Base
                0x00001000,         // Address Length
                )
            Memory32Fixed (ReadWrite,
                0xFEDC7000,         // Address Base
                0x00001000,         // Address Length
                )
        })
        Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
        {
            CreateWordField (F0CR, One, IRQN)
            IRQN = (One << IUA0) /* \_SB_.IUA0 */
            Return (F0CR) /* \_SB_.FUR0.F0CR */
        }

        Method (_STA, 0, NotSerialized)  // _STA: Status
        {
            If (CKST (Zero))
            {
                If ((UT0E == One))
                {
                    Return (0x0F)
                }

                Return (Zero)
            }
            Else
            {
                Return (Zero)
            }
        }
    }

    Device (FUR1)
    {
        Name (_HID, "HYGO0020")  // _HID: Hardware ID
        Name (_UID, One)  // _UID: Unique ID
        Name (F1CR, ResourceTemplate ()
        {
            IRQ (Edge, ActiveHigh, Exclusive, )
                {4}
            Memory32Fixed (ReadWrite,
                0xFEDCA000,         // Address Base
                0x00001000,         // Address Length
                )
            Memory32Fixed (ReadWrite,
                0xFEDC8000,         // Address Base
                0x00001000,         // Address Length
                )
        })
        Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
        {
            CreateWordField (F1CR, One, IRQN)
            IRQN = (One << IUA1) /* \_SB_.IUA1 */
            Return (F1CR) /* \_SB_.FUR1.F1CR */
        }

        Method (_STA, 0, NotSerialized)  // _STA: Status
        {
            If (CKST (One))
            {
                If ((UT1E == One))
                {
                    Return (0x0F)
                }

                Return (Zero)
            }
            Else
            {
                Return (Zero)
            }
        }
    }

    Device (FUR2)
    {
        Name (_HID, "HYGO0020")  // _HID: Hardware ID
        Name (_UID, 0x02)  // _UID: Unique ID
        Name (F2CR, ResourceTemplate ()
        {
            IRQ (Edge, ActiveHigh, Exclusive, )
                {3}
            Memory32Fixed (ReadWrite,
                0xFEDCE000,         // Address Base
                0x00001000,         // Address Length
                )
            Memory32Fixed (ReadWrite,
                0xFEDCC000,         // Address Base
                0x00001000,         // Address Length
                )
        })
        Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
        {
            CreateWordField (F2CR, One, IRQN)
            IRQN = (One << IUA2) /* \_SB_.IUA2 */
            Return (F2CR) /* \_SB_.FUR2.F2CR */
        }

        Method (_STA, 0, NotSerialized)  // _STA: Status
        {
            If (CKST (0x02))
            {
                If ((UT2E == One))
                {
                    Return (0x0F)
                }

                Return (Zero)
            }
            Else
            {
                Return (Zero)
            }
        }
    }

    Device (FUR3)
    {
        Name (_HID, "HYGO0020")  // _HID: Hardware ID
        Name (_UID, 0x03)  // _UID: Unique ID
        Name (F3CR, ResourceTemplate ()
        {
            IRQ (Edge, ActiveHigh, Exclusive, )
                {4}
            Memory32Fixed (ReadWrite,
                0xFEDCF000,         // Address Base
                0x00001000,         // Address Length
                )
            Memory32Fixed (ReadWrite,
                0xFEDCD000,         // Address Base
                0x00001000,         // Address Length
                )
        })
        Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
        {
            CreateWordField (F3CR, One, IRQN)
            IRQN = (One << IUA3) /* \_SB_.IUA3 */
            Return (F3CR) /* \_SB_.FUR3.F3CR */
        }

        Method (_STA, 0, NotSerialized)  // _STA: Status
        {
            If (CKST (0x03))
            {
                If ((UT3E == One))
                {
                    Return (0x0F)
                }

                Return (Zero)
            }
            Else
            {
                Return (Zero)
            }
        }
    }

    Device (COM4)
    {
        Name (_HID, EisaId ("PNP0501") /* 16550A-compatible COM Serial Port */)  // _HID: Hardware ID
        Name (_DDN, "COM4")  // _DDN: DOS Device Name
        Name (_UID, 0x04)  // _UID: Unique ID
        Name (TIRQ, Zero)
        Name (C4CR, ResourceTemplate ()
        {
            IO (Decode16,
                0x02E8,             // Range Minimum
                0x02E8,             // Range Maximum
                0x01,               // Alignment
                0x08,               // Length
                )
            IRQNoFlags (_Y8B)
                {3}
        })
        Method (_STA, 0, NotSerialized)  // _STA: Status
        {
            If (IER0)
            {
                Return (0x0F)
            }
            Else
            {
                Return (Zero)
            }
        }

        Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
        {
            CreateWordField (C4CR, \_SB.COM4._Y8B._INT, IRQL)  // _INT: Interrupts
            IRQL = (One << FRUI (WUR0))
            Return (C4CR) /* \_SB_.COM4.C4CR */
        }
    }

    Device (COM2)
    {
        Name (_HID, EisaId ("PNP0501") /* 16550A-compatible COM Serial Port */)  // _HID: Hardware ID
        Name (_DDN, "COM2")  // _DDN: DOS Device Name
        Name (_UID, 0x02)  // _UID: Unique ID
        Name (TIRQ, Zero)
        Name (C2CR, ResourceTemplate ()
        {
            IO (Decode16,
                0x02F8,             // Range Minimum
                0x02F8,             // Range Maximum
                0x01,               // Alignment
                0x08,               // Length
                )
            IRQNoFlags (_Y8C)
                {3}
        })
        Method (_STA, 0, NotSerialized)  // _STA: Status
        {
            If (IER1)
            {
                Return (0x0F)
            }
            Else
            {
                Return (Zero)
            }
        }

        Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
        {
            CreateWordField (C2CR, \_SB.COM2._Y8C._INT, IRQL)  // _INT: Interrupts
            IRQL = (One << FRUI (WUR1))
            Return (C2CR) /* \_SB_.COM2.C2CR */
        }
    }

    Device (COM3)
    {
        Name (_HID, EisaId ("PNP0501") /* 16550A-compatible COM Serial Port */)  // _HID: Hardware ID
        Name (_DDN, "COM3")  // _DDN: DOS Device Name
        Name (_UID, 0x03)  // _UID: Unique ID
        Name (TIRQ, Zero)
        Name (C3CR, ResourceTemplate ()
        {
            IO (Decode16,
                0x03E8,             // Range Minimum
                0x03E8,             // Range Maximum
                0x01,               // Alignment
                0x08,               // Length
                )
            IRQNoFlags (_Y8D)
                {4}
        })
        Method (_STA, 0, NotSerialized)  // _STA: Status
        {
            If (IER2)
            {
                Return (0x0F)
            }
            Else
            {
                Return (Zero)
            }
        }

        Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
        {
            CreateWordField (C3CR, \_SB.COM3._Y8D._INT, IRQL)  // _INT: Interrupts
            IRQL = (One << FRUI (WUR2))
            Return (C3CR) /* \_SB_.COM3.C3CR */
        }
    }

    Device (COM1)
    {
        Name (_HID, EisaId ("PNP0501") /* 16550A-compatible COM Serial Port */)  // _HID: Hardware ID
        Name (_DDN, "COM1")  // _DDN: DOS Device Name
        Name (_UID, One)  // _UID: Unique ID
        Name (TIRQ, Zero)
        Name (C1CR, ResourceTemplate ()
        {
            IO (Decode16,
                0x03F8,             // Range Minimum
                0x03F8,             // Range Maximum
                0x01,               // Alignment
                0x08,               // Length
                )
            IRQNoFlags (_Y8E)
                {4}
        })
        Method (_STA, 0, NotSerialized)  // _STA: Status
        {
            If (IER3)
            {
                Return (0x0F)
            }
            Else
            {
                Return (Zero)
            }
        }

        Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
        {
            CreateWordField (C1CR, \_SB.COM1._Y8E._INT, IRQL)  // _INT: Interrupts
            IRQL = (One << FRUI (WUR3))
            Return (C1CR) /* \_SB_.COM1.C1CR */
        }
    }

    Device (I2CA)
    {
        Name (_HID, "HYGO0010")  // _HID: Hardware ID
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
            If ((OSYS >= 0x07DF))
            {
                If ((IC0E == One))
                {
                    Return (0x0F)
                }

                Return (Zero)
            }
            Else
            {
                Return (Zero)
            }
        }

        Method (RSET, 0, NotSerialized)
        {
            SRAD (0x05, 0xC8)
        }
    }

    Device (I2CB)
    {
        Name (_HID, "HYGO0010")  // _HID: Hardware ID
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
            If ((OSYS >= 0x07DF))
            {
                If ((IC1E == One))
                {
                    Return (0x0F)
                }

                Return (Zero)
            }
            Else
            {
                Return (Zero)
            }
        }

        Method (RSET, 0, NotSerialized)
        {
            SRAD (0x06, 0xC8)
        }
    }

    Device (I2CC)
    {
        Name (_HID, "HYGO0010")  // _HID: Hardware ID
        Name (_UID, 0x02)  // _UID: Unique ID
        Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
        {
            IRQ (Edge, ActiveHigh, Exclusive, )
                {4}
            Memory32Fixed (ReadWrite,
                0xFEDC4000,         // Address Base
                0x00001000,         // Address Length
                )
        })
        Method (_STA, 0, NotSerialized)  // _STA: Status
        {
            If ((OSYS >= 0x07DF))
            {
                If ((IC2E == One))
                {
                    Return (0x0F)
                }

                Return (Zero)
            }
            Else
            {
                Return (Zero)
            }
        }

        Method (RSET, 0, NotSerialized)
        {
            SRAD (0x07, 0xC8)
        }
    }

    Device (I2CD)
    {
        Name (_HID, "HYGO0010")  // _HID: Hardware ID
        Name (_UID, 0x03)  // _UID: Unique ID
        Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
        {
            IRQ (Edge, ActiveHigh, Exclusive, )
                {6}
            Memory32Fixed (ReadWrite,
                0xFEDC5000,         // Address Base
                0x00001000,         // Address Length
                )
        })
        Method (_STA, 0, NotSerialized)  // _STA: Status
        {
            If ((OSYS >= 0x07DF))
            {
                If ((IC3E == One))
                {
                    Return (0x0F)
                }

                Return (Zero)
            }
            Else
            {
                Return (Zero)
            }
        }

        Method (RSET, 0, NotSerialized)
        {
            SRAD (0x08, 0xC8)
        }
    }

    Device (I2CE)
    {
        Name (_HID, "HYGO0010")  // _HID: Hardware ID
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
            If ((OSYS >= 0x07DF))
            {
                If ((IC4E == One))
                {
                    Return (0x0F)
                }

                Return (Zero)
            }
            Else
            {
                Return (Zero)
            }
        }

        Method (RSET, 0, NotSerialized)
        {
            SRAD (0x09, 0xC8)
        }
    }

    Device (I2CF)
    {
        Name (_HID, "HYGO0010")  // _HID: Hardware ID
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
            If ((OSYS >= 0x07DF))
            {
                If ((IC5E == One))
                {
                    Return (0x0F)
                }

                Return (Zero)
            }
            Else
            {
                Return (Zero)
            }
        }

        Method (RSET, 0, NotSerialized)
        {
            SRAD (0x0A, 0xC8)
        }
    }
}