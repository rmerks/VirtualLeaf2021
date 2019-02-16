# $Id$

# appends \ to the path if missing
# example: !insertmacro GetCleanDir "c:\blabla"
# Pop $0 => "c:\blabla\"
!macro GetCleanDir INPUTDIR
  ; ATTENTION: USE ON YOUR OWN RISK!
  ; Please report bugs here: http://stefan.bertels.org/
  !define Index_GetCleanDir 'GetCleanDir_Line${__LINE__}'
  Push $R0
  Push $R1
  StrCpy $R0 "${INPUTDIR}"
  StrCmp $R0 "" ${Index_GetCleanDir}-finish
  StrCpy $R1 "$R0" "" -1
  StrCmp "$R1" "\" ${Index_GetCleanDir}-finish
  StrCpy $R0 "$R0\"
${Index_GetCleanDir}-finish:
  Pop $R1
  Exch $R0
  !undef Index_GetCleanDir
!macroend
 
# similar to "RMDIR /r DIRECTORY", but does not remove DIRECTORY itself
# example: !insertmacro RemoveFilesAndSubDirs "$INSTDIR"
!macro RemoveFilesAndSubDirs DIRECTORY
  # ATTENTION: USE ON YOUR OWN RISK!
  # Please report bugs here: http://stefan.bertels.org/
  !define Index_RemoveFilesAndSubDirs 'RemoveFilesAndSubDirs_${__LINE__}'
 
  Push $R0
  Push $R1
  Push $R2
 
  !insertmacro GetCleanDir "${DIRECTORY}"
  Pop $R2
  FindFirst $R0 $R1 "$R2*.*"
${Index_RemoveFilesAndSubDirs}-loop:
  StrCmp $R1 "" ${Index_RemoveFilesAndSubDirs}-done
  StrCmp $R1 "." ${Index_RemoveFilesAndSubDirs}-next
  StrCmp $R1 ".." ${Index_RemoveFilesAndSubDirs}-next
  IfFileExists "$R2$R1\*.*" ${Index_RemoveFilesAndSubDirs}-directory
  ; file
  Delete "$R2$R1"
  goto ${Index_RemoveFilesAndSubDirs}-next
${Index_RemoveFilesAndSubDirs}-directory:
  ; directory
  RMDir /r "$R2$R1"
${Index_RemoveFilesAndSubDirs}-next:
  FindNext $R0 $R1
  Goto ${Index_RemoveFilesAndSubDirs}-loop
${Index_RemoveFilesAndSubDirs}-done:
  FindClose $R0
 
  Pop $R2
  Pop $R1
  Pop $R0
  !undef Index_RemoveFilesAndSubDirs
!macroend

!define PRODUCT_NAME "The Virtual Leaf"
!define PRODUCT_VERSION "1.0.1"
!define PRODUCT_PUBLISHER "Center for Mathematics and Computer Science (CWI)"
!define PRODUCT_WEB_SITE "www.cwi.nl"
!define EXECUTABLE "VirtualLeaf.exe"
!define PROGICON "leaficon.ico"
!define SETUP_BITMAP "leaficon.ico"

# MUI 1.67 compatible ------
!include "MUI.nsh"
!include "EnvVarUpdate.nsh"

# MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "leaficon.ico"
!define MUI_UNICON "leaficon.ico"

# Welcome page
!insertmacro MUI_PAGE_WELCOME

# License page
!insertmacro MUI_PAGE_LICENSE "..\doc\gpl3.txt"

# Components page
!insertmacro MUI_PAGE_COMPONENTS

# Directory page
!insertmacro MUI_PAGE_DIRECTORY

# Instfiles page
!insertmacro MUI_PAGE_INSTFILES

# Finish page
!insertmacro MUI_PAGE_FINISH

# Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

# Language files
!insertmacro MUI_LANGUAGE "English"

# MUI end ------

# set the name of the installer
outfile "VirtualLeaf-install.exe"
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
ShowInstDetails show
ShowUnInstDetails show
 
# define the directory to install to
installDir C:\VirtualLeaf

# Request application privileges
RequestExecutionLevel user

section "Virtual Leaf executable"
  #sectionIn RO
  # define the output path for the Virtual Leaf executable
  setOutPath $INSTDIR\bin
  writeUninstaller $INSTDIR\uninstaller.exe
  file ..\bin\VirtualLeaf.exe

  # Required DLLs
  setOutPath $INSTDIR\bin

  file ..\lib\libiconv\bin\libiconv-2.dll
  file ..\lib\libxml2\lib\libxml2.dll
  file ..\lib\libz\bin\zlib1.dll

  # Is the pegging of these DLLs to a specific QT version a problem?
  file C:\Qt\2010.02.1\mingw\bin\mingwm10.dll
  file C:\Qt\2010.02.1\mingw\bin\libgcc_s_dw2-1.dll
  file C:\Qt\2010.02.1\qt\bin\Qt3Support4.dll
  file C:\Qt\2010.02.1\qt\bin\QtCore4.dll
  file C:\Qt\2010.02.1\qt\bin\QtGui4.dll
  file C:\Qt\2010.02.1\qt\bin\QtNetwork4.dll
  file C:\Qt\2010.02.1\qt\bin\QtSql4.dll
  file C:\Qt\2010.02.1\qt\bin\QtXml4.dll

  # Prepend the Virtual Leaf bin directory to the installers PATH
  ${EnvVarUpdate} $0 "PATH" "P" "HKCU" "$INSTDIR\bin"  

  # create a shortcut named "VirtualLeaf" in the start menu programs directory
  # point the new shortcut at the program VirtualLeaf
  createShortCut "$SMPROGRAMS\VirtualLeaf.lnk" "$INSTDIR\bin\VirtualLeaf.exe"
sectionEnd

section "Image Formats"
  # define the output path for the Qt Image Formats
  setOutPath $INSTDIR\bin\imageformats
  file C:\Qt\2010.02.1\qt\plugins\imageformats\qgif4.dll
  file C:\Qt\2010.02.1\qt\plugins\imageformats\qico4.dll
  file C:\Qt\2010.02.1\qt\plugins\imageformats\qjpeg4.dll
  file C:\Qt\2010.02.1\qt\plugins\imageformats\qmng4.dll
  file C:\Qt\2010.02.1\qt\plugins\imageformats\qsvg4.dll
  file C:\Qt\2010.02.1\qt\plugins\imageformats\qtiff4.dll
sectionEnd


section "Virtual Leaf plugins"
  # define the output path for the Virtual Leaf models
  setOutPath $INSTDIR\bin\models
  file ..\bin\models\*
sectionEnd

section "Virtual Leaf data"
  # define the output path for the Virtual Leaf models
  setOutPath $INSTDIR\data\leaves
  file ..\data\leaves\*
sectionEnd

section "Virtual Leaf docs"
  # define the output path for the Virtual Leaf documentation
  setOutPath $INSTDIR\doc
  file ..\doc\*
sectionEnd

# create a section to define what the uninstaller does.
# the section will always be named "Uninstall"
section "Uninstall"

  # Always delete uninstaller first
  delete $INSTDIR\uninstaller.exe
 
  # Now delete installed file
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR"

  # Delete shortcut from start menu
  delete "$SMPROGRAMS\VirtualLeaf.lnk"

  # Last but not least - delete the path  
  ${un.EnvVarUpdate} $0 "PATH" "R" "HKCU" "$INSTDIR\bin"
sectionEnd

# finis
