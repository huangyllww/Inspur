/** @file
 Byosoft IPMI Protocol header file.
*/

#ifndef _IPMI_INTERFACE_PROTOCOL_H_
#define _IPMI_INTERFACE_PROTOCOL_H_

#include <BaseIpmi.h>

typedef struct _IPMI_INTERFACE_PROTOCOL IPMI_INTERFACE_PROTOCOL;


/**
 Send request, include Network Function, LUN, and command of IPMI, with/without
 additional data to BMC.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         Request             IPMI_CMD_HEADER structure, storing Network Function, LUN
                                        and various IPMI command, to send to BMC.
 @param[in]         SendData            Optional arguments, if an IPMI command is required to
                                        send with data, this argument is a pointer to the data buffer.
                                        If no data is required, set this argument as NULL.
 @param[in]         SendLength          When sending command with data, this argument is the length of the data,
                                        otherwise set this argument as 0.
 @param[out]        RecvData            Data buffer to put the data read from BMC.
 @param[out]        RecvLength          Length of Data readed from BMC.
 @param[out]        StatusCodes         The bit 15 of StatusCodes means this argument is valid or not:
                                        1. If bit 15 set to 1, this is a valid Status Code,
                                        and the Status Code is in low byte.
                                        2. If bit 15 set to 0, there is no Status Code
                                        For KCS system interface:
                                        StatusCodes is valid when return value is EFI_ABORTED. If the return
                                        value is EFI_DEVICE_ERROR,it does not guarantee StatusCodes is valid,
                                        the caller must check bit 15.
                                        For SMIC system interface: It is always valid.
                                        For BT system interface: It is no function.

 @retval EFI_SUCCESS                    Execute command successfully.
 @retval EFI_NO_MAPPING                 The request Network Function and the response Network Function
                                        does not match.
 @retval EFI_LOAD_ERROR                 Execute command successfully, but the completion code return
                                        from BMC is not 00h.
 @retval EFI_ABORTED                    For KCS system interface:
                                        1. When writing to BMC, BMC cannot enter "Write State",
                                        the error processing make BMC to enter "Idle State" successfully.
                                        2. When finishing reading from BMC, BMC cannot enter "Idle State",
                                        the error processing make BMC to enter "Idle State" successfully.
                                        For SMIC system interface: Unspecified error or abort.
                                        For BT system interface: It won't have this error.
 @retval EFI_TIMEOUT                    For KCS system interface:
                                        1. Output buffer is not full in a given time.
                                        2. Iutput buffer is not empty in a given time.
                                        For SMIC system interface:
                                        1. BUSY bit is not cleared or TX_DATA_RDY bit is not set in a given time.
                                        2. BUSY bit is not cleared or RX_DATA_RDY bit is not set in a given time.
                                        For BT system interface:
                                        1. B_BUSY and H2B_ATN bit are not cleared in a given time.
                                        2. B2H_ATN bit is not cleared in a given time.
 @retval EFI_DEVICE_ERROR               For KCS system interface:
                                        1. When writing to BMC, BMC cannot enter "Write State",
                                        the error processing cannot make BMC to enter "Idle State".
                                        2. When finishing reading from BMC, BMC cannot enter "Idle State",
                                        the error processing cannot make BMC to enter "Idle State".
                                        For SMIC system interface:
                                        Buffer full, attemp to write too many bytes to the BMC.
                                        For BT system interface: It won't have this error.
 @retval EFI_INVALID_PARAMETER          For all system interfaces:
                                        This or RecvData or RecvLength is NULL.
                                        For SMIC system interface:
                                        Illegal or unexpected control code, or illegal command.
*/
typedef
EFI_STATUS
(EFIAPI *IPMI_INTERFACE_PROTOCOL_EXECUTE_IPMI_CMD) (
  IN  IPMI_INTERFACE_PROTOCOL           *This,
  IN  IPMI_CMD_HEADER                   Request,
  IN  VOID                              *SendData OPTIONAL,
  IN  UINTN                             SendLength,
  OUT VOID                              *RecvData,
  OUT UINT8                             *RecvLength,
  OUT UINT16                            *StatusCodes OPTIONAL
  );


/**
 Return system interface type that BMC currently use.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.

 @retval IPMI_KCS                   If current system interface type is KCS.
 @retval IPMI_SMIC                  If current system interface type is SMIC.
 @retval IPMI_BT                    If current system interface type is BT.
*/
typedef
IPMI_INTERFACE_TYPE
(EFIAPI *IPMI_INTERFACE_PROTOCOL_GET_IPMI_INTERFACE_TYPE) (
  IN  IPMI_INTERFACE_PROTOCOL          *This
  );


/**
 Return system interface name that BMC currently use.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.
 @param[out]        InterfaceName       The buffer storing the name string. It is the caller's
                                        responsibility to provide enough space to store the name string.
                                        The longest name is five CHAR16, that is, four for interface
                                        name string and one for NULL character.

 @retval EFI_INVALID_PARAMETER          InterfaceName is NULL.
 @retval EFI_SUCCESS                    Return interface name string successfully.
*/
typedef
EFI_STATUS
(EFIAPI *IPMI_INTERFACE_PROTOCOL_GET_IPMI_INTERFACE_NAME) (
  IN  IPMI_INTERFACE_PROTOCOL           *This,
  OUT CHAR16                            *InterfaceName
  );


/**
 Return system interface address that BMC currently use.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         Index               Type of base address, one of three types:
                                        "IPMI_OS_BASE_ADDRESS_INDEX"
                                        "IPMI_POST_BASE_ADDRESS_INDEX"

 @retval The base address that BMC current use.
*/
typedef
UINT64
(EFIAPI *IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS) (
  IN  IPMI_INTERFACE_PROTOCOL           *This,
  IN  UINT8                             Index
  );


/**
 Return system interface offset that BMC currently use.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         Index               Type of base address, one of three types:
                                        "IPMI_OS_BASE_ADDRESS_INDEX"
                                        "IPMI_POST_BASE_ADDRESS_INDEX"

 @retval The base address offset that BMC current use.
*/
typedef
UINT64
(EFIAPI *IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS_OFFSET) (
  IN  IPMI_INTERFACE_PROTOCOL           *This,
  IN  UINT8                             Index
  );


/**
 Return base address type that BMC currently use.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.

 @retval IPMI_MEMORY                The base address is in memory space.
 @retval IPMI_IO                    The base address is in I/O space.
*/
typedef
IPMI_INTERFACE_ADDRESS_TYPE
(EFIAPI *IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS_TYPE) (
  IN  IPMI_INTERFACE_PROTOCOL          *This
  );


/**
 Return IPMI version that BMC current supports.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.

 @retval IPMI version. Upper four bits are msb, lower four bits are lsb. For example, if
         IPMI version is 2.0, this function return 0x20h.
*/
typedef
UINT8
(EFIAPI *IPMI_INTERFACE_PROTOCOL_GET_IPMI_VERSION) (
  IN  IPMI_INTERFACE_PROTOCOL           *This
  );


/**
 Return BMC firmware version.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.

 @retval Major revision is in high byte, minor revision is in low byte.
*/
typedef
UINT16
(EFIAPI *IPMI_INTERFACE_PROTOCOL_GET_BMC_FIRMWARE_VERSION) (
  IN  IPMI_INTERFACE_PROTOCOL            *This
  );


/**
 Return BMC status.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.

 @retval The enum IPMI_BMC_STATUS of BMC current status.
*/
typedef
IPMI_BMC_STATUS
(EFIAPI *IPMI_INTERFACE_PROTOCOL_GET_BMC_STATUS) (
  IN  IPMI_INTERFACE_PROTOCOL          *This
  );

/**
 Return BMC status.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.

 @retval The enum IPMI_BMC_STATUS of BMC current status.
*/
typedef
UINT8
(EFIAPI *IPMI_INTERFACE_PROTOCOL_GET_LAST_COMPLETE_CODE) (
  IN  IPMI_INTERFACE_PROTOCOL          *This
  );


/**
 Execute IPMB access via "Send Message" and "Get Message" command.
 Use Send Message command to IPMB, than corresponding response message will be placed in the
 Receive Message Queue, and use Get Message command to get message from the queue.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         Channel             Channel number to identify a particular connection.
 @param[in]         TargetAddr          Slave address of target device.
 @param[in]         Request             IPMI_CMD_HEADER structure, storing Network Function, LUN
                                        and various IPMI command, to send to BMC.
 @param[in]         SendData            Optional arguments, if an IPMI command is required to
                                        send with data, this argument is a pointer to the data buffer.
                                        If no data is required, set this argument as NULL.
 @param[in]         SendLength          When sending command with data, this argument is the length of the data,
                                        otherwise set this argument as 0.
 @param[out]        RecvData            Data buffer to put the data read from BMC.
 @param[out]        RecvLength          Length of Data readed from BMC.

 @retval EFI_SUCCESS                    Execute command successfully.
 @retval EFI_INVALID_PARAMETER          This or RecvData or RecvLength is NULL.
                                        When SendLength is not 0 but SendData is NULL.
 @retval EFI_TIMEOUT                    Can't get corresponding response message after limited maximum waiting time.
 @return Execute IPMI Command Status.
*/
typedef
EFI_STATUS
(EFIAPI *IPMI_INTERFACE_PROTOCOL_EXECUTE_IPMB_CMD) (
  IN  IPMI_INTERFACE_PROTOCOL           *This,
  IN  UINT8                             Channel,
  IN  UINT8                             TargetAddr,
  IN  IPMI_CMD_HEADER                   Request,
  IN  VOID                              *SendData OPTIONAL,
  IN  UINTN                             SendLength,
  OUT VOID                              *RecvData,
  OUT UINT8                             *RecvLength
  );


//
// Structure of IPMI_INTERFACE_PROTOCOL
//
struct _IPMI_INTERFACE_PROTOCOL {
  IPMI_INTERFACE_PROTOCOL_EXECUTE_IPMI_CMD             ExecuteIpmiCmd;
  IPMI_INTERFACE_PROTOCOL_GET_IPMI_INTERFACE_TYPE      GetIpmiInterfaceType;
  IPMI_INTERFACE_PROTOCOL_GET_IPMI_INTERFACE_NAME      GetIpmiInterfaceName;
  IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS        GetIpmiBaseAddress;
  IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS_TYPE   GetIpmiBaseAddressType;
  IPMI_INTERFACE_PROTOCOL_GET_IPMI_VERSION             GetIpmiVersion;
  IPMI_INTERFACE_PROTOCOL_GET_BMC_FIRMWARE_VERSION     GetBmcFirmwareVersion;
  IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS_OFFSET GetIpmiBaseAddressOffset;
  IPMI_INTERFACE_PROTOCOL_GET_BMC_STATUS               GetBmcStatus;
  IPMI_INTERFACE_PROTOCOL_EXECUTE_IPMB_CMD             ExecuteIpmbCmd;
  IPMI_INTERFACE_PROTOCOL_GET_LAST_COMPLETE_CODE       GetLastCompleteCode;
};

extern EFI_GUID gByoIpmiInterfaceProtocolGuid;
#endif
