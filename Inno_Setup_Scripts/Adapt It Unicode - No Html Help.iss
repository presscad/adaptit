; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

; edb 11 Oct 2013: Include InnoTools Downloader for installing Git
; (this is the local version in the Inno_Setup_Scripts directory). This needs
; the following:
; - itdownload.dll    // DLL that allows us to download 3rd party apps
; - it_download.iss   // ITD script to connect the DLL
; whm 25 Mar 2017: Code changes to enhance the installer and provide more flexible and 
; economic Git installation options. See also the Adapt It Unicode Git.iss script
; which creates a special Git_Downloader2_12_1_4AI.exe installer that is now included in the [Files]
; section below. The Git_Downloader2_12_1_4AI.exe installer is used by the main Adapt It program
; when it is desired to install a missing Git program that didn't get installed at the
; time Adapt It itself was installed (or removed after Adapt It was installed).
; IMPORTANT: RUN THE Adapt It Unicode Git.iss SCRIPT TO CREATE Git_Downloader2_12_1_4AI.exe
; BEFORE COMPILING THIS SCRIPT
; whm 4 April 2017: Removed rdwrtp7.exe and related Windows dlls from the Adapt It installers.
#include "it_download.iss"

#define MyAppName "Adapt It WX Unicode"
#define MyAppVersion "6.8.3"
#define MyAppURL "http://www.adapt-it.org/"
#define MyAppExeName "Adapt_It_Unicode.exe"
#define MyAppShortName "Adapt It"
#define SvnBase ".."

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppID={{7317EA81-BC6E-4A4F-AE2B-44ADE6A2188F}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\Adapt It WX Unicode
DefaultGroupName=Adapt It WX Unicode
LicenseFile={#SvnBase}\setup Unicode - No Html Help\LICENSING.txt
InfoBeforeFile={#SvnBase}\setup Unicode - No Html Help\Readme_Unicode_Version.txt
OutputBaseFilename=Adapt_It_WX_6_8_3_Unicode_No_HTML_HELP
SetupIconFile={#SvnBase}\res\ai_32.ico
Compression=lzma/Max
SolidCompression=true
OutputDir={#SvnBase}\AIWX Installers
VersionInfoCopyright=2017 by Bruce Waters, Bill Martin, SIL International
VersionInfoProductName=Adapt It WX Unicode
VersionInfoProductVersion=6.8.3
WizardImageFile="{#SvnBase}\res\ai_wiz_bg.bmp"
WizardSmallImageFile="{#SvnBase}\res\AILogo32x32.bmp"
WizardImageStretch=false
AppCopyright=2016 Bruce Waters, Bill Martin, SIL International
PrivilegesRequired=poweruser
DirExistsWarning=no
VersionInfoVersion=6.8.3
VersionInfoCompany=SIL
VersionInfoDescription=Adapt It WX Unicode
UsePreviousGroup=false
UsePreviousAppDir=false
DisableWelcomePage=true

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "{#SvnBase}\setup Unicode - No Html Help\Adapt_It_Unicode.exe"; DestDir: "{app}"; Flags: ignoreversion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Adapt It changes.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\Adapt_It_Quick_Start.htm"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\Help_for_Administrators.htm"; DestDir: "{app}"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\RFC5646message.htm"; DestDir: "{app}"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\GuesserExplanation.htm"; DestDir: "{app}"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Adapt It Reference.doc"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\Adapt It Tutorial.doc"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\AI_UserProfiles.xml"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\AI_USFM.xml"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\books.xml"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\curl-ca-bundle.crt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\iso639-3codes.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\aiDefault.css"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\KJV 1Jn 2.12-17.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\Known Issues and Limitations.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\LICENSING.txt"; DestDir: "{app}"; Flags: ignoreversion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\License_CPLv05.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\License_GPLv2.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\License_LGPLv21.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\Localization_Readme.txt"; DestDir: "{app}"; Flags: ignoreversion
; Source: "{#SvnBase}\setup Unicode - No Html Help\rdwrtp7.exe"; DestDir: "{app}"; Flags: ignoreversion
; Source: "{#SvnBase}\setup Unicode - No Html Help\ParatextShared.dll"; DestDir: "{app}"; Flags: IgnoreVersion
; Source: "{#SvnBase}\setup Unicode - No Html Help\ICSharpCode.SharpZipLib.dll"; DestDir: "{app}"; Flags: IgnoreVersion
; Source: "{#SvnBase}\setup Unicode - No Html Help\Interop.XceedZipLib.dll"; DestDir: "{app}"; Flags: IgnoreVersion
; Source: "{#SvnBase}\setup Unicode - No Html Help\NetLoc.dll"; DestDir: "{app}"; Flags: IgnoreVersion
; Source: "{#SvnBase}\setup Unicode - No Html Help\Utilities.dll"; DestDir: "{app}"; Flags: IgnoreVersion
Source: "{#SvnBase}\setup Unicode - No Html Help\Readme_Unicode_Version.txt"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: "{#SvnBase}\setup Unicode - No Html Help\SILConverters in AdaptIt.doc"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\Tok Pisin fragment 1John.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode - No Html Help\Images\Admin_help\*.gif"; DestDir: "{app}\Images\Admin_help\"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Images\Adapt_It_Quick_Start\*.gif"; DestDir: "{app}\Images\Adapt_It_Quick_Start\"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\az\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\az"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\az\az.po"; DestDir: "{app}\Languages\az"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\es\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\es"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\es\es.po"; DestDir: "{app}\Languages\es"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\es\wxstd.mo"; DestDir: "{app}\Languages\es"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\fr\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\fr"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\fr\fr.po"; DestDir: "{app}\Languages\fr"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\fr\wxstd.mo"; DestDir: "{app}\Languages\fr"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\id\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\id"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\id\id.po"; DestDir: "{app}\Languages\id"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\pt\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\pt"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\pt\pt.po"; DestDir: "{app}\Languages\pt"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\pt\wxstd.mo"; DestDir: "{app}\Languages\pt"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\ru\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\ru"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\ru\ru.po"; DestDir: "{app}\Languages\ru"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\ru\wxstd.mo"; DestDir: "{app}\Languages\ru"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\swh\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\swh"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\swh\swh.po"; DestDir: "{app}\Languages\swh"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\tpi\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\tpi"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\tpi\tpi.po"; DestDir: "{app}\Languages\tpi"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\tpi\books_tpi.xml"; DestDir: "{app}\Languages\tpi"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\tpi\tpi_readme.txt"; DestDir: "{app}\Languages\tpi"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\tpi\wxstd.mo"; DestDir: "{app}\Languages\tpi"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\zh\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\zh"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\zh\zh.po"; DestDir: "{app}\Languages\zh"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\Languages\zh\wxstd.mo"; DestDir: "{app}\Languages\zh"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\CC\Ansi2Utf8.exe"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\CC\CC.doc"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\CC\CC.hlp"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\CC\cc32.dll"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\CC\CCDebug.doc"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\CC\CCFiles.doc"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\CC\CCW32.exe"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\CC\CCW32.INI"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\CC\reverse_lx_ge.cct"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\CC\FwdSlashInsertAtPuncts.cct"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\CC\FwdSlashRemoveAtPuncts.cct"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\CC\Summary.doc"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode - No Html Help\CC\table series as one.cct"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 

[Registry]
Root: HKCU; Subkey: "Environment"; ValueName: "Path"; ValueType: "string"; ValueData: "{pf}\Git\bin;\{pf}\Git\cmd;{olddata}"; Check: NotOnPathAlready(); Flags: preservestringtype;
;Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{pf}\Git\bin;{pf}\Git\cmd;{olddata}"; Check: NotOnPathAlready(); Flags: preservestringtype;

[Icons]
Name: {group}\_{#MyAppName}; Filename: {app}\{#MyAppExeName}; WorkingDir: {app}; Comment: "Launch Adapt It Unicode"; 
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppShortName}}"; Filename: {#MyAppURL}; Comment: "Go to the Adapt It website at http://adapt-it.org"; 
Name: "{group}\Adapt It Quick Start"; Filename: "{app}\Adapt_It_Quick_Start.htm"; WorkingDir: "{app}"; Comment: "Launch Adapt It Quick Start in browser"; 
Name: "{group}\Help for Administrators (HTML)"; Filename: "{app}\Help_for_Administrators.htm"; WorkingDir: "{app}"; Comment: "Launch Help for Administrators"; 
Name: "{group}\Adapt It Tutorial"; Filename: "{app}\Adapt It Tutorial.doc"; WorkingDir: "{app}"; Comment: "Launch Adapt It Tutorial.doc in word processor"; 
Name: "{group}\Adapt It Reference"; Filename: "{app}\Adapt It Reference.doc"; WorkingDir: "{app}"; Comment: "Launch Adapt It Reference.doc in word processor"; 
Name: "{group}\Adapt It Changes"; Filename: "{app}\Adapt It changes.txt"; WorkingDir: "{app}"; Comment: "Launch Adapt It changes.txt in Notepad"; 
Name: {group}\Uninstall; Filename: {uninstallexe}; WorkingDir: {app}; Comment: "Uninstall Adapt It Unicode from this computer"; 
Name: {commondesktop}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: desktopicon; 
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\Adapt It Unicode"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\CC\Consistent Changes for Windows"; Filename: "{app}\CC\CCW32.exe"; WorkingDir: "{app}\CC"; IconFilename: "{app}\CC\CCW32.exe"; Comment: "Launch Consistent Changes GUI program"; 
Name: "{group}\Consistent Changes (standalone)"; Filename: "{app}\CC\CCW32.exe"; WorkingDir: "{app}\CC"; IconFilename: "{app}\CC\CCW32.exe"; Comment: "Launch Consistent Changes GUI program"; 
Name: "{group}\CC\CC Summary Document"; Filename: {app}\CC\Summary.doc; WorkingDir: {app}\CC; 
Name: "{group}\CC\Consistent Changes Documentation"; Filename: {app}\CC\CC.doc; WorkingDir: {app}\CC; 
Name: "{group}\CC\CC Files Document"; Filename: {app}\CC\CCFiles.doc; WorkingDir: {app}\CC; 
Name: "{group}\CC\CC Debug Document"; Filename: {app}\CC\CCDebug.doc; WorkingDir: {app}\CC; 

; edb 11 Oct 2013: Code changes to download / install Git
; whm 23 Mar 2017: Code changes to download / install Git
[Code]
//const GitSetupURL = 'https://github.com/msysgit/msysgit/releases/download/Git-1.9.5-preview20150319/Git-1.9.5-preview20150319.exe';
//const GitSetupURL = 'https://github.com/git-for-windows/git/releases/download/v2.6.2.windows.1/Git-2.6.2-32-bit.exe';
// whm 23Mar2017 updated the 32-bit Git download URL from adapt-it.org to Git version 2.12.1.
const GitSetupURL = 'http://www.adapt-it.org/Git-2.12.1-32-bit.exe';
//const GitSetupURL64 = 'https://github.com/git-for-windows/git/releases/download/v2.6.2.windows.1/Git-2.6.2-64-bit.exe';
var GitInstalled: Boolean;  // Is Git installed?
var ShouldInstallGit: Boolean; // should the installer download and run the Git installer?
var tmpResult: Integer;     
var GitName: string;
var msg: string;

procedure InitializeWizard();
begin
    GitInstalled := False;
    ShouldInstallGit := False;
    GitName := expandconstant('{tmp}\GitInstaller.exe');
    ITD_Init; // initialize the InnoTools Downloader
    itd_downloadafter(wpReady);

    // Test for Git by looking for its uninstaller in the registry
    // whm modified 22March2017 Registry keys are not supposed to be case sensitive, but in testing
    // if the key has ...\Wow6432Node\ instead of ...\WOW6432Node\... the test below fails. Probably
    // the failure is due to the RegKeyExists() function making a case-sensitive comparison. This used
    // to work, so perhaps an earlier version of Inno Setup had a non-case-sensitive comparison? Maybe
    // some versions of Windows store the key as Wow6432Node and other versions as WOW6432Node. In any case
    // to prevent problems for the Adapt It installer, we'll test key strings containing both Wow6432Node 
    // and WOW6432Node.
    // check for 64-bit Windows
    if (RegKeyExists(HKLM, 'SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Git_is1')) then
        GitInstalled := True;
    if (RegKeyExists(HKLM, 'SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Git_is1')) then
        GitInstalled := True;
    // check for 32-bit Windows
    if (RegKeyExists(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Git_is1')) then
        GitInstalled := True;
end;

procedure CurPageChanged(CurPageID: Integer);
begin
  if CurPageID=wpReady then begin //Lets install those files that were downloaded for us
    if (GitInstalled = False) then
      if (MsgBox('Adapt It needs to install a program called Git in order to track changes in your translated documents. Would you like to download and install this program?', mbConfirmation, MB_YESNO) = IDYES) then
        // user clicked YES -- install Git from the internet.
        begin
          // download the Git installer after the "ready to install" screen is shown
          ShouldInstallGit := True;
          ITD_AddFile(GitSetupURL, GitName);
        end
      else begin
        msg := 'You chose not to download and install the Git program as part of the Adapt It installation. Adapt It will still run, but it will not be able to track changes in your translated documents (nor be able to restore a previous version) until you install the Git program on this computer. If you have not previously downloaded the Git installer (29MB) and you have Internet access, the recommended way to obtain the Git program is to run the Adapt It installer again and choose to have it automatically download and install the Git program with the correct settings.' + Chr(13) + Chr(13);
        msg := msg + 'If you have no Internet access (or it is too slow/expensive) at the time you wish to Install Adapt It, the Git installer can be downloaded separately (from: http://git-scm.com/downloads) and installed at a later time after this installer has finished.';
        MsgBox(msg, mbInformation, MB_OK);
      end
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep=ssPostInstall then
    // Silently run the Git installer as a post-install step
    if (ShouldInstallGit = True) then begin
      // run the git installer silently, with the options loaded from the file
      // ai_git.inf. The git installer for Windows is made in Inno as well;
      // more info on the command line options for Inno installers can be
      // found here: http://www.jrsoftware.org/ishelp/index.php?topic=setupcmdline
      Exec(GitName, '/SILENT', '', SW_SHOW, ewWaitUntilTerminated, tmpResult);
    end;
end;

function NotOnPathAlready(): Boolean;
var
  BinDir, Path: String;
begin
  if RegQueryStringValue(HKEY_CURRENT_USER, 'Environment', 'Path', Path) then
  begin // Successfully read the value
    Log('HKCU\Environment\PATH = ' + Path);
    BinDir := ExpandConstant('{pf}\Git\bin');
    Log('Looking for Git\bin dir in %PATH%: ' + BinDir + ' in ' + Path);
    if Pos(LowerCase(BinDir), Lowercase(Path)) = 0 then
    begin
      Log('Did not find Git\bin dir in %PATH% so will add it');
      Result := True;
    end
    else
    begin
      Log('Found Git\bin dir in %PATH% so will not add it again');
      Result := False;
    end
  end
  else // The key probably doesn't exist
  begin
    Log('Could not access HKCU\Environment\PATH so assume it is ok to add it');
    Result := True;
  end;
end;

function NeedRestart(): Boolean;
begin
  // If Git was installed, prompt the user to reboot after everything's done
  Result := ShouldInstallGit;
end;
// end EDB Oct 2013

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, "&", "&&")}}"; Flags: nowait postinstall skipifsilent
