
    Scope (_SB)
    {
        OperationRegion (SMID, SystemIO, 0xB2, One)
        Field (SMID, ByteAcc, NoLock, Preserve)
        {
            SMIC,   8
        }

        Method (CNDP, 3, Serialized)
        {
            ShiftLeft (One, ToInteger (Arg2), Local0)
            ShiftLeft (Local0, Multiply (ToInteger (Arg1), 0x02), Local0)
            ShiftLeft (Local0, Multiply (ToInteger (Arg0), 0x10), Local0)
            If (LEqual (And (Local0, NVBM), Zero))
            {
                Return (Zero)
            }
            Else
            {
                Return (One)
            }
        }

        Method (NVDN, 0, NotSerialized)
        {
            If (LNotEqual (CNDP (Zero, Zero, Zero), Zero))
            {
                Notify (^NVDR.NV00, 0x81)
            }

            If (LNotEqual (CNDP (Zero, Zero, One), Zero))
            {
                Notify (^NVDR.NV01, 0x81)
            }

            If (LNotEqual (CNDP (Zero, One, Zero), Zero))
            {
                Notify (^NVDR.NV02, 0x81)
            }

            If (LNotEqual (CNDP (Zero, One, One), Zero))
            {
                Notify (^NVDR.NV03, 0x81)
            }

            If (LNotEqual (CNDP (Zero, 0x02, Zero), Zero))
            {
                Notify (^NVDR.NV04, 0x81)
            }

            If (LNotEqual (CNDP (Zero, 0x02, One), Zero))
            {
                Notify (^NVDR.NV05, 0x81)
            }

            If (LNotEqual (CNDP (Zero, 0x03, Zero), Zero))
            {
                Notify (^NVDR.NV06, 0x81)
            }

            If (LNotEqual (CNDP (Zero, 0x03, One), Zero))
            {
                Notify (^NVDR.NV07, 0x81)
            }

            If (LNotEqual (CNDP (Zero, 0x04, Zero), Zero))
            {
                Notify (^NVDR.NV08, 0x81)
            }

            If (LNotEqual (CNDP (Zero, 0x04, One), Zero))
            {
                Notify (^NVDR.NV09, 0x81)
            }

            If (LNotEqual (CNDP (Zero, 0x05, Zero), Zero))
            {
                Notify (^NVDR.NV10, 0x81)
            }

            If (LNotEqual (CNDP (Zero, 0x05, One), Zero))
            {
                Notify (^NVDR.NV11, 0x81)
            }

            If (LNotEqual (CNDP (Zero, 0x06, Zero), Zero))
            {
                Notify (^NVDR.NV12, 0x81)
            }

            If (LNotEqual (CNDP (Zero, 0x06, One), Zero))
            {
                Notify (^NVDR.NV13, 0x81)
            }

            If (LNotEqual (CNDP (Zero, 0x07, Zero), Zero))
            {
                Notify (^NVDR.NV14, 0x81)
            }

            If (LNotEqual (CNDP (Zero, 0x07, One), Zero))
            {
                Notify (^NVDR.NV15, 0x81)
            }

            If (LNotEqual (CNDP (One, Zero, Zero), Zero))
            {
                Notify (^NVDR.NV16, 0x81)
            }

            If (LNotEqual (CNDP (One, Zero, One), Zero))
            {
                Notify (^NVDR.NV17, 0x81)
            }

            If (LNotEqual (CNDP (One, One, Zero), Zero))
            {
                Notify (^NVDR.NV18, 0x81)
            }

            If (LNotEqual (CNDP (One, One, One), Zero))
            {
                Notify (^NVDR.NV19, 0x81)
            }

            If (LNotEqual (CNDP (One, 0x02, Zero), Zero))
            {
                Notify (^NVDR.NV20, 0x81)
            }

            If (LNotEqual (CNDP (One, 0x02, One), Zero))
            {
                Notify (^NVDR.NV21, 0x81)
            }

            If (LNotEqual (CNDP (One, 0x03, Zero), Zero))
            {
                Notify (^NVDR.NV22, 0x81)
            }

            If (LNotEqual (CNDP (One, 0x03, One), Zero))
            {
                Notify (^NVDR.NV23, 0x81)
            }

            If (LNotEqual (CNDP (One, 0x04, Zero), Zero))
            {
                Notify (^NVDR.NV24, 0x81)
            }

            If (LNotEqual (CNDP (One, 0x04, One), Zero))
            {
                Notify (^NVDR.NV25, 0x81)
            }

            If (LNotEqual (CNDP (One, 0x05, Zero), Zero))
            {
                Notify (^NVDR.NV26, 0x81)
            }

            If (LNotEqual (CNDP (One, 0x05, One), Zero))
            {
                Notify (^NVDR.NV27, 0x81)
            }

            If (LNotEqual (CNDP (One, 0x06, Zero), Zero))
            {
                Notify (^NVDR.NV28, 0x81)
            }

            If (LNotEqual (CNDP (One, 0x06, One), Zero))
            {
                Notify (^NVDR.NV29, 0x81)
            }

            If (LNotEqual (CNDP (One, 0x07, Zero), Zero))
            {
                Notify (^NVDR.NV30, 0x81)
            }

            If (LNotEqual (CNDP (One, 0x07, One), Zero))
            {
                Notify (^NVDR.NV31, 0x81)
            }
        }

        Device (NVDR)
        {
            Name (_HID, "ACPI0012")  // _HID: Hardware ID
            Method (_STA, 0, NotSerialized)  // _STA: Status
            {
                If (LEqual (NVBM, Zero))
                {
                    Return (Zero)
                }
                Else
                {
                    Return (0x0F)
                }
            }

            Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
            {
                If (LEqual (Arg0, Buffer (0x10)
                        {
                            /* 0000 */   0xA4, 0xE7, 0x10, 0x2F, 0x91, 0x9E, 0xE4, 0x11,
                            /* 0008 */   0x89, 0xD3, 0x12, 0x3B, 0x93, 0xF7, 0x5C, 0xBA
                        }))
                {
                    If (LEqual (ToInteger (Arg2), Zero))
                    {
                        Return (Buffer (One)
                        {
                             0x01
                        })
                    }
                }
            }

            Device (NV00)
            {
                Name (_ADR, Zero)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV01)
            {
                Name (_ADR, One)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV02)
            {
                Name (_ADR, 0x10)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV03)
            {
                Name (_ADR, 0x11)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV04)
            {
                Name (_ADR, 0x20)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV05)
            {
                Name (_ADR, 0x21)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV06)
            {
                Name (_ADR, 0x30)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV07)
            {
                Name (_ADR, 0x31)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV08)
            {
                Name (_ADR, 0x40)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV09)
            {
                Name (_ADR, 0x41)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV10)
            {
                Name (_ADR, 0x50)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV11)
            {
                Name (_ADR, 0x51)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV12)
            {
                Name (_ADR, 0x60)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV13)
            {
                Name (_ADR, 0x61)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV14)
            {
                Name (_ADR, 0x70)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV15)
            {
                Name (_ADR, 0x71)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV16)
            {
                Name (_ADR, 0x1000)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV17)
            {
                Name (_ADR, 0x1001)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV18)
            {
                Name (_ADR, 0x1010)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV19)
            {
                Name (_ADR, 0x1011)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV20)
            {
                Name (_ADR, 0x1020)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV21)
            {
                Name (_ADR, 0x1021)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV22)
            {
                Name (_ADR, 0x1030)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV23)
            {
                Name (_ADR, 0x1031)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV24)
            {
                Name (_ADR, 0x1040)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV25)
            {
                Name (_ADR, 0x1041)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV26)
            {
                Name (_ADR, 0x1050)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV27)
            {
                Name (_ADR, 0x1051)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV28)
            {
                Name (_ADR, 0x1060)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV29)
            {
                Name (_ADR, 0x1061)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV30)
            {
                Name (_ADR, 0x1070)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }

            Device (NV31)
            {
                Name (_ADR, 0x1071)  // _ADR: Address
                Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
                {
                    Return (DSMF (Arg0, Arg2, Arg3, _ADR))
                }
            }
        }

        Method (DSMF, 4, Serialized)
        {
            Name (_T_0, Zero)  // _T_x: Emitted by ASL Compiler
            If (LEqual (Arg0, Buffer (0x10)
                    {
                        /* 0000 */   0x36, 0x8B, 0xE6, 0x1E, 0xBD, 0xD4, 0x1A, 0x4A,
                        /* 0008 */   0x9A, 0x16, 0x4F, 0x8E, 0x53, 0xD4, 0x6E, 0x05
                    }))
            {
                While (One)
                {
                    Store (ToInteger (Arg1), _T_0)
                    If (LEqual (_T_0, Zero))
                    {
                        Return (Buffer (0x04)
                        {
                             0xFF, 0xFF, 0xFF, 0xFF
                        })
                    }
                    Else
                    {
                        If (LNotEqual (Match (Package (0x12)
                                        {
                                            One, 
                                            0x02, 
                                            0x03, 
                                            0x04, 
                                            0x05, 
                                            0x07, 
                                            0x0A, 
                                            0x0B, 
                                            0x0C, 
                                            0x0D, 
                                            0x0E, 
                                            0x0F, 
                                            0x10, 
                                            0x12, 
                                            0x13, 
                                            0x14, 
                                            0x15, 
                                            0x18
                                        }, MEQ, _T_0, MTR, Zero, Zero), Ones))
                        {
                            If (LGreater (SizeOf (Index (Arg2, Zero)), Zero))
                            {
                                Return (Buffer (One)
                                {
                                     0x02
                                })
                            }

                            Store (Zero, Local0)
                        }
                        Else
                        {
                            If (LNotEqual (Match (Package (0x0D)
                                            {
                                                0x06, 
                                                0x08, 
                                                0x09, 
                                                0x11, 
                                                0x16, 
                                                0x17, 
                                                0x19, 
                                                0x1A, 
                                                0x1B, 
                                                0x1C, 
                                                0x1D, 
                                                0x1E, 
                                                0x1F
                                            }, MEQ, _T_0, MTR, Zero, Zero), Ones))
                            {
                                Store (SizeOf (Index (Arg2, Zero)), Local0)
                            }
                            Else
                            {
                                Return (Buffer (One)
                                {
                                     0x01
                                })
                            }
                        }
                    }

                    Break
                }

                Store (ToInteger (Arg1), FIDX)
                Store (ToInteger (Arg3), NDEV)
                If (LGreater (Local0, Zero))
                {
                    Store (Local0, ISIZ)
                    CreateField (DerefOf (Index (Arg2, Zero)), Zero, Multiply (ISIZ, 0x08
                        ), IBUF)
                    Mid (IBUF, Zero, ISIZ, IDAT)
                }

                Store (0xEB, SMIC)
                Name (RBUF, Buffer (OSIZ) {})
                If (LGreater (OSIZ, Zero))
                {
                    CreateField (RBUF, Zero, Multiply (OSIZ, 0x08), OBUF)
                    Mid (ODAT, Zero, OSIZ, OBUF)
                }

                Return (RBUF)
            }

            Return (Buffer (One)
            {
                 0x02
            })
        }
    }
    
#if 0
    OperationRegion (DASD, SystemMemory, 0x7A7E8000, 0x0212)
    Field (DASD, ByteAcc, NoLock, Preserve)
    {
        FIDX,   32, 
        ISIZ,   32, 
        IDAT,   2048, 
        OSIZ,   32, 
        ODAT,   2048, 
        NDEV,   16, 
        NVBM,   32
    }    
#endif


    