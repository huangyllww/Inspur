#ifndef _FUSE_H_
#define _FUSE_H_

#include <Base.h>

// CDD Fuse
// #define CDD_CCX_BIST_EN_FUSE                0x5d0CC
// #define CDD_BIST_EN_FUSE_BITS               4
// #define CDD_CCX_COLD_RESET_MBIST_EN         0x5D0DF
// #define CDD_COLD_RESET_MBIST_EN_FUSE_BITS   25
#define MP0_CORE_DISABLE_HYEX  0x5D4CC
#define MP0_CORE_DISABLE_HYGX  0x5D478

// IOD Fuse
#define IOD_MP0_RUN_MBIST_HYEX           0x5D168
#define IOD_RUN_MBIST_FUSE_BITS_HYEX     2

#define IOD_MP0_RUN_MBIST_HYGX           0x5D304
#define IOD_RUN_MBIST_FUSE_BITS_HYGX     1

#define IOD_MP1_MAX_TDP_HYEX             0x5D268
#define IOD_MP1_MAX_TDP_START_BITS_HYEX  7
#define IOD_MP1_FMAX_HYEX                0x5D270
#define IOD_MP1_FMAX_START_BITS_HYEX     20

#define IOD_MP1_MAX_TDP_HYGX             0x5D394  /*start bit 2 - end bit 11*/
#define IOD_MP1_MAX_TDP_START_BITS_HYGX  2
#define IOD_MP1_FMAX_HYGX                0x5D388
#define IOD_MP1_FMAX_START_BITS_HYGX     8

#endif //_FUSE_H_
