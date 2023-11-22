/* $NoKeywords:$ */

/**
 * @file
 *
 * HygonCoreLaunchServicePpi Implementation
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonCoreLaunchServicePpi
 *
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
#include <HYGON.h>
#include <Ppi/HygonCoreTopologyServicesPpi.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/IdsLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioSmuLib.h>
#include <GnbRegisters.h>
#include <CddRegistersDm.h>
#include <Filecode.h>
#include <HygonNbioSmuPei.h>

#define FILECODE  NBIO_SMU_HYGX_HYGONNBIOSMUPEI_HYGONCORELAUNCHSERVICEPPI_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 * This service retrieves information about the layout of the cores on the given die.
 *
 * @param[in]  This                           A pointer to the
 *                                            HYGON_CORE_TOPOLOGY_SERVICES_PPI instance.
 * @param[in]  Socket                         Zero-based socket number to check.
 * @param[in]  Die                            The target die's identifier within Socket.
 * @param[out] NumberOfComplexes              Pointer to the number of enabled complexes on
 *                                            the given socket / die.
 * @param[out] NumberOfCores                  Pointer to the number of enabled cores per
 *                                            complex on the given socket / die.
 * @param[out] NumberOfThreads                Pointer to the number of enabled threads per
 *                                            core on the given socket / die.
 *
 * @retval EFI_SUCCESS                        The core topology information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER              Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER              Die is non-existent.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 *
 **/
EFI_STATUS
SmuGetCoreTopologyOnCdd (
  IN       HYGON_CORE_TOPOLOGY_SERVICES_PPI   *This,
  IN       UINTN                            Socket,
  IN       UINTN                            Cdd,
  OUT      UINTN                            *NumberOfComplexes,
  OUT      UINTN                            *NumberOfCores,
  OUT      UINTN                            *NumberOfThreads
  )
{
  EFI_STATUS                   Status;
  GNB_HANDLE                   *GnbHandle;
  THREAD_CONFIGURATION_STRUCT  ThreadConfiguration;

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuGetCoreTopologyOnCdd Entry\n");
  GnbHandle = NULL;

  Status = SmuGetGnbHandle (Socket, 0, &GnbHandle);
  ASSERT (GnbHandle != NULL);
  if (GnbHandle != NULL) {
    Status = NbioRegisterRead (
               GnbHandle,
               THREAD_CONFIGURATION_TYPE,
               CDD_SPACE (Cdd, THREAD_CONFIGURATION_ADDRESS_HYGX),
               &ThreadConfiguration,
               0
               );
    *NumberOfComplexes = ThreadConfiguration.HyGxField.ComplexCount + 1;
    *NumberOfCores     = ThreadConfiguration.HyGxField.CoreCount + 1;

    if (ThreadConfiguration.HyGxField.SMTMode == 1) {
      *NumberOfThreads = 2;
    } else {
      *NumberOfThreads = 1;
    }
  } else {
    Status = EFI_INVALID_PARAMETER;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "Socket %d CDD %d has %d Ccx %d Cores %d threads \n", Socket, Cdd, *NumberOfComplexes, *NumberOfCores, *NumberOfThreads);
  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuGetCoreTopologyOnCdd Exit\n");
  return Status;
}

/**
 * This service will start a core to fetch its first instructions from the reset
 * vector.  This service may only be called from the BSP.
 *
 * @param[in]  This                           A pointer to the
 *                                            HYGON_CORE_TOPOLOGY_SERVICES_PPI instance.
 * @param[in]  Socket                         Zero-based socket number of the target thread.
 * @param[in]  Die                            Zero-based die number within Socket of the target thread.
 * @param[in]  LogicalComplex                 Zero-based logical complex number of the target thread.
 * @param[in]  LogicalThread                  Zero-based logical thread number of the target thread.
 *
 * @retval EFI_SUCCESS                        The thread was successfully launched.
 * @retval EFI_DEVICE_ERROR                   The thread has already been launched.
 * @retval EFI_INVALID_PARAMETER              Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER              Die is non-existent.
 * @retval EFI_INVALID_PARAMETER              LogicalComplex is non-existent.
 * @retval EFI_INVALID_PARAMETER              LogicalThread is non-existent.
 *
 **/
EFI_STATUS
SmuLaunchThread (
  IN       HYGON_CORE_TOPOLOGY_SERVICES_PPI   *This,
  IN       UINTN                            Socket,
  IN       UINTN                            Cdd,
  IN       UINTN                            LogicalComplex,
  IN       UINTN                            LogicalThread
  )
{
  EFI_STATUS                   Status;
  GNB_HANDLE                   *GnbHandle;
  THREAD_ENABLE_STRUCT         ThreadEnable;
  THREAD_CONFIGURATION_STRUCT  ThreadConfiguration;
  UINTN                        NumberOfThreads;
  UINTN                        NumberOfCores;
  UINTN                        NumberOfLogicalThreads;
  UINT32                       SmuArg[6];

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuLaunchThread Entry\n");
  GnbHandle = NULL;

  Status = SmuGetGnbHandle (Socket, 0, &GnbHandle);
  ASSERT (GnbHandle != NULL);
  if (GnbHandle != NULL) {
    Status = NbioRegisterRead (
               GnbHandle,
               THREAD_ENABLE_TYPE,
               CDD_SPACE (Cdd, THREAD_ENABLE_ADDRESS_HYGX),
               &ThreadEnable,
               0
               );

    Status = NbioRegisterRead (
               GnbHandle,
               THREAD_CONFIGURATION_TYPE,
               CDD_SPACE (Cdd, THREAD_CONFIGURATION_ADDRESS_HYGX),
               &ThreadConfiguration,
               0
               );
    NumberOfThreads = (ThreadConfiguration.HyGxField.SMTMode == 0) ? 1 : 2;
    NumberOfCores   = ThreadConfiguration.HyGxField.CoreCount + 1;
    NumberOfLogicalThreads = NumberOfCores * NumberOfThreads;
    if ((LogicalComplex <= ThreadConfiguration.HyGxField.ComplexCount) && (LogicalThread < NumberOfLogicalThreads)) {
      if ((ThreadEnable.Field.ThreadEn & (1 << LogicalThread) << (LogicalComplex * 16)) == 0) {
        ThreadEnable.Field.ThreadEn |= ((1 << LogicalThread) << (LogicalComplex * 16));
        Status = NbioRegisterWrite (
                    GnbHandle,
                    THREAD_ENABLE_TYPE,
                    (UINT32)CDD_SPACE (Cdd, THREAD_ENABLE_ADDRESS_HYGX),
                    &ThreadEnable,
                    0
                    );
        NbioSmuServiceCommonInitArguments (SmuArg);
        SmuArg[0] = 0xAA55AA55;
        Status    = NbioSmuServiceRequest (NbioGetHostPciAddress (GnbHandle), 0, SMC_MSG_TestMessage, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
      } else {
        Status = EFI_DEVICE_ERROR;
      }
    } else {
      Status = EFI_INVALID_PARAMETER;
    }
  } else {
    Status = EFI_INVALID_PARAMETER;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuLaunchThread Exit\n");
  return Status;
}

/*----------------------------------------------------------------------------------------
 *                          P P I   D E S C R I P T O R
 *----------------------------------------------------------------------------------------
 */
HYGON_CORE_TOPOLOGY_SERVICES_PPI  mHygonCoreTopologyServicesPpi = {
  SmuGetCoreTopologyOnCdd,
  SmuLaunchThread
};
