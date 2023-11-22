/*******************************************************************
* 
*         Copyright (c) 2008 by Silicon Motion, Inc. (SMI)
* 
*  All rights are reserved. Reproduction or in part is prohibited
*  without the written consent of the copyright owner.
* 
*******************************************************************/
#include "BuildFlag.h"
#ifdef UEFI
#include "UefiSmi.h"
#else
#include "efiSmi.h"
#endif
#include "smi2d.h"
#include "defs.h"


/*
 * Wait until 2D engine is not busy.
 * All 2D operations are recommand to check 2D engine idle before start.
 *
 * Return: 0 = return because engine is idle and normal.
 *        -1 = return because time out (2D engine may have problem).
 */
long deWaitForNotBusy(
SMI_PRIVATE_DATA  *Private
)
{
    UINT32 dwVal;
    UINT32 i = 0x100000;
    
    while (i--)
    {
        dwVal = (UINT32)SMI_READ_MMIO(Private, DE_STATE2);
        if ((FIELD_GET(dwVal, DE_STATE2, DE_STATUS)      == DE_STATE2_DE_STATUS_IDLE) &&
            (FIELD_GET(dwVal, DE_STATE2, DE_FIFO)        == DE_STATE2_DE_FIFO_EMPTY) &&
            (FIELD_GET(dwVal, DE_STATE2, DE_MEM_FIFO)    == DE_STATE2_DE_MEM_FIFO_EMPTY))
        {
            return 0; /* Return because engine idle */
        }
    }
    
    return -1; /* Return because time out */

}

/*
 * This function sets the pixel format that will apply to the 2D Engine.
 */
void deSetPixelFormat(
SMI_PRIVATE_DATA  *Private,
    UINT32 bpp
)
{
    UINT32 de_format;
    
    de_format = (UINT32)SMI_READ_MMIO(Private, DE_STRETCH_FORMAT);
    
    switch (bpp)
    {
        case 8:
            de_format = FIELD_SET(de_format, DE_STRETCH_FORMAT, PIXEL_FORMAT, 8);
            break;
        default:
        case 16:
            de_format = FIELD_SET(de_format, DE_STRETCH_FORMAT, PIXEL_FORMAT, 16);
            break;
        case 32:
            de_format = FIELD_SET(de_format, DE_STRETCH_FORMAT, PIXEL_FORMAT, 32);
            break;
    }
    
    SMI_WRITE_MMIO(Private, DE_STRETCH_FORMAT, de_format);
}
/*
 * This function gets the transparency status from DE_CONTROL register.
 * It returns a double word with the transparent fields properly set,
 * while other fields are 0.
 */
UINT32 deGetTransparency(
SMI_PRIVATE_DATA  *Private
)
{
    UINT32 de_ctrl;

    de_ctrl = (UINT32)SMI_READ_MMIO(Private, DE_CONTROL);

    de_ctrl &= 
        FIELD_MASK(DE_CONTROL_TRANSPARENCY_MATCH) | 
        FIELD_MASK(DE_CONTROL_TRANSPARENCY_SELECT)| 
        FIELD_MASK(DE_CONTROL_TRANSPARENCY);

    return de_ctrl;
}
/* 
 * Function description:
 * When transparency is enable, the blt engine compares each pixel value 
 * (either source or destination) with DE_COLOR_COMPARE register.
 * If match, the destination pixel will NOT be updated.
 * If not match, the destination pixel will be updated.
 */
long deSetTransparency(
SMI_PRIVATE_DATA  *Private,
UINT32 enable,     /* 0 = disable, 1 = enable transparency feature */
UINT32 tSelect,    /* 0 = compare source, 1 = compare destination */
UINT32 tMatch,     /* 0 = Opaque mode, 1 = transparent mode */
UINT32 ulColor)    /* Color to compare. */
{
    UINT32 de_ctrl;

    if (deWaitForNotBusy(Private) != 0)
    {
        /* The 2D engine is always busy for some unknown reason.
           Application can choose to return ERROR, or reset it and
           continue the operation.
        */

        return -1;

        /* or */
        /* deReset(); */
    }

    /* Set mask */
    if (enable)
    {
        SMI_WRITE_MMIO(Private, DE_COLOR_COMPARE_MASK, 0x00ffffff);

        /* Set compare color */
        SMI_WRITE_MMIO(Private, DE_COLOR_COMPARE, ulColor);
    }
    else
    {
        SMI_WRITE_MMIO(Private, DE_COLOR_COMPARE_MASK, 0x0);
        SMI_WRITE_MMIO(Private, DE_COLOR_COMPARE, 0x0);
    }

    /* Set up transparency control, without affecting other bits
       Note: There are two operatiing modes: Transparent and Opague.
       We only use transparent mode because Opaque mode may have bug.
    */
    de_ctrl = SMI_READ_MMIO(Private, DE_CONTROL)
              & FIELD_CLEAR(DE_CONTROL, TRANSPARENCY)
              & FIELD_CLEAR(DE_CONTROL, TRANSPARENCY_MATCH)
              & FIELD_CLEAR(DE_CONTROL, TRANSPARENCY_SELECT);

    /* For DE_CONTROL_TRANSPARENCY_MATCH bit, always set it
       to TRANSPARENT mode, OPAQUE mode don't seem working.
    */
    de_ctrl |=
    ((enable)?
      FIELD_SET(0, DE_CONTROL, TRANSPARENCY, ENABLE)
    : FIELD_SET(0, DE_CONTROL, TRANSPARENCY, DISABLE))        |
    ((tMatch)?
      FIELD_SET(0, DE_CONTROL, TRANSPARENCY_MATCH, TRANSPARENT)
    : FIELD_SET(0, DE_CONTROL, TRANSPARENCY_MATCH, OPAQUE)) |
    ((tSelect)?
      FIELD_SET(0, DE_CONTROL, TRANSPARENCY_SELECT, DESTINATION)
    : FIELD_SET(0, DE_CONTROL, TRANSPARENCY_SELECT, SOURCE));

    SMI_WRITE_MMIO(Private, DE_CONTROL, de_ctrl);

    return 0;
}
/*
 * This function enable/disable clipping area for the 2d engine.
 * Note that the clipping area is always rectangular.
 * 
 */
long deSetClipping(
SMI_PRIVATE_DATA  *Private,
UINT32 enable, /* 0 = disable clipping, 1 = enable clipping */
UINT32 x1,     /* x1, y1 is the upper left corner of the clipping area */
UINT32 y1,     /* Note that the region includes x1 and y1 */
UINT32 x2,     /* x2, y2 is the lower right corner of the clippiing area */
UINT32 y2)     /* Note that the region will not include x2 and y2 */
{
    if (deWaitForNotBusy(Private) != 0)
    {
        /* The 2D engine is always busy for some unknown reason.
           Application can choose to return ERROR, or reset it and
           continue the operation.
        */

        return -1;

        /* or */
        /* deReset(); */
    }

    /* Upper left corner and enable/disable bit
       Note: This module defautls to clip outside region.
       "Clip inside" is not a useful feature since nothing gets drawn.
     */
    SMI_WRITE_MMIO(Private, DE_CLIP_TL,
        FIELD_VALUE(0, DE_CLIP_TL, TOP, y1) |
        ((enable)?
          FIELD_SET(0, DE_CLIP_TL, STATUS, ENABLE)
        : FIELD_SET(0, DE_CLIP_TL, STATUS, DISABLE))|
        FIELD_SET  (0, DE_CLIP_TL, INHIBIT,OUTSIDE) |
        FIELD_VALUE(0, DE_CLIP_TL, LEFT, x1));

    /* Lower right corner */
    SMI_WRITE_MMIO(Private, DE_CLIP_BR,
        FIELD_VALUE(0, DE_CLIP_BR, BOTTOM,y2) |
        FIELD_VALUE(0, DE_CLIP_BR, RIGHT, x2));

    return 0;
}

/*
 * 2D Engine Initialization.
 * This function must be called before other 2D functions.
 * Assumption: A specific video mode has been properly set up.
 */
void deInit(
SMI_PRIVATE_DATA  *Private
)
{
   SMIDEBUG( "De +\n" );
    //enable2DEngine(1);

    deReset(Private); /* Just be sure no left-over operations from other applications */

    /* Set up 2D registers that won't change for a specific mode. */

    /* Drawing engine bus and pixel mask, always want to enable. */
    SMI_WRITE_MMIO(Private, DE_MASKS, 0xFFFFFFFF);

    /* Pixel format, which can be 8, 16 or 32.
       Assuming setmode is call before 2D init, then pixel format
       is available in reg 0x80000 (Panel Display Control)
    */
    SMI_WRITE_MMIO(Private, DE_STRETCH_FORMAT,
        FIELD_SET  (0, DE_STRETCH_FORMAT, PATTERN_XY,   NORMAL)  |
        FIELD_VALUE(0, DE_STRETCH_FORMAT, PATTERN_Y,    0)       |
        FIELD_VALUE(0, DE_STRETCH_FORMAT, PATTERN_X,    0)       |
        FIELD_SET  (0, DE_STRETCH_FORMAT, ADDRESSING,   XY)      |
        FIELD_VALUE(0, DE_STRETCH_FORMAT, SOURCE_HEIGHT,3));

    /* Clipping and transparent are disable after INIT */
    deSetClipping(Private, 0, 0, 0, 0, 0);
    deSetTransparency(Private, 0, 0, 0, 0);
    SMIDEBUG("De -\n" );
}

/*
 *  resetFrameBufferMemory
 *      This function resets the Frame Buffer Memory
 */
void resetFrameBufferMemory(
SMI_PRIVATE_DATA  *Private
)
{
    //UINT32 ulReg;
    
    //SMIDEBUG("Resetting Memory\n");
        
    /* Only SM718 and SM750 has register to reset video memory */
}


/*
 * Reset 2D engine by 
 * 1) Aborting the current 2D operation.
 * 2) Re-enable 2D engine to normal state.
 */
void deReset(
SMI_PRIVATE_DATA  *Private
)
{
    UINT32 sysCtrl;

    /* Abort current 2D operation */
    sysCtrl = SMI_READ_MMIO(Private, DE_STATE1);
    sysCtrl = FIELD_SET(sysCtrl, DE_STATE1, DE_ABORT, ON);
    SMI_WRITE_MMIO(Private, DE_STATE1, sysCtrl);

    /* Re-enable 2D engine to normal state */
    sysCtrl = SMI_READ_MMIO(Private, DE_STATE1);
    sysCtrl = FIELD_SET(sysCtrl, DE_STATE1, DE_ABORT, OFF);
    SMI_WRITE_MMIO(Private, DE_STATE1, sysCtrl);
}

/*
 * Video Memory to Video Memory data transfer.
 * Note: 
 *        It works whether the Video Memroy is off-screeen or on-screen.
 *        This function is a one to one transfer without stretching or 
 *        mono expansion.
 */
long deVideoMem2VideoMemBlt(
SMI_PRIVATE_DATA  *Private,
UINT32 sBase,  /* Address of source: offset in frame buffer */
UINT32 sPitch, /* Pitch value of source surface in BYTE */
UINT32 sx,
UINT32 sy,     /* Starting coordinate of source surface */
UINT32 dBase,  /* Address of destination: offset in frame buffer */
UINT32 dPitch, /* Pitch value of destination surface in BYTE */
UINT32 bpp,    /* Color depth of destination surface */
UINT32 dx,
UINT32 dy,     /* Starting coordinate of destination surface */
UINT32 width, 
UINT32 height, /* width and height of rectangle in pixel value */
UINT32 rop2)   /* ROP value */
{
    UINT32 nDirection, de_ctrl, bytePerPixel;
    long opSign;
  SMIDEBUG("deVideoMem2VideoMemBlt +\n");
    if (deWaitForNotBusy(Private) != 0)
    {
        /* The 2D engine is always busy for some unknown reason.
           Application can choose to return ERROR, or reset it and
           continue the operation.
        */

        return -1;

        /* or */
        /* deReset(); */
    }

    nDirection = LEFT_TO_RIGHT;
    opSign = 1;    /* Direction of ROP2 operation: 1 = Left to Right, (-1) = Right to Left */
    bytePerPixel = bpp/8;
    de_ctrl = 0;

    /* If source and destination are the same surface, need to check for overlay cases */
    if (sBase == dBase && sPitch == dPitch)
    {
        /* Determine direction of operation */
        if (sy < dy)
        {
            /* +----------+
               |S         |
               |   +----------+
               |   |      |   |
               |   |      |   |
               +---|------+   |
                   |         D|
                   +----------+ */
    
            nDirection = BOTTOM_TO_TOP;
        }
        else if (sy > dy)
        {
            /* +----------+
               |D         |
               |   +----------+
               |   |      |   |
               |   |      |   |
               +---|------+   |
                   |         S|
                   +----------+ */
    
            nDirection = TOP_TO_BOTTOM;
        }
        else
        {
            /* sy == dy */
    
            if (sx <= dx)
            {
                /* +------+---+------+
                   |S     |   |     D|
                   |      |   |      |
                   |      |   |      |
                   |      |   |      |
                   +------+---+------+ */
    
                nDirection = RIGHT_TO_LEFT;
            }
            else
            {
                /* sx > dx */
    
                /* +------+---+------+
                   |D     |   |     S|
                   |      |   |      |
                   |      |   |      |
                   |      |   |      |
                   +------+---+------+ */
    
                nDirection = LEFT_TO_RIGHT;
            }
        }
    }

    if ((nDirection == BOTTOM_TO_TOP) || (nDirection == RIGHT_TO_LEFT))
    {
        sx += width - 1;
        sy += height - 1;
        dx += width - 1;
        dy += height - 1;
        opSign = (-1);
    }

    /* Note:
       DE_FOREGROUND are DE_BACKGROUND are don't care.
       DE_COLOR_COMPARE and DE_COLOR_COMPARE_MAKS are set by set deSetTransparency().
    */

    /* 2D Source Base.
       It is an address offset (128 bit aligned) from the beginning of frame buffer.
    */
    SMI_WRITE_MMIO(Private, DE_WINDOW_SOURCE_BASE, sBase);

    /* 2D Destination Base.
       It is an address offset (128 bit aligned) from the beginning of frame buffer.
    */
    SMI_WRITE_MMIO(Private, DE_WINDOW_DESTINATION_BASE, dBase);

    /* Program pitch (distance between the 1st points of two adjacent lines).
       Note that input pitch is BYTE value, but the 2D Pitch register uses
       pixel values. Need Byte to pixel convertion.
    */
    SMI_WRITE_MMIO(Private, DE_PITCH,
        FIELD_VALUE(0, DE_PITCH, DESTINATION, (dPitch/bytePerPixel)) |
        FIELD_VALUE(0, DE_PITCH, SOURCE,      (sPitch/bytePerPixel)));

    /* Screen Window width in Pixels.
       2D engine uses this value to calculate the linear address in frame buffer for a given point.
    */
    SMI_WRITE_MMIO(Private, DE_WINDOW_WIDTH,
        FIELD_VALUE(0, DE_WINDOW_WIDTH, DESTINATION, (dPitch/bytePerPixel)) |
        FIELD_VALUE(0, DE_WINDOW_WIDTH, SOURCE,      (sPitch/bytePerPixel)));

    /* Set the pixel format of the destination */
    deSetPixelFormat(Private, bpp);
    
#ifdef ENABLE_192_BYTES_PATCH
    /* This bug is fixed in SM718 for 16 and 32 bpp. However, in 8-bpp, the problem still exists. 
       The Version AA also have this problem on higher clock with 32-bit memory data bus, 
       therefore, it needs to be enabled here. 
       In version AA, the problem happens on the following configurations:
        1. M2XCLK = 336MHz w/ 32-bit, MCLK = 112MHz, and color depth set to 32bpp
        2. M2XCLK = 336MHz w/ 32-bit, MCLK = 84MHz, and color depth set to 16bpp or 32bpp.
       Somehow, the problem does not appears in 64-bit memory setting.
     */

    /* Workaround for 192 byte requirement when ROP is not COPY */
    if ((rop2 != ROP2_COPY) && ((width * bytePerPixel) > 192))
    {
        /* Perform the ROP2 operation in chunks of (xWidth * nHeight) */
        UINT32 xChunk = 192 / bytePerPixel; /* chunk width is in pixels */
        
        while (1)
        {
            deWaitForNotBusy(Private);
            SMI_WRITE_MMIO(Private, DE_SOURCE,
                FIELD_SET  (0, DE_SOURCE, WRAP, DISABLE) |
                FIELD_VALUE(0, DE_SOURCE, X_K1, sx)   |
                FIELD_VALUE(0, DE_SOURCE, Y_K2, sy));
            SMI_WRITE_MMIO(Private, DE_DESTINATION,
                FIELD_SET  (0, DE_DESTINATION, WRAP, DISABLE) |
                FIELD_VALUE(0, DE_DESTINATION, X,    dx)  |
                FIELD_VALUE(0, DE_DESTINATION, Y,    dy));
            SMI_WRITE_MMIO(Private, DE_DIMENSION,
                FIELD_VALUE(0, DE_DIMENSION, X,    xChunk) |
                FIELD_VALUE(0, DE_DIMENSION, Y_ET, height));

            de_ctrl = 
                FIELD_VALUE(0, DE_CONTROL, ROP, rop2) |
                FIELD_SET(0, DE_CONTROL, ROP_SELECT, ROP2) |
                FIELD_SET(0, DE_CONTROL, COMMAND, BITBLT) |
                ((nDirection == RIGHT_TO_LEFT) ? 
                FIELD_SET(0, DE_CONTROL, DIRECTION, RIGHT_TO_LEFT)
                : FIELD_SET(0, DE_CONTROL, DIRECTION, LEFT_TO_RIGHT)) |
                FIELD_SET(0, DE_CONTROL, STATUS, START);

            SMI_WRITE_MMIO(Private, DE_CONTROL, de_ctrl | deGetTransparency());

            if (xChunk == width) break;

            sx += (opSign * xChunk);
            dx += (opSign * xChunk);
            width -= xChunk;

            if (xChunk > width)
            {
                /* This is the last chunk. */
                xChunk = width;
            }
        }
    }
    else
#endif
    {
        deWaitForNotBusy(Private);

        SMI_WRITE_MMIO(Private, DE_SOURCE,
            FIELD_SET  (0, DE_SOURCE, WRAP, DISABLE) |
            FIELD_VALUE(0, DE_SOURCE, X_K1, sx)   |
            FIELD_VALUE(0, DE_SOURCE, Y_K2, sy));
        SMI_WRITE_MMIO(Private, DE_DESTINATION,
            FIELD_SET  (0, DE_DESTINATION, WRAP, DISABLE) |
            FIELD_VALUE(0, DE_DESTINATION, X,    dx)  |
            FIELD_VALUE(0, DE_DESTINATION, Y,    dy));
        SMI_WRITE_MMIO(Private, DE_DIMENSION,
            FIELD_VALUE(0, DE_DIMENSION, X,    width) |
            FIELD_VALUE(0, DE_DIMENSION, Y_ET, height));

        de_ctrl = 
            FIELD_VALUE(0, DE_CONTROL, ROP, rop2) |
            FIELD_SET(0, DE_CONTROL, ROP_SELECT, ROP2) |
            FIELD_SET(0, DE_CONTROL, COMMAND, BITBLT) |
            ((nDirection == RIGHT_TO_LEFT) ? 
            FIELD_SET(0, DE_CONTROL, DIRECTION, RIGHT_TO_LEFT)
            : FIELD_SET(0, DE_CONTROL, DIRECTION, LEFT_TO_RIGHT)) |
            FIELD_SET(0, DE_CONTROL, STATUS, START);

        SMI_WRITE_MMIO(Private, DE_CONTROL, de_ctrl | deGetTransparency(Private));
    }
  SMIDEBUG("deVideoMem2VideoMemBlt -\n");

    return 0;
}

/*
 * This function uses 2D engine to fill a rectangular area with a specific color.
 * The filled area includes the starting points.
 */
long deRectFill( /*resolution_t resolution, point_t p0, point_t p1, UINT32 color, UINT32 rop2)*/
SMI_PRIVATE_DATA  *Private,
UINT32 dBase,  /* Base address of destination surface counted from beginning of video frame buffer */
UINT32 dPitch, /* Pitch value of destination surface in BYTES */
UINT32 bpp,    /* Color depth of destination surface: 8, 16 or 32 */
UINT32 x,
UINT32 y,      /* Upper left corner (X, Y) of rectangle in pixel value */
UINT32 width, 
UINT32 height, /* width and height of rectange in pixel value */
UINT32 color,  /* Color to be filled */
UINT32 rop2)   /* ROP value */
{
    UINT32 de_ctrl, bytePerPixel;
  SMIDEBUG("deRectFill +\n");
  /*
  SMIDEBUG("dBase=%d,dPitch=%d,bpp=%d,dx=%d,\n dy=%d, Width=%d, Height=%d, color=%d\n,",
 dBase, 
 dPitch,
 bpp,
 x,    
 y,    
 width, 
 height,
 color
);
*/

    bytePerPixel = bpp/8;
    if (deWaitForNotBusy(Private) != 0)
    {
        /* The 2D engine is always busy for some unknown reason.
           Application can choose to return ERROR, or reset it and
           continue the operation.
        */

        return -1;

        /* or */
        /* deReset(); */
    }

    /* 2D Destination Base.
       It is an address offset (128 bit aligned) from the beginning of frame buffer.
    */
    SMI_WRITE_MMIO(Private, DE_WINDOW_DESTINATION_BASE, dBase);

    /* Program pitch (distance between the 1st points of two adjacent lines).
       Note that input pitch is BYTE value, but the 2D Pitch register uses
       pixel values. Need Byte to pixel convertion.
    */
    SMI_WRITE_MMIO(Private, DE_PITCH,
        FIELD_VALUE(0, DE_PITCH, DESTINATION, (dPitch/bytePerPixel)) |
        FIELD_VALUE(0, DE_PITCH, SOURCE,      (dPitch/bytePerPixel)));

    /* Screen Window width in Pixels.
       2D engine uses this value to calculate the linear address in frame buffer for a given point.
    */
    SMI_WRITE_MMIO(Private, DE_WINDOW_WIDTH,
        FIELD_VALUE(0, DE_WINDOW_WIDTH, DESTINATION, (dPitch/bytePerPixel)) |
        FIELD_VALUE(0, DE_WINDOW_WIDTH, SOURCE,      (dPitch/bytePerPixel)));

    SMI_WRITE_MMIO(Private, DE_FOREGROUND, color);

    /* Set the pixel format of the destination */
    deSetPixelFormat(Private, bpp);

#ifdef ENABLE_192_BYTES_PATCH
    /* Workaround for 192 byte requirement when ROP is not COPY */
    if (((rop2 != ROP2_COPY) || (rop2 != ROP2_Sn) || (rop2 != ROP2_Dn) || 
         (rop2 != ROP2_D) || (rop2 != ROP2_BLACK) || (rop2 != ROP2_WHITE)) && 
        ((width * bytePerPixel) > 192))
    {
        /* Perform the ROP2 operation in chunks of (xWidth * nHeight) */
        UINT32 xChunk = 192 / bytePerPixel; /* chunk width is in pixels */
        
        while (1)
        {
            deWaitForNotBusy(Private);
            
            SMI_WRITE_MMIO(Private, DE_DESTINATION,
                FIELD_SET  (0, DE_DESTINATION, WRAP, DISABLE) |
                FIELD_VALUE(0, DE_DESTINATION, X,    x)  |
                FIELD_VALUE(0, DE_DESTINATION, Y,    y));
                
            SMI_WRITE_MMIO(Private, DE_DIMENSION,
                FIELD_VALUE(0, DE_DIMENSION, X,    xChunk) |
                FIELD_VALUE(0, DE_DIMENSION, Y_ET, height));

            de_ctrl = 
                FIELD_SET  (0, DE_CONTROL,  STATUS,     START)          |
                FIELD_SET  (0, DE_CONTROL,  DIRECTION,  LEFT_TO_RIGHT)  |
                //FIELD_SET  (0, DE_CONTROL,LAST_PIXEL, OFF)            |
                FIELD_SET  (0, DE_CONTROL,  COMMAND,    RECTANGLE_FILL) |
                FIELD_SET  (0, DE_CONTROL,  ROP_SELECT, ROP2)           |
                FIELD_VALUE(0, DE_CONTROL,  ROP,        rop2);

            SMI_WRITE_MMIO(Private, DE_CONTROL, de_ctrl | deGetTransparency(Private));

            if (xChunk == width) break;

            x += xChunk;
            width -= xChunk;

            if (xChunk > width)
            {
                /* This is the last chunk. */
                xChunk = width;
            }
        }
    }
    else
#endif
    {
        SMI_WRITE_MMIO(Private, DE_DESTINATION,
            FIELD_SET  (0, DE_DESTINATION, WRAP, DISABLE) |
            FIELD_VALUE(0, DE_DESTINATION, X,    x)       |
            FIELD_VALUE(0, DE_DESTINATION, Y,    y));

        SMI_WRITE_MMIO(Private, DE_DIMENSION,
            FIELD_VALUE(0, DE_DIMENSION, X,    width) |
            FIELD_VALUE(0, DE_DIMENSION, Y_ET, height));

        de_ctrl = 
            FIELD_SET  (0, DE_CONTROL,  STATUS,     START)          |
            FIELD_SET  (0, DE_CONTROL,  DIRECTION,  LEFT_TO_RIGHT)  |
            //FIELD_SET  (0, DE_CONTROL,LAST_PIXEL, OFF)            |
            FIELD_SET  (0, DE_CONTROL,  COMMAND,    RECTANGLE_FILL) |
            FIELD_SET  (0, DE_CONTROL,  ROP_SELECT, ROP2)           |
            FIELD_VALUE(0, DE_CONTROL,  ROP,        rop2);

        SMI_WRITE_MMIO(Private, DE_CONTROL, de_ctrl | deGetTransparency(Private));
    }
//  SMI_PrintRegs(Private);
//  while (1);
  SMIDEBUG("deRectFill -\n");
    return 0;
}

