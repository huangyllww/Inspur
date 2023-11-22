/*******************************************************************
* 
*         Copyright (c) 2008 by Silicon Motion, Inc. (SMI)
* 
*  All rights are reserved. Reproduction or in part is prohibited
*  without the written consent of the copyright owner.
* 
*******************************************************************/
#ifndef _SMI2D_H_
#define _SMI2D_H_
#include "BuildFlag.h"

long deWaitForNotBusy(
SMI_PRIVATE_DATA  *Private
);

void deSetPixelFormat(
SMI_PRIVATE_DATA  *Private,
    UINT32 bpp
);

UINT32 deGetTransparency(
SMI_PRIVATE_DATA  *Private
);
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
UINT32 ulColor);    /* Color to compare. */

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
UINT32 y2);     /* Note that the region will not include x2 and y2 */

/*
 * 2D Engine Initialization.
 * This function must be called before other 2D functions.
 * Assumption: A specific video mode has been properly set up.
 */
void deInit(
SMI_PRIVATE_DATA  *Private
);

/*
 * Reset 2D engine by 
 * 1) Aborting the current 2D operation.
 * 2) Re-enable 2D engine to normal state.
 */
void deReset(
SMI_PRIVATE_DATA  *Private
);

/*
 *  resetFrameBufferMemory
 *      This function resets the Frame Buffer Memory
 */
void resetFrameBufferMemory(
SMI_PRIVATE_DATA  *Private
);

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
UINT32 rop2);   /* ROP value */

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
UINT32 rop2);   /* ROP value */

#endif /* _SMI2D_H_ */
