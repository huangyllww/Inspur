# How To Run  
## Prerequisites  
* A 64-bit Linux PC
* Administrator permission

## Step
* Add execute attribute  
  `chmod +x ByoFlash `
* Print help  
  `sudo ./ByoFlash -h `
* Print Version  
  `sudo ./ByoFlash -v `
* Flash BIOS image with BIOS info defined block   
  `sudo ./ByoFlash -bfu Image.bin`  
* Flash all BIOS image  
  `sudo ./ByoFlash -bfu Image.bin -all`  
* Flash all BIOS image but not check biosid  
  `sudo ./ByoFlash -bfu Image.bin -all -no-id-check`
* Flash all BIOS image but not check ac  
  `sudo ./ByoFlash -bfu Image.bin -all -no-ac-check`
* Dump BIOS image to Backup.bin  
  `sudo ./ByoFlash -bu Backup.bin `   
* Dump BIOS image to Backup.bin and then update BIOS image  
  `sudo ./ByoFlash -bu Backup.bin -bfu Image.bin -all`  
* Dump BIOS image to Backup.bin and then update BIOS image, reboot after update successful  
  `sudo ./ByoFlash -bu Backup.bin -bfu Image.bin -all -reboot`
* Reserved NVM region
  `sudo ./ByoFlash -resvnvm Bios.bin`  
* Reserved SMBIOS region
  `sudo ./ByoFlash -resvsmbios Bios.bin`  
* Update logo  
  `sudo ./ByoFlash -logo logo.jpg`  
* Update ucode  
  `sudo ./ByoFlash -ucode ucode.bin(need a specific ucode fv)`  
* Update OA3
  `sudo ./ByoFlash -oa3 Oa3.bin(need a specific oa3 file)`  
* Lock all flash region  
  `sudo ./ByoFlash -lock`  
* UnLock all flash region
  `sudo ./ByoFlash -unlock`  
* Query flash lock status  
  `sudo ./ByoFlash -locksts`  
* Reset password  
  `sudo ./ByoFlash -rstpwd`
 
## Notes
-  Must execute tool with administrator permission
-  Please don't press ctrl+alt+delete or change any media device
-  If set setup administrator password, the tool first check password, and then flash image
-  when Tool not work, must make sure the platform support it
-  This tool doesn't check the input file format, it just updates the input file into flash. The user needs to make sure the input file is correct.
-  when want to input special character like space,",!,and so on.  
-  You can use '' to contain them.To input ',can try use \'.

## ReleaseNote  
- V1.28  
  Add logo format check  
  Change reboot to cold reboot  
  Cancel password length check  
  Allow bios update from unsigned to signed  

- V1.27  
  Align print messages and change version print  
  Different tools not share same lock status  
  Add extra four single area update (secondbios der2 der1 pdr) if platform supported  
  Improve flash update stability  

- V1.26  
  Support more than one logo files to be updated if the platform supports.  
  Align Print messages  

- V1.25  
  Print Bios id when capsule update check bios id failed  

- V1.24  
  Fix Linux ByoFlash -cap error  

- V1.23  
  Add -cap option if platform supported  

- V1.22
  Add -no-id-check to skip check bios id  
  Add check ac and -no-ac-check to avoid check ac  
  Align and change help messages  
  -bfu align step check with other update  

- V1.21  
  Update CopyRights  
  Will not compare me area when update bios  
  Will Stay 1ms after each smi when update me  
  Will report success when rstpwd success  
  When not in admin mode,will report error  
  Change input logic to avoid skip special character

- V1.20  
  Fix lack -ucode error  
  Update help messages  
  Will not reboot when update fail

- V1.19  
  Add input check to avoid update bios option together  
  Add "-pwd password" after -rstpwd  

- V1.18  
  Simplify help info  
  Add missing SendSmi() in ByoSmiFlashClearEnv()  
  Add time out after trig each SMI  
  Compare Data before program flash  
  
- V1.17  
  gets smbios not smbios3 address  
  lock input before sign verify  
  system will wait 5 seconds when reboot  
  Report same messages when input error  

- V1.16  
  Add -me -irc -ec -gbe -nis option to update it only  
  fix wrong print messages of 0% progress bar  

- V1.15  
  fix post error  

- V1.14  
  Fix not update step when flash image use BIOS info issue  
  Check BIOS INFO before BIOS image default update  

- V1.13  
  Fix regression issue  
  Correct print help message  

- V1.12  
  Update for support -all option  
  Fix not sign data skip range to program issue  

- V1.11  
  Update for check multiple sign method  
  Update for support resvsmbios option  
  Add SUBFUNCTION_QUERY_PASSWORD_SET for query password  

- V1.10   
  Correct Windows/Linux some print messages  
  Add reboot option for reboot system when updated BIOS  

- V1.9  
  Support first backup and then update BIOS image  

- V1.8  
  Update for support BIOS ID String check  
  Fix print update status issue  
  Update for check BIOS related tool version  
  Fix x86 verify BIOS sign issue  

- V1.7  
  Disable input interrupt when update flash  
  Fix Logo update issue  
- V1.6  
  *Fix OA3 update issue
- V1.5  
  *Add function for GUI communication*  
- V1.4
  *Support lock/unlock/locksts option for flash lock setting. if flash locked, must unlock for ByoFlash write flash firstly* 
- V1.3  
 *Support ByoFlash -rstpwd option for reset administrator password*  
- V1.2  
 *Update help message*  