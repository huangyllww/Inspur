
#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <BoardIdType.h>
#include <Library/ByoHygonGpioLib.h>
#include <HygonCpmCommon.h>
#include <HygonGpioBase.h>


//HYGON_CPM_GPIO_ITEM  gCpmGpioT0 = GPIO_DEF_V3 (0,  0, 3  ,  GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH ,  GPIO_PU_EN);


BOOLEAN LibIsGpioRecoveryMode()
{
  UINT8                IsRecovery = FALSE;
  
  switch(PcdGet8(PcdBoardIdType)){
    case TKN_BOARD_ID_TYPE_NHVTB1:
//      LibCpmSetGpio(gCpmGpioT0.Pin, gCpmGpioT0.Setting.Raw);
//      IsRecovery = !LibCpmGetGpio(gCpmGpioT0.Pin);
      break;
  }

  return IsRecovery;
}


