

OperationRegion (PIRQ, SystemIO, 0x0C00, 0x02)
Field (PIRQ, ByteAcc, NoLock, Preserve)
{
    PIDX,   8, 
    PDAT,   8
}

IndexField (PIDX, PDAT, ByteAcc, NoLock, Preserve)
{
    PIRA,   8, 
    PIRB,   8, 
    PIRC,   8, 
    PIRD,   8, 
    PIRE,   8, 
    PIRF,   8, 
    PIRG,   8, 
    PIRH,   8, 
    Offset (0x0C), 
    SIRA,   8, 
    SIRB,   8, 
    SIRC,   8, 
    SIRD,   8, 
    PIRS,   8
}

Method (DSPI, 0, NotSerialized)
{
    PIRA = 0x1F
    PIRB = 0x1F
    PIRC = 0x1F
    PIRD = 0x1F
    PIRE = 0x1F
    PIRF = 0x1F
    PIRG = 0x1F
    PIRH = 0x1F
}

Name (PRS1, ResourceTemplate ()
{
    IRQ (Level, ActiveLow, Shared, )
        {10,11,14,15}
})
Name (BUFA, ResourceTemplate ()
{
    IRQ (Level, ActiveLow, Shared, ) {15}
})

Device (LNKA)
{
    Name (_HID, EisaId ("PNP0C0F") /* PCI Interrupt Link Device */)  // _HID: Hardware ID
    Name (_UID, One)  // _UID: Unique ID
    Method (_STA, 0, NotSerialized)  // _STA: Status
    {
        If ((PIRA && (PIRA != 0x1F)))
        {
            Return (0x0B)
        }
        Else
        {
            Return (0x09)
        }
    }

    Method (_PRS, 0, NotSerialized)  // _PRS: Possible Resource Settings
    {
        Return (PRS1) /* \_SB_.PRS1 */
    }

    Method (_DIS, 0, NotSerialized)  // _DIS: Disable Device
    {
        PIRA = 0x1F
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
        CreateWordField (BUFA, One, IRQX)
        IRQX = (One << PIRA) /* \_SB_.PIRA */
        Return (BUFA) /* \_SB_.BUFA */
    }

    Method (_SRS, 1, NotSerialized)  // _SRS: Set Resource Settings
    {
        CreateWordField (Arg0, One, IRA)
        FindSetRightBit (IRA, Local0)
        Local0--
        PIRA = Local0
    }
}

Device (LNKB)
{
    Name (_HID, EisaId ("PNP0C0F") /* PCI Interrupt Link Device */)  // _HID: Hardware ID
    Name (_UID, 0x02)  // _UID: Unique ID
    Method (_STA, 0, NotSerialized)  // _STA: Status
    {
        If ((PIRB && (PIRB != 0x1F)))
        {
            Return (0x0B)
        }
        Else
        {
            Return (0x09)
        }
    }

    Method (_PRS, 0, NotSerialized)  // _PRS: Possible Resource Settings
    {
        Return (PRS1) /* \_SB_.PRS1 */
    }

    Method (_DIS, 0, NotSerialized)  // _DIS: Disable Device
    {
        PIRB = 0x1F
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
        CreateWordField (BUFA, One, IRQX)
        IRQX = (One << PIRB) /* \_SB_.PIRB */
        Return (BUFA) /* \_SB_.BUFA */
    }

    Method (_SRS, 1, NotSerialized)  // _SRS: Set Resource Settings
    {
        CreateWordField (Arg0, One, IRA)
        FindSetRightBit (IRA, Local0)
        Local0--
        PIRB = Local0
    }
}

Device (LNKC)
{
    Name (_HID, EisaId ("PNP0C0F") /* PCI Interrupt Link Device */)  // _HID: Hardware ID
    Name (_UID, 0x03)  // _UID: Unique ID
    Method (_STA, 0, NotSerialized)  // _STA: Status
    {
        If ((PIRC && (PIRC != 0x1F)))
        {
            Return (0x0B)
        }
        Else
        {
            Return (0x09)
        }
    }

    Method (_PRS, 0, NotSerialized)  // _PRS: Possible Resource Settings
    {
        Return (PRS1) /* \_SB_.PRS1 */
    }

    Method (_DIS, 0, NotSerialized)  // _DIS: Disable Device
    {
        PIRC = 0x1F
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
        CreateWordField (BUFA, One, IRQX)
        IRQX = (One << PIRC) /* \_SB_.PIRC */
        Return (BUFA) /* \_SB_.BUFA */
    }

    Method (_SRS, 1, NotSerialized)  // _SRS: Set Resource Settings
    {
        CreateWordField (Arg0, One, IRA)
        FindSetRightBit (IRA, Local0)
        Local0--
        PIRC = Local0
    }
}

Device (LNKD)
{
    Name (_HID, EisaId ("PNP0C0F") /* PCI Interrupt Link Device */)  // _HID: Hardware ID
    Name (_UID, 0x04)  // _UID: Unique ID
    Method (_STA, 0, NotSerialized)  // _STA: Status
    {
        If ((PIRD && (PIRD != 0x1F)))
        {
            Return (0x0B)
        }
        Else
        {
            Return (0x09)
        }
    }

    Method (_PRS, 0, NotSerialized)  // _PRS: Possible Resource Settings
    {
        Return (PRS1) /* \_SB_.PRS1 */
    }

    Method (_DIS, 0, NotSerialized)  // _DIS: Disable Device
    {
        PIRD = 0x1F
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
        CreateWordField (BUFA, One, IRQX)
        IRQX = (One << PIRD) /* \_SB_.PIRD */
        Return (BUFA) /* \_SB_.BUFA */
    }

    Method (_SRS, 1, NotSerialized)  // _SRS: Set Resource Settings
    {
        CreateWordField (Arg0, One, IRA)
        FindSetRightBit (IRA, Local0)
        Local0--
        PIRD = Local0
    }
}

Device (LNKE)
{
    Name (_HID, EisaId ("PNP0C0F") /* PCI Interrupt Link Device */)  // _HID: Hardware ID
    Name (_UID, 0x05)  // _UID: Unique ID
    Method (_STA, 0, NotSerialized)  // _STA: Status
    {
        If ((PIRE && (PIRE != 0x1F)))
        {
            Return (0x0B)
        }
        Else
        {
            Return (0x09)
        }
    }

    Method (_PRS, 0, NotSerialized)  // _PRS: Possible Resource Settings
    {
        Return (PRS1) /* \_SB_.PRS1 */
    }

    Method (_DIS, 0, NotSerialized)  // _DIS: Disable Device
    {
        PIRE = 0x1F
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
        CreateWordField (BUFA, One, IRQX)
        IRQX = (One << PIRE) /* \_SB_.PIRE */
        Return (BUFA) /* \_SB_.BUFA */
    }

    Method (_SRS, 1, NotSerialized)  // _SRS: Set Resource Settings
    {
        CreateWordField (Arg0, One, IRA)
        FindSetRightBit (IRA, Local0)
        Local0--
        PIRE = Local0
    }
}

Device (LNKF)
{
    Name (_HID, EisaId ("PNP0C0F") /* PCI Interrupt Link Device */)  // _HID: Hardware ID
    Name (_UID, 0x06)  // _UID: Unique ID
    Method (_STA, 0, NotSerialized)  // _STA: Status
    {
        If ((PIRF && (PIRF != 0x1F)))
        {
            Return (0x0B)
        }
        Else
        {
            Return (0x09)
        }
    }

    Method (_PRS, 0, NotSerialized)  // _PRS: Possible Resource Settings
    {
        Return (PRS1) /* \_SB_.PRS1 */
    }

    Method (_DIS, 0, NotSerialized)  // _DIS: Disable Device
    {
        PIRF = 0x1F
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
        CreateWordField (BUFA, One, IRQX)
        IRQX = (One << PIRF) /* \_SB_.PIRF */
        Return (BUFA) /* \_SB_.BUFA */
    }

    Method (_SRS, 1, NotSerialized)  // _SRS: Set Resource Settings
    {
        CreateWordField (Arg0, One, IRA)
        FindSetRightBit (IRA, Local0)
        Local0--
        PIRF = Local0
    }
}

Device (LNKG)
{
    Name (_HID, EisaId ("PNP0C0F") /* PCI Interrupt Link Device */)  // _HID: Hardware ID
    Name (_UID, 0x07)  // _UID: Unique ID
    Method (_STA, 0, NotSerialized)  // _STA: Status
    {
        If ((PIRG && (PIRG != 0x1F)))
        {
            Return (0x0B)
        }
        Else
        {
            Return (0x09)
        }
    }

    Method (_PRS, 0, NotSerialized)  // _PRS: Possible Resource Settings
    {
        Return (PRS1) /* \_SB_.PRS1 */
    }

    Method (_DIS, 0, NotSerialized)  // _DIS: Disable Device
    {
        PIRG = 0x1F
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
        CreateWordField (BUFA, One, IRQX)
        IRQX = (One << PIRG) /* \_SB_.PIRG */
        Return (BUFA) /* \_SB_.BUFA */
    }

    Method (_SRS, 1, NotSerialized)  // _SRS: Set Resource Settings
    {
        CreateWordField (Arg0, One, IRA)
        FindSetRightBit (IRA, Local0)
        Local0--
        PIRG = Local0
    }
}

Device (LNKH)
{
    Name (_HID, EisaId ("PNP0C0F") /* PCI Interrupt Link Device */)  // _HID: Hardware ID
    Name (_UID, 0x08)  // _UID: Unique ID
    Method (_STA, 0, NotSerialized)  // _STA: Status
    {
        If ((PIRH && (PIRH != 0x1F)))
        {
            Return (0x0B)
        }
        Else
        {
            Return (0x09)
        }
    }

    Method (_PRS, 0, NotSerialized)  // _PRS: Possible Resource Settings
    {
        Return (PRS1) /* \_SB_.PRS1 */
    }

    Method (_DIS, 0, NotSerialized)  // _DIS: Disable Device
    {
        PIRH = 0x1F
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
        CreateWordField (BUFA, One, IRQX)
        IRQX = (One << PIRH) /* \_SB_.PIRH */
        Return (BUFA) /* \_SB_.BUFA */
    }

    Method (_SRS, 1, NotSerialized)  // _SRS: Set Resource Settings
    {
        CreateWordField (Arg0, One, IRA)
        FindSetRightBit (IRA, Local0)
        Local0--
        PIRH = Local0
    }
}