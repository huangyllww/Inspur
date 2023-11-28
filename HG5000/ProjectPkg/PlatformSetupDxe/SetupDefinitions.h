


#ifdef FORM_SET_VARSTORE

  efivarstore SETUP_DATA, 
    varid     = SETUP_DATA_ID, 
    attribute = 0x7, 
    name      = Setup, 
    guid      = PLATFORM_SETUP_VARIABLE_GUID; 
  
  efivarstore SETUP_VOLATILE_DATA,
    varid = SETUP_VOLATILE_DATA_ID,
    attribute = 0x6,
    name  = SetupVolatileData,
    guid  = PLATFORM_SETUP_VARIABLE_GUID;
    
#endif



#ifndef BOOT_FORM_SET

#include "Main/PlatformInfo.sd"
#include "Main/Memory/Memory.sd"
#include "Devices/PcieCfg/PcieCfg.sd"
#include "Main/PspInfo.sd" 
#include "Advanced/ConsoleRedirection/ConsoleRedirection.sd"
#include "Advanced/FormsetAdvanced.sd"
#include "Devices/Network/Network.sd"
#include "Devices/Video/VideoSetup.sd"
#include "Devices/AtaDriveCfg/AtaDriveCfg.sd"
//#include "Devices/NvmeCfg/NvmeCfg.sd"
#include "Devices/Usb/Usb.sd"
#include "Advanced/Virtualization/Virtualization.sd"

#include "Exit/FormsetExit.sd"

#endif

