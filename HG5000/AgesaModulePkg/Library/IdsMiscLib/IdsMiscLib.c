/**
 * @file
 *
 * AMD Integrated Debug Debug_library Routines
 *
 * Contains AMD AGESA debug macros and library functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */

 /*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Filecode.h>

#define FILECODE  LIBRARY_IDSMISCLIB_IDSMISCLIB_FILECODE
/*--------------------------------------------------------------------------------------*/
/**
 *  IDS back-end code for AGESA_TESTPOINT
 *
 *  @param[in] TestPoint  Progress indicator value, see @ref AGESA_POST_CODE
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
/*--------------------------------------------------------------------------------------*/
#define AGESA_POST_CODE_PREFIX 0xB0000000ul

#ifndef IDSOPT_NDEBUG
typedef struct _AGENSA_TESTPOINT_STRUCT{
    CHAR8 *PointName;
    UINT16 PointValue; 
}AGENSA_TESTPOINT_STRUCT;

AGENSA_TESTPOINT_STRUCT AgesaTestPointStruct[] = {
    {"TpUniversalAcpiEntry",0xA001},///<UniversalACPIentry
    {"TpUniversalAcpiExit",0xA002},///<UniversalACPIexit
    {"TpUniversalAcpiAbort",0xA003},///<UniversalACPIabort
    {"TpUniversalSmbiosEntry",0xA004},///<UniversalSMBIOSentry
    {"TpUniversalSmbiosExit",0xA005},///<UniversalSMBIOSexit
    {"TpUniversalSmbiosAbort",0xA006},///<UniversalSMBIOSabort

    //[0xA1XX]ForCZonlymemoryPostcodes
    {"TpProcMemBeforeMemDataInit",0xA101},///<Memorystructureinitialization(Publicinterface)
    {"TpProcMemBeforeSpdProcessing",0xA102},///<SPDDataprocessing(Publicinterface)
    {"TpProcMemAmdMemAuto",0xA103},///<Memoryconfiguration(Publicinterface)
    {"TpProcMemDramInit",0xA104},///<DRAMinitialization
    {"TpProcMemSPDChecking",0xA105},///<{TpProcMemSPDChecking
    {"TpProcMemModeChecking",0xA106},///<{TpProcMemModeChecking
    {"TpProcMemSpeedTclConfig",0xA107},///<SpeedandTCLconfiguration
    {"TpProcMemSpdTiming",0xA108},///<{TpProcMemSpdTiming
    {"TpProcMemDramMapping",0xA109},///<{TpProcMemDramMapping
    {"TpProcMemPlatformSpecificConfig",0xA10A},///<{TpProcMemPlatformSpecificConfig
    {"TPProcMemPhyCompensation",0xA10B},///<TPProcMemPhyCompensation
    {"TpProcMemStartDcts",0xA10C},///<{TpProcMemStartDcts
    {"TpProcMemBeforeDramInit",0xA10D},///<(Publicinterface)
    {"TpProcMemPhyFenceTraining",0xA10E},///<{TpProcMemPhyFenceTraining
    {"TpProcMemSynchronizeDcts",0xA10F},///<{TpProcMemSynchronizeDcts
    {"TpProcMemSystemMemoryMapping",0xA110},///<{TpProcMemSystemMemoryMapping
    {"TpProcMemMtrrConfiguration",0xA111},///<{TpProcMemMtrrConfiguration
    {"TpProcMemDramTraining",0xA112},///<{TpProcMemDramTraining
    {"TpProcMemBeforeAnyTraining",0xA113},///<(Publicinterface)
    {"TpProcMemWriteLevelizationTraining",0xA114},///<{TpProcMemWriteLevelizationTraining
    {"TpProcMemWlFirstPass",0xA115},///<Below800Mhzfirstpassstart
    {"TpProcMemWlSecondPass",0xA116},///<Above800Mhzsecondpassstart
    {"TpProcMemWlTrainTargetDimm",0xA117},///<TargetDIMMconfigured
    {"TpProcMemWlPrepDimms",0xA118},///<PrepareDIMMSforWL
    {"TpProcMemWlConfigDimms",0xA119},///<ConfigureDIMMSforWL
    {"TpProcMemReceiverEnableTraining",0xA11A},///<{TpProcMemReceiverEnableTraining
    {"TpProcMemRcvrStartSweep",0xA11B},///<Startsweeploop
    {"TpProcMemRcvrSetDelay",0xA11C},///<SetreceiverDelay
    {"TpProcMemRcvrWritePattern",0xA11D},///<Writetestpattern
    {"TpProcMemRcvrReadPattern",0xA11E},///<Readtestpattern
    {"TpProcMemRcvrTestPattern",0xA11F},///<Comparetestpattern
    {"TpProcMemRcvrCalcLatency",0xA120},///<CalculateMaxRdLatencyperchannel
    {"TpProcMemReceiveDqsTraining",0xA121},///<{TpProcMemReceiveDqsTraining
    {"TpProcMemRcvDqsSetDelay",0xA122},///<SetWriteDatadelay
    {"TpProcMemRcvDqsWritePattern",0xA123},///<Writetestpattern
    {"TpProcMemRcvDqsStartSweep",0xA124},///<Startreadsweep
    {"TpProcMemRcvDqsSetRcvDelay",0xA125},///<SetReceiveDQSdelay
    {"TpProcMemRcvDqsReadPattern",0xA126},///<ReadTestpattern
    {"TpProcMemRcvDqsTstPattern",0xA127},///<CompareTestpattern
    {"TpProcMemRcvDqsResults",0xA128},///<Updateresults
    {"TpProcMemRcvDqsFindWindow",0xA129},///<StartFindpassingwindow
    {"TpProcMemTransmitDqsTraining",0xA12A},///<{TpProcMemTransmitDqsTraining
    {"TpProcMemTxDqStartSweep",0xA12B},///<Startwritesweep
    {"TpProcMemTxDqSetDelay",0xA12C},///<SetTransmitDQdelay
    {"TpProcMemTxDqWritePattern",0xA12D},///<Writetestpattern
    {"TpProcMemTxDqReadPattern",0xA12E},///<ReadTestpattern
    {"TpProcMemTxDqTestPattern",0xA12F},///<CompareTestpattern
    {"TpProcMemTxDqResults",0xA130},///<Updateresults
    {"TpProcMemTxDqFindWindow",0xA131},///<StartFindpassingwindow
    {"TpProcMemMaxRdLatencyTraining",0xA132},///<{TpProcMemMaxRdLatencyTraining
    {"TpProcMemMaxRdLatStartSweep",0xA133},///<Startsweep
    {"TpProcMemMaxRdLatSetDelay",0xA134},///<Setdelay
    {"TpProcMemMaxRdLatWritePattern",0xA135},///<Writetestpattern
    {"TpProcMemMaxRdLatReadPattern",0xA136},///<ReadTestpattern
    {"TpProcMemMaxRdLatTestPattern",0xA137},///<CompareTestpattern
    {"TpProcMemOnlineSpareInit",0xA138},///<OnlineSpareinit
    {"TpProcMemBankInterleaveInit",0xA139},///<BankInterleaveInit
    {"TpProcMemNodeInterleaveInit",0xA13A},///<NodeInterleaveInit
    {"TpProcMemChannelInterleaveInit",0xA13B},///<ChannelInterleaveInit
    {"TpProcMemEccInitialization",0xA13C},///<ECCinitialization
    {"TpProcMemPlatformSpecificInit",0xA13D},///<PlatformSpecificInit
    {"TpProcMemBeforeAgesaReadSpd",0xA13E},///<Beforecalloutfor"AgesaReadSpd"
    {"TpProcMemAfterAgesaReadSpd",0xA13F},///<Aftercalloutfor"AgesaReadSpd"
    {"TpProcMemBeforeAgesaHookBeforeDramInit",0xA140},///<Beforeoptionalcallout"AgesaHookBeforeDramInit"
    {"TpProcMemAfterAgesaHookBeforeDramInit",0xA141},///<Afteroptionalcallout"AgesaHookBeforeDramInit"
    {"TpProcMemBeforeAgesaHookBeforeDQSTraining",0xA142},///<Beforeoptionalcallout"AgesaHookBeforeDQSTraining"
    {"TpProcMemAfterAgesaHookBeforeDQSTraining",0xA143},///<Afteroptionalcallout"AgesaHookBeforeDQSTraining"
    {"TpProcMemBeforeAgesaHookBeforeExitSelfRef",0xA144},///<Beforeoptionalcallout"AgesaHookBeforeDramInit"
    {"TpProcMemAfterAgesaHookBeforeExitSelfRef",0xA145},///<Afteroptionalcallout"AgesaHookBeforeDramInit"
    {"TpProcMemAfterMemDataInit",0xA146},///<AfterMemDataInit
    {"TpProcMemInitializeMCT",0xA147},///<BeforeInitializeMCT
    {"TpProcMemLvDdr3",0xA148},///<BeforeLVDDR3
    {"TpProcMemInitMCT",0xA149},///<BeforeInitMCT
    {"TpProcMemOtherTiming",0xA14A},///<BeforeOtherTiming
    {"TpProcMemUMAMemTyping",0xA14B},///<BeforeUMAMemTyping
    {"TpProcMemSetDqsEccTmgs",0xA14C},///<BeforeSetDqsEccTmgs
    {"TpProcMemMemClr",0xA14D},///<BeforeMemClr
    {"TpProcMemOnDimmThermal",0xA14E},///<BeforeOnDIMMThermal
    {"TpProcMemDmi",0xA14F},///<BeforeDMI
    {"TpProcMemEnd",0xA150},///<Endofmemorycode
    {"TpProcS3Init",0xA151},///<EntrypointS3Init
    {"TpProcMemSendMRS2",0xA180},///<SendingMRS2
    {"TpProcMemSendMRS3",0xA181},///<SeddingMRS3
    {"TpProcMemSendMRS1",0xA182},///<SendingMRS1
    {"TpProcMemSendMRS0",0xA183},///<SendingMRS0
    {"TpProcMemContinPatternGenRead",0xA184},///<ContinuousPatternRead
    {"TpProcMemContinPatternGenWrite",0xA185},///<ContinuousPatternWrite
    {"TpProcMem2dRdDqsTraining",0xA186},///<Mem:2dRdDqsTrainingbegin
    {"TpProcMemBefore2dTrainExtVrefChange",0xA187},///<Mem:BeforeoptionalcallouttoplatformBIOStochangeExternalVrefduring2dTraining
    {"TpProcMemAfter2dTrainExtVrefChange",0xA188},///<Mem:AfteroptionalcallouttoplatformBIOStochangeExternalVrefduring2dTraining
    {"TpProcMemConfigureDCTForGeneral",0xA189},///<ConfigureDCTForGeneralusebegin
    {"TpProcMemProcConfigureDCTForTraining",0xA18A},///<ConfigureDCTFortrainingbegin
    {"TpProcMemConfigureDCTNonExplicitSeq",0xA18B},///<ConfigureDCTForNon-Explicit
    {"TpProcMemSynchronizeChannels",0xA18C},///<ConfiguretoSyncchannels
    {"TpProcMemC6StorageAllocation",0xA18D},///<AllocateC6Storage
    {"TpProcMemLvDdr4",0xA18E},///<BeforeLVDDR4

    //BRCPU
    {"TpBrBeforeApLaunch",0xA190},///<BRbeforeAPlaunch
    {"TpBrInstallApLaunchedPpi",0xA191},///<InstallAPlaunchedPPI
    {"TpBrAfterApLaunch",0xA192},///<BRafterAPlaunch
    {"TpBrBeforePM",0xA193},///<BeforeCPUPM
    {"TpBrIoCstate",0xA194},///<EnableIOCstate
    {"TpBrC6",0xA195},///<EnableC6
    {"TpBrInstallCcxPeiCompletePpi",0xA196},///<InstallCCXPEIcompletePPI
    {"TpBrMemoryDoenCallBackEntry",0xA197},///<BRCPUmemorydonecallbackentry
    {"TpBrBeforeApmWeights",0xA198},///<BeforeAPMweights
    {"TpBrAfterApmWeights",0xA199},///<AfterAPMweights
    {"TpBrMemoryDoneCallBackEnd",0xA19A},///<BRCPUmemorydonecallbackend
    {"TpBrInitMidEntry",0xA19B},///<BRInitMidentry
    {"TpBrApm",0xA19C},///<BRenableAPM
    {"TpBrInitMidInstallProtocol",0xA19D},///<BRInitMidinstallprotocol
    {"TpBrInitMidEnd",0xA19E},///<BRInitMidend
    {"TpBrInitLateEntry",0xA19F},///<BRInitLateentry
    {"TpBrInitLateInstallProtol",0xA1A0},///<BRInitLateinstallprotocol
    {"TpBrInitLateEnd",0xA1A1},///<BRInitLateend
    {"TpBrDxeInstallCompleteProtocol",0xA1A2},///<BRDXEinstallcompleteprotocol
    {"TpUnbInstallCompletePpi",0xA1A3},///<UNBinstallcompletePPI
    {"TpUnbAfterApLaunchCallbackEntry",0xA1A4},///<UNBAfterApLaunchcallbackentry
    {"TpUnbAfterApLaunchCallbackEnd",0xA1A5},///<UNBAfterApLaunchcallbackend

    //S3InterfacePostCode
    {"TpIfBeforeAllocateS3SaveBuffer",0xA1EC},///<BeforetheS3savecodecallsouttoallocateabuffer
    {"TpIfAfterAllocateS3SaveBuffer",0xA1ED},///<AftertheS3savecodecallsouttoallocateabuffer
    {"TpIfBeforeAllocateMemoryS3SaveBuffer",0xA1EE},///<BeforethememoryS3savecodecallsouttoallocateabuffer
    {"TpIfAfterAllocateMemoryS3SaveBuffer",0xA1EF},///<AfterthememoryS3savecodecallsouttoallocateabuffer
    {"TpIfBeforeLocateS3PciBuffer",0xA1F0},///<Beforethememorycodecallsouttolocateabuffer
    {"TpIfAfterLocateS3PciBuffer",0xA1F1},///<Afterthememorycodecallsouttolocateabuffer
    {"TpIfBeforeLocateS3CPciBuffer",0xA1F2},///<Beforethememorycodecallsouttolocateabuffer
    {"TpIfAfterLocateS3CPciBuffer",0xA1F3},///<Afterthememorycodecallsouttolocateabuffer
    {"TpIfBeforeLocateS3MsrBuffer",0xA1F4},///<Beforethememorycodecallsouttolocateabuffer
    {"TpIfAfterLocateS3MsrBuffer",0xA1F5},///<Afterthememorycodecallsouttolocateabuffer
    {"TpIfBeforeLocateS3CMsrBuffer",0xA1F6},///<Beforethememorycodecallsouttolocateabuffer
    {"TpIfAfterLocateS3CMsrBuffer",0xA1F7},///<Afterthememorycodecallsouttolocateabuffer
    //PMUPostCode
    {"TpProcMemPmuFailed",0xA1F9},///<FailedPMUtraining.

    //[0xA2XX~0xA4XX]Reserved

    //[0xA5XX]assignedforAGESAPSPModule
    //PSPV1Modules
    {"TpPspPeiV1Entry",0xA501},///<PspPeiV1entry
    {"TpPspPeiV1Exit",0xA502},///<PspPeiV1exit
    {"TpPspPeiV1MemDiscoveredPpiCallbackEntry",0xA503},///<MemoryDiscoveredPpiCallbackentry
    {"TpPspPeiV1MemDiscoveredPpiCallbackExit",0xA504},///<MemoryDiscoveredPpiCallbackexit
    {"TpPspDxeV1Entry",0xA507},///<PspDxeV1entry
    {"TpPspDxeV1Exit",0xA508},///<PspDxeV1exit
    {"TpPspDxeV1PciCallbackEntry",0xA50A},///<PspDxeV1PspPciEnumerationCompleteCallBackentry
    {"TpPspDxeV1PciCallbackExit",0xA50B},///<PspDxeV1PspPciEnumerationCompleteCallBackexit
    {"TpPspDxeV1RTBCallbackEntry",0xA50C},///<PspDxeV1readytobootentry
    {"TpPspDxeV1RTBCallbackExit",0xA50D},///<PspDxeV1readytobootexit
    {"TpPspSmmV1Entry",0xA50E},///<PspSmmV1entry
    {"TpPspSmmV1Exit",0xA50F},///<PspSmmV1exit
    {"TpPspSmmV1SwSmiCallbackEntry",0xA510},///<PspSmmV1SwSmiCallBackentry},buildtheS3saveareaforresume
    {"TpPspSmmV1SwSmiCallbackExit",0xA511},///<PspSmmV1SwSmiCallBackexit",buildtheS3saveareaforresume
    {"TpPspSmmV1BspRsmEntry",0xA512},///<PspSmmV1BspSmmResumeVectorentry
    {"TpPspSmmV1BspRsmExit",0xA513},///<PspSmmV1BspSmmResumeVectorexit
    {"TpPspSmmV1ApRsmEntry",0xA514},///<PspSmmV1ApSmmResumeVectorentry
    {"TpPspSmmV1ApRsmExit",0xA515},///<PspSmmV1ApSmmResumeVectorexit
    {"TpPspP2CmboxV1Entry",0xA516},///<PspP2CmboxV1entry
    {"TpPspP2CmboxV1Exit",0xA517},///<PspP2CmboxV1exit
    //PSPV2Modules
    {"TpPspPeiV2Entry",0xA521},///<PspPeiV2entry
    {"TpPspPeiV2Exit",0xA522},///<PspPeiV2exit
    {"TpPspDxeV2Entry",0xA523},///<PspDxeV2entry
    {"TpPspDxeV2Exit",0xA524},///<PspDxeV2exit
    {"TpPspDxeV2MpCallBackEntry",0xA525},///<PspDxeV2PspMpServiceCallBackentry
    {"TpPspDxeV2MpCallBackExit",0xA526},///<PspDxeV2PspMpServiceCallBackexit
    {"TpPspDxeV2FlashAccCallBackEntry",0xA527},///<PspDxeV2FlashAccCallBackentry
    {"TpPspDxeV2FlashAccCallBackExit",0xA528},///<PspDxeV2FlashAccCallBackexit
    {"TpPspDxeV2RTBCallbackEntry",0xA529},///<PspDxeV2readytobootentry
    {"TpPspDxeV2RTBCallbackExit",0xA52A},///<PspDxeV2readytobootexit
    {"TpPspDxeV2ExitBsCallbackEntry",0xA52B},///<PspDxeV2exitbootserivceentry
    {"TpPspDxeV2ExitBsCallbackExit",0xA52C},///<PspDxeV2exitbootserivceexit
    {"TpPspSmmV2Entry",0xA52D},///<PspSmmV2entry
    {"TpPspSmmV2Exit",0xA52E},///<PspSmmV2exit
    {"TpPspSmmV2SwSmiCallbackEntry",0xA52F},///<PspSmmV2SwSmiCallBackentry},buildtheS3saveareaforresume
    {"TpPspSmmV2SwSmiCallbackExit",0xA530},///<PspSmmV2SwSmiCallBackexit},buildtheS3saveareaforresume
    {"TpPspSmmV2BspRsmEntry",0xA531},///<PspSmmV2BspSmmResumeVectorentry
    {"TpPspSmmV2BspRsmExit",0xA532},///<PspSmmV2BspSmmResumeVectorexit
    {"TpPspSmmV2ApRsmEntry",0xA533},///<PspSmmV2ApSmmResumeVectorentry
    {"TpPspSmmV2ApRsmExit",0xA534},///<PspSmmV2ApSmmResumeVectorexit
    {"TpPspP2CmboxV2Entry",0xA535},///<PspP2CmboxV2entry
    {"TpPspP2CmboxV2Exit",0xA536},///<PspP2CmboxV2exit
    {"TpPspRecoverApcbFail",0xA537},///<{"TpPspRecoverApcbFail
    //PSPfTpmmodules
    {"TpPspfTpmPeiEntry",0xA540},///<PspfTpmPeientry
    {"TpPspfTpmPeiExit",0xA541},///<PspfTpmPeiexit
    {"TpPspfTpmPeiMemCallbackEntry",0xA542},///<PspfTpmPeimemorycallbackentry
    {"TpPspfTpmPeiMemCallbackExit",0xA543},///<PspfTpmPeimemorycallbackexit
    {"TpPspfTpmDxeEntry",0xA544},///<PspfTpmDxeentry
    {"TpPspfTpmDxeExit",0xA545},///<PspfTpmDxeexit
    //P2CmailboxHandling[0xA59X]
    {"TpPspP2CmboxSpiGetAttribEntry",0xA591},///<PspP2CmboxCommandSpiGetAttribHandlingentry
    {"TpPspP2CmboxSpiSetAttribEntry",0xA592},///<PspP2CmboxCommandSpiSetAttribHandlingentry
    {"TpPspP2CmboxSpiGetBlockSizeEntry",0xA593},///<PspP2CmboxCommandSpiGetBlockSizeHandlingentry
    {"TpPspP2CmboxSpiReadFVEntry",0xA594},///<PspP2CmboxCommandSpiReadFVHandlingentry
    {"TpPspP2CmboxSpiWriteFVEntry",0xA595},///<PspP2CmboxCommandSpiWriteFVHandlingentry
    {"TpPspP2CmboxSpiEraseFVEntry",0xA596},///<PspP2CmboxCommandSpiEraseFVHandlingentry
    {"TpPspP2CmboxCmdExit",0xA59E},///<PspP2CmboxCommandHandlingexit
    {"TpPspP2CmboxCmdFailExit",0xA59F},///<PspP2CmboxCommandHandlingFailexit
    //C2PmailboxHandling
    {"TpPspC2PmboxBeforeSendCmdBase",0xA5B0},///<PSPC2Pmailboxentrybase[0xA5BX|Cmd]
    {"TpPspC2PmboxBeforeSendCmdDramInfo",0xA5B1},///<BeforesendC2PcommandMboxBiosCmdDramInfo
    {"TpPspC2PmboxBeforeSendCmdSmmInfo",0xA5B2},///<BeforesendC2PcommandMboxBiosCmdSmmInfo
    {"TpPspC2PmboxBeforeSendCmdSxInfo",0xA5B3},///<BeforesendC2PcommandMboxBiosCmdSleepSxInfo
    {"TpPspC2PmboxBeforeSendCmdRsmInfo",0xA5B4},///<BeforesendC2PcommandMboxBiosCmdRsmInfo
    {"TpPspC2PmboxBeforeSendCmdQueryCap",0xA5B5},///<BeforesendC2PcommandMboxBiosCmdQueryCap
    {"TpPspC2PmboxBeforeSendCmdBootDone",0xA5B6},///<BeforesendC2PcommandMboxBiosCmdBootDone
    {"TpPspC2PmboxBeforeSendCmdClearS3Sts",0xA5B7},///<BeforesendC2PcommandMboxBiosCmdClearS3Sts
    {"TpPspC2PmboxBeforeSendCmdS3DataInfo",0xA5B8},///<BeforesendC2PcommandMboxBiosCmdS3DataInfo
    {"TpPspC2PmboxBeforeSendCmdNop",0xA5B9},///<BeforesendC2PcommandMboxBiosCmdNop
    {"TpPspC2PmboxBeforeSendCmdHSTIQuery",0xA5C4},///<BeforesendC2PcommandMboxBiosCmdHSTIQuery
    {"TpPspC2PmboxBeforeSendCmdClrSmmLock",0xA5C7},///<BeforesendC2PcommandMboxBiosCmdClrSmmLock
    {"TpPspC2PmboxBeforeSendCmdPcieInfo",0xA5C8},///<BeforesendC2PcommandMboxBiosCmdPcieInfo
    {"TpPspC2PmboxBeforeSendCmdGetVersion",0xA5C9},///<BeforesendC2PcommandMboxBiosCmdGetVersion

    {"TpPspC2PmboxWaitCmdBase",0xA5D0},///<PSPC2Pmailboxexitbase[0xA5DX|Cmd]
    {"TpPspC2PmboxWaitCmdDramInfo",0xA5D1},///<WaitC2PcommandMboxBiosCmdDramInfofinished
    {"TpPspC2PmboxWaitCmdSmmInfo",0xA5D2},///<WaitC2PcommandMboxBiosCmdSmmInfofinished
    {"TpPspC2PmboxWaitCmdSxInfo",0xA5D3},///<WaitC2PcommandMboxBiosCmdSleepSxInfofinished
    {"TpPspC2PmboxWaitCmdRsmInfo",0xA5D4},///<WaitC2PcommandMboxBiosCmdRsmInfofinished
    {"TpPspC2PmboxWaitCmdQueryCap",0xA5D5},///<WaitC2PcommandMboxBiosCmdQueryCapfinished
    {"TpPspC2PmboxWaitCmdBootDone",0xA5D6},///<WaitC2PcommandMboxBiosCmdBootDonefinished
    {"TpPspC2PmboxWaitCmdClearS3Sts",0xA5D7},///<WaitC2PcommandMboxBiosCmdClearS3Stsfinished
    {"TpPspC2PmboxWaitCmdS3DataInfo",0xA5D8},///<WaitC2PcommandMboxBiosCmdS3DataInfofinished
    {"TpPspC2PmboxWaitCmdNop",0xA5D9},///<WaitC2PcommandMboxBiosCmdNopfinished
    {"TpPspC2PmboxWaitCmdHSTIQuery",0xA5E4},///<WaitC2PcommandMboxBiosCmdHSTIQueryfinished
    {"TpPspC2PmboxWaitCmdClrSmmLock",0xA5C7},///<WaitC2PcommandMboxBiosCmdClrSmmLockfinished
    {"TpPspC2PmboxWaitCmdPcieInfo",0xA5C8},///<WaitC2PcommandMboxBiosCmdPcieInfofinished
    {"TpPspC2PmboxWaitCmdGetVersion",0xA5E9},///<WaitC2PcommandMboxBiosCmdGetVersionfinished
    //fTPMcommandHandling[0xA5FX]
    {"TpPspfTpmSendCmdEntry",0xA5F0},///<PspfTpmsendTPMcommandentry
    {"TpPspfTpmSendCmdExit",0xA5F1},///<PspfTpmsendTPMcommandexit
    {"TpPspfTpmRcvCmdEntry",0xA5F2},///<PspfTpmreceiveTPMcommandentry
    {"TpPspfTpmRcvCmdExit",0xA5F3},///<PspfTpmreceiveTPMcommandexit


    //[0xA6XX~0xA8XX]Reserved

    //[0xA9XX},0xAAXX]assignedforAGESANBIOModule
    //[0xA900",0xA94F]NBIOPEIM/DXEDriver
    //NbioBase
    {"TpNbioBasePeiEntry",0xA900},///<AmdNbioBasePEIMdriverentry
    {"TpNbioBasePeiExit",0xA901},///<AmdNbioBasePEIMdriverexit
    {"TpNbioBaseDxeEntry",0xA902},///<AmdNbioBaseDXEdriverentry
    {"TpNbioBaseDxeExit",0xA903},///<AmdNbioBaseDXEdriverexit
    //PCIe
    {"TpNbioPCIePeiEntry",0xA904},///<AmdNbioPciePEIMdriverentry
    {"TpNbioPCIePeiExit",0xA905},///<AmdNbioPciePEIMdriverexit
    {"TpNbioPCIeDxeEntry",0xA906},///<AmdNbioPcieDXEdriverentry
    {"TpNbioPCIeDxeExit",0xA907},///<AmdNbioPcieDXEdriverexit
    //GFX
    {"TpNbioGfxPeiEntry",0xA908},///<AmdNbioGfxPEIMdriverentry
    {"TpNbioGfxPeiExit",0xA909},///<AmdNbioGfxPEIMdriverexit
    {"TpNbioGfxDxeEntry",0xA90A},///<AmdNbioGfxDXEdriverentry
    {"TpNbioGfxDxeExit",0xA90B},///<AmdNbioGfxDXEdriverexit
    //IOMMU
    {"TpNbioIommuDxeEntry",0xA90C},///<AmdNbioIommuDXEdriverentry
    {"TpNbioIommuDxeExit",0xA90D},///<AmdNbioIommuDXEdriverexit
    //ALIB
    {"TpNbioAlibDxeEntry",0xA90E},///<AmdNbioALIBDXEdriverentry
    {"TpNbioAlibDxeExit",0xA90F},///<AmdNbioALIBDXEdriverexit
    //SMU
    {"TpNbioSmuV8PeiEntry",0xA910},///<AmdSmuV8PEIMdriverentry
    {"TpNbioSmuV8PeiExit",0xA911},///<AmdSmuV8PEIMdriverexit
    {"TpNbioSmuV8DxeEntry",0xA912},///<AmdSmuV8DXEdriverentry
    {"TpNbioSmuV8DxeExit",0xA913},///<AmdSmuV8DXEdriverexit
    {"TpNbioSmuV9PeiEntry",0xA914},///<AmdSmuV9PEIMdriverentry
    {"TpNbioSmuV9PeiExit",0xA915},///<AmdSmuV9PEIMdriverexit
    {"TpNbioSmuV9DxeEntry",0xA916},///<AmdSmuV9DXEdriverentry
    {"TpNbioSmuV9DxeExit",0xA917},///<AmdSmuV9DXEdriverexit
    {"TpNbioSmuV10PeiEntry",0xA918},///<AmdSmuV10PEIMdriverentry
    {"TpNbioSmuV10PeiExit",0xA919},///<AmdSmuV10PEIMdriverexit
    {"TpNbioSmuV10DxeEntry",0xA91A},///<AmdSmuV10DXEdriverentry
    {"TpNbioSmuV10DxeExit",0xA91B},///<AmdSmuV10DXEdriverexit

    //IOMMUPEIM
    {"TpNbioIommuPEIEntry",0xA920},///<AmdNbioIommuPEIMdriverentry
    {"TpNbioIommuPEIExit",0xA921},///<AmdNbioIommuPEIMdriverexit

    //APCBDXE
    {"TpApcbDxeEntry",0xA922},///<APCBDXEEntry
    {"TpApcbDxeExit",0xA923},///<APCBDXEExit

    //APCBSMM
    {"TpApcbSmmEntry",0xA924},///<APCBSMMEntry
    {"TpApcbSmmExit",0xA925},///<APCBSMMExit

    //[0xA950},0xA99F]NBIOPPI/PROTOCOLCallback
    {"TpNbioTopologyConfigureCallbackEntry",0xA950},///<NbioTopologyConfigureCallbackentry
    {"TpNbioTopologyConfigureCallbackExit",0xA951},///<NbioTopologyConfigureCallbackexit
    {"TpMemoryConfigDoneCallbackPpiEntry",0xA952},///<MemoryConfigDoneCallbackPpientry
    {"TpMemoryConfigDoneCallbackPpiExit",0xA953},///<MemoryConfigDoneCallbackPpiexit
    {"TpDxioInitializationCallbackPpiEntry",0xA954},///<DxioInitializationCallbackPpientry
    {"TpDxioInitializationCallbackPpiExit",0xA955},///<DxioInitializationCallbackPpiexit
    {"TpDispatchSmuV9CallbackEntry",0xA956},///<DispatchSmuV9Callbackentry
    {"TpDispatchSmuV9CallbackExit",0xA957},///<DispatchSmuV9Callbackexit
    {"TpDispatchSmuV10CallbackEntry",0xA958},///<DispatchSmuV10Callbackentry
    {"TpDispatchSmuV10CallbackExit",0xA959},///<DispatchSmuV10Callbackexit
    {"TpAmdPcieMiscInitEntry",0xA95A},///<AmdPcieMiscInitEvententry
    {"TpAmdPcieMiscInitExit",0xA95B},///<AmdPcieMiscInitEventexit
    {"TpNbioBaseHookReadyToBootEntry",0xA95C},///<NbioBaseHookReadyToBootEvententry
    {"TpNbioBaseHookReadyToBootExit",0xA95D},///<NbioBaseHookReadyToBootEventexit
    {"TpNbioBaseHookPciIOEntry",0xA95E},///<NbioBaseHookPciIOEvententry
    {"TpNbioBaseHookPciIOExit",0xA95F},///<NbioBaseHookPciIOEventexit

    //[0xA980},0xA99F]BRGNBtask
    {"TpGnbEarlyInterfaceCZEntry",0xA970},///<GnbEarlyInterfaceCZentry
    {"TpGnbEarlyInterfaceCZExit",0xA971},///<GnbEarlyInterfaceCZexit
    {"TpPcieConfigurationInitEntry",0xA972},///<PcieConfigurationInitentry
    {"TpPcieConfigurationInitExit",0xA973},///<PcieConfigurationInitexit
    {"TpGnbEarlierInterfaceCZEntry",0xA974},///<GnbEarlierInterfaceCZentry
    {"TpGnbEarlierInterfaceCZExit",0xA975},///<GnbEarlierInterfaceCZexit
    {"TpPcieEarlyInterfaceCZEntry",0xA976},///<PcieEarlyInterfaceCZentry
    {"TpPcieEarlyInterfaceCZExit",0xA977},///<PcieEarlyInterfaceCZexit
    {"TpPciePostEarlyInterfaceCZEntry",0xA978},///<PciePostEarlyInterfaceCZentry
    {"TpPciePostEarlyInterfaceCZExit",0xA979},///<PciePostEarlyInterfaceCZexit
    {"TpGfxConfigPostInterfaceCZEntry",0xA97A},///<GfxConfigPostInterfaceCZentry
    {"TpGfxConfigPostInterfaceCZExit",0xA97B},///<GfxConfigPostInterfaceCZexit
    {"TpGfxPostInterfaceCZEntry",0xA97C},///<GfxPostInterfaceCZentry
    {"TpGfxPostInterfaceCZExit",0xA97D},///<GfxPostInterfaceCZexit
    {"TpGnbPostInterfaceCZEntry",0xA97E},///<GnbPostInterfaceCZentry
    {"TpGnbPostInterfaceCZExit",0xA97F},///<GnbPostInterfaceCZexit
    {"TpPciePostInterfaceCZEntry",0xA980},///<PciePostInterfaceCZentry
    {"TpPciePostInterfaceCZExit",0xA981},///<PciePostInterfaceCZexit

    {"TpGnbEnvInterfaceCZEntry",0xA982},///<GnbEnvInterfaceCZentry
    {"TpGnbEnvInterfaceCZExit",0xA983},///<GnbEnvInterfaceCZexit
    {"TpGfxConfigEnvInterfaceEntry",0xA984},///<GfxConfigEnvInterfaceentry
    {"TpGfxConfigEnvInterfaceExit",0xA985},///<GfxConfigEnvInterfaceexit
    {"TpGfxEnvInterfaceCZEntry",0xA986},///<GfxEnvInterfaceCZentry
    {"TpGfxEnvInterfaceCZExit",0xA987},///<GfxEnvInterfaceCZexit
    {"TpGfxMidInterfaceCZEntry",0xA988},///<GfxMidInterfaceCZentry
    {"TpGfxMidInterfaceCZExit",0xA989},///<GfxMidInterfaceCZexit
    {"TpGfxIntInfoTableInterfaceCZEntry",0xA98A},///<GfxIntInfoTableInterfaceCZentry
    {"TpGfxIntInfoTableInterfaceCZExit",0xA98B},///<GfxIntInfoTableInterfaceCZexit
    {"TpPcieMidInterfaceCZEntry",0xA98C},///<PcieMidInterfaceCZentry
    {"TpPcieMidInterfaceCZExit",0xA98D},///<PcieMidInterfaceCZexit
    {"TpGnbMidInterfaceCZEntry",0xA98E},///<GnbMidInterfaceCZentry
    {"TpGnbMidInterfaceCZExit",0xA98F},///<GnbMidInterfaceCZexit
    {"TpGnbSmuMidInterfaceCZEntry",0xA990},///<GnbSmuMidInterfaceCZentry
    {"TpGnbSmuMidInterfaceCZExit",0xA991},///<GnbSmuMidInterfaceCZexit
    {"TpInvokeAmdInitLateEntry",0xA992},///<InvokeAmdInitLateentry
    {"TpInvokeAmdInitLateExit",0xA993},///<InvokeAmdInitLateexit
    {"TpGnbSmuServiceRequestV8Entry",0xA994},///<GnbSmuServiceRequestV8entry
    {"TpGnbSmuServiceRequestV8Exit",0xA995},///<GnbSmuServiceRequestV8exit

    //[0xABXX]Reserved

    //[0xACXX]assignedforAGESACCXModule
    {"TpCcxIdsAfterApLaunch",0xAC10},///<CCXIDSIDS_HOOK_CCX_AFTER_AP_LAUNCH

    {"TpCcxPeiEntry",0xAC50},///<CCXPEIentry
    {"TpCcxPeiDownCoreCallbackEntry",0xAC51},///<CCXdowncoreentry
    {"TpCcxDxeEntry",0xAC55},///<CCXDXEentry
    {"TpCcxDxeMpCallbackEntry",0xAC56},///<CCXMPservicecallbackentry
    {"TpCcxDxeRtbCallBackEntry",0xAC57},///<CCXReadToBootcallbackentry
    {"TpCcxSmmEntry",0xAC5D},///<CCXSMMentry


    {"TpCcxPeiStartLaunchApsForS3",0xAC70},///<CCXPEIstarttolaunchAPsforS3
    {"TpCcxPeiEndLaunchApsForS3",0xAC71},///<CCXPEIendoflaunchingAPsforS3

    {"TpCcxDxeStartLaunchAp",0xAC90},///<CCXstarttolaunchAP
    {"TpCcxDxeEndLaunchAp",0xAC91},///<CCXlaunchAPisended
    {"TpCcxDxeLaunchApAbort",0xAC92},///<CCXlaunchAPabort
    {"TpCcxDxeMpCallbackAbort",0xAC93},///<CCXMPserviceabort
    {"TpCcxDxeCacWeights",0xAC94},///<CCXcacweights

    {"TpCcxPeiExit",0xACE0},///<CCXPEIexit
    {"TpCcxPeiDownCoreCallbackExit",0xACE1},///<CCXdowncoreexit
    {"TpCcxDxeExit",0xACE5},///<CCXDXEexit
    {"TpCcxDxeMpCallbackExit",0xACE6},///<CCXMPservicecallbackexit
    {"TpCcxDxeRtbCallBackExit",0xACE7},///<CCXReadToBootcallbackexit
    {"TpCcxSmmExit",0xACED},///<CCXSMMexit

    //[0xADXX]assignedforAGESADFModule
    {"TpDfPeiEntry",0xAD50},///<DFPEIentry
    {"TpDfDxeEntry",0xAD55},///<DFDXEentry
    {"TpDfReadyToBootEntry",0xAD56},///<DFReadytoBootentry
    {"TpFabricSmmEntry", 0xAD59}, ///FabricSmmEntry

    {"TpDfPeiExit",0xADE0},///<DFPEIexit
    {"TpDfDxeExit",0xADE5},///<DFDXEexit
    {"TpDfReadyToBootExit",0xADE6},///<DFReadytoBootexit
    {"TpFabricSmmExit", 0xADE9},///FabricSmmExit


    //[0xAEXX]Reserved
    //[0xAFXX]assignedforAGESAFCHModule
    ///@todoRemoveunusedFCHPCs
    {"TpFchInitResetDispatching",0xAF01},///<FCHInitResetdispatchpoint
    {"TpFchInitEnvDispatching",0xAF06},///<FCHInitEnvdispatchpoint
    {"TpFchInitMidDispatching",0xAF07},///<FCHInitMiddispatchpoint
    {"TpFchInitLateDispatching",0xAF08},///<FCHInitLatedispatchpoint
    {"TpFchInitS3EarlyDispatching",0xAF0B},///<FCHInitS3Earlydispatchpoint
    {"TpFchInitS3LateDispatching",0xAF0C},///<FCHInitS3Latedispatchpoint
    {"TpFchInitS3EarlyDispatchingEnd",0xAF0D},///<FCHInitS3Earlydispatchfinished
    {"TpFchInitS3LateDispatchingEnd",0xAF0E},///<FCHInitS3Latedispatchfinished
    {"TpFchPeiEntry",0xAF10},///<FCHPeiEntry
    {"TpFchPeiExit",0xAF11},///<FCHPeiExit
    {"TpFchMultiFchPeiEntry",0xAF12},///<FCHMultiFchPeiEntry
    {"TpFchMultiFchPeiExit",0xAF13},///<FCHMultiFchPeiExit
    {"TpFchDxeEntry",0xAF14},///<FCHDxeEntry
    {"TpFchDxeExit",0xAF15},///<FCHDxeExit
    {"TpFchMultiFchDxeEntry",0xAF16},///<FCHMultiFchDxeEntry
    {"TpFchMultiFchDxeExit",0xAF17},///<FCHMultiFchDxeExit
    {"TpFchSmmEntry",0xAF18},///<FCHSmmEntry
    {"TpFchSmmExit",0xAF19},///<FCHSmmExit
    {"TpFchSmmDispatcherEntry",0xAF20},///<FCHSmmDispatcherEntry
    {"TpFchSmmDispatcherExit",0xAF21},///<FCHSmmDispatcherExit
    {"TpFchInitResetHwAcpi",0xAF40},///<FCHInitResetHwAcpi
    {"TpFchInitResetAb",0xAF41},///<FCHInitResetABLink
    {"TpFchInitResetLpc",0xAF42},///<FCHInitResetLPC
    {"TpFchInitResetSpi",0xAF43},///<FCHInitResetSPI
    {"TpFchInitResetEspi",0xAF44},///<FCHInitReseteSPI
    {"TpFchInitResetSd",0xAF45},///<FCHInitResetSD
    {"TpFchInitResetEmmc",0xAF46},///<FCHInitReseteMMC
    {"TpFchInitResetSata",0xAF47},///<FCHInitResetSATA
    {"TpFchInitResetUsb",0xAF48},///<FCHInitResetUSB
    {"TpFchInitResetXgbe",0xAF49},///<FCHInitResetxGbE
    {"TpFchInitResetHwAcpiP",0xAF4F},///<FCHInitResetHwAcpiP
    {"TpFchInitEnvHwAcpi",0xAF50},///<FCHInitEnvHwAcpi
    {"TpFchInitEnvAb",0xAF51},///<FCHInitEnvABLink
    {"TpFchInitEnvLpc",0xAF52},///<FCHInitEnvLPC
    {"TpFchInitEnvSpi",0xAF53},///<FCHInitEnvSPI
    {"TpFchInitEnvEspi",0xAF54},///<FCHInitEnveSPI
    {"TpFchInitEnvSd",0xAF55},///<FCHInitEnvSD
    {"TpFchInitEnvEmmc",0xAF56},///<FCHInitEnveMMC
    {"TpFchInitEnvSata",0xAF57},///<FCHInitEnvSATA
    {"TpFchInitEnvUsb",0xAF58},///<FCHInitEnvUSB
    {"TpFchInitEnvXgbe",0xAF59},///<FCHInitEnvxGbE
    {"TpFchInitEnvHwAcpiP",0xAF5F},///<FCHInitEnvHwAcpiP
    {"TpFchInitMidHwAcpi",0xAF60},///<FCHInitMidHwAcpi
    {"TpFchInitMidAb",0xAF61},///<FCHInitMidABLink
    {"TpFchInitMidLpc",0xAF62},///<FCHInitMidLPC
    {"TpFchInitMidSpi",0xAF63},///<FCHInitMidSPI
    {"TpFchInitMidEspi",0xAF64},///<FCHInitMideSPI
    {"TpFchInitMidSd",0xAF65},///<FCHInitMidSD
    {"TpFchInitMidEmmc",0xAF66},///<FCHInitMideMMC
    {"TpFchInitMidSata",0xAF67},///<FCHInitMidSATA
    {"TpFchInitMidUsb",0xAF68},///<FCHInitMidUSB
    {"TpFchInitMidXgbe",0xAF69},///<FCHInitMidxGbE
    {"TpFchInitLateHwAcpi",0xAF70},///<FCHInitLateHwAcpi
    {"TpFchInitLateAb",0xAF71},///<FCHInitLateABLink
    {"TpFchInitLateLpc",0xAF72},///<FCHInitLateLPC
    {"TpFchInitLateSpi",0xAF73},///<FCHInitLateSPI
    {"TpFchInitLateEspi",0xAF74},///<FCHInitLateeSPI
    {"TpFchInitLateSd",0xAF75},///<FCHInitLateSD
    {"TpFchInitLateEmmc",0xAF76},///<FCHInitLateeMMC
    {"TpFchInitLateSata",0xAF77},///<FCHInitLateSATA
    {"TpFchInitLateUsb",0xAF78},///<FCHInitLateUSB
    {"TpFchInitLateXgbe",0xAF79},///<FCHInitLatexGbE
    {"EndFchTestPoints",0xAFFF},///<EndofTPrangeforFCH

    {"EndAgesaPcs",0xFFFF},///<LastdefinedAGESAPCs
};
#endif

VOID
IdsAgesaTestPoint (
  IN       AGESA_POST_CODE      TestPoint,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32   SizedTp;
  UINT32   TpPrefix;
#ifndef IDSOPT_NDEBUG
  CHAR8    *TpName = NULL;
  UINT32   Index;
#endif

  TpPrefix = AGESA_POST_CODE_PREFIX;
  if (PcdGetBool (PcdAgesaTestPointEnable)) {
    SizedTp = (UINT32) TestPoint;
    SizedTp |= TpPrefix;

    LibAmdIoWrite (PcdGet8 (PcdAgesaTestPointWidth), PcdGet16 (PcdIdsDebugPort), &SizedTp, StdHeader);
#ifndef IDSOPT_NDEBUG
    for(Index =0; AgesaTestPointStruct[Index].PointValue != 0xFFFF; Index++)
    {
        if((UINT16)SizedTp == AgesaTestPointStruct[Index].PointValue)
        {
            TpName = AgesaTestPointStruct[Index].PointName;
            break;
        }
    }
	IDS_HDT_CONSOLE(MAIN_FLOW, "IdsAgesaTestPoint[%08X](%a)\n", SizedTp, TpName ? TpName : "NULL");
#endif
  }
}

/**
 *  IDS Function to display FileCode and Line number on port 0x80 display
 *
 *  @param[in]    FileCode    FileCode and Line Number to display
 *
 **/
BOOLEAN
IdsErrorStop (
  IN       UINT32 FileCode
  )
{
  UINT64  Port80Message;
  UINT16  SimNowEnterDebuggerStatus;
  UINTN   Index;

  IDS_HDT_CONSOLE(MAIN_FLOW, "IdsErrorStop[%08X]\n", FileCode);

  // Write out FileCode one time and fire SimNow breakpoint
  IoWrite16 (0xE0, 0xDEAD);
  IoWrite32 (0x80, FileCode);
  SimNowEnterDebuggerStatus = LibAmdSimNowEnterDebugger ();

  Port80Message = 0xDEAD000000000000 | LShiftU64 (FileCode, 16);
  while (1) {
    if (SimNowEnterDebuggerStatus != 0) {
      break;
    }
    for (Index = 0; Index < 6; Index ++) {
      Port80Message = LRotU64 (Port80Message, 8);
      IoWrite32 (0x80, (UINT32)Port80Message);
      MicroSecondDelay (2500000);
    }
    // Rotate back to original value
    Port80Message = LRotU64 (Port80Message, 16);
  }
  return FALSE;
}

/**
 *  IDS Backend Function for ASSERT
 *
 * Halt execution with stop code display.  Stop Code is displayed on port 80, with rotation so that
 * it is visible on 8, 16, or 32 bit display.  The stop code is alternated with 0xDEAD on the display,
 * to help distinguish the stop code from a post code loop.
 * Additional features may be available if using simulation.
 *
 * @param[in]     FileCode    File code(define in FILECODE.h) mix with assert Line num.
 *
 * @retval         TRUE     No error
 **/
BOOLEAN
IdsAssert (
  IN      UINT32 FileCode
  )
{
  if (PcdGetBool (PcdAgesaAssertEnable)) {
    IdsErrorStop (FileCode);
  }
  return TRUE;
}
