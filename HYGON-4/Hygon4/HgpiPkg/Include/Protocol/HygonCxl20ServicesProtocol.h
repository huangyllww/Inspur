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
#ifndef __NBIO_CXL20_SERVICES_PROTOCOL_H__
#define __NBIO_CXL20_SERVICES_PROTOCOL_H__

#include <HYGON.h>
#include <Include/CxlCdat.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
// Current PROTOCOL revision
#define HYGON_NBIO_CXL20_SERVICES_REVISION    0x00

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
typedef struct _HYGON_NBIO_CXL20_SERVICES_PROTOCOL HYGON_NBIO_CXL20_SERVICES_PROTOCOL;

#pragma pack (push, 1)
/// CXL 2.0 Port Information Structure
typedef struct _HYGON_CXL20_PORT_INFO_STRUCT {
  PCI_ADDR  PortBDF;                ///< Bus/Device/Function of 2.0 Root Port in PCI_ADDR format
  UINT64    ComponentRegBaseAddr;
} HYGON_CXL20_PORT_INFO_STRUCT;
#pragma pack (pop)

// Protocol Definitions
/**
  This function gets information about a CXL2.0 root port.

  This
    A pointer to the HYGON_NBIO_CXL20_SERVICES_PROTOCOL instance.
  PortBDF
    Cxl 2.0 port PCI address
  PortInformation
    A pointer to an information structure to be populated by this function to
    identify the location of the CXL port.
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_CXL20_GET_ROOT_PORT_INFO) (
  IN  HYGON_NBIO_CXL20_SERVICES_PROTOCOL  *This,                       ///< ptr
  IN  PCI_ADDR                             PortBDF,                    ///< CXL 2.0 root port PCI address
  OUT HYGON_CXL20_PORT_INFO_STRUCT        *PortInformation             ///< port information
);

typedef
EFI_STATUS
(EFIAPI *HYGON_CXL20_GET_PORT_RB_LOCATION) (                              ///< Get CXL 1.1 port RB location
  IN  HYGON_NBIO_CXL20_SERVICES_PROTOCOL  *This,                          ///<
  IN  UINT8                                CxlRcBus,                      ///<
  OUT UINT8                               *SocketId,                      ///<
  OUT UINT8                               *LogicalDieId,                  ///<
  OUT UINT8                               *RbIndex                        ///<
  );

/**
  This function gets the pointer to the CDAT for a CXL device. Caller needs to free contents of CdatTable.

  This
    A pointer to the HYGON_NBIO_CXL20_SERVICES_PROTOCOL instance.
  Address
    PCI Address of CXL device.
  CdatTable
    A pointer to CDAT for a CXL device
**/
typedef
EFI_STATUS
(EFIAPI *HYGON_CXL_GET_CDAT) (                                     ///< get CDAT
  IN   HYGON_NBIO_CXL20_SERVICES_PROTOCOL  *This,                  ///<
  IN   UINT32                               Address,               ///<
  OUT  CDAT_TABLE                          *CdatTable              ///<
  );

/**
  This function reads a CDAT for a CXL device for Entries of CDAT struct type passed through

  This
    A pointer to the HYGON_NBIO_CXL20_SERVICES_PROTOCOL instance.
  CdatTable
    A pointer to CDAT of a CXL device
  CdatType
    CDAT Struct Type to return
  CdatStruct
    A pointer to struct entries in CDAT
 **/
typedef
EFI_STATUS
(EFIAPI *HYGON_CXL_PARSE_CDAT) (                                   ///< parse CDAT
  IN      HYGON_NBIO_CXL20_SERVICES_PROTOCOL  *This,               ///<
  IN      CDAT_TABLE                           CdatTable,          ///<
  IN      UINT8                                CdatType,           ///<
  OUT     VOID                                *CdatStruct          ///<
  );

/// The Protocol Definition for CXL Services
struct _HYGON_NBIO_CXL20_SERVICES_PROTOCOL {
  UINT32                                  Revision;                        ///< Revision
  HYGON_CXL20_GET_ROOT_PORT_INFO          Cxl20GetRootPortInfo;            ///< CXL 2.0 root port information
  HYGON_CXL20_GET_PORT_RB_LOCATION        GetCXL20PortRBLocation;          ///< CXL port RB location
  HYGON_CXL_GET_CDAT                      CxlGetCdat;                      ///< CXL Get Cdat table from a CXL device
  HYGON_CXL_PARSE_CDAT                    CxlParseCdat;                    ///< CXL Parse CDAT for entries
};

extern EFI_GUID gHygonNbioCXL20ServicesProtocolGuid;

#endif


