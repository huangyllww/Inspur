/* $NoKeywords:$ */

/**
 * @file
 *
 * Fabric NUMA Services Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Library
 */
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

#ifndef _FABRIC_NUMA_SERVICES_PROTOCOL_H_
#define _FABRIC_NUMA_SERVICES_PROTOCOL_H_

#pragma pack (push, 1)

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
/// Domain type
typedef enum {
  SocketIntlv,
  DieIntlv,
  NoIntlv,
  NbioIntLv,
  MaxDomainType
} DOMAIN_TYPE;

/// Socket interleave domain
typedef struct {
  UINT32    SocketCount;  ///< Socket Count
  UINT32    SocketMap;    ///< Socket Map
} SOCKET_INTLV_DOMAIN;

/// Cdd interleave domain
typedef struct {
  UINT32    CddCount;     ///< Cdd Count
  UINT32    CddMap;       ///< Cdd Map
} CDD_INTLV_DOMAIN;

/// No interleave domain
typedef struct {
  UINT32    Socket;       ///< Socket
  UINT32    CddMap;       ///< CddMap
} NO_INTLV_DOMAIN;

/// Domain Info
typedef struct {
  DOMAIN_TYPE    Type;            ///< Type
  union {
    ///< Interleave
    SOCKET_INTLV_DOMAIN    Socket; ///< Socket
    CDD_INTLV_DOMAIN       Cdd;    ///< Cdd
    NO_INTLV_DOMAIN        None;   ///< None
  } Intlv;
} DOMAIN_INFO;

typedef struct {
  DOMAIN_TYPE        Type;              ///< Type
  UINT32             SocketMap;         ///< 
  UINT32             DieMap;            ///< Bitmap indicating IOD location
  UINT32             NbioMap;           ///< Bitmap indicating NBIO location
  UINT32             CxlDomain;         ///< CXL domain number(NUMA NODE)
} CXL_DOMAIN_INFO;

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */

///
/// Forward declaration for the FABRIC_NUMA_SERVICES_PROTOCOL
///
typedef struct _FABRIC_NUMA_SERVICES_PROTOCOL FABRIC_NUMA_SERVICES_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *FABRIC_NUMA_SERVICES2_GET_DOMAIN_INFO) (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
     OUT   UINT32                         *NumberOfDomainsInSystem,
     OUT   DOMAIN_INFO                   **DomainInfo,
     OUT   UINT32                         *NumberOfCxlDomainsInSystem,
     OUT   CXL_DOMAIN_INFO               **CxlDomainInfo
  );

typedef
EFI_STATUS
(EFIAPI *FABRIC_NUMA_SERVICES2_DRAM_DOMAIN_XLAT) (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
  IN       UINTN                           Socket,
  IN       UINTN                           Cdd,
     OUT   UINT32                         *Domain
  );

typedef
EFI_STATUS
(EFIAPI *FABRIC_NUMA_SERVICES2_CXL_DOMAIN_XLAT) (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
  IN       UINTN                           Socket,
  IN       UINTN                           Die,
  IN       UINTN                           Rb,
     OUT   UINT32                         *Domain
  );

typedef
EFI_STATUS
(EFIAPI *FABRIC_NUMA_SERVICES2_GET_MAX_DOMAINS) (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
  IN       UINT8                           *MaxDramDomains,
  IN       UINT8                           *MaxCxlDomains
  );

///
/// When installed, the Fabric NUMA Services Protocol produces a collection of
/// services that return various information associated with non-uniform memory
/// architecture.
///
struct _FABRIC_NUMA_SERVICES_PROTOCOL {
  UINTN                                     Revision;       ///< Revision Number
  FABRIC_NUMA_SERVICES2_GET_DOMAIN_INFO     GetDomainInfo;  ///< Get Domain Info
  FABRIC_NUMA_SERVICES2_DRAM_DOMAIN_XLAT    DomainXlat; ///< Domain Translation
  FABRIC_NUMA_SERVICES2_CXL_DOMAIN_XLAT     CxlDomainXlat;  ///< Domain Translation
  FABRIC_NUMA_SERVICES2_GET_MAX_DOMAINS     GetMaxDomains;  ///< Get maximum proximity domain
};

extern EFI_GUID  gHygonFabricNumaServicesProtocolGuid;

#pragma pack (pop)
#endif // _FABRIC_NUMA_SERVICES_PROTOCOL_H_
