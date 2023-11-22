#ifndef _BIOS2SMU_SMC_H_
#define _BIOS2SMU_SMC_H_

// *** IMPORTANT ***
// SMU TEAM: Always increment the interface version if
// any structure is changed in this file
#define BIOS_2_SMU_SMC_IF_VERSION  0x48000001

// SMU Response Codes:
#define SMC_Result_OK                 0x1
#define SMC_Result_Failed             0xFF
#define SMC_Result_UnknownCmd         0xFE
#define SMC_Result_CmdRejectedPrereq  0xFD
#define SMC_Result_CmdRejectedBusy    0xFC

// Message Definitions:
#define SMC_MSG_TestMessage                 0x1
#define SMC_MSG_GetSmuVersion               0x2
#define SMC_MSG_GetBiosIfVersion            0x3
#define SMC_MSG_DfPstateChng                0x4 // DO NOT CHANGE, USED IN DV
#define SMC_MSG_CpuPstateChng               0x5
#define SMC_MSG_ConfigureWaflLinks          0x6
#define SMC_MSG_QueryReadyDramTraining      0x7
#define SMC_MSG_SetSharedDramAddress        0x8
#define SMC_MSG_EnableSmuFeatures           0x9
#define SMC_MSG_DisableSmuFeatures          0xA
#define SMC_MSG_PmLogSetDramAddrHigh        0xB
#define SMC_MSG_PmLogSetDramAddrLow         0xC
#define SMC_MSG_DramLogSetDramAddrHigh      0xD
#define SMC_MSG_DramLogSetDramAddrLow       0xE
#define SMC_MSG_HsioTestMessage             0xF
#define SMC_MSG_ReadCoreCacWeightRegister   0x10
#define SMC_MSG_ControlPerfMon              0x11
#define SMC_MSG_GetThrottlerThresholds      0x12
#define SMC_MSG_SleepEntry                  0x13
#define SMC_MSG_EnableHotPlug               0x14
#define SMC_MSG_DisableHotPlug              0x15
#define SMC_MSG_GPIOService                 0x16
#define SMC_MSG_SetI2CBusSwitchBaseAddress  0x17
#define SMC_MSG_ConfigureNTBUsageMode       0x18
#define SMC_MSG_EnableNTBOptions            0x19
#define SMC_MSG_DisableNTBOptions           0x1A
#define SMC_MSG_spare0                      0x1B
#define SMC_MSG_spare1                      0x1C
#define SMC_MSG_SetBiosDramAddrHigh         0x1D
#define SMC_MSG_SetBiosDramAddrLow          0x1E
#define SMC_MSG_SetToolsDramAddrHigh        0x1F
#define SMC_MSG_SetToolsDramAddrLow         0x20
#define SMC_MSG_TransferTableSmu2Dram       0x21
#define SMC_MSG_TransferTableDram2Smu       0x22
#define SMC_MSG_AcBtcStartCal               0x23
#define SMC_MSG_AcBtcStopCal                0x24
#define SMC_MSG_AcBtcEndCal                 0x25
#define SMC_MSG_DcBtc                       0x26
#define SMC_MSG_BtcRestoreOnS3Resume        0x27
#define SMC_MSG_GetNameString               0x28
#define SMC_MSG_CheckTdcCompatibility       0x29
#define SMC_MSG_CheckEdcCompatibility       0x2A
#define SMC_MSG_DramLogSetDramSize          0x2B
#define SMC_MSG_ReadPerfmonBaseAddress      0x2C
#define SMC_MSG_SetPerfmonDramAddressHigh   0x2D
#define SMC_MSG_SetPerfmonDramAddressLow    0x2E
#define SMC_MSG_SetGPIOInterruptCommand     0x2F
#define SMC_MSG_GetGPIOInterruptStatus      0x30
#define SMC_MSG_DisablePSI                  0x35    // reserved for SMU feature
#define SMC_MSG_TdpTjmax                    0x36    // reserved for SMU feature
#define SMC_MSG_PCIeInterruptRemap          0x3B

#define SMC_MSG_SmiHandlerReady             0x3C
#define SMC_MSG_RtcRecordDataReady          0x3D
#define SMC_MSG_GetCpuSerialNumber          0x47

#define SMC_Message_Count                   0x3F

#endif
