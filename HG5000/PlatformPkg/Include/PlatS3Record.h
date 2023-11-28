
#ifndef __PLAT_S3_RECORD_H__
#define __PLAT_S3_RECORD_H__




#define PLAT_S3_RECORD_SIGNATURE  SIGNATURE_64('_', 'P', 'L', 'A', 'T', 'S', '3', 'R')
 

typedef struct {
  UINT64  Signature;

  UINT8  SmmCommCtx[32];
  UINT32 SmmCommCtxSize;
  
} PLATFORM_S3_RECORD;



#endif                        // __PLAT_S3_RECORD_H__

