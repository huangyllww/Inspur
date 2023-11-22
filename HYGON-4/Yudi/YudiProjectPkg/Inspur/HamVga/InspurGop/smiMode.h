#ifndef _SMIMODE_H_
#define _SMIMODE_H_

#define DEFAULT_INPUT_CLOCK 14318181 /* Default reference clock */
#define MHz(x) (x*1000000) /* Don't use this macro if x is fraction number */

/* The following maximum value is not the optimum value. */
/* Maximum Master Clock is about 190MHz */
#define MAXIMUM_MASTER_CLOCK        MHz(190)

/* Maximum Chip Clock (MXCLK) is 1 GHz */
#define MAXIMUM_CHIP_CLOCK          MHz(1000)


UINT32 roundedDiv(UINT32 num, UINT32 denom);
UINT32 absDiff(UINT32 a, UINT32 b);
UINT32 twoToPowerOfx(UINT32 x);


typedef enum _clock_type_t
{
    MXCLK_PLL,      /* Programmable Master clock */
    PRIMARY_PLL,    /* Programmable Primary pixel clock */
    SECONDARY_PLL,  /* Programmable Secondary pixel clock */
    VGA0_PLL,
    VGA1_PLL,
}
clock_type_t;

typedef struct pll_value_t
{
    clock_type_t clockType;
    UINT32 inputFreq; /* Input clock frequency to the PLL */
    UINT32 M;
    UINT32 N;
    UINT32 OD;
    UINT32 POD;
}
pll_value_t;

UINT32 getPowerMode(SMI_PRIVATE_DATA  *Private);
void setPowerMode(
SMI_PRIVATE_DATA  *Private,
UINT32 powerMode);

void setCurrentGate(
SMI_PRIVATE_DATA  *Private,
UINT32 gate
);

UINT32 formatPllReg(pll_value_t *pPLL);
UINT32 calcPLL(pll_value_t *pPLL);
clock_type_t getClockType();

typedef enum _spolarity_t
{
    POS, /* positive */
    NEG, /* negative */
}
spolarity_t;

typedef struct _mode_parameter_t
{
    /* Horizontal timing. */
    UINT32 horizontal_total;
    UINT32 horizontal_display_end;
    UINT32 horizontal_sync_start;
    UINT32 horizontal_sync_width;
    spolarity_t horizontal_sync_polarity;

    /* Vertical timing. */
    UINT32 vertical_total;
    UINT32 vertical_display_end;
    UINT32 vertical_sync_start;
    UINT32 vertical_sync_height;
    spolarity_t vertical_sync_polarity;

    /* Refresh timing. */
    UINT32 pixel_clock;
    UINT32 horizontal_frequency;
    UINT32 vertical_frequency;
    
    /* Clock Phase. This clock phase only applies to Panel. */
    spolarity_t clock_phase_polarity;
}
mode_parameter_t;


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
);

long setModeEx(SMI_PRIVATE_DATA  *Private, 
mode_parameter_t *pModeParam,   /* mode information about pixel clock, horizontal total, etc. */
UINT32 modeWidth,
unsigned bpp);

#endif /* _SMIMODE_H_ */
