/* $NoKeywords:$ */ 
/*****************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 *
 ******************************************************************************
 */

#ifndef __CXL_DEVICE_DRIVER_H__
#define __CXL_DEVICE_DRIVER_H__

#include <Uefi.h>

#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Pci22.h>
#include <IndustryStandard/PciNew.h>
#include <IndustryStandard/Cxl.h>
#include <IndustryStandard/Cxl20.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/GnbPciLib.h>
#include <Library/GnbCxlLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include <HYGON.h>

#include <Protocol/ComponentName.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/PciIo.h>
#include <Protocol/HygonCxlManagerProtocol.h>
#include <Protocol/HygonCxl11ServicesProtocol.h>

#define  MAX_CXL_DEVICES                 16
#define  MAX_CXL_MEM_POOLS_PER_DEVICE    2
#define  CXL_VERSION_20                  0x20
#define  CXL_VERSION_11                  0x11
#define  UNKNOWN_VERSION                 0x00

#define  MEM_32_BAR                      0
#define  MEM_64_BAR                      2

#define  CXL11_MEMBAR0_ALIGN               0xFFFF
#define  CXL11_MEMBAR0_SIZE                0x10000

#pragma pack (push, 1)
typedef struct {
  UINT64                    Base;          ///< Base address of memory range
  UINT64                    Size;          ///< Size of memory range
  UINT64                    Alignment;     ///< Alignment requirement for Base address
  UINT8                     EfiType;       ///< EFI_MEMORY_TYPE of memory range
  BOOLEAN                   MemoryActive;
  UINT8                     MediaType;     ///000b: Volatile memory; 001b: Non-Volatile memory; 010b: Get info via CDAT
  UINT8                     MemoryClass;   ///000b: Memory Class; 001b: Storage Class; 010b: Get info via CDAT
} HYGON_CXL_MEMORY_POOL;

// Forward reference
typedef struct _CXL_DEVICE {
  EFI_HANDLE                  Controller;
  EFI_PCI_IO_PROTOCOL        *StartPciIo;
  PCI_ADDR                    PciAddr;     ///< PCI address of the CXL CxlDevice
  UINT16                      VendorId;
  UINT16                      DeviceId;
  UINT8                       CxlVersion;
  UINT8                       MemHwInitMode;
  UINT16                      MemPoolCount;
  HYGON_CXL_MEMORY_POOL       MemPool[MAX_CXL_MEM_POOLS_PER_DEVICE];
  HYGON_CXL_MEMORY_POOL       HostMemPool;  
  UINT16                      FlexBusDeviceCapPtr; 
  //UINT16                      RegLocatorCapPtr;
  UINT64                      ComponentRegBaseAddr;
  UINT64                      DeviceRegBaseAddr;
  BOOLEAN                     DeviceProbed;
  //BOOLEAN                     DeviceConfigured;
  //CXL11_PORT_INFO             RootBridgeInfo;
  HYGON_CXL_PORT_INFO_STRUCT  Cxl11PortInfo;
} CXL_DEVICE;

typedef union {
  struct {
    UINT32             MemIo:1;
    UINT32             MemType:2;
    UINT32             Prefetch:1;
    UINT32             BaseAddress:28;
  } Field;
  UINT32 Value;
} PCIE_BAR_STRUCT;

typedef union {
  struct {
    UINT32                            MEM_BASE_TYPE:4;
    UINT32                            MEM_BASE_31_20:12;
    UINT32                            MEM_LIMIT_TYPE:4;
    UINT32                            MEM_LIMIT_31_20:12;
  } Field;
  UINT32 Value;
} MEM_BASE_LIMIT_PCIERCCFG_UNION;

typedef union {
  struct {
    UINT32                            PREF_MEM_BASE_TYPE:4;
    UINT32                            PREF_MEM_BASE_31_20:12;
    UINT32                            PREF_MEM_LIMIT_TYPE:4;
    UINT32                            PREF_MEM_LIMIT_31_20:12;
  } Field;
  UINT32 Value;
} PREF_BASE_LIMIT_PCIERCCFG_UNION;

#pragma pack (pop)
//
// Global Variables
//
extern EFI_DRIVER_BINDING_PROTOCOL     gCxlDeviceDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL     gCxlDeviceComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL    gCxlDeviceComponentName2;

extern HYGON_CXL_MANAGER_PROTOCOL           *gCxlMgrProtocol;
extern HYGON_NBIO_CXL11_SERVICES_PROTOCOL   *gCxl11ServicesProtocol;

extern CXL_DEVICE                      gCxlDeviceList[];
extern UINTN                           gCxlDeviceCount;

//
// Driver binding functions
//
/**
  Supported function of Driver Binding protocol for this driver.
  Test to see if this driver supports ControllerHandle.

  @param This                   Protocol instance pointer.
  @param Controller             Handle of device to test.
  @param RemainingDevicePath    A pointer to the device path. Should be ignored by
                                device driver.

  @retval EFI_SUCCESS           This driver supports this device.
  @retval EFI_ALREADY_STARTED   This driver is already running on this device.
  @retval other                 This driver does not support this device.

**/
EFI_STATUS
EFIAPI
CxlDeviceSupported (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Controller,
  IN  EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );

/**
  This routine is called to Start this driver on ControllerHandle
  right after .Supported() is called.

  @param This                   Protocol instance pointer.
  @param Controller             Handle of device to bind driver to.
  @param RemainingDevicePath    A pointer to the device path. Should be ignored by
                                device driver.

  @retval EFI_SUCCESS           This driver is added to this device.
  @retval EFI_ALREADY_STARTED   This driver is already running on this device.
  @retval other                 Some error occurs when binding this driver to this device.

**/
EFI_STATUS
EFIAPI
CxlDeviceStart (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Controller,
  IN  EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );

/**
  Stop this driver on ControllerHandle.

  @param This               A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param Controller         A handle to the device being stopped.
  @param NumberOfChildren   The number of child device handles in ChildHandleBuffer.
  @param ChildHandleBuffer  An array of child handles to be freed.

  @retval EFI_SUCCESS       This driver is removed from this device.
  @retval other             Some error occurs when removing this driver from this device.

**/
EFI_STATUS
EFIAPI
CxlDeviceStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Controller,
  IN  UINTN                        NumberOfChildren,
  IN  EFI_HANDLE                   *ChildHandleBuffer
  );

//
// Forward reference declaration
//
/**
  Retrieves a Unicode string that is the user readable name of the UEFI Driver.

  @param This           A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
  @param Language       A pointer to a three character ISO 639-2 language identifier.
                        This is the language of the driver name that that the caller
                        is requesting, and it must match one of the languages specified
                        in SupportedLanguages.  The number of languages supported by a
                        driver is up to the driver writer.
  @param DriverName     A pointer to the Unicode string to return.  This Unicode string
                        is the name of the driver specified by This in the language
                        specified by Language.

  @retval EFI_SUCCESS           The Unicode string for the Driver specified by This
                                and the language specified by Language was returned
                                in DriverName.
  @retval EFI_INVALID_PARAMETER Language is NULL.
  @retval EFI_INVALID_PARAMETER DriverName is NULL.
  @retval EFI_UNSUPPORTED       The driver specified by This does not support the
                                language specified by Language.
**/
EFI_STATUS
EFIAPI
CxlDeviceComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  );

/**
  Retrieves a Unicode string that is the user readable name of the controller
  that is being managed by an UEFI Driver.

  @param This                   A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
  @param ControllerHandle       The handle of a controller that the driver specified by
                                This is managing.  This handle specifies the controller
                                whose name is to be returned.
  @param OPTIONAL   ChildHandle The handle of the child controller to retrieve the name
                                of.  This is an optional parameter that may be NULL.  It
                                will be NULL for device drivers.  It will also be NULL
                                for a bus drivers that wish to retrieve the name of the
                                bus controller.  It will not be NULL for a bus driver
                                that wishes to retrieve the name of a child controller.
  @param Language               A pointer to a three character ISO 639-2 language
                                identifier.  This is the language of the controller name
                                that that the caller is requesting, and it must match one
                                of the languages specified in SupportedLanguages.  The
                                number of languages supported by a driver is up to the
                                driver writer.
  @param ControllerName         A pointer to the Unicode string to return.  This Unicode
                                string is the name of the controller specified by
                                ControllerHandle and ChildHandle in the language
                                specified by Language from the point of view of the
                                driver specified by This.

  @retval EFI_SUCCESS           The Unicode string for the user readable name in the
                                language specified by Language for the driver
                                specified by This was returned in DriverName.
  @retval EFI_INVALID_PARAMETER ControllerHandle is not a valid EFI_HANDLE.
  @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is not a valid
                                EFI_HANDLE.
  @retval EFI_INVALID_PARAMETER Language is NULL.
  @retval EFI_INVALID_PARAMETER ControllerName is NULL.
  @retval EFI_UNSUPPORTED       The driver specified by This is not currently
                                managing the controller specified by
                                ControllerHandle and ChildHandle.
  @retval EFI_UNSUPPORTED       The driver specified by This does not support the
                                language specified by Language.
**/
EFI_STATUS
EFIAPI
CxlDeviceComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  EFI_HANDLE                   ChildHandle OPTIONAL,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **ControllerName
  );

EFI_STATUS 
InitCxl11Device (
  IN CXL_DEVICE                   *CxlDevice
  );

EFI_STATUS 
InitCxl20Device (
  IN CXL_DEVICE                   *CxlDevice
  );

void CxlEnableMmioAccess (
  IN EFI_PCI_IO_PROTOCOL          *PciIo
  );

void DumpCxlAvailResource (
  IN   FABRIC_CXL_AVAIL_RESOURCE  *CxlResourceSize
  );

void CxlGetMemPoolSize (
  IN CXL_DEVICE                *CxlDevice
  );

EFI_STATUS 
CxlEnableMemPool (
  IN CXL_DEVICE                *CxlDevice
  );

EFI_STATUS 
CxlConvertMemoryPool (
    IN UINT64                   Base,
    IN UINT64                   Size,
    IN EFI_GCD_MEMORY_TYPE      GcdMemoryType,
    IN UINT64                   Attributes
  );

EFI_STATUS
CxlLockDvsec (
  IN CXL_DEVICE                *CxlDevice
  );

void ProbeCxl11Device (
  IN CXL_DEVICE                *CxlDevice
  );

void ProbeCxl20Device (
  IN CXL_DEVICE                *CxlDevice
  );

#endif
