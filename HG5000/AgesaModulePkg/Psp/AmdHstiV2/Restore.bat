if not exist "AmdHstiV2Bin.inf" (
  @echo "BuildSource.bat haven't been executed"
  goto END
)
del AmdHstiV2.inf /Q /F
ren AmdHstiV2Bin.inf AmdHstiV2.inf
:END