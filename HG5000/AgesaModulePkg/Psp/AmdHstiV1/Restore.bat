if not exist "AmdHstiV1Bin.inf" (
  @echo "BuildSource.bat haven't been executed"
  goto END
)
del AmdHstiV1.inf /Q /F
ren AmdHstiV1Bin.inf AmdHstiV1.inf
:END