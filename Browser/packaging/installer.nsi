!macro CreateShortcuts
    CreateShortcut "$DESKTOP\SecureBrowser.lnk" "$INSTDIR\SecureBrowser.exe" "" "$INSTDIR\SecureBrowser.exe" 0
    CreateDirectory "$SMPROGRAMS\SecureBrowser"
    CreateShortcut "$SMPROGRAMS\SecureBrowser\SecureBrowser.lnk" "$INSTDIR\SecureBrowser.exe" "" "$INSTDIR\SecureBrowser.exe" 0
    CreateShortcut "$SMPROGRAMS\SecureBrowser\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
!macroend

Name "SecureBrowser"
OutFile "SecureBrowser_Setup_x64.exe"
InstallDir "$PROGRAMFILES64\SecureBrowser"
RequestExecutionLevel admin

Section "Install"
    SetOutPath $INSTDIR

    ; Include standard binaries
    File "..\build\src\Release\SecureBrowser.exe"

    ; Include CEF framework dependencies
    File "..\cef\cef_binary\Release\libcef.dll"
    File "..\cef\cef_binary\Resources\icudtl.dat"
    File "..\cef\cef_binary\Resources\v8_context_snapshot.bin"

    ; UI and Resources
    SetOutPath "$INSTDIR\resources\ui"
    File /r "..\resources\ui\*"

    ; Generate uninstaller
    WriteUninstaller "$INSTDIR\uninstall.exe"

    !insertmacro CreateShortcuts
SectionEnd

Section "Uninstall"
    Delete "$INSTDIR\SecureBrowser.exe"
    Delete "$INSTDIR\libcef.dll"
    Delete "$INSTDIR\icudtl.dat"
    Delete "$INSTDIR\v8_context_snapshot.bin"
    RMDir /r "$INSTDIR\resources"
    Delete "$INSTDIR\uninstall.exe"
    RMDir "$INSTDIR"

    Delete "$DESKTOP\SecureBrowser.lnk"
    RMDir /r "$SMPROGRAMS\SecureBrowser"
SectionEnd
