if not exist "HygonHstiBin.inf" (
  @echo "BuildSource.bat haven't been executed"
  goto END
)
del HygonHsti.inf /Q /F
ren HygonHstiBin.inf HygonHsti.inf
:END