hxx项目定义了BSCPKG_SUPPORT

uefi
legacy 
uefi + legacy
来控制csm

smbios type 39




cmos bad：
	1. default language
	2. default boot order

clear cmos后setup下不恢复默认值的部分：
1. secure boot
2. bmc网络配置
3. UEFI HII配置中的修改






1. 3008升级fw
2. del键按多次会死机
3. 串口线无输出
4. linux flash tool源码是否可以提供
5. 海光上pstate和cstate的默认值，按海光推荐做。
6. usb list中过滤bmc usb
7. bios update后setup值恢复的值的范围。
8. bmc中的bios编译时间的格式问题
9. clear cmos后有概率不开机









[05]
1. 解决不上bmc固件芯片进setup后bmc页面进入有问题（改为灰选）（bug 2664）.
2. 添加板载PCIE slot使能开关。
3. setup下添加amd和nvidia显卡信息。
4. 添加部分smbios type 9信息。
5. setup pci list中过滤"Non-Essential Instrumentation"和"Other en/decryption"类型。
6. 解决错误安装了MSDM acpi table问题。
7. 隐藏setup下bmc user配置页面。
8. 添加linux和dos下的flash工具。


[06]
1. 解决relesae版本串口重定向无法工作的问题。
2. 当运行legacy SAS option rom时，关闭bmc看门狗。













http://192.168.6.20/backup/Tongfang_Hygon_Server/Tongfang_Hygon_Server_0.0.4_R41057_R_bin_20200320.zip
http://192.168.6.20/backup/Tongfang_Hygon_Server/Tongfang_Hygon_Server_0.0.4_R41057_D_bin_20200320.zip
http://192.168.6.20/backup/Tongfang_Hygon_Server/Tongfang_Hygon_Server_0.0.4_R41057_src_20200320.zip






//------------------------- 待解决问题 -------------------------









//-------------------------------------------------------------
bmc固件是在U76
bios固件是在U79
必须要优先插蓝色内存slot的才能开机


4DIE封装是
die0 2个USB 3.0 2个USB2.0 
die1 2个USB 3.0 2个USB2.0
DIE2 DIE3没有


//-------------------------------------------------------------
#IntelFrameworkModulePkg/Bus/Isa/IsaSerialDxe/IsaSerialDxe.inf
#  dpx: gEfiIsaIoProtocolGuid + PNP0501
#  out: gEfiSerialIoProtocolGuid

MdeModulePkg/Universal/SerialDxe/SerialDxe.inf	
  dpx: none
  out: gEfiSerialIoProtocolGuid

ByoModulePkg/Console/TerminalDxe/TerminalDxe.inf
  dpx: gEfiSerialIoProtocolGuid
  out: gEfiSimpleTextInProtocolGuid
       gEfiSimpleTextOutProtocolGuid
       
       
       
       


//-------------------------------------------------------------
HYGON CBS
  FCH common options
    AC power loss options
      Ac loss control        default:always off    (PcdPwrFailShadow)
      -> ProgramSpecificFchInitEnvAcpiMmio
         -> AcLossControl ((UINT8) LocalCfgPtr->HwAcpi.PwrFailShadow);





//-------------------------------------------------------------
copy /y %WORKSPACE%\Token_5000def.h %WORKSPACE%\Build\Token.h
copy /y %WORKSPACE%\Token_5000def.h %WORKSPACE%\Token.h

if /I "%PSP_MP%" == "1" (
  set PSP_XML_FILE=PspData_5000_MP.xml
) else (
  set PSP_XML_FILE=PspData_5000_ES.xml
)


copy /y %WORKSPACE%\Token_3000def.h %WORKSPACE%\Build\Token.h
copy /y %WORKSPACE%\Token_3000def.h %WORKSPACE%\Token.h

if /I "%PSP_MP%" == "1" (
  set PSP_XML_FILE=PspData_3000_MP.xml
) else (
  set PSP_XML_FILE=PspData_3000_ES.xml
)



[5000 token.h]
#define HideDieUSBHC             0, 0, 1, 1, 1, 1, 1, 1
#define PLATFORM_SELECT          13

[3000 token.h]
#define NSOCKETS	         1
#define DIMM_SLOTS_PER_SCKT      4
#define HideDieUSBHC             0, 1, 1, 1, 1, 1, 1, 1
#define PLATFORM_SELECT          11





//-------------------------------------------------------------
T1DMHG-E2 双路主板
海光 CPU 有 7100/5100 系列区分,5100 系列是在 7100 系列 4 个 Die 的基础上拿掉 2 个 Die
cpu  x 2
dimm x 32

为了满足 DDR4 信号完整性的要求（避免信号分支 stub 导致的 SI 问题），BIOS MRC 对插入
DIMM 的顺序做了规定：同一个内存通道必须先插入远离 CPU 的 DIMM,再插入靠近 CPU 的另一个
DIMM。即在 T1DMHG-E2 主板上同一个通道内存必须先插蓝色 Slot 再插黑色 Slot,同时为了性能
最优,建议先插满蓝色内存 Slot 再插黑色内存 Slot。

J41 J42 J43 J44 J45 J46 6 个 PCIE Slot 是非标 Slot,必须要经过 RISER CARD转接后才能插 
PCIE Card,禁止直接插 PCIE 卡否则会烧坏主板

每个 CPU 只集成有 4 个 USB3.0,4 个 USB2.0 信号,为了保证只上一个 CPU 时所有 USB 设备能够
使用,从设计上引入了 4 Port USB3.0 Hub。



服务器产品专案： SVID:0X1D05， SSID: 0x1E03





//-------------------------------------------------------------
1.目前的code整体是基于H620 SVN：37800
SVN: http://192.168.6.5/svn/unc/ProjectsRef/Tongfang/Hygon/Server/trunk

H620 项目的code路径如下：
SVN: http://192.168.6.5/svn/unc/ProjectsRef/Higon/H620_Normal

这一版本的code，基本的开关机，进系统之类的，没有问题；
2.	目前机器有两套，一个可以正常开机，放在实验室；另一个不能开机，现在还在我的桌子上；
3.	第一版测试版本的BIOS已经release给同泰怡，应该年后会进行测试；






//-------------------------------------------------------------
AgesaModulePkg\Nbio\SMU\AmdNbioSmuV9Dxe\AmdNbioSmuV9Dxe.c

AgesaModulePkg\Nbio\IOAPIC

PSP 串口打印：AmdABLDebug_SUPPORT




1DPC 1R 2667 2R 2400
2DPC 1R 2133 2R 1866






//--------------------------------------------------------------
  UINT8  Die0XHCIPort[4];
           [0] - 2.0
           [1] - 2.0
           [2] - 3.0
           [3] - 3.0

  UINT8  Die1XHCIPort[4];
  UINT8  Die4XHCIPort[4];
  UINT8  Die5XHCIPort[4];


  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXhciPlatformPortDisableMap|
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}|VOID*|0x0003EF36


XXXXYYYY
33332222
  1 - disable
  0 - enable

USBCONTAINERx0130 (SYSHUB::USB::CONTAINER::PORT_CONTROL)
  [7:4] 3.0 port count
  [3:0] 2.0 port count

USBCONTAINERx0128 (SYSHUB::USB::CONTAINER::PORT_DISABLE_WRITE_ONCE)
  [31:16] disable USB3 port[n]
  [15:00] disable USB2 port[n]

Note: 
The SuperSpeed Port0 (first SS port) must not be disabled if any USB2 or SuperSpeed ports are operational
The USB2.0 Port0 (first USB2 port) must not be disabled if any USB2 or SuperSpeed ports are operational










//--------------------------------------------------------------
[00] - second
[01] - second alarm
[02] - minute
[03] - minute alarm
[04] - hour
[05] - hour alarm
[06] - day of week
[07] - date of month
[08] - month
[09] - year
[0A] - 
[0B] - 
[0C] - 
[0D] - date alarm
[32] - AltCentury
[48] - century


0xFED80300 + 0x74   bit2 = 1

FCH::PM::AcpiPm1EvtBlk - PMx060 
                           [15:2] AcpiPm1EvtBlk



                           
                           

                           

//--------------------------------------------------------------                           
oneof
  varid       = CBS_CONFIG.CbsCmnFchUart0LegacyConfig,
  prompt      = STRING_TOKEN (AMD_CBS_STR_UART_0_LEGACY_OPTIONS),
  help        = STRING_TOKEN (AMD_CBS_STR_UART_0_LEGACY_OPTIONS_HELP),
  option text = STRING_TOKEN (AMD_CBS_STR_DISABLED), value = 0, flags = DEFAULT | RESET_REQUIRED | MANUFACTURING;
  option text = STRING_TOKEN (AMD_CBS_STR_0X2E8),    value = 1, flags = RESET_REQUIRED;
  option text = STRING_TOKEN (AMD_CBS_STR_0X2F8),    value = 2, flags = RESET_REQUIRED;
  option text = STRING_TOKEN (AMD_CBS_STR_0X3E8),    value = 3, flags = RESET_REQUIRED;
  option text = STRING_TOKEN (AMD_CBS_STR_0X3F8),    value = 4, flags = RESET_REQUIRED;
endoneof;


#define FCH_AL2AHBx20_LEGACY_UART_IO_ENABLE            0xFEDC0020ul 
bit0 2e8-2ef, 0 - disable, 1 - enable
bit1 2f8-2ff, 0 - disable, 1 - enable
bit2 3e8-3ff, 0 - disable, 1 - enable
bit3 3f8-3ff, 0 - disable, 1 - enable
[09:08] which uart uses 2E8-2EF,  0 - UART0, 1 - UART1, 2 - UART2, 3 - UART3
[11:10]                 2F8-2FF
[13:12]                 3E8-3EF
[15:14]                 3F8-3FF


  if (FchParams->FchRunTime.FchDeviceEnableMap & BIT11) {
    UartLegacy[0] = PcdGet8 (FchUart0LegacyEnable);              // 4
  } else {
    UartLegacy[0] = 0;
  }

  IoEnable16 = 0;
  for (UartChannel = 0; UartChannel < 4; UartChannel++ ) {
    if (UartLegacy[UartChannel]) {
      IoEnable16 |= (BIT0 << (UartLegacy[UartChannel] - 1)) + (UartChannel << (8 + ((UartLegacy[UartChannel] - 1) * 2)));
    }
  }

BIT3 + 0
  







//---------------------------------------------------------------
每个SOC的die0和die1上各有一个XHCI controller， 每个controller是包含2个2.0 port 和 2个3.0 port

die0 xhci       2.0 x 2, 3.0 x 2   (port x 2)
die1 xhci       2.0 x 2, 3.0 x 2   (port x 2)
die2
die3

die0 xhci       2.0 x 2, 3.0 x 2   (port x 2)
die1 xhci       2.0 x 2, 3.0 x 2   (port x 2)
die2
die3


--------------------------------------------------------
3.0  P[0]   v
3.0  P[1]   v
2.0  P[0]   v
2.0  P[1]   x

3.0  P[2]   x
3.0  P[3]   x
2.0  P[2]   v
2.0  P[3]   v





--------------------------------------------------------
Driver C9C30DD7-91D6-4F75-BF08-846E6020F1A8 was discovered but not loaded!!
Driver A047B50A-CB25-40B0-A27E-781FBD88E10E was discovered but not loaded!!
Driver F8673422-16DE-449C-8728-AB0361DBF9F0 was discovered but not loaded!!
Driver D1D093D6-4F63-4A3A-A900-358518711E6C was discovered but not loaded!!
Driver 0097CC3D-5333-4CF8-9572-75B1BDEB18E6 was discovered but not loaded!!

AmdNbioIOMMUZPDxe           AmdNbioIOMMUZPDxeEntry
SetupMouse                  InitializeSetupMouse
LegacyInterruptHookDxe      InitializeLegacyInterruptHook
DxeWB25Q256                 DriverEntry
DxeWB25Q128                 DriverEntry






---------------------------------------------------------
APCB - AGESA Platform Control Block

HygonWrapperPkg.inc.fdf 
AgesaSp2ZpModulePkg.pei.inc.fdf 
CbsZeppelin.pei.inc.fdf 
CpmXXX.pei.inc.fdf

AgesaSp3ZpModulePkg.dxe.inc.fdf  
CbsZeppelin.dxe.inc.fdf

AgesaPkg/Addendum/Apcb
AmdCpmPkg/Addendum/Oem

AgesaPkg/Addendum/Apcb/<Platform>/Include/ApcbCustomizedDefinitions.h 
NUMBER_OF_DIMMS_PER_CHANNEL      每个 Channel 的 DIMM 插槽数量
NUMBER_OF_CHANNEL_PER_SOCKET     每个 Socket 的内存通道数量
BLDCFG_I2C_MUX_ADDRESS           I2C mux 的地址
BLDCF_SPD_CH_#_DIMM#_ADDRESS     DIMM 插槽的 I2C 地址

HOST_TO_APCB_CHANNEL_XLAT

  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosT17Socket0ChannelADimm0Locator|"P0_DIMM_A0"|VOID*|0x00001031   #10 character string maximum  


IO Space 0x0000~0x0FFF 必须分给 die 0







//-------------------------------------------------------------------
1.Fixed system will hang by USB KBC.
  //Byosoft- {PMIO_BASE >> 8,  FCH_PMIOA_REG54, 0x00, BIT4 + BIT6},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG54, 0x00, BIT4},          /// Byosoft+



  

//-------------------------------------------------------------------
  FchInitReset (FchParams);


#define ASPEED2500_SIO_INDEX_PORT  0x4E
#define ASPEED2500_SIO_DATA_PORT   (ASPEED2500_SIO_INDEX_PORT+1)
#define ASPEED2500_SIO_UNLOCK      0xA5
#define ASPEED2500_SIO_LOCK        0xAA
#define ASPEED2500_LOGICAL_DEVICE  7


VOID
Decodeport80 (VOID)
{
  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_UNLOCK);  
  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_UNLOCK);

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_LOGICAL_DEVICE); 
  IoWrite8 (0xED, 0);//short delay.
  IoWrite8 (ASPEED2500_SIO_DATA_PORT, 0X0D);  
  IoWrite8 (0xED, 0);//short delay.

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, 0x30); 
  IoWrite8 (0xED, 0);//short delay.
  IoWrite8 (ASPEED2500_SIO_DATA_PORT, 0x1);  
  IoWrite8 (0xED, 0);//short delay. 
  
  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_LOGICAL_DEVICE); 
  IoWrite8 (0xED, 0);//short delay.
  IoWrite8 (ASPEED2500_SIO_DATA_PORT, 7);  
  IoWrite8 (0xED, 0);//short delay.
  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, 0x30); 
  IoWrite8 (0xED, 0);//short delay.
  IoWrite8 (ASPEED2500_SIO_DATA_PORT, 0x80);  
  IoWrite8 (0xED, 0);//short delay.  
  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, 0x38); 
  IoWrite8 (0xED, 0);//short delay.
  IoWrite8 (ASPEED2500_SIO_DATA_PORT, 0x0C);  
  IoWrite8 (0xED, 0);//short delay. 

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_LOCK); 
}



  

//-------------------------------------------------------------------
(41,00,00) 005D1000 (01,04,00) 00 00000000 ROM(796B6018,1AE00) DP:PciRoot(0x4)/Pci(0x1,0x1)/Pci(0x0,0x0)
(42,00,00) 66111002 (03,00,00) 00 00000000 ROM(79696018,1E400) DP:PciRoot(0x4)/Pci(0x1,0x2)/Pci(0x0,0x0)
(51,00,00) 10B98086 (02,00,00) 00 00000000 ROM(79686018,0F000) DP:PciRoot(0x5)/Pci(0x1,0x1)/Pci(0x0,0x0)





//-------------------------------------------------------------------
InstallProtocolInterface: 60FF8964-E906-41D0-AFED-F241E974E08E 0
InstallProtocolInterface: FA20568B-548B-4B2B-81EF-1BA08D4A3CEC 0
SmmLockBoxDxeLib SaveLockBox - Enter
SmmLockBoxDxeLib SaveLockBox - Exit (Success)
SmmLockBoxDxeLib SetLockBoxAttributes - Enter
SmmLockBoxDxeLib SetLockBoxAttributes - Exit (Success)
SmmLockBoxDxeLib SaveLockBox - Enter
SmmLockBoxDxeLib SaveLockBox - Exit (Success)
SmmLockBoxDxeLib SetLockBoxAttributes - Enter
SmmLockBoxDxeLib SetLockBoxAttributes - Exit (Success)

---mSmbiosEfiEntry:7B9DC000
---SMM IPL locked SMRAM window
---PlatformAfterConsoleEndHook()






//-------------------------------------------------------------------
00900F11
000F0000
0FF00000
--------------
00900000 -> 009000
         -> 009011 -> 0x9010



         


         
//-------------------------------------------------------------------         
  IDS_HDT_CONSOLE (CPU_TRACE, "  AmdCcxZenZpDxeInit Entry\n");

#define IDS_HDT_CONSOLE    IdsLibDebugPrint

  AgesaModulePkg/Ccx/Zen/CcxZenZpDxe/AmdCcxZenZpDxe.inf {
  <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesZpLib/CcxResetTablesZpLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    FabricWdtLib|AgesaModulePkg/Library/FabricWdtZpLib/FabricWdtZpLib.inf
    CcxIdsCustomPstatesLib|AgesaModulePkg/Library/CcxIdsCustomPstateNullLib/CcxIdsCustomPstatesNullLib.inf
  }

AmdIdsDebugPrintf
  PcdAmdIdsDebugPrintEnable

PcdAmdIdsDebugPrintRedirectIOEnable|FALSE
PcdAmdIdsDebugPrintHdtOutEnable|FALSE
PcdAmdIdsDebugPrintSerialPortEnable|TRUE


--->
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintEnable|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPortDetectCableConnection|FALSE



  
  
  
  
  
  
//-------------------------------------------------------------------------
HiiGetBrowserData
  InternalHiiBrowserCallback
    mUefiFormBrowser2->BrowserCallback
      BrowserCallback
        LoadAllHiiFormset      // gBeInteractiveStage && RetrieveData


ExitFormCallback
  BrowserExtension->ExecuteAction (BROWSER_ACTION_SUBMIT | BROWSER_ACTION_EXIT, 0)
    ExecuteAction
      SubmitForm
        SubmitForSystem
          gBrowserFormSetList
            SubmitForFormSet
              SubmitCallback
                SubmitCallbackForForm
                  FormSet->ConfigAccess->Callback(EFI_BROWSER_ACTION_SUBMITTED)


      
  




//-------------------------------------------------------------------------
ProcessUserInput
  ProcessAction
    ExtractDefault(gCurrentSelection->FormSet, gCurrentSelection->Form, DefaultId, gBrowserSettingScope, GetDefaultForAll, NULL, FALSE, FALSE);


ExitFormCallback
  BrowserExtension->ExecuteAction (BROWSER_ACTION_DEFAULT, 0)
    ExecuteAction
      ExtractDefault(FormSet, Form, DefaultId, gBrowserSettingScope, GetDefaultForAll, NULL, FALSE, FALSE)


      

      
      
      
//------------------------------------------------------------------------- 
[5000]     
die 0   0  - 15  (PCIE)
        16 - 31  (CPU)
die 1   32 - 47  (CPU)
        48 - 63  (PCIE)      

[7000]        
//  0 - 15     P0
// 16 - 31     xGMI
// 32 - 47     P1
// 48 - 63     xGMI
// 64 - 79     xGMI
// 80 - 95     P2
// 96 - 111    xGMI
// 112- 127    P3      
      
      
      
      
      



//-------------------------------------------------------------------------
            Name (CRS1, ResourceTemplate ()
            {
                WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,             // bus
                    0x0000,             // Granularity
                    0x0000,             // Range Minimum
                    0x007F,             // Range Maximum
                    0x0000,             // Translation Offset
                    0x0080,             // Length
                    ,, _Y01)
                    
                IO (Decode16,                                                               // Io CF8 - CFF
                    0x0CF8,             // Range Minimum
                    0x0CF8,             // Range Maximum
                    0x01,               // Alignment
                    0x08,               // Length
                    )
                    
                WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,       // Io 0 - 2FF
                    0x0000,             // Granularity
                    0x0000,             // Range Minimum
                    0x02FF,             // Range Maximum
                    0x0000,             // Translation Offset
                    0x0300,             // Length
                    ,, _Y03, TypeStatic)
                    
                WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,       // Io 300 - 3AF
                    0x0000,             // Granularity
                    0x0300,             // Range Minimum
                    0x03AF,             // Range Maximum
                    0x0000,             // Translation Offset
                    0x00B0,             // Length
                    ,, , TypeStatic)
                    
                WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,       // 3E0 - CF7
                    0x0000,             // Granularity
                    0x03E0,             // Range Minimum
                    0x0CF7,             // Range Maximum
                    0x0000,             // Translation Offset
                    0x0918,             // Length
                    ,, _Y04, TypeStatic)
                    
                WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,       // ? 0x03B0 - 3DF
                    0x0000,             // Granularity
                    0x0000,             // Range Minimum
                    0x0000,             // Range Maximum
                    0x0000,             // Translation Offset
                    0x0000,             // Length
                    ,, _Y05, TypeStatic)
                    
                WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,       // D00 - FFF   <---
                    0x0000,             // Granularity
                    0x0D00,             // Range Minimum
                    0x0FFF,             // Range Maximum
                    0x0000,             // Translation Offset
                    0x0300,             // Length
                    ,, _Y02, TypeStatic)
                    
                DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,      // A0000 - BFFFF
                    0x00000000,         // Granularity
                    0x00000000,         // Range Minimum
                    0x00000000,         // Range Maximum
                    0x00000000,         // Translation Offset
                    0x00000000,         // Length
                    ,, _Y06, AddressRangeMemory, TypeStatic)
                    
                DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                    0x00000000,         // Granularity
                    0x000C0000,         // Range Minimum
                    0x000DFFFF,         // Range Maximum
                    0x00000000,         // Translation Offset
                    0x00020000,         // Length
                    ,, , AddressRangeMemory, TypeStatic)
                    
                DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                    0x00000000,         // Granularity
                    0x02000000,         // Range Minimum
                    0xFFDFFFFF,         // Range Maximum
                    0x00000000,         // Translation Offset
                    0xFDE00000,         // Length
                    ,, _Y07, AddressRangeMemory, TypeStatic)
                    
                QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                    0x0000000000000000, // Granularity
                    0x0000000000000000, // Range Minimum
                    0x0000000000000000, // Range Maximum
                    0x0000000000000000, // Translation Offset
                    0x0000000000000000, // Length
                    ,, _Y08, AddressRangeMemory, TypeStatic)
            })



            
  