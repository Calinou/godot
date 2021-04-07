!define PRODUCT_NAME "Godot Engine"
!define VERSION "4.0.0.0"
!define PRODUCT_VERSION "4.0.0.0"
!define PRODUCT_GROUP "Godot Engine contributors"
!define PRODUCT_PUBLISHER "Godot Engine contributors"
!define PRODUCT_WEB_SITE "https://godotengine.org/"
!define PRODUCT_DIR_REGKEY "Software\${PRODUCT_NAME}"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_ID "{60d07aaa-400e-40f5-b073-a796c34d9d78}"

Name "${PRODUCT_NAME}"
OutFile "godot-setup-x86_64.exe"
Unicode true
InstallDir "$LocalAppdata\Godot"
SetCompressor /SOLID lzma
ManifestDPIAware true
SetOverwrite ifdiff
BrandingText "${PRODUCT_NAME}"
RequestExecutionLevel user

# Start default section.
Section
    # Set the installation directory as the destination for the following actions.
    SetOutPath "$INSTDIR"

	File "godot.exe"
	CreateShortcut "$SMPROGRAMS\Godot.lnk" "$INSTDIR\godot.exe"

    WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section uninstall
    # Always delete the uninstaller first.
    Delete "$INSTDIR\uninstall.exe"

    Delete "$INSTDIR\godot.exe"
    Delete "$SMPROGRAMS\Godot.lnk"

    RMDir "$INSTDIR"
SectionEnd
