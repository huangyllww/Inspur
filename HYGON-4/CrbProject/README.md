# HygonCrbProjects

### Platform Code Download&Build


1. ref [EdkRepo Usage for CRB platform](http://221.228.236.229:28090/byocore/byo/-/wikis/EdkRepo%20Usage%20for%20CRB%20platform) to download edkrepo.

2. config git:

    ```
    git config --global core.autocrlf false
    git config --global pull.rebase true
    ```

3. Code download:

    * Hygon3000Crb

      ```
      edkrepo clone Hygon3000Crb Hygon3000Crb
      ```

    * Hygon7500Crb

      ```
      edkrepo clone Hygon7500Crb Hygon7500Crb
      ```

4. build:

    * Hygon3000Crb

      ```
      py -3 CrbProject\Hygon3000Pkg\PlatformBuild.py             # CPU Gen2 + Gen3
      py -3 CrbProject\Hygon3000Pkg\PlatformBuild.py -D OLD_CPU  # CPU Gen1 + Gen2
      ```

    * Hygon7500Crb

      ```
      py -3 CrbProject\Hygon7500Pkg\PlatformBuild.py             # CPU Gen2 + Gen3
      py -3 CrbProject\Hygon7500Pkg\PlatformBuild.py -D OLD_CPU  # CPU Gen1 + Gen2
      ```

---

### Q&A

Q: How to add pre-builded PSP image for testing ?

A: 

1. set `TKN_BUILD_PSP = 0` in PlatformBuild.py.

2. update ProjectPkg.fdf:

   ```
   #FchFw
   0x00020000|0x00001000
   gPlatformModuleTokenSpaceGuid.PcdFchFwFlashBase|gPlatformModuleTokenSpaceGuid.PcdFchFwFlashSize
   FILE = PlatformPkg/psp/ref/fch.bin             # <--- give file
   
   #PSP
   0x00087000|0x00879000                          # <--- offset maybe needs update
   gPlatformModuleTokenSpaceGuid.PcdPspFlashBase|gPlatformModuleTokenSpaceGuid.PcdPspFlashSize
   FILE = PlatformPkg/psp/ref/psp.bin             # <--- give file
   ```

---

### New CodeBase Changes

  1. remove token.h
  2. W3 -> W4, more warning
  3. PcdSet -> PcdSetS, AsciiStrCpy -> AsciiStrCpyS



