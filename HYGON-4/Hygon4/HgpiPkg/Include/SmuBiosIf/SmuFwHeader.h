#ifndef _SMU_FW_HEADER_H_
#define _SMU_FW_HEADER_H_

#pragma pack(push, 1)

// *** IMPORTANT ***
// SMU TEAM: Always increment the interface version if
// any structure is changed in this file
#define SMU_FW_HEADER_VERSION  0x48000001

struct SMU9_Firmware_Footer {
  UINT32    Signature;
};

typedef struct SMU9_Firmware_Footer SMU9_Firmware_Footer;

typedef struct {
  UINT8     Nonce[16];          // [0x00] Unique image id
  UINT32    HeaderVersion;      // [0x10] Version of the header
  UINT32    SizeFWSigned;       // [0x14] Size of the FW to be included in signature in bytes
  UINT32    EncOption;          // [0x18] 0 - Not encrypted, 1 - encrypted
  UINT32    EncAlgID;           // [0x1C] Encryption algorithm id
  UINT8     EncParameters[16];  // [0x20] Encryption Parameters
  UINT32    SigOption;          // [0x30] 0 - not signed 1 - signed
  UINT32    SigAlgID;           // [0x34] Signature algorithm ID
  UINT8     SigParameters[16];  // [0x38] Signature parameter
  UINT32    CompOption;         // [0x48] Compression option
  UINT32    CompAlgID;          // [0x4C] Compression Algorithm ID
  UINT32    UnCompImageSize;    // [0x50] Uncompressed Image Size
  UINT32    CompImageSize;      // [0x54] compressed Image Size
  UINT8     CompParameters[8];  // [0x58] Compression Parameters
  UINT32    ImageVersion;       // [0x60] Off Chip Firmware Version
  UINT32    APUFamilyID;        // [0x64] APU Family ID or SoC ID
  UINT32    FirmwareLoadAddr;   // [0x68] Firmware Load address (default 0)
  UINT32    SizeImage;          // [0x6C] Size of entire signed image including key tokens
  UINT32    SizeFWUnSigned;     // [0x70] Size of Un-signed portion of the FW
  UINT32    FirmwareSplitAddr;  // [0x74] Joining point of combined FWs (e.g. Nwd/Swd split address)
  UINT8     Reserved[8];        // [0x78] *** RESERVED ***
  UINT8     EncKey[16];         // [0x80] Encryption Key (Wrapped MEK)
  UINT8     SigningInfo[16];    // [0x90] Signing tool specific information
  UINT8     Padd[96];           // [0xA4] *** RESERVED ***
} SMU_Firmware_Header;

#define SMU9_MAX_CUS      2
#define SMU9_PSMS_PER_CU  10
#define SMU9_Num_tests    3

#define SMU9_FIRMWARE_HEADER_LOCATION  0x1FF80
#define SMU9_UNBCSR_START_ADDR         0xC0100000

#pragma pack(pop)

#endif
