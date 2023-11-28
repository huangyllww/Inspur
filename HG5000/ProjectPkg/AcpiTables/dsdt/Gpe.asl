
/*
    Scope (_GPE)
    {
        Method (_L0B, 0, NotSerialized)  // _Lxx: Level-Triggered GPE
        {
            Notify (\_SB.PCI0.S0D0, 0x02)
            Notify (\_SB.S0D1.S0D1, 0x02)
            Notify (\_SB.S0D2.S0D2, 0x02)
            Notify (\_SB.S0D3.S0D3, 0x02)
            Notify (\_SB.S1D0.S1D0, 0x02)
            Notify (\_SB.S1D1.S1D1, 0x02)
            Notify (\_SB.S1D2.S1D2, 0x02)
            Notify (\_SB.S1D3.S1D3, 0x02)
            Notify (\_SB.PWRB, 0x02)
        }
        Method (_L09, 0, NotSerialized)  // _Lxx: Level-Triggered GPE
        {
            Notify (\_SB.S0D3.D3B2.LAN0, 0x02)
            Notify (\_SB.S0D3.D3B2.LAN1, 0x02)
        }
        Method (_L0A, 0, NotSerialized)  // _Lxx: Level-Triggered GPE
        {
            DOCK ()
            Store (Not (GE10), E10C)
        }

        Method (_INI, 0, NotSerialized)  // _INI: Initialize
        {
//-         \_SB.NVDN ()
            DOCK ()
        }

        Name (DOKB, Buffer (0x03) {})
        Method (DOCK, 0, NotSerialized)
        {
            CreateWordField (DOKB, Zero, BSZE)
            CreateByteField (DOKB, 0x02, DKST)
            Store (0x03, BSZE)
            Sleep (0x02)
            If (GE10)
            {
                Store (One, DKST)
                Store (0xF0D1, DBG8)
            }
            Else
            {
                Store (Zero, DKST)
                Store (0xF0D2, DBG8)
            }

//-         \_SB.ALIB (0x0A, DOKB)
        }        
    }
*/    
