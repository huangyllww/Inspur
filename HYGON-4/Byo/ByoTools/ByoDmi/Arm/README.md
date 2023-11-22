# How To Run  
## Prerequisites  
* A 64-bit ARM platform PC
* Administrator permission

## Step
* Add execute attribute  
  `chmod +x ByoDmi `
* Print help  
  `sudo ./ByoDmi -h `
* Print Version  
  `sudo ./ByoDmi -v `
* View Smbios information  
  `sudo ./ByoDmi -smbiosinfo`
* View type 0 all information    
  `sudo ./ByoDmi -viewall 0 `
* View type 0 number 0 information    
  `sudo ./ByoDmi -view 0 0 `
* Modify type 1  Product Name:  
  `sudo ./ByoDmi -type 1 0 5 ByoSoft`
* Modify type 1  UUID:  
  `sudo ./ByoDmi -type 1 0 8 11 22 33 44 55 66 77 88 99 00 11 22 33 44 55 66`  
* Lock all flash region  
  `sudo ./ByoDmi -lock`  
* Unlock all flash region  
  `sudo ./ByoDmi -unlock`  

 
## Notes
-  Must execute tool with administrator permission
-  when Tool not work, must make sure the platform support it 
-  if update new string length larger than original string length, need reboot to display the new string
-  when want to input special character like space,",!,and so on.  
-  You can use '' to contain them.To input ',can try use \'.

## ReleaseNotes  
- V1.23  
  Update some type with latest spec  
  -view offset show single offset print  
  -type add check input whether enough  

- V1.22  
  Support check password when PcdSystemPasswordHashIteration set as ture  
  Password hash type add sha512 and sha384 support  

- V1.21  
  Add 0x before hex data print  
  Update type 17 to support latest spec  

- V1.20  
  Different tools not share same lock status  
  Change to use gcc4.9  

- V1.19  
  Will print null if smbios string is empty  

- V1.18  
  Password add support for sm3 hash type  
  Align and correct printf messages  

- V1.17  
  Report version messages when check bios version fail  

- V1.16  
  Update CopyRights  
  type 11 starts from offset 1  
  Align type print messages  

- V1.15  
  Fix type 0,4,9,13,127 view error  
  Allow update type 4 offset 4,7  
  View have same format and will report error when input error

- V1.14  
  Reorganize ByoDmi and its -view offset function  
  Change help messages format  
  Change type1 uuid print error  

- V1.13  
  Upgrade IsFactoryMode() function  
  Remove print typehandle when directly used  
  change type 9 device/function number print messages  
  change report messages to follow byoflash  

- V1.12  
  Report error when input error  
  add -view offset to follow shell byodmi  
  gets smbios not smbios3 address  

- V1.11  
  Update FmcNorFlashDmaComplete function  
  update type 4,9,13,17,20  
  update get version and check wrong input  

- V1.10  
  Update for support new platform  

- V1.9  
  Add view type 1 sub-item with offset feature  
  Fix type 7 Maximum size and Installed size display issue  

- V1.8  
  Correct and alignment print message  

- V1.7    
  Update Copyright information  

- V1.6  
  change struct SYSTEM_PASSWORD to follow changes in bios  
  Fix view type 3 SKU number issue  

- V1.5  
  Update print help message  
  Correct the malloc length  
  Fix segment fault if not argument input issue  

- V1.4  
  Fix Arm version ByoDmi lock/unlock error  
  Update for check BIOS related tool version  
  Support HashType from Password  

- V1.3  
  Add Lock status check  
  Add Password check  
  Fix type 17 display issue  
  Correct type 16 display issue  
  Remove type 4 help message  
- V1.2
  *Correct Type 16 display message
  *Fix Type 11 update string issue
- V1.1  
  *Fix SMBIOS handle larger than DMI_STRUCT_MAX issue*  
- V1.0  
 *First version*
