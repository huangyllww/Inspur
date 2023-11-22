/*******************************************************************
* 
*         Copyright (c) 2008 by Silicon Motion, Inc. (SMI)
* 
*  All rights are reserved. Reproduction or in part is prohibited
*  without the written consent of the copyright owner.
* 
*******************************************************************/
#define NSFPGA = 1 
#define UEFI = 1 
//#define DebugDriver = 1
//
//shadow buffer
//
//#undef  SHADOWFB
#define  SHADOWFB

//
//	SMI DEBUG
//
#if 1
#undef  SMI_DBG
#else
#define  SMI_DBG
#endif


#ifdef SMI_DBG
#define SMIDEBUG(...) DEBUG((EFI_D_ERROR, __VA_ARGS__));
#else
#define SMIDEBUG(...) 
#endif
