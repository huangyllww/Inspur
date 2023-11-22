## How To Run  
## Prerequisites  
* A server boot to shell


## Step
* Boot to internal shell
* Enter U disk file system
  `fs0: `
* Print help  
  `ByoDmi.efi -h `
* Print Version  
  `ByoDmi.efi -v `
* View Smbios information  
  `ByoDmi.efi -smbiosinfo`
* View type 0 all information    
  `ByoDmi.efi -viewall 0 `
* View type 0 number 0 information    
  `ByoDmi.efi -view 0 0 `
* Modify type 1  Product Name:  
  `ByoDmi.efi -type 1 0 5 ByoSoft`
* Modify type 1  UUID:  
  `ByoDmi.efi -type 1 0 8 11 22 33 44 55 66 77 88 99 00 11 22 33 44 55 66`
  `ByoDmi.efi -type 1 0 8 auto`
* Lock all tools
  `ByoDmi.efi -lock`  
* Unlock all tools
  `ByoDmi.efi -unlock`  

## Notes
-  Must execute tool with administrator permission
-  when Tool not work, must make sure the platform support it 
-  when want to input special character like space,",!,and so on.  
-  You can use "" to contain them.To input ",can try use \".Shell mode can try use ^"

## ReleaseNote  
- V1.20  
  Update some type with latest spec  
  -view offset show single offset print  
  -type add check input whether enough  

- V1.19  
  Add 0x before hex data print  
  Update type 17 to support latest spec  

- V1.18  
  Different tools not share same lock status  

- V1.17  
  Will print null if smbios string is empty  

- V1.16  
  Align the ways to get/set password and lock status with OS tool  

- V1.15  
  Align and correct printf messages 

- V1.14  
  Check bios version  
  Report version messages when check bios version fail  
  Align type 3 messages  

- V1.13  
  Update CopyRights  
  type 11 starts from offset 1  
  Align type print messages  

- V1.12  
  Fix type 0,4,9,13,127 view error  
  Allow update type 4 offset 4,7  
  View have same format and will report error when input error  

- V1.11  
  *Reorganize ByoDmi and its -view offset function*  
  *Change help messages format*  
  *Change type1 uuid print error*  
- V1.10
  *check strhex when input uuid*  
  *change type 9 device/function number print messages  *  
  *change report messages to follow byoflash*  
- V1.9
 *Report error when input error* 
 *add -view offset to follow linux byodmi*
- V1.8
 *update type 9,17,20,33* 
- V1.7
 *Fix type 7 Maximum size ,Installed size display issue and print error* 
- V1.6
 *Fix some errors*  
 *Print error messages when input error*  
 *Add -view type 0-41*  
- V1.5
 *Fix update type11 string error*  
- V1.4
 *Aarch64 and x86 are combined*  
 *update strings with space is possible*  
 *Add AlderLake platform support*
 *change input password logic to make backspace useful*
- V1.3
 *Fix some errors*  
 *Update help massages*  
- V1.2 
 *Fix many errors*  
