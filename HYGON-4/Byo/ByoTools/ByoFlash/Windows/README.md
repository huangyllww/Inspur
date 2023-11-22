# How To Run  
## Prerequisites  
* A 64-bit Windows PC
* Administrator permission

## Step
* Open cmd with administrator
* Print help  
  `ByoFlash.exe -h `
* Print Version  
  `ByoFlash.exe -v `
* Flash BIOS image with BIOS info defined block   
  `ByoFlash.exe -bfu Image.bin`  
* Flash all BIOS image  
  `ByoFlash.exe -bfu Image.bin -all`  
* Flash all BIOS image but not check biosid  
  `sudo ./ByoFlash -bfu Image.bin -all -no-id-check`
* Flash all BIOS image but not check ac    
  `sudo ./ByoFlash -bfu Image.bin -all -no-ac-check`
* Dump BIOS image to Backup.bin  
  `ByoFlash.exe -bu Backup.bin `  
* Dump BIOS image to Backup.bin and then update BIOS image  
  `ByoFlash.exe -bu Backup.bin -bfu Image.bin -all`  
* Dump BIOS image to Backup.bin and then update BIOS image, reboot after update successful  
  `ByoFlash.exe -bu Backup.bin -bfu Image.bin -all -reboot` 
* Reserved NVM region
  `ByoFlash.exe -resvnvm Bios.bin`  
* Reserved smbios
  `ByoFlash.efi -resvsmbios Bios.bin` 
* Update logo  
  `ByoFlash.exe -logo logo.jpg`  
* Update ucode  
  `ByoFlash.exe -ucode ucode.bin(need a specific ucode fv)`  
* Update OA3
  `ByoFlash.exe -oa3 Oa3.bin(need a specific oa3 file)`  
* Lock all flash region  
  `ByoFlash.exe -lock`  
* UnLock all flash region
  `ByoFlash.exe -unlock`  
* Query flash lock status  
  `ByoFlash.exe -locksts`  
* Reset password  
  `ByoFlash.exe -rstpwd`  

 
## Notes
-  Must execute tool with administrator permission
-  Please don't press ctrl+alt+delete or change any media device
-  If set setup administrator password, the tool first check password, and then flash image
-  when Tool not work, must make sure the platform support it
-  This tool doesn't check the input file format, it just updates the input file into flash. The user needs to make sure the input file is correct.
-  when want to input special character like space,",!,and so on.  
-  You can use "" to contain them.To input ",can try use \".Shell mode can try use ^"

## ReleaseNotes  
- V1.29  
  Cancel password length check  
  Allow bios update from unsigned to signed  

- V1.28  
  Add logo format check  
  ByoFlash will reboot after print end messages  
  ByoFlash will delete sys file when exit

- V1.27  
  Align print messages and change version print  
  Different tools not share same lock status  
  Add extra four single area update (secondbios der2 der1 pdr) if platform supported  
  Improve flash update stability  

- V1.26  
  Align Print messages  

- V1.25  
  Print Bios id when capsule update check bios id failed  

- V1.24  
  Support more than one logo files to be updated if the platform supports.

- V1.23  
  Will print bios version when check biosid failed  

- V1.22  
  Add -cap option to update capsule.cap  
  DISABLE_USB_POWERBUTTON when update  

- V1.21  
  Add -no-id-check to skip check bios id  
  Add check ac and -no-ac-check to avoid check ac  
  Align and change help messages  
  -bfu align step check with other update  

- V1.20  
  Update CopyRights  
  Will not compare me area when update bios  
  Will Stay 1ms after each smi when update me  
  Will report success when rstpwd success  
  When not in admin mode,will report error  
  Change input logic to avoid skip special character

- V1.19  
  Fix lack -ucode error  
  Update help messages  

- V1.18  
  Add input check to avoid update bios option together  
  Add "-pwd password" after -rstpwd  

- V1.17  
  Simplify help info  
  Add time out after trig each SMI  
  Compare Data before program flash  
  allow inut space as password  

- V1.16  
  system will wait 5 seconds when reboot  
  Report same messages when input error  

- V1.15  
  Add -me -irc -ec -gbe -nis option to update it only  
  fix windows byoflash update mode is 0 as default  
  fix wrong print messages of 0% progress bar  

- V1.14   
  Fix check BIOS INFO issue by wrong sub function  

- V1.13  
  Fix not update step when flash image use BIOS info issue  
  Check BIOS INFO before BIOS image default update  

- V1.12  
  Fix regression issue  
  Correct print help message  

- V1.11  
  Update for support -all option  
  Fix not sign data skip range to program issue  

- V1.10  
  Update for check multiple sign method  
  Update for support resvsmbios option  
  Add SUBFUNCTION_QUERY_PASSWORD_SET for query password   

- V1.9  
  Correct Windows/Linux some print messages  
  Fix search BIOS ID issue  
  Add reboot option for reboot system when updated BIOS  

- V1.8  
  Support first backup and then update BIOS image  

- V1.7  
  Update for support BIOS ID String check  
  Fix print update status issue  
  Update for check BIOS related tool version
  Fix x86 verify BIOS sign issue  
  Update Windows vcxproj for release build  

- V1.6  
  Add win 10 X64 driver to resource   
  Fix Logo update issue  
  Fix Windows check password not include some special character issue  
- V1.5  
  *Fix OA3 update issue
- V1.4  
  *Fix resvnvm option issue* 
  *Add function for GUI communication*  
- V1.3  
  *Support lock/unlock/locksts option for flash lock setting. if flash locked, must unlock for ByoFlash write flash firstly* 
- V1.2  
  *Support ByoFlash -rstpwd option for reset administrator password*  
- V1.1  
  *Update help message*  
- V1.0
  *First release*