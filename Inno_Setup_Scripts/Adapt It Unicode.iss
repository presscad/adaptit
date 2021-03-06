; IMPORTANT: RUN THE Adapt It Unicode Git.iss SCRIPT TO CREATE Git_Downloader2_12_1_4AI.exe
; BEFORE COMPILING THIS SCRIPT
; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

; edb 11 Oct 2013: Include InnoTools Downloader for installing Git
; (this is the local version in the Inno_Setup_Scripts directory). This needs
; the following:
; - itdownload.dll    // DLL that allows us to download 3rd party apps
; - it_download.iss   // ITD script to connect the DLL
; whm 25 Mar 2017: Code changes to enhance the installer and provide more flexible and 
; economic Git installation options. 
; whm 25Jun2018 Updated version of Git to 2.18.0 and Git_Downloader to 2_18_0.
; See also the Adapt It Unicode Git.iss script which creates a special 
; Git_Downloader2_18_0_4AI.exe installer that is now included in the [Files] section below. 
; The Git_Downloader2_18_0_4AI.exe installer is used by the main Adapt It program
; when it is desired to install a missing Git program that didn't get installed at the
; time Adapt It itself was installed (or removed after Adapt It was installed).
; IMPORTANT: RUN THE Adapt It Unicode Git.iss SCRIPT TO CREATE Git_Downloader2_18_0_4AI.exe
; BEFORE COMPILING THIS SCRIPT
; whm 4 April 2017: Removed rdwrtp7.exe and related Windows dlls from the Adapt It installers.
#include "it_download.iss"

#define MyAppName "Adapt It WX Unicode"
#define MyAppVersion "6.10.1"
#define MyAppPublisher "Adapt It"
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
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\Adapt It WX Unicode
DefaultGroupName=Adapt It WX Unicode
LicenseFile={#SvnBase}\setup Unicode\LICENSING.txt
InfoBeforeFile={#SvnBase}\setup Unicode\Readme_Unicode_Version.txt
OutputBaseFilename=Adapt_It_WX_6_10_1_Unicode
SetupIconFile={#SvnBase}\res\ai_32.ico
Compression=lzma/Max
SolidCompression=true
OutputDir={#SvnBase}\AIWX Installers
VersionInfoCopyright=2020 by Bruce Waters, Bill Martin, SIL International
VersionInfoProductName=Adapt It WX Unicode
VersionInfoProductVersion=6.10.1
WizardImageFile="{#SvnBase}\res\ai_wiz_bg.bmp"
WizardSmallImageFile="{#SvnBase}\res\AILogo32x32.bmp"
WizardImageStretch=false
AppCopyright=2020 Bruce Waters, Bill Martin, SIL International
PrivilegesRequired=poweruser
DirExistsWarning=no
VersionInfoVersion=6.10.1
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
Source: "{#SvnBase}\setup Unicode\Adapt_It_Unicode.exe"; DestDir: "{app}"; Flags: ignoreversion; 
Source: "{#SvnBase}\setup Git\Git_Downloader_2_18_0_4AI.exe"; DestDir: "{app}"; Flags: ignoreversion; 
Source: "{#SvnBase}\setup Unicode\Adapt It changes.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\Adapt_It_Quick_Start.htm"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\Help_for_Administrators.htm"; DestDir: "{app}"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\RFC5646message.htm"; DestDir: "{app}"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\GuesserExplanation.htm"; DestDir: "{app}"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Adapt It Reference.doc"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\Adapt It Tutorial.doc"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\Adapt_It_Unicode.htb"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\AI_UserProfiles.xml"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\AI_USFM.xml"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\books.xml"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\curl-ca-bundle.crt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\iso639-3codes.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\aiDefault.css"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\KJV 1Jn 2.12-17.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\Known Issues and Limitations.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\LICENSING.txt"; DestDir: "{app}"; Flags: ignoreversion; 
Source: "{#SvnBase}\setup Unicode\License_CPLv05.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\License_GPLv2.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\License_LGPLv21.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\Localization_Readme.txt"; DestDir: "{app}"; Flags: ignoreversion
; Source: "{#SvnBase}\setup Unicode\rdwrtp7.exe"; DestDir: "{app}"; Flags: ignoreversion
; Source: "{#SvnBase}\setup Unicode\ParatextShared.dll"; DestDir: "{app}"; Flags: IgnoreVersion
; Source: "{#SvnBase}\setup Unicode\ICSharpCode.SharpZipLib.dll"; DestDir: "{app}"; Flags: IgnoreVersion
; Source: "{#SvnBase}\setup Unicode\Interop.XceedZipLib.dll"; DestDir: "{app}"; Flags: IgnoreVersion
; Source: "{#SvnBase}\setup Unicode\NetLoc.dll"; DestDir: "{app}"; Flags: IgnoreVersion
; Source: "{#SvnBase}\setup Unicode\Utilities.dll"; DestDir: "{app}"; Flags: IgnoreVersion
Source: "{#SvnBase}\setup Unicode\Readme_Unicode_Version.txt"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: "{#SvnBase}\setup Unicode\SILConverters in AdaptIt.doc"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\Tok Pisin fragment 1John.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SvnBase}\setup Unicode\Images\Admin_help\AdminMenuSetupBECollab.gif"; DestDir: "{app}\Images\Admin_help\"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Images\Admin_help\AdminMenuSetupPTCollab.gif"; DestDir: "{app}\Images\Admin_help\"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Images\Admin_help\*.gif"; DestDir: {app}\Images\Admin_help\; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Images\Adapt_It_Quick_Start\*.gif"; DestDir: "{app}\Images\Adapt_It_Quick_Start\"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\az\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\az"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\az\az.po"; DestDir: "{app}\Languages\az"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\es\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\es"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\es\es.po"; DestDir: "{app}\Languages\es"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\es\wxstd.mo"; DestDir: "{app}\Languages\es"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\fr\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\fr"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\fr\fr.po"; DestDir: "{app}\Languages\fr"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\fr\wxstd.mo"; DestDir: "{app}\Languages\fr"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\id\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\id"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\id\id.po"; DestDir: "{app}\Languages\id"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\pt\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\pt"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\pt\pt.po"; DestDir: "{app}\Languages\pt"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\pt\wxstd.mo"; DestDir: "{app}\Languages\pt"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\ru\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\ru"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\ru\ru.po"; DestDir: "{app}\Languages\ru"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\ru\wxstd.mo"; DestDir: "{app}\Languages\ru"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\swh\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\swh"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\swh\swh.po"; DestDir: "{app}\Languages\swh"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\tpi\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\tpi"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\tpi\tpi.po"; DestDir: "{app}\Languages\tpi"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\tpi\books_tpi.xml"; DestDir: "{app}\Languages\tpi"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\tpi\tpi_readme.txt"; DestDir: "{app}\Languages\tpi"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\tpi\wxstd.mo"; DestDir: "{app}\Languages\tpi"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\zh\Adapt_It_Unicode.mo"; DestDir: "{app}\Languages\zh"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\zh\zh.po"; DestDir: "{app}\Languages\zh"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\Languages\zh\wxstd.mo"; DestDir: "{app}\Languages\zh"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\CC\Ansi2Utf8.exe"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\CC\CC.doc"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\CC\CC.hlp"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\CC\cc32.dll"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\CC\CCDebug.doc"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\CC\CCFiles.doc"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\CC\CCW32.exe"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\CC\CCW32.INI"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\CC\reverse_lx_ge.cct"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\CC\FwdSlashInsertAtPuncts.cct"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\CC\FwdSlashRemoveAtPuncts.cct"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\CC\Summary.doc"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 
Source: "{#SvnBase}\setup Unicode\CC\table series as one.cct"; DestDir: "{app}\CC"; Flags: IgnoreVersion; 

[Registry]
Root: HKCU; Subkey: "Environment"; ValueName: "Path"; ValueType: "string"; ValueData: "{pf}\Git\bin;\{pf}\Git\cmd;{olddata}"; Check: NotOnPathAlready(); Flags: preservestringtype;
;Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{pf}\Git\bin;{pf}\Git\cmd;{olddata}"; Check: NotOnPathAlready(); Flags: preservestringtype;

[Icons]
Name: {group}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; WorkingDir: {app}; Comment: "Launch Adapt It Unicode"; 
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
; whm 25 Mar 2017: Code changes to enhance the installer and provide more flexible and 
; economic Git installation options.
[Code]
//const GitSetupURL = 'https://github.com/msysgit/msysgit/releases/download/Git-1.9.5-preview20150319/Git-1.9.5-preview20150319.exe';
//const GitSetupURL = 'https://github.com/git-for-windows/git/releases/download/v2.6.2.windows.1/Git-2.6.2-32-bit.exe';
// whm 23Mar2017 updated the 32-bit Git download URL from adapt-it.org to Git version 2.12.1.
// whm 25Jun2018 updated the 32-bit Git download URL from adapt-it.org to Git version 2.18.0
const GitInstallerFileName = 'Git-2.18.0-32-bit.exe';
const GitSetupURL = 'http://www.adapt-it.org/' + GitInstallerFileName;
//const GitSetupURL64 = 'https://github.com/git-for-windows/git/releases/download/v2.6.2.windows.1/Git-2.6.2-64-bit.exe';
  Btn1Desc = 'Only install Adapt It. I will use Adapt It without Git, or I will install Git later.';
  Btn2Desc = 'This option requires access to the Internet and will download about 36MB of data. ' +
             'A copy of the Git installer is saved to the Adapt It installation directory.';
  Btn3Desc = 'This option installs Git using a previously downloaded Git installer. ' +
             'Git can be installed this way without accesing the Internet.';
  GitDesc = 'To track changes in your translated documents, Adapt It uses a program called Git. ' +
            'You have the following Git install options:';

var GitInstalled: Boolean;  // Is Git installed?
  ShouldInstallGit: Boolean; // should the installer download and run the Git installer?
  GitInstallerExistsLocally: Boolean; // can use existing Git installer if it exists
  tmpResult: Integer;
  PathToAIInstallation: string; // gets the result ofexpandconstant('{app}') after "Next" from wpSelectDir page     
  GitInstallerPathAndName: string;  // location and name of the local copy of the Git installer, normally C:\Program Files (x86)\Adapt It WX Unicode\Git-2.12.1-32-bit.exe
  msg: string;
  GitInstallOptionsPageID: Integer;
  GitInstallOptionsPage: TWizardPage;
  NoInsGitRadioButton: TNewRadioButton;
  NoInsGitDescLabel: TLabel;
  InsGitFmNetRadioButton: TNewRadioButton;
  InsGitFmNetDescLabel: TLabel;
  InsGitFmFileRadioButton: TNewRadioButton;
  InsGitFmFileDescLabel: TLabel;

function CreateRadioButton(rbParent: TWizardPage; rbTop: Integer; rbChecked: Boolean; 
         rbCaption: string): TNewRadioButton;
begin
  Result := TNewRadioButton.Create(WizardForm);
  with Result do
  begin
    Parent := rbParent.Surface;
    Top := rbTop;
    Width := rbParent.SurfaceWidth;
    Font.Style := [fsBold];
    Font.Size := 9
    Caption := rbCaption;
    Checked := rbChecked;
  end;
end;

function CreateBtnDescLabelBelowBtn (rbParent: TWizardPage; rbTop: Integer; 
         rbCaption: string): TLabel;
begin
  Result := TLabel.Create(WizardForm);
  with Result do
  begin
    Parent := rbParent.Surface;
    Left := 12;
    Top := rbTop;
    Width := rbParent.SurfaceWidth; 
    Height := 30;
    AutoSize := False;
    Wordwrap := True;
    Caption := rbCaption;
  end
end;

// This InitializeWizard() procedure is an event function where we can make changes to the wizard 
// or wizard pages at startup. We create our custom wizard page here with its 3 radio button 
// controls. You can't use the InitializeSetup event function for this since at the time it 
// is triggered, the wizard form does not yet exist.
procedure InitializeWizard();
begin

    // whm addition 24March2017
    // ========= Create Custom GitInstallOptionsPage with 3 radio buttons and short description labels ========
    GitInstallOptionsPage := CreateCustomPage(wpSelectTasks, 'Adapt It needs to install a program called Git', GitDesc);
    GitInstallOptionsPageID := GitInstallOptionsPage.ID;

    // See the CurPageChanged() procedure for comments about the radio button defaults, which are set in that 
    // procedure to be the most likely selection desired by an administrator/user.
    NoInsGitRadioButton := CreateRadioButton(GitInstallOptionsPage, 12, False, 'Do not try to install Git at this time');
    NoInsGitDescLabel := CreateBtnDescLabelBelowBtn(GitInstallOptionsPage, NoInsGitRadioButton.Top + 20, Btn1Desc);
  
    InsGitFmNetRadioButton := CreateRadioButton(GitInstallOptionsPage, 68, True, 'Download and install Git from the Internet');
    InsGitFmNetDescLabel := CreateBtnDescLabelBelowBtn(GitInstallOptionsPage, InsGitFmNetRadioButton.Top + 20, Btn2Desc);
  
    InsGitFmFileRadioButton := CreateRadioButton(GitInstallOptionsPage, 128, False, 'Browse this computer to find a Git installer');
    InsGitFmFileDescLabel := CreateBtnDescLabelBelowBtn(GitInstallOptionsPage, InsGitFmFileRadioButton.Top + 20, Btn3Desc);
    // ========= Create Custom GitInstallOptionsPage with 3 radio buttons and short description labels ========

    GitInstalled := False;
    ShouldInstallGit := False;
    GitInstallerExistsLocally := False;
    WizardForm.LicenseAcceptedRadio.Checked := True; // make the default choice True

    // Note: Here in InitializeWizard is too early to expand some constants like {app} which is 
    // not initialized until the user has clicked "Next" at the wpSelectDir wizard page. If you 
    // try to get a value for {app} here it results in a run-time error: "attempt to expand the 
    // app var before it was initialized." Exception: {tmp} can be expanded here within
    // InitializeWizard, as Erik employed it here. We now want to keep a copy of the downloaded 
    // installer for possible future use from within Adapt It, or for copying to a thumb drive 
    // for installation on a different computer, without having to do repeated downloads of the 
    // Git installer. Hence, we aren't making use of {tmp} any longer.

    ITD_Init; // initialize the (ITD) InnoTools Downloader
    //ITD_DownloadAfter(wpReady);

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

// The wizard calls this ShouldSkipPage() event function to determine whether or not a 
// particular page (specified by PageID) should be shown at all. If you return True, 
// the page will be skipped; if you return False, the page may be shown. 
// Note: This event function isn't called for the wpPreparing, and wpInstalling pages, 
// nor for pages that Setup has already determined should be skipped (for example, 
// wpSelectComponents in an install containing no components).
function ShouldSkipPage(PageID: Integer): Boolean;
begin
  // Initialize result to not skip any page (not necessary, but safer)
  Result := False;
  // If the page should be skipped (omitted from the wizard) assign Result to True
  if PageID = GitInstallOptionsPageID then
  begin
    Result := GitInstalled;
  end
end;

// The CurPageChanged() procedure is called after a new wizard page (specified by CurPageID) 
// is shown - before the user has a chance to interact with the page. It is therefore one
// place we can set some initial defaults for the controls on the page - possibly as the
// result of previous wizard page settings.
procedure CurPageChanged(CurPageID: Integer);
begin
  if CurPageID = GitInstallOptionsPageID then 
  begin
    // The GitInstallOptionsPage shows in the wizard when GitInstalled is False, so
    // that's how we got here.
    // Try to be smart about what Git install option button should be pre-selected.
    // If there is a local Git installer available, that should probably be the first
    // choice since it will be faster, and cheaper to do the Git installation by just
    // running the local Git installer. So, if we detect the existence of a local Git
    // installer, we set the default radio button of the GitInstallOptionsPage to be 
    // the third option: "Browse this computer to find a Git Installer" (and the other 
    // radio buttons to False).
    // If there is no local copy of the installer where we would expect it, that probably
    // means that this computer has not had Adapt It on it before - at least not installed
    // with this version of our Inno Setup script that saves a copy of the installer in
    // a known place. So, I think next logical button choice would probably be to try 
    // to install Git from the Internet - by pre-selecting the "Download and install Git 
    // from the Internet" button. The installer will balk if the download can't 
    // proceed due to bad or non-existing Internet access. A lot of users, especially 
    // administrators doing the installation of Adapt It on a single machine would have 
    // decent Internet.
    // Although the "Do not try to install Git at this time" option is available I don't
    // thing there is a well-defined scenario where we would have that option be the 
    // initial button selection.
    GitInstallerPathAndName := expandconstant('{app}\' + GitInstallerFileName);
    GitInstallerExistsLocally := FileExists(GitInstallerPathAndName);
    if GitInstallerExistsLocally then
    begin
      NoInsGitRadioButton.Checked := False;
      InsGitFmNetRadioButton.Checked := False;
      InsGitFmFileRadioButton.Checked := True;
    end
    else
    begin
      // Make the initial selection be to Download and install Git from the Internet"
      NoInsGitRadioButton.Checked := False;
      InsGitFmNetRadioButton.Checked := True;
      InsGitFmFileRadioButton.Checked := False;
    end;
  end
end;


// The NextButtonClick() function is called when the user clicks the Next button.
// The CurPage's controls can be read at this point and variables assigned from
// the controls' values.
// If the function is set to a True Result, the wizard will move to the next page.
// If the function is set to a False Result, it will remain on the current page.
function NextButtonClick(CurPage: Integer): Boolean;
var
  Prompt: String;
  FileName: String;
  Filter: String;
  InitialDirectory: String;
  UseFileDialog: Boolean;
begin
  UseFileDialog := False;
  // Note: The wizard pages, when they appear, follow the order they are listed in the
  // case statement below. We only give special treatment for some of them here in
  // NexButtonClick().
	case CurPage of
		wpWelcome : // disabled in [Setup]
		  ;
		wpLicense : // shows content of LICENSING.txt
		  ;
		wpInfoBefore : // shows content of Readme_Unicode_Version.txt
		  ;
		wpSelectDir :
    begin
      // Previous version of script downloaded the Git installer to a tmp file called GitInstaller.exe
      // and the tmp download gets automatically removed after it is executed. We now want to save a
      // copy of the Git installer by its actual name (with version numbers), so it can be utilized for
      // subsequent downloads on the local computer or copied to a thumb drive to use for installation
      // on other computers.
      GitInstallerPathAndName := expandconstant('{app}\' + GitInstallerFileName);
      PathToAIInstallation := expandconstant('{app}');
      // Create the C:\Program Files (x86)\Adapt It WX Unicode\ dir if it doesn't exist so
      // we can save a copy of the downloaded installer there.
      if DirExists(PathToAIInstallation) = False then
      begin
        CreateDir(PathToAIInstallation);
      end;
      // Check if GitInstaller.exe exists in the {app} folder. If so we can offer to use if the user
      // opts to use a previously downloaded copy of the Git installer.
      GitInstallerExistsLocally := FileExists(GitInstallerPathAndName);
    end;
		wpSelectComponents : // whm Note: this could be used to reduce installers to just one for Windows
		  ;
		wpSelectProgramGroup : // set name of Start Menu Folder (default: Adapt It WX Unicode)
		  ;
		wpSelectTasks :  // provides a checkbox: Create a desktop shortcur
		  ;
    GitInstallOptionsPageID :  // our custom page for setting Git install options
      if NoInsGitRadioButton.Checked then 
      begin
      // Make these statements context sensitive. The administrator may select the "Do not try
      // to install Git at this time" button even before s/he realizes that there is a local copy of 
      // the installer available. 
        msg := 'You chose not to download and install the Git program as part of the Adapt It installation. ' +
               'Adapt It will still run, but it will not be able to track changes in your translated documents ' +
               '(nor be able to restore a previous version) until you install the Git program on this computer. ';
        If GitInstallerExistsLocally then
        begin
          // We remind the administrator/user that a local Git installer is available and we recommend they
          // install Git now while they are in the Adapt It installer.
          msg := msg + 'A Git installer is available on this computer at:' + Chr(13) + 
                 '     ' + GitInstallerPathAndName + '.' + Chr(13) +
                 'That installer could be used to install Git without accessing the Internet. ' +
                 'We recommend that you click on OK below, then click on the Back button and select the ' +
                 'option "Browse this computer to find a Git installer...". ' +
                 'The installer will help you install Git before you finish using this installer.' + Chr(13) + Chr(13);
        end
        else
        begin
          // 
          msg := msg + 'If you have not previously downloaded the Git installer (36MB) and you have Internet access, ' +
               'the recommended way to obtain the Git program is to run the Adapt It installer again and choose ' +
               'to have it automatically download and install the Git program with the correct settings.' + Chr(13) + Chr(13);
        end;
        // Remind the administrator/user that once Git has been downloaded once, that
        // installer will be preserved in the Adapt It installation folder where it could be
        // copied to a USB drive and made available when Adapt It and Git are installed on other
        // computers - making it possible to install Adapt It and Git on multiple computers once
        // the first installation has been done.
        PathToAIInstallation := expandconstant('{app}');
        msg := msg + 'You should know that the Adapt It installer now saves a copy of the Git installer the first time it is ' + 
               'downloaded from the Internet. That copy of the Git installer is preserved in the Adapt It ' +
               'installation folder located at:' + Chr(13) + 
               '     '  + PathToAIInstallation + '.' + Chr(13) + 
               'You can use the local copy of the Git installer to install Git on this computer by running ' +
               'this installer again, or by transfering this installer and the Git installer via thumb drive ' +
               'to the other computer, and running this installer on that other computer.' + Chr(13) + Chr(13);

        // Tell the administrator that if they want to install Git apart from using this installer,
        // they can do so from a cmd terminal call, supplying the /SILENT option on the command line, 
        // which will cause it to be installed without any prompts - accepting all the default options 
        // required during the installation - as this installer does.
        msg := msg + 'The Git installer can be downloaded separately (from: http://git-scm.com/downloads) ' +
               'and installed at a later time apart from using this installer. It you decide to install Git ' +
               'that way, you should run the Git installer in a cmd window using this command:' + Chr(13) +
               '     ' + GitInstallerFileName + ' /SILENT';
        MsgBox(msg, mbInformation, MB_OK);
      end
      else if InsGitFmNetRadioButton.Checked then
      begin
        // GitInstallerPathAndName is set in wpSelectDir case above
        // Tell ITD we want to download the Git installer from the Internet as a post-install step 
        // after the "ready to install" screen is shown. 
        ITD_DownloadAfter(wpReady);
        ShouldInstallGit := True;
        ITD_AddFile(GitSetupURL, GitInstallerPathAndName);
      end
      else if InsGitFmFileRadioButton.Checked then
      begin
        // Check if a suitable Git installer exists in the {app} folder. If so we can offer to use 
        // it since the user has chosen to use a previously downloaded copy of the Git installer.
        // GitInstallerExistsLocally was set in the earlier wpSelectDir wizard page above
        GitInstallerPathAndName := expandconstant('{app}\' + GitInstallerFileName);
        GitInstallerExistsLocally := FileExists(GitInstallerPathAndName);
        if GitInstallerExistsLocally then
        begin
          if (MsgBox('A Git installer is available on this computer at: ' + GitInstallerPathAndName + '.' + Chr(13) +
                 'That installer could be used to install Git without accessing the Internet. ' +
                 'Do you want Adapt It to install Git now using that installer?', mbConfirmation, MB_YESNO) = IDYES) then
          begin
            // Install Git with the /SILENT switch. The Git installer has a nice, compatible GUI that
            // shows progress of the installation.
            Exec(GitInstallerPathAndName, '/SILENT', '', SW_SHOW, ewWaitUntilTerminated, tmpResult);
          end
          else
          begin
            UseFileDialog := True;
            // File dialog routine called below
          end
        end
        else
        begin
           // GitInstallerExistsLocally was False
           UseFileDialog := True;
        end;
        if UseFileDialog then
        begin
            // User opted to not use the existing Git installer that we found, perhaps because
            // the user wants to use a different (updated) one from a thumb drive, so present 
            // the file dialog and allow navigating to the desired installer.
            Prompt := 'Select a Git installer file, for example Git-2.12.1-32-bit.exe [version 2.12.1 numbers may vary]';
            FileName := 'Git-*.exe';
            InitialDirectory := PathToAIInstallation;
            Filter := 'Git installer (*.exe)|*.exe|All Files|*.*';
            if GetOpenFileName(Prompt, FileName, InitialDirectory, Filter, 'exe') then
            begin
              // Successful; user clicked OK
              // FileName contains the selected filename
              GitInstallerPathAndName := FileName;
              // Install Git with the /SILENT switch. The Git installer has a nice, compatible GUI that
              // shows progress of the installation.
              Exec(GitInstallerPathAndName, '/SILENT', '', SW_SHOW, ewWaitUntilTerminated, tmpResult);
            end
            else
            begin
               // User canceled from the GetOpenFileName dialog
               MsgBox('No Git Installer was selected. Git will not be installed if you continue.' + Chr(13) +
                      'If you still want to install Git, after you click OK, you can click the ' +
                      'Back button to access the Git install options page, and try again.', mbInformation, MB_OK);
            end;
            UseFileDialog := False;        
        end;
      end
      ;
		wpReady : // 
		  ;
		wpPreparing :
		  ;
		wpInstalling :
		  ;
		wpInfoAfter :
		  ;
		wpFinished :
		  ;
	else
		;
	end;
  Result := True;
end;

// The CurStepChanged() procedure can be used to perform your own pre-install and 
// post-install tasks. If it is alled with CurStep=ssInstall just before the actual 
// installation starts, with CurStep=ssPostInstall just after the actual installation 
// finishes, and with CurStep=ssDone just before Setup terminates after a successful 
// install. We use it only as CurStep=ssPostInstall.
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep=ssPostInstall then
    // Silently run the Git installer as a post-install step
    if (ShouldInstallGit = True) then begin
      // Run the git installer silently, with the options loaded from the file
      // ai_git.inf. The git installer for Windows is made in Inno as well;
      // more info on the command line options for Inno installers can be
      // found here: http://www.jrsoftware.org/ishelp/index.php?topic=setupcmdline
      Exec(GitInstallerPathAndName, '/SILENT', '', SW_SHOW, ewWaitUntilTerminated, tmpResult);
    end;
end;

// The NotOnPathAlready() function is used in the [Registry] section to ensure the 
// Windows path to Git is set properly.
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

// The NeedRestart() function Return True to instruct Setup to prompt the user 
// to restart the system at the end of a successful installation, False otherwise.
function NeedRestart(): Boolean;
begin
  // If Git was installed, prompt the user to reboot after everything's done
  Result := ShouldInstallGit;
end;
// end EDB Oct 2013

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, "&", "&&")}}"; Flags: nowait postinstall skipifsilent
