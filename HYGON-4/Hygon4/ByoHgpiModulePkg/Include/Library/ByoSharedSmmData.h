#ifndef __BYO_SHARED_SMM_DATA_H__
#define __BYO_SHARED_SMM_DATA_H__

#include <Base.h>


#define BYO_SHARED_SMM_DATA_SIGNATURE  SIGNATURE_64('S', 'H', 'A', 'R', 'E', 'D', 'A', 'T')

typedef struct {
  UINT64  Signature;
  UINT32  FlashSize;
  UINT32  ForceColdReset:1;
  UINT32  ApcbWritten:1;
  UINT32  NeedSysReset:1;
  UINT32  CbsSyncMmLibOn:1;
} BYO_SHARED_SMM_DATA;

extern BYO_SHARED_SMM_DATA   *gByoSharedSmmData;

#endif
