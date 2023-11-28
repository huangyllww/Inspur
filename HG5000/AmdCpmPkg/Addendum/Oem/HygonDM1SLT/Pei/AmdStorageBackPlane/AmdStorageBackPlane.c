/*****************************************************************************
 *
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

#include <AmdCpmPei.h>
#include <Library/DebugLib.h>
#include <Ppi/Pca9545aPpi.h>
#include <Ppi/Pca9535aPpi.h>
#include <Ppi/Ds125Br401aPpi.h>
#include "AmdStorageBackPlane.h"
#include <Ppi/I2cMaster.h>	
#include <Ppi/M24Lc128Ppi.h>



TUNING  SataBackplaneTuning[] = {
  // ReDriver 0, Channel B
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  // ReDriver 0, Channel A
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},

  // ReDriver 1, Channel B
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  // Redriver 1, Channel A
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  
  // ReDriver 2, Channel B
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  // ReDriver 2, Channel A
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},

  // ReDriver 3, Channel B
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  // ReDriver 3, Channel A
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},

  // ReDriver 4, Channel B
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  // ReDriver 4, Channel A
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},

  // ReDriver 5, Channel B
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  // ReDriver 5, Channel A
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0},
  {1, 0, 0}
};


/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD Storage Backplane Init PEIM driver
 *
 * This function configures the SATA backplane on the crb board.
 * This will need to be extended to detect the NVME backplane and initialize 
 * correctly depending on which storage backplane is installed.  TBD.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 *
 */

EFI_STATUS
EFIAPI
AmdStorageBackPlanePeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS               Status = EFI_SUCCESS;
  EFI_PEI_PCA9545A_PPI    *Pca9545aPpi;
  EFI_PEI_DS125BR401A_PPI *Ds125Br401aPpi;
	EFI_PEI_M24LC128_PPI    *M24Lc128Ppi;
  UINT8                    MuxAddress;
  UINT8                    ReDriverAddr;
	UINT8										 ReDriverChannel;
  UINT8                   *IdByte = 0;
  UINT8                   *ReDriverId =0;
  UINT8                    Settings = 0;
 
  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gPca9545aPpiGuid,
     0,
     NULL,
     &Pca9545aPpi
     );
  if (EFI_SUCCESS != Status) return Status;


  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gDs125Br401aPpiGuid,
     0,
     NULL,
     &Ds125Br401aPpi
     );
  if (EFI_SUCCESS != Status) return Status;


  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gM24Lc128PpiGuid,
     0,
     NULL,
     &M24Lc128Ppi
     );
  if (EFI_SUCCESS != Status) return Status;


  // get the ID byte value
  // BIT0:  0 = BIOS must program the redrivers, 1 = backplane auto-configures the redrivers
  // BIT1:  0 = no SATA, 1 = has SATA 
  // BIT2:  0 = no NVME, 1 = has NVME
  //
  // SATA only backplane that we must program will be 0x02
  // NVME backplane that we must program will be 0x06
  //
  // Any other value and we can skip programming.  We still need to know SATA vs NVME
  // for programming the I/O lanes
  
  // Steer the mux to the ID EEPROM 
  Pca9545aPpi->Set (PeiServices, 0, 0x70, BIT3);  // The ID EEPROM is on channel three
  
  Status = M24Lc128Ppi->Read (PeiServices, 0, 0x51, 0, 1, IdByte);

  /* Here are some shenanigans to get around hardware issues.
   *
   * Many of the initial batch of SATA backplanes do not have the ID 
   * EEPROMs programmed.  Some escaped into the wild before they could be
   * programmed.  Workaround is to look for a re-driver at an address that 
   * varies between the SATA and NVME backplanes to ID which one we have.
   *
   */

  if (0xff == *IdByte) {
    //  Attempt to identify the SATA backplane by alternate means.
    //  Point to to channel 3 on the mux
    Pca9545aPpi->Set (PeiServices, 0, 0x70, BIT2);      // point to channel 3 on the mux
    //  Offset 0x58 should be a DS125BR401A redriver with a device ID of 0x44 at offset 0x51
    Ds125Br401aPpi->Get (PeiServices, 0, 0x58, 0x51, ReDriverId);
    if (0x44 == *ReDriverId) {
      *IdByte = 0x02;   // fake to SATA backplane  
      DEBUG((EFI_D_ERROR, "Found re-driver -- looks like a SATA backplane with a blank ID EEPROM\n"));
    } else {
      DEBUG((EFI_D_ERROR, "storage backplane not found!\n"));
      return Status;
    }
  }

  DEBUG((EFI_D_ERROR, "storage backplane type = %02x\n", *IdByte));

  if (0x02 == *IdByte) {

    DEBUG((EFI_D_ERROR, "    Configure for SATA only\n"));
  
    // Walk over the first three channels of the four channel mux.

    for (MuxAddress = 1; MuxAddress <= 0x4; MuxAddress *= 2) { 
      Pca9545aPpi->Set (PeiServices, 0, 0x70, MuxAddress); 
  
      // Two re-drivers behind each channel on the mux.
      for (ReDriverAddr = 0x58; ReDriverAddr <= 0x59; ReDriverAddr++) { 
        Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, 0x06, 0x18);
        // The following register is marked reserved in the datasheet.  
        // Writing to 0x01 per Ravi March 7, 2017.
        Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, 0x4C, 0x01);

        // Program settings in each re-driver per hardware team.
     
   
        for (ReDriverChannel = 0; ReDriverChannel < 8; ReDriverChannel++) {
          Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, VOD_Register + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), 0xA8 | SataBackplaneTuning[Settings].vod); 
          Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, EQ_Register  + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), SataBackplaneTuning[Settings].eq); 
          Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, DEM_Register + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), SataBackplaneTuning[Settings].dem);
          Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, DET_Register + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), 0x2);

          DEBUG((EFI_D_ERROR, " - ReDriverChannel = %02x, Settings index = %02x\n", ReDriverChannel, Settings));  
          DEBUG((EFI_D_ERROR, " - VOD address = %02x, VOD data = %02x\n", VOD_Register + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), SataBackplaneTuning[Settings].vod ));
          DEBUG((EFI_D_ERROR, " - EQ address  = %02x, EQ data  = %02x\n", EQ_Register + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), SataBackplaneTuning[Settings].eq ));
          DEBUG((EFI_D_ERROR, " - DEM address = %02x, DEM data = %02x\n", DEM_Register + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), SataBackplaneTuning[Settings].dem ));

          Settings++;
        }
      }
    }
  }

  if (0x06 == *IdByte) {

    DEBUG((EFI_D_ERROR, "    Configure for SATA/NVME mix\n"));
  
	  // Channels 1 & 2 each have four re-drivers for NVME lanes

    for (MuxAddress = 1; MuxAddress <= 0x2; MuxAddress *= 2) { 
      Pca9545aPpi->Set (PeiServices, 0, 0x70, MuxAddress); 
  
      // Four NVME drivers per channels 1 & 2
      for (ReDriverAddr = 0x58; ReDriverAddr <= 0x5B; ReDriverAddr++) { 
        Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, 0x06, 0x18);

		  	// Program settings in each re-driver per hardware team.
        //
      
        // B side
		  	for (ReDriverChannel = 0; ReDriverChannel < 4; ReDriverChannel++) {
			   	Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, VOD_Register + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), 0xAB); // 0xA8 | vod_rx);
				  Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, EQ_Register  + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), 1); //eq_rx);
  				Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, DEM_Register + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), 0); //dem_rx);

  			}

        // A side
		  	for (ReDriverChannel = 4; ReDriverChannel < 8; ReDriverChannel++) {
			   	Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, VOD_Register + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), 0xAB); // 0xA8 | vod_rx);
				  Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, EQ_Register  + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), 1); //eq_rx);
  				Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, DEM_Register + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), 0); //dem_rx);
  			}
      }
    }

		// Now program the two re-drivers behind channel three -- these are for the SATA lanes
		
		Pca9545aPpi->Set (PeiServices, 0, 0x70, BIT2); 
		
		for (ReDriverAddr = 0x5C; ReDriverAddr <= 0x5D; ReDriverAddr++) { 
			Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, 0x06, 0x18);

				// Program settings in each re-driver per hardware team.
				for (ReDriverChannel = 0; ReDriverChannel < 8; ReDriverChannel++) {
					Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, VOD_Register + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), 0xA8 | vod_rx);
					Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, EQ_Register  + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), eq_rx);
					Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, DEM_Register + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), dem_rx);
					Ds125Br401aPpi->Set (PeiServices, 0, ReDriverAddr, DET_Register + (ReDriverChannel * 7) + (ReDriverChannel < 4 ? 0 : 1), 0x2);
				}
			}  	

  }

  return Status;
}




