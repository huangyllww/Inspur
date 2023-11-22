# How To Run  
## Prerequisites  
* A 64-bit Windows PC
* Administrator permission

## Step
* Open cmd with administrator
* Print help  
  `ByoDmi.exe -h `
* Print Version  
  `ByoDmi.exe -v `
* View Smbios information  
  `ByoDmi.exe -smbiosinfo`
* View type 0 all information    
  `ByoDmi.exe -viewall 0 `
* View type 0 number 0 information    
  `ByoDmi.exe -view 0 0 `
* Modify type 1  Product Name:  
  `ByoDmi.exe -type 1 0 5 ByoSoft`
* Modify type 1  UUID:  
  `ByoDmi.exe -type 1 0 8 11 22 33 44 55 66 77 88 99 00 11 22 33 44 55 66`  
* Lock all flash region  
  `ByoDmi.exe -lock`  
* Unlock all flash region  
  `ByoDmi.exe -unlock`  

 
## Notes
-  Must execute tool with administrator permission
-  when Tool not work, must make sure the platform support it 
-  when want to input special character like space,",!,and so on.  
-  You can use "" to contain them.To input ",can try use \".Shell mode can try use ^"

## ReleaseNotes  
- V1.23  
  Update some type with latest spec  
  -view offset show single offset print  
  -type add check input whether enough  

- V1.22  
  Add 0x before hex data print  
  Update type 17 to support latest spec  
  Check return status of write flash  

- V1.21  
  Different tools not share same lock status  

- V1.20  
  Will print null if smbios string is empty  

- V1.19  
  Align and correct printf messages 

- V1.18  
  Report version messages when check bios version fail  
  ByDmi fix copymem size too big  

- V1.17  
  Update CopyRights  
  type 11 starts from offset 1  
  Align type print messages  

- V1.16  
  Fix type 0,4,9,13,127 view error  
  Allow update type 4 offset 4,7  
  View have same format and will report error when input error  

- V1.15  
  Reorganize ByoDmi and its -view offset function  
  Change help messages format  
  Change type1 uuid print error  

- V1.14  
  Remove print typehandle when directly used  
  change type 9 device/function number print messages  
  change report messages to follow byoflash  

- V1.13  
  Report error when input error  
  add -view offset to follow shell byodmi  
  gets smbios not smbios3 address  

- V1.12   
  update type 4,9,13,17,20  
  update get version and check wrong input  
- V1.11  
  Add view type 1 sub-item with offset feature  
  Fix type 7 Maximum size and Installed size display issue  

- V1.10  
  Correct and alignment print message  

- V1.9  
  Update Copyright information  

- V1.8  
  Fix view type 3 SKU number issue  

- V1.7  
  Update print help message  
  Fix segment fault if not argument input issue  

- V1.6  
  Update for check BIOS related tool version  
  Update Windows vcxproj for release build  
  
- V1.5  
  Add Win 10 X64 driver to resource  
  Add Lock status check  
  Add Password check  
  Fix type 17 display issue  
  Correct type 16 display issue  
  Remove type 4 help message  
- V1.4  
  *Correct Type 16 display message
  *Fix Type 11 update string issue
- V1.3  
  *Fix SMBIOS handle larger than DMI_STRUCT_MAX issue*  
-  V1.2  
  *Fix Set DMI use wrong buffer size issue*  
-  V1.1  
  *Fix some bugs*  
  *Update SMI communicate buffer and send SMI command register*  