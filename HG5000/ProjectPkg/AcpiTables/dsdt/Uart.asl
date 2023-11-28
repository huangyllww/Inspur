  Name(UT0E, 0x0F) //default 0xF = enabled
  Name(UT1E, 0x0F)
  Name(UT2E, 0x0)
  Name(UT3E, 0x0)
  
  OperationRegion(LUIE, SystemMemory, 0xFEDC0020, 0x4) //Legacy Uart Io Enable
  Field(LUIE, AnyAcc, NoLock, Preserve)
  {
    IER0, 1,   //2E8 0=decode disabled  1=decode enabled
    IER1, 1,   //2F8
    IER2, 1,   //3E8
    IER3, 1,   //3F8
    RESV, 4,   //Reserved
    WUR0, 2,   //0x2E8  0=Uart0, 1=Uart1, 2=Uart2, 3=Uart3
    WUR1, 2,   //0x2F8
    WUR2, 2,   //0x3E8
    WUR3, 2,   //0x3F8
  }

  OperationRegion(FPIC, SystemIo, 0xc00, 0x02) //Fch Pci Interrupt Connector
  Field(FPIC, AnyAcc, NoLock, Preserve)
  {
    FPII, 8,
    FPID, 8,
  }
  IndexField(FPII, FPID, ByteAcc, NoLock, Preserve) {
    Offset (0xF4),      //Interrupt for UART0
    IUA0, 8,
    Offset (0xF5),      //Interrupt for UART1
    IUA1, 8,
    Offset (0xF8),      //Interrupt for UART2
    IUA2, 8,
    Offset (0xF9),      //Interrupt for UART3
    IUA3, 8,
  }

  // The default IRQ of UART is defined in TSHwAcpiEnvService.c
  // The platform BIOS overrides the default settings as follows:
  //   FCH_TS_IRQ_UART0 = 0x3
  //   FCH_TS_IRQ_UART1 = 0x4
  //   FCH_TS_IRQ_UART2 = 0x5
  //   FCH_TS_IRQ_UART3 = 0xC
  Method(FRUI,1, Serialized)  //Fch Return UART Interrupt (APIC)
  {
    if (LEqual(Arg0, 0))     {Return (IUA0)}
    elseif (LEqual(Arg0, 1)) {Return (IUA1)}
    elseif (LEqual(Arg0, 2)) {Return (IUA2)}
    elseif (LEqual(Arg0, 3)) {Return (IUA3)}
    else {Return (3)}        //should not be here
  }

  Method(CKST,1, Serialized)  //Check if non-legacy UART is available
  {
    if (LAnd (LEqual (IER0, 1), LEqual (WUR0, Arg0)))     {Return (0)}
    elseif (LAnd (LEqual (IER1, 1), LEqual (WUR1, Arg0))) {Return (0)}
    elseif (LAnd (LEqual (IER2, 1), LEqual (WUR2, Arg0))) {Return (0)}
    elseif (LAnd (LEqual (IER3, 1), LEqual (WUR3, Arg0))) {Return (0)}
    else {Return (1)}
  }

  // UART0
  Device(FUR0) {
    Name(_HID,"HYGO0020")            // Hardware Device ID
    Name(_UID,0x0)
    Name(_CRS, ResourceTemplate() {
      IRQ(Edge, ActiveHigh, Exclusive) {4} //FCH_TS_IRQ_UART0, default: 4
      Memory32Fixed(ReadWrite, 0xFEDC9000, 0x1000)
      Memory32Fixed(ReadWrite, 0xFEDC7000, 0x1000)
    })
    Method (_STA, 0x0, NotSerialized) {
      if (CKST(0)) {                //0: UART 0
        Return (UT0E)
      } else {
        Return (0)
      }
    }
  } // End Device(FUR0)

  // UART1
  Device(FUR1) {
    Name(_HID,"HYGO0020")            // Hardware Device ID
    Name(_UID,0x1)
    Name(_CRS, ResourceTemplate() {
      IRQ(Edge, ActiveHigh, Exclusive) {3} //FCH_TS_IRQ_UART1, default: 3
      Memory32Fixed(ReadWrite, 0xFEDCA000, 0x1000)
      Memory32Fixed(ReadWrite, 0xFEDC8000, 0x1000)
    })
    Method (_STA, 0x0, NotSerialized) {
      if (CKST(1)) {                 //1: UART 1
        Return (UT1E)
      } else {
        Return (0)
      }
    }
  } // End Device(FUR1)

// UART2
  Device(FUR2) {
    Name(_HID,"HYGO0020")            // Hardware Device ID
    Name(_UID,0x2)
    Name(_CRS, ResourceTemplate() {
      IRQ(Edge, ActiveHigh, Exclusive) {4} //FCH_TS_IRQ_UART2, default: 4
      Memory32Fixed(ReadWrite, 0xFEDCE000, 0x1000)
      Memory32Fixed(ReadWrite, 0xFEDCC000, 0x1000)
    })
    Method (_STA, 0x0, NotSerialized) {
      if (CKST(2)) {                 //1: UART 2
        Return (UT2E)
      } else {
        Return (0)
      }
    }
  } // End Device(FUR1)
  
  // UART3
  Device(FUR3) {
 //   Name(_HID,"AMDI0020")            // Hardware Device ID
    Name(_HID,"HYGO0020")
    Name(_UID,0x3)
    Name(_CRS, ResourceTemplate() {
      IRQ(Edge, ActiveHigh, Exclusive) {3} //FCH_TS_IRQ_UART3, default: 3
      Memory32Fixed(ReadWrite, 0xFEDCF000, 0x1000)
      Memory32Fixed(ReadWrite, 0xFEDCD000, 0x1000)
    })
    Method (_STA, 0x0, NotSerialized) {
      if (CKST(3)) {                 //1: UART 3
        Return (UT3E)
      } else {
        Return (0)
      }
    }
  } // End Device(FUR3)
      
      
  Device(COM4) {  // COM Port
    Name(_HID, EISAID("PNP0501"))
    Name(_DDN, "COM4")
    Name(_UID, 0x4)
    Name(TIRQ, 0)

    //*****************************************************
    // Method _STA:  Return Status
    //*****************************************************
    Method (_STA, 0, NotSerialized) { // Return Status of the UART
      if (IER0) {Return (0x0F)}
      else {Return (0x00)}
    } // end of Method _STA

    //*****************************************************
    //  Method _CRS:  Return Current Resource Settings
    //*****************************************************
    Method (_CRS, 0, NotSerialized) {
      Name (BUF0, ResourceTemplate() {
        IO (Decode16, 0x2E8, 0x2E8, 0x01, 0x08)
        IRQNoFlags() {3}
      })

      CreateWordField (BUF0, 0x09, IRQL) // IRQ
      ShiftLeft(1, FRUI(WUR0), IRQL)

      Return(BUF0) // Return the result
    } // end _CRS Method
  } // end of Device COM4

  Device(COM2) {  // COM Port
    Name(_HID, EISAID("PNP0501"))
    Name(_DDN, "COM2")
    Name(_UID, 0x2)
    Name(TIRQ, 0)


    //*****************************************************
    // Method _STA:  Return Status
    //*****************************************************
    Method (_STA, 0, NotSerialized) { // Return Status of the UART
      if (IER1) {Return (0x0f)}
      else {Return (0x00)}
    } // end of Method _STA

    //*****************************************************
    //  Method _CRS:  Return Current Resource Settings
    //*****************************************************
    Method (_CRS, 0, NotSerialized) {
      Name (BUF0, ResourceTemplate() {
        IO (Decode16, 0x2F8, 0x2F8, 0x01, 0x08)
        IRQNoFlags() {3}
      })

      CreateWordField (BUF0, 0x09, IRQL) // IRQ
      ShiftLeft(1, FRUI(WUR1), IRQL)

      Return(BUF0) // Return the result
    } // end _CRS Method
  } // end of Device COM2

  Device(COM3) {  // COM Port
    Name(_HID, EISAID("PNP0501"))
    Name(_DDN, "COM3")
    Name(_UID, 0x3)
    Name(TIRQ, 0)

    //*****************************************************
    // Method _STA:  Return Status
    //*****************************************************
    Method (_STA, 0, NotSerialized) { // Return Status of the UART
      if (IER2) {Return (0x0f)}
      else {Return (0x00)}
    } // end of Method _STA

    //*****************************************************
    //  Method _CRS:  Return Current Resource Settings
    //*****************************************************
    Method (_CRS, 0, NotSerialized) {
      Name (BUF0, ResourceTemplate() {
        IO (Decode16, 0x3E8, 0x3E8, 0x01, 0x08)
        IRQNoFlags() {4}
      })

      CreateWordField (BUF0, 0x09, IRQL) // IRQ
      ShiftLeft(1, FRUI(WUR2), IRQL)

      Return(BUF0) // Return the result
    } // end _CRS Method
  } // end of Device COM3

  Device(COM1) {  // COM Port
    Name(_HID, EISAID("PNP0501"))
    Name(_DDN, "COM1")
    Name(_UID, 0x1)
    Name(TIRQ, 0)

    //*****************************************************
    // Method _STA:  Return Status
    //*****************************************************
    Method (_STA, 0, NotSerialized) { // Return Status of the UART
      if (IER3) {Return (0x0f)}
      else {Return (0x00)}
    } // end of Method _STA

    //*****************************************************
    //  Method _CRS:  Return Current Resource Settings
    //*****************************************************
    Method (_CRS, 0, NotSerialized) {
      Name (BUF0, ResourceTemplate() {
        IO (Decode16, 0x3F8, 0x3F8, 0x01, 0x08)
        IRQNoFlags() {4}
      })

      CreateWordField (BUF0, 0x09, IRQL) // IRQ
      ShiftLeft(1, FRUI(WUR3), IRQL)

      Return(BUF0) // Return the result
    } // end _CRS Method
  } // end of Device COM1

