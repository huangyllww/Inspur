#include "UefiSmi.h"
#include "smiMode.h"
#include "defs.h"

/* Perform a rounded division. 
 * For example, if the result is 4.5, this function returns 5.
 * If the result is 4.4, this function returns 4.
 */
UINT32 roundedDiv(UINT32 num, UINT32 denom)
{
    /* n / d + 1 / 2 = (2n + d) / 2d */
    return (2 * num + denom) / (2 * denom);
}

/* Absolute differece between two numbers */
UINT32 absDiff(UINT32 a, UINT32 b)
{
    if ( a >= b )
        return(a - b);
    else
        return(b - a);
}

/* This function calculates 2 to the power of x 
   Input is the power number.
 */
UINT32 twoToPowerOfx(UINT32 x)
{
    UINT32 i;
    UINT32 result = 1;

    for (i=1; i<=x; i++)
        result *= 2;

    return result;
}

/*
 * This function gets the power mode, one of three modes: 0, 1 or Sleep.
 * On hardware reset, power mode 0 is default.
 */
UINT32 getPowerMode(
SMI_PRIVATE_DATA  *Private
)
{
    return (FIELD_GET(SMI_READ_MMIO(Private, POWER_MODE_CTRL), POWER_MODE_CTRL, MODE));
}

/*
 * SM50x can operate in one of three modes: 0, 1 or Sleep.
 * On hardware reset, power mode 0 is default.
 */
void setPowerMode(SMI_PRIVATE_DATA  *Private, UINT32 powerMode)
{
    UINT32 control_value = 0;

    control_value = SMI_READ_MMIO(Private, POWER_MODE_CTRL);

    switch (powerMode)
    {
        case POWER_MODE_CTRL_MODE_MODE0:
            control_value = FIELD_SET(control_value, POWER_MODE_CTRL, MODE, MODE0);
            break;

        case POWER_MODE_CTRL_MODE_MODE1:
            control_value = FIELD_SET(control_value, POWER_MODE_CTRL, MODE, MODE1);
            break;

        case POWER_MODE_CTRL_MODE_SLEEP:
            control_value = FIELD_SET(control_value, POWER_MODE_CTRL, MODE, SLEEP);
            break;

        default:
            break;
    }

    /* Set up other fields in Power Control Register */
    if (powerMode == POWER_MODE_CTRL_MODE_SLEEP)
        control_value = FIELD_SET(control_value, POWER_MODE_CTRL, OSC_INPUT, OFF);
    else
        control_value = FIELD_SET(control_value, POWER_MODE_CTRL, OSC_INPUT, ON);
    
    /* Program new power mode. */
    SMI_WRITE_MMIO(Private, POWER_MODE_CTRL, control_value);
}

void setCurrentGate(SMI_PRIVATE_DATA  *Private,UINT32 gate)
{
	//SMIDEBUG("setCurrentGate +\n");
    //UINT32 gate_reg;
    UINT32 gate_reg;
	
    //UINT32 mode;
    UINT32 mode;

    /* Get current power mode. */
    mode = getPowerMode(Private);

    switch (mode)
    {
        case POWER_MODE_CTRL_MODE_MODE0:
            gate_reg = MODE0_GATE;
            break;

        case POWER_MODE_CTRL_MODE_MODE1:
            gate_reg = MODE1_GATE;
            break;

        default:
            gate_reg = MODE0_GATE;
            break;
    }
    SMI_WRITE_MMIO(Private, gate_reg, gate);
	//SMIDEBUG("setCurrentGate -\n");
}
/*
 * Set up the corresponding bit field of the programmable PLL register.
 *
 * Input: Pointer to PLL structure with type and values set up properly.
 *        Usually, calcPllValue() function will be called before this to calculate the values first.
 *
 */
UINT32 formatPllReg(pll_value_t *pPLL)
{
    UINT32 ulPllReg = 0;

    /* Note that all PLL's have the same format. Here, we just use Panel PLL parameter
       to work out the bit fields in the register.
       On returning a 32 bit number, the value can be applied to any PLL in the calling function.
    */
    ulPllReg =
        FIELD_SET(  0, PRIMARY_PLL_CTRL, BYPASS, OFF)
      | FIELD_SET(  0, PRIMARY_PLL_CTRL, POWER,  ON)
      | FIELD_SET(  0, PRIMARY_PLL_CTRL, INPUT,  OSC)
      | FIELD_VALUE(0, PRIMARY_PLL_CTRL, POD,    pPLL->POD)      
      | FIELD_VALUE(0, PRIMARY_PLL_CTRL, OD,     pPLL->OD)
      | FIELD_VALUE(0, PRIMARY_PLL_CTRL, N,      pPLL->N)
      | FIELD_VALUE(0, PRIMARY_PLL_CTRL, M,      pPLL->M);


    return(ulPllReg);
}

/* 
 * This function gets the available clock type
 *
 */
clock_type_t getClockType()
{
//    clock_type_t clockType;

	return SECONDARY_PLL;
}

/*
 * A local function to calculate the clock value of the given PLL.
 *
 * Input:
 *      Pointer to a PLL structure to be calculated based on the
 *      following formula:
 *      inputFreq * M / N / (2 to the power of OD) / (2 to the power of POD)
 */
UINT32 calcPLL(pll_value_t *pPLL)
{
    return (pPLL->inputFreq * pPLL->M / pPLL->N / twoToPowerOfx(pPLL->OD) / twoToPowerOfx(pPLL->POD));
}

/*
 * Given a requested clock frequency, this function calculates the 
 * best M, N & OD values for the PLL.
 * 
 * Input: Requested pixel clock in Hz unit.
 *        The followiing fields of PLL has to be set up properly:
 *        pPLL->clockType, pPLL->inputFreq.
 *
 * Output: Update the PLL structure with the proper M, N and OD values
 * Return: The actual clock in Hz that the PLL is able to set up.
 *
 * The PLL uses this formula to operate: 
 * requestClk = inputFreq * M / N / (2 to the power of OD) / (2 to the power of POD)
 *
 * The PLL specification mention the following restrictions:
 *      1 MHz <= inputFrequency / N <= 25 MHz
 *      200 MHz <= outputFrequency <= 1000 MHz --> However, it seems that it can 
 *                                                 be set to lower than 200 MHz.
 */
UINT32 calcPllValue(
UINT32 ulRequestClk, /* Required pixel clock in Hz unit */
pll_value_t *pPLL           /* Structure to hold the value to be set in PLL */
)
{
    /* SM750NS don't have prgrammable PLL and M/N values to work on.
       Just return the requested clock. */
    return ulRequestClk;
}

void setVclock_inspur(
	SMI_PRIVATE_DATA  *Private,
	UINT32 pll
)
{
	UINT32 tmp0, tmp1;
	
//1. outer_bypass_n=0
	tmp0 = (UINT32)SMI_READ_MMIO(Private, CRT_PLL1_750NS);
	tmp0 &= 0xBFFFFFFF;
       SMI_WRITE_MMIO(Private, CRT_PLL1_750NS, tmp0);

//2. pll_pd=1?inter_bypass=1
       SMI_WRITE_MMIO(Private, CRT_PLL1_750NS, 0x21000000);

// 3. config pll 
	SMI_WRITE_MMIO(Private, CRT_PLL1_750NS, pll);

// 4. delay 5 us
	gBS->Stall(5);		/* delay for 5 Microseconds */
	
// 5. pll_pd =0
	 tmp1 = pll & ~0x01000000;
        SMI_WRITE_MMIO(Private, CRT_PLL1_750NS, tmp1);
		
// 6. delay 100 us
	gBS->Stall(100);		/* delay for 100 Microseconds */
	
// 7. inter_bypass=0
	 tmp1 &= ~0x20000000;
        SMI_WRITE_MMIO(Private, CRT_PLL1_750NS, tmp1);
		
// 8. delay 1 us
	gBS->Stall(1);		/* delay for 1 Microsecond */
	
// 9. outer_bypass_n=1
	 tmp1 |= 0x40000000;
        SMI_WRITE_MMIO(Private, CRT_PLL1_750NS, tmp1);
}

/*
	SM750NS SMI FPGA:
    This function takes care the extra registers and bit fields required to set
    up a mode in SM750NS FPGA board.

	Explanation about Display Control register:
    FPGA only supports 7 predefined pixel clocks, and clock select is 
    in bit 4:0 of new register 0x802a8.
*/
UINT32 displayControlAdjust_SM750NS(
	SMI_PRIVATE_DATA  *Private,
	mode_parameter_t *pModeParam, 
	UINT32 ulTmpValue
)
{
	//SMIDEBUG("displayControlAdjust_SM750NS +\n");
	UINT32 x, y;
	UINT32 pll1; /* bit[31:0] of PLL */
	UINT32 pll2; /* bit[63:32] of PLL */

	x = pModeParam->horizontal_display_end;
	y = pModeParam->vertical_display_end;

	/* SM750NS has to set up a new register for PLL control (CRT_PLL1_750NS & CRT_PLL2_750NS).
	   The values to set are different for ASIC and FPGA verification.
	*/
	if ( x == 800 && y == 600 )
		{ pll1 = CRT_PLL1_750NS_40MHZ; pll2 = CRT_PLL2_750NS_40MHZ;}
	else if (x == 1024 && y == 768)
		{ pll1 = CRT_PLL1_750NS_65MHZ; pll2 = CRT_PLL2_750NS_65MHZ;}
	else if (x == 1280 && y == 1024)
		{ pll1 = CRT_PLL1_750NS_108MHZ; pll2 = CRT_PLL2_750NS_108MHZ;}
	else /* default to VGA clock */
		{ pll1 = CRT_PLL1_750NS_25MHZ; pll2 = CRT_PLL2_750NS_25MHZ;}

    SMI_WRITE_MMIO(Private, CRT_PLL1_750NS, pll1);
    SMI_WRITE_MMIO(Private, CRT_PLL2_750NS, pll2);

    /* SM750LE and NS has to set up the top-left and bottom-right
       registers as well.
       Note that normal SM750/SM718 only use those two register for
       auto-centering mode.
    */
    SMI_WRITE_MMIO(Private, SECONDARY_AUTO_CENTERING_TL,
      FIELD_VALUE(0, SECONDARY_AUTO_CENTERING_TL, TOP, 0)
    | FIELD_VALUE(0, SECONDARY_AUTO_CENTERING_TL, LEFT, 0));

    SMI_WRITE_MMIO(Private, SECONDARY_AUTO_CENTERING_BR, 
      FIELD_VALUE(0, SECONDARY_AUTO_CENTERING_BR, BOTTOM, y-1)
    | FIELD_VALUE(0, SECONDARY_AUTO_CENTERING_BR, RIGHT, x-1));

    /* Assume common fields in dispControl have been properly set before
       calling this function.
       This function only sets the extra fields in dispControl.
    */

	/* Set bit 25 of display controller: Select CRT or VGA clock */
    ulTmpValue = FIELD_SET(ulTmpValue, SECONDARY_DISPLAY_CTRL, CRTSELECT, CRT);

    /* Set bit 14 of display controller */
    ulTmpValue &= FIELD_CLEAR(SECONDARY_DISPLAY_CTRL, CLOCK_PHASE);
    if (pModeParam->clock_phase_polarity == NEG)
        ulTmpValue = FIELD_SET(ulTmpValue, SECONDARY_DISPLAY_CTRL, CLOCK_PHASE, ACTIVE_LOW);
    else
        ulTmpValue = FIELD_SET(ulTmpValue, SECONDARY_DISPLAY_CTRL, CLOCK_PHASE, ACTIVE_HIGH);

    SMI_WRITE_MMIO(Private, SECONDARY_DISPLAY_CTRL, ulTmpValue);

	//SMIDEBUG("displayControlAdjust_SM750NS -\n");
	return ulTmpValue;
}

/*
	SM750NS  FPGA:
    This function takes care the extra registers and bit fields required to set
    up a mode in SM750NS FPGA board.

	Explanation about Display Control register:
    FPGA only supports 7 predefined pixel clocks, and clock select is 
    in bit 4:0 of new register 0x802a8.
*/
UINT32 displayControlAdjust_SM750NS_F(
	SMI_PRIVATE_DATA  *Private,
	mode_parameter_t *pModeParam, 
	UINT32 ulTmpValue
)
{
	//SMIDEBUG("displayControlAdjust_SM750NS_F +\n");
	UINT32 x, y;
	UINT32 pll1; /* bit[31:0] of PLL */
	UINT32 pll2; /* bit[63:32] of PLL */

	x = pModeParam->horizontal_display_end;
	y = pModeParam->vertical_display_end;

	/* SM750NS_F has to set up a new register for PLL control (CRT_PLL1_750NS & CRT_PLL2_750NS).
	   The values to set are different for ASIC and FPGA verification.
	*/
	if ( x == 800 && y == 600 )
		{ pll1 = CRT_PLL1_750NS_F_40MHZ; pll2 = CRT_PLL2_750NS_F_40MHZ;}
	else if (x == 1024 && y == 768)
		{ pll1 = CRT_PLL1_750NS_F_65MHZ; pll2 = CRT_PLL2_750NS_F_65MHZ;}
	else if (x == 1280 && y == 1024)
		{ pll1 = CRT_PLL1_750NS_F_108MHZ; pll2 = CRT_PLL2_750NS_F_108MHZ;}
	else /* default to VGA clock */
		{ pll1 = CRT_PLL1_750NS_F_25MHZ; pll2 = CRT_PLL2_750NS_F_25MHZ;}

    SMI_WRITE_MMIO(Private, CRT_PLL2_750NS, pll2);
    //setVclock_inspur(Private, pll1);
    SMI_WRITE_MMIO(Private, CRT_PLL1_750NS, pll1);

    /* SM750LE and NS has to set up the top-left and bottom-right
       registers as well.
       Note that normal SM750/SM718 only use those two register for
       auto-centering mode.
    */
    SMI_WRITE_MMIO(Private, SECONDARY_AUTO_CENTERING_TL,
      FIELD_VALUE(0, SECONDARY_AUTO_CENTERING_TL, TOP, 0)
    | FIELD_VALUE(0, SECONDARY_AUTO_CENTERING_TL, LEFT, 0));

    SMI_WRITE_MMIO(Private, SECONDARY_AUTO_CENTERING_BR, 
      FIELD_VALUE(0, SECONDARY_AUTO_CENTERING_BR, BOTTOM, y-1)
    | FIELD_VALUE(0, SECONDARY_AUTO_CENTERING_BR, RIGHT, x-1));

    /* Assume common fields in dispControl have been properly set before
       calling this function.
       This function only sets the extra fields in dispControl.
    */

	/* Set bit 25 of display controller: Select CRT or VGA clock */
    ulTmpValue = FIELD_SET(ulTmpValue, SECONDARY_DISPLAY_CTRL, CRTSELECT, CRT);

    /* Set bit 14 of display controller */
    ulTmpValue &= FIELD_CLEAR(SECONDARY_DISPLAY_CTRL, CLOCK_PHASE);
    if (pModeParam->clock_phase_polarity == NEG)
        ulTmpValue = FIELD_SET(ulTmpValue, SECONDARY_DISPLAY_CTRL, CLOCK_PHASE, ACTIVE_LOW);
    else
        ulTmpValue = FIELD_SET(ulTmpValue, SECONDARY_DISPLAY_CTRL, CLOCK_PHASE, ACTIVE_HIGH);

    SMI_WRITE_MMIO(Private, SECONDARY_DISPLAY_CTRL, ulTmpValue);

	//SMIDEBUG("displayControlAdjust_SM750NS_F -\n");
	return ulTmpValue;
}

/* 
 * Program the hardware for a specific video mode
 */
void programModeRegisters(
		SMI_PRIVATE_DATA  *Private,
		mode_parameter_t *pModeParam,   /* mode information about pixel clock, horizontal total, etc. */
		UINT32 ulBpp,            /* Color depth for this mode */
		UINT32 ulBaseAddress,    /* Offset in frame buffer */
		UINT32 ulPitch,          /* Mode pitch value in byte: no of bytes between two lines. */
		pll_value_t *pPLL               /* Pre-calculated values for the PLL */
		)
{
    UINT32 ulTmpValue, ulReg, ulReservedBits;
    UINT32 palette_ram;
    //UINT32 offset;

	//SMIDEBUG("programModeRegisters +\n");

	setPowerMode(Private, POWER_MODE_CTRL_MODE_MODE0);

	/* Enable display power gate */
    ulTmpValue = SMI_READ_MMIO(Private, CURRENT_GATE);
    ulTmpValue = FIELD_SET(ulTmpValue, CURRENT_GATE, LOCALMEM, ON);
    ulTmpValue = FIELD_SET(ulTmpValue, CURRENT_GATE, DISPLAY, ON);
    setCurrentGate(Private, ulTmpValue);

    if (pPLL->clockType == SECONDARY_PLL)
    {
        /* Secondary Display Control: SECONDARY_PLL */
        SMI_WRITE_MMIO(Private, SECONDARY_PLL_CTRL, formatPllReg(pPLL)); 

        /* Frame buffer base for this mode */
        SMI_WRITE_MMIO(Private, SECONDARY_FB_ADDRESS,
              FIELD_SET(0, SECONDARY_FB_ADDRESS, STATUS, PENDING)
            | FIELD_SET(0, SECONDARY_FB_ADDRESS, EXT, LOCAL)
            | FIELD_VALUE(0, SECONDARY_FB_ADDRESS, ADDRESS, ulBaseAddress));

        /* Pitch value (Sometime, hardware people calls it Offset) */
        SMI_WRITE_MMIO(Private, SECONDARY_FB_WIDTH,
              FIELD_VALUE(0, SECONDARY_FB_WIDTH, WIDTH, ulPitch)
            | FIELD_VALUE(0, SECONDARY_FB_WIDTH, OFFSET, ulPitch));

        SMI_WRITE_MMIO(Private, SECONDARY_HORIZONTAL_TOTAL,
              FIELD_VALUE(0, SECONDARY_HORIZONTAL_TOTAL, TOTAL, pModeParam->horizontal_total - 1)
            | FIELD_VALUE(0, SECONDARY_HORIZONTAL_TOTAL, DISPLAY_END, pModeParam->horizontal_display_end - 1));

        SMI_WRITE_MMIO(Private, SECONDARY_HORIZONTAL_SYNC,
              FIELD_VALUE(0, SECONDARY_HORIZONTAL_SYNC, WIDTH, pModeParam->horizontal_sync_width)
            | FIELD_VALUE(0, SECONDARY_HORIZONTAL_SYNC, START, pModeParam->horizontal_sync_start - 1));

        SMI_WRITE_MMIO(Private, SECONDARY_VERTICAL_TOTAL,
              FIELD_VALUE(0, SECONDARY_VERTICAL_TOTAL, TOTAL, pModeParam->vertical_total - 1)
            | FIELD_VALUE(0, SECONDARY_VERTICAL_TOTAL, DISPLAY_END, pModeParam->vertical_display_end - 1));

        SMI_WRITE_MMIO(Private, SECONDARY_VERTICAL_SYNC,
              FIELD_VALUE(0, SECONDARY_VERTICAL_SYNC, HEIGHT, pModeParam->vertical_sync_height)
            | FIELD_VALUE(0, SECONDARY_VERTICAL_SYNC, START, pModeParam->vertical_sync_start - 1));

        /* Set control register value */
        ulTmpValue = SMI_READ_MMIO(Private, SECONDARY_DISPLAY_CTRL) 
          |
            (pModeParam->vertical_sync_polarity == POS
            ? FIELD_SET(0, SECONDARY_DISPLAY_CTRL, VSYNC_PHASE, ACTIVE_HIGH)
            : FIELD_SET(0, SECONDARY_DISPLAY_CTRL, VSYNC_PHASE, ACTIVE_LOW))
          | (pModeParam->horizontal_sync_polarity == POS
            ? FIELD_SET(0, SECONDARY_DISPLAY_CTRL, HSYNC_PHASE, ACTIVE_HIGH)
            : FIELD_SET(0, SECONDARY_DISPLAY_CTRL, HSYNC_PHASE, ACTIVE_LOW))
          | FIELD_SET(0, SECONDARY_DISPLAY_CTRL, SELECT, SECONDARY)
          | FIELD_SET(0, SECONDARY_DISPLAY_CTRL, TIMING, ENABLE)
          | FIELD_SET(0, SECONDARY_DISPLAY_CTRL, PLANE, ENABLE) 
          | (ulBpp == 8
            ? FIELD_SET(0, SECONDARY_DISPLAY_CTRL, FORMAT, 8)
            : (ulBpp == 16
            ? FIELD_SET(0, SECONDARY_DISPLAY_CTRL, FORMAT, 16)
            : FIELD_SET(0, SECONDARY_DISPLAY_CTRL, FORMAT, 32)));

        /* TODO: Check if the auto expansion bit can be cleared here */
	#ifdef NSFPGA
		displayControlAdjust_SM750NS_F(Private, pModeParam, ulTmpValue);
	#else
		displayControlAdjust_SM750NS(Private, pModeParam, ulTmpValue);
	#endif

	ulTmpValue = SMI_READ_MMIO(Private, DISPLAY_CONTROL_750LE) 
		| FIELD_SET(0, DISPLAY_CONTROL_750LE, VDD, HIGH)
		| FIELD_SET(0, DISPLAY_CONTROL_750LE, DATA, ENABLE)
		| FIELD_SET(0, DISPLAY_CONTROL_750LE, BIAS, HIGH) 
	    | FIELD_SET(0, DISPLAY_CONTROL_750LE, EN, HIGH);
	SMI_WRITE_MMIO(Private, DISPLAY_CONTROL_750LE, ulTmpValue);

        /* Palette RAM. */
        palette_ram = SECONDARY_PALETTE_RAM;
    }
    else 
    {
        /* Primary display control clock: PRIMARY_PLL */
        SMI_WRITE_MMIO(Private, PRIMARY_PLL_CTRL, formatPllReg(pPLL));

        /* Program primary PLL, if applicable */
        if (pPLL->clockType == PRIMARY_PLL)
        {
            SMI_WRITE_MMIO(Private, PRIMARY_PLL_CTRL, formatPllReg(pPLL));

            /* Program to Non-VGA mode when using primary PLL */
            SMI_WRITE_MMIO(Private, VGA_CONFIGURATION, 
                FIELD_SET(SMI_READ_MMIO(Private, VGA_CONFIGURATION), VGA_CONFIGURATION, PLL, PRIMARY));
            SMI_WRITE_MMIO(Private, VGA_CONFIGURATION, 
                FIELD_SET(SMI_READ_MMIO(Private, VGA_CONFIGURATION), VGA_CONFIGURATION, MODE, GRAPHIC));

            /* Select  primary display data path when using primary PLL */
            SMI_WRITE_MMIO(Private, PRIMARY_DISPLAY_CTRL, 
                FIELD_SET(SMI_READ_MMIO(Private, PRIMARY_DISPLAY_CTRL), PRIMARY_DISPLAY_CTRL, SELECT, PRIMARY));
            SMI_WRITE_MMIO(Private, SECONDARY_DISPLAY_CTRL, 
                FIELD_SET(SMI_READ_MMIO(Private, SECONDARY_DISPLAY_CTRL), SECONDARY_DISPLAY_CTRL, SELECT, PRIMARY));
        }
        /* Frame buffer base for this mode */
        SMI_WRITE_MMIO(Private, PRIMARY_FB_ADDRESS,
              FIELD_SET(0, PRIMARY_FB_ADDRESS, STATUS, CURRENT)
            | FIELD_SET(0, PRIMARY_FB_ADDRESS, EXT, LOCAL)
            | FIELD_VALUE(0, PRIMARY_FB_ADDRESS, ADDRESS, ulBaseAddress));

        /* Pitch value (Sometime, hardware people calls it Offset) */
        SMI_WRITE_MMIO(Private, PRIMARY_FB_WIDTH,
              FIELD_VALUE(0, PRIMARY_FB_WIDTH, WIDTH, ulPitch)
            | FIELD_VALUE(0, PRIMARY_FB_WIDTH, OFFSET, ulPitch));

        SMI_WRITE_MMIO(Private, PRIMARY_WINDOW_WIDTH,
              FIELD_VALUE(0, PRIMARY_WINDOW_WIDTH, WIDTH, pModeParam->horizontal_display_end - 1)
            | FIELD_VALUE(0, PRIMARY_WINDOW_WIDTH, X, 0));

        SMI_WRITE_MMIO(Private, PRIMARY_WINDOW_HEIGHT,
              FIELD_VALUE(0, PRIMARY_WINDOW_HEIGHT, HEIGHT, pModeParam->vertical_display_end - 1)
            | FIELD_VALUE(0, PRIMARY_WINDOW_HEIGHT, Y, 0));

        SMI_WRITE_MMIO(Private, PRIMARY_PLANE_TL,
              FIELD_VALUE(0, PRIMARY_PLANE_TL, TOP, 0)
            | FIELD_VALUE(0, PRIMARY_PLANE_TL, LEFT, 0));

        SMI_WRITE_MMIO(Private, PRIMARY_PLANE_BR, 
              FIELD_VALUE(0, PRIMARY_PLANE_BR, BOTTOM, pModeParam->vertical_display_end - 1)
            | FIELD_VALUE(0, PRIMARY_PLANE_BR, RIGHT, pModeParam->horizontal_display_end - 1));

        SMI_WRITE_MMIO(Private, PRIMARY_HORIZONTAL_TOTAL,
              FIELD_VALUE(0, PRIMARY_HORIZONTAL_TOTAL, TOTAL, pModeParam->horizontal_total - 1)
            | FIELD_VALUE(0, PRIMARY_HORIZONTAL_TOTAL, DISPLAY_END, pModeParam->horizontal_display_end - 1));

        SMI_WRITE_MMIO(Private, PRIMARY_HORIZONTAL_SYNC,
              FIELD_VALUE(0, PRIMARY_HORIZONTAL_SYNC, WIDTH, pModeParam->horizontal_sync_width)
            | FIELD_VALUE(0, PRIMARY_HORIZONTAL_SYNC, START, pModeParam->horizontal_sync_start - 1));

        SMI_WRITE_MMIO(Private, PRIMARY_VERTICAL_TOTAL,
              FIELD_VALUE(0, PRIMARY_VERTICAL_TOTAL, TOTAL, pModeParam->vertical_total - 1)
            | FIELD_VALUE(0, PRIMARY_VERTICAL_TOTAL, DISPLAY_END, pModeParam->vertical_display_end - 1));

        SMI_WRITE_MMIO(Private, PRIMARY_VERTICAL_SYNC,
              FIELD_VALUE(0, PRIMARY_VERTICAL_SYNC, HEIGHT, pModeParam->vertical_sync_height)
            | FIELD_VALUE(0, PRIMARY_VERTICAL_SYNC, START, pModeParam->vertical_sync_start - 1));

        /* Set control register value */
        ulTmpValue =
            (pModeParam->clock_phase_polarity == POS
            ? FIELD_SET(0, PRIMARY_DISPLAY_CTRL, CLOCK_PHASE, ACTIVE_HIGH)
            : FIELD_SET(0, PRIMARY_DISPLAY_CTRL, CLOCK_PHASE, ACTIVE_LOW))
          | (pModeParam->vertical_sync_polarity == POS
            ? FIELD_SET(0, PRIMARY_DISPLAY_CTRL, VSYNC_PHASE, ACTIVE_HIGH)
            : FIELD_SET(0, PRIMARY_DISPLAY_CTRL, VSYNC_PHASE, ACTIVE_LOW))
          | (pModeParam->horizontal_sync_polarity == POS
            ? FIELD_SET(0, PRIMARY_DISPLAY_CTRL, HSYNC_PHASE, ACTIVE_HIGH)
            : FIELD_SET(0, PRIMARY_DISPLAY_CTRL, HSYNC_PHASE, ACTIVE_LOW))
          | FIELD_SET(0, PRIMARY_DISPLAY_CTRL, TIMING, ENABLE)
          | FIELD_SET(0, PRIMARY_DISPLAY_CTRL, PLANE, ENABLE)
          | (ulBpp == 8
            ? FIELD_SET(0, PRIMARY_DISPLAY_CTRL, FORMAT, 8)
            : (ulBpp == 16
            ? FIELD_SET(0, PRIMARY_DISPLAY_CTRL, FORMAT, 16)
            : FIELD_SET(0, PRIMARY_DISPLAY_CTRL, FORMAT, 32)));

        /* Added some masks to mask out the reserved bits. 
         * Sometimes, the reserved bits are set/reset randomly when 
         * writing to the PRIMARY_DISPLAY_CTRL, therefore, the register
         * reserved bits are needed to be masked out.
         */
        ulReservedBits =(UINT32) FIELD_SET(0, PRIMARY_DISPLAY_CTRL, RESERVED_1_MASK, ENABLE) |
                         FIELD_SET(0, PRIMARY_DISPLAY_CTRL, RESERVED_2_MASK, ENABLE) |
                         FIELD_SET(0, PRIMARY_DISPLAY_CTRL, RESERVED_3_MASK, ENABLE);

        ulReg = (SMI_READ_MMIO(Private, PRIMARY_DISPLAY_CTRL) & ~ulReservedBits)
              & FIELD_CLEAR(PRIMARY_DISPLAY_CTRL, CLOCK_PHASE)
              & FIELD_CLEAR(PRIMARY_DISPLAY_CTRL, VSYNC_PHASE)
              & FIELD_CLEAR(PRIMARY_DISPLAY_CTRL, HSYNC_PHASE)
              & FIELD_CLEAR(PRIMARY_DISPLAY_CTRL, TIMING)
              & FIELD_CLEAR(PRIMARY_DISPLAY_CTRL, VERTICAL_PAN)
              & FIELD_CLEAR(PRIMARY_DISPLAY_CTRL, HORIZONTAL_PAN)
              & FIELD_CLEAR(PRIMARY_DISPLAY_CTRL, PLANE)
              & FIELD_CLEAR(PRIMARY_DISPLAY_CTRL, FORMAT);

        SMI_WRITE_MMIO(Private, PRIMARY_DISPLAY_CTRL, ulTmpValue | ulReg);

        /* 
         * PRIMARY_DISPLAY_CTRL register seems requiring few writes
         * before a value can be succesfully written in.
         * Added some masks to mask out the reserved bits.
         * Note: This problem happens by design. The hardware will wait for the
         *       next vertical sync to turn on/off the plane.
         */
        while((SMI_READ_MMIO(Private, PRIMARY_DISPLAY_CTRL) & ~ulReservedBits) != (ulTmpValue|ulReg))
        {
            SMI_WRITE_MMIO(Private, PRIMARY_DISPLAY_CTRL, ulTmpValue | ulReg);
        }

        /* Palette RAM */
        palette_ram = PRIMARY_PALETTE_RAM;
    }

    /* For 16- and 32-bpp,  fill palette with gamma values. */
    {
        /* Start with RGB = 0,0,0. */
        ulTmpValue = 0x000000;
        //for (offset = 0; offset < 256 * 4; offset += 4)
        {
            //SMI_WRITE_MMIO(Private, palette_ram + offset, ulTmpValue);	//allan dbg

            /* Advance RGB by 1,1,1. */
            //ulTmpValue += 0x010101;
        }
    }
	//SMIDEBUG("programModeRegisters -\n");
}

/*
 * Input:
 *     1) pLogicalMode contains information such as x, y resolution and bpp.
 *     2) A user defined parameter table for the mode.
 *
 * This function calculate and programs the hardware to set up the
 * requested mode.
 *
 * This function allows the use of user defined parameter table if
 * predefined Vesa parameter table (gDefaultModeParamTable) does not fit.
 *
 * Return: 0 (or NO_ERROR) if mode can be set successfully.
 *         -1 if any set mode error.
 */
long 
setModeEx(
	SMI_PRIVATE_DATA  *Private, 
	mode_parameter_t *pModeParam, 
	UINT32 modeWidth,
	unsigned bpp 
)
{
    //SMIDEBUG("setModeEx +\n");
    //    mode_parameter_t pModeParam; /* physical parameters for the mode */
    pll_value_t pll;
    UINT32 ulActualPixelClk, ulTemp, ulPitch;
    //UINT8  Data;
    
    /*
     * Set up PLL, a structure to hold the value to be set in clocks.
     */
    pll.inputFreq = DEFAULT_INPUT_CLOCK; /* Defined in CLOCK.H */

    /* Get the Clock Type */
    pll.clockType = getClockType();

    /* 
     * Call calcPllValue() to fill up the other fields for PLL structure.
     * Sometime, the chip cannot set up the exact clock required by User.
     * Return value from calcPllValue() gives the actual possible pixel clock.
     */
    ulActualPixelClk = calcPllValue(pModeParam->pixel_clock, &pll);
    /* If calling function don't have a preferred pitch value, 
       work out a 16 byte aligned pitch value.
    */
        /* 
         * Pitch value calculation in Bytes.
         * Usually, it is (screen width) * (byte per pixel).
         * However, there are cases that screen width is not 16 pixel aligned, which is
         * a requirement for some OS and the hardware itself.
         * For standard 4:3 resolutions: 320, 640, 800, 1024 and 1280, they are all
         * 16 pixel aligned and pitch is simply (screen width) * (byte per pixel).
         *   
         * However, 1366 resolution, for example, has to be adjusted for 16 pixel aligned.
         */

        ulTemp = (modeWidth + 15) & ~15; /* This calculation has no effect on 640, 800, 1024 and 1280. */
        ulPitch = ulTemp * (bpp / 8);

#if 0
	/* Write CRT88 before setting the other registers. */
	Data = 0x88;
	Private->PciIo->Mem.Write (
	                  Private->PciIo,
	                  EfiPciIoWidthUint8,
	                  EFI_PCI_IO_PASS_THROUGH_BAR,
	                  VGA_CR_INDEX,
	                  1,
	                  &Data
	                  );
	Data = 0x06;
	Private->PciIo->Mem.Write (
	                  Private->PciIo,
	                  EfiPciIoWidthUint8,
	                  EFI_PCI_IO_PASS_THROUGH_BAR,
	                  VGA_CR_DATA,
	                  1,
	                  &Data
	                  );

#endif
    	/* Program the hardware to set up the mode. */
	programModeRegisters(
        Private, 
        pModeParam,
        bpp, 
        0, 
        ulPitch, 
        &pll);
        
    //SMIDEBUG("setModeEx -\n");
    return (0);
}
