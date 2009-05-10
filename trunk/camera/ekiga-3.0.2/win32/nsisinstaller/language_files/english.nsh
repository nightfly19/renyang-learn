;;
;;  english.nsh
;;
;;  Default language strings for the Windows Ekiga NSIS installer.
;;  Windows Code page: 1252
;;
;;  Version 3
;;  Note: If translating this file, replace "!insertmacro EKIGA_MACRO_DEFAULT_STRING"
;;  with "!insertmacro EKIGA_MACRO_DEFAULT_STRING".

; Make sure to update the EKIGA_MACRO_LANGUAGEFILE_END macro in
; langmacros.nsh when updating this file

; Startup Checks
!insertmacro EKIGA_MACRO_DEFAULT_STRING INSTALLER_IS_RUNNING			"The installer is already running."
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_IS_RUNNING				"An instance of Ekiga is currently running. Exit Ekiga and then try again."
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_INSTALLER_NEEDED			"The GTK+ runtime environment is either missing or needs to be upgraded.$\rPlease install v${GTK_VERSION} or higher of the GTK+ runtime"

; License Page
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_LICENSE_BUTTON			"Next >"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_LICENSE_BOTTOM_TEXT			"$(^Name) is released under the GNU General Public License (GPL). The license is provided here for information purposes only. $_CLICK"

; Components Page
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SECTION_TITLE			"Ekiga videophone (required)"
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_SECTION_TITLE			"GTK+ Runtime Environment (required)"
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_THEMES_SECTION_TITLE			"GTK+ Themes"
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_NOTHEME_SECTION_TITLE		"No Theme"
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_WIMP_SECTION_TITLE			"Wimp Theme"
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_BLUECURVE_SECTION_TITLE		"Bluecurve Theme"
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_LIGHTHOUSEBLUE_SECTION_TITLE		"Light House Blue Theme"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SHORTCUTS_SECTION_TITLE		"Shortcuts"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_DESKTOP_SHORTCUT_SECTION_TITLE	"Desktop"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_STARTMENU_SHORTCUT_SECTION_TITLE	"Start Menu"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SECTION_DESCRIPTION			"Core Ekiga files and dlls"
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_SECTION_DESCRIPTION			"A multi-platform GUI toolkit, used by Ekiga"
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_THEMES_SECTION_DESCRIPTION		"GTK+ Themes can change the look and feel of GTK+ applications."
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_NO_THEME_DESC			"Don't install a GTK+ theme"
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_WIMP_THEME_DESC			"GTK-Wimp (Windows impersonator) is a GTK theme that blends well into the Windows desktop environment."
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_BLUECURVE_THEME_DESC			"The Bluecurve theme."
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_LIGHTHOUSEBLUE_THEME_DESC		"The Lighthouseblue theme."
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SHORTCUTS_SECTION_DESCRIPTION	"Shortcuts for starting Ekiga"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_DESKTOP_SHORTCUT_DESC		"Create a shortcut to Ekiga on the Desktop"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_STARTMENU_SHORTCUT_DESC		"Create a Start Menu entry for Ekiga"

; GTK+ Directory Page
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_UPGRADE_PROMPT			"An old version of the GTK+ runtime was found. Do you wish to upgrade?$\rNote: Ekiga may not work unless you do."

; Installer Finish Page
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_FINISH_VISIT_WEB_SITE		"Visit the Windows Ekiga Web Page"

; Ekiga Section Prompts and Texts
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_UNINSTALL_DESC			"Ekiga (remove only)"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_RUN_AT_STARTUP			"Run Ekiga at Windows startup"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_PROMPT_CONTINUE_WITHOUT_UNINSTALL	"Unable to uninstall the currently installed version of Ekiga. The new version will be installed without removing the currently installed version."

; GTK+ Section Prompts
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_INSTALL_ERROR			"Error installing GTK+ runtime. Would you like to continue anyway?"
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_BAD_INSTALL_PATH			"The path you entered can not be accessed or created."

; GTK+ Themes section
!insertmacro EKIGA_MACRO_DEFAULT_STRING GTK_NO_THEME_INSTALL_RIGHTS		"You do not have permission to install a GTK+ theme."

; Uninstall Section Prompts
!insertmacro EKIGA_MACRO_DEFAULT_STRING un.EKIGA_UNINSTALL_ERROR_1		"The uninstaller could not find registry entries for Ekiga.$\rIt is likely that another user installed this application."
!insertmacro EKIGA_MACRO_DEFAULT_STRING un.EKIGA_UNINSTALL_ERROR_2		"You do not have permission to uninstall this application."

; Spellcheck Section Prompts
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_SECTION_TITLE		"Spellchecking Support"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_ERROR			"Error Installing Spellchecking"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_DICT_ERROR		"Error Installing Spellchecking Dictionary"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_SECTION_DESCRIPTION	"Support for Spellchecking.  (Internet connection required for installation)"
!insertmacro EKIGA_MACRO_DEFAULT_STRING ASPELL_INSTALL_FAILED			"Installation Failed"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_BRETON			"Breton"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_CATALAN			"Catalan"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_CZECH			"Czech"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_WELSH			"Welsh"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_DANISH			"Danish"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_GERMAN			"German"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_GREEK			"Greek"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_ENGLISH			"English"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_ESPERANTO		"Esperanto"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_SPANISH			"Spanish"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_FAROESE			"Faroese"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_FRENCH			"French"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_ITALIAN			"Italian"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_DUTCH			"Dutch"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_NORWEGIAN		"Norwegian"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_POLISH			"Polish"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_PORTUGUESE		"Portuguese"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_ROMANIAN			"Romanian"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_RUSSIAN			"Russian"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_SLOVAK			"Slovak"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_SWEDISH			"Swedish"
!insertmacro EKIGA_MACRO_DEFAULT_STRING EKIGA_SPELLCHECK_UKRAINIAN		"Ukrainian"

