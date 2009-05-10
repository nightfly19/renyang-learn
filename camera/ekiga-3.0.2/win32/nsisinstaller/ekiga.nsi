; Ekiga.nsi
; ====================================================
; NSIS Installer for Ekiga Win32
; Original Authors: Herman Bloggs <hermanator12002@yahoo.com> 
; and Daniel Atallah <daniel_atallah@yahoo.com> (GAIM Installler)
; Original version : Installer for Ekiga win32
; Update: Luba Vincent <luba@novacom.be>
; Installer Version: 1.0
; Created : 09/01/06
; Last Update: 10/13/06
; ====================================================

!addPluginDir ${NSISPLUGINDIR}
; ===========================
; Global Variables
var name
var STARTUP_RUN_KEY
var ALREADY_INSTALLED
; ===========================
; Configuration

Name $name
;SetCompressor /SOLID lzma
OutFile "${TARGET_DIR}/ekiga-setup-${EKIGA_VERSION}.exe"
;!ifdef DEBUG
;OutFile "ekiga-setup-${EKIGA_VERSION}-debug.exe"
;!else
;!endif

; ===========================
; Includes
!include "MUI.nsh"
!include "Sections.nsh"
!include "FileFunc.nsh"
!include "Library.nsh"
!include "${NSISSYSTEMDIR}/System.nsh"

!insertmacro GetParameters
!insertmacro GetOptions
!insertmacro GetParent

; ===========================
; Defines

!define EKIGA_REG_KEY "SOFTWARE\ekiga"
!define EKIGA_UNINST_EXE "ekiga-uninst.exe"
!define EKIGA_UNINSTALL_KEY			"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Ekiga"
!define HKLM_APP_PATHS_KEY			"SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\ekiga.exe"
!define EKIGA_REG_LANG				"Installer Language"
!define EKIGA_STARTUP_RUN_KEY			"SOFTWARE\Microsoft\Windows\CurrentVersion\Run"

; ===========================
; Modern UI configuration
!define MUI_ICON                                "${EKIGA_DIR}/win32/ico/ekiga.ico"
!define MUI_UNICON                              "${EKIGA_DIR}/win32/ico/ekiga-uninstall.ico" 

!define MUI_HEADERIMAGE
!define MUI_COMPONENTSPAGE_SMALLDESC
!define MUI_ABORTWARNING

;Finish Page config
!define MUI_FINISHPAGE_RUN			"$INSTDIR\ekiga.exe"
!define MUI_FINISHPAGE_RUN_CHECKED
  
; ===========================
; Pages

!insertmacro MUI_PAGE_WELCOME

; Alter License section
!define MUI_LICENSEPAGE_BUTTON		        $(EKIGA_LICENSE_BUTTON)
!define MUI_LICENSEPAGE_TEXT_BOTTOM		$(EKIGA_LICENSE_BOTTOM_TEXT)

!insertmacro MUI_PAGE_COMPONENTS

; Ekiga install dir page
!insertmacro MUI_PAGE_DIRECTORY

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; ===========================
; Languages

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Hungarian"
!insertmacro MUI_LANGUAGE "French"

!define EKIGA_DEFAULT_LANGFILE                  "${INSTALLER_DIR}/language_files/english.nsh"


!include "${INSTALLER_DIR}/langmacros.nsh"

!insertmacro EKIGA_MACRO_INCLUDE_LANGFILE "HUNGARIAN"		"${INSTALLER_DIR}/language_files/hungarian.nsh"
!insertmacro EKIGA_MACRO_INCLUDE_LANGFILE "FRENCH"		"${INSTALLER_DIR}/language_files/french.nsh"
!insertmacro EKIGA_MACRO_INCLUDE_LANGFILE "ENGLISH"		"${INSTALLER_DIR}/language_files/english.nsh"

; ===========================
; Sections
Section -SecUninstallOldEkiga
        ; Check install rights..
        Call CheckUserInstallRights
        Pop $R0
        
        ;If ekiga is currently set to run on startup,
        ;  save the section of the Registry where the setting is before uninstalling,
        ;  so we can put it back after installing the new version
        ClearErrors
        ReadRegStr $STARTUP_RUN_KEY HKCU "${EKIGA_STARTUP_RUN_KEY}" "Ekiga"
        IfErrors +3
        StrCpy $STARTUP_RUN_KEY "HKCU"
        Goto +4
        ClearErrors
        ReadRegStr $STARTUP_RUN_KEY HKLM "${EKIGA_STARTUP_RUN_KEY}" "Ekiga"
        IfErrors +2
        StrCpy $STARTUP_RUN_KEY "HKLM"

        StrCmp $R0 "HKLM" ekiga_hklm
        StrCmp $R0 "HKCU" ekiga_hkcu done

        ekiga_hkcu:
                ReadRegStr $R1 HKCU ${EKIGA_REG_KEY} ""
                ReadRegStr $R2 HKCU ${EKIGA_REG_KEY} "Version"
                ReadRegStr $R3 HKCU "${EKIGA_UNINSTALL_KEY}" "UninstallString"
                Goto try_uninstall

        ekiga_hklm:
                ReadRegStr $R1 HKLM ${EKIGA_REG_KEY} ""
                ReadRegStr $R2 HKLM ${EKIGA_REG_KEY} "Version"
                ReadRegStr $R3 HKLM "${EKIGA_UNINSTALL_KEY}" "UninstallString"

        ; If previous version exists .. remove
        try_uninstall:
                StrCmp $R1 "" done
                ; Version key started with 0.60a3. Prior versions can't be
                ; automaticlly uninstalled.
                StrCmp $R2 "" uninstall_problem
                ; Check if we have uninstall string..
                IfFileExists $R3 0 uninstall_problem
                ; Have uninstall string.. go ahead and uninstall.
                SetOverwrite on
                ; Need to copy uninstaller outside of the install dir
                ClearErrors
                CopyFiles /SILENT $R3 "$TEMP\${EKIGA_UNINST_EXE}"
                SetOverwrite off
                IfErrors uninstall_problem
                ; Ready to uninstall..
                ClearErrors
                ExecWait '"$TEMP\${EKIGA_UNINST_EXE}" /S _?=$R1'
                IfErrors exec_error
                Delete "$TEMP\${EKIGA_UNINST_EXE}"
                Goto done

        exec_error:
                Delete "$TEMP\${EKIGA_UNINST_EXE}"
                Goto uninstall_problem
 
        uninstall_problem:
                ; We can't uninstall.  Either the user must manually uninstall or we ignore and reinstall over it.
                MessageBox MB_OKCANCEL $(EKIGA_PROMPT_CONTINUE_WITHOUT_UNINSTALL) /SD IDOK IDOK done
                Quit
 
        done:
SectionEnd

;--------------------------------
;Ekiga Install Section

Section $(EKIGA_SECTION_TITLE) SecEkiga
  SectionIn 1 RO

  ; Check install rights..
  Call CheckUserInstallRights
  Pop $R0

  ; Get GTK+ lib dir if we have it..

  StrCmp $R0 "NONE" ekiga_install_files
  StrCmp $R0 "HKLM" ekiga_hklm ekiga_hkcu

  ekiga_hklm:
    WriteRegStr HKLM "${HKLM_APP_PATHS_KEY}" "" "$INSTDIR\ekiga.exe"
    WriteRegStr HKLM ${EKIGA_REG_KEY} "" "$INSTDIR"
    WriteRegStr HKLM ${EKIGA_REG_KEY} "Version" "${EKIGA_VERSION}"
    WriteRegStr HKLM "${EKIGA_UNINSTALL_KEY}" "DisplayName" $(EKIGA_UNINSTALL_DESC)
    WriteRegStr HKLM "${EKIGA_UNINSTALL_KEY}" "UninstallString" "$INSTDIR\${EKIGA_UNINST_EXE}"
    ; Sets scope of the desktop and Start Menu entries for all users.
    SetShellVarContext "all"
    Goto ekiga_install_files

  ekiga_hkcu:
    WriteRegStr HKCU ${EKIGA_REG_KEY} "" "$INSTDIR"
    WriteRegStr HKCU ${EKIGA_REG_KEY} "Version" "${EKIGA_VERSION}"
    WriteRegStr HKCU "${EKIGA_UNINSTALL_KEY}" "DisplayName" $(EKIGA_UNINSTALL_DESC)
    WriteRegStr HKCU "${EKIGA_UNINSTALL_KEY}" "UninstallString" "$INSTDIR\${EKIGA_UNINST_EXE}"
    Goto ekiga_install_files

  ekiga_install_files:
    SetOutPath "$INSTDIR"
    ; Ekiga files
    SetOverwrite on
    File "${TARGET_DIR}\Ekiga\*.exe"
    File "${TARGET_DIR}\Ekiga\*.dll"
    File /r "${TARGET_DIR}\Ekiga\icons"
    File /r "${TARGET_DIR}\Ekiga\ekiga"
    File /r "${TARGET_DIR}\Ekiga\sounds"
    File /r "${TARGET_DIR}\Ekiga\help"
    File /r "${TARGET_DIR}\Ekiga\share\locale"
    File /r "${TARGET_DIR}\Ekiga\plugins"
    File /r "${TARGET_DIR}\Ekiga\etc"
    File /r "${TARGET_DIR}\Ekiga\lib"
    File /r "${TARGET_DIR}\Ekiga\share"

    IfFileExists "$INSTDIR\ekiga.exe" 0 new_installation
    StrCpy $ALREADY_INSTALLED 1

  new_installation:
    File "${EKIGA_DIR}/win32/ico/ekiga.ico"

    ; If we don't have install rights.. we're done
    StrCmp $R0 "NONE" done
    SetOverwrite off

    ; Write out installer language
    WriteRegStr HKCU "${EKIGA_REG_KEY}" "${EKIGA_REG_LANG}" "$LANGUAGE"

    ; write out uninstaller
    SetOverwrite on
    WriteUninstaller "$INSTDIR\${EKIGA_UNINST_EXE}"
    SetOverwrite off

    ; If we previously had ekiga setup to run on startup, make it do so again
    StrCmp $STARTUP_RUN_KEY "HKCU" +1 +2
    WriteRegStr HKCU "${EKIGA_STARTUP_RUN_KEY}" "Ekiga" "$INSTDIR\ekiga.exe"
    StrCmp $STARTUP_RUN_KEY "HKLM" +1 +2
    WriteRegStr HKLM "${EKIGA_STARTUP_RUN_KEY}" "Ekiga" "$INSTDIR\ekiga.exe"

    SetOutPath "$INSTDIR"

  done:
SectionEnd ; end of default Ekiga section

;--------------------------------
;Shortcuts

SubSection /e $(EKIGA_SHORTCUTS_SECTION_TITLE) SecShortcuts
  Section $(EKIGA_DESKTOP_SHORTCUT_SECTION_TITLE) SecDesktopShortcut
    SetOutPath "$INSTDIR"
    SetShellVarContext "all"
    SetOverwrite on
    CreateShortCut "$DESKTOP\Ekiga.lnk" "$INSTDIR\ekiga.exe" "" "$INSTDIR\ekiga.ico"
    SetOverwrite off
    SetShellVarContext "current"
  SectionEnd

  Section $(EKIGA_STARTMENU_SHORTCUT_SECTION_TITLE) SecStartMenuShortcut
    SetOutPath "$INSTDIR"
    SetShellVarContext "all"
    SetOverwrite on
    CreateDirectory "$SMPROGRAMS\Ekiga"
    CreateShortCut "$SMPROGRAMS\Ekiga\Ekiga.lnk" "$INSTDIR\ekiga.exe" ""  "$INSTDIR\ekiga.ico"
    CreateShortcut "$SMPROGRAMS\Ekiga\Uninstall Ekiga.lnk" "$INSTDIR\${EKIGA_UNINST_EXE}" "" "" "" "" "" "Uninstall Ekiga"
    SetOverwrite off
    SetShellVarContext "current"
  SectionEnd
  
  Section $(EKIGA_RUN_AT_STARTUP) SecStartup
     SetOutPath $INSTDIR
     CreateShortCut "$SMSTARTUP\Ekiga.lnk" "$INSTDIR\ekiga.exe" "" "" 0 SW_SHOWMINIMIZED
  SectionEnd
SubSectionEnd


;--------------------------------
;Uninstaller Section


Section Uninstall
  Call un.CheckUserInstallRights
  Pop $R0
  StrCmp $R0 "NONE" no_rights
  StrCmp $R0 "HKCU" try_hkcu try_hklm

  try_hkcu:
    ReadRegStr $R0 HKCU ${EKIGA_REG_KEY} ""
    StrCmp $R0 $INSTDIR 0 cant_uninstall
    ; HKCU install path matches our INSTDIR.. so uninstall
    DeleteRegKey HKCU ${EKIGA_REG_KEY}
    DeleteRegKey HKCU "${EKIGA_UNINSTALL_KEY}"
    Goto cont_uninstall

  try_hklm:
    ReadRegStr $R0 HKLM ${EKIGA_REG_KEY} ""
    StrCmp $R0 $INSTDIR 0 try_hkcu
    ; HKLM install path matches our INSTDIR.. so uninstall
    DeleteRegKey HKLM ${EKIGA_REG_KEY}
    DeleteRegKey HKLM "${EKIGA_UNINSTALL_KEY}"
    DeleteRegKey HKLM "${HKLM_APP_PATHS_KEY}"
    ; Sets start menu and desktop scope to all users..
    SetShellVarContext "all"

  cont_uninstall:
    ; The WinPrefs plugin may have left this behind..
    DeleteRegValue HKCU "${EKIGA_STARTUP_RUN_KEY}" "Ekiga"
    DeleteRegValue HKLM "${EKIGA_STARTUP_RUN_KEY}" "Ekiga"
    ; Remove Language preference info
    DeleteRegKey HKCU ${EKIGA_REG_KEY} ;${MUI_LANGDLL_REGISTRY_ROOT} ${MUI_LANGDLL_REGISTRY_KEY}


    RMDir "$INSTDIR\pixmaps"
    RMDir "$INSTDIR\ekiga"
    RMDir "$INSTDIR\sounds"
    RMDir "$INSTDIR\locale"
    Delete /REBOOTOK "$INSTDIR\*.*"
    RMDir /r /REBOOTOK "$INSTDIR"

    SetShellVarContext "all"
    Delete /REBOOTOK "$SMPROGRAMS\Ekiga\*.*"
    Delete /REBOOTOK "$SMSTARTUP\Ekiga.lnk"
    RMDir "$SMPROGRAMS\Ekiga"
    Delete "$DESKTOP\Ekiga.lnk"
    
    SetShellVarContext "current"

    Delete "$INSTDIR\${EKIGA_UNINST_EXE}"

    ;Try to remove Ekiga install dir .. if empty
    RMDir "$INSTDIR"

    ; Shortcuts..
    RMDir /r "$SMPROGRAMS\Ekiga"
    Delete "$DESKTOP\Ekiga.lnk"

    Goto done

  cant_uninstall:
    MessageBox MB_OK $(un.EKIGA_UNINSTALL_ERROR_1) /SD IDOK
    Quit

  no_rights:
    MessageBox MB_OK $(un.EKIGA_UNINSTALL_ERROR_2) /SD IDOK
    Quit

  done:
SectionEnd ; end of uninstall section

; ///////////////////////////////////////
;; Functions
; ///////////////////////////////////////

; ===========================
; Init Global parameters

Function .onInit
  Push $R0
  SystemLocal::Call 'kernel32::CreateMutexA(i 0, i 0, t "ekiga_installer_running") i .r1 ?e'
  Pop $R0
  StrCmp $R0 0 +3
  MessageBox MB_OK|MB_ICONEXCLAMATION "Another instance of the installer is already running" /SD IDOK
  Abort
  Call RunCheck

  StrCpy $name "Ekiga"

  ${GetParameters} $R0
  ClearErrors
  ${GetOptions} $R0 "/L=" $LANGUAGE
  IfErrors 0 skip_lang

  StrCpy $LANGUAGE ${LANG_ENGLISH}
  !insertmacro MUI_LANGDLL_DISPLAY

  skip_lang:
    ; If install path was set on the command, use it.
    StrCmp $INSTDIR "" 0 instdir_done
 
    ;  If ekiga is currently intalled, we should default to where it is currently installed
    ClearErrors
    ReadRegStr $INSTDIR HKCU "${EKIGA_REG_KEY}" ""
    IfErrors +2
    StrCmp $INSTDIR "" 0 instdir_done
    ClearErrors
    ReadRegStr $INSTDIR HKLM "${EKIGA_REG_KEY}" ""
    IfErrors +2
    StrCmp $INSTDIR "" 0 instdir_done

    Call CheckUserInstallRights
    Pop $R0

    StrCmp $R0 "HKLM" 0 user_dir
    StrCpy $INSTDIR "$PROGRAMFILES\Ekiga"
    Goto instdir_done

  user_dir:
    Push $SMPROGRAMS
    ${GetParent} $SMPROGRAMS $R2
    ${GetParent} $R2 $R2
    StrCpy $INSTDIR "$R2\Ekiga"

  instdir_done:
    Pop $R0
FunctionEnd

Function un.onInit
  StrCpy $name "Ekiga"
FunctionEnd


; ===========================
; Check if another instance 
; of the installer is running
!macro RunCheckMacro UN
Function ${UN}RunCheck
  Push $R0
  Processes::FindProcess "ekiga.exe"
  IntCmp $R0 0 done
  MessageBox MB_YESNO|MB_ICONEXCLAMATION "Ekiga is running. To continue installation I need to shut it down. Shall I proceed?" /SD IDYES IDNO abort_install
  Processes::KillProcess "ekiga.exe"
  Goto done

  abort_install:
    Abort
  
  done:
    Pop $R0
FunctionEnd
!macroend
!insertmacro RunCheckMacro ""
!insertmacro RunCheckMacro "un."


!macro CheckUserInstallRightsMacro UN
Function ${UN}CheckUserInstallRights
  Push $0
  Push $1
  ClearErrors
  UserInfo::GetName
  IfErrors Win9x
  Pop $0
  UserInfo::GetAccountType
  Pop $1

  StrCmp $1 "Admin" 0 +3
  StrCpy $1 "HKLM"
  Goto done
  StrCmp $1 "Power" 0 +3
  StrCpy $1 "HKLM"
  Goto done
  StrCmp $1 "User" 0 +3
  StrCpy $1 "HKCU"
  Goto done
  StrCmp $1 "Guest" 0 +3
  StrCpy $1 "NONE"
  Goto done
  ; Unknown error
  StrCpy $1 "NONE"
  Goto done

  Win9x:
    StrCpy $1 "HKLM"

  done:
    Exch $1
    Exch
    Pop $0
FunctionEnd
!macroend
!insertmacro CheckUserInstallRightsMacro ""
!insertmacro CheckUserInstallRightsMacro "un."

;
; Usage:
;   Push $0 ; Path string
;   Call VerifyDir
;   Pop $0 ; 0 - Bad path  1 - Good path
;
Function VerifyDir
  Exch $0
  Push $1
  Push $2
  Loop:
    IfFileExists $0 dir_exists
    StrCpy $1 $0 ; save last
    ${GetParent} $0 $0
    StrLen $2 $0
    ; IfFileExists "C:" on xp returns true and on win2k returns false
    ; So we're done in such a case..
    IntCmp $2 2 loop_done
    ; GetParent of "C:" returns ""
    IntCmp $2 0 loop_done
    Goto Loop

  loop_done:
    StrCpy $1 "$0\EkIgaFooB"
    ; Check if we can create dir on this drive..
    ClearErrors
    CreateDirectory $1
    IfErrors DirBad DirGood

  dir_exists:
    ClearErrors
    FileOpen $1 "$0\ekigafoo.bar" w
    IfErrors PathBad PathGood

  DirGood:
    RMDir $1
    Goto PathGood1

  DirBad:
    RMDir $1
    Goto PathBad1

  PathBad:
    FileClose $1
    Delete "$0\ekigafoo.bar"
  PathBad1:
    StrCpy $0 "0"
    Push $0
    Goto done

  PathGood:
    FileClose $1
    Delete "$0\ekigafoo.bar"
  PathGood1:
    StrCpy $0 "1"
    Push $0

  done:
    Exch 3 ; The top of the stack contains the output variable
    Pop $0
    Pop $2
    Pop $1
FunctionEnd

;--------------------------------
;Descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecEkiga} \
        $(EKIGA_SECTION_DESCRIPTION)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecStartup} \
        "Will launch ekiga when windows Starts"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecShortcuts} \
        $(EKIGA_SHORTCUTS_SECTION_DESCRIPTION)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktopShortcut} \
        $(EKIGA_DESKTOP_SHORTCUT_DESC)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecStartMenuShortcut} \
        $(EKIGA_STARTMENU_SHORTCUT_DESC)

!insertmacro MUI_FUNCTION_DESCRIPTION_END
