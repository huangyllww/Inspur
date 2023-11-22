/* $NoKeywords:$ */
/**
 * @file
 *
 * Nbio CXL 1.1 service Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      
 * @e sub-project:  Nbio
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2022 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 * HYGON GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(HYGON) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 *
 ***************************************************************************/
 
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#ifndef __NBIO_CXL11_SERVICES_PROTOCOL_H__
#define __NBIO_CXL11_SERVICES_PROTOCOL_H__

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
// Current PROTOCOL revision
#define HYGON_NBIO_CXL11_SERVICES_REVISION    0x00

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
typedef struct _HYGON_NBIO_CXL11_SERVICES_PROTOCOL HYGON_NBIO_CXL11_SERVICES_PROTOCOL;

#pragma pack (push, 1)

/// Port Information Structure
typedef struct _HYGON_CXL_PORT_INFO_STRUCT {
  PCI_ADDR  EndPointBDF;                ///< Bus/Device/Function of Root Port in PCI_ADDR format
  UINT8     SocketId;
  UINT8     LogicalDieId;
  UINT8     PhysicalDieId;
  UINT8     RbIndex;
  UINT32    DspRcrb;                    ///< Downstream Port RCRB address
  UINT32    UspRcrb;                    ///< Upstream Port RCRB address
  UINT32    DspMemBar0;                 ///< Downstream port MEMBAR0
  UINT32    UspMemBar0;                 ///< Upstream port MEMBAR0
  UINT8     PhysicalPortId;             ///< Physical port location
  UINT8     PortWidth;                  ///< Lane width of the port
  PCI_ADDR  ParentPciAddr;
} HYGON_CXL_PORT_INFO_STRUCT;

/// RCEC map Information Structure
typedef struct _HYGON_RCEC_MAP_INFO_STRUCT {
  UINT8     RcecId; 
  UINT8     SocketId;
  UINT8     LogicalDieId;
  UINT8     PhysicalDieId;
  UINT8     RbIndex;
  UINT8     RbBusNumber;
  UINT8     StartRciepBus;
  UINT8     EndRciepBus;
  UINT8     DspCount;
  UINT32    DspRcrb[8];
} HYGON_RCEC_MAP_INFO_STRUCT;

#pragma pack (pop)

// Protocol Definitions
/**
  This function gets information about a CXL1.1 root port.

  This
    A pointer to the HYGON_NBIO_CXL11_SERVICES_PROTOCOL instance.
  PortIndex
    Cxl 1.1 port index in system
  PortInformation
    A pointer to an information structure to be populated by this function to
    identify the location of the CXL port.
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_CXL11_GET_ROOT_PORT_INFO_BY_INDEX) (
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,                       ///< ptr
  IN  UINTN                                PortIndex,                  ///< port index
  OUT HYGON_CXL_PORT_INFO_STRUCT          *PortInformation             ///< port information ptr
);

/**
  This function gets information about a CXL1.1 root port.

  This
    A pointer to the HYGON_NBIO_CXL11_SERVICES_PROTOCOL instance.
  EndPointBus
    Cxl 1.1 port PCI bus number
  PortInformation
    A pointer to an information structure to be populated by this function to
    identify the location of the CXL port.
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_CXL11_GET_ROOT_PORT_INFO_BY_BUS) (
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,                       ///< ptr
  IN  UINT8                                EndPointBus,                ///< CXL 1.1 Endpoint bus number
  OUT HYGON_CXL_PORT_INFO_STRUCT          *PortInformation             ///< port information ptr
);

/**
  This function configures a specific PCIe root port for CXL capabilities.

  This
    A pointer to the HYGON_NBIO_CXL11_SERVICES_PROTOCOL instance.
  EndpointBDF
    Bus/Device/Function of Endpoint in PCI_ADDR format.
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_CXL11_CONFIGURE_ROOT_PORT) (
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,                          ///<
  IN  PCI_ADDR                             EndpointBDF                    ///<
);

typedef
EFI_STATUS
(EFIAPI *HYGON_CXL11_GET_PORT_RB_LOCATION) (                              ///< get port rb location
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,                          ///<
  IN  UINT8                                EndPointBus,                   ///<
  OUT UINT8                               *SocketId,                      ///<
  OUT UINT8                               *PhysicalDieId,                 ///<
  OUT UINT8                               *LogicalDieId,                  ///<
  OUT UINT8                               *RbIndex                        ///<
  );

/**
  This function gets RCEC map information.

  This
    A pointer to the HYGON_NBIO_CXL11_SERVICES_PROTOCOL instance.
  RcecIndex
   RCEC index in system
  RcecMapInformation
    A pointer to an RCEC map information structure.
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_CXL11_GET_RCEC_INFO_BY_INDEX) (
  IN  HYGON_NBIO_CXL11_SERVICES_PROTOCOL  *This,                       ///< ptr
  IN  UINT8                                RcecIndex,                  ///< RCEC index
  OUT HYGON_RCEC_MAP_INFO_STRUCT          *RcecMapInformation          ///< RCEC map information ptr
);

/// The Protocol Definition for CXL Services
struct _HYGON_NBIO_CXL11_SERVICES_PROTOCOL {
  UINT32                                  Revision;                        ///< revision
  UINTN                                   Cxl11Count;                      ///< CXL1.1 count
  HYGON_CXL11_GET_ROOT_PORT_INFO_BY_INDEX Cxl11GetRootPortInfoByIndex;     ///< cxl root port information
  HYGON_CXL11_GET_ROOT_PORT_INFO_BY_BUS   Cxl11GetRootPortInfoByBus;       ///< cxl root port information
  HYGON_CXL11_CONFIGURE_ROOT_PORT         Cxl11ConfigureRootPort;          ///< configuring the root port
  HYGON_CXL11_GET_PORT_RB_LOCATION        GetCxl11PortRBLocation;          ///< CXL port RB location
  UINTN                                   RcecCount;                       ///< RCEC count
  HYGON_CXL11_GET_RCEC_INFO_BY_INDEX      GetRcecMapInfoByIndex;           ///< RCEC map information
};

extern EFI_GUID gHygonNbioCxl11ServicesProtocolGuid;

#endif


