/////////////////////////////////////////////////////////////////////////////
/// \project		adaptit
/// \file			Adapt_It.cpp
/// \author			Bill Martin
/// \date_created	05 January 2004
/// \date_revised	14 May 2008
/// \copyright		2008 Bruce Waters, Bill Martin, SIL International
/// \license		The Common Public License or The GNU Lesser General Public License (see license directory)
/// \description	This is the implementation file for the CAdapt_ItApp class. 
/// The CAdapt_ItApp class initializes Adapt It's application and gets it running. 
/// \derivation		The CAdapt_ItApp class is derived from wxApp, and inherits its support for the document/view framework. 
/////////////////////////////////////////////////////////////////////////////
// Pending Implementation Items in Adapt_It.cpp (in order of importance): (search for "TODO")
// 1. Determine what to do about wxWidgets page setup dialog margins only 
//    taking millimetres, and how to reconcile this with the Units of Measure...
// 2. 
// Unanswered questions: (search for "???")
// 1. 
// 
/////////////////////////////////////////////////////////////////////////////
//#pragma warning( disable : 4189 )

#if defined(__GNUG__) && !defined(__APPLE__) //The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
    #pragma implementation "Adapt_It.h"
#endif


// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
// Include your minimal set of headers here, or wx.h
#include <wx/wx.h>
#endif

// wxWidgets library includes
#include <wx/docview.h>	// includes wxWidgets doc/view framework
#include "Adapt_ItCanvas.h"
#include "Adapt_It_Resources.h"
#include <wx/snglinst.h> // for wxSingleInstanceChecker in OnInit
#include <wx/config.h> // for wxConfig in OnInit
#include <wx/datetime.h> // for wxDateTime in OnInit
#include <wx/filesys.h> // for wxFileName
#include <wx/utils.h> // for ::wxDirExists, ::wxSetWorkingDirectory, etc
#include <wx/textfile.h> // for wxTextFile
#include <wx/encconv.h> // for wxEncodingConverter
#include <wx/fontmap.h> // for wxFontMapper
#include <wx/printdlg.h> // for print data and page setup data
#include <wx/dir.h> // for wxDir
#include <wx/hashmap.h> // for equivalent to MFC's CMapStringToOb* pMap in DoKBIntegrityCheck()
#include <wx/datstrm.h> // for wxDataOutputStream() and wxDataInputStream()
#include <wx/wfstream.h> // for wxFileOutputStream() and wxFileInputStream()
#include <wx/txtstrm.h> // for wxTextOutputStream()
#include <wx/cmdline.h> // for wxCmdLineParser
#include <wx/wizard.h> // for wxWizard
#include <wx/fdrepdlg.h> // for wxFindReplaceDialog
#include <wx/fontenum.h> // for wxFontEnumerator
#include <wx/progdlg.h> // for wxProgressDialog
#include <wx/busyinfo.h>
#include <wx/propdlg.h> // for wxPropertySheetDialog
#include <wx/stdpaths.h> // for GetResourcesDir and GetLocalizedResourcesDir

// the next are for wxHtmlHelpController
#include <wx/filesys.h>
#include <wx/fs_arc.h> //#include <wx/fs_zip.h>
//#include <wx/help.h> //(wxWidgets chooses the appropriate help controller class)
//#include <wx/helpbase.h> //(wxHelpControllerBase class)
//#include <wx/helpwin.h> //(Windows Help controller)
//#include <wx/msw/helpchm.h> //(MS HTML Help controller)
//#include <wx/generic/helpext.h> //(external HTML browser controller)
//#include <wx/html/helpfrm.h>
#include <wx/html/helpctrl.h> //(wxHTML based help controller: wxHtmlHelpController)
#include <wx/cshelp.h> // for wxHelpControllerHelpProvider

#include <wx/display.h> // for wxDisplay

// Other includes
#include "Adapt_It.h"
#include "MainFrm.h"
#include "Text.h"
#include "Adapt_ItDoc.h"
#include "Adapt_ItView.h"
#include "AdaptitConstants.h"
#include "KB.h"
#include "helpers.h"
#include "FontPage.h"
#include "PhraseBox.h"
#include "LanguagesPage.h"
#include "USFMPage.h"
#include "FilterPage.h"
#include "PunctCorrespPage.h"
#include "CaseEquivPage.h"
#include "OpenExistingProjectDlg.h"
#include "ProjectPage.h"
#include "DocPage.h"
#include "StartWorkingWizard.h"
#include "TargetUnit.h"
#include "RefString.h"
#include "CCTabbedDialog.h"
#include "WhichFilesDlg.h" // renamed from original "RestoreKBDlg.h"
#include "SourcePhrase.h"
#include "ProgressDlg.h" // formerly called RestoreKBProgress.h
#include "Pile.h"
#include "TransformToGlossesDlg.h"
#include "EarlierTranslationDlg.h"
#include "Cell.h"
#include "WhichBook.h"
#include "SourceBundle.h"
#include "BString.h"
#include "XML.h"
#include "NoteDlg.h"
#include "ViewFilteredMaterialDlg.h"
#include "WaitDlg.h"
#include "AIPrintout.h"
#include "ConsistentChanger.h"
#include "ChooseLanguageDlg.h"

#if !wxUSE_WXHTML_HELP
    #error "This program can't be built without wxUSE_WXHTML_HELP set to 1"
#endif // wxUSE_WXHTML_HELP

// The following include by David A. Jones is described in an article on The Code Project
// called "Memory Leak Detection". It generates better memory leak detection reporting.
// Note: This header and how it works are found on: http://www.codeproject.com/cpp/MemLeakDetect.asp
// Its code is not compiled into the program in release versions, but vld.h need only be included
// when memory leaks are detected by the debugger's Output report, and it is not obvious what is
// the cause of the leak from the report.
// Steps I did to make this work with VC 7.1 (and again for VC 8.0):
// 1. copied vld.dll to c:\WINDOWS\System32
// 2. copied vld.ini to c:\WINDOWS
// 3. copied vld.lib to Visual C++ installation's Lib subdirectory
// 4. copied vld.h to Visual C++ installation's Include subdirectory
// 5. copied dbghelp.dll to the directory where the executable you are debugging resides (...\Debug)
// 6. downloaded a copy of msvcrtd.dll from the web and also put it into ...\Debug
// 7. the following #include to include vld.h
// Note: The VLD code is beta so we have to deal with a couple problems, but it still works.
// The Microsoft Development Environment may issue a couple debug messages that read:
// "Unhandled exception at 0x7c901230 (NTDLL.DLL) in Adapt_It.exe: User breakpoint."
// Just click "Continue" to pass through these exceptions, then look at the reports in the
// debug output and click on the first line of the stack trace which should be the line of
// code were the variable was allocated on the heap that was never deleted.
// If "memory leaks detected" and source of leak is unclear, uncomment the following include
//#include "vld.h"

/// This global is defined in TransferMarkersDlg.cpp.
extern bool gbPropagationNeeded;	

/// This global is defined in TransferMarkersDlg.cpp.
extern TextType gPropagationType;

/// BEW added 23May05, for support of returning to the previous context's TextType
/// value after an inline section with TextType none, or after a footnote (since the
/// latter can be embedded inline within verse, poetry, headings, subtitles, section
/// headings, etc).
TextType gPreviousTextType; // moved here to global space in the Doc

/// A compatibility struct for the source language font, which is only used to hold the
/// Logfont info used by MFC and which is stored within Adapt It's config files. Many of
/// the values stored in the fontInfo structs are not used by the wxWidgets version, but
/// are read and written back out to the config files to maintain backward compatibility.
fontInfo SrcFInfo;

/// A compatibility struct for the target language font, which is only used to hold the
/// Logfont info used by MFC and which is stored within Adapt It's config files. Many of
/// the values stored in the fontInfo structs are not used by the wxWidgets version, but
/// are read and written back out to the config files to maintain backward compatibility.
fontInfo TgtFInfo;

/// A compatibility struct for the navigation language font, which is only used to hold the
/// Logfont info used by MFC and which is stored within Adapt It's config files. Many of
/// the values stored in the fontInfo structs are not used by the wxWidgets version, but
/// are read and written back out to the config files to maintain backward compatibility.
fontInfo NavFInfo;

/// A global instance of PageOffsets. Used for saving top and bottom logical coord offsets 
/// for printing pages, stored in m_pagesList. The pgOffsets are populated in the 
/// PaginateDoc() function in the CAdapt_ItView and AIPrintout classes.
PageOffsets pgOffsets;

// wxList definitions
#include <wx/listimpl.cpp>

/// This macro together with the macro list declaration in the .h file
/// complete the definition of a new safe pointer list class called POList.
WX_DEFINE_LIST(POList);

/// This macro together with the macro list declaration in the .h file
/// complete the definition of a new safe pointer list class called CCellList.
WX_DEFINE_LIST(CCellList);

/// This macro together with the macro list declaration in the .h file
/// complete the definition of a new safe pointer list class called KPlusCList.
WX_DEFINE_LIST(KPlusCList);

/// Length of the byte-order-mark (BOM) which consists of the three bytes 0xEF, 0xBB and 0xBF
/// in UTF-8 encoding.
#define nBOMLen 3

/// Length of the byte-order-mark (BOM) which consists of the two bytes 0xFF and 0xFE in
/// in UTF-16 encoding.
#define nU16BOMLen 2

// globals

/// This global is defined in Adapt_ItView.cpp.
extern bool gbVerticalEditInProgress; // defined in Adapt_ItView.cpp (for vertical edit functionality)

/// This global is defined in Adapt_ItView.cpp.
extern bool gbAdaptBeforeGloss; // defined in Adapt_ItView.cpp (for vertical edit functionality)

/// BEW added 01Oct06, so that calling WriteProjectSettingsConfiguration()
/// which is called at the end, now, of OnNewDocument() and OnOpenDocument(), does not get called in
/// OnNewDocument() when gbPassedMFCinitialization is FALSE, since the latter function call is called by
/// MFC within ProcessShellCommand() which is called from within InitInstance() - and since the m_bBookMode
/// defaults to FALSE, and m_nBookIndex defaults to -1 at every launch, this MFC call of OnNewDocument()
/// would otherwise unlaterally turn off book mode which was on when the app last exitted. We can't have
/// this happen, so we use this new flag to suppress the config file write for the project settings until
/// we are actually in the wizard (and hence passed all the MFC initializations).
bool gbPassedMFCinitialization = FALSE; 

/// When TRUE gbUpdateDocTitleNeeded causes OnIdle() to update the window title if the open attempt failed,
/// otherwise the failed doc name remains in the window's title bar. Note, since OnOpenRecentFile always 
/// returns TRUE, we have to unilaterally cause OnIdle to do the update of the title to ensure we catch 
/// the failures.
bool gbUpdateDocTitleNeeded = FALSE; // is set only by a failed MRU document open

/// This global is defined in DocPage.cpp.
extern bool gbReachedDocPage; // BEW added 10Nov05  (see DocPage.cpp especially its OnInitDialog())

/// This global is defined in PhraseBox.cpp.
extern bool gbCameToEnd; // see PhraseBox.cpp

/// This global is defined in Adapt_ItView.cpp.
extern int gnBeginInsertionsSequNum;

/// This global is defined in Adapt_ItView.cpp.
extern int gnEndInsertionsSequNum;

/// This global boolean is used to inform functions involved in opening documents whether the file is
/// being opened via the OnMRUFile() function, i.e., due to the user attempting to open the file from
/// the MRU list. If the file no longer exists and is an XML one, we use the bool TRUE value to give the
/// user a nice informative message and avoid ugly error messages, and allow him to try again. TRUE if
/// while the OnMRUFile() function is active due to the user attempting to open a file from the MRU list
/// (if the file no longer exists and is an XML one, we use the bool TRUE value to give the user a nice
/// informative message and avoid ugly error messages, and allow him to try again.
bool gbTryingMRUOpen = FALSE;	

/// This global is defined in SplitDialog.cpp.
extern bool gbIsDocumentSplittingDialogActive; // see SplitDialog.cpp

/// This global is defined in PhraseBox.cpp.
extern bool gbSuppressStoreForAltBackspaceKeypress;

/// This global is defined in MainFrm.cpp.
extern bool gbIgnoreScriptureReference_Receive;

/// This global is defined in MainFrm.cpp.
extern bool gbIgnoreScriptureReference_Send;

/// This global is defined in MainFrm.cpp.
extern SPList* gpDocList;

// The following declarations moved here from the View's global space

///BEW added 11Oct05 to support hiliting the cell under the clicked green wedge, or note icon.
CPile* gpGreenWedgePile;

/// BEW added 11Oct05, to allow clicked wedge's topmost cell in the layout
/// to be given background highlighting, so user has visual feedback as to
/// which pile the View Filtered Material dialog pertains to.
CPile* gpNotePile;

/// BEW added 17Sep05, to support adjusting the dialog position according to where the click on the wedge or
/// note icon was (because the phrase box might be a long way away - even off screen).
/// The point is in screen coords, as set at start of OnLButtonDown().
wxPoint gptLastClick;

// BEW added 24Jun05; globals for free translation support

/// BEW added 27Jun05 for free trans support, make sure it is always FALSE when OnLButtonDown() is
/// entered. Default FALSE, TRUE if pile pointer is NULL on GetPrevPile() call.
bool gbBundleStartIteratingBack;

/// The offset to the current free translation string in pSrcPhrase->m_markers.
int gnOffsetInMarkersStr; 

/// The free translation length, including final space if any, in pSrcPhrase->m_markers.
int gnLengthInMarkersStr; 

/// An array of pointers to CPile instances. It is created on the heap in OnInit(), 
/// and disposed of in OnExit().
wxArrayPtrVoid*	gpCurFreeTransSectionPileArray;

/// An array of pointers used as a scratch array, for composing the free translations which are written to
/// screen. Element pointers point to FreeTrElement structs - each of which contains the information 
/// relevant to writing a subpart of the free translation in a single rectangle under a single strip.
wxArrayPtrVoid*	gpFreeTransArray; // new creates on heap in InitInstance, and disposes in ExitInstance

/// Pointer to first pile in a free translation section. gpLastPile points to the last pile in the same
/// free translation section.
CPile* gpFirstPile;

/// Pointer to last pile in the free translation section. gpFirstPile points to the first pile in the
/// same free translation section.
CPile* gpLastPile;

/// The contents of App's m_punctuation[1] string with spaces removed.
wxString gSpacelessTgtPunctuation;

/// Suppress the calling of SetupCurrentFreeTransSection() when the Shorten or Lengthen button
/// has been pressed (otherwise it wipes out the action of the button), and also when doing bundle 
/// advances (which involve a call to RecalcLayout() and therefore
/// would call SetupCurrentFreeTransSection() each time), when Advance() button was
/// pressed and the next empty location is long way ahead past a big section of
/// already free translated material which lies just ahead.
bool gbSuppressSetup;

/// whm added 13Apr07, a flag to insure that the warning about hacked character data being detected is only 
/// issued once - upon first detection of such data in config files and/or data files.
bool gbHackedDataCharWarningGiven = FALSE;

/// BEW added 29Apr06, so that Remove Free Translation button in ViewFilteredMaterial dialog can be used
/// to delete the free translation and its markers, and then a subsequent <Prev, Next> or Advance clicked without
/// that click inserting an empty \free \free* filtered marker pair in the location just left (which had its free
/// translation just removed) - this, of course, only is relevant while Free Trans Mode is turned on.
bool gbFreeTranslationJustRemovedInVFMdialog = FALSE;

/// Save m_bSuppressTargetHighlighting flag's value while the 'Accept Changes Without Stopping' toggle
/// is ON. When the latter is ON, the app's m_bSuppressTargetHighlighting value must be TRUE, otherwise
/// MFC's lazy window updates get out of synch with updates of the pointers on which the layout relies
/// and the app crashes. This does not happen if no highlighting takes place. The original setting for
/// highlighting is restored when the toggle command is turned back off.
bool gbSaveHilightingSetting = FALSE;

// The above declarations moved here from the View's global space

/// For support of book folders mode and the option of binary or xml intput/output when doing the
/// Transform Adaptations to Glosses operation - which needs to work whether book mode is currently on
/// or not and whether book folders are present or not, (and for any possible mix of xml or binary kb
/// files and document files; in fact, the same considerations about file type and book folders mode are
/// relevant to any operation involving the enumerated set of documents - so also relevant to restoring
/// the knowledge base from the existing documents and reporting retranslations). TRUE if the current
/// project's Adaptations folder contains Bible book folders (whether or not book mode is currently on)
/// (for XML or binary, a local Boolean will suffice).
bool gbHasBookFolders = FALSE; // 

// for support of auto-capatalization

/// whm added 12Aug04 Initial flag for auto-cap support possible. This flag keeps track of initial
/// checkbox on "Define Any Lower To Upper Case Equivalences" Wizard panel/Dialog that reads:
/// [ ]Check here if the source text contains both capital letters (upper case) and small letters (lower
/// case). The remaining controls on the panel do not appear unless this checkbox is checked. This value
/// is now stored in the AI-ProjectConfiguration.aic file.
bool gbSrcHasUcAndLc = FALSE;

/// Switch for turning auto-capitalization support on or off.
bool gbAutoCaps = FALSE;

/// Refers to first character of the source word or phrase; TRUE if the source is upper case, otherwise FALSE.
bool gbSourceIsUpperCase = FALSE;

/// Refers to first character of the contents of the phrase box, which may contain a gloss or an
/// adaptation depending on the value of gbIsGlossing.
bool gbNonSourceIsUpperCase = FALSE; 

/// if the source has an initial UC character and the first KB lookup fails, then a KB lookup is done
/// with UC first character, and if found this flag is set TRUE.
bool gbMatchedKB_UCentry = FALSE; 

/// Set TRUE if source language is caseless, or user does not define any case equivalences.
bool gbNoSourceCaseEquivalents = FALSE; 

/// Set TRUE if user does not define any case equivalences for the target language.
bool gbNoTargetCaseEquivalents = FALSE; 

/// Set TRUE if user does not define any case equivalences for the gloss language.
bool gbNoGlossCaseEquivalents = FALSE;

/// Store first character of adaption or gloss when it is lower case.
wxChar gcharNonSrcLC;

/// Store first character of adaption or gloss when it is upper case.
wxChar gcharNonSrcUC;

/// Store first lwr case char of source text word or phrase (after punct stripped).
wxChar gcharSrcLC;

/// Store first upr case char of source text word or phrase (after punct stripped).
wxChar	gcharSrcUC;

/// Use to suppress the message box asking if the src language has upper/lower case distinction, when
/// the flag is being restored to TRUE from reading the project's config file.
bool gbSuppressAutoCapsAsk = FALSE; 

/// This global is defined in OpenExistingProjectDlg.cpp.
extern bool gbExcludeCurrentProject;

/// This global is defined in Adapt_ItView.cpp.
extern bool gbGlossingUsesNavFont;

/// This global is defined in Adapt_ItView.cpp.
extern bool gbRemovePunctuationFromGlosses;

/// This global is defined in Adapt_ItView.cpp.
extern int gnSelectionLine;

/// This global is defined in Adapt_ItView.cpp.
extern int gnSelectionStartSequNum;

/// This global is defined in Adapt_ItView.cpp.
extern int gnSelectionEndSequNum;

/// This global is defined in Adapt_ItView.cpp.
extern bool gbPrintFooter;

// next two are for version 2.0 which includes the option of a 3rd line for glossing

/// This global is defined in Adapt_ItView.cpp.
extern bool	gbIsGlossing; // when TRUE, the phrase box and its line have glossing text

/// This global is defined in Adapt_ItView.cpp.
extern bool	gbEnableGlossing; // TRUE makes Adapt It revert to Shoebox functionality only

/// When FALSE back slash characters found anywhere in a text are interpreted as introducing 
/// standard format markers (the default). When TRUE back slash characters may be interpreted
/// as word-building characters. When the gbSfmOnlyAfterNewlines flag is TRUE, any standard 
/// format marker escape characters which do not follow a newline are not assumed to belong
/// to a sfm, and so we treat them in such cases as ordinary word-building characters
/// (on the assumption we are dealing with a hacked legacy encoding in which the
/// escape character is an alphabetic glyph in the font).
bool gbSfmOnlyAfterNewlines = FALSE;

/// Defined as the backslash character.
/// NOTE: Older versions of Adapt It allowed the user to designate the character to be used, 
/// but in all recent versions, gSFescapechar is always defined as the backslash character.
/// For cross-platform considerations, the gSFescapechar is handled totally apart from the 
/// wxWidgets PathSeparator.
wxChar gSFescapechar = _T('\\');	// the standard format escape char, default is backslash
									
/// TRUE while there is no m_targetBox created, during setup of the view. It is used to
/// prevent premature merges.
bool gbDoingInitialSetup = FALSE;

/// A global pointer to the application instance.
CAdapt_ItApp* gpApp;

/// ANSI version is always left to right (LTR) reading; this flag can only
/// be changed in the NRoman version, using the extra Layout menu. When TRUE text is right to left
/// (RTL) layout.
bool gbRTL_Layout = FALSE;

/// For ANSI version, no bloating of the size of the phrase box is
/// needed, but for an unknown reason, the CRichEditCtrl's vertical
/// size is smaller than required for showing all the text; adding in
/// external and internal leadings helps (at expense of making the
/// line height too large, but is not enough because the style is 
/// diff. So I'll the sum of the ext & int leadings to the box 
/// height, and +4 to allow for the thicker boundary at top and 
/// bottom.
int gnVerticalBoxBloat = 0; 

/// TRUE if <New Project> selected in the Wizard, otherwise FALSE.
bool gbWizardNewProject = FALSE;

/// Global pointer to the help controller instance.
wxHtmlHelpController* m_pHelpController = (wxHtmlHelpController*)NULL;

/// Global pointer to the help controller instance.
//wxHelpController* m_pHelpController = (wxHelpController*)NULL;

/// This global is defined in Adapt_ItView.cpp.
extern short gnExpandBox;  // see start of Adapt_ItView.cpp for explanation of these two

/// This global is defined in Adapt_ItView.cpp.
extern short gnNearEndFactor; 

/// This global is defined in PhraseBox.cpp.
extern long gnStart; // int in MFC

/// This global is defined in PhraseBox.cpp.
extern long gnEnd; // int in MFC

/// This global is defined in Adapt_ItView.cpp.
extern	bool gbLegacySourceTextCopy; // defined in Adapt_ItView.cpp, to govern default phrase box text

/// Pointer to the Start Working Wizard instance.
CStartWorkingWizard* pStartWorkingWizard = (CStartWorkingWizard*)NULL; // pointer to the sheet

//enum WizSheetPage nLastWizPage = project_page;

/// Pointer to the project page instance
CProjectPage* pProjectPage = (CProjectPage*)NULL;

/// Pointer to the languages page instance
CLanguagesPage* pLanguagesPage = (CLanguagesPage*)NULL;

/// Pointer to the Wizard's font page instance
CFontPageWiz* pFontPageWiz = (CFontPageWiz*)NULL;

/// Pointer to the Wizard's punct correspondence page instance
CPunctCorrespPageWiz* pPunctCorrespPageWiz = (CPunctCorrespPageWiz*)NULL;

/// Pointer to the Wizard's case equivalences page instance
CCaseEquivPageWiz* pCaseEquivPageWiz = (CCaseEquivPageWiz*)NULL;

/// Pointer to the Wizard's usfm page instance
CUSFMPageWiz* pUsfmPageWiz = (CUSFMPageWiz*)NULL;

/// Pointer to the Wizard's filter page instance
CFilterPageWiz*	pFilterPageWiz = (CFilterPageWiz*)NULL;

/// Pointer to the doc page instance
CDocPage* pDocPage = (CDocPage*)NULL;

/// Pointer to the usfm page instance
CUSFMPagePrefs*	pUSFMPageInPrefs = (CUSFMPagePrefs*)NULL;		// To be able to access the USFM page from other Preferences page(s)

/// Pointer to the filter page instance
CFilterPagePrefs* pFilterPageInPrefs = (CFilterPagePrefs*)NULL;	// To be able to access the Filtering page from other Preferences page(s)

/// Contains the sequence number for the last auto save operation; -1 if auto save is turned off.
int nSequNumForLastAutoSave;

/// Indicates whether a document is being opened via the MRU list. Its value is set to TRUE in the main
/// frame's OnMRUFile() function.
bool gbViaMostRecentFileList = FALSE;

/// Indicates the count of entries in each of the 10 MapKeyStringToTgtUnit maps in the KB. Used for
/// support of preallocation of storage prior to opening KB editor with list controls of many entries.
int gnMapLength[10];

/// This global is defined in PhraseBox.cpp.
extern int nCurrentSequNum;

// This global is defined in FindReplace.cpp.
//extern bool gbReplaceAllIsCurrent;

/// A global that indicates whether it is possible to open a file from the MRU list. It is set TRUE if
/// m_bDisableBookMode is TRUE when SetupDirectories() is called (the post MRU file click is the time
/// when this is significant, because there is not enough info to recreate the folder path if books.xml
/// failed on input or parse).
bool gbAbortMRUOpen = FALSE; 

// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
// NT and OT book name string constants, for GUI - especially setting up a folder for
// every Bible book, and the 67th will be "Other Texts" for all non-scripture work.

/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _00BK = _("Genesis");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _01BK = _("Exodus");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _02BK = _("Leviticus");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _03BK = _("Numbers");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _04BK = _("Deuteronomy");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _05BK = _("Joshua");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _06BK = _("Judges");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _07BK = _("Ruth");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _08BK = _("1 Samuel");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _09BK = _("2 Samuel");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _10BK = _("1 Kings");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _11BK = _("2 Kings");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _12BK = _("1 Chronicles");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _13BK = _("2 Chronicles");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _14BK = _("Ezra");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _15BK = _("Nehemiah");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _16BK = _("Esther");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _17BK = _("Job");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _18BK = _("Psalms");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _19BK = _("Proverbs");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _20BK = _("Ecclesiastes");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _21BK = _("Song of Songs");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _22BK = _("Isaiah");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _23BK = _("Jeremiah");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _24BK = _("Lamentations");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _25BK = _("Ezekiel");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _26BK = _("Daniel");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _27BK = _("Hosea");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _28BK = _("Joel");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _29BK = _("Amos");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _30BK = _("Obadiah");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _31BK = _("Jonah");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _32BK = _("Micah");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _33BK = _("Nahum");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _34BK = _("Habakkuk");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _35BK = _("Zephaniah");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _36BK = _("Haggai");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _37BK = _("Zechariah");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _38BK = _("Malachi");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _39BK = _("Matthew");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _40BK = _("Mark");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _41BK = _("Luke");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _42BK = _("John");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _43BK = _("Acts");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _44BK = _("Romans");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _45BK = _("1 Corinthians");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _46BK = _("2 Corinthians");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _47BK = _("Galatians");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _48BK = _("Ephesians");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _49BK = _("Philippians");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _50BK = _("Colossians");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _51BK = _("1 Thessalonians");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _52BK = _("2 Thessalonians");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _53BK = _("1 Timothy");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _54BK = _("2 Timothy");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _55BK = _("Titus");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _56BK = _("Philemon");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _57BK = _("Hebrews");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _58BK = _("James");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _59BK = _("1 Peter");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _60BK = _("2 Peter");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _61BK = _("1 John");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _62BK = _("2 John");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _63BK = _("3 John");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _64BK = _("Jude");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _65BK = _("Revelation");
/// Bible books folders support - folder name defaults for when there is no "books.XML" file in work folder.
const wxChar* _66BK = _("Other Texts");

// now the book codes

/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _00BKCODE = _("GEN");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _01BKCODE = _("EXO");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _02BKCODE = _("LEV");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _03BKCODE = _("NUM");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _04BKCODE = _("DEU");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _05BKCODE = _("JOS");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _06BKCODE = _("JDG");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _07BKCODE = _("RUT");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _08BKCODE = _("1SA");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _09BKCODE = _("2SA");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _10BKCODE = _("1KI");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _11BKCODE = _("2KI");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _12BKCODE = _("1CH");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _13BKCODE = _("2CH");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _14BKCODE = _("EZR");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _15BKCODE = _("NEH");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _16BKCODE = _("EST");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _17BKCODE = _("JOB");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _18BKCODE = _("PSA");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _19BKCODE = _("PRO");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _20BKCODE = _("ECC");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _21BKCODE = _("SNG");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _22BKCODE = _("ISA");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _23BKCODE = _("JER");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _24BKCODE = _("LAM");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _25BKCODE = _("EZK");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _26BKCODE = _("DAN");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _27BKCODE = _("HOS");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _28BKCODE = _("JOL");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _29BKCODE = _("AMO");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _30BKCODE = _("OBA");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _31BKCODE = _("JON");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _32BKCODE = _("MIC");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _33BKCODE = _("NAH");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _34BKCODE = _("HAB");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _35BKCODE = _("ZEP");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _36BKCODE = _("HAG");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _37BKCODE = _("ZEC");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _38BKCODE = _("MAL");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _39BKCODE = _("MAT");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _40BKCODE = _("MRK");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _41BKCODE = _("LUK");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _42BKCODE = _("JHN");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _43BKCODE = _("ACT");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _44BKCODE = _("ROM");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _45BKCODE = _("1CO");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _46BKCODE = _("2CO");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _47BKCODE = _("GAL");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _48BKCODE = _("EPH");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _49BKCODE = _("PHP");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _50BKCODE = _("COL");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _51BKCODE = _("1TH");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _52BKCODE = _("2TH");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _53BKCODE = _("1TI");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _54BKCODE = _("2TI");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _55BKCODE = _("TIT");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _56BKCODE = _("PHM");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _57BKCODE = _("HEB");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _58BKCODE = _("JAS");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _59BKCODE = _("1PE");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _60BKCODE = _("2PE");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _61BKCODE = _("1JN");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _62BKCODE = _("2JN");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _63BKCODE = _("3JN");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _64BKCODE = _("JUD");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _65BKCODE = _("REV");
/// Bible books folders support - 3-letter book code defaults for when there is no "books.XML" file in work folder.
const wxChar* _66BKCODE = _("OTX");

wxString dummyBack = _("&Finish"); // the wxstd.mo file doesn't seem to have this wizard button for localizing, so I add it here
wxString dummyFinish = _("< &Back"); // the wxstd.mo file doesn't seem to have this wizard button for localizing, so I add it here

// support for USFM and SFM Filtering

/// The actual count of default Unit-style strings contained in the defaultSFM string array. The field values
/// are delimited by ':' characters. This number can be obtained automatically by uncommenting the 
/// #define Output_Default_Style_Strings line at the beginning of XML.h file (see instructions in
/// documentation for the defaultSFM[] string array). The number is automatically produced as the last line of 
/// string output written to AI_USFM_full.txt file when Output_Default_Style_Strings is defined in XML.h.
const int gnDefaultSFMs = 282;	// get this number from last line of string output written to AI_USFM_full.txt 
								// (see note below). It is the count the actual number of strings contained
								// in the defaultSFM string array below!

/// An array of wxStrings which, when parsed by ParseAndFillStruct(), is used as default standard
/// format marker style definitions if, for some reason, the program cannot find the AI_USFM.xml
/// control file. 
/// The default data strings can be produced automatically by making the following temporary
/// changes in the code:
/// 1. Temporarily uncomment the #define Output_Default_Style_Strings line at the beginning of XML.h file. 
/// 2. Copy the current AI_USFM_full.xml file from the project's xml folder to the "Adapt It Work"
/// folder (this file which contains the full style information and from which the AI_USFM.xml file for
/// distribution to users is produced via the UsfmXml.cct and UsfmXmlTidy.cct changes operations).
/// While the Output_Default_Strings define is active, running the Adapt It program up through the
/// appearance of the Start Working Wizard causes the routines in XML.cpp to read the full
/// AI_USFM_full.xml file (instead of AI_USFM.xml) and produce the temporary AI_USFM_full.txt file
/// which simply contains the Unix-format default strings ready to paste into the code block below.
/// 3. Copy the strings from the AI_USFM_full.txt file and paste them here for recompilation. Note 
/// that the last line of the file contains the number of usfm strings and should be manually pasted 
/// to the value of const gnDefaultSFMs above. Also, Remember to remove the last comma from the 
/// last string item once it is pasted into this array initialization list.
/// 4. Comment out the #define Output_Default_Style_Strings line at the beginning of XML.h file so that
/// the application won't continue to produce the temporary AI_USFM_full.txt file in the work folder,
/// but will instead revert back to using the normal (abbreviated) AI_USFM.xml file on program startup.
const wxString defaultSFM[] = 
{
_T("id::File identification (BOOKID, FILENAME, EDITOR, MODIFICATION DATE):1:1::::1::1:id:11::id - File - Identification:0:1:65535::::::3::::::__normal:id::1:"),
_T("ide::File encoding information:1::1::::1:::0::ide - File - Encoding:0:1:65535::::::3::::::__normal:ide::1:"),
_T("h::Running header text for a book (basic):1:1:1:1::1:1:1:hdr:10::h - File - Header:0:12:::::::3::::::_vernacular_base:h:::"),
_T("h1::Running header text:1::1:1::1:1:1:hdr:10::h1 - File - Header:0:12:::::::1::::::h:h1:::"),
_T("h2::Running header text, left side of page:1::1:1::1:1:1:hdr-left:10::h2 - File - Left Header:0:12:::::::0::::::h1:h2:::"),
_T("h3::Running header text, right side of page:1::1:1::1:1:1:hdr-rght:10::h3 - File - Right Header:0:12:::::::2::::::h1:h3:::"),
_T("rem::Comments and remarks:1::1:1::1:1:1:comment:34::rem - File - Remark:0:9:16711680::::::3::::::_notes_base:rem:::"),
_T("restore::Project restore information:1::1::::1:::34::restore - File - Restore Information:0:12:16711680::::::3::::::__normal:restore:::"),
_T("lit::For a comment or note inserted for liturgical use:1::1:1::1:1:1:lit-note:34::lit - Special Text - Liturgical note:0:12:::1::::2::::::p:lit:1:1:"),
_T("nt::Note::1:1:1::1:1:1:note:34::nt - Note:0:9:16711680::::::3::::::_notes_base:nt:::"),
_T("nc::Note centered::1:1:1::1:1:1:note:34::nc - Note centered:0:9:16711680::::::1::::::nt:nc:::"),
_T("c::Chapter number (basic):1:1:::::1:1::1:1:c - Chapter Number:0:18:::1::::1:8:4::::_heading_base:p:1:1:"),
_T("ca:ca*:Second (alternate) chapter number (for coding dual versification; useful for places where different traditions of chapter breaks need to be supported in the same translation):1::1:1:::1:::1::ca...ca* - Chapter Number - Alternate:1:16:2263842::1::::3::::::::::"),
_T("cl::Chapter label used for translations that add a word such as 'Chapter' before chapter numbers (e.g. Psalms). The subsequent text is the chapter label.:1::1:1:::1:::1::cl - Chapter - Publishing Label:0:18:::1::::1:8:4::::_heading_base:p:1:1:"),
_T("cp::Published chapter number (this is a chapter marking that would be used in the published text):1::1:1:::1:::1::cp - Chapter Number - Publishing Alternate:0:18:16711680::1::::1:8:4::::_heading_base:p:1:1:"),
_T("cd::Chapter Description (Publishing option D, e.g. in Russian Bibles):1:::1:::1:1:chapter descr:1::cd - Chapter - Description:0:11:::::::3:8:4::::_heading_base:p:1:1:"),
_T("v::A verse number (basic):1:1:::::1:1::1::v - Verse Number:1:10:16711935::1:::1:3::::::::::"),
_T("vt::Verse text:1:1:::::1:1::1::vt - Verse text vt:1:12::::::1:3::::::::::"),
_T("vn::Verse number:1:1:::::1:1::1::vn - Verse number vn:1:10:16711935::1:::1:3::::::::::"),
_T("va:va*:Second (alternate) verse number (for coding dual numeration in Psalms; see also NRSV Exo 22.1-4):1::1:1:1:::::1::va...va* - Verse Number - Alternate:1:10:2263842::1:::1:3::::::::::"),
_T("vp:vp*:Published verse marker - this is a verse marking that would be used in the published text:1::1:1:1:::::1::vp...vp* - Verse Number - Publishing Alternate:1:10:16711680::1:::1:3::::::::::"),
_T("mt::The main title of the book (if single level):1:1::1::1:1:1:main title:4:1:mt - Title - Major Title Level 1:0:20:::1::::1:8:4::::_heading_base:c:1:1:"),
_T("mt1::The main title of the book (if multiple levels) (basic):1:::1::1:1:1:main title L1:4:1:mt1 - Title - Major Title Level 1:0:20:::1::::1:2:4::::_heading_base:c:1:1:"),
_T("mt2::A secondary title usually occurring before the main title (basic):1:::1::1:1:1:secondary title L2:5:1:mt2 - Title - Major Title Level 2:0:16::1:::::1::2::::_heading_base:mt:1:1:"),
_T("mt3::A secondary title occurring after the main title:1:::1::1:1:1:secondary title L3:5:1:mt3 - Title - Major Title Level 3:0:14:::1::::1:2:2::::_heading_base:c:1:1:"),
_T("mt4::A small secondary title sometimes occuring within parentheses:1:::1::1:1:1:secondary title L4:5:1:mt4 - Title - Major Title level 4:0:12:::::::1:2:2::::_heading_base:c:1:1:"),
_T("st::Secondary title::1::1::1:1:1:secondary title:5:1:st - Secondary title:0:16:::1::::1::2::::_heading_base:mt:1:1:"),
_T("mte::The main title of the book repeated at the end of the book (if single level):1:::1::1:1:1:main title at end:4:1:mte - Title - [Uncommon] Major Title Ending Level 1:0:20:::1::::1:8:4::::_heading_base::1::"),
_T("mte1::The main title of the book repeated at the end of the book (if multiple levels):1:::1::1:1:1:main title at end L1:4:1:mte1 - Title - [Uncommon] Major Title Ending Level 1:0:20:::1::::1:8:4::::_heading_base::1::"),
_T("mte2::A secondary title occurring before or after the 'ending' main title:1:::1::1:1:1:secondary title at end L2:5:1:mte2 - Title - [Uncommon] Major Title Ending Level 2:0:16::1:::::1::2::::_heading_base::1::"),
_T("div::Division heading::1::1::1:1:1:division head:3:1:div - Division heading:0:16:::1::::1:6:3::::s:dvrf:1:1:"),
_T("bn::Psalms book number::1::1::1:1:1:Psalm book number:0:1:bn - Psalms book number:0:11:::1::::1:6:3::::s:br:1:1:"),
_T("ms::A major section division heading, level 1 (if single level) (basic):1:::1::1:1:1:major sect head:3:1:ms - Heading - Major Section Level 1:0:14:::1::::1:16:4::::_heading_base:mr:1:1:"),
_T("ms1::A major section division heading, level 1 (if multiple levels):1:::1::1:1:1:major sect head L1:3:1:ms1 - Heading - Major Section Level 1:0:14:::1::::1:16:4::::_heading_base:mr:1:1:"),
_T("ms2::A major section division heading, level 2:1:::1::1:1:1:major sect head L2:3:1:ms2 - Heading - Major Section Level 2:0:14:::1::::1:16:4::::_heading_base:mr:1:1:"),
_T("ms3::A major section division heading, level 3:1:::1::1:1:1:major sect head L3:3:1:ms3 - Heading - Major Section Level 3:0:14::1:::::1:16:4::::_heading_base:mr:1:1:"),
_T("s::A section heading, level 1 (if single level) (basic):1:1::1::1:1:1:sect head:3:1:s - Heading - Section Level 1:0:12:::1::::1:8:4::::_heading_base:p:1:1:"),
_T("s1::A section heading, level 1 (if multiple levels):1:::1::1:1:1:sect head L1:3:1:s1 - Heading - Section Level 1:0:12:::1::::1:8:4::::_heading_base:p:1:1:"),
_T("s2::A section heading, level 2 (e.g. Proverbs 22-24):1:::1::1:1:1:sect head L2:3:1:s2 - Heading - Section Level 2:0:12::1:::::1:8:4::::_heading_base:p:1:1:"),
_T("s3::A section heading, level 3 (e.g. Genesis 'The First Day'):1:::1::1:1:1:sect head L3:3:1:s3 - Heading - Section Level 3:0:12::1:::::0:6:3::::_heading_base:p:1:1:"),
_T("s4::A section heading, level 4:1:::1::1:1:1:sect head L4:3:1:s4 - Heading - Section Level 4:0:12::1:::::0:6:3::::_heading_base:p:1:1:"),
_T("sr::A section division references range heading:1:::1::1:1:1:sect head range refs:3:1:sr - Heading - Section Range References:0:12:::1::::1::4::::_heading_base:p:1:1:"),
_T("sx::Extra heading 1::1::1::1:1:1:sect head extra 1:3:1:sx - Extra heading 1:0:12:::1::::1:6:3::::_heading_base:p:1:1:"),
_T("sz::Extra heading 2::1::1::1:1:1:sect head extra 2:3:1:sz - Extra heading 2:0:12::1:::::1:6:3::::_heading_base:p:1:1:"),
_T("sp::A heading, to identify the speaker (e.g. Job) (basic):1:1::1::1:1:1:speaker:0:1:sp - Heading - Speaker:0:12::1:::::0:8:4::::_heading_base:q:1:1:"),
_T("d::A Hebrew text heading, to provide description (e.g. Psalms):1:::1::1:1:1:descr title:1:1:d - Heading - Descriptive Title - Hebrew Subtitle:0:12::1:::::1:4:4::::_heading_base:q:1:1:"),
_T("di::Descriptive title (Hebrew subtitle)::1::1::1:1:1:descr title:1:1:di - Descr title or Heb subtitle di:0:12::1:::::1:4:4::::_heading_base:q:1:1:"),
_T("hl::Hebrew letter::1:1:1:::1:::0::hl - Hebrew letter:0:12:::::::1:4:4::::_heading_base:q:1:1:"),
_T("r::Parallel reference(s) (basic):1:1:1:1::1:1:1:ref:33::r - Heading - Parallel References:0:12::1:::::1::4::::_heading_base:p:1:1:"),
_T("dvrf::Division reference::1:1:1::1::1:div-ref:0::dvrf - Division ref:0:12::1:::::1::3::::_heading_base:p:1:1:"),
_T("mr::A major section division references range heading (basic):1::1:1::1::1:mjr-sect-refs:0::mr - Heading - Major Section Range References:0:12::1:::::1::4::::ms:p:1:1:"),
_T("br::Psalms book reference::1:1:1::1::1:Ps-bk-ref:0::br - Psalms book ref:0:12::1:::::1::4::::r:c:1:1:"),
_T("x:x*:A list of cross references (basic):1::1:1:1:1:1:1:x-refs:33::x...x* - Cross Reference:4:10:::::::3::::::_notes_base:x:::"),
_T("xo:xo*:The cross reference origin reference (basic):1::1::1:1::::33::xo - Cross Reference - Origin Reference:1:10:::1::::3::::::::::"),
_T("xt:xt*:The cross reference target reference(s), protocanon only (basic):1::1::1:1::::33::xt - Cross Reference - Target References:1:10:::::::3::::::::::"),
_T("xk:xk*:A cross reference keyword:1::1::1:1::::33::xk - Cross Reference - Keyword:1:10::1:::::3::::::::::"),
_T("xq:xq*:A cross-reference quotation from the scripture text:1::1::1:1::::33::xq - Cross Reference - Quotation:1:10::1:::::3::::::::::"),
_T("xot:xot*:Cross-reference target reference(s), Old Testament only:1::1::1:1::::33::xot...xot* - Cross Reference - OT Target Refs (optional):1:12:::::::3::::::::::"),
_T("xnt:xnt*:Cross-reference target reference(s), New Testament only:1::1::1:1::::33::xnt...xnt* - Cross Reference - NT Target Refs (optional):1:12:::::::3::::::::::"),
_T("xdc:xdc*:Cross-reference target reference(s), Deuterocanon only:1::1::1:1::::33::xdc...xdc* - Cross Reference - DC Target Refs:1:10:::::::3::::::::::"),
_T("rr::Right margin reference::1:1:1::1::1:rt-marg-ref:32::rr - Right margin ref:0:9::1:::::2::::::qr:rr:::"),
_T("rq:rq*:A cross-reference indicating the source text for the preceding quotation:1::1:1::1::1:x-ref to source:32::rq...rq* - Cross Reference - Inline Quotation References:1:10::1:::::2::::::::::"),
_T("@::Cross reference, origin reference::1:1:1::1::1:x-refs orig:33::@ - Cross ref origin ref:1:10:::1::::3::::::::::"),
_T("xr::Cross reference target references::1:1:1::1::1:x-refs tgt:33::xr - Cross ref target ref:1:10:::::::3::::::::::"),
_T("p::Paragraph text, with first line indent (basic):1:1:::::1:1:paragraph:1:1:p - Paragraph - Normal - First Line Indent:0:12:::::::3:::::.125:_body_text:p:::"),
_T("pi::Paragraph text, level 1 indent (if sinlge level), with first line indent; often used for discourse (basic):1:1:::::1:1:para indented:1:1:pi - Paragraph - Indented - Level 1 - First Line Indent:0:12:::::::3:::.25:.25:.125:p:pi:::"),
_T("pi1::Paragraph text, level 1 indent (if multiple levels), with first line indent; often used for discourse:1::::::1:1:para indented L1:1:1:pi1 - Paragraph - Indented - Level 1 - First Line Indent:0:12:::::::3:::.25:.25:.125:pi:pi1:::"),
_T("pi2::Paragraph text, level 2 indent, with first line indent; often used for discourse:1::::::1:1:para indented L2:1:1:pi2 - Paragraph - Indented - Level 2 - First Line Indent:0:12:::::::3:::.5:.25:.125:pi1:pi2:::"),
_T("pi3::Paragraph text, level 3 indent, with first line indent; often used for discourse:1::::::1:1:para indented L3:1:1:pi3 - Paragraph - Indented - Level 3 - First Line Indent:0:12:::::::3:::.75:.25:.125:pi2:pi3:::"),
_T("pgi::Indented paragraph::1:::::1:1:para indented:1:1:pgi - Indented paragraph:0:12:::::::3:::.25:.25:.125:p:pgi:::"),
_T("ph::Paragraph text, with level 1 hanging indent (if single level):1::::::1:1:para hang indent:1:1:DEPRECATED ph - Paragraph - Hanging Indent - Level 1:0:12:::::::3:::.5::-.25:_body_text:ph:::"),
_T("ph1::Paragraph text, with level 1 hanging indent (if multiple levels):1::::::1:1:para hang indent L1:1:1:DEPRECATED ph1 - Paragraph - Hanging Indent - Level 1:0:12:::::::3:::.5::-.25:ph:ph1:::"),
_T("ph2::Paragraph text, with level 2 hanging indent:1::::::1:1:para hang indent L2:1:1:DEPRECATED ph2 - Paragraph - Hanging Indent - Level 2:0:12:::::::3:::.75::-.25:ph1:ph2:::"),
_T("ph3::Paragraph text, with level 3 hanging indent:1::::::1:1:para hang indent L3:1:1:DEPRECATED ph3 - Paragraph - Hanging Indent - Level 3:0:12:::::::3:::1::-.25:ph2:ph3:::"),
_T("phi::Paragraph text, indented with hanging indent:1::::::1:1:para indent hang indent:1:1:DEPRECATED phi - Paragraph - Indented - Hanging Indent:0:12:::::::3:::1:::_body_text:phi:::"),
_T("m::Paragraph text, with no first line indent (may occur after poetry) (basic):1:1:::::1:1:paragraph margin:1:1:m - Paragraph - Margin - No First Line Indent:0:12:::::::3::::::p:m:::"),
_T("pmo::Embedded text opening:1::::::1:1:para embedded text opening:1:1:pmo - Paragraph - Embedded Text Opening:0:12:::::::3:::.25:.25::pm:pm:::"),
_T("mi::Paragraph text, indented, with no first line indent; often used for discourse:1:1:::::1:1:para indent no 1st line indent:1:1:mi - Paragraph - Indented - No First Line Indent:0:12:::::::3:::.25:.25::pi:mi:::"),
_T("pc::Paragraph spanning chapters::1::1:::1:1:para spans chapters:1:1:pc - Paragraph spanning chapters:0:12:::::::3::::::m:pc:::"),
_T("pc::Paragraph text, centered (for Inscription):1::::::1:1:para centered inscription:1:1:pc - Paragraph - Centered (for Inscription):0:12:::::::1::::::_body_text:pc:::"),
_T("pr::Paragraph text, right aligned:1::::::1:1:para right aligned:1:1:DEPRECATED pr - Paragraph - Right Aligned:0:12:::::::2::::::p:pr:::"),
_T("pt::Preface title::1::1::1:1:1:preface title:0:1:pt - Preface title:0:14:::1::::1::6::::_heading_base:pp:1:1:"),
_T("ps::Preface section heading::1::1::1:1:1:preface sect head:0:1:ps - Preface sect heading:0:12:::1::::1:4:2::::s:pp:1:1:"),
_T("ps::Paragraph text, no break with next paragraph text at chapter boundary:1:::1:::1:1:para spans chapters:1:1:OBSOLETE ps - Paragraph - No Break with Next Paragraph:0:12:::::::3::::::m:ps:::"),
_T("psi::Paragraph text, indented, with no break with next paragraph text (at chapter boundary):1::::::1:1:para spans chapters indent:1:1:OBSOLETE psi - Paragraph - Indented - No Break with Next:0:12:::::::3:::.25:.25:.125:pi:pi:::"),
_T("pp::Preface paragraph::1::1::1:1:1:preface paragraph:0:1:pp - Preface paragraph:0:10:::::::3:::::.125:p:pp:::"),
_T("pq::Preface poetry::1::1::1:1:1:preface poetry:0:1:pq - Preface poetry:0:10:::::::3:::.5:::q:pq:::"),
_T("pm::Preface continue at margin::1::1::1:1:1:preface at margin:0:1:pm - Preface continue at margin:0:10:::::::3::::::m:pm:::"),
_T("pm::Embedded text paragraph:1:::::1:1:1:paragraph embedded text:1:1:pm - Paragraph - Embedded Text:0:12:::::::3:::.25:.25:.125:p:pm:::"),
_T("pmc::Embedded text closing:1:::::1:1:1:para embedded text closing:1:1:pmc - Paragraph - Embedded Text Closing:0:12:::::::3:::.25:.25::pm:pmc:::"),
_T("pmr::Embedded text refrain (e.g. Then all the people shall say, 'Amen!'):1:::::1:1:1:para embedded text refrain:1:1:pmr - Paragraph - Embedded Text Refrain:0:12:::::::2:::.25:.25::pm:p:::"),
_T("nb::Paragraph text, with no break from previous paragraph text (at chapter boundary) (basic):1::::::1:1:para no break:1:1:nb - Paragraph - No Break with Previous Paragraph:0:12:::::::3::::::m:p:::"),
_T("cls::Closure of an Epistle:1::::::1:1:Epistle close:1::cls - Paragraph - Closure of an Epistle:0:12:::::::2::::::p:cls:1:1:"),
_T("q::Poetry text, level 1 indent (if single level):1:1:::::1:1:poetry:2:1:q - Poetry - Indent Level 1 - Single Level Only:0:12:::::::3:::1.25::-1:_body_text:q:::"),
_T("q1::Poetry text, level 1 indent (if multiple levels) (basic):1::::::1:1:poetry L1:2:1:q1 - Poetry - Indent Level 1:0:12:::::::3:::1.25::-1:q:q1:::"),
_T("q2::Poetry text, level 2 indent (basic):1:1:::::1:1:poetry L2:2:1:q2 - Poetry - Indent Level 2:0:12:::::::3:::1.25::-.75:q:q2:::"),
_T("q3::Poetry text, level 3 indent:1:1:::::1:1:poetry L3:2:1:q3 - Poetry - Indent Level 3:0:12:::::::3:::1.25::-.5:q2:q3:::"),
_T("q4::Poetry text, level 4 indent:1:1:::::1:1:poetry L3:2:1:q4 - Poetry - Indent Level 4:0:12:::::::3:::1.25::-.25:q3:q4:::"),
_T("qc::Poetry text, centered:1:1:::::1:1:poetry centered:2:1:qc - Poetry - Centered:0:12:::::::1::::::q:qc:::"),
_T("qr::Poetry text, Right Aligned:1:1:::::1:1:poetry right margin:2:1:qr - Poetry - Right Aligned:0:12:::::::2::::::q:qr:::"),
_T("qa::Poetry text, Acrostic marker/heading:1:::1:::1:1:acrostic hdg:2:1:qa - Poetry - Acrostic Heading/Marker:0:12::1:::::3::::::_heading_base:q:1:1:"),
_T("qac:qac*:Poetry text, Acrostic markup of the first character of a line of acrostic poetry:1::::1::1:::2::qac...qac* - Poetry Text - Acrostic Letter:1:12::1:::::3::::::::::"),
_T("qs:qs*:Poetry text, Selah:1:::1:1::1:::6::qs...qs* - Poetry Text - Selah:1:12::1:::::3::::::::::"),
_T("qm::Poetry, left margin::1:::::1:1:poetry margin:2:1:qm - Poetry left margin:0:12:::::::3::::::q:qm:::"),
_T("qm::Poetry text, embedded, level 1 indent (if single level):1::::::1:1:poetry embed:2:1:qm - Poetry - Embedded Text - Indent Level 1 - Single Level Only:0:12:::::::3:::1::-.75:q:qm:::"),
_T("qm1::Poetry text, embedded, level 1 indent (if multiple levels):1::::::1:1:poetry embed L1:2:1:qm1 - Poetry - Embedded Text - Indent Level 1:0:12:::::::3:::1::-.75:qm:qm1:::"),
_T("qm2::Poetry text, embedded, level 2 indent:1::::::1:1:poetry embed L2:2:1:qm2 - Poetry - Embedded Text - Indent Level 2:0:12:::::::3:::1::-.5:qm1:qm2:::"),
_T("qm3::Poetry text, embedded, level 3 indent:1::::::1:1:poetry embed L3:2:1:qm3 - Poetry - Embedded Text - Indent Level 3:0:12:::::::3:::1::-.25:qm2:qm3:::"),
_T("f:f*:A Footnote text item (basic):1:1::1:1:1:1:1:footnote:9::f...f* - Footnote:4:10:::::::3::::::_notes_base:f:::"),
_T("fe::Footnote (end)::1:::::1:::1::fe - Footnote end PNG:1:10:::::::3::::::::::"),
_T("fe:fe*:An Endnote text item:1:::1:1:1:1:1:endnote:9::fe...fe* - Endnote:4:10:::::::3::::::_notes_base:fe:::"),
_T("fr:fr*:The origin reference for the footnote (basic):1::::1:1:1:::9::fr - Footnote - Reference:1:10:::1::::3::::::::::"),
_T("fk:fk*:A footnote keyword (basic):1::::1:1:1:::9::fk - Footnote - Keyword:1:10::1:1::::3::::::::::"),
_T("fq:fq*:A footnote scripture quote or alternate rendering (basic):1::::1:1:1:::9::fq - Footnote - Quotation or Alternate Rendering:1:10:::::::3::::::::::"),
_T("fqa:fqa*:A footnote alternate rendering for a portion of scripture text:1::::1:1:1:::9::fqa - Footnote - Alternate Translation Rendering:1:10::1:::::3::::::::::"),
_T("fl:fl*:A footnote label text item, for marking or 'labelling' the type or alternate translation being provided in the note.:1::::1:1:1:::9::fl - Footnote - Label Text:1:10::1:1::::3::::::::::"),
_T("fp:fp*:A Footnote additional paragraph marker:1::::1:1:1:::9::fp - Footnote Paragraph Mark:1:10:::::::3::::::::::"),
_T("ft:ft*:Footnote text, Protocanon (basic):1::::1:1:1:::9::ft - Footnote - Text:1:10:::::::3::::::::::"),
_T("fdc:fdc*:Footnote text, applies to Deuterocanon only:1::::1:1:1:::9::fdc...fdc* - Footnote - DC text:1:10:::::::3::::::::::"),
_T("fv:fv*:A verse number within the footnote text:1::::1:1::::9::fv...fv* - Footnote - Embedded Verse Number:1:10:::1:::1:3::::::::::"),
_T("fm:fm*:An additional footnote marker location for a previous footnote:1::::1:1::::9::fm - Footnote - Additional Caller to Previous Note:1:10::::::1:3::::::::::"),
_T("F::Footnote (end)::1:::::1:::1::F - Footnote end PNG:1:10:::::::3::::::::::"),
_T("qt:qt*:For Old Testament quoted text appearing in the New Testament (basic):1::::1::1:1:Quotation:1::qt...qt* - Special Text - Quoted Text - OT in NT:1:12::1:::::3::::::::::"),
_T("nd:nd*:For name of diety (basic):1::::1:::::6::nd...nd* - Special Text - Name of Deity:1:12::::1:::3::::::::::"),
_T("tl:tl*:For transliterated words:1::::1:::::6::tl...tl* - Special Text - Transliterated Word:1:12::1:::::3::::::::::"),
_T("dc:dc*:Deuterocanonical/LXX additions or insertions in the Protocanonical text:1::::1:::::6::dc...dc* - Special Text - Deuterocanonical/LXX Additions:1:12::1:::::3::::::::::"),
_T("bk:bk*:For the quoted name of a book:1::::1:::::6::bk...bk* - Special Text - Quoted book title:1:12::1:::::3::::::::::"),
_T("sig:sig*:For the signature of the author of an Epistle:1::::1:::::6::sig...sig* - Special Text - Author's Signature (Epistles):1:12::1:::::3::::::::::"),
_T("pn:pn*:For a proper name:1::::1:::::6::pn...pn* - Special Text - Proper Name:1:12:::1:1:::3::::::::::"),
_T("wj:wj*:For marking the words of Jesus:1::::1:::::1::wj...wj* - Special Text - Words of Jesus:1:12:255::::::3::::::::::"),
_T("k:k*:For a keyword:1::::1:::::6::k...k* - Special Text - Keyword:1:12::1:1::::3::::::::::"),
_T("sls:sls*:To represent where the original text is in a secondary language or from an alternate text source:1::::1:::::1::sls...sls* - Special Text - Secondary Language or Text Source:1:12::1:::::3::::::::::"),
_T("ord:ord*:For the text portion of an ordinal number:1::::1:::::6::ord...ord* - Special Text - Ordinal number text portion:1:12::::::1:3::::::::::"),
_T("add:add*:For a translational addition to the text:1::::1:1:1:1:addl material:0::add...add* - Special Text - Translational Addition:1:12:2263842:1:1::::3::::::::::"),
_T("no:no*:A character style, use normal text:1::::1:::::1::no...no* - Character - Normal Text:1:12:::::::3::::::::::"),
_T("bd:bd*:A character style, use bold text:1::::1:::::6::bd...bd* - Character - Bold Text:1:12:::1::::3::::::::::"),
_T("it:it*:A character style, use italic text:1::::1:::::6::it...it* - Character - Italic Text:1:12::1:::::3::::::::::"),
_T("bdit:bdit*:A character style, use bold + italic text:1::::1:::::6::bdit...bdit* - Character - BoldItalic Text:1:12::1:1::::3::::::::::"),
_T("em:em*:A character style, use emphasized text style:1::::1:::::6::em...em* - Character - Emphasized Text:1:12::1:::::3::::::::::"),
_T("sc:sc*:A character style, for small capitalization text:1::::1:::::6::sc...sc* - Character - Small Caps:1:12:::::1::3::::::::::"),
_T("pro:pro*:For indicating pronunciation in CJK texts:1::1:1:1:::::6::pro...pro* - Special Text - CJK Pronunciation:1:10:128::::::3::::::::::"),
_T("imt::Introduction main title, level 1 (if single level) (basic):1:::1::1:1:1:intro main title:0:1:imt - Introduction - Major Title Level 1:0:14:::1::::1:8:4::::_intro_base:ip:1:1:"),
_T("imt1::Introduction major title, level 1 (if multiple levels):1:::1::1:1:1:intro major title L1:0:1:imt1 - Introduction - Major Title Level 1:0:14:::1::::1:8:4::::imt:ip:1:1:"),
_T("imt2::Introduction major title, level 2:1:::1::1:1:1:intro major title L2:0:1:imt2 - Introduction - Major Title Level 2:0:13::1:::::1:6:3::::imt1:ip:1:1:"),
_T("imt3::Introduction major title, level 3:1:::1::1:1:1:intro major title L3:0:1:imt3 - Introduction - Major Title Level 3:0:12:::1::::1:2:2::::imt2:ip:1:1:"),
_T("imt4::Introduction major title, level 4 (usually within parenthesis):1:::1::1:1:1:intro major title L4:0:1:imt4 - Introduction - Major Title Level 4:0:12::1:::::1:2:2::::imt3:ip:1:1:"),
_T("imte::Introduction major title at introduction end, level 1 (if single level):1:::1::1:1:1:intro major title at end:0:1:imte - Introduction - [Uncommon] Major Title at Introduction End Level 1:0:20:::1::::1::::::imt:ie:::"),
_T("imte1::Introduction major title at introduction end, level 1 (if multiple levels):1:::1::1:1:1:intro major title at end:0:1:imte - Introduction - [Uncommon] Major Title at Introduction End Level 1:0:20:::1::::1::::::imt:ie:::"),
_T("imte2::Introduction major title at introduction end, level 2:1:::1::1:1:1:intro major title at end:0:1:imte - Introduction - [Uncommon] Major Title at Introduction End Level 2:0:16::1:::::1::::::imt:ie:::"),
_T("is::Introduction section heading, level 1 (if single level) (basic):1:1::1::1:1:1:intro sect head:0:1:is - Introduction - Section Heading Level 1:0:12:::1::::1:8:4::::s:ip:1:1:"),
_T("is1::Introduction section heading, level 1 (if multiple levels):1:::1::1:1:1:intro sect head L1:0:1:is1 - Introduction - Section Heading Level 1:0:12:::1::::1:8:4::::is:ip:1:1:"),
_T("is2::Introduction section heading, level 2:1:::1::1:1:1:intro sect head L2:0:1:is2 - Introduction - Section Heading Level 2:0:12:::1::::1:8:4::::is1:ip:1:1:"),
_T("ip::Introduction prose paragraph (basic):1:1::1::1:1:1:intro paragraph:0:1:ip - Introduction - Paragraph:0:10:::::::3:::::.125:_intro_base:ip:::"),
_T("ipi::Introduction prose paragraph, indented, with first line indent:1:1::1::1:1:1:intro paragraph indented:0:1:ipi - Introduction - Indented Para - first line indent:0:10:::::::3:::.25:.25:.125:ip:ipi:::"),
_T("ipq::Introduction prose paragraph, quote from the body text:1:::1::1:1:1:intro para quote:0:1:ipq - Introduction - Paragraph - quote from text:0:10::1:::::3:::.25:.25:.125:ip:ipq:::"),
_T("ipr::Introduction prose paragraph, right aligned:1:::1::1:1:1:intro para right align:0:1:ipr - Introduction - Paragraph - right aligned:0:10::1:::::2:::.25:.25::ip:ipr:::"),
_T("iq::Introduction poetry text, level 1 (if single level):1:1::1::1:1:1:intro poetry:0:1:iq - Introduction - Poetry Level 1:0:10::1:::::3:::1::-.75:ip:iq:::"),
_T("iq1::Introduction poetry text, level 1 (if multiple levels):1:::1::1:1:1:intro poetry L1:0:1:iq1 - Introduction - Poetry Level 1:0:10:::::::3:::1::-.75:iq:iq1:::"),
_T("iq2::Introduction poetry text, level 2:1:1::1::1:1:1:intro poetry L2:0:1:iq2 - Introduction - Poetry Level 2:0:10:::::::3:::1::-.5:iq:iq2:::"),
_T("iq3::Introduction poetry text, level 3:1:::1::1:1:1:intro poetry L3:0:1:iq3 - Introduction - Poetry Level 3:0:10:::::::3:::1::-.25:iq:iq3:::"),
_T("im::Introduction prose paragraph, with no first line indent (may occur after poetry):1:1::1::1:1:1:intro para no indent:0:1:im - Introduction - Paragraph - no first line indent:0:10:::::::3::::::ip:im:::"),
_T("imi::Introduction prose paragraph text, indented, with no first line indent:1:1::1::1:1:1:intro para no indent:0:1:imi - Introduction - Indented Para - no first line indent:0:10:::::::3:::.25:.25::ipi:imi:::"),
_T("ili::A list entry, level 1 (if single level):1:::1::1:1:1:intro list L1:0:1:ili - Introduction - List Entry - Level 1:0:12:::::::3:::.625::-.375:_list_base:ili:::"),
_T("ili1::A list entry, level 1 (if multiple levels):1:::1::1:1:1:intro list L1:0:1:ili1 - Introduction - List Entry - Level 1:0:12:::::::3:::.5::-.25:ili:ili1:::"),
_T("ili2::A list entry, level 2:1:::1::1:1:1:intro list L2:0:1:ili2 - Introduction - List Entry - Level 2:0:12:::::::3:::.75::-.25:ili1:ili2:::"),
_T("imq::Introduction prose paragraph, quote from the body text, with no first line indent:1:::1::1:1:1:intro para quote no indent:0:1:imq - Introduction - Paragraph - quote from text - no first line indent:0:10::1:::::3:::.25:.25::imi:imq:::"),
_T("ib::Introduction blank line:1::1:1::1::::0:1:ib - Introduction - Blank Line:0:12:::::::3::::::_intro_base:ib:::"),
_T("iot::Introduction outline title (basic):1:::1::1:1:1:intro outline title:0:1:iot - Introduction - Outline Title:0:12:::1::::1:8:4::::imt:io1:1:1:"),
_T("io::Introduction outline text, level 1 (if single level):1:1::1::1:1:1:intro outline:0:1:io - Introduction - Outline Level 1:0:10:::::::3:::.5:::_intro_base:io:::"),
_T("io1::Introduction outline text, level 1 (if multiple levels) (basic):1:1::1::1:1:1:intro outline L1:0:1:io1 - Introduction - Outline Level 1:0:10:::::::3:::.5:::io:io1:::"),
_T("io2::Introduction outline text, level 2:1:1::1::1:1:1:intro outline L2:0:1:io2 - Introduction - Outline Level 2:0:10:::::::3:::.75:::io1:io2:::"),
_T("io3::Introduction outline text, level 3:1:1::1::1:1:1:intro outline L3:0:1:io3 - Introduction - Outline Level 3:0:10:::::::3:::1:::io2:io3:::"),
_T("io4::Introduction outline text, level 4:1:1::1::1:1:1:intro outline L4:0:1:io4 - Introduction - Outline Level 4:0:10:::::::3:::1.25:::io3:io4:::"),
_T("ior:ior*:Introduction references range for outline entry; for marking references separately:1:::1:1:1::::6::ior...ior* - Introduction - Outline References Range:1:10:::::::3::::::::::"),
_T("iex::Introduction explanatory or bridge text (e.g. explanation of missing book in Short Old Testament):1:::1::1:1:1:intro explain text:0:1:iex - Introduction - Explanatory or Bridge Text:0:10:::::::3:4:4:::.125:ip:iex:::"),
_T("iqt:iqt*:For quoted scripture text appearing in the introduction:1:::1:1:1::::6::iqt...iqt* - Special Text - Quoted Scripture Text in Introduction:1:12::1:::::3::::::::::"),
_T("ie::Introduction ending marker:1::1:::1:1:::0::ie - Introduction - End Marker:8:12:::::::3:12:4:1.5:1.5::_intro_base:ie:::"),
_T("li::A list entry, level 1 (if single level):1::::::1:1:list item:1:1:li - List Entry - Level 1:0:12:::::::0:::.625::-.375:_list_base:li:::"),
_T("li1::A list entry, level 1 (if multiple levels):1::::::1:1:list item L1:1:1:li1 - List Entry - Level 1:0:12:::::::0:::.5::-.25:li:li1:::"),
_T("li2::A list entry, level 2:1::::::1:1:list item L2:1:1:li2 - List Entry - Level 2:0:12:::::::0:::.75::-.25:li1:li2:::"),
_T("li3::A list entry, level 3:1::::::1:1:list item L3:1:1:li3 - List Entry - Level 3:0:12:::::::0:::1::-.25:li2:li3:::"),
_T("li4::A list entry, level 4:1::::::1:1:list item L4:1:1:li4 - List Entry - Level 4:0:12:::::::0:::1.25::-.25:li3:li4:::"),
_T("qh::List or Genealogy::1:::::1:1:list item:1:1:qh - List or Genealogy:0:12:::::::0:::.625::-.375:q:qh:::"),
_T("tr::A new table row:1::::::1:1:table row:1:1:tr - Table - Row:0:12:::::::0:::.5::-.25:_body_text:tr:::"),
_T("tr1::A table Row:1::::::1:1:table row L1:1:1:OBSOLETE tr1 - Table - Row - Level 1:0:12:::::::0:::.5::-.25:tr:tr1:::"),
_T("tr2::A table Row:1::::::1:1:table row L2:1:1:OBSOLETE tr2 - Table - Row - Level 2:0:12:::::::0:::.75::-.25:tr1:tr2:::"),
_T("th1::A table heading, column 1:1::::::1:::1::th1 - Table - Column 1 Heading:1:12::1:::::0::::::::::"),
_T("th2::A table heading, column 2:1::::::1:::1::th2 - Table - Column 2 Heading:1:12::1:::::0::::::::::"),
_T("th3::A table heading, column 3:1::::::1:::1::th3 - Table - Column 3 Heading:1:12::1:::::0::::::::::"),
_T("th4::A table heading, column 4:1::::::1:::1::th4 - Table - Column 4 Heading:1:12::1:::::0::::::::::"),
_T("thr1::A table heading, column 1, right aligned:1::::::1:::1::thr1 - Table - Column 1 Heading - Right Aligned:1:12::1:::::2::::::::::"),
_T("thr2::A table heading, column 2, right aligned:1::::::1:::1::thr2 - Table - Column 2 Heading - Right Aligned:1:12::1:::::2::::::::::"),
_T("thr3::A table heading, column 3, right aligned:1::::::1:::1::thr3 - Table - Column 3 Heading - Right Aligned:1:12::1:::::2::::::::::"),
_T("thr4::A table heading, column 4, right aligned:1::::::1:::1::thr4 - Table - Column 4 Heading - Right Aligned:1:12::1:::::2::::::::::"),
_T("tc1::A table cell item, column 1:1::::::1:::1::tc1 - Table - Column 1 Cell:1:12:::::::0::::::::::"),
_T("tc2::A table cell item, column 2:1::::::1:::1::tc2 - Table - Column 2 Cell:1:12:::::::0::::::::::"),
_T("tc3::A table cell item, column 3:1::::::1:::1::tc3 - Table - Column 3 Cell:1:12:::::::0::::::::::"),
_T("tc4::A table cell item, column 4:1::::::1:::1::tc4 - Table - Column 4 Cell:1:12:::::::0::::::::::"),
_T("tcr1::A table cell item, column 1, right aligned:1::::::1:::1::tcr1 - Table - Column 1 Cell - Right Aligned:1:12:::::::2::::::::::"),
_T("tcr2::A table cell item, column 2, right aligned:1::::::1:::1::tcr2 - Table - Column 2 Cell - Right Aligned:1:12:::::::2::::::::::"),
_T("tcr3::A table cell item, column 3, right aligned:1::::::1:::1::tcr3 - Table - Column 3 Cell - Right Aligned:1:12:::::::2::::::::::"),
_T("tcr4::A table cell item, column 4, right aligned:1::::::1:::1::tcr4 - Table - Column 4 Cell - Right Aligned:1:12:::::::2::::::::::"),
_T("gm::Glossary main entry::1::1::1:1:1:glossary main entry:0:1:gm - Glossary main entry:0:12:::::::3::::::_body_text:gp:::"),
_T("gs::Glossary subentry::1::1::1:1:1:glossary subentry:0:1:gs - Glossary subentry:0:12:::::::3::::::gm:gp:::"),
_T("gd::Glossary definition::1::1:1:1:1:1:glossary definition:0:1:gd - Glossary definition:1:12:::::::3::::::::::"),
_T("gp::Glossary paragraph::1::1::1:1:1:glossary paragraph:0:1:gp - Glossary paragraph:0:12:::::::3::::::_body_text:gp:::"),
_T("tis::Topical index heading (level 1)::1::1:::1:1:topical index L1:0:1:tis - Topical index heading L1:0:12:::::::3::::::_heading_base:tpi:::"),
_T("tpi::Topical index heading (level 2)::1::1:::1:1:topical index L2:0:1:tpi - Topical index heading L2:0:12:::::::3::::::tis:tps:::"),
_T("tps::Topical index heading (level 3)::1::1:::1:1:topical index L3:0:1:tps - Topical index heading L3:0:12:::::::3::::::tpi:tir:::"),
_T("tir::Topical index reference::1::1:::1:1:topical index reference:0:1:tir - Topical index ref:0:12:::::::3::::::_body_text:tir:::"),
_T("w:w*:A wordlist text item:1::::1::1:::6::w...w* - Peripheral Ref - Wordlist Entry:1:12:16711935:1:1::::3::::::::::"),
_T("wr:wr*:A Wordlist text item:1::::1::1:::6::OBSOLETE wr...wr* - Auxiliary - Wordlist/Glossary Reference:1:12::1:::::3::::::::::"),
_T("wh:wh*:A Hebrew wordlist text item:1::::1::1:::6::wh...wh* - Peripheral Ref - Hebrew Wordlist Entry:1:12:16711935:1:1::::3::::::::::"),
_T("wg:wg*:A Greek Wordlist text item:1::::1::1:::6::wg...wg* - Peripheral Ref - Greek Wordlist Entry:1:12:16711935:1:1::::3::::::::::"),
_T("ndx:ndx*:A subject index text item:1::::1::1:::6::ndx...ndx* - Peripheral Ref - Subject Index Entry:1:12:16711935:1:1::::3::::::::::"),
_T("periph::Periheral content division marker which should be followed by an additional division argument/title.:1:::1:::1:1:Periph matter div:0::periph - Peripherals - Content Division Marker:0:14:33023::1::::3:16:4::::_peripherals_base:periph:::"),
_T("p1::Front or back matter text paragraph, level 1 (if multiple levels):1:::::1:1:1:Periph matter para L1:0:1:p1 - Periph - Front/Back Matter Paragraph Level 1:0:12:::::::3:::::.125:_peripherals_base:p1:::"),
_T("p2::Front or back matter text paragraph, level 2 (if multiple levels):1:::1::1:1:1:Periph matter para L2:0:1:p2 - Periph - Front/Back Matter Paragraph Level 2:0:12:::::::3:::.125::.125:_peripherals_base:p2:::"),
_T("k1::Concordance main entry text or keyword, level 1:1:::::1:1:1:conc main entry/keyword L1:0:1:k1 - Periph - Concordance Keyword Level 1:0:12:::::::3::::::_peripherals_base:k1:::"),
_T("k2::Concordance main entry text or keyword, level 2:1:::::1:1:1:conc main entry/keyword L2:0:1:k2 - Periph - Concordance Keyword Level 2:0:12:::::::3::::::_peripherals_base:k2:::"),
_T("xtSee:xtSee*:Concordance and Names Index markup for an alternate entry target reference.:1:::1:::1:::0::xtSee - Concordance and Names Index - Alternate Entry Target Reference:1:12:16711680:1:::::3::::::::::"),
_T("xtSeeAlso:xtSeeAlso*:Concordance and Names Index markup for an additional entry target reference.:1:::1:::1:::0::xtSeeAlso - Concordance and Names Index - Additional Entry Target Reference:1:12:16711680:1:::::3::::::::::"),
_T("pub::Front matter publication data:1:::1:::1:::0::OBSOLETE pub Peripherals - Front Matter Publication Data:0:10:::::::3::::::_peripherals_base:pub:::"),
_T("toc::Front matter table of contents:1:::1:::1:::0::OBSOLETE toc Peripherals - Front Matter Table of Contents:0:10:::::::3::::::_peripherals_base:toc:::"),
_T("toc1::Long table of contents text:1:::1:::1:::0::toc1 - File - Long Table of Contents Text:0:12:16384:1:1::::3::::::_peripherals_base:toc1:::"),
_T("toc2::Short table of contents text:1:::1:::1:::0::toc2 - File - Short Table of Contents Text:0:12:16384:1:::::3::::::_peripherals_base:toc2:::"),
_T("toc3::Book Abbreviation:1:::1:::1:::0::toc3 - File - Book Abbreviation:0:12:128:1:1::::3::::::_peripherals_base:toc3:::"),
_T("pref::Front matter preface:1:::1:::1:::0::OBSOLETE pref Peripherals - Front Matter Preface:0:10:::::::3::::::_peripherals_base:pref:::"),
_T("intro::Front matter introduction:1:::1:::1:::0::OBSOLETE intro Peripherals - Front Matter Introduction:0:10:::::::3::::::_peripherals_base:intro:::"),
_T("conc::Back matter concordance:1:::1:::1:::0::OBSOLETE conc Peripherals - Back Matter Concordance:0:10:::::::3::::::_peripherals_base:conc:::"),
_T("glo::Back matter glossary:1:::1:::1:::0::OBSOLETE glo Peripherals - Back Matter Glossary:0:10:::::::3::::::_peripherals_base:glo:::"),
_T("idx::Back matter index:1:::1:::1:::0::OBSOLETE idx Peripherals - Back Matter Index:0:10:::::::3::::::_peripherals_base:idx:::"),
_T("maps::Back matter map index:1:::1:::1:::0::OBSOLETE maps Peripherals - Back Matter Map Index:0:10:::::::3::::::_peripherals_base:maps:::"),
_T("cov::Other peripheral materials - cover:1:::1:::1:::0::OBSOLETE cov Peripherals - Other - Cover:0:10:::::::3::::::_peripherals_base:cov:::"),
_T("spine::Other peripheral materials - spine:1:::1:::1:::0::OBSOLETE spine Peripherals - Other - Spine:0:10:::::::3::::::_peripherals_base:spine:::"),
_T("pubinfo::Publication information - Lang,Credit,Version,Copies,Publisher,Id,Logo:1:::1:::1:::0::OBSOLETE pubinfo - Publication - Information:0:12:16711680::::::3::::::__normal:pubinfo:::"),
_T("pb::Page Break used for new reader portions and children's bibles where content is controlled by the page:1::::::1:1:new page:0:1:pb - Break - Page Break:0:12:::::::3::::::p:p:::"),
_T("b::Poetry text stanza break (e.g. stanza break) (basic):1::::::1:1:stanza break:2::b - Poetry - Stanza Break (Blank Line):0:12:::::::0::::::_body_text:b:::"),
_T("hr::Horizontal rule::1:1:::1:1:::0::hr - Horizontal rule:8:12:::::::3:4:4::::_body_text:hr:::"),
_T("fig:fig*:Illustration [Columns to span, height, filename, caption text]:1::1:1:1::1:::0::fig...fig* - Auxiliary - Figure/Illustration/Map:1:12:::::::3::::::::::"),
_T("loc::Picture location::1:1::::1:::0::loc - Picture location:0:12:::::::3::::::_body_text:loc:::"),
_T("cap::Picture caption::1:1:1::1:1:1:picture caption:0::cap - Picture caption:0:12:::::::3::::::_body_text:cap:::"),
_T("cat::Picture catalog number::1:1::::1:::0::cat - Picture catalog number:0:12:::::::3::::::_body_text:cat:::"),
_T("des::Picture description::1:1::::1:::0::des - Picture description:0:12:::::::3::::::_body_text:des:::"),
_T("px::Paragraph extra 1::1:::::1:1:para extra 1:1:1:px - Paragraph extra 1:0:12:::::::3::::::p:px:::"),
_T("pz::Paragraph extra 2::1:::::1:1:para extra 2:1:1:pz - Paragraph extra 2:0:12:::::::3::::::p:pz:::"),
_T("qx::Poetry extra 1::1:::::1:1:poetry extra 1:2:1:qx - Poetry extra 1:0:12:::::::3::::::q:qx:::"),
_T("qz::Poetry extra 2::1:::::1:1:poetry extra 2:2:1:qz - Poetry extra 2:0:12:::::::3::::::q:qz:::"),
_T("addpn:addpn*:For chinese words to be dot underline and underline:1::::1::1:::1::(addpn...addpn*) - Special Text for Chinese:1:12:2263842:1:1:1:::3::::::::::"),
_T("efm:efm*:ID or Caller for an extended (study) note. Used within a source project duplicte (target) text when autoring study material.:1::1:::1:1:::0::efm - Study Note - ID/Caller:1:10:255::1::::3::::::::::"),
_T("ef:ef*:A Study Note text item:1::1:::1:1:::0::ef...ef* - Study Note:1:12:::::::3::::::::::"),
_T("bt::Back-translation (and all \bt... initial forms):1:1:1:::1::1:back-trans:0::bt - Back-translation:1:12:16711680::::::3::::::::::"),
_T("free:free*:Free translation:1:1:1:::1::1:free-trans:0::free - Free translation:1:12:4194500::::::3::::::::::"),
_T("note:note*:Adapt It note:1:1:1:::1::1:note:0::note - Adapt It note:1:10:16711680::::::3::::::::::"),
_T("__normal::Normal:1:1:1:::::::0::Normal:0:11:::::::3:::::::__normal:::"),
_T("_src_lang_interlinear::Source Language Interlinear Text:1:1:1:::::::0::Source Language:0:11:::::::0::::::__normal:_src_lang_interlinear::1:"),
_T("_tgt_lang_interlinear::Target Language Interlinear Text:1:1:1:::::::0::Target Language:0:11:::::::0::::::__normal:_tgt_lang_interlinear::1:"),
_T("_gls_lang_interlinear::Gloss Language Interlinear Text:1:1:1:::::::0::Gloss Language:0:11:::::::0::::::__normal:_gls_lang_interlinear::1:"),
_T("_nav_lang_interlinear::Navigation Language Interlinear Text:1:1:1:::::::0::Navigation Language:0:11:::::::0::::::__normal:_nav_lang_interlinear::1:"),
_T("_hdr_ftr_interlinear::Header-Footer Interlinear Text:1:1:1:::::::0::Hdr-Ftr Interlinear:7:11:::::::0::::::__normal:_hdr_ftr_interlinear:::"),
_T("_small_para_break::Small Paragraph Break:1:1:1:::::::0::Small Para Break:0:4:::::::3::::::__normal:_small_para_break:::"),
_T("_body_text::Body Text:1:1:1:::::::0::_BodyText_Base:0:11:::::::3::::::_vernacular_base:_body_text:::"),
_T("_heading_base::Heading Base:1:1:1:::1::::0::_Heading_Base:0:12:::::::1:6::.1:.1::_vernacular_base:_heading_base:1:1:"),
_T("_intro_base::Intro Base:1:1:1:::1::::0::_Intro_Base:0:11::1:::::3::::::_vernacular_base:_intro_base:::"),
_T("_list_base::List Base:1:1:1:::1::::0::_List_Base:0:12:::::::3::::::_vernacular_base:_list_base:::"),
_T("_notes_base::Notes Base:1:1:1:::1::::0::_Notes_Base:0:9:::::::3::::::_vernacular_base:_notes_base:::"),
_T("_peripherals_base::Peripherals Base:1:1:1:::1::::0::_Peripherals_Base:0:10:::::::3::::::_vernacular_base:_peripherals_base:::"),
_T("_vernacular_base::Vernacular Base:1:1:1:::::::0::_Vernacular_Base:0:11:::::::3::::::__normal:_vernacular_base:::"),
_T("_annotation_ref::Annotation Reference:1:1:1:::::::0::_annotation_reference:1:10:4210943:::::1:3::::::::::"),
_T("_annotation_text::Annotation Text:1:1:1:::::::0::_annotation_text:0:10:::::::0::::::__normal:_annotation_text:::"),
_T("_dft_para_font::Default Paragraph Font:1:1:1:::::::0::Default Paragraph Font:5:10:::::::3::::::::::"),
_T("_footnote_caller::Footnote Caller:1:1:1:::::::0::Footnote Caller:3:10:16711680:::::1:3::::::::::"),
_T("_normal_table::Normal Table:1:1:1:::::::0::Normal Table:2:10:::::::0:::::::_normal_table:::"),
_T("_table_grid::Table Grid:1:1:1:::::::0::Table Grid:2:10:::::::0::::::_normal_table:_table_grid:::"),
_T("_footer::Footer:1:1:1:::::::0::footer:6:10:::::::3::::::_body_text:_footer:::"),
_T("_header::Header:1:1:1:::::::0::header:7:10:::::::3::::::_body_text:_header:::"),
_T("_horiz_rule::Horizontal Rule:1:1:1:::::::0::Horizontal rule:8:10:::::::3:::::::_horiz_rule:::"),
_T("_single_boxed_para::Single Boxed Paragraph:1:1:1:::::::0::Single Boxed Paragraph:9:10:::::::3::::::__normal:_single_boxed_para:::"),
_T("_double_boxed_para::Double Boxed Paragraph:1:1:1:::::::0::Double Boxed Paragraph:9:10:::::::3::::::__normal:_double_boxed_para:::"),
_T("_unknown_para_style::Unknown Paragraph Style Marker:1:1:1:::::::0::Unknown Para Style Marker:0:12:255::::::3::::::_body_text:_unknown_para_style:::"),
_T("_unknown_char_style::Unknown Character Style Marker:1:1:1:::::::0::Unknown Char Style Marker:1:12:255::::::3::::::::::"),
_T("_hidden_note::Hidden Note:1:1:1:::::::0::Hidden Note:10:10:8388608:1:::::3:2::::.3:p:_hidden_note:::")
};


////////////////////////////////////////////////////////////////////////////////////////////
/// \return     pointer to a BookNamePair struct whose members have been assigned
/// \param      pPair      <- the struce whose members are assigned values
/// \param      dirNm      -> the book's folder name used for actual folder/dir creation
/// \param      seeNm      -> the book's visible name (localizable) for the interface
/// \param      bookcode   -> three-letter code for the book (should be same as used on \id line)
/// \remarks
/// Called from: SetupDefaultBooksArray(). Assigns the members of the BookNamePair struct 
/// pointed to by pPair with the values pointed to by the function's parameters.
////////////////////////////////////////////////////////////////////////////////////////////
BookNamePair* MakePair(BookNamePair*& pPair, wxChar* dirNm, wxChar* seeNm, wxChar* bookcode)
{
	pPair->dirName = dirNm;
	pPair->seeName = seeNm;
	pPair->bookCode = bookcode;
	return pPair;
}

/// A global scratch pointer. Instances of BookNamePair are stored in the m_pBibleBooks pointer array.
BookNamePair* gpBookNamePair; 

/// A global scratch pointer. Instances of USFMAnalysis are stored in the m_pUsfmStylesMap, 
/// m_pPngStylesMap, and m_pUsfmAndPngStylesMap (of type MapSfmToUSFMAnalysisStruct).
USFMAnalysis* gpUsfmAnalysis; 

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pBookStructs  <- the array of void pointers to hold the pointers to book structs
/// \remarks
/// Called from: the App's OnInit() if, on app startup, the app cannot find the books.xml file. 
/// SetupDefaultBooksArray() is called in order to establish a default list of books/divisions
/// for the application to use in the event that a books.xml file is not available.
////////////////////////////////////////////////////////////////////////////////////////////
void SetupDefaultBooksArray(wxArrayPtrVoid* pBookStructs)
{
	wxArrayPtrVoid singles;
	wxArrayPtrVoid codes;
	SetupSinglesArray(&singles);
	SetupBookCodesArray(&codes);
	for (int i = 0; i < 67; i++)
	{
		wxChar* book = (wxChar*)singles[i];
		wxChar* itsCode = (wxChar*)codes[i];
		gpBookNamePair = new BookNamePair;
		pBookStructs->Add((void*)MakePair(gpBookNamePair,book,book,itsCode));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pBooks  <- the array of void pointers to hold the pointers to book structs
/// \remarks
/// Called from: the App's SetupDefaultBooksArray().  SetupDefaultBooksArray() in turn is only
/// called if the app cannot find the books.xml file.
////////////////////////////////////////////////////////////////////////////////////////////
void SetupSinglesArray(wxArrayPtrVoid* pBooks)
{
	//pBooks->SetSize(68,10); // space for all 67, with a bit to spare, and nGrowBy of 10
	pBooks->Add((void*)_00BK);
	pBooks->Add((void*)_01BK);
	pBooks->Add((void*)_02BK);
	pBooks->Add((void*)_03BK);
	pBooks->Add((void*)_04BK);
	pBooks->Add((void*)_05BK);
	pBooks->Add((void*)_06BK);
	pBooks->Add((void*)_07BK);
	pBooks->Add((void*)_08BK);
	pBooks->Add((void*)_09BK);
	pBooks->Add((void*)_10BK);
	pBooks->Add((void*)_11BK);
	pBooks->Add((void*)_12BK);
	pBooks->Add((void*)_13BK);
	pBooks->Add((void*)_14BK);
	pBooks->Add((void*)_15BK);
	pBooks->Add((void*)_16BK);
	pBooks->Add((void*)_17BK);
	pBooks->Add((void*)_18BK);
	pBooks->Add((void*)_19BK);
	pBooks->Add((void*)_20BK);
	pBooks->Add((void*)_21BK);
	pBooks->Add((void*)_22BK);
	pBooks->Add((void*)_23BK);
	pBooks->Add((void*)_24BK);
	pBooks->Add((void*)_25BK);
	pBooks->Add((void*)_26BK);
	pBooks->Add((void*)_27BK);
	pBooks->Add((void*)_28BK);
	pBooks->Add((void*)_29BK);
	pBooks->Add((void*)_30BK);
	pBooks->Add((void*)_31BK);
	pBooks->Add((void*)_32BK);
	pBooks->Add((void*)_33BK);
	pBooks->Add((void*)_34BK);
	pBooks->Add((void*)_35BK);
	pBooks->Add((void*)_36BK);
	pBooks->Add((void*)_37BK);
	pBooks->Add((void*)_38BK);
	pBooks->Add((void*)_39BK);
	pBooks->Add((void*)_40BK);
	pBooks->Add((void*)_41BK);
	pBooks->Add((void*)_42BK);
	pBooks->Add((void*)_43BK);
	pBooks->Add((void*)_44BK);
	pBooks->Add((void*)_45BK);
	pBooks->Add((void*)_46BK);
	pBooks->Add((void*)_47BK);
	pBooks->Add((void*)_48BK);
	pBooks->Add((void*)_49BK);
	pBooks->Add((void*)_50BK);
	pBooks->Add((void*)_51BK);
	pBooks->Add((void*)_52BK);
	pBooks->Add((void*)_53BK);
	pBooks->Add((void*)_54BK);
	pBooks->Add((void*)_55BK);
	pBooks->Add((void*)_56BK);
	pBooks->Add((void*)_57BK);
	pBooks->Add((void*)_58BK);
	pBooks->Add((void*)_59BK);
	pBooks->Add((void*)_60BK);
	pBooks->Add((void*)_61BK);
	pBooks->Add((void*)_62BK);
	pBooks->Add((void*)_63BK);
	pBooks->Add((void*)_64BK);
	pBooks->Add((void*)_65BK);
	pBooks->Add((void*)_66BK);
	// Access using code like this:
	// Int16 index = some value;
	// wxChar* bookP = (wxChar*)(*m_pBibleBooks)[index];
	// or wxString aBookName = (wxChar*)(*m_pBibleBooks)[index];
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pBookCodes  <- the array of void pointers to hold the pointers to book codes
/// \remarks
/// Called from: the App's SetupDefaultBooksArray().  SetupDefaultBooksArray() in turn is only
/// called if the app cannot find the books.xml file.
////////////////////////////////////////////////////////////////////////////////////////////
void SetupBookCodesArray(wxArrayPtrVoid* pBookCodes)
{
	//pBookCodes->SetSize(68,10); // space for all 67, with a bit to spare, and nGrowBy of 10
	pBookCodes->Add((void*)_00BKCODE);
	pBookCodes->Add((void*)_01BKCODE);
	pBookCodes->Add((void*)_02BKCODE);
	pBookCodes->Add((void*)_03BKCODE);
	pBookCodes->Add((void*)_04BKCODE);
	pBookCodes->Add((void*)_05BKCODE);
	pBookCodes->Add((void*)_06BKCODE);
	pBookCodes->Add((void*)_07BKCODE);
	pBookCodes->Add((void*)_08BKCODE);
	pBookCodes->Add((void*)_09BKCODE);
	pBookCodes->Add((void*)_10BKCODE);
	pBookCodes->Add((void*)_11BKCODE);
	pBookCodes->Add((void*)_12BKCODE);
	pBookCodes->Add((void*)_13BKCODE);
	pBookCodes->Add((void*)_14BKCODE);
	pBookCodes->Add((void*)_15BKCODE);
	pBookCodes->Add((void*)_16BKCODE);
	pBookCodes->Add((void*)_17BKCODE);
	pBookCodes->Add((void*)_18BKCODE);
	pBookCodes->Add((void*)_19BKCODE);
	pBookCodes->Add((void*)_20BKCODE);
	pBookCodes->Add((void*)_21BKCODE);
	pBookCodes->Add((void*)_22BKCODE);
	pBookCodes->Add((void*)_23BKCODE);
	pBookCodes->Add((void*)_24BKCODE);
	pBookCodes->Add((void*)_25BKCODE);
	pBookCodes->Add((void*)_26BKCODE);
	pBookCodes->Add((void*)_27BKCODE);
	pBookCodes->Add((void*)_28BKCODE);
	pBookCodes->Add((void*)_29BKCODE);
	pBookCodes->Add((void*)_30BKCODE);
	pBookCodes->Add((void*)_31BKCODE);
	pBookCodes->Add((void*)_32BKCODE);
	pBookCodes->Add((void*)_33BKCODE);
	pBookCodes->Add((void*)_34BKCODE);
	pBookCodes->Add((void*)_35BKCODE);
	pBookCodes->Add((void*)_36BKCODE);
	pBookCodes->Add((void*)_37BKCODE);
	pBookCodes->Add((void*)_38BKCODE);
	pBookCodes->Add((void*)_39BKCODE);
	pBookCodes->Add((void*)_40BKCODE);
	pBookCodes->Add((void*)_41BKCODE);
	pBookCodes->Add((void*)_42BKCODE);
	pBookCodes->Add((void*)_43BKCODE);
	pBookCodes->Add((void*)_44BKCODE);
	pBookCodes->Add((void*)_45BKCODE);
	pBookCodes->Add((void*)_46BKCODE);
	pBookCodes->Add((void*)_47BKCODE);
	pBookCodes->Add((void*)_48BKCODE);
	pBookCodes->Add((void*)_49BKCODE);
	pBookCodes->Add((void*)_50BKCODE);
	pBookCodes->Add((void*)_51BKCODE);
	pBookCodes->Add((void*)_52BKCODE);
	pBookCodes->Add((void*)_53BKCODE);
	pBookCodes->Add((void*)_54BKCODE);
	pBookCodes->Add((void*)_55BKCODE);
	pBookCodes->Add((void*)_56BKCODE);
	pBookCodes->Add((void*)_57BKCODE);
	pBookCodes->Add((void*)_58BKCODE);
	pBookCodes->Add((void*)_59BKCODE);
	pBookCodes->Add((void*)_60BKCODE);
	pBookCodes->Add((void*)_61BKCODE);
	pBookCodes->Add((void*)_62BKCODE);
	pBookCodes->Add((void*)_63BKCODE);
	pBookCodes->Add((void*)_64BKCODE);
	pBookCodes->Add((void*)_65BKCODE);
	pBookCodes->Add((void*)_66BKCODE);
	// Access using code like this:
	// Int16 index = some value;
	// or wxString aBookCode = ((*m_pBibleBooks)[index]).bookCode;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if the input string is "1", FALSE if the input string is "0".
/// \param      temp  -> input string representing either "1" or "0"
/// \remarks
/// Called from: the App's ParseAndFillStruct() which fills out the members of a USFMAnalysis
/// struct. GetBoolValueFromStr1orStr0() and ParseAndFillStruct() are only called if the app 
/// cannot find the AI_USFM.xml file and it must use SetupDefaultStylesMap() to establish its
/// internal list of USFM styles.
////////////////////////////////////////////////////////////////////////////////////////////
bool GetBoolValueFromStr1orStr0(wxString temp)
{
	// WX version helper for ParseAndFillStruct below
	if (temp == _T("1"))
		return TRUE;
	else if (temp == _T("0"))
		return FALSE;
	else
	{
		wxASSERT(FALSE);
		return TRUE; 
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pSFM        <- pointer to the USFMAnalysis struct whose members are to be assigned
/// \param      defaultSFM  -> The Unix-like default string representing the fields of a
///                             USFMAnalysis struct separated by colon ":" characters
/// \remarks
/// Called from: the App's SetupDefaultStylesMap() which fills out the members of a USFMAnalysis
/// struct. GetBoolValueFromStr1orStr0() and ParseAndFillStruct() are only called if the app 
/// cannot find the AI_USFM.xml file and it must use SetupDefaultStylesMap() to establish its
/// internal map of USFM styles.
////////////////////////////////////////////////////////////////////////////////////////////
void ParseAndFillStruct(USFMAnalysis*& pSFM, wxString defaultSFM)
{
	// parse the defaultSFM line and assign its fields to the appropriate
	// fields of the USFMAnalysis struct
	wxString temp = _T("");
	USFMAnalysisField fieldEnum = marker; //int fieldEnum = 0;
	int intValOfEnum = (int)marker;
	bool msgGiven = FALSE;
	for (int i = 0; i < (int)defaultSFM.Length(); i++)
	{
		if (defaultSFM[i] != _T(':'))
			temp += defaultSFM[i];
		else
		{
			// we're at the end of a field
			// empty non-string fields are assumed to represent a value of "0"
			if (temp == _T("") 
				&& !(fieldEnum == marker || fieldEnum == endMarker 
				     || fieldEnum == description || fieldEnum == navigationText
					 || fieldEnum == styleName || fieldEnum == basedOn 
					 || fieldEnum == nextStyle))
				temp = _T("0");
			// in the switch below:
			// strings are directly assigned the field contents
			// booleans are cast from their integral value to bool
			// enums are cast from their integral value to their enum type
			switch (fieldEnum)
			{
				case marker:           pSFM->marker = temp; break;
				case endMarker:        pSFM->endMarker = temp; break;
				case description:      pSFM->description = temp; break;
				case usfm:             pSFM->usfm = GetBoolValueFromStr1orStr0(temp); break;
				case png:              pSFM->png = GetBoolValueFromStr1orStr0(temp); break;
				case filter:           pSFM->filter = GetBoolValueFromStr1orStr0(temp); break;
				case userCanSetFilter: pSFM->userCanSetFilter = GetBoolValueFromStr1orStr0(temp); break;
				case inLine:           pSFM->inLine = GetBoolValueFromStr1orStr0(temp); break;
				case special:          pSFM->special = GetBoolValueFromStr1orStr0(temp); break;
				case bdryOnLast:       pSFM->bdryOnLast = GetBoolValueFromStr1orStr0(temp); break;
				case inform:           pSFM->inform = GetBoolValueFromStr1orStr0(temp); break;
				case navigationText:   pSFM->navigationText = temp; break;
				case textType:         pSFM->textType  = (TextType)wxAtoi(temp); break;
				case wrap:             pSFM->wrap = GetBoolValueFromStr1orStr0(temp); break;
				case styleName:        pSFM->styleName = temp; break; // addition for version 3
				case styleType:        pSFM->styleType = (StyleType)wxAtoi(temp); break;
				case fontSize:         pSFM->fontSize = wxAtoi(temp); break;
				case color:            pSFM->color = wxAtoi(temp); break;
				case italic:           pSFM->italic = GetBoolValueFromStr1orStr0(temp); break;
				case bold:             pSFM->bold = GetBoolValueFromStr1orStr0(temp); break;
				case underline:        pSFM->underline = GetBoolValueFromStr1orStr0(temp); break;
				case smallCaps:        pSFM->smallCaps = GetBoolValueFromStr1orStr0(temp); break;
				case superScript:      pSFM->superScript = GetBoolValueFromStr1orStr0(temp); break;
				case justification:    pSFM->justification = (Justification)wxAtoi(temp); break;
				case spaceAbove:       pSFM->spaceAbove = wxAtoi(temp); break;
				case spaceBelow:       pSFM->spaceBelow = wxAtoi(temp); break;
				case leadingMargin:    pSFM->leadingMargin = (float)wxAtof(temp); break;   // float
				case followingMargin:  pSFM->followingMargin = (float)wxAtof(temp); break; // float
				case firstLineIndent:  pSFM->firstLineIndent = (float)wxAtof(temp);break;  // float
				case basedOn:          pSFM->basedOn = temp; break;		// addition for version 3
				case nextStyle:        pSFM->nextStyle = temp; break;	// addition for version 3
				case keepTogether:     pSFM->keepTogether = GetBoolValueFromStr1orStr0(temp); break;	// addition for version 3
				case keepWithNext:     pSFM->keepWithNext = GetBoolValueFromStr1orStr0(temp); break;	// addition for version 3
				default: 
					{
						// This shouldn't happen. It would be an internal error due to 
						// incorrectly formatted default strings in defaultSFM[] above
						// If there is a serious problem this could be called 266 times,
						// so only give it once.
						if (!msgGiven)
						{
							wxMessageBox(_T("The defaultSFM[] array is incorrectly formatted!!\n"));
						}
						msgGiven = TRUE;
					}
			}
			intValOfEnum++;
			fieldEnum = (USFMAnalysisField)intValOfEnum;
			temp = _T("");
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: the App's OnInit(). It is only called if the app cannot find the AI_USFM.xml 
/// file and it must use SetupDefaultStylesMap() to establish its internal map of USFM styles.
/// When AI_USFM.xml is available (the usual case) the m_pUsfmStylesMap, m_pPngStylesMap, and 
/// m_pUsfmAndPngStylesMap are populated by the AtSFMEndTag() function in XML.cpp, rather than 
/// by this SetupDefaultStylesMap() function.
////////////////////////////////////////////////////////////////////////////////////////////
void SetupDefaultStylesMap()
{
	// Note: This function is only called when AI_USFM.xml is not available
	// for some reason. When AI_USFM.xml is available (the usual case) the
	// m_pUsfmStylesMap, m_pPngStylesMap, and m_pUsfmAndPngStylesMap are populated 
	// by the AtSFMEndTag() function in XML.cpp, rather than here.
	// Any changes to the filling of the CMapStringToOb routines below shold also
	// be carried over to the AtSFMEndTag() routine in XML.cpp
	// We don't want duplicate entries in any of the three maps, so we'll
	// always do a lookup first, and only add a marker if it doesn't already
	// exist in the given map.
	for (int i = 0; i < gnDefaultSFMs; i++)
	{
		gpUsfmAnalysis = new USFMAnalysis;
		// set the gpUsfmAnalysis struct attributes from the parsed defaultSFM 
		// strings values
		ParseAndFillStruct(gpUsfmAnalysis,defaultSFM[i]);
		wxString bareMkr = gpUsfmAnalysis->marker;
		int posn = bareMkr.Find(_T('*')); // do we have an end marker?
		if (posn >= 0 && bareMkr[bareMkr.Length() -1] == _T('*'))
		{
			bareMkr = bareMkr.Mid(0,posn-1); // strip off asterisk for attribute lookup
		}
		// map the parsed struct to the appropriate map on the heap
		MapSfmToUSFMAnalysisStruct::iterator iter;
		if (gpUsfmAnalysis->usfm)
		{
			iter = gpApp->m_pUsfmStylesMap->find(bareMkr);
			if (iter == gpApp->m_pUsfmStylesMap->end())
			{
				// key doesn't already exist in the map, so add it
				(*gpApp->m_pUsfmStylesMap)[bareMkr] = gpUsfmAnalysis;
			}
			// for UsfmAndPng map put the Usfm marker there as priority
			iter = gpApp->m_pUsfmAndPngStylesMap->find(bareMkr);
			if (iter == gpApp->m_pUsfmAndPngStylesMap->end())
			{
				// key doesn't already exist in the map, so add it
				(*gpApp->m_pUsfmAndPngStylesMap)[bareMkr] = gpUsfmAnalysis;
			}
		
		}
		if (gpUsfmAnalysis->png)
		{
			iter = gpApp->m_pPngStylesMap->find(bareMkr);
			if (iter == gpApp->m_pPngStylesMap->end())
			{
				// key doesn't already exist in the map, so add it
				(*gpApp->m_pPngStylesMap)[bareMkr] = gpUsfmAnalysis;
			}
			// for UsfmAndPng map put the Usfm marker there only if unique
			iter = gpApp->m_pUsfmAndPngStylesMap->find(bareMkr);
			if (iter == gpApp->m_pUsfmAndPngStylesMap->end())
			{
				// key doesn't already exist in the map, so add it
				(*gpApp->m_pUsfmAndPngStylesMap)[bareMkr] = gpUsfmAnalysis;
			}
		}
		if (!gpUsfmAnalysis->usfm && !gpUsfmAnalysis->png)
		{
			// the marker is neither usfm nor png and therefore not stored in
			// any of the three possible maps, so we must delete it
			delete gpUsfmAnalysis;
		}
		else
		{
			gpApp->m_pMappedObjectPointers->Add(gpUsfmAnalysis);
		}
		gpUsfmAnalysis = NULL;
	}
}

/*
enum wxLanguage
{
    // wxLanguage symbol		//	enum	Canonical Description in
    //						val	Name	    Plain English
    //					info->CanonicalName   info->Description
    // user's default/preffered language as got from OS:
    wxLANGUAGE_DEFAULT,			//	0	en_US	
    // unknown language, if wxLocale::GetSystemLanguage fails:
    wxLANGUAGE_UNKNOWN,			//	1	null	null	

    wxLANGUAGE_ABKHAZIAN,		//	2	ab	Abkhazian
    wxLANGUAGE_AFAR,			//	3	aa	Afar
    wxLANGUAGE_AFRIKAANS,		//	4	af_ZA	Afrikaans
    wxLANGUAGE_ALBANIAN,		//	5	sq_AL	Albanian
    wxLANGUAGE_AMHARIC,			//	6	am	Amharic
    wxLANGUAGE_ARABIC,			//	7	ar	Arabic
    wxLANGUAGE_ARABIC_ALGERIA,	//	8	ar_DZ	Arabic (Algeria)
    wxLANGUAGE_ARABIC_BAHRAIN,	//	9	ar_BH	Arabic (Bahrain)
    wxLANGUAGE_ARABIC_EGYPT,		//	10	ar_EG Arabic (Egypt)
    wxLANGUAGE_ARABIC_IRAQ,		//	11	ar_IQ	Arabic (Iraq)
    wxLANGUAGE_ARABIC_JORDAN,		//	12	ar_JO	Arabic (Jordan)
    wxLANGUAGE_ARABIC_KUWAIT,		//	13	ar_KW	Arabic (Kuwait)
    wxLANGUAGE_ARABIC_LEBANON,	//	14	ar_LB Arabic (Lebanon)
    wxLANGUAGE_ARABIC_LIBYA,		//	15	ar_LY	Arabic (Libya)
    wxLANGUAGE_ARABIC_MOROCCO,	//	16	ar_MA	Arabic (Morocco)
    wxLANGUAGE_ARABIC_OMAN,		//	17	ar_OM	Arabic (Oman)
    wxLANGUAGE_ARABIC_QATAR,		//	18	ar_QA	Arabic (Qatar)
    wxLANGUAGE_ARABIC_SAUDI_ARABIA,	//	19	ar_SA	Arabic (Saudi Arabia)
    wxLANGUAGE_ARABIC_SUDAN,		//	20	ar_SD	Arabic (Sudan)
    wxLANGUAGE_ARABIC_SYRIA,		//	21	ar_SY	Arabic (Syria)
    wxLANGUAGE_ARABIC_TUNISIA,	//	22	ar_TN	Arabic (Tunisia)
    wxLANGUAGE_ARABIC_UAE,		//	23	ar_AE	Arabic (Uae)
    wxLANGUAGE_ARABIC_YEMEN,		//	24	ar_YE	Arabic (Yemen)
    wxLANGUAGE_ARMENIAN,		//	25	hy	Armenian
    wxLANGUAGE_ASSAMESE,		//	26	as	Assamese
    wxLANGUAGE_AYMARA,			//	27	ay	Aymara
    wxLANGUAGE_AZERI,			//	28	az	Azeri
    wxLANGUAGE_AZERI_CYRILLIC,	//	29	az_Cyrl	Azeri (Cyrillic)
    wxLANGUAGE_AZERI_LATIN,		//	30	az_Latn	Azeri (Latin)	
    wxLANGUAGE_BASHKIR,			//	31	ba	Bashkir
    wxLANGUAGE_BASQUE,			//	32	eu_ES	Basque
    wxLANGUAGE_BELARUSIAN,		//	33	be_BY	Belarusian
    wxLANGUAGE_BENGALI,			//	34	bn	Bengali
    wxLANGUAGE_BHUTANI,			//	35	dz	Bhutani // not in IANA
    wxLANGUAGE_BIHARI,			//	36	bh	Bihari
    wxLANGUAGE_BISLAMA,			//	37	bi	Bislama
    wxLANGUAGE_BRETON,			//	38	br	Breton
    wxLANGUAGE_BULGARIAN,		//	39	bg_BG	Bulgarian
    wxLANGUAGE_BURMESE,			//	40	my	Burmese
    wxLANGUAGE_CAMBODIAN,		//	41	km	Cambodian // not in IANA
    wxLANGUAGE_CATALAN,			//	42	ca_ES	Catalan
    wxLANGUAGE_CHINESE,			//	43	zh_TW	Chinese
    wxLANGUAGE_CHINESE_SIMPLIFIED,	//	44	zh_CN	Chinese (Simplified)
    wxLANGUAGE_CHINESE_TRADITIONAL,	//	45	zh_TW	Chinese (Traditional)
    wxLANGUAGE_CHINESE_HONGKONG,	//	46	zh_HK	Chinese (Hongkong)
    wxLANGUAGE_CHINESE_MACAU,		//	47	zh_MO	Chinese (Macau)
    wxLANGUAGE_CHINESE_SINGAPORE,	//	48	zh_SG	Chinese (Singapore)
    wxLANGUAGE_CHINESE_TAIWAN,	//	49	zh_TW	Chinese (Taiwan)
    wxLANGUAGE_CORSICAN,		//	50	co	Corsican
    wxLANGUAGE_CROATIAN,		//	51	hr_HR	Croatian
    wxLANGUAGE_CZECH,			//	52	cs_CZ	Czech
    wxLANGUAGE_DANISH,			//	53	da_DK	Danish
    wxLANGUAGE_DUTCH,			//	54	nl_NL	Dutch
    wxLANGUAGE_DUTCH_BELGIAN,		//	55	nl_BE	Dutch (Belgian)
    wxLANGUAGE_ENGLISH,			//	56	en_GB	English
    wxLANGUAGE_ENGLISH_UK,		//	57	en_GB	English (U.K.)
    wxLANGUAGE_ENGLISH_US,		//	58	en_US	English (U.S.)
    wxLANGUAGE_ENGLISH_AUSTRALIA,	//	59	en_AU	English (Australia)
    wxLANGUAGE_ENGLISH_BELIZE,	//	60	en_BZ	English (Belize)
    wxLANGUAGE_ENGLISH_BOTSWANA,	//	61	en_BW	English (Botswana)
    wxLANGUAGE_ENGLISH_CANADA,	//	62	en_CA	English (Canada)
    wxLANGUAGE_ENGLISH_CARIBBEAN,	//	63	en_CB	English (Caribbean)
    wxLANGUAGE_ENGLISH_DENMARK,	//	64	en_DK	English (Denmark)
    wxLANGUAGE_ENGLISH_EIRE,		//	65	en_IE	English (Eire)
    wxLANGUAGE_ENGLISH_JAMAICA,	//	66	en_JM	English (Jamaica)
    wxLANGUAGE_ENGLISH_NEW_ZEALAND,	//	67	en_NZ	English (New Zealand)
    wxLANGUAGE_ENGLISH_PHILIPPINES,	//	68	en_PH	English (Philippines)
    wxLANGUAGE_ENGLISH_SOUTH_AFRICA,	//	69	en_ZA	English (South Africa)
    wxLANGUAGE_ENGLISH_TRINIDAD,	//	70	en_TT	English (Trinidad)
    wxLANGUAGE_ENGLISH_ZIMBABWE,	//	71	en_ZW	English (Zimbabwe)
    wxLANGUAGE_ESPERANTO,		//	72	eo	Esperanto
    wxLANGUAGE_ESTONIAN,		//	73	et_EE	Estonian
    wxLANGUAGE_FAEROESE,		//	74	fo_FO	Faeroese
    wxLANGUAGE_FARSI,			//	75	fa_IR	Farsi
    wxLANGUAGE_FIJI,			//	76	fj	Fiji
    wxLANGUAGE_FINNISH,			//	77	fi_FI	Finnish
    wxLANGUAGE_FRENCH,			//	78	fr_FR	French
    wxLANGUAGE_FRENCH_BELGIAN,	//	79	fr_BE	French (Belgian)
    wxLANGUAGE_FRENCH_CANADIAN,	//	80	fr_CA	French (Canadian)
    wxLANGUAGE_FRENCH_LUXEMBOURG,	//	81	fr_LU	French (Luxembourg)
    wxLANGUAGE_FRENCH_MONACO,		//	82	fr_MC	French (Monaco)
    wxLANGUAGE_FRENCH_SWISS,		//	83	fr_CH	French (Swiss)
    wxLANGUAGE_FRISIAN,			//	84	fy	Frisian
    wxLANGUAGE_GALICIAN,		//	85	gl_ES	Galician
    wxLANGUAGE_GEORGIAN,		//	86	ka	Georgian
    wxLANGUAGE_GERMAN,			//	87	de_DE	German
    wxLANGUAGE_GERMAN_AUSTRIAN,	//	88	de_AT	German (Austrian)
    wxLANGUAGE_GERMAN_BELGIUM,	//	89	de_BE	German (Belgium)
    wxLANGUAGE_GERMAN_LIECHTENSTEIN,	//	90	de_LI	German (Liechtenstein)
    wxLANGUAGE_GERMAN_LUXEMBOURG,	//	91	de_LU	German (Luxembourg)
    wxLANGUAGE_GERMAN_SWISS,		//	92	de_CH	German (Swiss)
    wxLANGUAGE_GREEK,			//	93	el_GR	Greek
    wxLANGUAGE_GREENLANDIC,		//	94	kl_GL	Greenlandic
    wxLANGUAGE_GUARANI,			//	95	gn	Guarani
    wxLANGUAGE_GUJARATI,		//	96	gu	Gujarati
    wxLANGUAGE_HAUSA,			//	97	ha	Hausa
    wxLANGUAGE_HEBREW,			//	98	he_IL	Hebrew
    wxLANGUAGE_HINDI,			//	99	hi_IN	Hindi
    wxLANGUAGE_HUNGARIAN,		//	100	hu_HU	Hungarian
    wxLANGUAGE_ICELANDIC,		//	101	is_IS	Icelandic
    wxLANGUAGE_INDONESIAN,		//	102	id_ID	Indonesian
    wxLANGUAGE_INTERLINGUA,		//	103	ia	Interlingua
    wxLANGUAGE_INTERLINGUE,		//	104	ie	Interlingue
    wxLANGUAGE_INUKTITUT,		//	105	iu	Inuktitut
    wxLANGUAGE_INUPIAK,			//	106	ik	Inupiak
    wxLANGUAGE_IRISH,			//	107	ga_IE	Irish
    wxLANGUAGE_ITALIAN,			//	108	it_IT	Italian
    wxLANGUAGE_ITALIAN_SWISS,		//	109	it_CH	Italian (Swiss)
    wxLANGUAGE_JAPANESE,		//	110	ja_JP	Japanese
    wxLANGUAGE_JAVANESE,		//	111	jw	Javanese
    wxLANGUAGE_KANNADA,			//	112	kn	kannada
    wxLANGUAGE_KASHMIRI,		//	113	ks	Kashmiri
    wxLANGUAGE_KASHMIRI_INDIA,	//	114	ks_IN	Kashmiri (India)
    wxLANGUAGE_KAZAKH,			//	115	kk	Kazakh
    wxLANGUAGE_KERNEWEK,		//	116	kw_GB	Kernewek
    wxLANGUAGE_KINYARWANDA,		//	117	rw	Kinyarwanda
    wxLANGUAGE_KIRGHIZ,			//	118	ky	Kirghiz
    wxLANGUAGE_KIRUNDI,			//	119	rn	Kirundi
    wxLANGUAGE_KONKANI,			//	120	kok	Konkani // canonical is null kok is IANA code
    wxLANGUAGE_KOREAN,			//	121	ko_KR	Korean
    wxLANGUAGE_KURDISH,			//	122	ku	Kurdish
    wxLANGUAGE_LAOTHIAN,		//	123	lo	Laothian
    wxLANGUAGE_LATIN,			//	124	la	Latin
    wxLANGUAGE_LATVIAN,			//	125	lv_LV	Latvian
    wxLANGUAGE_LINGALA,			//	126	ln	Lingala
    wxLANGUAGE_LITHUANIAN,		//	127	lt_LT	Lithuanian
    wxLANGUAGE_MACEDONIAN,		//	128	mk_MK	Macedonian
    wxLANGUAGE_MALAGASY,		//	129	mg	Malagasy
    wxLANGUAGE_MALAY,			//	130	ms_MY	Malay
    wxLANGUAGE_MALAYALAM,		//	131	ml	Malayalam
    wxLANGUAGE_MALAY_BRUNEI_DARUSSALAM,	//	132	ms_BN	Malay (Brunei Darussalam)
    wxLANGUAGE_MALAY_MALAYSIA,	//	133	ms_MY	Malay (Malaysia)
    wxLANGUAGE_MALTESE,			//	134	mt_MT	Maltese
    wxLANGUAGE_MANIPURI,		//	135	mni	Manipuri // canonical is null mni is IANA code
    wxLANGUAGE_MAORI,			//	136	mi	Maori
    wxLANGUAGE_MARATHI,			//	137	mr_IN	Marathi
    wxLANGUAGE_MOLDAVIAN,		//	138	mo	Moldavian
    wxLANGUAGE_MONGOLIAN,		//	139	mn	Mongolian
    wxLANGUAGE_NAURU,			//	140	na	Nauru
    wxLANGUAGE_NEPALI,			//	141	ne	Nepali
    wxLANGUAGE_NEPALI_INDIA,		//	142	ne_IN	Nepali (India)
    wxLANGUAGE_NORWEGIAN_BOKMAL,	//	143	nb_NO	Norwegian (Bokmal)
    wxLANGUAGE_NORWEGIAN_NYNORSK,	//	144	nn_NO	Norwegian (Nynorsk)
    wxLANGUAGE_OCCITAN,			//	145	oc	Occitan
    wxLANGUAGE_ORIYA,			//	146	or	Oriya
    wxLANGUAGE_OROMO,			//	147	om	(Afan) Oromo
    wxLANGUAGE_PASHTO,			//	148	ps	Pashto, Pushto
    wxLANGUAGE_POLISH,			//	149	pl_PL	Polish
    wxLANGUAGE_PORTUGUESE,		//	150	pt_PT	Portuguese
    wxLANGUAGE_PORTUGUESE_BRAZILIAN,	//	151	pt_BR	Portuguese (Brazilian)
    wxLANGUAGE_PUNJABI,			//	152	pa	Punjabi
    wxLANGUAGE_QUECHUA,			//	153	qu	Quechua
    wxLANGUAGE_RHAETO_ROMANCE,	//	154	rm	Rhaeto-Romance
    wxLANGUAGE_ROMANIAN,		//	155	ro_RO	Romanian
    wxLANGUAGE_RUSSIAN,			//	156	ru_RU	Russian
    wxLANGUAGE_RUSSIAN_UKRAINE,	//	157	ru_UA	Russian (Ukraine)
    wxLANGUAGE_SAMOAN,			//	158	sm	Samoan
    wxLANGUAGE_SANGHO,			//	159	sg	Sangho
    wxLANGUAGE_SANSKRIT,		//	160	sa	Sanskrit
    wxLANGUAGE_SCOTS_GAELIC,		//	161	gd	Scots Gaelic
    wxLANGUAGE_SERBIAN,			//	162	sr	Serbian // both null in wx; values from IANA
    wxLANGUAGE_SERBIAN_CYRILLIC,	//	163	sr_YU	Serbian (Cyrillic)
    wxLANGUAGE_SERBIAN_LATIN,		//	164	sr_YU	Serbian (Latin)
    wxLANGUAGE_SERBO_CROATIAN,	//	165	sh	Serbo-Croatian
    wxLANGUAGE_SESOTHO,			//	166	st	Sesotho
    wxLANGUAGE_SETSWANA,		//	167	tn	Setswana
    wxLANGUAGE_SHONA,			//	168	sn	Shona
    wxLANGUAGE_SINDHI,			//	169	sd	Sindhi
    wxLANGUAGE_SINHALESE,		//	170	si	Sinhalese
    wxLANGUAGE_SISWATI,			//	171	ss	Siswati
    wxLANGUAGE_SLOVAK,			//	172	sk_SK	Slovak
    wxLANGUAGE_SLOVENIAN,		//	173	sl_SI	Slovenian
    wxLANGUAGE_SOMALI,			//	174	so	Somali
    wxLANGUAGE_SPANISH,			//	175	es_ES	Spanish
    wxLANGUAGE_SPANISH_ARGENTINA,	//	176	es_AR	Spanish (Argentina)
    wxLANGUAGE_SPANISH_BOLIVIA,	//	177	es_BO	Spanish (Bolivia)
    wxLANGUAGE_SPANISH_CHILE,		//	178	es_CL	Spanish (Chile)
    wxLANGUAGE_SPANISH_COLOMBIA,	//	179	es_CO	Spanish (Colombia)
    wxLANGUAGE_SPANISH_COSTA_RICA,	//	180	es_CR	Spanish (Costa Rica)
    wxLANGUAGE_SPANISH_DOMINICAN_REPUBLIC,	//	181	es_DO	Spanish (Dominican republic)
    wxLANGUAGE_SPANISH_ECUADOR,	//	182	es_EC	Spanish (Ecuador)
    wxLANGUAGE_SPANISH_EL_SALVADOR,	//	183	es_SV	Spanish (El Salvador)
    wxLANGUAGE_SPANISH_GUATEMALA,	//	184	es_GT	Spanish (Guatemala)
    wxLANGUAGE_SPANISH_HONDURAS,	//	185	es_HN	Spanish (Honduras)
    wxLANGUAGE_SPANISH_MEXICAN,	//	186	es_MX	Spanish (Mexico)
    wxLANGUAGE_SPANISH_MODERN,	//	187	es_ES	Spanish (Modern)
    wxLANGUAGE_SPANISH_NICARAGUA,	//	188	es_NI	Spanish (Nicaragua)
    wxLANGUAGE_SPANISH_PANAMA,	//	189	es_PA	Spanish (Panama)
    wxLANGUAGE_SPANISH_PARAGUAY,	//	190	es_PY	Spanish (Paraguay)
    wxLANGUAGE_SPANISH_PERU,		//	191	es_PE	Spanish (Peru)
    wxLANGUAGE_SPANISH_PUERTO_RICO,	//	192	es_PR	Spanish (Puerto Rico)
    wxLANGUAGE_SPANISH_URUGUAY,	//	193	es_UY	Spanish (Uruguay)
    wxLANGUAGE_SPANISH_US,		//	194	es_US	Spanish (U.S.)
    wxLANGUAGE_SPANISH_VENEZUELA,	//	195	es_VE	Spanish (Venezuela)
    wxLANGUAGE_SUNDANESE,		//	196	su	Sundanese
    wxLANGUAGE_SWAHILI,			//	197	sw_KE	Swahili
    wxLANGUAGE_SWEDISH,			//	198	sv_SE	Swedish
    wxLANGUAGE_SWEDISH_FINLAND,	//	199	sv_FI	Swedish (Finland)
    wxLANGUAGE_TAGALOG,			//	200	tl_PH	Tagalog
    wxLANGUAGE_TAJIK,			//	201	tg	Tajik
    wxLANGUAGE_TAMIL,			//	202	ta	Tamil
    wxLANGUAGE_TATAR,			//	203	tt	Tatar
    wxLANGUAGE_TELUGU,			//	204	te	Telugu
    wxLANGUAGE_THAI,			//	205	th_TH	Thai
    wxLANGUAGE_TIBETAN,			//	206	bo	Tibetan
    wxLANGUAGE_TIGRINYA,		//	207	ti	Tigrinya
    wxLANGUAGE_TONGA,			//	208	to	Tonga
    wxLANGUAGE_TSONGA,			//	209	ts	Tsonga
    wxLANGUAGE_TURKISH,			//	210	tr_TR	Turkish
    wxLANGUAGE_TURKMEN,			//	211	tk	Turkmen
    wxLANGUAGE_TWI,				//	212	tw	Twi
    wxLANGUAGE_UIGHUR,			//	213	ug	Uighur
    wxLANGUAGE_UKRAINIAN,		//	214	uk_UA	Ukrainian
    wxLANGUAGE_URDU,			//	215	ur	Urdu
    wxLANGUAGE_URDU_INDIA,		//	216	ur_IN	Urdu (India)
    wxLANGUAGE_URDU_PAKISTAN,	//	217	ur_PK	Urdu (Pakistan)
    wxLANGUAGE_UZBEK,			//	218	uz	Uzbek
    wxLANGUAGE_UZBEK_CYRILLIC,	//	219	uz	Uzbek (Cyrillic)
    wxLANGUAGE_UZBEK_LATIN,		//	220	uz	Uzbek (Latin)
    wxLANGUAGE_VIETNAMESE,		//	221	vi_VN	Vietnamese
    wxLANGUAGE_VOLAPUK,			//	222	vo	Volapuk
    wxLANGUAGE_WELSH,			//	223	cy	Welsh
    wxLANGUAGE_WOLOF,			//	224	wo	Wolof
    wxLANGUAGE_XHOSA,			//	225	xh	Xhosa
    wxLANGUAGE_YIDDISH,			//	226	yi	Yiddish
    wxLANGUAGE_YORUBA,			//	227	yo	Yoruba
    wxLANGUAGE_ZHUANG,			//	228	za	Zhuang
    wxLANGUAGE_ZULU,			//	229	zu	Zulu

    // for custom, user-defined languages:
    wxLANGUAGE_USER_DEFINED		//	230	null	null
};
*/

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: The App's ChooseInterfaceLanguage(), and CLanguagesPage::InitDialog().
/// An array consisting of triplets of full language names, the wxWidgets language enum 
/// symbols, and the short canonical name (used for subdirectories). Used to correlate
/// full language names, enum symbols and canonical names. Note: This language data is taken 
/// from the void wxLocale::InitLanguagesDB() function in the wxWidgets intl.cpp library 
/// source file.
////////////////////////////////////////////////////////////////////////////////////////////
LangInfo langsKnownToWX[] =
{
    { _("(Use system default language) [%s]"), wxLANGUAGE_DEFAULT, _T("default") },	// 0

	// Note: I've added a couple three-letter short canonical forms from the IANA language registry
	// which may be found at: http://www.iana.org/assignments/language-subtag-registry

    { _T("Abkhazian"), wxLANGUAGE_ABKHAZIAN, _T("ab") },							// 2
	{ _T("Afar"), wxLANGUAGE_AFAR, _T("aa") },										// 3
	{ _T("Afrikaans"), wxLANGUAGE_AFRIKAANS, _T("af_ZA") },							// 4
    { _T("Albanian"), wxLANGUAGE_ALBANIAN, _T("sq_AL") },							// 5
    { _T("Amharic"), wxLANGUAGE_AMHARIC, _T("am") },								// 6
    { _T("Arabic"), wxLANGUAGE_ARABIC, _T("ar") },									// 7
    { _T("Arabic (Algeria)"), wxLANGUAGE_ARABIC_ALGERIA, _T("ar_DZ") },				// 8
    { _T("Arabic (Bahrain)"), wxLANGUAGE_ARABIC_BAHRAIN, _T("ar_BH") },				// 9
    { _T("Arabic (Egypt)"), wxLANGUAGE_ARABIC_EGYPT, _T("ar_EG") },					// 10
    { _T("Arabic (Iraq)"), wxLANGUAGE_ARABIC_IRAQ, _T("ar_IQ") },					// 11
    { _T("Arabic (Jordan)"), wxLANGUAGE_ARABIC_JORDAN, _T("ar_JO") },				// 12
    { _T("Arabic (Kuwait)"), wxLANGUAGE_ARABIC_KUWAIT, _T("ar_KW") },				// 13
    { _T("Arabic (Lebanon)"), wxLANGUAGE_ARABIC_LEBANON, _T("ar_LB") },				// 14
    { _T("Arabic (Libya)"), wxLANGUAGE_ARABIC_LIBYA, _T("ar_LY") },					// 15
    { _T("Arabic (Morocco)"), wxLANGUAGE_ARABIC_MOROCCO, _T("ar_MA") },				// 16
    { _T("Arabic (Oman)"), wxLANGUAGE_ARABIC_OMAN, _T("ar_OM") },					// 17
    { _T("Arabic (Qatar)"), wxLANGUAGE_ARABIC_QATAR, _T("ar_QA") },					// 18
    { _T("Arabic (Saudi Arabia)"), wxLANGUAGE_ARABIC_SAUDI_ARABIA, _T("ar_SA") },	// 19
    { _T("Arabic (Sudan)"), wxLANGUAGE_ARABIC_SUDAN, _T("ar_SD") },					// 20
    { _T("Arabic (Syria)"), wxLANGUAGE_ARABIC_SYRIA, _T("ar_SY") },					// 21
    { _T("Arabic (Tunisia)"), wxLANGUAGE_ARABIC_TUNISIA, _T("ar_TN") },				// 22
    { _T("Arabic (Uae)"), wxLANGUAGE_ARABIC_UAE, _T("ar_AE") },						// 23
    { _T("Arabic (Yemen)"), wxLANGUAGE_ARABIC_YEMEN, _T("ar_YE") },					// 24
	{ _T("Armenian"), wxLANGUAGE_ARMENIAN, _T("hy") },								// 25
	{ _T("Assamese"), wxLANGUAGE_ASSAMESE, _T("as") },								// 26
	{ _T("Aymara"), wxLANGUAGE_AYMARA, _T("ay") },									// 27
	{ _T("Azeri"), wxLANGUAGE_AZERI, _T("az") },									// 28
	{ _T("Azeri (Cyrillic)"), wxLANGUAGE_AZERI_CYRILLIC, _T("az_Cyrl") },			// 29
	{ _T("Azeri (Latin)"), wxLANGUAGE_AZERI_LATIN, _T("az_Latn") },					// 30
	{ _T("Bashkir"), wxLANGUAGE_BASHKIR, _T("ba") },								// 31
    { _T("Basque"), wxLANGUAGE_BASQUE, _T("eu_ES") },								// 32
    { _T("Belarusian"), wxLANGUAGE_BELARUSIAN, _T("be_BY") },						// 33
    { _T("Bengali"), wxLANGUAGE_BENGALI, _T("bn") },								// 34
	{ _T("Bhutani"), wxLANGUAGE_BHUTANI, _T("dz") },								// 35 // in IANA registry under "Dzongkha" (uses Tibt script)
	{ _T("Bihari"), wxLANGUAGE_BIHARI, _T("bh") },									// 36
	{ _T("Bislama"), wxLANGUAGE_BISLAMA, _T("bi") },								// 37
    { _T("Breton"), wxLANGUAGE_BRETON, _T("br") },									// 38
    { _T("Bulgarian"), wxLANGUAGE_BULGARIAN, _T("bg_BG") },							// 39
	{ _T("Burmese"), wxLANGUAGE_BURMESE, _T("my") },								// 40
	{ _T("Cambodian"), wxLANGUAGE_CAMBODIAN, _T("km") },							// 41 // in IANA registry under "Central Khmer" (uses Khmr script)
    { _T("Catalan"), wxLANGUAGE_CATALAN, _T("ca_ES") },								// 42
    { _T("Chinese"), wxLANGUAGE_CHINESE, _T("zh_TW") },								// 43
    { _T("Chinese (Simplified)"), wxLANGUAGE_CHINESE_SIMPLIFIED, _T("zh_CN") },		// 44
    { _T("Chinese (Traditional)"), wxLANGUAGE_CHINESE_TRADITIONAL, _T("zh_TW") },	// 45
    { _T("Chinese (Hongkong)"), wxLANGUAGE_CHINESE_HONGKONG, _T("zh_HK") },			// 46
    { _T("Chinese (Macau)"), wxLANGUAGE_CHINESE_MACAU, _T("zh_MO") },				// 47
    { _T("Chinese (Singapore)"), wxLANGUAGE_CHINESE_SINGAPORE, _T("zh_SG") },		// 48
    { _T("Chinese (Taiwan)"), wxLANGUAGE_CHINESE_TAIWAN, _T("zh_TW") },				// 49
	{ _T("Corsican"), wxLANGUAGE_CORSICAN, _T("co") },								// 50
    { _T("Croatian"), wxLANGUAGE_CROATIAN, _T("hr_HR") },							// 51
    { _T("Czech"), wxLANGUAGE_CZECH, _T("cs_CZ") },									// 52
    { _T("Danish"), wxLANGUAGE_DANISH, _T("da_DK") },								// 53
    { _T("Dutch"), wxLANGUAGE_DUTCH, _T("nl_NL") },									// 54
	{ _T("Dutch (Belgian)"), wxLANGUAGE_DUTCH_BELGIAN, _T("nl_BE") },				// 55
    { _T("English"), wxLANGUAGE_ENGLISH, _T("en_GB") },								// 56
    { _T("English (U.K.)"), wxLANGUAGE_ENGLISH_UK, _T("en_GB") },					// 57
    { _T("English (U.S.)"), wxLANGUAGE_ENGLISH_US, _T("en_US") },					// 58
    { _T("English (Australia)"), wxLANGUAGE_ENGLISH_AUSTRALIA, _T("en_AU") },		// 59
    { _T("English (Belize)"), wxLANGUAGE_ENGLISH_BELIZE, _T("en_BZ") },				// 60
    { _T("English (Botswana)"), wxLANGUAGE_ENGLISH_BOTSWANA, _T("en_BW") },			// 61
    { _T("English (Canada)"), wxLANGUAGE_ENGLISH_CANADA, _T("en_CA") },				// 62
    { _T("English (Caribbean)"), wxLANGUAGE_ENGLISH_CARIBBEAN, _T("en_CB") },		// 63
    { _T("English (Denmark)"), wxLANGUAGE_ENGLISH_DENMARK, _T("en_DK") },			// 64
    { _T("English (Eire)"), wxLANGUAGE_ENGLISH_EIRE, _T("en_IE") },					// 65
    { _T("English (Jamaica)"), wxLANGUAGE_ENGLISH_JAMAICA, _T("en_JM") },			// 66
    { _T("English (New Zealand)"), wxLANGUAGE_ENGLISH_NEW_ZEALAND, _T("en_NZ") },	// 67
    { _T("English (Philippines)"), wxLANGUAGE_ENGLISH_PHILIPPINES, _T("en_PH") },	// 68
    { _T("English (South Africa)"), wxLANGUAGE_ENGLISH_SOUTH_AFRICA, _T("en_ZA") },	// 69
    { _T("English (Trinidad)"), wxLANGUAGE_ENGLISH_TRINIDAD, _T("en_TT") },			// 70
    { _T("English (Zimbabwe)"), wxLANGUAGE_ENGLISH_ZIMBABWE, _T("en_ZW") },			// 71
    { _T("Esperanto"), wxLANGUAGE_ESPERANTO, _T("eo") },							// 72
    { _T("Estonian"), wxLANGUAGE_ESTONIAN, _T("et_EE") },							// 73
	{ _T("Faeroese"), wxLANGUAGE_FAEROESE, _T("fo_FO") },							// 74
    { _T("Farsi"), wxLANGUAGE_FARSI, _T("fa_IR") },									// 75
	{ _T("Fiji"), wxLANGUAGE_FIJI, _T("fj") },										// 76
    { _T("Finnish"), wxLANGUAGE_FINNISH, _T("fi_FI") },								// 77
    { _T("French"), wxLANGUAGE_FRENCH, _T("fr_FR") },								// 78
    { _T("French (Belgian)"), wxLANGUAGE_FRENCH_BELGIAN, _T("fr_BE") },				// 79
    { _T("French (Canadian)"), wxLANGUAGE_FRENCH_CANADIAN, _T("fr_CA") },			// 80
    { _T("French (Luxembourg)"), wxLANGUAGE_FRENCH_LUXEMBOURG, _T("fr_LU") },		// 81
    { _T("French (Monaco)"), wxLANGUAGE_FRENCH_MONACO, _T("fr_MC") },				// 82
    { _T("French (Swiss)"), wxLANGUAGE_FRENCH_SWISS, _T("fr_CH") },					// 83
	{ _T("Frisian"), wxLANGUAGE_FRISIAN, _T("fy") },								// 84
    { _T("Galician"), wxLANGUAGE_GALICIAN, _T("gl_ES") },							// 85
    { _T("Georgian"), wxLANGUAGE_GEORGIAN, _T("ka") },								// 86
    { _T("German"), wxLANGUAGE_GERMAN, _T("de_DE") },								// 87
    { _T("German (Austrian)"), wxLANGUAGE_GERMAN_AUSTRIAN, _T("de_AT") },			// 88
    { _T("German (Belgium)"), wxLANGUAGE_GERMAN_BELGIUM, _T("de_BE") },				// 89
    { _T("German (Liechtenstein)"), wxLANGUAGE_GERMAN_LIECHTENSTEIN, _T("de_LI") },	// 90
    { _T("German (Luxembourg)"), wxLANGUAGE_GERMAN_LUXEMBOURG, _T("de_LU") },		// 91
    { _T("German (Swiss)"), wxLANGUAGE_GERMAN_SWISS, _T("de_CH") },					// 92
    { _T("Greek"), wxLANGUAGE_GREEK, _T("el_GR") },									// 93
	{ _T("Greenlandic"), wxLANGUAGE_GREENLANDIC, _T("kl_GL") },						// 94
	{ _T("Guarani"), wxLANGUAGE_GUARANI, _T("gn") },								// 95
	{ _T("Gujarati"), wxLANGUAGE_GUJARATI, _T("gu") },								// 96
	{ _T("Hausa"), wxLANGUAGE_HAUSA, _T("ha") },									// 97
    { _T("Hebrew"), wxLANGUAGE_HEBREW, _T("he_IL") },								// 98
    { _T("Hindi"), wxLANGUAGE_HINDI, _T("hi_IN") },									// 99
    { _T("Hungarian"), wxLANGUAGE_HUNGARIAN, _T("hu_HU") },							// 100
    { _T("Icelandic"), wxLANGUAGE_ICELANDIC, _T("is_IS") },							// 101
    { _T("Indonesian"), wxLANGUAGE_INDONESIAN, _T("id_ID") },						// 102
	{ _T("Interlingua"), wxLANGUAGE_INTERLINGUA, _T("ia") },						// 103
	{ _T("Interlingue"), wxLANGUAGE_INTERLINGUE, _T("ie") },						// 104
	{ _T("Inuktitut"), wxLANGUAGE_INUKTITUT, _T("iu") },							// 105
	{ _T("Inupiak"), wxLANGUAGE_INUPIAK, _T("ik") },								// 106
	{ _T("Irish"), wxLANGUAGE_IRISH, _T("ga_IE") },									// 107
    { _T("Italian"), wxLANGUAGE_ITALIAN, _T("it_IT") },								// 108
    { _T("Italian (Swiss)"), wxLANGUAGE_ITALIAN_SWISS, _T("it_CH") },				// 109
    { _T("Japanese"), wxLANGUAGE_JAPANESE, _T("ja_JP") },							// 110
	{ _T("Javanese"), wxLANGUAGE_JAVANESE, _T("jw") },								// 111
	{ _T("kannada"), wxLANGUAGE_KANNADA, _T("kn") },								// 112
	{ _T("Kashmiri"), wxLANGUAGE_KASHMIRI, _T("ks") },								// 113
	{ _T("Kashmiri (India)"), wxLANGUAGE_KASHMIRI_INDIA, _T("ks_IN") },				// 114
	{ _T("Kazakh"), wxLANGUAGE_KAZAKH, _T("kk") },									// 115
	{ _T("Kernewek"), wxLANGUAGE_KERNEWEK, _T("kw_GB") },							// 116
	{ _T("Kinyarwanda"), wxLANGUAGE_KINYARWANDA, _T("rw") },						// 117
	{ _T("Kirghiz"), wxLANGUAGE_KIRGHIZ, _T("ky") },								// 118
	{ _T("Kirundi"), wxLANGUAGE_KIRUNDI, _T("rn") },								// 119
	{ _T("Konkani"), wxLANGUAGE_KONKANI, _T("kok") },								// 120 // wx's canonicalName is null; IANA registry uses 3-letter code kok
    { _T("Korean"), wxLANGUAGE_KOREAN, _T("ko_KR") },								// 121
    { _T("Kurdish"), wxLANGUAGE_KURDISH, _T("ku") },								// 122
    { _T("Kyrgyz"), wxLANGUAGE_KIRGHIZ, _T("ky") },									// 118 // same as Kirghiz (wxLANGUAGE_KIRGHIZ)
	{ _T("Laothian"), wxLANGUAGE_LAOTHIAN, _T("lo") },								// 123
	{ _T("Latin"), wxLANGUAGE_LATIN, _T("la") },									// 124
    { _T("Latvian"), wxLANGUAGE_LATVIAN, _T("lv_LV") },								// 125
	{ _T("Lingala"), wxLANGUAGE_LINGALA, _T("ln") },								// 126
    { _T("Lithuanian"), wxLANGUAGE_LITHUANIAN, _T("lt_LT") },						// 127
    { _T("Macedonian"), wxLANGUAGE_MACEDONIAN, _T("mk_MK") },						// 128
	{ _T("Malagasy"), wxLANGUAGE_MALAGASY, _T("mg") },								// 129
    { _T("Malay"), wxLANGUAGE_MALAY, _T("ms_MY") },									// 130
	{ _T("Malayalam"), wxLANGUAGE_MALAYALAM, _T("ml") },							// 131
	{ _T("Malay (Brunei Darussalam)"), wxLANGUAGE_MALAY_BRUNEI_DARUSSALAM, _T("ms_BN") },	// 132
	{ _T("Malay (Malaysia)"), wxLANGUAGE_MALAY_MALAYSIA, _T("ms_MY") },				// 133
	{ _T("Maltese"), wxLANGUAGE_MALTESE, _T("mt_MT") },								// 134
	{ _T("Manipuri"), wxLANGUAGE_MANIPURI, _T("mni") },								// 135 // wx's canonicalName is null; IANA registry uses 3-letter code mni
	{ _T("Maori"), wxLANGUAGE_MAORI, _T("mi") },									// 136
	{ _T("Marathi"), wxLANGUAGE_MARATHI, _T("mr_IN") },								// 137
	{ _T("Moldavian"), wxLANGUAGE_MOLDAVIAN, _T("mo") },							// 138
    { _T("Mongolian"), wxLANGUAGE_MONGOLIAN, _T("mn") },							// 139
	{ _T("Nauru"), wxLANGUAGE_NAURU, _T("na") },									// 140
	{ _T("Nepali"), wxLANGUAGE_NEPALI, _T("ne") },									// 141
	{ _T("Nepali (India)"), wxLANGUAGE_NEPALI_INDIA, _T("ne_IN") },					// 142
    { _T("Norwegian Bokmal"), wxLANGUAGE_NORWEGIAN_BOKMAL, _T("nb_NO") },			// 143
    { _T("Norwegian Nynorsk"), wxLANGUAGE_NORWEGIAN_NYNORSK, _T("nn_NO") },			// 144
	{ _T("Occitan"), wxLANGUAGE_OCCITAN, _T("oc") },								// 145
	{ _T("Oriya"), wxLANGUAGE_ORIYA, _T("or") },									// 146
	{ _T("(Afan) Oromo"), wxLANGUAGE_OROMO, _T("om") },								// 147
	{ _T("Pashto, Pushto"), wxLANGUAGE_PASHTO, _T("ps") },							// 148
    { _T("Polish"), wxLANGUAGE_POLISH, _T("pl_PL") },								// 149
    { _T("Portuguese"), wxLANGUAGE_PORTUGUESE, _T("pt_PT") },						// 150
    { _T("Portuguese (Brazilian)"), wxLANGUAGE_PORTUGUESE_BRAZILIAN, _T("pt_BR") },	// 151
    { _T("Punjabi"), wxLANGUAGE_PUNJABI, _T("pa") },								// 152
	{ _T("Quechua"), wxLANGUAGE_QUECHUA, _T("qu") },								// 153
	{ _T("Rhaeto-Romance"), wxLANGUAGE_RHAETO_ROMANCE, _T("rm") },					// 154
    { _T("Romanian"), wxLANGUAGE_ROMANIAN, _T("ro_RO") },							// 155
    { _T("Russian"), wxLANGUAGE_RUSSIAN, _T("ru_RU") },								// 156
    { _T("Russian (Ukraine)"), wxLANGUAGE_RUSSIAN_UKRAINE, _T("ru_UA") },			// 157
	{ _T("Samoan"), wxLANGUAGE_SAMOAN, _T("sm") },									// 158
	{ _T("Sangho"), wxLANGUAGE_SANGHO, _T("sg") },									// 159
	{ _T("Sanskrit"), wxLANGUAGE_SANSKRIT, _T("sa") },								// 160
	{ _T("Scots Gaelic"), wxLANGUAGE_SCOTS_GAELIC, _T("gd") },						// 161
    { _T("Serbian"), wxLANGUAGE_SERBIAN, _T("sr") },								// 162 // wxLANGUATE_SERVIAN not in InitLanguagesDB() returns null; IANA registry has code sr
	{ _T("Serbian (Cyrillic)"), wxLANGUAGE_SERBIAN_CYRILLIC, _T("sr_YU") },			// 163
	{ _T("Serbian (Latin)"), wxLANGUAGE_SERBIAN_LATIN, _T("sr_YU") },				// 164
	{ _T("Serbo-Croatian"), wxLANGUAGE_SERBO_CROATIAN, _T("sh") },					// 165
	{ _T("Sesotho"), wxLANGUAGE_SESOTHO, _T("st") },								// 166
	{ _T("Setswana"), wxLANGUAGE_SETSWANA, _T("tn") },								// 167
	{ _T("Shona"), wxLANGUAGE_SHONA, _T("sn") },									// 168
	{ _T("Sindhi"), wxLANGUAGE_SINDHI, _T("sd") },									// 169
	{ _T("Sinhalese"), wxLANGUAGE_SINHALESE, _T("si") },							// 170
	{ _T("Siswati"), wxLANGUAGE_SISWATI, _T("ss") },								// 171
    { _T("Slovak"), wxLANGUAGE_SLOVAK, _T("sk_SK") },								// 172
    { _T("Slovenian"), wxLANGUAGE_SLOVENIAN, _T("sl_SI") },							// 173
	{ _T("Somali"), wxLANGUAGE_SOMALI, _T("so") },									// 174
    { _T("Spanish"), wxLANGUAGE_SPANISH, _T("es_ES") },								// 175
    { _T("Spanish (Argentina)"), wxLANGUAGE_SPANISH_ARGENTINA, _T("es_AR") },		// 176
    { _T("Spanish (Bolivia)"), wxLANGUAGE_SPANISH_BOLIVIA, _T("es_BO") },			// 177
    { _T("Spanish (Chile)"), wxLANGUAGE_SPANISH_CHILE, _T("es_CL") },				// 178
    { _T("Spanish (Colombia)"), wxLANGUAGE_SPANISH_COLOMBIA, _T("es_CO") },			// 179
    { _T("Spanish (Costa Rica)"), wxLANGUAGE_SPANISH_COSTA_RICA, _T("es_CR") },		// 180
    { _T("Spanish (Dominican republic)"), wxLANGUAGE_SPANISH_DOMINICAN_REPUBLIC, _T("es_DO") },	// 181
    { _T("Spanish (Ecuador)"), wxLANGUAGE_SPANISH_ECUADOR, _T("es_EC") },			// 182
    { _T("Spanish (El Salvador)"), wxLANGUAGE_SPANISH_EL_SALVADOR, _T("es_SV") },	// 183
    { _T("Spanish (Guatemala)"), wxLANGUAGE_SPANISH_GUATEMALA, _T("es_GT") },		// 184
    { _T("Spanish (Honduras)"), wxLANGUAGE_SPANISH_HONDURAS, _T("es_HN") },			// 185
    { _T("Spanish (Mexico)"), wxLANGUAGE_SPANISH_MEXICAN, _T("es_MX") },			// 186
    { _T("Spanish (Modern)"), wxLANGUAGE_SPANISH_MODERN, _T("es_ES") },				// 187
    { _T("Spanish (Nicaragua)"), wxLANGUAGE_SPANISH_NICARAGUA, _T("es_NI") },		// 188
    { _T("Spanish (Panama)"), wxLANGUAGE_SPANISH_PANAMA, _T("es_PA") },				// 189
    { _T("Spanish (Paraguay)"), wxLANGUAGE_SPANISH_PARAGUAY, _T("es_PY") },			// 190
    { _T("Spanish (Peru)"), wxLANGUAGE_SPANISH_PERU, _T("es_PE") },					// 191
    { _T("Spanish (Puerto Rico)"), wxLANGUAGE_SPANISH_PUERTO_RICO, _T("es_PR") },	// 192
    { _T("Spanish (Uruguay)"), wxLANGUAGE_SPANISH_URUGUAY, _T("es_UY") },			// 193
    { _T("Spanish (U.S.)"), wxLANGUAGE_SPANISH_US, _T("es_US") },					// 194
    { _T("Spanish (Venezuela)"), wxLANGUAGE_SPANISH_VENEZUELA, _T("es_VE") },		// 195
    { _T("Sundanese"), wxLANGUAGE_SUNDANESE, _T("su") },							// 196
    { _T("Swahili"), wxLANGUAGE_SWAHILI, _T("sw_KE") },								// 197
    { _T("Swedish"), wxLANGUAGE_SWEDISH, _T("sv_SE") },								// 198
    { _T("Swedish (Finland)"), wxLANGUAGE_SWEDISH_FINLAND, _T("sv_FI") },			// 199
    { _T("Tagalog"), wxLANGUAGE_TAGALOG, _T("tl_PH") },								// 200
	{ _T("Tajik"), wxLANGUAGE_TAJIK, _T("tg") },									// 201
    { _T("Tamil"), wxLANGUAGE_TAMIL, _T("ta") },									// 202
    { _T("Tatar"), wxLANGUAGE_TATAR, _T("tt") },									// 203
	{ _T("Telugu"), wxLANGUAGE_TELUGU, _T("te") },									// 204
    { _T("Thai"), wxLANGUAGE_THAI, _T("th_TH") },									// 205
	{ _T("Tibetan"), wxLANGUAGE_TIBETAN, _T("bo") },								// 206
	{ _T("Tigrinya"), wxLANGUAGE_TIGRINYA, _T("ti") },								// 207
	{ _T("Tonga"), wxLANGUAGE_TONGA, _T("to") },									// 208
	{ _T("Tsonga"), wxLANGUAGE_TSONGA, _T("ts") },									// 209
    { _T("Turkish"), wxLANGUAGE_TURKISH, _T("tr_TR") },								// 210
	{ _T("Turkmen"), wxLANGUAGE_TURKMEN, _T("tk") },								// 211
	{ _T("Twi"), wxLANGUAGE_TWI, _T("tw") },										// 212
	{ _T("Uighur"), wxLANGUAGE_UIGHUR, _T("ug") },									// 213
    { _T("Ukrainian"), wxLANGUAGE_UKRAINIAN, _T("uk_UA") },							// 214
	{ _T("Urdu"), wxLANGUAGE_URDU, _T("ur") },										// 215
	{ _T("Urdu (India)"), wxLANGUAGE_URDU_INDIA, _T("ur_IN") },						// 216
	{ _T("Urdu (Pakistan)"), wxLANGUAGE_URDU_PAKISTAN, _T("ur_PK") },				// 217
	{ _T("Uzbek"), wxLANGUAGE_UZBEK, _T("uz") },									// 218
	{ _T("Uzbek (Cyrillic)"), wxLANGUAGE_UZBEK_CYRILLIC, _T("uz") },				// 219 // no script indicated; canonicalName could be uz__Cyrl [see Azeri above]
	{ _T("Uzbek (Latin)"), wxLANGUAGE_UZBEK_LATIN, _T("uz") },						// 220 // no script indicated; canonicalName could be uz_Latn [see Azeri above]
	{ _T("Vietnamese"), wxLANGUAGE_VIETNAMESE, _T("vi_VN") },						// 221
	{ _T("Volapuk"), wxLANGUAGE_VOLAPUK, _T("vo") },								// 222
	{ _T("Welsh"), wxLANGUAGE_WELSH, _T("cy") },									// 223
	{ _T("Wolof"), wxLANGUAGE_WOLOF, _T("wo") },									// 224
	{ _T("Xhosa"), wxLANGUAGE_XHOSA, _T("xh") },									// 225
	{ _T("Yiddish"), wxLANGUAGE_YIDDISH, _T("yi") },								// 226
	{ _T("Yoruba"), wxLANGUAGE_YORUBA, _T("yo") },									// 227
	{ _T("Zhuang"), wxLANGUAGE_ZHUANG, _T("za") },									// 228
	{ _T("Zulu"), wxLANGUAGE_ZULU, _T("zu") },										// 229
	// wxLANGUAGE_UNKNOWN needs to be here at the end of the array so we can search the array
	// using a while loop while langsKnownToWX[ct].name != NULL.
    { NULL, wxLANGUAGE_UNKNOWN, NULL}												// 1
};

// beginning of AIModalDialog class implementation !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// whm Note: The AIModalDialog class exists as a base dialog class for Adapt It modal dialogs.
// AIModalDialog functions practically identical to its own base class wxDialog. It simply 
// overrides ShowModal and the class destructor in order to suspend wxIdleEvent processing 
// while the dialog is being displayed/modal, and restores idle processing when the dialog is 
// destroyed. Suspending idle events helps to prevent crashes due to time-triggered events 
// such as autosave that, if performed, encounter bad pointers

////////////////////////////////////////////////////////////////////////////////////////////
/// \remarks
/// Constructor for the AIModalDialog class.
/// Used as a base class for all Adapt It dialogs which are invoked as modal dialogs.
////////////////////////////////////////////////////////////////////////////////////////////
AIModalDialog::AIModalDialog( wxWindow *parent, const wxWindowID id, const wxString& title,
			const wxPoint& pos, const wxSize& size, const long windowStyle ) :
			wxDialog(parent, id, title, pos, size, windowStyle)
{
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \remarks
/// Destructor for the AIModalDialog class.
/// Used as the base class destructor all Adapt It dialogs which are invoked as modal dialogs.
/// This destructor calls the wxIdleEvent::SetMode(wxIDLE_PROCESS_ALL), and the 
/// wxUpdateUIEvent::SetMode(wxUPDATE_UI_PROCESS_ALL), which allows the application to resume
/// its normal idle processing once the AIModalDialog based dialog closes.
////////////////////////////////////////////////////////////////////////////////////////////
AIModalDialog::~AIModalDialog()
{
	// override of wxDialog's destructor to start up idle processing again
	wxIdleEvent::SetMode(wxIDLE_PROCESS_ALL);
	wxUpdateUIEvent::SetMode(wxUPDATE_UI_PROCESS_ALL);
	//wxDialog::~wxDialog(); // causes crash if base destructor is called explicitly
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \remarks
/// Override of the wxDialog's ShowModal() method.
/// This override effectively turns off the wxIdleEvent and wxUpdateUIEvent idle processing while
/// any AIModalDialog based dialog is being shown in modal fashion.
////////////////////////////////////////////////////////////////////////////////////////////
int AIModalDialog::ShowModal()
{
	// override of wxDialog's ShowModal() method to stop all idle processing including
	// processing of UI events while the modal dialog is displaying.
	wxIdleEvent::SetMode(wxIDLE_PROCESS_SPECIFIED);
	wxUpdateUIEvent::SetMode(wxUPDATE_UI_PROCESS_SPECIFIED);
	return wxDialog::ShowModal();
}
// end of AIModalDialog class !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

IMPLEMENT_APP(CAdapt_ItApp)
// This is used in the application class implementation file to make the 
// application class known to wxWidgets for dynamic construction. 
// Use this instead of MyApp myApp;

BEGIN_EVENT_TABLE(CAdapt_ItApp, wxApp)
	//EVT_IDLE(CAdapt_ItApp::OnIdle)
	// OnIdle() handler was moved to CMainFrame. Having it here in 
	// the App was causing File | Exit and x App cancel to become 
	// unresponsive. Same code works fine there.

	// File Menu handlers !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	EVT_MENU(wxID_NEW, CAdapt_ItApp::OnFileNew)	// In MFC event table but not implemented anywhere in MFC.
	// OnFileOpen is in the Doc 
	// Note: In the MFC version OnFileOpen() is implemented in the Doc and sets the App's
	// m_curAdaptationsPath, and calls DoFileOpen() here in the App which in turn  
	// calls the Doc's OnOpenDocument().

	// OnFileSave is in the Doc
	// OnUpdateFileSave is in the Doc
	// OnFileClose is in the Doc
	// OnUpdateFileClose is in the Doc
	// OnFileExportSource is in the View
	// OnUpdateFileExportSource is in the View
	// OnFileExport is in the View
	// OnUpdateFileExport is in the View
	// OnFileExportToRtf is in the View
	// OnUpdateFileExportToRtf is in the View
	// OnFilePrint is in the View
	// OnUpdateFilePrint is in the View
	// OnFilePrintPreview is in the View
	// OnUpdateFilePrintPreview is in the View
	// According to the wxWidgets developers, the "Print Setup..." menu selection is obsolete since
	// Windows 95. Users are expecte to do any necessary print setup from the main print dialog.
	//EVT_MENU(wxID_PRINT_SETUP, OnFilePrintSetup) 
	// OnUpdateFilePrintSetup is in the View
	EVT_MENU(wxID_PAGE_SETUP,CAdapt_ItApp::OnFilePageSetup)
	EVT_UPDATE_UI(wxID_PAGE_SETUP, CAdapt_ItApp::OnUpdateFilePageSetup)
	// OnFileStartupWizard is in the View
	EVT_UPDATE_UI(ID_FILE_STARTUP_WIZARD, CAdapt_ItApp::OnUpdateFileStartupWizard)
	// OnFileCloseProject is in the View
	// OnUpdateFileCloseKb is in the View
	// OnFileSaveKB is in the View
	// OnUpdateFileSaveKb is in the View
	EVT_MENU(ID_FILE_BACKUP_KB, CAdapt_ItApp::OnFileBackupKb)
	EVT_UPDATE_UI(ID_FILE_BACKUP_KB, CAdapt_ItApp::OnUpdateFileBackupKb)
	EVT_MENU(ID_FILE_CHECK_KB, CAdapt_ItApp::OnFileCheckKb)
	EVT_UPDATE_UI(ID_FILE_CHECK_KB, CAdapt_ItApp::OnUpdateFileCheckKb)
	EVT_MENU(ID_FILE_RESTORE_KB, CAdapt_ItApp::OnFileRestoreKb)
	EVT_UPDATE_UI(ID_FILE_RESTORE_KB, CAdapt_ItApp::OnUpdateFileRestoreKb)
	EVT_MENU(ID_FILE_CHANGEFOLDER, CAdapt_ItApp::OnFileChangeFolder)
	EVT_UPDATE_UI(ID_FILE_CHANGEFOLDER, CAdapt_ItApp::OnUpdateFileChangeFolder)
	//EVT_MENU(ID_FILE_SAVE_AS_XML, CAdapt_ItApp::OnFileSaveAsXml)    // removed for wx version - Bruce
																	  //removed from MFC 3Apr08
	//EVT_UPDATE_UI(ID_FILE_SAVE_AS_XML, CAdapt_ItApp::OnUpdateFileSaveAsXml)    // removed for wx version
																				 //removed from MFC 3Apr08
	// OnFileExportKb is in the View
	// OnUpdateFileExportKb is in the View
	// OnImportToKb is in the View
	// OnUpdateImportToKb is in the View
	//EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, CAdapt_ItApp::OnOpenRecentFile)// renamed to OnMRUFile and moved
																			// to CMainFrame

	// We must not associate OnExit with wxID_EXIT. If we do,
	// OnExit() execute immediately upon selection of File|Exit
	// causing all sorts of problems. Rather we must allow 
	// OnExit() to be called by the doc/view framework after
	// it has deleted the current doc and view. Otherwise, if
	// this handler below is enabled, the app won't exit
	// without crashing in the process.
	//EVT_MENU(wxID_EXIT, OnExit)

	// Edit Menu handlers !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// OnEditUndo is in the View
	// OnUpdateEditUndo is in the View
	// OnEditCut is in the View
	// OnUpdateEditCut is in the View
	// OnEditCopy is in the View
	// OnUpdateEditCopy is in the View
	// OnEditPaste is in the View
	// OnUpdateEditPaste is in the View
	// OnGoTo is in the View
	// OnUpdateGoTo is in the View
	// OnEditPunctCorresp is in the View  // incorporated into Edit|Preferences
	// OnUpdateEditPunctCorresp is in the View  // incorporated into Edit|Preferences
	// OnEditSourceText is in the View
	// OnUpdateEditSourceText is in the View
	// OnEditConsistencyCheck is in the View
	// OnUpdateEditConsistencyCheck is in the View
	// Lower to Upper Case Equivalences... removed as a separate menu item from Edit 
	// menu (now handled by Edit Preferences)
	// OnEditPreferences is in the View
	// OnUpdateEditPreferences is in the View

	// View Menu handlers !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//OnViewToolBar is in the MainFrame
	//OnUpdateViewToolBar is in the MainFrame
	//OnViewStatusBar is in the MainFrame
	//OnUpdateViewStatusBar is in the MainFrame
	//OnViewComposeBar is in the MainFrame
	//OnUpdateViewComposeBar is in the MainFrame
	//OnCopySource is in the View
	//OnUpdateCopySource is in the View // not in MFC version
	//OnFitWindow is in the View
	//OnMarkerWrapsStrip is in the View
	//OnUpdateMarkerWrapsStrip is in the View
	//OnUnits is in the View
	//OnUpdateUnits is in the View

	// Tools Menu handlers !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//OnFind is in the View
	//OnUpdateFind is in the View
	//OnReplace is in the View
	//OnUpdateReplace is in the View
	EVT_MENU(ID_TOOLS_DEFINE_CC, CAdapt_ItApp::OnToolsDefineCC)
	EVT_UPDATE_UI(ID_TOOLS_DEFINE_CC, CAdapt_ItApp::OnUpdateLoadCcTables)
	EVT_MENU(ID_UNLOAD_CC_TABLES, CAdapt_ItApp::OnToolsUnloadCcTables)
	EVT_UPDATE_UI(ID_UNLOAD_CC_TABLES, CAdapt_ItApp::OnUpdateUnloadCcTables)
	//OnUseConsistentChanges is in the View
	//OnUpdateUseConsistentChanges is in the View
	//OnAcceptChanges is in the View
	//OnUpdateAcceptChanges is in the View
	//OnToolsKbEditor is in the View
	//OnUpdateToolsKbEditor is in the View

	// Note: Automatic capitalization is no longer a separate menu item
	// now that there is a tab in Edit Prefences for Capitalization
	EVT_MENU(ID_TOOLS_AUTO_CAPITALIZATION, CAdapt_ItApp::OnToolsAutoCapitalization)
	EVT_UPDATE_UI(ID_TOOLS_AUTO_CAPITALIZATION, CAdapt_ItApp::OnUpdateToolsAutoCapitalization)
	//OnRetransReport is in the View
	//OnUpdateRetransReport is in the View

	// Advanced Menu handlers !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//OnAdvancedEnableglossing is in the View
	//OnUpdateAdvancedEnableglossing  is in the View
	//OnAdvancedGlossingUsesNavFont  is in the View
	//OnUpdateAdvancedGlossingUsesNavFont  is in the View
	EVT_MENU(ID_ADVANCED_TRANSFORM_ADAPTATIONS_INTO_GLOSSES, CAdapt_ItApp::OnAdvancedTransformAdaptationsIntoGlosses)
	EVT_UPDATE_UI(ID_ADVANCED_TRANSFORM_ADAPTATIONS_INTO_GLOSSES, CAdapt_ItApp::OnUpdateAdvancedTransformAdaptationsIntoGlosses)
	EVT_MENU(ID_ADVANCED_BOOKMODE, CAdapt_ItApp::OnAdvancedBookMode)
	EVT_UPDATE_UI(ID_ADVANCED_BOOKMODE, CAdapt_ItApp::OnUpdateAdvancedBookMode)
	//OnAdvancedDelay  is in the View
	//OnUpdateAdvancedDelay  is in the View

	// Help menu handlers !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//OnAppAbout is in CMainFrame in wxWidgets version

	//EVT_WIZARD_PAGE_CHANGING(IDC_WIZARD,CAdapt_ItApp::WizardPageIsChanging)
	//EVT_WIZARD_FINISHED(-1,CAdapt_ItApp::OnWizardFinish) // not needed, can handle directly
	// by checking the return bool value of RunWizard.
END_EVENT_TABLE()

// ///////////////////////////////////////////////////////////////////////////
// NOTE: Under wxWidgets I've changed the following from static TCHAR arrays to
// just wxStrings, since all the methods in wxWidgets are efficient 
// and sufficiently UNICODE aware with wxStrings.
// 
// whm I've decided that the literal strings that are used to identify parts of
// the configuration files should NOT be localizable. Therefore, I've used the _T() 
// literal string macro (which makes them ignored for translation when GetText 
// and/or PoEdit collects the translatable strings for localization). Reason: If 
// these strings are localized, they would create a situation in which the config 
// files would only be readable by others using the same localized interface 
// version of Adapt It. This might become a problem if say a team using the 
// French interface were to pack up their Adapt It documents and sent them to a 
// consultant who is using a different interface version of Adapt It. Packed 
// files would not be compatible except for unpacking on machines using the 
// same language interface version.

/// The name used internally within the project settings configuration file.
wxString szProjectSettings = _T("ProjectSettings"); // don't need \n now
/// The name used internally within the basic settings configuration file.
wxString szBasicSettings = _T("Settings"); // don't need \n now

/// The name of the project configuration file without the .aic extension.
wxString szProjectConfiguration = _T("AI-ProjectConfiguration");

/// The name of the basic configuration file without the .aic extension.
wxString szBasicConfiguration = _T("AI-BasicConfiguration");

/// The name that introduces the properties of the source font within both
/// the basic and project configuration files.
wxString szSourceFont = _T("SourceFont"); // don't need \n now

/// The name that introduces the properties of the target font within both
/// the basic and project configuration files.
wxString szTargetFont = _T("TargetFont"); // don't need \n now

/// The name that introduces the properties of the navigation font within both
/// the basic and project configuration files.
wxString szNavTextFont = _T("NavTextFont"); // don't need \n now

/// The label that identifies the following number as a "negative height" value
/// for the current font being described in the basic and project configuration
/// files. See the PointSizeToNegHeight() and NegHeightToPointSize() helper
/// functions in help.cpp for information on how a "negative height" value is
/// converted to a standard point size.
wxString szHeight = _T("Height");

/// The label that identifies the following number as a font "Width". This number
/// is not used in the wxWidgets version of Adapt It. It is set to zero if the
/// wx version creates the configuration file, or preserves any existing Width
/// value when loading a configuration file created by an MFC version of Adapt It.
wxString szWidth = _T("Width");

/// The label that identifies the following number as a font "Escapement". 
/// This number is not used in the wxWidgets version of Adapt It. It is set to 
/// zero if the wx version creates the configuration file, or preserves any 
/// existing Escapement value when loading a configuration file created by an 
/// MFC version of Adapt It.
wxString szEscapement = _T("Escapement");

/// The label that identifies the following number as a font "Orientation". 
/// This number is not used in the wxWidgets version of Adapt It. It is set to 
/// zero if the wx version creates the configuration file, or preserves any 
/// existing Orientation value when loading a configuration file created by an 
/// MFC version of Adapt It.
wxString szOrientation = _T("Orientation");

/// The label that identifies the following number as a font "Weight". 
/// The wx version converts this value internally to its own enum value
/// for wxLIGHT (400-499), wxNORMAL (500-699), or wxBOLD (700 or more). 
/// For compatability with the MFC version, the wx version writes the 
/// numerical value only (as strings) in the configuration files.
wxString szWeight = _T("Weight");

/// The label that identifies the following number as font "Italic" if
/// "1", or normal if "0". The wx version converts this value internally 
/// to its own enum value for wxITALIC or wxNORMAL. 
/// For compatability with the MFC version, the wx version writes the 
/// numerical value only (as strings) in the configuration files.
wxString szItalic = _T("Italic");

/// The label that identifies the following number as font "StrikeOut". 
/// This number is not used in the wxWidgets version of Adapt It. It is set to 
/// zero if the wx version creates the configuration file, or preserves any 
/// existing StrikeOut value when loading a configuration file created by an 
/// MFC version of Adapt It.
wxString szStrikeOut = _T("StrikeOut");

/// The label that identifies the following number as font "Underline" if
/// "1", or normal if "0". The wx version converts this value internally 
/// to its own boolean values of TRUE for Underline, FALSE otherwise.
/// For compatability with the MFC version, the wx version writes the 
/// numerical value only (as strings) in the configuration files.
wxString szUnderline = _T("Underline");

/// The label that identifies the following number as bitmap composite for
/// font "PitchAndFamily". The wx version is only interested in the font
/// family part of this composite value, which it determines by masking
/// out the 4 low order bits of the value. Only the font family values are
/// used within the wx version. The font "pitch" value is stored for later
/// composition again with the "family" value for saving the composite in
/// the configuration files.
/// For compatability with the MFC version, the wx version writes the 
/// numerical value only (as strings) in the configuration files.
wxString szPitchAndFamily = _T("PitchAndFamily");

/// The label that identifies the following number as a font "FontEncoding".
/// The wx version makes use of this value for ANSI builds. It is not included
/// in a configuration file created by the MFC version. 
/// Note: setting the font encoding of the actual font is done after CharSet value 
/// is read from the config file because, if the config file was produced by the 
/// MFC app, it won't have this szFontEncoding field, but ig will have the 
/// CharSet value. 
wxString szFontEncoding = _T("FontEncoding");

/// The label that identifies the following number as a font "CharSet".
/// The wx version retains the CharSet value mainly for backwards compatibility
/// with the MFC version's configure files. Note: It is when the CharSet value is
/// read from the configuration file that font encoding is established for the font
/// in ANSI builds (if the congig file has a FontEncoding value. The wx version
/// uses its MapMFCCharsetToWXFontEncoding(), and MapWXFontEncodingToMFCCharset()
/// functions to map values to and from the closest font encoding equivalents that
/// it knows about.
wxString szCharSet = _T("CharSet");

/// The label that identifies the following number as font "OutPrecision". 
/// This number is not used in the wxWidgets version of Adapt It. It is set to 
/// 3 (OUT_OUTLINE_PRECIS) as its default for TrueType fonts if the wx version 
/// creates the configuration file, or preserves any existing OutPrecision value 
/// when loading a configuration file created by an MFC version of Adapt It.
wxString szOutPrecision = _T("OutPrecision");

/// The label that identifies the following number as font "ClipPrecision". 
/// This number is not used in the wxWidgets version of Adapt It. It is set to 
/// 2 (CLIP_STROKE_PRECIS) as its default if the wx version creates the 
/// configuration file, or preserves any existing ClipPrecision value when 
/// loading a configuration file created by an MFC version of Adapt It.
wxString szClipPrecision = _T("ClipPrecision");

/// The label that identifies the following number as font "Quality". 
/// This number is not used in the wxWidgets version of Adapt It. It is set to 
/// 0 (ANTIALIASED_QUALITY) as its default if the wx version creates the 
/// configuration file, or preserves any existing Quality value when 
/// loading a configuration file created by an MFC version of Adapt It.
wxString szQuality = _T("Quality");

/// The label that identifies the following string as font "FaceName". 
/// The wx version attempts to set the FaceName of its fonts using this
/// string value stored in the configurations files by calling the font's
/// SetFaceName() method. It SetFaceName() does not succeed, the application
/// calls the font's GetFaceName() method to have the system decide on a
/// suitable font.
wxString szFaceName = _T("FaceName");

//wxString szSystem = _("System"); // unused

/// The label that identifies the following number as a font "Color".
/// The wx version uses its Int2wxColour(), and WxColour2Int() functions to 
/// map values to and from the the numerical color equivalent values. Note: 
/// A font's color is handled in objects of the wxFontData class rather than
/// in the wxFont class.
wxString szColor = _T("Color");

// stuff I am not yet using is below
//wxString szWordWrap = _("WordWrap"); // unused
//wxString szTabStops = _("TabStops"); // unused

// entry names for the settings info

/// The label that identifies the following string as the project's 
/// "SourceLanguageName". This value is written in the "Settings" part
/// of the basic configuration file and in the "ProjectSettings" part
/// of the project configuration file. Adapt It stores this path
/// in the App's m_sourceName global variable.
wxString szSourceLanguageName = _T("SourceLanguageName"); // stored in the App's m_sourceName

/// The label that identifies the following string as the project's 
/// "TargetLanguageName". This value is written in the "Settings" part
/// of the basic configuration file and in the "ProjectSettings" part
/// of the project configuration file. Adapt It stores this path
/// in the App's m_targetName global variable.
wxString szTargetLanguageName = _T("TargetLanguageName"); // stored in the App's m_targetName

/// The label that identifies the following string as the project's 
/// "TargetLanguageName". This value is written in the "Settings" part
/// of the basic configuration file. After validating this path to 
/// insure its validity on the local machine, Adapt It stores this path
/// in the App's m_workFolderPath global variable.
wxString szAdaptitPath = _T("AdaptItPath"); // stored in the App's m_workFolderPath

/// The label that identifies the following string as the project's 
/// "ProjectName". This value is written in the "Settings" part
/// of the basic configuration file. Adapt It stores this name in
/// the App's m_curProjectName global variable.
wxString szCurProjectName = _T("ProjectName"); // stored in the App's m_curProjectName

/// The label that identifies the following string as the project's 
/// "ProjectFolderPath". This value is written in the "ProjectSettings" 
/// part of the project configuration file. Adapt It stores this path in
/// the App's m_curProjectPath global variable.
wxString szCurLanguagesPath = _T("ProjectFolderPath"); // stored in the App's m_curProjectPath

/// The label that identifies the following string as the project's 
/// "DocumentsFolderPath". This value is written in the "ProjectSettings" 
/// part of the project configuration file. Adapt It stores this path in
/// the App's m_curAdaptionsPath global variable.
wxString szCurAdaptionsPath = _T("DocumentsFolderPath"); // stored in the App's m_curAdaptionsPath

/// The label that identifies the following string as the project's 
/// "KnowledgeBaseName". This value is written in the "ProjectSettings" 
/// part of the project configuration file. Adapt It stores this name in
/// the App's m_curKBName global variable.
wxString szCurKBName = _T("KnowledgeBaseName"); // stored in the App's m_curKBName

/// The label that identifies the following string as the project's 
/// "KnowledgeBasePath". This value is written in the "ProjectSettings" 
/// part of the project configuration file. Adapt It stores this path in
/// the App's m_curKBName global variable.
wxString szCurKBPath = _T("KnowledgeBasePath"); // stored in the App's m_curKBName

/// The label that identifies the following string as the project's 
/// "KBBackupPath". This value is written in the "ProjectSettings" 
/// part of the project configuration file. Adapt It stores this path in
/// the App's m_curKBBackupPath global variable.
wxString szCurKBBackupPath = _T("KBBackupPath"); // stored in the App's m_curKBBackupPath

/// The label that identifies the following string as the project's 
/// "LastNewDocumentFolder". This value is written in the "ProjectSettings" 
/// part of the project configuration file. Adapt It stores this path in
/// the App's m_lastSourceFileFolder global variable.
wxString szLastSourceFileFolder = _T("LastNewDocumentFolder"); // stored in the App's m_lastSourceFileFolder

/// The label that identifies the following string as the project's 
/// "LastSourceTextExportPath". This value is written in the "ProjectSettings" 
/// part of the project configuration file. Adapt It stores this path in
/// the App's m_lastSrcExportPath global variable.
wxString szLastSourceExportPath = _T("LastSourceTextExportPath"); // stored in the App's m_lastSrcExportPath

/// The label that identifies the following string as the project's 
/// "KB_ExportPath". This value is written in the "ProjectSettings" 
/// part of the project configuration file. Adapt It stores this path in
/// the App's m_kbExportPath global variable.
wxString szKBExportPath = _T("KB_ExportPath"); // stored in the App's m_kbExportPath

/// The label that identifies the following string as the project's 
/// "RetranslationReportPath". This value is written in the "ProjectSettings" 
/// part of the project configuration file. Adapt It stores this path in
/// the App's m_retransReportPath global variable.
wxString szRetranslationReportPath = _T("RetranslationReportPath"); // stored in the App's m_retransReportPath

/// The label that identifies the following string as the project's 
/// "RTFExportPath". This value is written in the "ProjectSettings" 
/// part of the project configuration file. Adapt It stores this path in
/// the App's m_rtfExportPath global variable.
wxString szRTFExportPath = _T("RTFExportPath"); // stored in the App's m_rtfExportPath

// the following ones relate to view parameters

/// The label that identifies the following string encoded number as the application's 
/// "MaxToDisplay". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_nMaxToDisplay global variable.
wxString szMaxToDisplay = _T("MaxToDisplay"); // stored in the App's m_nMaxToDisplay

/// The label that identifies the following string encoded number as the application's 
/// "MinPrecedingContext". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_nPrecedingContext global variable.
wxString szMinPrecContext = _T("MinPrecedingContext"); // stored in the App's m_nPrecedingContext

/// The label that identifies the following string encoded number as the application's 
/// "MinFollowingContext". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_nFollowingContext global variable.
wxString szMinFollContext = _T("MinFollowingContext"); // stored in the App's m_nFollowingContext

/// The label that identifies the following string encoded number as the application's 
/// "Leading". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_curLeading global variable.
wxString szLeading = _T("Leading"); // stored in the App's m_curLeading

/// The label that identifies the following string encoded number as the application's 
/// "LeftMargin". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_curLMargin global variable.
wxString szLeftMargin = _T("LeftMargin"); // stored in the App's m_curLMargin

/// The label that identifies the following string encoded number as the application's 
/// "InterpileGapWidth". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_curGapWidth global variable.
wxString szGapWidth = _T("InterpileGapWidth"); // stored in the App's m_curGapWidth

/// The label that identifies the following string encoded number as the application's 
/// "SuppressFirstLine". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_bSuppressFirst global variable.
wxString szSuppressFirst = _T("SuppressFirstLine"); // stored in the App's m_bSuppressFirst

/// The label that identifies the following string encoded number as the application's 
/// "SuppressLastLine". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_bSuppressLast global variable.
wxString szSuppressLast = _T("SuppressLastLine"); // stored in the App's m_bSuppressLast

/// The label that identifies the following string encoded number as the application's 
/// "HidePunctuationFlag". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_bHidePunctuation global variable.
wxString szHidePunctuation = _T("HidePunctuationFlag"); // stored in the App's m_bHidePunctuation

/// The label that identifies the following string encoded number as the application's 
/// "SpecialTextColor". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_specialTextColor global variable.
wxString szSpecialTextColor = _T("SpecialTextColor"); // stored in the App's m_specialTextColor

/// The label that identifies the following string encoded number as the application's 
/// "RetranslationTextColor". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_reTranslnTextColor global variable.
wxString szReTranslnTextColor = _T("RetranslationTextColor"); // stored in the App's m_reTranslnTextColor

/// The label that identifies the following string encoded number as the application's 
/// "PhraseBoxExpansionMultiplier". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's gnExpandBox global variable.
wxString szPhraseBoxExpansionMultiplier = _T("PhraseBoxExpansionMultiplier"); // stored in the App's gnExpandBox

/// The label that identifies the following string encoded number as the application's 
/// "TooNearEndMultiplier". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's gnNearEndFactor global variable.
wxString szTooNearEndMultiplier = _T("TooNearEndMultiplier"); // stored in the App's gnNearEndFactor

/// The label that identifies the following string encoded number as the application's
/// "LegacyCopyForPhraseBox". This value is written in the "ProjectSettings" part of the project
/// configuration file. Adapt It stores this path in the App's gbLegacySourceTextCopy global variable.
wxString szLegacyCopyForPhraseBox = _T("LegacyCopyForPhraseBox");

// Next two were for old punct, for when source & target are not differentiated

/// A label now unused, that was used in old versions for punctuation. It is only retained
/// for reading legacy config files safely.
wxString szPunctuation = _T("Punctuation");

/// A label now unused, that was used in old versions for punctuation. It is only retained
/// for reading legacy config files safely.
wxString szPunctAsWordBuilding = _T("PunctuationAsWordBuilding");

// Next four were for newer variables but currently unused, for when source and target punctuation are potentially differentiated

/// A label now unused, that was used in old versions for punctuation. It is only retained
/// for reading legacy config files safely.
wxString szSrcPunctuation = _T("SourcePunctuation");

/// A label now unused, that was used in old versions for punctuation. It is only retained
/// for reading legacy config files safely.
wxString szSrcPunctAsWordBuilding = _T("SourcePunctAsWordBuilding");

/// A label now unused, that was used in old versions for punctuation. It is only retained
/// for reading legacy config files safely.
wxString szTgtPunctuation = _T("TargetPunctuation");

/// A label now unused, that was used in old versions for punctuation. It is only retained
/// for reading legacy config files safely.
wxString szTgtPunctAsWordBuilding = _T("TargetPunctAsWordBuilding");

/// The label that identifies the following string as the application's 
/// "PunctuationPairs(stores space for an empty cell)". 
/// This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this punctuation in the App's m_punctPairs global array. The
/// application uses the PunctPairsToString() and StringToPunctPairs() functions to 
/// manipulate the pairs of punctuation data.
wxString szPunctPairs = _T("PunctuationPairs(stores space for an empty cell)"); // stored in the App's m_punctPairs

/// The label that identifies the following string as the application's 
/// "PunctuationTwoCharacterPairs". Used only in the Unicode version
/// This value is written in the "Settings" part of the basic configuration.
/// file. Adapt It stores this punctuation in the App's m_twopunctPairs global array. The
/// application uses the TwoPunctPairsToString() and StringToTwoPunctPairs() functions to 
/// manipulate the pairs of punctuation data.
wxString szTwoPunctPairs = _T("PunctuationTwoCharacterPairs"); // stored in the App's m_twopunctPairs
#ifdef _UNICODE

/// The label that identifies the following string as the application's 
/// "PunctuationPairsSourceSet(stores space for an empty cell)". 
/// This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this punctuation in the App's m_twopunctPairs global string. The
/// application uses the PunctPairsToTwoStrings() and TwoStringsToPunctPairs() functions to 
/// manipulate the pairs of punctuation data, punctuation data for the source language used here.
wxString szPunctPairsSrc = _T("PunctuationPairsSourceSet(stores space for an empty cell)"); // stored in the App's m_twopunctPairs

/// The label that identifies the following string as the application's 
/// "PunctuationTwoCharacterPairsSourceSet(ditto)". 
/// This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this punctuation in the App's m_twopunctPairs global string. The
/// application uses the TwoPunctPairsToTwoStrings() and TwoStringsToTwoPunctPairs() functions to 
/// manipulate the pairs of punctuation data, punctuation data for the source language used here.
wxString szTwoPunctPairsSrc = _T("PunctuationTwoCharacterPairsSourceSet(ditto)"); // stored in the App's m_twopunctPairs

/// The label that identifies the following string as the application's 
/// "PunctuationPairsTargetSet(stores space for an empty cell)". 
/// This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this punctuation in the App's m_twopunctPairs global string. The
/// application uses the PunctPairsToTwoStrings() and TwoStringsToPunctPairs() functions to 
/// manipulate the pairs of punctuation data, punctuation data for the target language used here.
wxString szPunctPairsTgt = _T("PunctuationPairsTargetSet(stores space for an empty cell)"); // stored in the App's m_twopunctPairs

/// The label that identifies the following string as the application's 
/// "PunctuationTwoCharacterPairsTargetSet(ditto)". 
/// This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this punctuation in the App's m_twopunctPairs global string. The
/// application uses the TwoPunctPairsToTwoStrings() and TwoStringsToTwoPunctPairs() functions to 
/// manipulate the pairs of punctuation data, punctuation data for the target language used here.
wxString szTwoPunctPairsTgt = _T("PunctuationTwoCharacterPairsTargetSet(ditto)"); // stored in the App's m_twopunctPairs
#endif

/// The label that identifies the following string encoded number as the application's 
/// "SuppressWelcome". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_bSuppressWelcome global variable.
wxString szSuppressWelcome = _T("SuppressWelcome"); // stored in the App's m_bSuppressWelcome

/// The label that identifies the following string encoded number as the application's 
/// "SuppressTargetHighlighting". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_bSuppressTargetHighlighting global variable.
wxString szSuppressTargetHighlighting = _T("SuppressTargetHighlighting"); // stored in the App's m_bSuppressTargetHighlighting

/// The label that identifies the following string encoded number as the application's 
/// "AutoInsertionsHighlightColor". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_AutoInsertionsHighlightColor global variable.
/// Adapt It uses the WxColour2Int() and Int2wxColour() helper functions to convert between the integer
/// and wx color enum symbols.
wxString szAutoInsertionsHighlightColor = _T("AutoInsertionsHighlightColor"); // stored in the App's m_AutoInsertionsHighlightColor

/// The label that identifies the following string encoded number as the application's 
/// "UseStartupWizardOnLaunch". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_bUseStartupWizardOnLaunch global variable.
wxString szUseStartupWizardOnLaunch = _T("UseStartupWizardOnLaunch"); // stored in the App's m_bUseStartupWizardOnLaunch

/// The label that identifies the following string encoded number as the application's 
/// "BackupKnowledgeBase(Boolean)". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_bAutoBackupKB global variable.
wxString szBackupKBFlag = _T("BackupKnowledgeBase(Boolean)"); // stored in the App's m_bAutoBackupKB

/// The label that identifies the following string encoded number as the application's 
/// "TimeSpanForDocument - minutes". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_timeSettings global variable.
wxString szTS_DOC_MINS = _T("TimeSpanForDocument - minutes"); // stored in the App's m_timeSettings

/// The label that identifies the following string encoded number as the application's 
/// "TimeSpanForDocument - seconds". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It no longer makes use of the seconds time span value.
wxString szTS_DOC_SECS = _T("TimeSpanForDocument - seconds");

/// The label that identifies the following string encoded number as the application's 
/// "TimeSpanForKB - minutes". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_timeSettings global variable.
wxString szTS_KB_MINS = _T("TimeSpanForKB - minutes"); // stored in the App's m_timeSettings

/// The label that identifies the following string encoded number as the application's 
/// "TimeSpanForKB - seconds". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It no longer makes use of the seconds time span value.
wxString szTS_KB_SECS = _T("TimeSpanForKB - seconds");

/// The label that identifies the following string encoded number as the application's 
/// "NoAutoSaveFlag(Boolean)". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_bNoAutoSave global variable.
wxString szNoAutoSaveFlag = _T("NoAutoSaveFlag(Boolean)"); // stored in the App's m_bNoAutoSave

/// The label that identifies the following string encoded number as the application's 
/// "DocumentTimeSpanButtonIsON(Boolean)". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_bIsDocTimeButton global variable.
wxString szIsDocTimeButtonFlag = _T("DocumentTimeSpanButtonIsON(Boolean)"); // stored in the App's m_bIsDocTimeButton

/// The label that identifies the following string encoded number as the application's 
/// "PhraseBoxMovesForSave". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_nMoves global variable.
wxString szPhraseBoxMoves = _T("PhraseBoxMovesForSave"); // stored in the App's m_nMoves

/// The label that identifies the following string encoded number as the application's 
/// "ColorOfNavigationText". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_navTextColor global variable.
/// Adapt It uses the WxColour2Int() and Int2wxColour() helper functions to convert between the integer
/// and wx color enum symbols.
wxString szNavTextColor = _T("ColorOfNavigationText"); // stored in the App's m_navTextColor

/// The label that identifies the following string as the application's 
/// "DefaultCCTablePath". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_defaultTablePath global variable.
wxString szDefaultTablePath = _T("DefaultCCTablePath"); // stored in the App's m_defaultTablePath

/// The label that identifies the following string encoded number as the application's 
/// "FitWithinWindowFlag(Boolean)". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It no longer uses this variable.
wxString szFitFlag = _T("FitWithinWindowFlag(Boolean)"); // a relic, unused as of version 2.4.0

/// The label that identifies the following string encoded number as the application's 
/// "MarkersWrapStripsFlag(Boolean)". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_bMarkerWrapsStrip global variable.
wxString szMarkerWrapsFlag = _T("MarkersWrapStripsFlag(Boolean)"); // stored in the App's m_bMarkerWrapsStrip

/// The label that identifies the following string encoded number as the application's 
/// "BackupDocumentFlag". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_bBackupDocument global variable.
wxString szBackupDocument = _T("BackupDocumentFlag"); // stored in the App's m_bBackupDocument

// window position and size

/// The label that identifies the following string encoded number as the application's 
/// "TopLeftX". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_ptViewTopLeft.x global variable.
wxString szTopLeftX = _T("TopLeftX");

/// The label that identifies the following string encoded number as the application's 
/// "TopLeftY". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_ptViewTopLeft.y global variable.
wxString szTopLeftY = _T("TopLeftY");

/// The label that identifies the following string encoded number as the application's 
/// "WinSizeCX". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_szView.x global variable.
wxString szWSizeCX = _T("WinSizeCX");

/// The label that identifies the following string encoded number as the application's 
/// "WinSizeCY". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_szView.y global variable.
wxString szWSizeCY = _T("WinSizeCY");

// restoring earlier doc location

/// The label that identifies the following string as the application's 
/// "LastDocumentPath". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_lastDocPath global variable.
wxString szLastDocPath = _T("LastDocumentPath");

/// The label that identifies the following string encoded number as the application's 
/// "LastActiveSequenceNumber". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's nLastActiveSequNum global variable.
wxString szLastActiveSequNum = _T("LastActiveSequenceNumber");

/// The label that identifies the following string encoded number as the application's 
/// "IsMainWindowMaximized". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_bZoomed global variable.
wxString szZoomed = _T("IsMainWindowMaximized");

/// The label that identifies the following string as the application's 
/// "LastExportPath". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_lastExportPath global variable.
wxString szLastExportPath = _T("LastExportPath");

// print margins, etc

/// The label that identifies the following string encoded number as the application's 
/// "UseInchesForMeasuring". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_bIsInches global variable.
wxString szLoEnglishFlag = _T("UseInchesForMeasuring");

/// The label that identifies the following string encoded number as the application's 
/// "UsePortraitOrientation". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_bIsPortraitOrientation global variable.
wxString szUsePortraitOrientation = _T("UsePortraitOrientation");

/// The label that identifies the following string encoded number as the application's 
/// "TopPrintMargin". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_marginTop and m_marginTopMM global variable.
wxString szMarginTop = _T("TopPrintMargin");

/// The label that identifies the following string encoded number as the application's 
/// "BottomPrintMargin". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_marginBottom and m_marginBottomMM global variable.
wxString szMarginBottom = _T("BottomPrintMargin");

/// The label that identifies the following string encoded number as the application's 
/// "LeftPrintMargin". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_marginLeft and m_marginLeftMM global variable.
wxString szMarginLeft = _T("LeftPrintMargin");

/// The label that identifies the following string encoded number as the application's 
/// "RightPrintMargin". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_marginRight and m_marginRightMM global variable.
wxString szMarginRight = _T("RightPrintMargin");

/// The label that identifies the following string encoded number as the application's 
/// "LastUsedPageWidth". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_pageWidth and m_pageWidthMM global variable.
wxString szLastPageWidth = _T("LastUsedPageWidth");

/// The label that identifies the following string encoded number as the application's 
/// "LastUsedPageLength". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_pageLength and m_pageLengthMM global variable.
wxString szLastPageLength = _T("LastUsedPageLength");

/// The label that identifies the following string encoded number as the application's 
/// "PaperSizeCode". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_paperSizeCode global variable.
wxString szPaperSizeCode = _T("PaperSizeCode");

/// The label that identifies the following string encoded number as the application's 
/// "RTL_Layout". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_bRTL_Layout global variable.
wxString szRTL_Layout = _T("RTL_Layout");

// support for user-assignable SFM escape char

/// The label that identifies the following string encoded number as the application's 
/// "SFMescapeChar". This value is no longer used in Adapt I, but is retained for backward
/// compatibility of configuration files.
wxString szSFMescapechar = _T("SFMescapeChar");

/// The label that identifies the following string encoded number as the application's 
/// "SFMafterNewlines". Adapt It stores this value in the App's gbSfmOnlyAfterNewlines global variable.
wxString szSFMafterNewlines = _T("SFMafterNewlines");

#ifdef _RTL_FLAGS
// NR version support

/// The label that identifies the following string encoded number as the application's 
/// "SourceIsRTL". Adapt It stores this value in the App's m_bSrcRTL global variable.
/// The label is used only for the Unicode version.
wxString szRTLSource = _T("SourceIsRTL");

/// The label that identifies the following string encoded number as the application's 
/// "TargetIsRTL". Adapt It stores this value in the App's m_bTgtRTL global variable.
/// The label is used only for the Unicode version.
wxString szRTLTarget = _T("TargetIsRTL");

/// The label that identifies the following string encoded number as the application's 
/// "NavTextIsRTL". Adapt It stores this value in the App's m_bNavTextRTL global variable.
/// The label is used only for the Unicode version.
wxString szRTLNavText = _T("NavTextIsRTL");
#endif

// support for auto-capitalization

/// The label that identifies the following string as the application's 
/// "LowerCaseSourceLanguageChars". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_srcLowerCaseChars global variable.
wxString szLowerCaseSourceChars = _T("LowerCaseSourceLanguageChars");

/// The label that identifies the following string as the application's 
/// "UpperCaseSourceLanguageChars". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_srcUpperCaseChars global variable.
wxString szUpperCaseSourceChars = _T("UpperCaseSourceLanguageChars");

/// The label that identifies the following string as the application's 
/// "LowerCaseTargetLanguageChars". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this path in the App's m_tgtLowerCaseChars global variable.
wxString szLowerCaseTargetChars = _T("LowerCaseTargetLanguageChars");

/// The label that identifies the following string as the application's 
/// "UpperCaseTargetLanguageChars". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_tgtUpperCaseChars global variable.
wxString szUpperCaseTargetChars = _T("UpperCaseTargetLanguageChars");

/// The label that identifies the following string as the application's 
/// "LowerCaseGlossLanguageChars". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_glossLowerCaseChars global variable.
wxString szLowerCaseGlossChars = _T("LowerCaseGlossLanguageChars");

/// The label that identifies the following string as the application's 
/// "UpperCaseGlossLanguageChars". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_glossUpperCaseChars global variable.
wxString szUpperCaseGlossChars = _T("UpperCaseGlossLanguageChars");

/// The label that identifies the following string encoded value as the application's 
/// "AutoCapitalizationFlag". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's gbAutoCaps global variable.
wxString szAutoCapitalization = _T("AutoCapitalizationFlag");

/// The label that identifies the following string encoded value as the application's 
/// "SourceHasUpperCaseAndLowerCase". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's gbSrcHasUcAndLc global variable.
wxString szSrcHasUcAndLc = _T("SourceHasUpperCaseAndLowerCase");// added in wx version for Case page simplification

// version 3.x support for USFM and SFM Filtering

/// The label that identifies the following string encoded value as the application's 
/// "UseSFMarkerSet". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's gProjectSfmSetForConfig global variable.
wxString szUseSFMarkerSet = _T("UseSFMarkerSet");

/// The label that identifies the following string encoded value as the application's 
/// "UsfmOnly". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's gProjectSfmSetForConfig global enum variable.
wxString szUsfmOnly = _T("UsfmOnly");

/// The label that identifies the following string encoded value as the application's 
/// "PngOnly". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's gProjectSfmSetForConfig global enum variable.
wxString szPngOnly = _T("PngOnly");

/// The label that identifies the following string encoded value as the application's 
/// "UsfmAndPng". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's gProjectSfmSetForConfig global enum variable.
wxString szUsfmAndPng = _T("UsfmAndPng");

/// The label that identifies the following string encoded value as the application's 
/// "UseFilterMarkers". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's gProjectFilterMarkersForConfig global variable.
wxString szUseFilterMarkers = _T("UseFilterMarkers");

/// The label that identifies the following string encoded value as the application's 
/// "ChangeFixedSpaceToRegularSpace". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_bChangeFixedSpaceToRegularSpace global variable.
wxString szChangeFixedSpaceToRegSpace = _T("ChangeFixedSpaceToRegularSpace");

// version 2.4.0 support for user-defined font size in dialogs
/// The label that identifies the following string encoded value as the application's 
/// "FontSizeForDialogs(points)". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_dialogFontSize global variable.
wxString szFontSizeForDialogs = _T("FontSizeForDialogs(points)");

// support for named Bible book folders (plus "Other Texts" catchall folder) for storing docs
/// The label that identifies the following string encoded value as the application's 
/// "BookModeFlag". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_bBookMode global variable.
wxString szBookMode = _T("BookModeFlag");

/// The label that identifies the following string encoded value as the application's 
/// "BookIndexValue". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_bBookMode global variable.
wxString szBookIndex = _T("BookIndexValue");

// for saving the m_bSaveAsXML value in project config file
/// The label that identifies the following string encoded value as the application's 
/// "SaveAsXML". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_bSaveAsXML global  variable.
wxString szSaveAsXML = _T("SaveAsXML");

// rde: added support for calling EncConverters to pre-process a target word
// whm: the followng three wxString values can remain defined whether or not 
// the USE_SIL_CONVERTERS define is set:

/// The label that identifies the following string encoded value as the application's 
/// "SilConverterName". This value is written in the "Settings" part of the basic configuration 
/// file. Adapt It stores this value in the App's m_strSilEncConverterName global  variable.
wxString szSilConverterName = _T("SilConverterName");                     // string

/// The label that identifies the following string encoded value as the application's
/// "SilConverterDirectionForward". This value is written in the "Settings" part of the basic
/// configuration file. Adapt It stores this value in the App's m_bSilConverterDirForward global
/// variable.
wxString szSilConverterDirForward = _T("SilConverterDirectionForward");   // bool

/// The label that identifies the following string encoded value as the application's
/// "SilConverterNormalizeOutput". This value is written in the "Settings" part of the basic
/// configuration file. Adapt It stores this value in the App's m_eSilConverterNormalizeOutput global
/// variable.
wxString szSilConverterNormalize = _T("SilConverterNormalizeOutput");     // enum NormalizeFlags


/// The label that identifies the following string encoded value as the application's
/// "DoAdaptingBeforeGlossing_InVerticalEdit". This value is written in the "ProjectSettings" part of
/// the basic configuration file. Adapt It stores this value in the App's gbAdaptBeforeGloss global
/// variable.
wxString szDoAdaptingBeforeGlossing_InVerticalEdit = _T("DoAdaptingBeforeGlossing_InVerticalEdit");   // bool // vertical edit settings


/////////////////////////////////////////////////////////////////////////////
// CAdapt_ItApp construction

// initialize static class variable
bool CAdapt_ItApp::bLookAheadMerge = FALSE;

/// **** DO NOT PUT INITIALIZATIONS IN THE APP'S CONSTRUCTOR *****
/// **** ALL INITIALIZATIONS SHOULD BE DONE IN THE APP'S OnInit() METHOD *****
CAdapt_ItApp::CAdapt_ItApp()
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!! NOTE: DO NOT PLACE ANY APPLICATION INITIALIZATIONS HERE IN THE APP's 
	// CONSTRUCTOR!!! wxWidgets versions subsequent to 2.4.2 call this constructor
	// much earlier than version 2.4.2 and previous versions, and any initializations
	// that involve the wxWidgets library especially cannot be guaranteed to be
	// available for initialization here. Instead, DO ALL INITIALIZATION in the
	// App's OnInit() method.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}// end of CAdapt_ItApp constructor

/// **** ALL CLEANUP SHOULD BE DONE IN THE APP'S OnExit() METHOD ****
CAdapt_ItApp::~CAdapt_ItApp()
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!! NOTE: DO NOT PLACE ANY APPLICATION CLEANUP CODE HERE IN THE APP's 
	// DESTRUCTOR!!! wxWidgets versions subsequent to 2.4.2 call this destructor
	// much later than version 2.4.2 and previous versions, and any cleanup
	// that involve the wxWidgets library especially cannot be guaranteed to
	// work here. Instead, DO ALL CLEANUP in the App's OnExit() method.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a pointer to the CMainFrame object
/// \remarks
/// Called from: any part of the source code where we need to obtain a pointer to the application's
/// main frame.
////////////////////////////////////////////////////////////////////////////////////////////
CMainFrame* CAdapt_ItApp::GetMainFrame()
{
	wxASSERT(m_pMainFrame != NULL);
    return m_pMainFrame;
}


// The idea for this function was inspired by a blog by Julian Smart, wxWidgets developer at:
// http://wxwidgets.blogspot.com/2007/12/programming-for-eee-pc-with-wxwidgets.html This is a function
// to fit the dialog it is called on around the dialog's contents, but then it checks that the dialog
// will fit in a certain maxSize. If not, the size of the dialog is adjusted smaller to fit within the
// maxSize and, if a scrolled window is passed in scrolledWindow, scrolling is enabled in the
// scrolledWindow in the required orientation(s). The maxSize would normally be the display size of the
// computer the program is running on, but, it could be a wizard, property sheet, or other dialog which
// contains sub-windows/pages which we want to fit within that wizard, property sheet, or other dialog,
// and make these sub-windows/pages be scrollable if they would otherwise exceed the maxSize. The main
// purpose of this function is to prevent dialogs, wizards, property sheets, etc, from ending up with
// controls off the screen on computers that have a small resolution (i.e., 800 x 600 or smaller which
// is typical of the new generation of mini-notebook computers such as the Asus Eee PC, or the Aspire
// One, Dell Mini 9, etc).
bool CAdapt_ItApp::FitWithScrolling(wxDialog* dialog, wxScrolledWindow* scrolledWindow, wxSize maxSize)
{
    wxSizer* sizer = dialog->GetSizer();
    if (!sizer)
        return false;
        
    sizer->SetSizeHints(dialog);
        
    wxSize windowSize = dialog->GetSize();

    // Allow for caption size on wxWidgets < 2.9
#if defined(__WXGTK__) && !wxCHECK_VERSION(2,9,0)
    int allowExtraHeight = 30;
#else
    int allowExtraHeight = 0;
#endif
    int scrollBarSize = 20;

	// whm Note: The wxDisplay class can be used to determine the sizes and locations of displays
	// connected to the system.
	// The GetFromWindow() method "Returns the index of the display on which the given window lies. If 
	// the window is on more than one display it gets the display that overlaps the window the most. 
	// Returns wxNOT_FOUND if the window is not on any connected display."
	// The GetClientArea() method "Returns [as a wxRect] the client area of the display. The client 
	// area is the part of the display available for the normal (non full screen) windows, usually 
	// it is the same as GetGeometry but it could be less if there is a taskbar (or equivalent) on 
	// this display." 
	// The GetSize() is a method of wxRect that returns a size for the display (from the rect returned
	// by GetClientArea()).

    //wxSize displaySize = wxDisplay(wxDisplay::GetFromWindow(dialog)).GetClientArea().GetSize();
    wxSize displaySize = maxSize;

    bool resizeVertically = (windowSize.y >= (displaySize.y - allowExtraHeight));
    bool resizeHorizontally = (windowSize.x >= displaySize.x);
    
    if (resizeVertically || resizeHorizontally)
    {
        int scrollBarExtraX = 0, scrollBarExtraY = 0;
    
        if (scrolledWindow)
        {
            // Allow extra for a scrollbar, assuming we resizing in one direction only.
            if ((resizeVertically && !resizeHorizontally) && (windowSize.x < (displaySize.x - scrollBarSize)))
                scrollBarExtraX = scrollBarSize;
            if ((resizeHorizontally && !resizeVertically) && (windowSize.y < (displaySize.y - scrollBarSize)))
                scrollBarExtraY = scrollBarSize;

            scrolledWindow->SetScrollRate(resizeHorizontally ? 10 : 0, resizeVertically ? 10 : 0);
        }
            
        wxSize limitTo = windowSize + wxSize(scrollBarExtraX, scrollBarExtraY);
        if (resizeVertically)
            limitTo.y = displaySize.y - allowExtraHeight;
        if (resizeHorizontally)
            limitTo.x = displaySize.x;
            
        dialog->SetMinSize(limitTo);
        dialog->SetSize(limitTo);
        
        dialog->SetSizeHints( limitTo.x, limitTo.y, dialog->GetMaxWidth(), dialog->GetMaxHeight() );
    }

    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the path to the directory which contains all of the
///             localization subdirectories (es, fr, in, ru, zh_TW, etc) which contain .mo files
/// \remarks
/// Called from: the App's ChooseInterfaceLanguage().
/// Gets the path from which we expect to find the 2-letter ("xx") or 5-letter ("xx_XX") 
/// subdirectories that contain localization .mo files for Adapt It and/or the wxWidgets 
/// library strings. If the directory cannot be determined, or there are no subdirectories 
/// of the expected form containing at least one localization .mo file, an empty string
/// is returned.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetDefaultPathForLocalizationSubDirectories()
{
	// According to the wx docs:
	// Adapt It looks for its possible interface translations (localizations)
	// at the following locations, depending on the OS:
	// 
	// - On Windows, there appears to be no well established location for localization files. 
	// For example I looked at two wxWidgets based applications Poedit and Audacity.
	// The Poedit program places its localization files in: 
	// "C:\Program Files\Poedit\share\locale\<lang>\LC_MESSAGES\"
	// The Audacity program uses a simpler method, putting its localization files in:
	// C:\Program Files\Audacity\Languages\<lang>.
	// For Windows at least, I like the simplified way that Audacity does it. So, we add a 
	// "Languages" subdirectory so that the localization subdirectories and files will be installed
	// at:  "C:\Program Files\Adapt It WX\Languages\" or "C:\Program Files\Adapt It WX Unicode\Languages\"
	// and Adapt It on the Windows port will look there.
	// 
	// - On Unix, the localization files are installed in "<prefix>/share/locale" so Adapt It 
	// looks for localization <lang> subfolders and files in the /usr/share/locale/ subdirectory. The 
	// <lang> subfolders have an additional LC_MESSAGES subfolder which contains the adaptit.mo files.
	// 
	// - On Mac OS X, localization files are installed in the "<appname>.app/Contents/Resources/locale" 
	// bundle subdirectory, so Adapt It looks for localization <lang> subfolders and files in the 
	// AdaptIt.app/Contents/Resources/locale bundle subdirectory. The <lang> subfolders also have an
	// additional LC_MESSAGES subfolder which contains the AdaptIt.mo files.
	// In all cases, the <lang> subdirectory is named with the two (or five) letter short canonical 
	// name of the language from the old ISO639 standard. Within these individual <lang> subdirectories, 
	// the localization translations are contained in a compiled <appname>.mo file. Additionally there
	// should be a wxWidgets library file called wxstd.mo in the directory if that
	// language has also been localized for the wxWidgets library itself.
	//
//	// !!! testing only below
//#if wxCHECK_VERSION(2, 7, 0)
//	wxString resourcesDir,localizedResourcesDir;
//	wxString dataDir, localDataDir, documentsDir;
//	wxString userConfigDir, userDataDir, userLocalDataDir;
//	wxString executablePath;
//#ifndef __WXMSW__
//	wxString installPrefix;
//#endif
//	wxStandardPaths stdPaths;
//	resourcesDir = stdPaths.GetResourcesDir(); // GetResourcesDir() is new with wxWidgets 2.7.0
//	wxLogDebug(_T("The wxStandardPaths::GetResourcesDir()  = %s"),resourcesDir.c_str());
//	localizedResourcesDir = stdPaths.GetLocalizedResourcesDir(_T("es")); // GetLocalizedResourcesDir() is new with wxWidgets 2.7.0
//	wxLogDebug(_T("The wxStandardPaths::GetLocalizedResourcesDir(_T(\"es\")) = %s"),localizedResourcesDir.c_str());
//	dataDir = stdPaths.GetDataDir();
//	wxLogDebug(_T("The wxStandardPaths::GetDataDir() = %s"),dataDir.c_str());
//	localDataDir = stdPaths.GetLocalDataDir();
//	wxLogDebug(_T("The wxStandardPaths::GetLocalDataDir() = %s"),localDataDir.c_str());
//	documentsDir = stdPaths.GetDocumentsDir();
//	wxLogDebug(_T("The wxStandardPaths::GetDocumentsDir() = %s"),documentsDir.c_str());
//	userConfigDir = stdPaths.GetUserConfigDir();
//	wxLogDebug(_T("The wxStandardPaths::GetUserConfigDir() = %s"),userConfigDir.c_str());
//	userDataDir = stdPaths.GetUserDataDir();
//	wxLogDebug(_T("The wxStandardPaths::GetUserDataDir() = %s"),userDataDir.c_str());
//	userLocalDataDir = stdPaths.GetUserLocalDataDir();
//	wxLogDebug(_T("The wxStandardPaths::GetUserLocalDataDir() = %s"),userLocalDataDir.c_str());
//	executablePath = stdPaths.GetExecutablePath();
//	wxLogDebug(_T("The wxStandardPaths::GetExecutablePath() = %s"),executablePath.c_str());
//#ifndef __WXMSW__
//	installPrefix = stdPaths.GetInstallPrefix();
//	wxLogDebug(_T("The wxStandardPaths::GetInstallPrefix() = %s"),installPrefix.c_str());
//#endif
//#endif // #if wxCHECK_VERSION(2, 7, 0)
	// Test results: The wxStandardPaths::GetLocalizedResourcesDir() function returns the folder we want on each platform. 
	// Note: for Debug builds, the GetLocalizedResourcesDir() actually returns the parent directory of the directory in which the
	// application actually exists (on Windows at least).

	// The global m_executingAppPathName contains the application name, but I only need the executable
	// name part so I'll use the built-in wxApp
	// function GetAppName() instead.
	wxString appName;
	appName = GetAppName();
	
	// Determine the path to the localization subdirectory where any localization subdirectories would be located.
	// Note: Our function FindAppPath() determined the most likely path where the Adapt It executable
	// program is located and stored that path in m_appInstallPathOnly. 
	wxString localizationFilePath;
	wxString pathToLocalizationFolders;
#ifdef __WXMAC__
	// On the Mac appName is "Adapt It"
	// Set a suitable default localizationFilePath for the Mac.
	// There does not appear to be a wxStandardPaths method which gives us the path for locating the
	// <lang> localization subdirectories so we'll just hard code it for the Mac.
	// Note: The actual full path and name on the Mac for a given <lang> is: 
	// "Adapt It.app/Contents/Resources/locale/<lang>/LC_MESSAGES/Adapt It.mo"
	localizationFilePath += appName + _T(".app/Contents/Resources/locale"); // the path separator is added by the caller
	pathToLocalizationFolders = localizationFilePath;
#endif

#ifdef __WXGTK__
	// On Linux appName is "adaptit"
	// Set a suitable default localizationFilePath for Linux.
	// There does not appear to be a wxStandardPaths method which gives us the path for locating the
	// <lang> localization subdirectories so we'll just hard code it for Ubuntu Linux.
	// Note: The actual full path and name for a given <lang> is "/usr/share/locale/<lang>/LC_MESSAGES/adaptit.mo"
	localizationFilePath = _T("/usr/share/locale"); // the path separator is added by the caller
	pathToLocalizationFolders = localizationFilePath;
#endif //__WXGTK__

#ifdef __WXMSW__
	// Windows uses the m_appInstallPathOnly
	localizationFilePath = m_appInstallPathOnly; //m_setupFolder;
	if (wxDir::Exists(localizationFilePath + PathSeparator + _T("Languages")))
	{
		pathToLocalizationFolders = localizationFilePath + PathSeparator + _T("Languages");
	}
	else
	{
		pathToLocalizationFolders = localizationFilePath;
	}
#endif
	wxLogDebug(_T("pathToLocalizationFolders = %s"),pathToLocalizationFolders.c_str());
	return pathToLocalizationFolders;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the path to the directory which contains the XML
///             control files AI_USFM.xml and books.xml
/// \remarks
/// Called from: the App's OnInit();
/// Gets the path where we expect to find the AI_USFM.xml and books.xml files. Adapt It
/// reads and parses these xml files at startup. 
/// If the directory cannot be determined an empty string is returned.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetDefaultPathForXMLControlFiles()
{
	// Adapt It looks for its possible interface translations (localizations)
	// at the following locations, depending on the OS:
	// - On Windows, there appears to be no well established location for XML control files. 
	// For Windows these XML files will be installed together with the executable program at:  
	// "C:\Program Files\Adapt It WX\ or C:\Program Files\Adapt It WX Unicode\" 
	// and Adapt It on the Windows port will look there.
	// - On Unix, the XML control files are installed in "/usr/share/adaptit" so Adapt It 
	// on Linux looks for them there.
	// - On Mac OS X, localization files are installed in the "<appname>.app/Contents/Resources" 
	// bundle subdirectory, so Adapt It on the Mac looks for them there.
	//
	// Note: the wxStandardPaths::GetDataDir() could possibly be used for determining this path
	// but, since I'm not sure if it existed before version 2.7.0 of the wxWidgets library, I'll
	// hard code the paths we expect here.
	
	wxString appName;
	appName = GetAppName();
	
	wxString pathToXMLFolders;
#ifdef __WXMAC__
	// On the Mac appName is "Adapt It"
	// Set a suitable default path for the xml files on the Mac.
	pathToXMLFolders += appName + _T(".app/Contents/Resources"); // the path separator is added by the caller
#endif

#ifdef __WXGTK__
	// On Linux appName is "adaptit"
	// Set a suitable default path for the xml files on Ubuntu Linux.
	pathToXMLFolders = _T("/usr/share/locale"); // the path separator is added by the caller
#endif //__WXGTK__

#ifdef __WXMSW__
	// Windows uses the m_appInstallPathOnly, the same place the program file is installed
	pathToXMLFolders = m_appInstallPathOnly;
#endif
	wxLogDebug(_T("pathToXMLFolders = %s"),pathToXMLFolders.c_str());
	return pathToXMLFolders;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the path to the directory which contains the HTML Help
///             files for the appropriate platform
/// \remarks
/// Called from: the App's OnInit();
/// Gets the path where we expect to find the HTML Help files. 
/// If the directory cannot be determined an empty string is returned.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetDefaultPathForHelpFiles()
{
	// Adapt It looks for its HTML Help files at the following locations, depending on the OS:
	// 
	// - On Windows, there appears to be no well established location for HTML Help files. 
	// For Windows the Html Help files will be installed together with the executable program at:  
	// "C:\Program Files\Adapt It WX\ or C:\Program Files\Adapt It WX Unicode\" 
	// and Adapt It on the Windows port will look there.
	// 
	// - On Unix, the Html Help files are installed in "/usr/share/adaptit/help/" which in turn 
	// may contain a "common/" subdirectory with .gif and .css files (pointed to by the help files).
	// The .../help/ subdirectory may also contain one or more <lang>/ folders which contain the
	// .html .hhp .hhc help files for Adapt It.
	// 
	// - On Mac OS X, Html Help files are installed in the "AdaptIt.app/Contents/SharedSupport/"
	// folder of the bundle subdirectory.
	//
	// Note: the wxStandardPaths class apparently doesn't have a method for locating the help files
	// on the various ports.
	
	wxString appName;
	appName = GetAppName();
	m_htbHelpFileName = appName + _T(".htb");
	
	wxString pathToHtmlHelpFiles;
#ifdef __WXMAC__
	// On the Mac appName is "Adapt It"
	// Set a suitable default path for the Html Help files on the Mac.
	pathToHtmlHelpFiles += appName + _T(".app/Contents/SharedSupport"); // the path separator is added by the caller
#endif

#ifdef __WXGTK__
	// On Linux appName is "adaptit"
	// Set a suitable default path for the Html Help files on Ubuntu Linux.
	pathToHtmlHelpFiles = _T("/usr/share/adaptit/help"); // the path separator is added by the caller
#endif //__WXGTK__

#ifdef __WXMSW__
	// Windows uses the m_appInstallPathOnly, the same place the program file is installed
	pathToHtmlHelpFiles = m_appInstallPathOnly;
#endif
	wxLogDebug(_T("pathToHtmlHelpFiles = %s m_htbHelpFileName = %s"),pathToHtmlHelpFiles.c_str(),m_htbHelpFileName.c_str());
	return pathToHtmlHelpFiles;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      initialPath  -> the path where subdirectories are expected/gathered into arrayStr
/// \param      arrayStr     <- wxArrayString to receive the list of subdirectories
/// \remarks
/// Called from: the App's PathHas_mo_LocalizationFile() and CChooseLanguageDlg::InitDialog().
/// Gets a list in the form of a wxStringArray of the subdirectories that exist in the 
/// initialPath directory. GetListOfSubDirectories does not empty arrayStr before
/// adding directories to the array, so it is up to the caller to empty the array if needed.
/// If no subdirectories are found arrayStr will not be changed.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::GetListOfSubDirectories(const wxString initialPath, wxArrayString &arrayStr)
{
	if (initialPath.IsEmpty())
		return;
	wxString filename,saveDir;
	saveDir = ::wxGetCwd();
	::wxSetWorkingDirectory(initialPath);
	wxDir dir(initialPath);

	//wxLogNull logNo;	// eliminates any spurious messages from the system while reading read-only folders/files
	wxASSERT(dir.IsOpened());
	bool bGotOne = dir.Open(initialPath) && dir.GetFirst(&filename, _T(""), wxDIR_DIRS); // wxDIR_DIRS gets only directories
	//wxLogDebug(_T("List of subDirs:"));
	while (bGotOne)
	{
		//wxLogDebug(_T("%s"), filename.c_str());
		arrayStr.Add(filename);
		bGotOne = dir.GetNext(&filename);
	}
	// restore the previous cwd
	::wxSetWorkingDirectory(saveDir);
	// end of scope for wxLogNull
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxLanguage value if a language value for fullLangName could be found otherwise 
///             wxLANGUAGE_UNKNOWN
/// \param      fullLangName  -> a wxString representing the common language name (Description)
/// \remarks
/// Called from: the App's ChooseInterfaceLanguage().
/// Attempts to get the wxLanguage value represented by the incoming fullLangName value. It first 
/// searches the langsKnownToWX[] array comparing the langsKnownToWX[].name (Description) with fullLangName. If it 
/// finds a name (Description) matching fullLangName it returns its wxLanguage value. Since the
/// string values coming in fullLangName were derived originally from 
////////////////////////////////////////////////////////////////////////////////////////////
wxLanguage CAdapt_ItApp::GetLanguageFromFullLangName(const wxString fullLangName)
{
	// Scan through the langsKnownToWX[] array of LangInfo structs for the full language name
	// as found in langsKnownToWX[].fullName. If found return the corresponding langsKnownToWX[].code.
	int ct = 0;
	while (langsKnownToWX[ct].fullName != NULL)
	{
		if (langsKnownToWX[ct].fullName == fullLangName)
		{
			// we've found the shortName in our langsKnownToWX[] array, so return its langsKnownToWX[].code
			return langsKnownToWX[ct].code;
		}
		ct++;
	}
	// If we get here we've searched the whole array and not found dirStr or its prefix in langsKnownToWX[].
	return wxLANGUAGE_UNKNOWN;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxLanguage value if the language value for dirStr could be found otherwise 
///             wxLANGUAGE_UNKNOWN
/// \param      dirStr        -> the wxString representing the short name of the localization subdirectory 
/// \param      fullLangName  <- a wxString to receive the full language name langsKnownToWX[].fullName
/// \remarks
/// Called from: the App's ChooseInterfaceLanguage().
/// Attempts to get the wxLanguage value represented by the incoming dirStr value. It first searches 
/// the langsKnownToWX[] array comparing the langsKnownToWX[].shortName with dirStr. If it finds a matching shortName
/// matching dirStr it returns its wxLanguage value and sets the fullLangName to the full
/// language name in langsKnownToWX[].fullName. If it does not find a match on the first run, it checks to see 
/// if shortName is of the xx_XX form. If so, it strips off the _XX part of the string and uses just 
/// the initial xx part of the canonical short name for comparison with dirStr. If it finds a match 
/// for the basic language xx part, we consider that to be an acceptable substitute localization. 
////////////////////////////////////////////////////////////////////////////////////////////
wxLanguage CAdapt_ItApp::GetLanguageFromDirStr(const wxString dirStr, wxString &fullLangName)
{
	// Scan through the langsKnownToWX[] array of LangInfo structs for the dirStr
	int ct = 0;
	while (langsKnownToWX[ct].fullName != NULL)
	{
		if (langsKnownToWX[ct].shortName == dirStr)
		{
			// we've found the shortName in our langsKnownToWX[] array, so assign fullLangName
			// the string value in langsKnownToWX[ct].fullName
			fullLangName = langsKnownToWX[ct].fullName;
			return langsKnownToWX[ct].code;
		}
		ct++;
	}

	// It may be that dirStr is of the form xx_XX and we might recognize the language, but not the
	// country/region XX. Such a localization would likely be acceptable or at least better than none. 
	// So, in case dirStr is of the form xx_XX we search the langsKnownToWX[] array again looking for just the
	// xx part.
	ct = 0;
	while (langsKnownToWX[ct].fullName != NULL)
	{
		wxString str;
		str = langsKnownToWX[ct].shortName;
		if (str.Length() > 2 && str.GetChar(2) == _T('_'))
		{
			str = str.Mid(0,2);
		}
		if (str == dirStr)
		{
			// we've found the xx part of our shortName in our langsKnownToWX[] array, so assign 
			// fullLangName the string value in langsKnownToWX[ct].fullName
			fullLangName = langsKnownToWX[ct].fullName;
			return langsKnownToWX[ct].code;
		}
		ct++;
	}
	// If we get here we've searched the whole array and not found dirStr or its prefix in langsKnownToWX[].
	return wxLANGUAGE_UNKNOWN;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if the dirPath represents a subdirectory containing a child or grandchild
///             localization directory(s) which contain an <appName>.mo file
/// \param      dirPath         -> the path to look for the <lang> localization subfolder
/// \param      subFolderName = -> the name of a specific subfolder to contain <appName>.mo
/// \remarks
/// Called from: the App's LocalizationFilesExist() and CChooseLanguageDlg::OnBrowseForPath(). 
/// Determines if dirPath has one or more child or grandchild subfolders which contain an 
/// <appName>.mo file. 
/// The incoming dirPath would normally be the a path pointing to the "Languages" directory (on 
/// Windows); or the /usr/share/locale/ directory on Linux; or /<appName.app/Contents/Resources
/// directory on the Mac. These directory locations should contain at least one localization 
/// <lang> subfolder containing an <appName.mo file. If subFolderName is not a null string, 
/// this function will only return TRUE if a subfolder exists at dirPath with the subFolderName. 
/// If subFolderName is a null string the function does not check the names of the localization 
/// subfolders, but only checks to see if an <appName>.mo file can be found in at least one 
/// such subfolder.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::PathHas_mo_LocalizationFile(wxString dirPath, wxString subFolderName)
{
	// TODO: Check of LC_MESSAGES as a child folder for each <lang> on dirPath under non-Windows ports.
	if (dirPath.IsEmpty())
		return FALSE;

	wxString appName;
	appName = GetAppName();

	wxArrayString subDirList;
	subDirList.Clear();
	GetListOfSubDirectories(dirPath,subDirList);
	
	wxLogNull logNo;	// eliminates any spurious messages from the system while reading read-only folders/files

	int ct;
	for (ct = 0; ct < (int)subDirList.GetCount(); ct++)
	{
		bool bNamesMatch = TRUE; // assume match unless subFolderName parameter has a non-null string
		if (!subFolderName.IsEmpty())
		{
			wxString subDir = subDirList.Item(ct);
			wxString subFldr = subFolderName;
#ifdef __WXMSW__
			subDir.MakeLower();
			subFldr.MakeLower();
#endif
			if (subFldr != subDir)
			{
				bNamesMatch = FALSE;
			}
		}

		dirPath = dirPath + PathSeparator + subDirList.Item(ct);
		// check for existence of "LC_MESSAGES" subfolder within dirPath, and if it exists, search
		// within it
		if (wxDir::Exists(dirPath + PathSeparator + _T("LC_MESSAGES")))
			dirPath = dirPath + PathSeparator + _T("LC_MESSAGES");
		wxDir dPath(dirPath);
		// whm Note: with wxDir we must call .Open() before enumerating files or calling IsOpen()! Otherwise it asserts on wxGTK!
		if (dPath.Open(dirPath) && bNamesMatch && dPath.HasFiles(_T("*.mo")) && dPath.HasFiles(appName + _T(".mo")))
		{
			return TRUE;
		}
	}
	// if we make it here none were found
	// end of scope for wxLogNull
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if locale and localizations were successfully created, otherwise FALSE
/// \param      shortLangName -> the short (Canonical) name of the language/locale
/// \param      longLangName  -> the long (Description) name of the language/locale
/// \param      pathPrefix    -> the path prefix where localization subfolders and files are found
/// \remarks
/// Called from: the App's OnInit() and CChooseLanguageDlg::OnOK(). Deletes any existing wxLocale
/// object and creates a new one on the heap assigned to m_pLocale. Adds the pathPrefix to the
/// new locale and calls AddCatalog to load any new localization.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::InitializeLanguageLocale(wxString shortLangName, wxString longLangName, wxString pathPrefix)
{
	// delete any existing locale object
	if (m_pLocale)
		delete m_pLocale;

	bool bLoadOK = TRUE;
	wxLogNull nolog; // avoid spurious messages from the system
	m_pLocale = new wxLocale(longLangName, shortLangName, _T(""), TRUE, TRUE);
	if (!pathPrefix.IsEmpty())
		m_pLocale->AddCatalogLookupPathPrefix(pathPrefix);
	if (!m_pLocale->AddCatalog(GetAppName()))
	{
		bLoadOK = FALSE;
	}
	if (shortLangName.Find(_T("en_")) == 0 // we have shortLangName with en_ at the start of the name
		|| (shortLangName.Length() == 2 && shortLangName == _T("en"))) // or, we have the two-letter name en
	{
		// For our default English locale we don't need a loaded string catalog table, we just use the
		// program strings, so don't check m_pLocale->IsLoaded() for English.
		; // do nothing
	}
	else
	{
		// For a non-default non-English localization we can check if the catalog is loaded OK
		if(!m_pLocale->IsLoaded(GetAppName()))
		{
			bLoadOK = FALSE;
		}
	}
	return bLoadOK;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: the App's ProcessUILanguageInfoFromConfig() and ChangeUILanguage(). Stores the user's choice
/// of interface language (using the members of the CurrLocalizationInfo struct) in the global configuration 
/// (the Registry on Windows, or a hidden file in the user's folder on Linux/Mac.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::SaveCurrentUILanguageInfoToConfig()
{
	// This function uses the data currently in the global struct currLocalizationInfo

	// save current CurrLocalizationInfo struct's member information in registry/hidden 
	// settings file
	wxString oldPath = m_pConfig->GetPath(); // is always absolute path "/Recent_File_List"
	m_pConfig->SetPath(_T("/Settings"));
    
	m_pConfig->Write(_T("ui_language"), (long)currLocalizationInfo.curr_UI_Language);
    m_pConfig->Write(_T("ui_language_code"), currLocalizationInfo.curr_shortName);
    m_pConfig->Write(_T("ui_language_name"), currLocalizationInfo.curr_fullName);
#ifdef _UNICODE
    m_pConfig->Write(_T("ui_language_path_unicode"), currLocalizationInfo.curr_localizationPath);
#else
    m_pConfig->Write(_T("ui_language_path"), currLocalizationInfo.curr_localizationPath);
#endif
	
	wxLogDebug(_T("Writing to m_pConfig:\n   curr_UI_Language = %d\n   curr_shortName = %s\n   curr_fullName = %s\n   curr_localizationPath = %s"),
		currLocalizationInfo.curr_UI_Language,
		currLocalizationInfo.curr_shortName.c_str(),
		currLocalizationInfo.curr_fullName.c_str(),
		currLocalizationInfo.curr_localizationPath.c_str());

	// restore the oldPath back to "/Recent_File_List"
	m_pConfig->SetPath(oldPath);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: CChooseLanguageDlg::OnOK(). Stores a user defined interface language in the 
/// global configuration (the Registry on Windows, or a hidden file in the user's folder on 
/// Linux/Mac. User defined languages may be stored in registry/hidden settings keys of the form 
/// user_defined_language_n, where n may be an integer from 0 through 8. This function uses 
/// the incoming parameter data to create a composite unix-like string delimited by 
/// colons (:), and saves this composit string to one of 9 keys in the Registry or hidden 
/// settings file. Up to 9 user defined languages can be remembered in this way, using keys 
/// with names such as user_defined_language_0, user_defined_language_1, and so on up 
/// through user_defined_language_8. This function first checks to see if the incoming user 
/// defined language was previously saved in one of the user_defined_language_n keys. If so,
/// only any updated path is saved there since the short and full language names will be
/// unchanged. If the incoming language is not already stored, the first available key that 
/// has not yet been assigned a string is used. If all keys are currently filled with user 
/// defined language info, the oldest string is replaced by the new language composite string.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::SaveUserDefinedLanguageInfoStringToConfig(int &wxLangCode, const wxString shortName, const wxString fullName, const wxString localizationPath)
{
	// A composite unix string for a user defined language such as Tok Pisin might look like
	// this:
	// 231:tpi:Tok Pisin:C:\Program Files\Adapt It\Languages
	// The first 3 fields of the unix string are parsed using the : delimiter. The 4th field
	// contains all the remaining characters of the string (including any colon chars that
	// may exist in the path (i.e., the : after the volume C).

	wxString oldPath = m_pConfig->GetPath(); // is always absolute path "/Recent_File_List"
	m_pConfig->SetPath(_T("/Settings"));

	// Take stock of the user_defined_language_n entry keys the the registry/hidden settings file. 
	// The keyArray[] array will contain what is stored there.
	int ct;
	const int nKeys = 9;
	wxArrayString keyArray;
	keyArray.Alloc(9);
	wxArrayString foundCodesArray;
	bool bKeysPresent = FALSE;
	bool bKeyMissing = FALSE;
	bool bAllKeysAssigned = FALSE;
	for (ct = 0; ct < nKeys; ct++)
	{
		wxLogNull logNo;	// eliminates spurious message from the system: "Can't read value of key
							//'HKCU\Software\Adapt_It_WX\Settings' Error" [valid until end of this block]
		wxString str;
#ifdef _UNICODE
		str << _T("user_defined_language_u_") << ct;
#else
		str << _T("user_defined_language_") << ct;
#endif
		bool valReadOK = TRUE;
		wxString tempStr;
		valReadOK = m_pConfig->Read(str, &tempStr);
		keyArray.Add(tempStr);
		if (valReadOK && keyArray[ct] == _T("[UNASSIGNED]"))
		{
			bAllKeysAssigned = FALSE;
		}
		if (valReadOK && keyArray[ct] != _T("[UNASSIGNED]"))
		{
			bKeysPresent = TRUE;
			int pos;
			pos = keyArray[ct].Find(_T(':'));
			wxASSERT(pos > 0 && pos <= 3); // the language code number should represented as a three letter string between 231 and 255
			wxString codeStr = keyArray[ct].Left(pos);
			foundCodesArray.Add(codeStr);
		}
		if (!valReadOK)
		{
			bKeyMissing = TRUE;
			keyArray[ct] = _T("[UNASSIGNED]");	// assign "[UNASSIGNED]" string to any keyArray element for which 
												// there is no key
		}
		// end of wxLogNull logNo scope
	}

	// Note: our keyArray[] at this point should always contain exactly 9 items regardless of 
	// whether some or all of the keys were present when SaveUserDefinedLanguageInfoStringToConfig()
	// was called.

	// Sort the array. Since the wxLanguage number (as string) is first in compositeStr, we sort the array
	// so that unassigned keys are moved to the end where we want them.
	//keyArray.Sort();  // sorts the array in ascending order (the unassigned keys get sorted to the end of the list)
	
	// Check whether the language represented by our compositeStr has been assigned previously; we
	// don't want to store duplicate user languages, but we do want to update the path part of the
	// string if the user language already exists with a different path.
	// Make a search string of just the shortName and fullName including the surrounding colons.
	int nCodeAssigned = -1;
	wxString subStr;
	subStr << _T(':') << shortName << _T(':') << fullName << _T(':'); // for Tok Pisin this would be ":tpi:Tok Pisin:"
	bool bAlreadyAssigned = FALSE;
	for (ct = 0; ct < nKeys; ct++)
	{
		if (keyArray[ct].Find(subStr) != -1)
		{
			bAlreadyAssigned = TRUE;
			int pos;
			pos = keyArray[ct].Find(_T(':'));
			wxASSERT(pos > 0 && pos <= 3); // the language code number should represented as a three letter string between 231 and 255
			wxString codeStr = keyArray[ct].Left(pos);
			nCodeAssigned = wxAtoi(codeStr);
			// Update the path part (it may have been changed by the user).
			wxString tempStr = keyArray[ct];
			int posPath = pos + subStr.Length();
			tempStr = tempStr.Left(posPath);
			tempStr = tempStr + localizationPath; // use the incoming (possibly updated) path
			keyArray[ct] = tempStr; // update the keyArray item
			break;
		}
	}

	if (nCodeAssigned == -1)
	{
		// This language code was not assigned previously. If other user defined languages were
		// previously assigned, we assign this one the next available number in the sequence. If no
		// other user defined languages were previously assigned, we start with wxLANGUAGE_USER_DEFINED
		// + 1.
		if (bAllKeysAssigned)
		{
            // All 9 available user_defined_language_n keys have been previously assigned, so we remove
			// the oldest by copying keyArray[1] to keyArray[0], keyArray[2] to keyArray[1], etc., all
			// the way through keyArray[9] copied to keyArray[8]. Then we assign make this newest key
			// "[UNASSIGNED]" so that it will get assigned the current language in the routine below
			// (where it will get assigned the next numerical value of 240). This would ordinarily leave
			// the numerical ordering 232 through 240. At the next running of the application the App's
			// OnInit() calls ProcessUILanguageInfoFromConfig() which will detect this situation and
			// will renumber the user defined languages before calling AddLangauge() to add them to the
			// wxLanguage database.
			for (ct = 0; ct < nKeys - 1; ct++)
			{
				keyArray[ct] = keyArray[ct + 1];
			}
			keyArray[nKeys] = _T("[UNASSIGNED]");
			int nCount = foundCodesArray.GetCount();
			wxASSERT(nCount > 0);
			foundCodesArray.Sort();
			wxString highestCodeStr = foundCodesArray[nCount - 1];
			nCodeAssigned = wxAtoi(highestCodeStr) + 1; // get the next number beyond the value for highestCodeStr
			//nCodeAssigned = wxLANGUAGE_USER_DEFINED + 1 + nKeys; // should be 240
		}
		else if (bKeysPresent)
		{
			// should be at least one slot available
			int nCount = foundCodesArray.GetCount();
			wxASSERT(nCount > 0);
			foundCodesArray.Sort();
			wxString highestCodeStr = foundCodesArray[nCount - 1];
			nCodeAssigned = wxAtoi(highestCodeStr) + 1; // get the next number beyond the value for highestCodeStr
		}
		else
		{
			nCodeAssigned = wxLANGUAGE_USER_DEFINED + 1;
		}
	}
	wxASSERT(nCodeAssigned >= wxLANGUAGE_USER_DEFINED+1 && nCodeAssigned <= 255);
	wxLangCode = nCodeAssigned;
	
	// Assign this language, but only if it has not already been assigned.
	if (!bAlreadyAssigned)
	{
		// Make a composite unix-like string from the parameters
		wxString compositeStr;
		compositeStr << wxLangCode << _T(':') << shortName << _T(':') << fullName << _T(':') << localizationPath;

		// Assign compositeStr to the first empty array element in keyArray[].
		bool bAssigned = FALSE;
		for (ct = 0; ct < nKeys; ct++)
		{
			// if key is unassigned put the compositeStr there
			if (keyArray[ct] == _T("[UNASSIGNED]"))
			{
				keyArray[ct] = compositeStr;
				bAssigned = TRUE;
				break;
			}
		}

		//// check to see if we need to make space in the array for this compositeStr.
		//if (!bAssigned)
		//{
		//	// The composite string didn't get assigned so that means the array is full, so 
		//	// we can assign the current compositeStr to the last element which should be 
		//	// the oldest.
		//	keyArray[8] = compositeStr;
		//}
	}
	
	// Sort the array again since we may have added or inserted something.
	//keyArray.Sort(); // sorts the array in ascending order (the unassigned keys get sorted to the end of the list)
	
	// Rewrite all 9 keys from keyArray[] back to the registry/hidden settings file. This is 
	// done even if we did not add a compositeStr language, but, since we did sort the keyArray[], 
	// it may have changed the order.
	int ctkey;
	for (ctkey = 0; ctkey < nKeys; ctkey++)
	{
		wxString str;
#ifdef _UNICODE
		str << _T("user_defined_language_u_") << ctkey;
#else
		str << _T("user_defined_language_") << ctkey;
#endif
		m_pConfig->Write(str, keyArray[ctkey]);
	}

	// restore the oldPath back to "/Recent_File_List"
	m_pConfig->SetPath(oldPath);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: CChooseLanguageDlg::OnOK(). This function searches for a user_defined_language_n 
/// key using the incoming parameter data. If the key is found, it effectively removes the key 
/// value by making it a null string. We also sort the user_defined_language_n keys and write them
/// back to the registry/hidden settings file so that they are always kept in numerical order.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::RemoveUserDefinedLanguageInfoStringFromConfig(const wxString shortName, const wxString fullName)
{
	// The string key values associated with the user_defined_language_n keys are a composite 
	// unix-like string delimited by colons (:). There are 9 keys in the registry or hidden 
	// settings file, so up to 9 user defined languages can be remembered there.
	// A composite unix string for a user defined language such as Tok Pisin might look like
	// this:
	// 231:tpi:Tok Pisin:C:\Program Files\Adapt It\Languages
	// The first 3 fields of the unix string are parsed using the : delimiter. The 4th field
	// is assumed to compose all the remaining characters of the localization path, including 
	// any colon chars that may exist in the path after the volume label, i.e.,  C:\....

	// Read any existing strings associated with user_defined_language_n keys into an array
	// called keyArray[].
	wxString oldPath = m_pConfig->GetPath(); // is always absolute path "/Recent_File_List"
	m_pConfig->SetPath(_T("/Settings"));

	int ct;
	const int nKeys = 9;
	wxArrayString keyArray;
	bool bKeysPresent = FALSE;
	bool bKeyMissing = FALSE;
	bool bAllKeysAssigned = FALSE;
	for (ct = 0; ct < nKeys; ct++)
	{
		wxLogNull logNo;	// eliminates spurious message from the system: "Can't read value of key
							//'HKCU\Software\Adapt_It_WX\Settings' Error" [valid until end of this block]
		wxString str;
#ifdef _UNICODE
		str << _T("user_defined_language_u_") << ct;
#else
		str << _T("user_defined_language_") << ct;
#endif
		bool valReadOK = TRUE;
		wxString tempStr;
		valReadOK = m_pConfig->Read(str, &tempStr);
		keyArray.Add(tempStr);
		if (valReadOK && keyArray[ct] == _T("[UNASSIGNED]"))
		{
			bAllKeysAssigned = FALSE;
		}
		if (valReadOK && keyArray[ct] != _T("[UNASSIGNED]"))
		{
			bKeysPresent = TRUE;
		}
		if (!valReadOK)
		{
			bKeyMissing = TRUE;
			keyArray[ct] = _T("[UNASSIGNED]");	// assign "[UNASSIGNED]" string to any keyArray element for which 
												// there is no key
		}
		// end of wxLogNull logNo scope
	}
	// Note: our keyArray[] at this point should always contain exactly 9 items regardless of 
	// whether some or all of the keys were present when SaveUserDefinedLanguageInfoStringToConfig()
	// was called.

	if (!bKeysPresent)
	{
		// there are no user_defined_language_n keys in the registry/hidden settings file, so we have
		// nothing to remove, so we quietly return to the caller.
		return;
	}
	else
	{
		// Scan through our keyArray[] parsing the composite unix-like string elements
		// to find the string key containing the language info that we want to remove.
		// Remove it by assigning the "[UNASSIGNED]" string to the key.
		// Make a search string of just the shortName and fullName including the 
		// surrounding colons.
		wxString subStr;
		subStr << _T(':') << shortName << _T(':') << fullName << _T(':'); // for Tok Pisin this would be ":tpi:Tok Pisin:"
		bool bFound = FALSE;
		for (ct = 0; ct < nKeys; ct++)
		{
			if (keyArray[ct].Find(subStr) != -1)
			{
				bFound = TRUE;
				keyArray[ct] = _T("[UNASSIGNED]");
				break;
			}
		}

		wxASSERT(bFound);
		
		// Note: At the next running of the app, OnInit() calls ProcessUILanguageInfoFromConfig() function
		// which reads all the registry/hidden settings file information and cleans it up if necessary,
		// this cleanup includes reordering of wxLanguage values before calling AddLanguage(), and
		// insuring that any [UNASSIGNED] keys are placed last in the list.

		// Sort the keyArray.
		// Sort the array. Since the wxLanguage number (as string) is first in compositeStr, we sort the array
		// so that unassigned keys are moved to the end where we want them.
		//keyArray.Sort(); // sorts the array in ascending order (the unassigned keys get sorted to the end of the list)
		
		// Finally, write the 9 keys back to the registry
		for (ct = 0; ct < nKeys; ct++)
		{
			wxString str;
#ifdef _UNICODE
			str << _T("user_defined_language_u_") << ct;
#else
			str << _T("user_defined_language_") << ct;
#endif
			m_pConfig->Write(str, keyArray[ct]);
		}
	}
	
	// restore the oldPath back to "/Recent_File_List"
	m_pConfig->SetPath(oldPath);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     the CurrLocalizationInfo struct with its members populated from m_pConfig info
/// \remarks
/// Called from: the App's OnInit(). Retrieves any previously stored user defined languages from
/// m_pConfig using the registry (Windows)/hidden settings file (Linux/Mac), and adds the language 
/// info to the current locale with wxLocale::AddLanguage(). Then it retrieves the user's current 
/// localization information stored in the m_pConfig and returns it to the caller. If no previous 
/// interface has been chosen (i.e., on first run of Adapt It), the string members of the 
/// CurrLocalizationInfo struct being returned are set to null strings and the curr_UI_Language 
/// is set to wxLANGUAGU_UNKNOWN.
////////////////////////////////////////////////////////////////////////////////////////////
CurrLocalizationInfo CAdapt_ItApp::ProcessUILanguageInfoFromConfig()
{
	// Some wx example code uses the following:
	// wxString lng = wxConfig::Get()->Read(_T("ui_language"));
	// whm note: The above wxConfig->Get()... call creates a memory leak in our case since 
	// it does not make it possible to delete the object created in the wxConfig->Get() call. 
	// We want to get the ui_language data in our own m_pConfig object anyway.
	// So we use our m_pConfig object which was created in OnInit() and deleted in OnExit()
    
	// Retrieve the interface language information from m_pConfig if it exists.
	wxString oldPath = m_pConfig->GetPath(); // is always absolute path "/Recent_File_List"
	m_pConfig->SetPath(_T("/Settings"));

	wxLogNull logNo;	// eliminates spurious message from the system: "Can't read value of key
						//'HKCU\Software\Adapt_It_WX\Settings' Error" [valid until end of this block]

	// First, retrieve the current localization info
	CurrLocalizationInfo currLocInfo; // populate a local struct which we return to caller
	// initialize currLocInfo to some recognizable defaults since m_pConfig::Read() below
	// will not change the value unless it is found in the registry
	currLocInfo.curr_UI_Language = wxLANGUAGE_UNKNOWN; // value is 1
    currLocInfo.curr_shortName.Empty();
    currLocInfo.curr_fullName.Empty();
    currLocInfo.curr_localizationPath.Empty();
	
	m_pConfig->Read(_T("ui_language"), &currLocInfo.curr_UI_Language);
	m_pConfig->Read(_T("ui_language_code"),&currLocInfo.curr_shortName);
	m_pConfig->Read(_T("ui_language_name"),&currLocInfo.curr_fullName);
#ifdef _UNICODE
	m_pConfig->Read(_T("ui_language_path_unicode"),&currLocInfo.curr_localizationPath);
#else
	m_pConfig->Read(_T("ui_language_path"),&currLocInfo.curr_localizationPath);
#endif
	
	// Next, retrieve any user defined language data from the user_defined_language_n keys.
	int ct;
	const int nKeys = 9;
	wxArrayString keyArray;
	keyArray.Alloc(9);
	bool bKeysPresent = FALSE;
	bool bKeyMissing = FALSE;
	bool bAllKeysAssigned = FALSE;

	for (ct = 0; ct < nKeys; ct++)
	{
		wxString str;
#ifdef _UNICODE
		str << _T("user_defined_language_u_") << ct;
#else
		str << _T("user_defined_language_") << ct;
#endif
		bool valReadOK = TRUE;
		wxString tempStr;
		valReadOK = m_pConfig->Read(str, &tempStr);
		keyArray.Add(tempStr);
		if (valReadOK && keyArray[ct] == _T("[UNASSIGNED]"))
		{
			bAllKeysAssigned = FALSE;
		}
		if (valReadOK && keyArray[ct] != _T("[UNASSIGNED]"))
		{
			bKeysPresent = TRUE;
		}
		if (!valReadOK)
		{
			bKeyMissing = TRUE;
			keyArray[ct] = _T("[UNASSIGNED]");	// assign null string to any keyArray element for which 
									// there is no key
		}
	}
	
	if (bKeysPresent)
	{
		// User defined language keys are present. Do some sanity checks and then add them to the 
		// current locale.
		// Note: The wxLanguage Database gets any user defined languages added here at each running
		// of the app, taking the user defined languages from the registry/hidden settings file. 
		// It may be that during the previous running of the application the user added a user defined
		// language when all user_defined_language_n keys were already assigned. In that case Adapt It
		// removed the oldest assigned language from the keys (which would have been assigned the
		// wxLangauge numerical value of 231); moved the other 8 keys up in the list (keeping their
		// values ranging from 232 through 239), and added the most recent language to the last key
		// with a value of 240. Therefore, with this scenario, it could be that the wxLanguage value of
		// the user_defined_language_0 key is 232 and the last key has a value of 240. If this is the 
		// case, we can here (and here only) renumber the wxLanguage values before we call AddLanguage(). 
		// In such a case we would renumber the user defined languages 231 through 239, to keep from the
		// possibility of running out of wxLanguage values (which are enum values with a max value of
		// 255).
		// 
		// First we sort the keyArray in case RemoveUserDefinedLanguageInfoStringFromConfig() was
		// called previously and left an "[UNASSIGNED]" string assigned to a key in the middle of the
		// keys somewhere.
		// Note: This is the only place in the handling of the registry/hidden settings that we will
		// sort the keys.
		keyArray.Sort(); // Sorts in ascending order and places any unassigned keys toward the end.

		int nCodeToAssign;
		nCodeToAssign = wxLANGUAGE_USER_DEFINED + 1;
		for (ct = 0; ct < nKeys; ct++)
		{
			wxString str;
#ifdef _UNICODE
			str << _T("user_defined_language_u_") << ct;
#else
			str << _T("user_defined_language_") << ct;
#endif
			if (keyArray[ct] != _T("[UNASSIGNED]"))
			{
				// parse out the shortName and fullName parts of the unix-like string
				wxString tempStr,tempCode,tempShortName,tempFullName,tempPath;
				tempStr = keyArray[ct];
				tempCode = tempStr.Mid(0,tempStr.Find(_T(':')));
				tempStr = tempStr.Remove(0,tempStr.Find(_T(':'))+1);
				tempShortName = tempStr.Mid(0,tempStr.Find(_T(':')));
				tempStr = tempStr.Remove(0,tempStr.Find(_T(':'))+1);
				tempFullName = tempStr.Mid(0,tempStr.Find(_T(':')));
				tempStr = tempStr.Remove(0,tempStr.Find(_T(':'))+1);
				tempPath = tempStr;
                // If the existing wxLanguage value for this user_defined_language_n is the same as the
                // ui_language key, but this user_defined_language represents a different language than
				// the language represented by ui_language_code and ui_language_name, we have a
				// wxLanguage value conflict (our current ui_language and a user defined language
				// would be assigned the same wxLanguage value in the code below). We resolve the
				// conflict by assigning to this user_defined_language a wxLanguage value that doesn't
				// conflict with the ui_language or any other existing user_defined_language's 
				// curr_UI_Language.
				int nTempCode;
				nTempCode = wxAtoi(tempCode);
				if (nTempCode == currLocInfo.curr_UI_Language
					&& (tempShortName != currLocInfo.curr_shortName || tempFullName != currLocInfo.curr_fullName))
				{
					int code;
					code = GetFirstAvailableLanguageCodeOtherThan(currLocInfo.curr_UI_Language, keyArray);
					if (code != -1)
					{
						// We found a suitable different language code for this user_defined_language
						// key to avoid clash of wxLanguage values being stored in the registry/hidden
						// settings file.
						wxString compositeStr;
						compositeStr << code << _T(':') << tempShortName << _T(':') << tempFullName << _T(':') << tempPath;
						m_pConfig->Write(str, compositeStr);
					}
				}
			
				const wxLanguageInfo *info = wxLocale::FindLanguageInfo(tempFullName);
				if (!info)
				{
					// this language is not yet in the wxLanguage database
					wxLanguageInfo langInfo;
					langInfo.Language = nCodeToAssign;
					langInfo.CanonicalName = tempShortName;
		#ifdef __WIN32__
					// Win32 language identifiers
					langInfo.WinLang = 0;			// We don't know the Windows LANG_xxxx value so enter zero
					langInfo.WinSublang = 0;		// We don't know the Windows SUBLANG_xxxx value so enter zero 
		#endif
					langInfo.Description = tempFullName;
					gpApp->m_pLocale->AddLanguage(langInfo);
					
					// Now write the key
					// Make a composite unix-like string from the data stored in the registry/hidden
					// settings file, but use the currLocInfo.curr_localizationPath in case the path
					// that was stored was not up to date.
					wxString compositeStr;
					compositeStr << nCodeToAssign << _T(':') << tempShortName << _T(':') << tempFullName << _T(':') << currLocInfo.curr_localizationPath; //tempPath;
					m_pConfig->Write(str, compositeStr);
					// If our compositeStr represents the current interface language to be used, we
					// should check to see if the wxLanguage value stored in the ui_language key needs
					// updating. 
					if (tempFullName == currLocInfo.curr_fullName && tempShortName == currLocInfo.curr_shortName)
					{
						// Our compositeStr represents the current interface language to be used, so
						// update the wxLanguage value of ui_language if necessary
						if (nCodeToAssign != currLocInfo.curr_UI_Language)
						{
							m_pConfig->Write(_T("ui_language"), nCodeToAssign);
						}
					}
					nCodeToAssign++;
				}
				else
				{
                    // The language was already in the wxLanguage database, which means that either (1)
                    // it no longer needs to be considered a user defined language, or (2) it was
                    // somehow a duplicate user defined language in the list. In either case we don't
                    // need to write this user defined language back to a user_defined_language_n key,
                    // and we make the key "[UNASSIGNED]" instead.
					m_pConfig->Write(str, _T("[UNASSIGNED]"));
				}
			}
			else
			{
				// This keyArray element was already "[UNASSIGNED]" so we write it back that way
				m_pConfig->Write(str, _T("[UNASSIGNED]"));
			}
		}
	}

	// restore the oldPath back to "/Recent_File_List"
	m_pConfig->SetPath(oldPath);
    return currLocInfo; 
	// end of wxLogNull logNo here
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if localization files exist at a known path; FALSE otherwise
/// \remarks
/// Called from: the App's OnInit(). Determines if localization files are present at a known
/// path on the local computer. This function first checks the registry/hidden settings file to 
/// see if a previous localization path was set (in ui_language_path). If so, it checks to see 
/// if localization files are actually currently located there. If so it sets the
/// ui_localizationFilesExist key to TRUE and returns TRUE. If no files are located at a 
/// previously set path as indicated by the ui_language_path key, it checks to see if 
/// localization files are located at the default location for the current platform. If 
/// localization files actually exist there it sets the path stored in the ui_language_path 
/// key to reflect where they are actually located, and sets the ui_localizationFilesExist key
/// to TRUE and returns TRUE. If localization files could not be found by inspecting the 
/// ui_language_path key nor at the default location, the ui_localizationFilesExist key is 
/// set to FALSE and this function returns FALSE. Therefore, when this function returns, 
/// the ui_localizationFilesExist key is guaranteed to be set to either TRUE or FALSE and 
/// the return value of this function will agree with that key settings.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::LocalizationFilesExist()
{
	bool bLocalizationFilesFound = FALSE; // assume FALSE unless we actually find them
	// First check to see if the ui_language_path key exists.
	// 
	// Retrieve the interface language information from m_pConfig if it exists.
	wxString oldPath = m_pConfig->GetPath(); // is always absolute path "/Recent_File_List"
	m_pConfig->SetPath(_T("/Settings"));

	wxLogNull logNo;	// eliminates spurious message from the system: "Can't read value of key
						//'HKCU\Software\Adapt_It_WX\Settings' Error" [valid until end of this block]
	wxString uiLangPath;
	uiLangPath.Empty();
#ifdef _UNICODE
	m_pConfig->Read(_T("ui_language_path_unicode"),&uiLangPath);
#else
	m_pConfig->Read(_T("ui_language_path"),&uiLangPath);
#endif

	if (!uiLangPath.IsEmpty())
	{
		// The key was is the registry/hidden settings file, so check if localization files actually
		// exist at the specified location, and for the ui_language_code (used as subdir name).
		// Since ui_language_path exists, there should also be a ui_language_code value that we can
		// use to insure that the specific localization subfolder named actually exists.
		wxString shortNameStr;
		shortNameStr.Empty();
		m_pConfig->Read(_T("ui_language_code"), &shortNameStr);
        // If shortNameStr is empty at this point, it means that no actual ui_language_code key was present
        // and so the PathHas_mo_LocalizationFile() test below will not require a specific subdir to be
        // present as a localization folder.
		if (PathHas_mo_LocalizationFile(uiLangPath, shortNameStr))
		{
			// The uiLangPath contains at least one subfolder which in turn contains an <appName>.mo
			// file, so we set the ui_localizationFilesExist key to TRUE, and return TRUE (at end).
			bLocalizationFilesFound = TRUE;
#ifdef _UNICODE
			m_pConfig->Write(_T("ui_localizationFilesExist_unicode"), bLocalizationFilesFound);
#else
			m_pConfig->Write(_T("ui_localizationFilesExist"), bLocalizationFilesFound);
#endif
		}
		else
		{
			// The current ui_language_path contains a bad localization files path. So, we'll check to
			// see if localization files are located at the platform default location.
			wxString defaultPath;
			defaultPath = GetDefaultPathForLocalizationSubDirectories();
			if (PathHas_mo_LocalizationFile(defaultPath, shortNameStr))
			{
				// The defaultPath contains at least one subfolder which in turn contains an <appName>.mo
				// file, so we correct the bad ui_localization_path key, set the ui_localizationFilesExist 
				// key to TRUE, and return TRUE (at end).
				bLocalizationFilesFound = TRUE;
#ifdef _UNICODE
				m_pConfig->Write(_T("ui_language_path_unicode"), defaultPath);
				m_pConfig->Write(_T("ui_localizationFilesExist_unicode"), bLocalizationFilesFound);
#else
				m_pConfig->Write(_T("ui_language_path"), defaultPath);
				m_pConfig->Write(_T("ui_localizationFilesExist"), bLocalizationFilesFound);
#endif

			}
			else
			{
				bLocalizationFilesFound = FALSE;
#ifdef _UNICODE
				m_pConfig->Write(_T("ui_localizationFilesExist_unicode"), bLocalizationFilesFound);
#else
				m_pConfig->Write(_T("ui_localizationFilesExist"), bLocalizationFilesFound);
#endif
			}
		}
	}
	else
	{
		// The ui_language_path key was not in the registry/hidden settings file. This probably means
		// that no ui_language has yet been stored in the registry/hidden settings file. We could check
		// to see if ui_language, ui_language_code and ui_language_name exist unexpectedly, but if they
		// do without a ui_language_path key, it would only mean that someone has been monkeying with
		// the registry/hidden settings file, deleting some keys but not others. We can't prevent all
		// possible silly things from being done, so we will just assume that no localization files
		// exist which will bring up the ChooseLanguageDlg dialog at program startup (and fix any
		// problems that exist in the stored keys).
		bLocalizationFilesFound = FALSE;
#ifdef _UNICODE
		m_pConfig->Write(_T("ui_localizationFilesExist_unicode"), bLocalizationFilesFound);
#else
		m_pConfig->Write(_T("ui_localizationFilesExist"), bLocalizationFilesFound);
#endif
	}
	
	
	
	// restore the oldPath back to "/Recent_File_List"
	m_pConfig->SetPath(oldPath);
	return bLocalizationFilesFound;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if user made a choice different from the current language; FALSE if user canceled or
///             made no change
/// \remarks
/// Called from: the App's OnInit() and ChangeUILanguage(). If localization files are present, it 
/// presents the user with the CChooseLanguageDlg dialog from which he can choose an interface 
/// language localization for Adapt It from a list of possible language localizations. If there are
/// no localization files installed/available at the localization files path, and no previous 
/// ui_language was ever saved in the registry/hidden settings file, the dialog is not shown and 
/// ChooseInterfaceLanguage() simply sets the global CurrLocalizationInfo struct's curr_UI_Language
/// to wxLANGUAGE_DEFAULT (and its other members appropriately). Also, if localization files are
/// present, but the user aborts the choice, or a suitable interface language could not be 
/// determined, ChooseInterfaceLanguage() sets the global CurrLocalizationInfo struct's 
/// curr_UI_Language to wxLANGUAGE_DEFAULT (and its other members appropriately).
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::ChooseInterfaceLanguage(enum SetInterfaceLanguage setInterface)
{
	// See LangInfo struct and langsKnownToWX[] for valid wxLanguage languages identifiers known to wxWidgets.

	CurrLocalizationInfo currLocInfo;
	currLocInfo = currLocalizationInfo;
	bool bChangeMade = FALSE;
	
	if (setInterface == userInitiated || LocalizationFilesExist() || wxGetKeyState(WXK_ALT))
	{

		CChooseLanguageDlg dlg(NULL); // use NULL as "parent" window since our main frame may not be created yet
		dlg.Centre();
		//int choice = -1;
		if (dlg.ShowModal() == wxID_OK)
		{
			// CChooseLanguageDlg insures that the CurrLocalizationInfo struct's members are filled
			// in appropriately for the chosen language, so nothing more to do here.
			;
			//choice = dlg.m_nLanguageChoice;
			if (dlg.m_bChangeMade)
				bChangeMade = TRUE;
		}
		else
		{
			// The user aborted from the CChooseLanguageDlg dialog, so we set the CurrLocalizationInfo
			// struct's members back to their values before calling CChooseLanguageDlg.
			currLocalizationInfo.curr_UI_Language = currLocInfo.curr_UI_Language;
			currLocalizationInfo.curr_shortName = currLocInfo.curr_shortName;
			currLocalizationInfo.curr_fullName = currLocInfo.curr_fullName;
			currLocalizationInfo.curr_localizationPath = currLocInfo.curr_localizationPath;
			m_localizationInstallPath = currLocalizationInfo.curr_localizationPath; // save it here too
		}
	}
	else
	{
		// Localization files could not be found, and user did not hold down the ALT key to force the
		// CChooseLanguageDlg dialog to appear. Therefore we quietly assign default English language 
		// values to the currLocalizationInfo struct (which will be saved to the registry/hidden 
		// settings file when SaveCurrentUILanguageInforToConfig is called below).
		int sysLanguage = wxLocale::GetSystemLanguage();
		const wxLanguageInfo *info = wxLocale::GetLanguageInfo(sysLanguage);		
		currLocalizationInfo.curr_UI_Language = sysLanguage;
		currLocalizationInfo.curr_shortName = info->CanonicalName;
		currLocalizationInfo.curr_fullName = info->Description;
		currLocalizationInfo.curr_localizationPath = GetDefaultPathForLocalizationSubDirectories();
		m_localizationInstallPath = currLocalizationInfo.curr_localizationPath; // save it here too
	}
	// the CurrLocalizationInfo struct's members are now filled appropriately, so save the data
	// in the m_pConfig configuration.
    SaveCurrentUILanguageInfoToConfig();
	
	return bChangeMade;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: the View's OnChangeInterfaceLanguage() and CLanguagesPage::OnUILanguage(). 
/// Calls the ChooseInterfaceLanguage() function which presents the user with the CChooseLanguageDlg dialog 
/// from which he can choose an interface language for the application from a list of languages 
/// that have localization <appName>.mo files on in localization subfolders on the localization path.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::ChangeUILanguage()
{
	bool bChangeMade;
	bChangeMade = ChooseInterfaceLanguage(userInitiated);	// calls CChooseLanguageDlg and insures that currLocalizationInfo's 
															// curr_UI_Language is something other than wxLANGUAGE_UNKNOWN
    if (bChangeMade)
	{
		// Try to get as many of the interface elements to layout with new localization strings as possible.
		// 
		// Note: wx docs say of wxWindow::UpdateWindowUI():
		// This function sends wxUpdateUIEvents to the window. The particular implementation depends on 
		// the window; for example a wxToolBar will send an update UI event for each toolbar button, 
		// and a wxFrame will send an update UI event for each menubar menu item. You can call this 
		// function from your application to ensure that your UI is up-to-date at this point (as far 
		// as your wxUpdateUIEvent handlers are concerned). This may be necessary if you have called 
		// wxUpdateUIEvent::SetMode or wxUpdateUIEvent::SetUpdateInterval to limit the overhead that 
		// wxWidgets incurs by sending update UI events in idle time.
		//flags should be a bitlist of one or more of the following values.
		//enum wxUpdateUI
		//{
		//    wxUPDATE_UI_NONE          = 0x0000, // No particular value
		//    wxUPDATE_UI_RECURSE       = 0x0001, // Call the function for descendants
		//    wxUPDATE_UI_FROMIDLE      = 0x0002  // Invoked from On(Internal)Idle
		//};
		//If you are calling this function from an OnInternalIdle or OnIdle function, make sure you 
		//pass the wxUPDATE_UI_FROMIDLE flag, since this tells the window to only update the UI 
		//elements that need to be updated in idle time. Some windows update their elements only 
		//when necessary, for example when a menu is about to be shown. The following is an 
		//example of how to call UpdateWindowUI from an idle function.
		//void MyWindow::OnInternalIdle()
		//{
		//    if (wxUpdateUIEvent::CanUpdate(this))
		//        UpdateWindowUI(wxUPDATE_UI_FROMIDLE);
		//}
		// Note: The code below unfortunately does not update the translated strings immeditately, so
		// it is still best to advise the user to start up Adapt It again. Looking at Audacity and
		// PoEdit (which are applications created with wxWidgets) - they also require a restart after
		// changing the interface. It might be possible to programattically offer to restart Adapt It,
		// but that might have some unintended side effects, so I won't go down that road at this point.
		//if (m_pMainFrame != NULL)
		//{
		//	m_pMainFrame->UpdateWindowUI(wxUPDATE_UI_RECURSE);
		//	if (m_pMainFrame->m_pMenuBar != NULL)
		//		m_pMainFrame->m_pMenuBar->UpdateWindowUI(wxUPDATE_UI_RECURSE);
		//	if (m_pMainFrame->m_pStatusBar != NULL)
		//		m_pMainFrame->m_pStatusBar->UpdateWindowUI(wxUPDATE_UI_RECURSE);
		//	if (m_pMainFrame->m_pToolBar != NULL)
		//		m_pMainFrame->m_pToolBar->UpdateWindowUI(wxUPDATE_UI_RECURSE);
		//	if (m_pMainFrame->m_pControlBar != NULL)
		//		m_pMainFrame->m_pControlBar->UpdateWindowUI(wxUPDATE_UI_RECURSE);
		//	if (m_pMainFrame->m_pComposeBar != NULL)
		//		m_pMainFrame->m_pComposeBar->UpdateWindowUI(wxUPDATE_UI_RECURSE);
		//}
		wxMessageBox(_("The user interface language change may not fully take effect until you restart Adapt It."),
                 m_FrameAndDocProgramTitle,
                 wxOK | wxCENTRE | wxICON_INFORMATION);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     the int value of the first available wxLanguage code for a user defined language
/// \param      codeToAvoid  -> an int value to avoid when finding a wxLanguage code
/// \param      keyArray     -> a wxArrayString of registry/hidden setting file keys (pre-populated)
/// \remarks
/// Called from: The App's ProcessUILanguageInfoFromConfig().
/// Determines the next available wxLanguage value that can be assigned to a user_defined_langauge-n
/// registry/hidden settings file key. Since the number of user defined language keys is restricted to
/// no more than 9, the possible return values can range from wxLANGUAGE_USER_DEFINED + 1 through
/// wxLANGUAGE_USER_DEFINED + 1 + nKeys. Since the current wxWidgets version's (2.8.8)
/// wxLANGUAGE_USER_DEFINED symbol is defined to be 230, the values that can be returned by this
/// function range from 231 through 239. If no value can be determined, the function returns -1.
////////////////////////////////////////////////////////////////////////////////////////////
int CAdapt_ItApp::GetFirstAvailableLanguageCodeOtherThan(const int codeToAvoid, wxArrayString keyArray)
{
	int ct;
	wxArrayString codeStrArray;
	int tempCode;
	int codeToReturn = -1;
	const int nKeys = 9; // the max number of keys we allow in the registry/hidden settings file
	wxString tempStr;
	bool bHasUnassigned = FALSE;
	// Gather the language code strings (first part of the key string before the first colon).
	// The incoming keyArray will have key strings associated with any user_defined_language_n 
	// keys defined in the registry/hidden settings file.
	// An example of a key string might be "232:tpi:Tok Pisin:c:\Program Files\Adapt It\Languages"
	// where the "232" is the string equivalent of the wxLanguage value assigned for that
	// user_defined_language_n key.
	for (ct = 0; ct < nKeys; ct++)
	{
		if (keyArray[ct] != _T("[UNASSIGNED]"))
		{
			tempStr = keyArray[ct];
			tempStr = tempStr.Mid(0,tempStr.Find(_T(':')));
			tempCode = wxAtoi(tempStr);
			wxASSERT(tempCode > 230 && tempCode < (wxLANGUAGE_USER_DEFINED + 1 + nKeys));
			codeStrArray.Add(tempStr);
		}
		else
		{
			bHasUnassigned = TRUE;
		}
	}

	// At this point codeStrArray contains all of the code strings used in the keyArray.
	// Sort the codeStrArray to make it easier to determine the first available code.
	codeStrArray.Sort(); // sorts in ascending order by default

	// Now scan the codeStrArray to find the lowest wxLanguage code that has not already been assigned
	// and avoids codeToAvoid.
	bool bFound = FALSE;
	int maxCode = 0;
	int minCode = 999;
	if (codeStrArray.GetCount() > 0)
	{
		int cct, checkCode;
		for (cct = 0; cct < (int)codeStrArray.GetCount(); cct++)
		{
			int code;
			code = wxAtoi(codeStrArray[cct]);
			if (code > maxCode)
				maxCode = code;
			if (code < minCode)
				minCode = code;
			checkCode = wxLANGUAGE_USER_DEFINED + 1 + cct; // increments by the value of loop counter cct, i.e., 231, 232, 233 ... 239
			if (checkCode != code && code != codeToAvoid)
			{
				// We found a checkCode that is not already used and not the codeToAvoid
				if (!bFound)
				{
					// get only the first one available
					codeToReturn  = checkCode;
					bFound = TRUE;
				}
			}
		}
	}
	else
	{
		// there are no user_defined_language_n keys assigned so assign codeToReturn the value 
		// wxLANGUAGE_USER_DEFINED + 1, unless that is the codeToAvoid, in which case assigne
		// codeToReturn the value wxLANGUAGE_USER_DEFINED + 2.
		if (wxLANGUAGE_USER_DEFINED + 1 != codeToAvoid)
			codeToReturn = wxLANGUAGE_USER_DEFINED + 1;
		else
			codeToReturn = wxLANGUAGE_USER_DEFINED + 2;
	}
	// if codeToReturn is still -1 and there are unassigned keys try to find a value using the minCode
	// and maxCode values
	if (codeToReturn == -1 && bHasUnassigned)
	{
		if (minCode < 999 && minCode > (wxLANGUAGE_USER_DEFINED + 1) && minCode != codeToAvoid)
		{
			codeToReturn = wxLANGUAGE_USER_DEFINED + 1;
		}
		else if (maxCode >= wxLANGUAGE_USER_DEFINED + 1 && (maxCode + 1) < (wxLANGUAGE_USER_DEFINED + 1 + nKeys))
		{
			if (maxCode + 1 != codeToAvoid)
				codeToReturn = maxCode + 1;
			else
				codeToReturn = maxCode + 2;
		}
	}
    // At this point, if codeToReturn is still -1 it means that there were no values available within
	// the range wxLANGUAGE_USER_DEFINED + 1 through wxLANGUAGE_USER_DEFINED + 1 + nKeys. In such cases
	// we just return -1 to tell the caller that we could not get a code.
	return codeToReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called once when the application is first executed. OnInit() is the main place where all
/// application level variables are to be initialized in a wxWidgets based application. It is
/// basically the equivalent of MFC's InitInstance(), but seems to be called earlier in the 
/// execution of a program at startup that is InitInstance() on MFC. Note: The order in which
/// things are initialized can be important in OnInit(). For example, we cannot do anything with
/// the main frame or variables it initializes until the CMainFrame is created in OnInit().
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::OnInit() // MFC calls this InitInstance()
{
	// wxWidgets NOTES:
	// 1. Do NOT attempt to call or manipulate anything from within the CMainFrame 
	// constructor (see MainFrm.cpp) before a CMainFrame object is constructed here (see
	// the new CMainFrame() statement below. For example, removing a menu from the main 
	// frame cannot be done in the CMainFrame constructor because the main frame menu itself is 
	// not loaded and constructed until the SetMenuBar call here in CAdapt_ItApp::OnInit() 
	// is executed with: m_pMainFrame->SetMenuBar(AIMenuBarFunc(m_pDocManager));
	// 2. All Application initialization should be done here in OnInit() rather
	// than in the App's constructor CAdapt_ItApp::CAdapt_ItApp(). This is especially
	// true for versions of wxWidgets after 2.4.2 (2.5.x, 2.6.x, 2.7.x, 2.8.x etc), because in
	// the more recent wxWidgets' versions the application's constructor is "called
	// much earlier than previously." Also Note that any application level cleanup
	// should be done in CAdapt_ItApp::OnExit(), and NOT in the application's
	// destructor CAdapt_ItApp:~CAdapt_ItApp(), for similar reasons (i.e., the
	// app's destructor gets called much later than in previous versions).
	
	// whm moved initialization of global pointer to the appliction gpApp here at the
	// beginning of OnInit() to make it available sooner.
    gpApp = this;

	// whm added for wx version:
	// Adapt It has many windows and buttons, so in order to reduce the overhead of UI
	// update events we set the update interval to happen only every nUpdateInterval
	// milliseconds. Note: SetUpdateInterval() is a static function.
	// whm update note: testing seems to show that CPU usage is not affected by setting
	// the nUpdateInterval differently than its default value of 0. The CPU usage stays
	// around 50% for most states of the application (except when showing a wxMessageBox
	// when it drops to near 0%).
	//static long nUpdateInterval = -1;
	//wxUpdateUIEvent::SetUpdateInterval(nUpdateInterval);

	m_pLocale = (wxLocale*)NULL;

	// Initializations below are from MFC's CAdapt_ItApp's constructor
	// //////////////////////////////////////////////////////////////////////////////////////////////////////
	// MFC note: Place all significant initialization in InitInstance
	//m_pCurView = NULL; // BEW added 22Sep05 
	// whm Note: I'm not implementing this m_pCurView pointer in the wxWidgets version. 
	// Bruce added it because MFC's split window facility was found to be creating a second 
	// view (and hence a different view pointer value) depending on which view was in focus.
	// The MFC app was crashing when the view was split due various places in the code where 
	// the MFC design assumed the view didn't change. This problem of crashing because of not 
	// having a stable view pointer surfaced early on and more frequently in the wxWidgets 
	// version, and that's why I decided not to implement a splitter window, and also 
	// (because of wxWidgets doc/view creating multiple view pointers at other times) I 
	// eventually decided it was safer just to move all variables being kept on the Doc 
	// and View here to the App. 

	m_bJustLaunched = FALSE;	// set true at end of OnInit(); then set false in OnIdle call 
								// within the main frame once it is created (below)
	
	m_dialogFontSize = 12; // default, but user can override // unused in WX version
	m_pKB = (CKB*)NULL; 
	m_pGlossingKB = (CKB*)NULL; // added - not done in MFC version
	m_bExistingAdaption = FALSE;
	m_bKBReady = FALSE;
	m_bGlossingKBReady = FALSE;
	m_bAutoInsert = FALSE;
	m_bTablesLoaded = FALSE; // whm added 23May04 not initialized in MFC
	
	gbSfmOnlyAfterNewlines = FALSE; // default, so sfm escape char anywhere defines a sfm
#ifdef _RTL_FLAGS
	// default reading order should be LTR unless user changes it, or config file changes it
	m_bSrcRTL = FALSE;
	m_bTgtRTL = FALSE;
	m_bNavTextRTL = FALSE;
#endif

    // MFC Note: define the Unicode label for use in the final step, to get separate Unicode folder
    // structures NOTE: for version 2.0.6 and onwards, we use "Unicode<SP>" rather than "NR<SP>", and
    // the RenameNRtoUnicode( ) function does the required rename silently.
#ifdef _UNICODE
	m_FrameAndDocProgramTitle = _("Adapt It Unicode"); // whm added 25Apr07
	m_strNR = _("Unicode ");
#else // ANSI version
	m_FrameAndDocProgramTitle = _("Adapt It"); // whm added 25Apr07
	m_strNR = "";
#endif

	// initialize the directory paths
	m_workFolderPath = _T("");
	m_localPathPrefix = _T("");

	// The following use the _T() macro as they shouldn't be translated/localized
	m_theWorkFolder = m_theWorkFolder.Format(_T("Adapt It %sWork"),m_strNR.c_str());
	// whm Note: In the MFC version the "Adaptations" folder is localizable, so we
	// will do the same here
	// TODO: check the Unpack process when unpacking a packed file coming from 
	// a different localization.
	m_adaptionsFolder = _("Adaptations");
	m_lastSourceFileFolder = m_workFolderPath;
	m_curProjectPath = _T("");
	m_curAdaptionsPath = _T("");
	m_curKBName = _T("");
	m_curKBPath = _T("");
	m_curProjectName = _T("");
	m_curGlossingKBName  = _T("Glossing");
	m_curGlossingKBPath = _T("");

	// consistent changes pointers and paths
	m_pConsistentChanger[0] = (CConsistentChanger*)NULL; // whm added 19Jun07
	m_pConsistentChanger[1] = (CConsistentChanger*)NULL; // " " "
	m_pConsistentChanger[2] = (CConsistentChanger*)NULL; // " " "
	m_pConsistentChanger[3] = (CConsistentChanger*)NULL; // " " "

	m_defaultTablePath = _T("");
	m_tableName[0] = _T("");
	m_tableName[1] = _T("");
	m_tableName[2] = _T("");
	m_tableName[3] = _T("");
	m_tableFolderPath[0] = _T("");
	m_tableFolderPath[1] = _T("");
	m_tableFolderPath[2] = _T("");
	m_tableFolderPath[3] = _T("");

	m_lastExportPath = _T("");
	m_lastDocPath = _T(""); // config file will set this
	m_lastSrcExportPath = _T("");
	m_kbExportPath = _T("");
	m_rtfExportPath = _T("");
	m_retransReportPath = _T("");

	m_bSuppressWelcome = FALSE;
	m_bSuppressTargetHighlighting = FALSE;
	nSequNumForLastAutoSave = -1; // global (-1 = _("turned off")
	m_bBackupDocument = FALSE;

	m_bCCTableLoaded[0] = m_bCCTableLoaded[1] = m_bCCTableLoaded[2] = m_bCCTableLoaded[3] = FALSE;

	m_bUseStartupWizardOnLaunch = TRUE;
	m_bAutoBackupKB = TRUE;
	m_bNoAutoSave = TRUE;

	m_ptViewTopLeft.x = 20;
	m_ptViewTopLeft.y = 20;
	m_szView.x = 640;
	m_szView.y = 580;
	m_bMarkerWrapsStrip = TRUE;
	m_bZoomed = FALSE;

	m_bIsInches = FALSE; // Default to Metric for wx version, especially since in the Windows page setup we can only use mm for margins
	m_bIsPortraitOrientation = TRUE;
	m_paperSizeCode = 9; // always mapped to MFC's enum for paper size code = 9; // A4

	//englishToMetric = 2.5;	// Bruce uses this as an approximation...
	// The following are OK for maintaining compatibility with MFC's config files, but they should not
	// be used for conversions relating to screen and printer display contexts. 
	config_only_thousandthsInchToMillimetres = (float)0.025; // conversion factor rounded from 0.0254
	config_only_millimetresToThousandthsInch = (float)40.00; // conversion factor rounded from 39.37
	
	thousandthsInchToMillimetres = (float)0.0254; // more precise conversion factor for display context calculations
	millimetresToThousandthsInch = (float)39.37; // more precise conversion factor for display context calculations
	
	// Default values are set here and stored in config file in MFC's MM_LOENGLISH mapping mode units 
	// (thousandths of an inch). The conversion between English and Metric in MFC used two different
	// degrees of precision for the conversion; MFC uses a conversion factor of 0.025 when converting
	// margins but a more precise 0.0254 when converting page size dimensions. This creates a bit of a
	// mismatch for the wxWidgets version which handles page setup and printing values throughout in
	// millimeters and does those conversions regularly with the more precise 0.0254 conversion factor.
	// To keep compatibility with the MFC version and its config files, we need to use 0.025 for
	// calculating margin values, but 0.0254 for calculating page dimensions.
	m_pageWidth = 8267;		// 210mm / 0.0254mm/in (result truncated to whole int) in thousandths of an inch = 8267 A4
	m_pageWidthMM = wxRound(m_pageWidth * thousandthsInchToMillimetres); // 210mm	A4 page width in mm
	m_pageLength = 11692;	// 297mm / 0.0254mm/in (result truncated to whole int) in thousandths of an inch = 11692 A4
	m_pageLengthMM = wxRound(m_pageLength * thousandthsInchToMillimetres); // 297mm A4 page height in mm
	// the default margin values below use less precise conversion factor of 0.0250mm/in
	m_marginTop = 1000;		// 25mm / 0.0250mm/in (result truncated to whole int) in thousandths of an inch = 1000; // one inch
	m_marginTopMM = (int)(m_marginTop * config_only_thousandthsInchToMillimetres); // 25mm top margin in mm
	m_marginBottom = 1000;	// 25mm / 0.0250mm/in (result truncated to whole int) in thousandths of an inch = 1000; // one inch
	m_marginBottomMM = (int)(m_marginBottom * config_only_thousandthsInchToMillimetres); // 25mm bottom margin in mm
	m_marginLeft = 1000;	// 25mm / 0.0250mm/in (result truncated to whole int) in thousandths of an inch = 1000; // one inch
	m_marginLeftMM = (int)(m_marginLeft * config_only_thousandthsInchToMillimetres); // 25mm left margin in mm
	m_marginRight = 1000;	// 25mm / 0.0250mm/in (result truncated to whole int) in thousandths of an inch = 1000; // one inch
	m_marginRightMM = (int)(m_marginRight * config_only_thousandthsInchToMillimetres); // 25mm right margin in mm

	m_bEarlierProjectChosen = FALSE;
	m_bEarlierDocChosen = FALSE;
	nLastActiveSequNum = 0; // config file will set this
	
	// initialize font pointers to NULL outside InitializeFonts() because InitializeFonts() may
	// also be called from SetDefaults().
	m_pSourceFont = (wxFont*)NULL;
	m_pTargetFont = (wxFont*)NULL;
	m_pNavTextFont = (wxFont*)NULL;
	m_pDlgSrcFont = (wxFont*)NULL;
	m_pDlgTgtFont = (wxFont*)NULL;
	m_pDlgGlossFont = (wxFont*)NULL;
	m_pComposeFont = (wxFont*)NULL;
	m_pRemovalsFont = (wxFont*)NULL;
	m_pVertEditFont = (wxFont*)NULL;
	
	m_pSrcFontData = (wxFontData*)NULL;
	m_pTgtFontData = (wxFontData*)NULL;
	m_pNavFontData = (wxFontData*)NULL;

	InitializeFonts();	// incorporates the font initialization parts of MFC's Initialize() that
						// came later in OnInit()

	InitializePunctuation();

	// if it is the NR version, permit the dialogs with extra RTL-related controls to show
	// the latter
#ifdef _RTL_FLAGS
	m_bShowRTL_GUI = TRUE;
#else
	m_bShowRTL_GUI = FALSE;
#endif
	m_bRTL_Layout = FALSE;

	gbRTL_Layout = FALSE; // a global helps faster access for speed critical stuff, don't need 
						  // to get a pointer to the app first
	m_nCurDelay = 0; // default is no delay (zero ticks)
	m_bBookMode = FALSE; // default is OFF for "book mode" (ie. Bible books folders)
	m_nBookIndex = -1; // -1 means "no index is defined", index values are 0 based
	m_nDivSize[0] = 17; // history books
	m_nDivSize[1] = 5; // wisdom books
	m_nDivSize[2] = 17; // prophecy books
	m_nDivSize[3] = 27; // new testament books
	m_nDivSize[4] = 1; // Other Texts (catchall)
	m_pBibleBooks = new wxArrayPtrVoid();
	m_strDivLabel[0] = _("History books");
	m_strDivLabel[1] = _("Wisdom books");
	m_strDivLabel[2] = _("Prophecy books");
	m_strDivLabel[3] = _("New Testament books");
	m_strDivLabel[4] = _("Other Texts");
	m_nBookIndex = 0; // safe default, but we should do better in InitInstance()
	m_pCurrBookNamePair = NULL;
	m_bibleBooksFolderPath.Empty(); // determinable always from m_nBookIndex
	m_nLastBookIndex = -1;
	m_nDefaultBookIndex = 39;
	m_nTotalBooks = 67;
	m_bDisableBookMode = FALSE; // start off enabled
	gbAbortMRUOpen = FALSE;

	// !!! whm added 19Jan05 AI_USFM.xml file processing and USFM Filtering
	m_bUsingDefaultUsfmStyles = FALSE;
	m_bChangeFixedSpaceToRegularSpace = FALSE;// fixed spaces default to join words into phrases for adapting
	m_pUsfmStylesMap = new MapSfmToUSFMAnalysisStruct;
	m_pPngStylesMap = new MapSfmToUSFMAnalysisStruct;
	m_pUsfmAndPngStylesMap = new MapSfmToUSFMAnalysisStruct;
	m_pMappedObjectPointers = new wxArrayPtrVoid;// array of all pointers to USFMAnalysis
											// structs on the heap - for ease of 
											// deletion in the app destructor
	gCurrentSfmSet = UsfmOnly;	// may be set to PngOnly if, in reading the
								// project config file, if there is no UseUSFMarkerSet
								// setting defined there (i.e., the project was created
							// prior to Version 3.x)
	gProjectSfmSetForConfig = UsfmOnly;	// Retains any value set in the project config file
										// unless user explicitly changes it in Edit Preferences
										// USFM/Filtering tab or the Start Working... wizard USFM/Filtering
										// page when creating a new project.

	m_bSingleQuoteAsPunct = FALSE; // BEW added March 17, 2005
	m_bDoubleQuoteAsPunct = TRUE; // BEW added April 28, 2005
	m_bCopySourcePunctuation = TRUE;// BEW added April 20, 2005

	m_bFreeTranslationMode = FALSE; // BEW added 21Jun05, free translation mode is off by default
	m_bTargetIsDefaultFreeTrans = FALSE; // default is to have no precomposed free translation formed
	m_bGlossIsDefaultFreeTrans = FALSE; // ditto
	m_bComposeBarWasAskedForFromViewMenu = FALSE; // the bar is initially unopened by the Compose Bar command on View menu
	m_bDefineFreeTransByPunctuation = TRUE; // default
	m_freeTransDefaultBackgroundColor = wxColour(189,255,189); //(COLORREF)0xBDFFBD; // light pastel green
	m_freeTransCurrentSectionBackgroundColor = wxColour(255,200,200); //(COLORREF)0xC8C8FF; // light pastel pink
	
	m_bSaveAsXML = TRUE; // BEW added 04Aug05; default for version 3
	m_bNotesExist = FALSE; // 12Sep05 BEW
	m_bUnpacking = FALSE; // BEW added 10Jan06

	// RDE: added 3 Apr 06 in support of calling SilEncConverters for preprocessing 
	// the target word form (c.f. Consistent Changes)
    m_strSilEncConverterName.Empty();
    m_bSilConverterDirForward = TRUE;
    m_bECConnected = FALSE;
#ifdef USE_SIL_CONVERTERS
    m_eSilConverterNormalizeOutput = NormalizeFlags_None;
#endif
	m_bTransliterationMode = FALSE;

	// BEW added 2Sep08
	gbAdaptBeforeGloss = TRUE; // in vertical edit, do adaptations updating before doing glosses updating
	// //////////////////////////////////////////////////////////////////////////////////////////////////////
	// Above initializations are from MFC's CAdapt_ItApp's constructor

	m_eolStr = wxTextFile::GetEOL();	// this retrieves the platform specific end-of-line character string
										// for external text files:
										// On Windows it is "\r\n"; on Linux it is "\n"; on Macintosh it is "\r"

	// BEW added 01Oct06, we set it TRUE when program counter reaches the end of InitInstance()
	// because there is an MFC call of OnNewDocument() made in InitInstances()'s ProcessShellCommand() call
	// and our addition of code to immediately write out the project settings config file at the end of
	// OnNewDocument() would result in book mode being unilaterally reset to OFF before we even got to the
	// wizard. So we use th gbPassedMFCinitialization flag to suppress the config file write until we are
	// actually in the wizard
	gbPassedMFCinitialization = FALSE;


	// //////// Initializations below were originally in the Doc in the MFC version ///////
	// Variable initializations below moved here from the Doc because the 
	// wxWidgets' doc/view framework deletes the Doc and recreates it 
	// afresh (calling the Doc's constructor when a new Doc is opened) whereas MFC's
	// deletes its contents but reuses the old Doc pointer.
	gPreviousTextType = verse;
	m_pBuffer = (wxString*)NULL; // MFC had =0
	m_nInputFileLength = 0;
	m_curOutputFilename = _T("");
	m_curOutputPath = _T("");
	m_curOutputBackupFilename = _T("");
	m_altOutputBackupFilename = _T("");
	//m_saveLastFolderPath = _T("");
	m_currentUnknownMarkersStr = _T(""); // whm added 31May05
	m_pSourcePhrases = (SPList*)NULL; // MFC had = 0

	// Set text colors (see below)
	
	int width = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
#ifdef _RTL_FLAGS
	// NR version, we will use most of screen width
	if (width < 800)
		m_docSize = wxSize(width - 20,600); // the cy parameter is changed by the view later
	else
		m_docSize = wxSize(width - 40,600);
#else // ANSI version, LTR only
	if (width < 800)
		m_docSize = wxSize(width - 40,600);
	else
		m_docSize = wxSize(width - 80,600);
#endif
	// The following are initialized to their current values on entry to the View's OnEditPreferences()
	m_sfmSetBeforeEdit = UsfmOnly; // some reasonable default whm added 10Jun05 for Bruce
	m_filterMarkersBeforeEdit.Empty(); // some reasonable default whm added 10Jun05 for Bruce
	// The following are set to their new values on exit after OK in the View's OnEditPreferences()
	m_sfmSetAfterEdit = UsfmOnly; // some reasonable default whm added 10Jun05 for Bruce
	m_filterMarkersAfterEdit.Empty(); // some reasonable default whm added 10Jun05 for Bruce
	KeepYourHandsToYourself = FALSE; // Added by JF.

	// //////// Initializations above were originally in the Doc in the MFC version ///////

	// //////// Initializations below were originally in the View in the MFC version ///////
	// /////////////////////////////////////////////////////////////////////
	// Variable initializations below moved here from the View because the 
	// wxWidgets' doc/view framework deletes the View and recreates it 
	// afresh (calling the View's constructor when a new view is opened)
	m_pTargetBox = (CPhraseBox*)NULL; // added for persistent target box

	m_pEarlierTransDlg = (CEarlierTranslationDlg*)NULL;

	m_pNoteDlg = (CNoteDlg*)NULL; // needed in wx version 
	m_pViewFilteredMaterialDlg = (CViewFilteredMaterialDlg*)NULL; // needed in wx version

	m_pBundle = (CSourceBundle*)NULL;
	m_pActivePile = (CPile*)NULL; // start with null, in case user invokes Allow Glossing in the Advanced
						  // menu after launching and only a project is open but not a document
	m_pAnchor = (CCell*)NULL;
	m_selectionLine = -1;
	m_nActiveSequNum = -1; // negative until set after a file is read in and laid out

	m_bSelectByArrowKey = FALSE;
	m_bSuppressLast = TRUE; // default, very first time launched; thereafter uses config file 
							// stored value
	m_bSuppressFirst = TRUE; // default, very first time launched; thereafter uses config file 
							 // stored value
	// default modes
	m_bDrafting = TRUE;
	m_bSingleStep = FALSE; // default was TRUE before version 1.1.0
	m_bAcceptDefaults = FALSE;
	m_bRestorePunct = FALSE;
	m_bComposeWndVisible = FALSE; // initially, don't show it (keep interface looking simple)
	m_bUseConsistentChanges = FALSE;
	m_bUseSilConverter = FALSE;
	m_bSaveToKB = TRUE;
	m_bForceAsk = FALSE;
	m_bCopySource = TRUE;
	m_bMarkerWrapsStrip = TRUE;
	m_bRespectBoundaries = TRUE;
	m_bHidePunctuation = FALSE; // default, on very first launch
	m_bStartViaWizard = FALSE; // set it in the start of the wizard code, clear it in OnIdle()
	m_bUserTypedSomething = FALSE;
	m_bUseToolTips = TRUE;
	m_nTooltipDelay = 20000; // tooltip delay time is 20 seconds (20000 ms)

	m_pFindDlg = (CFindDlg*)NULL;
	m_pReplaceDlg = (CReplaceDlg*)NULL;
		
	gbEnableGlossing = FALSE;
	gbIsGlossing = FALSE;
	gbGlossingUsesNavFont = FALSE;
	gbRemovePunctuationFromGlosses = FALSE;
	gnSelectionLine = -1;
	gnSelectionStartSequNum = -1;
	gnSelectionEndSequNum = -1;
	gbPrintFooter = TRUE;
	gnStart = 0;
	gnEnd = -1;
	
	gbIgnoreScriptureReference_Receive = TRUE;
	gbIgnoreScriptureReference_Send = TRUE;
	gbLegacySourceTextCopy = TRUE; // default is legacy behaviour, to copy the source text (unless
								   // the project config file establishes the FALSE value instead)
	// Variable initializations above moved here from the View
	// /////////////////////////////////////////////////////////////////////
	/*
	// /////////////////// BEGIN WXWIDGETS LIBRARY CODE FRAGMENT TESTING ////////////////////////////////
	
	// Testing of general C++ code fragments (not depending on Adapt It code) can be done here to save
	// time in loading the Adapt It main frame, etc.
	
	// Testing the weird world of rectangles in MFC and wxWidgets!!!
	const int MAXITERATIONS = 18;
	bool bNormalize = TRUE;
	struct coord
	{
		int x;
		int y;
		int xx;
		int yy;
		int w;
		int h;
		double quadrant;
	};
	//                        
	coord c[MAXITERATIONS] = {
	//    point 1   point 2 width,height                           (relative posn of
	//		x,y,    xx,yy,      w,h  Q    Test #  Cartesian Quadrant points in constructor)
	//		----------------------------------------------------------------------
			2,3,	10,7,		8,4, 4,   // Test 1  4th - lower right topL and botR
			10,7,	2,3,		8,4, 4,   // Test 2  4th - lower right topL and botR swapped
			2,-3,	10,-7,		8,4, 1,   // Test 3  1st - upper right botL and topR
			10,-7,	2,-3,		8,4, 1,   // Test 4  1st - upper right botL and topR swapped
			-2,3,	-10,7,		8,4, 3,   // Test 5  3rd - lower left  topR and botL
			-10,7,	-2,3,		8,4, 3,   // Test 6  3rd - lower left  topR and botL swapped
			-2,-3,	-10,-7,		8,4, 2,   // Test 7  2nd - upper left  botR and topL
			-10,-7,	-2,-3,		8,4, 2,   // Test 8  2nd - upper left  botR and topL swapped
			2,1,	10,-3,		8,4, 4.1, // Test 9  spans 4th & 1st   botL and topR
			10,-3,	2,1,		8,4, 4.1, // Test 10 spans 4th & 1st   botL and topR swapped
			-10,1,	-2,-3,		8,4, 2.3, // Test 11 spans 2nd & 3rd   botL and topR
			-2,-3,	-10,1,		8,4, 3.3, // Test 12 spans 2nd & 3rd   botL and topR swapped
			-3,3,	5,7,		8,4, 3.4, // Test 13 spans 3rd & 4th   topL and botR
			5,7,	-3,3,		8,4, 3.4, // Test 14 spans 3rd & 4th   topL and botR swapped
			-3,-3,	5,-7,		8,4, 1.2, // Test 15 spans 1st & 2nd   botL and topR
			5,-7,	-3,-3,		8,4, 1.2, // Test 16 spans 1st & 2nd   botL and topR swapped
			-3,3,	5,-1,		8,4, 0,   // Test 17 spand all quads   botL and topR
			5,-1,	-3,3,		8,4, 0,   // Test 18 spand all quads   botL and topR swapped
			};
	int recttop, rectbottom;
	int recttopx, rectbottomx;
	int recttopn, rectbottomn;
	int rectleft, rectright;
	int rectleftx, rectrightx;
	int rectleftn, rectrightn;
	int rectwidth, rectheight;
	int rectwidthx, rectheightx;
	int rectwidthn, rectheightn;
	int ct;
	for (ct = 0; ct < MAXITERATIONS; ct++)
	{
		//if (c[ct].x > c[ct].xx)
		//{
		//	int tmp = c[ct].x;
		//	c[ct].x = c[ct].xx;
		//	c[ct].xx = tmp;
		//}
		//if (c[ct].y > c[ct].yy)
		//{
		//	int tmp = c[ct].y;
		//	c[ct].y = c[ct].yy;
		//	c[ct].yy = tmp;
		//}
		wxPoint topLeft(c[ct].x,c[ct].y);
		wxPoint botRight(c[ct].xx,c[ct].yy);
		wxRect rect(topLeft,botRight); // create rect with two points
		wxSize sz(abs(c[ct].xx - c[ct].x),abs(c[ct].yy - c[ct].y)); // calculated size (ignore c[ct].w and c[ct].h)
		wxRect rectx(topLeft,sz); // create rects with point and size
		wxRect rectn(topLeft.x,topLeft.y,c[ct].w,c[ct].h); // x,y, width and height come directly from the constants
		if (bNormalize)
		{
			rect = NormalizeRect(rect);
			rectx = NormalizeRect(rectx);
			rectn = NormalizeRect(rectn);
		}
		recttop = rect.GetTop();
		recttopx = rectx.GetTop();
		recttopn = rectn.GetTop();
		rectbottom = rect.GetBottom();
		rectbottomx = rectx.GetBottom();
		rectbottomn = rectn.GetBottom();
		rectleft = rect.GetLeft();
		rectleftx = rectx.GetLeft();
		rectleftn = rectn.GetLeft();
		rectright = rect.GetRight();
		rectrightx = rectx.GetRight();
		rectrightn = rectn.GetRight();
		rectwidth = rect.GetWidth();
		rectwidthx = rectx.GetWidth();
		rectwidthn = rectn.GetWidth();
		rectheight = rect.GetHeight();
		rectheightx = rectx.GetHeight();
		rectheightn = rectn.GetHeight();
		if (bNormalize)
			wxLogDebug(_T("-------------Test %d Quadrant %1.1f !!NORMALIZED!!-------------:"),ct+1,c[ct].quadrant);
		else
			wxLogDebug(_T("-------------Test %d Quadrant %1.1f-------------:"),ct+1,c[ct].quadrant);
		wxLogDebug(_T("rect created with topLeft(%d,%d) botRight(%d,%d):"),topLeft.x,topLeft.y,botRight.x,botRight.y);
		wxLogDebug(_T("x coord diff = %d  y coord diff = %d"),c[ct].xx - c[ct].x,c[ct].yy - c[ct].y);
		wxLogDebug(_T("CSize sz.x = %d CSize sz.y = %d"),sz.x,sz.y);
		wxLogDebug(_T("rectx created with topLeft(%d,%d),wxSize(%d,%d) from calc coords:"),c[ct].x,c[ct].y,sz.x,sz.y);
		wxLogDebug(_T("rectn created with topLeft(%d,%d) width %d, height %d direct from constants w & h:"),c[ct].x,c[ct].y,c[ct].w,c[ct].h);
		wxLogDebug(_T("   recttop    = %3d  recttopx    = %3d  recttopn    = %3d"),recttop,recttopx,recttopn);
		wxLogDebug(_T("   rectbottom = %3d  rectbottomx = %3d  rectbottomn = %3d"),rectbottom,rectbottomx,rectbottomn);
		wxLogDebug(_T("   rectleft   = %3d  rectleftx   = %3d  rectleftn   = %3d"),rectleft,rectleftx,rectleftn);
		wxLogDebug(_T("   rectright  = %3d  rectrightx  = %3d  rectrightn  = %3d"),rectright,rectrightx,rectrightn);
		wxLogDebug(_T("   rectwidth  = %3d  rectwidthx  = %3d  rectwidthn  = %3d"),rectwidth,rectwidthx,rectwidthn);
		wxLogDebug(_T("   rectheight = %3d  rectheightx = %3d  rectheightn = %3d"),rectheight,rectheightx,rectheightn);
		int i;
		i = 1;
	}
	// The Set methods are sensitive to the order they are called on a wxRect
	
	// set left and top first
	// a 5 by 5 rect with top left at 120,120
	// and bottom right at 125,115
	wxRect r1;
	r1.SetLeft(120);
	r1.SetTop(120);
	r1.SetRight(125);
	r1.SetBottom(125);
	// set right & bottom first, then left & top
	// to "same" points as above
	wxRect r2;
	r2.SetRight(125);
	r2.SetBottom(125);
	r2.SetLeft(120);
	r2.SetTop(120);

	// Only setting the bottom and right points
	wxRect r3;
	r3.SetRight(125);
	r3.SetBottom(115);

	wxLogDebug (_T("r1.x           = %d"),r1.x);
	wxLogDebug (_T("r1.y           = %d"),r1.y);
	wxLogDebug (_T("r1.GetRight()  = %d"),r1.GetRight());
	wxLogDebug (_T("r1.GetBottom() = %d"),r1.GetBottom());
	wxLogDebug (_T("r1.GetWidth()  = %d"),r1.GetWidth());
	wxLogDebug (_T("r1.GetHeight() = %d"),r1.GetHeight());
	
	wxLogDebug (_T("r2.x           = %d"),r2.x);
	wxLogDebug (_T("r2.y           = %d"),r2.y);
	wxLogDebug (_T("r2.GetRight()  = %d"),r2.GetRight());
	wxLogDebug (_T("r2.GetBottom() = %d"),r2.GetBottom());
	wxLogDebug (_T("r2.GetWidth()  = %d"),r2.GetWidth());
	wxLogDebug (_T("r2.GetHeight() = %d"),r2.GetHeight());
	
	wxLogDebug (_T("r3.x           = %d"),r3.x);
	wxLogDebug (_T("r3.y           = %d"),r3.y);
	wxLogDebug (_T("r3.GetRight()  = %d"),r3.GetRight());
	wxLogDebug (_T("r3.GetBottom() = %d"),r3.GetBottom());
	wxLogDebug (_T("r3.GetWidth()  = %d"),r3.GetWidth());
	wxLogDebug (_T("r3.GetHeight() = %d"),r3.GetHeight());

	int stop;
	stop = 1;

	// /////////////////// END OF WXWIDGETS LIBRARY CODE FRAGMENT TESTING ////////////////////////////////
	*/

	// Variable initializations below moved here from the App's constructor
	// /////////////////////////////////////////////////////////////////////
	// bUserSelectedFileNew below is used in wxWidgets version to reinit 
	// KB structures in view->OnCreate(). This is to allow for the fact that
	// with only 1 doc allowed in WX, it first deletes the current doc and
	// view and recreates new ones before calling OnNewDocument(). The MFC
	// version does not do this, but reuses its old doc and view pointers
	// under the CSingleDocTemplate() that MFC Adapt It uses.
	bUserSelectedFileNew = FALSE;	

	PathSeparator = wxFileName::GetPathSeparator();

	m_pDocManager = (wxDocManager*)NULL;// was originally in App constructor's preamble
	m_pMainFrame = (CMainFrame*)NULL;// was originally in App constructor's preamble
	// //////// Initializations above were originally in the App's constructor in the MFC version ///////

   
	const wxString name = wxString::Format(_T("Adapt_ItApp-%s"), wxGetUserId().c_str());
	// on my Windows machine name = "Adapt_ItApp-Bill Martin"
	// on my Linux machine name = "Adapt_ItApp-wmartin"
	
	m_pChecker = (wxSingleInstanceChecker*)NULL;
 	
	// Note: If we decide to prevent more than one instance of Adapt It WX from running at the same
	// time we can uncomment the code block below
	//{ // begin block for wxLogNull()
	//	wxLogNull logNo; // eliminates spurious "Deleted stale lock file '/home/user/Adapt_ItApp-wmartin' on Linux
	//	m_pChecker = new wxSingleInstanceChecker(name); // must delete m_checker in OnExit()
	//	wxASSERT(m_pChecker != NULL);
	//	if ( m_pChecker->IsAnotherRunning() )
	//	{
	//		wxMessageBox(_("Another program instance is already running for the current user, aborting."),_T(""),wxICON_ERROR);

	//		return FALSE;
	//	}
	//} // end of block for wxLogNull(), the ~wxLogNull destructor is called here

	// Is CoInitialize (initializes COM) available on Linux and Mac???
//#ifdef _UNICODE
	// RichEdit stuff not needed in wxWidgets version
	//  // this should be done early on. The AfxInitRichEdit() call will load version 1
	//	// so we must do its work manually ourselves to get version 3 used
	//   m_hmodRichEd1 = LoadLibrary(_T("riched20.dll")); // we've decided not to use Rich Edit 
	//						      // controls, but this can stay
//#endif

	// CoInitialize() won't compile on Linux
#ifdef USE_SIL_CONVERTERS
	CoInitialize(NULL); // initialize COM, so Unicode version can check for UTF8 when no BOM present
						// in data (rde: now needed for Ansi version as well, since it's needed 
						// for SilConverters)
#endif

	// whm added 7Dec05
	// Retrieve the path to the setup folder (i.e., the path the executable was launched from)
	// We store the path in m_setupFolder
	// WX NOTE: The MFC code will only work on some standard Window's 
	// configurations. It certainly will not work on Linux or other platforms, 
	// since only Windows uses C:\Program Files\somedir as a location for 
	// installation of user programs. Also, other platforms like Linux do not 
	// have drive letter designations such as C:, so hardcoding any path
	// prefixed with C:\ will not work across platforms.
	// For the wxWidgets version, we may be potentially running on any platform,
	// hence, so certain installed files such as books.xml file will be placed 
	// in platform-specific directories. For non-Windows platforms is is not 
	// sufficient to simply know where the application that is currently running 
	// resides at run time.
	// Different platforms expect certain kinds of files to be installed in 
	// certain expected locations.
	// 
	// The function FindAppPath(const wxString& argv0, const wxString& cwd, 
	//                          const wxString& appVariableName) 
	// was suggested by Julian Smart as one means to do this.
	// FindAppPath() makes the following checks in order:
	// 1. Checks for existance of a path associated with an environment variable
	//    name (passed in as the third parameter) [for situations in which the
	//    installation program set up an environment variable]
	// 2. Classic Mac just needs the current working directory when the 
	//    application starts, so the second parameter cwd is simply returned.
	// 3. If above checks fail (or are inappropriate because of a null string
	//    passed as third parameter), the function checks the commandline
	//    argv[0] string passed as third parameter argv0. By default 
	//    this argv0 should contain the path and name of the executable on 
	//    most computer systems. FindAppPath() checks whether arfv0 represents
	//    an absolute or relative path and insures that the executable actually
	//    exists there.
	// 4. If all above fails, FindAppPath() searches the computer's possible 
	//    PATH environment variable paths for the executable. If the executable
	//    is found, its path string is returned, otherwise the FindAppPath()
	//    just returns an empty string.
	//    
	// NOTE: regardless of whether Unicode or ANSI version is being run, the
	// string returned from FindAppPath() should be the location where we
	// should be able to find the books.xml file.

	// !!!!!!!!!!!!!!!!!!! SET UP SOME STANDARD PATHS BELOW !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// !!! testing only below


#if wxCHECK_VERSION(2, 7, 0)
	wxString resourcesDir; //,localizedResourcesDir;
	wxString dataDir, localDataDir, documentsDir;
	wxString userConfigDir, userDataDir, userLocalDataDir;
	wxString executablePath;
#ifndef __WXMSW__
	wxString installPrefix;
#endif
	wxStandardPaths stdPaths;
	resourcesDir = stdPaths.GetResourcesDir(); // GetResourcesDir() is new with wxWidgets 2.7.0
	wxLogDebug(_T("The wxStandardPaths::GetResourcesDir()  = %s"),resourcesDir.c_str());
	//localizedResourcesDir = stdPaths.GetLocalizedResourcesDir(_T("es")); // GetLocalizedResourcesDir() is new with wxWidgets 2.7.0
	//wxLogDebug(_T("The wxStandardPaths::GetLocalizedResourcesDir(_T(\"es\")) = %s"),localizedResourcesDir.c_str());
	dataDir = stdPaths.GetDataDir();
	wxLogDebug(_T("The wxStandardPaths::GetDataDir() = %s"),dataDir.c_str());
	localDataDir = stdPaths.GetLocalDataDir();
	wxLogDebug(_T("The wxStandardPaths::GetLocalDataDir() = %s"),localDataDir.c_str());
	documentsDir = stdPaths.GetDocumentsDir();
	wxLogDebug(_T("The wxStandardPaths::GetDocumentsDir() = %s"),documentsDir.c_str());
	userConfigDir = stdPaths.GetUserConfigDir();
	wxLogDebug(_T("The wxStandardPaths::GetUserConfigDir() = %s"),userConfigDir.c_str());
	userDataDir = stdPaths.GetUserDataDir();
	wxLogDebug(_T("The wxStandardPaths::GetUserDataDir() = %s"),userDataDir.c_str());
	userLocalDataDir = stdPaths.GetUserLocalDataDir();
	wxLogDebug(_T("The wxStandardPaths::GetUserLocalDataDir() = %s"),userLocalDataDir.c_str());
	executablePath = stdPaths.GetExecutablePath();
	wxLogDebug(_T("The wxStandardPaths::GetExecutablePath() = %s"),executablePath.c_str());
#ifndef __WXMSW__
	installPrefix = stdPaths.GetInstallPrefix();
	wxLogDebug(_T("The wxStandardPaths::GetInstallPrefix() = %s"),installPrefix.c_str());
#endif
#endif // #if wxCHECK_VERSION(2, 7, 0)

	m_setupFolder = FindAppPath(argv[0], wxGetCwd(), _T(""));
	m_appInstallPathOnly = FindAppPath(argv[0], wxGetCwd(), _T(""));
	// On Windows the m_appInstallPathOnly will be something like (if installed to default location):
	// "C:\Program Files\Adapt It WX" or "C:\Program Files\Adapt It WX Unicode\"
	// On Linux/GTK the m_setupFolder will be something like:
	// "/usr/bin/" or "/usr/local/bin/"
	// On the Mac the m_setupFolder will be something like:
	// "/Applications"

	// The m_xmlInstallPath stores the path where the AI_USFM.xml and books.xml files are installed on the
	// given platform.
	// On wxMSW: "C:\Program Files\Adapt It WX\ or C:\Program Files\Adapt It WX Unicode\"
	// On wxGTK: "/usr/share/adaptit/"     [adaptit here is the name of a directory]
	// On wxMac: "AdaptIt.app/Contents/Resources"   [bundle subdirectory] ??? TODO: check this location
	m_xmlInstallPath = GetDefaultPathForXMLControlFiles();

	// The m_localizationInstallPath stores the path where the <lang> localization files are installed on
	// the given platform.
	// On wxMSW: "C:\Program Files\Adapt It WX\Languages\ or C:\Program Files\Adapt It WX Unicode\Languages\"
	// On wxGTK: "/usr/share/locale/"    which then contains multiple "<lang>/LC_MESSAGES/adaptit.mo"
	// On wxMac: "AdaptIt.app/Contents/Resources/locale"   [bundle subdirectory] // this is where Poedit
	// puts its localization files.
	m_localizationInstallPath = GetDefaultPathForLocalizationSubDirectories();

	// The m_helpInstallPath stores the path where the help files are installed on the given platform.
	// On wxMSW: "C:\Program Files\Adapt It WX\ or C:\Program Files\Adapt It WX Unicode\"
	// On wxGTK: "/usr/share/adaptit/help/"  containing: common/.gif and .css
	//                                     containing: <lang>/  .html .hhp .hhc etc
	// On wxMac: AdaptIt.app/Contents/SharedSupport   [bundle subdirectory]  ??? TODO: check this location
	m_helpInstallPath = GetDefaultPathForHelpFiles();
	// Note: The m_htbHelpFileName is also determined in  GetDefaultPathForHelpFiles() call above

	// !!!!!!!!!!!!!!!!!!! SET UP SOME STANDARD PATHS ABOVE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	
	// make a timesettings struct for autosaving feature & put some default values in it
	// (after first launch, values will come from the m_pConfig file settings instead)
	wxDateTime time;
	time.SetToCurrent();
	m_timeSettings.m_tLastDocSave = time;
	m_timeSettings.m_tLastKBSave = time;
	//wxTimeSpan tsDoc(0,0,10,0); // 0 days, 0 hours, 10 minutes, 0 seconds - MFC version
	//wxTimeSpan tsKB(0,0,15,0); // 0 days, 0 hours, 15 minutes, 0 seconds - MFC version
	// Note: wxWidgets' time span has different categories!! (we probably don't need to save
	// data files every 10 or 15 seconds!!). Note also: GetSeconds() and GetMinutes() also
	// work differently in the wxWidgets version!!
	wxTimeSpan tsDoc(0,10,0,0); // 0 hours, 10 minutes, 0 seconds, 0 milliseconds - wxWidgets
	wxTimeSpan tsKB(0,15,0,0); // 0 hours, 15 minutes, 0 seconds, 0 milliseconds - wxWidgets
	m_timeSettings.m_tsDoc = tsDoc;
	m_timeSettings.m_tsKB = tsKB;
	m_nMoves = 100; // initial default, save every 100 phrase box moves
	m_bIsDocTimeButton = TRUE; // initial default, save according to time interval, not moves

	// Change the registry key to something appropriate
	// MFC used: SetRegistryKey(_T("SIL-PNG Applications"));
	// wxConfig (below) stores the key "Adapt_It_WX" in HKEY_USERS "Software" 
	// branch of the registry (under Windows), or in a hidden text config file under 
	// Unix/Linux (named .Adapt_It_WX).
	// wxConfig provides an interface which hides the differences between the Windows 
	// registry and the standard Unix text format configuration files. With this wxConfig
	// class we could store various kinds of string and int information, but we will
	// mainly use it for the file history (MRU) list and the ui_language setting. 
	// Later we may also use m_pConfig to set other things like the default path.
	m_pConfig = new wxConfig(_T("Adapt_It_WX")); // must delete m_pConfig in OnExit()
	wxASSERT(m_pConfig != NULL);
	// Note: We'll use "Adapt_It_WX" to distinguish the wxWidgets version from MFC version 
	// 
	// We also need another wxConfig object that we can use to query whether the SIL Converters has
	// been installed (on Windows) machines.
	m_pConfigSIL = new wxConfig(_T("SIL"),wxEmptyString,wxEmptyString,wxEmptyString,wxCONFIG_USE_LOCAL_FILE);
	wxASSERT(m_pConfigSIL != NULL);

	// Retrieve the m_bUseToolTips setting from m_pConfig if it exists.
	{
	// begin a local block to restrict scope of wxLogNull logNo
	wxString oldPath = m_pConfig->GetPath(); // is always absolute path "/Recent_File_List"
	m_pConfig->SetPath(_T("/Settings"));
	wxLogNull logNo;	// eliminates spurious message from the system: "Can't read value of key
						//'HKCU\Software\Adapt_It_WX\Settings' Error" [valid until end of this block]
	m_pConfig->Read(_T("use_tooltips"), &m_bUseToolTips);
	// restore the oldPath back to "/Recent_File_List"
	m_pConfig->SetPath(oldPath);
	// end of local block for wxLogNull logNo
	}

	// Retrieve the m_nTooltipDelay time from m_pConfig if is exists
	{
	// begin a local block to restrict scope of wxLogNull logNo
	// Here the oldPath will be set to "\" or something like that, but normally will be set to path "/Recent_File_List" (see farther below).
	wxString oldPath = m_pConfig->GetPath(); 
	m_pConfig->SetPath(_T("/Settings"));
	wxLogNull logNo;	// eliminates spurious message from the system: "Can't read value of key
						//'HKCU\Software\Adapt_It_WX\Settings' Error" [valid until end of this block]
	m_pConfig->Read(_T("time_tooltips_displayed_ms"), &m_nTooltipDelay);
	// TODO: Uncomment line below when figure out why it seems to disable tooltip function in most of
	// the interface
	//wxToolTip::SetDelay(m_nTooltipDelay);
	// restore the oldPath back to "/Recent_File_List"
	m_pConfig->SetPath(oldPath);
	// end of local block for wxLogNull logNo
	}

	m_bConfigFileHasFontEncodingInfo = FALSE;	// normally false for config files written by
												// the MFC version; set to true for those 
												// written by the wx version

	// Get system locale and encoding info:
	m_systemEncoding = wxLocale::GetSystemEncoding();			// Windows: m_systemEncoding = wxFONTENCODING_CP1252
																//  Ubuntu: m_systemEncoding = wxFONTENCODING_UTF8
																//     Mac: m_systemEncoding = wxFONTENCODING_MACROMAN
	m_systemEncodingName = wxLocale::GetSystemEncodingName();	// Windows: m_systemEncodingName = "windows-1252"
																//  Ubuntu: m_systemEncodingName = "UTF-8"
																//     Mac: m_systemEncodingName = ???
	m_systemLanguage = wxLocale::GetSystemLanguage();			// Windows: m_systemLanguage = 58
																//  Ubuntu: m_systemLanguage = 58
																//     Mac: m_systemLanguage = 58
	m_languageInfo = wxLocale::GetLanguageInfo(m_systemLanguage);
	//wxLogDebug("m_languageInfo->Description = %s",m_languageInfo->Description.c_str()); // "English (U.S.)"
	//wxLogDebug("m_languageInfo->CanonicalName = %s",m_languageInfo->CanonicalName.c_str()); // "en_US"
	//wxLogDebug("m_languageInfo->Language = %d",m_languageInfo->Language); // 58 (both Windows and Ubuntu)
//#ifdef __WIN32__
//	wxLogDebug("m_languageInfo->WinLang = %d",m_languageInfo->WinLang); // Windows: 9
//	wxLogDebug("m_languageInfo->WinSublang = %d",m_languageInfo->WinSublang); // Windows: 1
//#endif
//	wxLogDebug("m_languageInfo->LayoutDirection = %d",m_languageInfo->LayoutDirection); //wxLayout_LeftToRight (both)
	
	wxASSERT(!m_appInstallPathOnly.IsEmpty()); //wxASSERT(!m_setupFolder.IsEmpty());

	// NOTE: We determine Adapt It's user interface language here early in OnInit() before the main
	// frame and other visible parts of the interface are shown to the user. If the user changes 
	// the interface language choice at this early point in the running application, the app won't 
	// have to be restarted to fully exhibit the localized interface.
	// 
	// Get any interface language data stored in the m_pConfig (registry/hidden settings file); 
	// ProcessUILanguageInfoFromConfig adds any previously stored user defined languages to 
	// the current locale, and returns the current localization info in currLocalizationInfo.
	currLocalizationInfo = ProcessUILanguageInfoFromConfig(); // this needs to come after m_languageInfo is defined above

	// On first run of Adapt It (when no previous interface language choice has been made), 
	// or if the user holds down the ALT key while running the application, the following 
	// ChooseInterfaceLanguage() call will check to see if localizations other 
	// than the default system language are present on the user's computer. If so, 
	// ChooseInterfaceLanguage() will present the user with a "Select your preferred 
	// language for Adapt It's menus and other messages" dialog in which an alternative
	// interface language choice can be made. For first run of Adapt It, the default choice 
	// is "Use default language" (wxLANGUAGE_DEFAULT). 
	// The only choices available in the dialog are languages for which actual localization
	// subfolders exist in the localization path and contain an <appName>.mo localization 
	// file. 
	// Once the user makes the choice the interface is set to use that language.
	// After the first run of Adapt It, the user's choice of UI language is saved in the 
	// registry on Windows or a hidden settings file on Linux/Mac, and the user won't be 
	// asked again at program startup - but the user can later change the interface language
	// choice from within the app via View | "Choose Interface Language...".

	if (currLocalizationInfo.curr_UI_Language == wxLANGUAGE_UNKNOWN || wxGetKeyState(WXK_ALT))
	{
		// can disregard return bool value of ChooseInterfaceLanguage() here
		ChooseInterfaceLanguage(firstRun);	// calls CChooseLanguageDlg and insures that currLocalizationInfo's 
											// curr_UI_Language is something other than wxLANGUAGE_UNKNOWN
	}

	// for testing below:
	//wxString pathToLocaleSubDir;
	//pathToLocaleSubDir = GetDefaultPathForLocalizationSubDirectories();

	// InitializeLanguageLocale() below deletes any exising wxLocale object and creates a new wxLocale
	// for the currently selected language (using the non-default wxLocale constructor).
	// The currLocalizationInfo struct was initialized with the interface language of choice above, so
	// now we can create the wxLocale object
	if (!InitializeLanguageLocale(currLocalizationInfo.curr_shortName, currLocalizationInfo.curr_fullName, currLocalizationInfo.curr_localizationPath))
	{
		// Loading of localization catalog failed. This means that either m_pLocale->AddCatalog(GetAppName())
		// or m_pLocale->IsLoaded(GetAppName()) failed in InitializeLanguageLocale().
		// Notify user of the problem.
		wxString msg;
		// We use the _T() macro below since localized translations of these messages would never be
		// shown anyway (the localization failed to load).
		msg = msg.Format(_T("Adapt It failed to initialize and load the localization for the %s language at the following path:\n%s\nYou may need to re-install the localization files or tell Adapt It where they are located by selecting the \"Change Interface Language..\" item on the View menu."),currLocalizationInfo.curr_fullName.c_str(),currLocalizationInfo.curr_localizationPath.c_str());
		wxMessageBox(msg,_T("Previous localization file not found"),wxICON_WARNING);
	}

	wxString EncodingName = wxFontMapper::Get()->GetEncodingName(m_systemEncoding); 
																// Windows: EncodingName = "windows-1252"
																//  Ubuntu: EncodingName = "UTF-8"
	wxFontEncoding fontenc = wxFontMapper::Get()->GetEncodingFromName(EncodingName); 
																// Windows: fontenc = wxFONTENCODING_CP1252
																//  Ubuntu: fontenc = wxFONTENCODING_UTF8
	//static const wxChar** encNames = wxFontMapper::Get()->GetAllEncodingNames(fontenc);
	bool fontAvail;
	fontAvail = wxFontMapper::Get()->IsEncodingAvailable(fontenc);	
																// Windows: fontAvail = true
																//  Ubuntu: fontAvail = true
	//wxFontEncoding altFontEnc;
	//wxString facename = _T("");
	//bool gotAltFont;
	//gotAltFont = wxFontMapper::Get()->GetAltForEncoding(fontenc, &altFontEnc, facename, false);
	//															// Windows: gotAltFont = true
	//															// Windows: altFontEnc = wxFONTENCODING_ISO8859_1
	//															//  Ubuntu: gotAltFont = false
	//															//  Ubuntu: (none)

    m_pDocManager = new wxDocManager; // must delete m_pDocManager in OnExit()
	wxASSERT(m_pDocManager != NULL);

	//LoadStdProfileSettings(9);  // Load standard INI file options (including MRU) - MFC version
	// Set things up for file history (MRU) list
	// File History is loaded below after Main Menu is created
	wxString strOldPath = m_pConfig->GetPath();// store old wxConfig path in case we need it
	// whm Note: Recently used files (MRU) that get recorded in the registry/hidden settings file
	// are not really compatible between Unicode and Ansi versions. It may well be the case that
	// the user switches between the Unicode and Ansi versions. In that case we don't want there
	// to be a mix of files in the MRU, some using Unicode and some Ansi. Therefore I'm setting a
	// different registry/hidden settings file key for Unicode. In either case the path that is
	// set for m_pConfig will be one or the other as default so that wxFileHistory will use it
	// appropriately.
#ifdef _UNICODE
	m_pConfig->SetPath(_T("/Recent_File_List_Unicode"));// set wxConfig path in registry/hidden settings file
#else
	m_pConfig->SetPath(_T("/Recent_File_List"));// set wxConfig path in registry/hidden settings file
#endif
	// wx note: In SetPath above we should use the _T() macro rather than _() so the config string
	// stays the same regardless of interface language chosen. Also, since the "Recent_File_List"
	// key is probably the most commonly used, we'll always leave the "Path" for wxConfig::SetPath(Path)
	// set as _T("/Recent_File_List"). Whenever we change a setting other than an item
	// under Recent_File_List, we'll save the oldPath and restore it after doing the other setting.

	// Create doc template to manage doc/view framework.
	// Since wxWidgets data files probably won't be compatible with those produced
	// by MFC, we'll initially use .adx instead of .adt for wxWidgets AI documents
    wxDocTemplate* pDocTemplate;
	
	pDocTemplate = new wxDocTemplate(m_pDocManager, m_FrameAndDocProgramTitle, _T(".xml"), _T(""), _T("xml"), // previously ".adx" and "adx"
                                     _T("Adapt It Document"), _T("Adapt It View"),
                                     CLASSINFO(CAdapt_ItDoc),
                                     CLASSINFO(CAdapt_ItView));
	// pDocTemplate above will be destroyed when m_pDocManager is deleted in OnExit()
	wxASSERT(pDocTemplate != NULL);
	// Note: We could have another wxDocTemplate instance for plain text documents if we
	// wanted them to be managed by the doc/view framework.

	// The following are for HTML Help
    m_pHelpController = new wxHtmlHelpController(wxHF_DEFAULT_STYLE | wxHF_OPEN_FILES);
	wxHelpControllerHelpProvider* provider = new wxHelpControllerHelpProvider;
	wxHelpProvider::Set(provider);
    
	// Get/Set Help info (window size etc) in the config object.
	// Help window settings are saved in the registry/.Settings folder that stays the default and is
	// called "Recent_File_List". There are 10 settings in that folder plus UseConfig() creates a 
	// subfolder within "Recent_File_List" called "wxHtmlWindow" which contains another 10 help font settings.
	m_pHelpController->UseConfig(m_pConfig);

	// Required for images in the online documentation
    wxInitAllImageHandlers(); // the help sample program does this, although it is not documented anywhere in wxWidgets!
	// Note: There are also individual handlers, i.e., wxImage::AddHandler(new wxGIFHandler), 
	// wxFileSystem::AddHandler(new wxZipFSHandler), etc., but ones used here are what the help sample uses.
	
    // Note: The docs for wxHelpController say, "Note that if you use .zip or .htb formats for your
    // books, you must add this line to your application initialization: wxFileSystem::AddHandler(new
    // wxArchiveFSHandler); or nothing will be shown in your help window."
	wxFileSystem::AddHandler(new wxArchiveFSHandler); // docs say to use this one


    m_pMainFrame = new CMainFrame(m_pDocManager, (wxFrame*) NULL, -1, m_FrameAndDocProgramTitle,
                                wxPoint(0, 0), wxSize(735, 500),
                                wxDEFAULT_FRAME_STYLE 
								| wxFRAME_NO_WINDOW_MENU);	// wxFRAME_NO_WINDOW_MENU removes
															// the 'Window' menu from the
															// main frame that doc/view normally
															// places there. It is also removed
															// from any child view windows.
	// m_pMainFrame above will be destroyed when m_pDocManager is deleted in OnExit()
	wxASSERT(m_pMainFrame != NULL);

	// Notes: about Menu differences on the Mac:
	// 1. On the Mac, the "About" menu item should go in a particular place (in the Apple menu on 
	// Mac OS 8/9, or in the program menu on Mac OS X). wxMac will automatically move it to the 
	// proper menu the help menu's "About" identifier is wxID_ABOUT (I changed it from MFC's 
	// ID_APP_ABOUT to wxID_ABOUT), to automatically make the menu adjustments for the Mac.
	// 2. On the Mac, the "Preferences" menu also goes in a particular place (in the AdaptIt menu
	// to the left of the File menu). Preferences is moved there automatically if the standard
	// wxID_PREFERENCES is used, so I changed the MFC identifier ID_EDIT_PREFERENCES to 
	// wxID_PREFERENCES.
	// 3. On the Mac, the "Help" menu is handled specially too according to Mac custom when the
	// Help menu identifier the menu uses is wxID_HELP; this one was already set correctly.
	// 4. Keyboard shortcuts for menu items on the Mac are also automatically changed from "CTRL" 
	// on the menu to use the clover leaf shaped Command key symbol plus the appropriate key.


	// Give the main frame an icon (frame icons and task bar icons use 16x16 icons)
	// This xpm format should work for all platforms. It embeds the icon resource inside
	// the executable file, so no need to have to hunt for it from an external resource file.
	/* XPM */ 
	static const char * xpm_data[] = {
	"16 16 10 1",
	" 	c None",
	"!	c #FFFF00",
	"#	c black",
	"$	c #FF0000",
	"%	c #808000",
	"&	c #808080",
	"'	c #00FFFF",
	"(	c #00FF00",
	")	c #000080",
	"*	c #C0C0C0",
	"!!!!!!!!!!!!!!!!",
	"!##!!!!!!!!!!!!!",
	"#$!#!!!!!!!!!!!!",
	"#$!#!!!!!!!!!!!!",
	"####!!%%%%%!!!!!",
	"#$!#!%#####&!!!!",
	"#$!#!%#'('#%!!!!",
	"#$!#!%#'''#%!!!!",
	"!!!!!%#(((#%!!!!",
	"!!&!!%#####%!!!!",
	"!!&)!%*****%!###",
	")))))%**))%%!!#!",
	"!!&)!%*****%!!#!",
	"!!&!!%%%%%%%!!#!",
	"!!!!!%%!!!%%!!#!",
	"!!!!!!!!!!!!!###"};
	wxIcon icon( xpm_data );
	m_pMainFrame->SetIcon(icon); 
	//m_pMainFrame->SetIcon(wxIcon(wxICON(adaptit)));	// "adaptit" is a resource name
													// of an ICON defined in Adapt_It.rc
	// Only allow one document at a time to be open
	m_pDocManager->SetMaxDocsOpen(1);

	// testing !!!
	//m_pMainFrame->Connect(wxID_ANY, wxEVT_KEY_DOWN, (wxObjectEventFunction) &CAdapt_ItApp::OnShiftDown);
	// testing !!!

	// Get the File Menu, tell the doc manager that we want the File History on the
	// File Menu, and Load the File History (MRU) to it
	wxMenu* pFileMenu = m_pMainFrame->GetMenuBar()->GetMenu(0);
	wxASSERT(pFileMenu != NULL);
	m_pDocManager->FileHistoryUseMenu(pFileMenu);
	// This must come after Main Menu is created and FileHistoryUseMenu call
	m_pDocManager->FileHistoryLoad(*m_pConfig); // Load the File History (MRU) list from *m_pConfig

	// The following commands probably have equivalents in wxWidgets' wxMimeTypesManager.
	// However, the wx version does not use serialized .adt type data files. I don't think it 
	// would necessarily be a good thing for users to be able to open AI documents by double 
	// clicking on Adapt It's data files. They are now stored in .xml files which we wouldn't want
	// to associate with the Adapt It application anyway. Moreover, when I try double clicking the MFC 
	// version's .adt files, I get an error from the system that says, "Adapt_It MFC 
	// Application has encountered a problem and needs to close. We are sorry for the inconvenience."
	// Therefore, I'll not convert the following MFC function calls.
	// Enable DDE Execute open
	//EnableShellOpen();
	//RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	//CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);
	// wx version Note: The wx docs on wxApp::OnInit() say, "Notice that if you want 
	// to use the command line processing provided by wxWidgets you have to call the base class 
	// version in the derived class OnInit()."
	static const wxCmdLineEntryDesc cmdLineDesc[] = 
	{
		{ wxCMD_LINE_SWITCH, _T("h"), _T("help"), _T("Command Line operation not implemented!"), 
			wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP  },
		{ wxCMD_LINE_NONE }
	};

   // Note: In the MFC version, InitiInstance() sets up CCommandLineInfo cmdInfo.
   // InitInstance() then calls ProcessShellCommand(cmdInfo) which has a switch
   // statement which switches on CCommandLineInfo::FileNew, and calls the app's
   // OnFileNew() to initiate the doc/view creation process at program startup.
   m_pParser = new wxCmdLineParser(cmdLineDesc, argc, argv);
   if (m_pParser->Parse())
     return false;

	// TODO: The command line parser above is not fully implemented.
	// As far as I know Adapt It has no need for command line arguments nor any need
	// to parse them, so I'm not going to implement this even though wxWidgets has
	// a sophisticated wxCmdLineParser class. The one thing that ParseCommandLine()
	// does in the MFC version is to start the application with a new View as though 
    // File|New were executed giving the app access to valid pView and pDoc pointers.
    // We provide the equivalent below where we call OnFileNew() directly.

	// for richer memory leak dumps
    // whm - wxWidgets has its own memory leak facilities built into its library
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// some extra initializations, required for Adapt It
	m_bKBReady = FALSE;
//#ifdef _UNICODE
//	RenameNRtoUnicode(); // not used in wx version
//#endif
	EnsureWorkFolderPresent(); 
	// m_workFolderPath is set in here, and this becomes the current working directory.
	// m_localPathPrefix is also set in here and is used in MakeForeignBasicConfigFileSafe() below.
	// Bruce added 15Mar04 a parameter to EnsureWorkFolderPresent(m_localizedMyDocsPath), but
	// it's not needed for the wxWidgets version.
	// The m_workFolderPath typically is set to:
	// On Windows:	"C:\Documents and Settings\<UserName>\My Documents\Adapt It (Unicode) Work" 
	// On Linux:	"/home/<UserName>/Adapt It (Unicode) Work" 
	// On MacOSX:	"/Users/<UserName>/Adapt It (Unicode) Work"

	// in case the user is using a copied (ie. 'foreign') basic configuration file from some else's
	// adaptation work on another machine, check and fix any bad path names (actually, only 5 of
	// them need to be potentially fixed, the others won't crash anything because of fail-safe
	// code elsewhere in the app)
	wxString configFName = szBasicConfiguration + _T(".aic");
	MakeForeignBasicConfigFileSafe(configFName,m_workFolderPath);

	m_backgroundMode = wxSOLID;	// whm added 6July2006 Set to wxSOLID here in App's OnInit because wxDC's 
								// SetTextBackground uses by default background mode of wxTRANSPARENT
								// which doesn't show up on screen regardless of what color we set
								// with SetTextBackground(). Each call to SetTextBackground should have
								// a prior call to SetBackgroundMode(m_backgroundMode), in which the
								// m_backgroundMode = wxSOLID.

	// BEW added 06Mar06 code to MFC version to get the desktop's rectangle size and check for
	// spurious overwriting of the coords of the window location (that has happened on some rare
	// occasions). Bruce first created an off-screen temporary static control window to get a
	// valid CWnd*, but there is a simpler way in wxWidgets which I'm using here.
	wxRect desktopWndRect = wxGetClientDisplayRect();
	// once set, these two points should not change while the app runs
	wndTopLeft = wxPoint(desktopWndRect.GetLeft(),desktopWndRect.GetTop());
	wndBotRight = wxPoint(desktopWndRect.GetRight(),desktopWndRect.GetBottom());

	// The original position of MFC's Initialize() was here. In wx version it is divided up
	// into InitializeFonts() and InitializePunctuation() which are both called earler in 
	// OnInit() above.
	
	// create the global Page Setup Dialog Data
	pPgSetupDlgData = new wxPageSetupDialogData; // must delete in OnExit()
	wxASSERT(pPgSetupDlgData != NULL);
	// create the global Print Data
	pPrintData = new wxPrintData; // must delete in OnExit()
	wxASSERT(pPrintData != NULL);
	// Set some defaults for the page setup dialog so they will show as defaults if the user
	// accesses the page setup dialog.
	// Margin MM values determined using the less precise config_only_thousandthsInchToMillimetres conversion factor
	pPgSetupDlgData->SetMarginTopLeft(wxPoint(m_marginLeftMM,m_marginTopMM));
	pPgSetupDlgData->SetMarginBottomRight(wxPoint(m_marginRightMM,m_marginBottomMM));
	// Paper size MM values determined using the more precise thousandthsInchToMillimetres conversion factor
	pPgSetupDlgData->SetPaperSize(wxSize(m_pageWidthMM,m_pageLengthMM)); 
	//SetPaperId() overrides the explicit paper dimensions passed in wxPageSetupDialogData::SetPaperSize()
	pPgSetupDlgData->SetPaperId(wxPAPER_A4); // default to A4
	// 
	wxSize testSize;
	testSize = pPgSetupDlgData->GetPaperSize();
	// Set default orientation (Portrait)
	pPrintData->SetOrientation(wxPORTRAIT);
	// copy the printData info from the pageSetupDialog
	(*pPrintData) = pPgSetupDlgData->GetPrintData();

	// //////////////////////////////////////////////////////////////////////////////////////////////
	// Since we are about to read the config files, any data structures containing data that might be
	// changed from the reading of config files need to be created by this point in OnInit().
	// //////////////////////////////////////////////////////////////////////////////////////////////
	
	// If the user holds the SHIFT-DOWN key here at program startup she can bypass the normal reading
	// of Adapt It's config file settings, forcing the program to load a set of default settings
	// instead.
	// The following routines can be forced to defaults by the user holding down the SHIFT-KEY:
	// 1. Basic Program-wide Adapt It Settings. See GetBasicConfigFileSettings() which is only
	//    called from this OnInit() method (see below).
	// 2. Project Settings. See GetProjectConfiguration() which is called from OnOpenDocument(),
	//    from DoUnpackDocument(), from the Project Page's OnWizardPageChanging (when an existing
	//    document was selected), and the Open Existing Project Dialog that gets called from
	//    AccessOtherAdaptionProject() while doing a transform glosses into adaptations operation.
	bool bConfigFilesRead;
	bConfigFilesRead = GetBasicConfigFileSettings(); // GetBasicConfigFileSettings detects SHIFT-DOWN
	if (bConfigFilesRead)
	{
		int found = m_punctuation[0].Find(_T('\'')); // look for vertical ordinary quote (ie. apostrophe)
		if (found >= 0)
			m_bSingleQuoteAsPunct = TRUE;
		else
			m_bSingleQuoteAsPunct = FALSE;
		found = m_punctuation[0].Find(_T('\"')); // look for vertical ordinary double-quote
		if (found >= 0)
			m_bDoubleQuoteAsPunct = TRUE;
		else
			m_bDoubleQuoteAsPunct = FALSE;
		// Convert and set the PaperSizeCode, and Print margins that were read from the basic config file
		pPgSetupDlgData->SetPaperId(MapMFCtoWXPaperSizeCode(m_paperSizeCode));
		pPgSetupDlgData->SetMarginTopLeft(wxPoint(m_marginLeftMM,m_marginTopMM));
		pPgSetupDlgData->SetMarginBottomRight(wxPoint(m_marginRightMM,m_marginBottomMM));
		pPgSetupDlgData->SetPaperSize(wxSize(m_pageWidthMM,m_pageLengthMM));
		// Note: SetPageOrientation() sets the page orientation in our pPrintData global 
		SetPageOrientation(m_bIsPortraitOrientation);
		
		// initialize default path for cc tables
		m_tableFolderPath[0] = m_defaultTablePath;
		m_tableFolderPath[1] = m_defaultTablePath;
		m_tableFolderPath[2] = m_defaultTablePath;
		m_tableFolderPath[3] = m_defaultTablePath;
	}

	// TODO: Before implementing the ProcessShellCommand() below go back and do 
	// the ParseCommandLine() above.
	// Dispatch commands specified on the command line
	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;

	// For all intents and purposes, MFC's ProcessShellCommand() functions within
	// Adapt It to simply initiate a call to the App's OnFileNew() method in order
	// to create a doc/view as a stand-in at program startup.
	// We can do the same in the wx version by also calling the App's OnFileNew()
	// method. This will provide for a Doc and View for the opening canvas on the 
	// Main Frame. This Doc and View insure there are valid pDoc and pView pointers 
	// obtainable by routines like OnFilePageSetup() which need access to them when 
	// no user selectable Doc and View are loaded, such as when the user cancels at 
	// the opening wizard or closes the document or project. App->OnFileNew() simply
	// calls m_pDocManager->OnFileNew() which in turn calls CreateDocument(). Note 
	// that the CreateDocument() call by itself actually sufficient, since it
	// also creates an associated view. It is therefore not necessary to call 
	// CreateView() in addition to CreateDocument. In fact, when I did include a 
	// CreateView() call below, it was causing problems, because OnCreate() was 
	// then associating a different pointer with the view, causing some view members 
	// (like m_pBundle) to have invalid pointers at some locations in the code 
	// resulting in crashes.
	//CAdapt_ItDoc* pDoc;
	//pDoc = (CAdapt_ItDoc*)m_pDocManager->CreateDocument(m_workFolderPath,wxDOC_NEW);
	// !!!!!!!!!!!!!!!!!
	wxCommandEvent event = wxID_NEW;
	OnFileNew(event);
	// !!!!!!!!!!!!!!!!!
	//CAdapt_ItView* pView;
	//pView = (CAdapt_ItView*)m_pDocManager->CreateView(pDoc,wxDOC_NEW);

	// The differences between MFC and wxWidgets' implementation of doc/view
	// coupled with Adapt It's design in which initializations are scattered
	// over many code locations make for a real messy conversion process.
	//
	// In the MFC version, OnInitialUpdat() is "called by the framework after the 
	// view is first attached to the document, but before the view is initially 
	// displayed." To get that equivalency here in the wxWidgets version, I 
	// determined by tracing through the MFC code exactly when OnInitialUpdate() 
	// is called or not called in the following startup/opening scenarios:
	// (which is not really what the MFC docs say about OnInitialUpdate)
	//		Scenario:							OnInitialUpdate() called:
	// 1. File|New...									YES
	// 2. File|Open...									NO
	// 3. File|Start Here...(more work on existing doc)	NO
	// 4. File|Startup Wizard..							NO
	// 5. Load file from MRU list on File Menu			YES
	// 6. After transform into glosses, File|New		YES (same as File|New...)
	//	  After transform into glosses, File|Open		NO
	// 7. Consistency Check...							NO
	// Summary: Taking the actual MFC code to contain the necessary conditions then,
	// OnInitialUpdate() and the various initializations therein, should be called 
	// for only two main scenarios:
	// (1) for File | New Menu selection (OnFileNew), and 
	// (2) when loading a file from the MRU list. 
	// OnFileNew() above calls CreateDocument() which eventually calls OnNewDocument()
	// which calls OnInitialUpdate(). Hence, we do not need to explicityl call 
	// OnInitialUpdate() here from OnInit(). 
	//CAdapt_ItView* pView = (CAdapt_ItView*) GetView();
	//wxASSERT(pView != NULL);
	// !!!!!!!!!!!!!!!!!
	//pView->OnInitialUpdate();
	// !!!!!!!!!!!!!!!!!

	// Creation of targetbox moved to the View's OnCreate() method
	// Create the target box using custom constructor
	// WX Note: m_pTargetBox is a child of the view's canvas (which itself
	// is derived from wxScrolledWindow. As a child of the canvas window,
	// m_pTargetBox will be automatically destroyed when pView->canvas is
	// destroyed during doc/view's cleanup.
	//
	//m_pTargetBox = new CPhraseBox(pView->canvas, -1,_T(""), //pApp->m_targetBox.Create(canvas, IDW_TARGET_EDITBOX,_T(""), 
	//		wxDefaultPosition,wxDefaultSize,
	//		wxTAB_TRAVERSAL | wxSIMPLE_BORDER | wxWANTS_CHARS); // Check wxCLIP_CHILDREN and wxWANTS_CHARS
	//		//(long)0); // Check wxCLIP_CHILDREN and wxWANTS_CHARS
	//		//wxDefaultValidator, _T("targetBox"));
	//m_pTargetBox->SetValue(_T(""));
	// hide and disable the target box until input is expected
	//m_pTargetBox->Hide();
	//m_pTargetBox->Enable(FALSE); 

	/////////////////////////////////////////////////////////////////////////////
    // These toggle menu items should be initially set as follows (TRUE=checked; FALSE=unchecked):
	// whm verified 2Sep06 that at least some are needed to be pre-set as follows in order for the
	// calls to menu handlers to operate successfully in the View's OnInitialUpdate() function where
	// the initial state of toggled toolbar buttons is set.
    m_pMainFrame->GetMenuBar()->Check(ID_VIEW_TOOLBAR, TRUE);
    m_pMainFrame->GetMenuBar()->Check(ID_VIEW_STATUS_BAR, TRUE);
    m_pMainFrame->GetMenuBar()->Check(ID_VIEW_COMPOSE_BAR, FALSE);
    m_pMainFrame->GetMenuBar()->Check(ID_COPY_SOURCE, TRUE);
    //m_pMainFrame->GetMenuBar()->Check(ID_FIT_WINDOW, TRUE);
    m_pMainFrame->GetMenuBar()->Check(ID_MARKER_WRAPS_STRIP, TRUE);
    m_pMainFrame->GetMenuBar()->Check(ID_USE_CC, FALSE);
    m_pMainFrame->GetMenuBar()->Check(ID_ACCEPT_CHANGES, FALSE);
    //m_pMainFrame->GetMenuBar()->Check(ID_TOOLS_AUTO_CAPITALIZATION, FALSE);
    m_pMainFrame->GetMenuBar()->Check(ID_ADVANCED_ENABLEGLOSSING, FALSE);
    m_pMainFrame->GetMenuBar()->Check(ID_ADVANCED_GLOSSING_USES_NAV_FONT, FALSE);
    //m_pMainFrame->GetMenuBar()->Check(ID_ADVANCED_REMOVE_PUNCT_WHEN_STORING_GLOSSES, FALSE); //removed in v. 3

	// The following code for setting the composebar font and RTL was moved here 
	// to the App from the CMainFraim constructor where it was in the MFC version
	wxPanel* pComposeBar;
	pComposeBar = m_pMainFrame->m_pComposeBar;
	wxASSERT(pComposeBar != NULL);
	wxTextCtrl* pComposeBarEdit = (wxTextCtrl*)pComposeBar->FindWindowById(IDC_EDIT_COMPOSE);
	wxASSERT(pComposeBarEdit != NULL);
	pComposeBarEdit->SetFont(*m_pTargetFont); 

	// Add RTL support for Unicode version
#ifdef _RTL_FLAGS
	//HWND hWnd = pEdit->GetSafeHwnd(); // MFC
	//LONG lAlign = (LONG)::GetWindowLong(hWnd,GWL_EXSTYLE); // MFC
	// Note: GetHandle() returns the platform-specific handle of the physical
	// window. It needs to be cast to an appropriate handle, such as HWND
	// for Windows, Widget for Motif, or GtkWidget for GTK. Therefore, the
	// next line below will need platform specific conditional compiles.
	// TODO: implement platform specific conditional compiles on next line.
	//HWND hWnd = (HWND)pEdit->GetHandle(); 
	//LONG lAlign = (LONG)::GetWindowLong(hWnd,GWL_EXSTYLE);
	if (gpApp->m_bTgtRTL)
	{
		pComposeBarEdit->SetLayoutDirection(wxLayout_RightToLeft);
		//lAlign ^= WS_EX_RIGHT;
		//lAlign ^= WS_EX_RTLREADING;
	}
	else
	{
		pComposeBarEdit->SetLayoutDirection(wxLayout_LeftToRight);
		//lAlign &= ~(WS_EX_RIGHT | WS_EX_RTLREADING);
	}
	//::SetWindowLong(hWnd,GWL_EXSTYLE,lAlign);
#endif


	// Hide the Glossing check box
	wxPanel* pBar;
	pBar = m_pMainFrame->m_pControlBar;
	wxASSERT(pBar != NULL);
	wxCheckBox* pCheckboxIsGlossing = (wxCheckBox*)pBar->FindWindowById(IDC_CHECK_ISGLOSSING);
	wxASSERT(pCheckboxIsGlossing != NULL);
	pCheckboxIsGlossing->Show(FALSE);

	m_pMainFrame->Centre(wxBOTH);
    m_pMainFrame->Show(TRUE);

    SetTopWindow(m_pMainFrame);

	// MFC has this code for drag and drop at this point
	// Enable drag/drop open
	// wxWidgets has various drag and drop classes, but I'm not going to try to implement it here
	//m_pMainWnd->DragAcceptFiles();

	// MFC Note: the following flag is TRUE while there is no m_targetBox created, during setup of the 
	// view - because if it turns out that the first sourcephrase does not have a KB entry, but 
	// the first two source phrases are in fact a phrase (ie. have a KB entry for that phrase) 
	// then the setup will do the required merge, and this will not have come about by the user 
	// hitting ENTER to jump to the next location; so certain things pertinent to that state will
	// not obtain (in particular m_targetBox will be NULL), and so we have
	// to test for the null box while the gbDoingInitialSetup flag is TRUE, and use a successful
	// test to enable us to jump code which would otherwise crash the app; the gbDoingInitialSetup
	// flag is then turned off at the end of the Finish button's code in the Wizard, and the end 
	// of the handler for the NEW... document command. We also must set the flag to true whenever
	// the user closes a document, because if he then does a <New Document> command in the wizard,
	// the phrase box will also at that point not be in existence, and otherwise a crash would
	// happen when in OnButtonMerge()
	gbDoingInitialSetup = TRUE;

	// The wxWidgets version has the "Export Target Text As UTF-8..." Menu Item on 
	// the File Menu and the Layout Menu as a top level menu in the AIMenuBarFunc() menu 
	// resource. With wxDesigner handling resources, it's easier to start with the item 
	// in the menu and programmatically delete it, rather than create it from scratch.
	// So, for ANSI version, we'll just remove them from the MenuBar.
#ifndef _UNICODE
	// ANSI only
	//AddForceUTF8Command(); // File... Export Target Text As UTF-8... menu command
	// Rather than calling a AddForceUTF8Command() function we'll just use the
	// pFileMenu pointer defined above and remove the menu item, deleting it to 
	// avoid memory leaks.
	// whm Bruce later made the following change to the MFC version:
	// BEW removed 8Dec06 because we'll force UTF8 conversion for the standard command from now on
	// leave this function in the app, commented out, it is a useful template for how to add
	// a menu command dynamically
	wxASSERT(pFileMenu != NULL);
	wxMenuItem* pRemMenuItem;
	//pRemMenuItem = pFileMenu->Remove(ID_FORCEUTF8);
	//wxASSERT(pRemMenuItem != NULL);
	//delete pRemMenuItem; // to avoid memory leaks
	//pRemMenuItem = (wxMenuItem*)NULL;

	// In the wx version we started with the Layout menu loaded with 
	// other menu resources. Here we'll remove it for the ANSI version.
	wxMenu* pLayoutMenu = m_pMainFrame->GetMenuBar()->GetMenu(5);
	wxASSERT(pLayoutMenu != NULL);
	// first delete the "Layout Window Right To Left\tCTRL+1" menu item
	pRemMenuItem = pLayoutMenu->Remove(ID_ALIGNMENT);
	wxASSERT(pRemMenuItem != NULL);
	delete pRemMenuItem; // to avoid memory leaks
	pRemMenuItem = (wxMenuItem*)NULL;
	// then delete the top level "Layout" menu
	wxMenuBar* pMenuBar = m_pMainFrame->GetMenuBar();
	wxMenu* pRemMenu;
	pRemMenu = pMenuBar->Remove(5);
	wxASSERT(pRemMenu != NULL);
	delete pRemMenu; // to avoid memory leaks
	pRemMenu = (wxMenu*)NULL;
#else
	// Unicode version
	// Initialize the Layout menu to LTR
	wxMenuBar* pMenuBar = m_pMainFrame->GetMenuBar();
	wxASSERT(pMenuBar != NULL);
	wxMenuItem * pLayoutMenuAlignment = pMenuBar->FindItem(ID_ALIGNMENT);
	wxASSERT(pLayoutMenuAlignment != NULL);
	// Set the menu item text to default value "Layout Window Right To Left\tCTRL+1"
	// note: default display is LTR so menu item should read what clicking it should make the layout become
	// after clicking. The menu text may be changed to appropriate value upon reading reading a project
	// config file (the change is made in ???).
	pLayoutMenuAlignment->SetText(_("Layout Window Right To Left\tCtrl-1"));
#endif 

	// insure that the Use Tooltips menu item in the Help menu is checked or unchecked according to the
	// current value of m_bUseToolTips (retrieved from registry/hidden settings via wxConfig object earlier in OnInit)
	wxASSERT(pMenuBar != NULL);
	wxMenuItem * pUseToolTips = pMenuBar->FindItem(ID_HELP_USE_TOOLTIPS);
	wxASSERT(pUseToolTips != NULL);
	pUseToolTips->Check(m_bUseToolTips);

	// change the unnamedN title to "Untitled - Adapt It"
	wxString viewTitle = _("Untitled - Adapt It");
	GetDocument()->SetTitle(viewTitle);
	GetDocument()->SetFilename(viewTitle,TRUE); // here TRUE means "notify the views" whereas


	//wxStandardPaths stdPaths;
	//wxString globalDataPath;
	//globalDataPath = stdPaths.GetDataDir();
	//wxLogDebug(_T("GetDataDir() path for AI_USFM.xml and books.xml is: %s"),globalDataPath.c_str());
	//wxString msg = msg.Format(_T("GetDataDir() path for AI_USFM.xml and books.xml is: %s"),globalDataPath.c_str());
	//wxMessageBox(msg,_T(""),wxICON_INFORMATION);

	// Display message in status bar that we are loading the books.xml and AI_USFM.xml files (brief)
	wxString message = _("Loading books.xml and AI_USFM.xml ...");
	wxStatusBar* pStatusBar = m_pMainFrame->GetStatusBar();//CStatusBar* pStatusBar;
	wxASSERT(pStatusBar != NULL);
	pStatusBar->SetStatusText(message,0); // use first field 0

	// Bible book folder support (accessing books.xml file, which can have its Christian content
	// removed if users want to fiddle with it) - if the file is absent from the work folder, then
	// default English bible book names plus "Other Texts" are set up; and the default radio button
	// labels would be "History books", .... etc.
	wxString booksfilePath = m_workFolderPath + PathSeparator + _("books.xml");

	bool bBooksFileExists = ::wxFileExists(booksfilePath) && !::wxDirExists(booksfilePath);

	// if the file does not exist in the work folder, look for it in the m_xmlInstallPath folder and if there then copy
	// it to the work folder before opening it from the latter location
	if (!bBooksFileExists)
	{

		wxString filecopyPath;
		filecopyPath = m_xmlInstallPath + PathSeparator + _("books.xml");
		bBooksFileExists = ::wxFileExists(filecopyPath) && !::wxDirExists(filecopyPath);
		if (bBooksFileExists)
		{
			wxLogDebug(_T("Copying existing books.xml from m_xmlInstallPath to user's work folder."));
			// copy the file to the work folder
			bool success;
			// Use the simple wxCopyFile() method
			success = wxCopyFile(filecopyPath, booksfilePath, TRUE); // TRUE = overwrite
			if (!success)
			{
				bBooksFileExists = FALSE;
				wxLogDebug(_T("Could NOT copy existing books.xml from m_xmlInstallPath to user's work folder."));
			}
		}
	}

	if (bBooksFileExists)
	{
		// parse the file, and set up m_pBibleBooks CPtrArray containing ptrs to BookNamePair structs;
		// and the Which Book? dialog's 5 button labels for the book sublists
		// (make sure xml file has a defaultBook=" some number " attribute, for setting m_nBookIndex 
		// to that number less one (the XML user-readable should use 1-based numbers))
		
		// whm added 30Nov07 If a customized books.xml file is used with fewer than 5 divisions 
		// ReadBooks_XML will only populate as many divisions as are actually listed within the
		// books.xml file. We don't want the remaining default labels to be used in this case. 
		// Therefore we should set all five division labels to null strings before
		// reading in the books.xml file (which we know exists since we're now in this block).
		// we also need to allow the books.xml to specify the division sizes for the same reasons
		// as for the division labels above.
		int ct;
		for (ct = 0; ct < 5; ct++)
		{
			m_strDivLabel[ct] = _T("");
			m_nDivSize[ct] = 0;
		}

		bool bReadOK = ReadBooks_XML(booksfilePath);
		if (!bReadOK)
		{
			// a bad parse, or failure to read the file in off the disk correctly, means we must prevent
			// book mode from being entered, and revert to normal mode (accessing Adaptations folder only)
			// IDS_DISABLE_BOOK_MODE
			wxMessageBox(_("Warning: book folder mode will be disabled until a books.xml file is stored \nin the work folder and it is read in and parsed correctly.\nYou should exit the application and fix the books.xml file\nbefore trying to do more work."), _T(""), wxICON_WARNING);
			m_bDisableBookMode = TRUE; // it will stay disabled until we exit the app & fix the books.xml file
			// disabling the mode does not change the m_bBookMode value, nor the m_nBookIndex value, and
			// these continue to be saved in the document; however, m_bDisableBookMode will force the paths
			// to be set to the Adaptations folder in the project, and so the document saved formerly under book
			// mode will not be accessible (unless the user moves it to the Adaptations folder using Windows
			// Explorer) until the disabling is removed, and if the document were to be saved it would get saved
			// in the Adaptations folder. To remove disabling, fix the books.xml file so it parses correctly,
			// or if it did not get read in, so that it gets read in. However, it is unlikely that it did not
			// get read in; most likely there was a parse error - and the user should have seen an error message to
			// that effect originating from the ReadBooks_XML call above. If there was not such, then it must have
			// been a bad file read.
		}
	}
	else
	{
		// no "books.xml" file in the work folder, so use default setup; m_pBibleBooks pointer is already created
		// but as yet has no content
		SetupDefaultBooksArray(m_pBibleBooks); // hard coded for 67 folders
		
		wxLogDebug(_T("No books.xml file found in work folder, so setting defaults via SetupDefaultBooksArray()."));
		
		// assume not in book mode, project config file will override this, so no point to set a default here
		m_nBookIndex = -1; 
		m_pCurrBookNamePair = NULL;
		m_bBookMode = FALSE;
		m_nDefaultBookIndex = 39; // so we'd get "Matthew" if the Bible books are set up as here
	}

	// !!! whm added 19Jan05 AI_USFM.xml file processing and USFM Filtering below
	// If the AI_USFM.xml file is absent from the work folder, then
	// default styles for USFM are set up.
	// 
	// whm ammended 14Jun05. If the Output_Default_Style_Strings symbol is defined (in XML.h)
	// the program will look for the AI_USFM_full.xml file in the App Path instead of AI_USFM.xml
	// and produce a file called the AI_USFM_full.txt that contains the Unix-like default strings. 
	// With this modification, the AI_USFM_full.xml file does not need to be renamed to AI_USFM.xml; 
	// AI_USFM_full.xml only needs to be located in the work folder, i.e., the "Adapt It Work" 
	// folder for it to be used in producing the Unix-like default strings.
	#ifdef Output_Default_Style_Strings
	wxString AIstyleFileWorkFolderPath = m_workFolderPath + PathSeparator + _T("AI_USFM_full.xml");
	#else
	wxString AIstyleFileWorkFolderPath = m_workFolderPath + PathSeparator + _T("AI_USFM.xml");
	#endif

	// Does AI_USFM.xml exist in the work folder
	bool bWorkStyleFileExists = wxFileExists(AIstyleFileWorkFolderPath);

	// Note: The path to the m_setupFolder may differ from the default installation location
	// if user installed Adapt It to a non-default user defined folder, The call to 
	// FindAppPath() above determines the path to the currently running executable, 
	// which is safer than hard coding the path to a predetermined setup location.
	wxString filecopyPath = m_xmlInstallPath + PathSeparator + _T("AI_USFM.xml");

	bool bSetupStyleFileExists = wxFileExists(filecopyPath);

	// If the file does not exist in the work folder, look for it in the setup folder 
	// and, if there, then copy it to the work folder before opening it from the latter 
	// location.
	if (!bWorkStyleFileExists)
	{
		// There is no AI_USFM.xml file in the work folder, so get one from
		// the setup folder if one is there, and copy it to the work folder preserving
		// the date it had in the setup folder.
		if (bSetupStyleFileExists)
		{
			// copy the file to the work folder
			bool copyOK;
			copyOK = wxCopyFile(filecopyPath, AIstyleFileWorkFolderPath, TRUE); // TRUE = overwrite
			if (copyOK)
			{
				// Testing shows that the wxCopyFile call above actually preserves the mod dates, 
				// so we don't need code below for the wxWidgets version since a successful copy will 
				// automatically make the newly copied file in the work folder have same date it has 
				// in the installation/setup folder.
				//wxFileName fnInWorkFolderPath(AIstyleFileWorkFolderPath);
				//wxFileName fnInSetupFolderPath(filecopyPath);
				//wxDateTime dtWorkFolderFileAccessTime,dtWorkFolderFileModTime,dtWorkFolderFileCreateTime;
				//wxDateTime dtSetupFolderFileAccessTime,dtSetupFolderFileModTime,dtSetupFolderFileCreateTime;
				//fnInWorkFolderPath.GetTimes(&dtWorkFolderFileAccessTime,&dtWorkFolderFileModTime,&dtWorkFolderFileCreateTime);
				//fnInSetupFolderPath.GetTimes(&dtSetupFolderFileAccessTime,&dtSetupFolderFileModTime,&dtSetupFolderFileCreateTime);
				//// we only need to compare the Modtimes
				//if (dtWorkFolderFileModTime != dtSetupFolderFileModTime)
				//{
				//	// file in work folder is not same as the one in setup folder so make Modtime of file
				//	// in work folder match the Modtime of file in setup folder
				//	fnInWorkFolderPath.SetTimes(&dtSetupFolderFileAccessTime,&dtSetupFolderFileModTime,&dtSetupFolderFileCreateTime);
				//}
				bWorkStyleFileExists = TRUE;
				wxLogDebug(_T("Copying existing AI_USFM.xml from m_xmlInstallPath to user's work folder."));
			}
			else
			{
				bWorkStyleFileExists = FALSE;
				wxLogDebug(_T("Could NOT copy existing AI_USFM.xml from m_xmlInstallPath to user's work folder."));
			}
		}
	}
	else
	{
		bool bSetupFolderHasNewerVersion = FALSE;
		if (bSetupStyleFileExists)
		{
			// The AI_USFM.xml file exists, but there may have been a more recent installation 
			// just done in which the file in the setup folder exists as a later version with 
			// changed content, so we have to check for a later version and if so we copy it 
			// to replace the old version in the work folder - if we don't do this, tweaks done 
			// to the file in incremental version updates won't get implemented unless the user 
			// manually moves the file to the work folder and we don't want to expect users to 
			// have to remember to do such a thing.
			wxFileName fnInWorkFolderPath(AIstyleFileWorkFolderPath);
			wxFileName fnInSetupFolderPath(filecopyPath);
			wxDateTime dtWorkFolderFileAccessTime,dtWorkFolderFileModTime,dtWorkFolderFileCreateTime;
			wxDateTime dtSetupFolderFileAccessTime,dtSetupFolderFileModTime,dtSetupFolderFileCreateTime;
			fnInWorkFolderPath.GetTimes(&dtWorkFolderFileAccessTime,&dtWorkFolderFileModTime,&dtWorkFolderFileCreateTime);
			fnInSetupFolderPath.GetTimes(&dtSetupFolderFileAccessTime,&dtSetupFolderFileModTime,&dtSetupFolderFileCreateTime);
			// Check if we have a newer version in the setup folder
			//BOOL bSetupFolderHasNewerVersion = statusInSetupFolder.m_mtime > statusInWorkFolder.m_mtime;
			bSetupFolderHasNewerVersion = dtSetupFolderFileModTime > dtWorkFolderFileModTime;
		}
		// If setup folder has a newer version, we will first delete what is in the work 
		// folder, then copy the file from the setup folder to replace it. If setup folder 
		// version is not newer then do nothing.
		if (bSetupFolderHasNewerVersion)
		{
			if (!wxRemoveFile(AIstyleFileWorkFolderPath))
			{
				wxMessageBox(_("Could not remove the AI_USFM.xml file from the work folder."),
					_T(""), wxICON_INFORMATION);
			}
			if (bSetupStyleFileExists)
			{
				// copy the file to the work folder
				bool copyOK;
				copyOK = wxCopyFile(filecopyPath, AIstyleFileWorkFolderPath, TRUE); // TRUE = overwrite
				if (copyOK)
				{
					// Testing shows that the wxCopyFile call above actually preserves the mod dates, 
					// so we don't need code below for the wxWidgets version since a successful copy will 
					// automatically make the newly copied file in the work folder have same date it has 
					// in the installation/setup folder.
					//wxFileName fnInWorkFolderPath(AIstyleFileWorkFolderPath);
					//wxFileName fnInSetupFolderPath(filecopyPath);
					//wxDateTime dtWorkFolderFileAccessTime,dtWorkFolderFileModTime,dtWorkFolderFileCreateTime;
					//wxDateTime dtSetupFolderFileAccessTime,dtSetupFolderFileModTime,dtSetupFolderFileCreateTime;
					//fnInWorkFolderPath.GetTimes(&dtWorkFolderFileAccessTime,&dtWorkFolderFileModTime,&dtWorkFolderFileCreateTime);
					//fnInSetupFolderPath.GetTimes(&dtSetupFolderFileAccessTime,&dtSetupFolderFileModTime,&dtSetupFolderFileCreateTime);
					//// we only need to compare the Modtimes
					//if (dtWorkFolderFileModTime != dtSetupFolderFileModTime)
					//{
					//	// file in work folder is not same as the one in setup folder so make Modtime of file
					//	// in work folder match the Modtime of file in setup folder
					//	fnInWorkFolderPath.SetTimes(&dtSetupFolderFileAccessTime,&dtSetupFolderFileModTime,&dtSetupFolderFileCreateTime);
					//}
					bWorkStyleFileExists = TRUE;
					wxLogDebug(_T("Copying newer version of AI_USFM.xml to user's work folder."));
				}
				else
				{
					bWorkStyleFileExists = FALSE;
					wxLogDebug(_T("Could not copy newer version of AI_USFM.xml to user's work folder."));
				}
			}
		}
	}

	if (bWorkStyleFileExists)
	{
		// parse the file, and set up the three CMapStringToOb maps containing 
		// mappings between usfm tags and the usfm analysis structs on the heap;
		bool bReadOK = ReadSFM_XML(AIstyleFileWorkFolderPath);
		if (!bReadOK)
		{
			// a bad parse, or failure to read the file in off the disk correctly, means we will
			// inform the user of the situation and use a default set of styles based on our
			// original AI_USFM.xml file
			// IDS_USING_DEFAULT_USFM_STYLES
			wxMessageBox(_("Warning: using default USFM styles until a AI_USFM.xml file is stored in the work folder and it is read in and parsed correctly. You should exit the application and fix the AI_USFM.xml file if you have previously told Adapt It to hide certain standard format markers."), _T(""), wxICON_INFORMATION);
			m_bUsingDefaultUsfmStyles = TRUE; 
			// To use any non-default styles, user needs to fix the AI_USFM.xml file so it parses correctly,
			// or if it did not get read in, so that it gets read in. However, it is unlikely that it did not
			// get read in; most likely there was a parse error - and the user should have seen an error message to
			// that effect originating from the ReadStyles_XML call above. If there was not such, then it must have
			// been a bad file read.
		}
	}
	else
	{
		// no "AI_USFM.xml" file in the work folder, so use default setup; 
		// m_pUsfmStylesMap, m_pPngStylesMap, and m_pUsfmAndPngStylesMap 
		// pointers are already created but as yet have no content
		wxLogDebug(_T("AI_USFM.xml not found in work folder - using internal styles via SetupDefaultStylesMap."));
		SetupDefaultStylesMap(); // hard coded for 267 default usfm styles
	}

    // Initialise the help system. We do it here because our m_setupFolder was determined above
    // and we now know the path to the setup folder where any help file is installed.
    //  
    // Determine the path to the installation folder where Adapt_It.xxx is located
	// Note: Our function FindAppPath() determined the most likely path where the Adapt It executable
	// program is located and stored it in m_setupFolder. 
	
	wxString appName;
	appName = GetAppName();
	
	wxString helpFilePath,helpFileName;
	// Note: earlier in OnInit() m_helpInstallPath was determined in GetDefaultPathForHelpFiles() which
	// also determined the name stored in m_htbHelpFileName.
	helpFilePath = m_helpInstallPath;
	// The file name stored in helpFileName below will be Adapt_It.htb for Windows; adaptit.htb for Linux and AdaptIt.htb for Mac. 
	// The extension may be .zip, .htb or .hhp when using wxHtmlHelpController
	helpFileName = m_htbHelpFileName;  
//#ifdef __WXMAC__
//	helpFileName = appName + _T(".htb"); // may be .zip, .htb or .hhp if we switch to wxHtmlHelpController
//#endif

#ifdef __WXGTK__
	// The call to m_pHelpController->Initialize() below will fail on wxGTK unless wxUSE-LIBMSPACK is 1
#ifndef wxUSE_LIBMSPACK
	wxLogDebug(_T("wxUSE_LIBMSPACK is NOT DEFINED! The MS Windows Adapt_It_Help.chm help file will not display properly."));
#endif
	//helpFileName = appName + _T(".htb"); // may be .zip, .htb or .hhp if we switch to wxHtmlHelpController
#endif

//#ifdef __WXMSW__
//	helpFileName = appName + _T(".htb"); // TODO: use the .htb form for Windows too! // may be .zip, .htb or .hhp if we switch to wxHtmlHelpController
//#endif
	helpFilePath = helpFilePath + PathSeparator + helpFileName;
	wxLogDebug(_T("helpFilePath = %s"),helpFilePath.c_str());
    if ( !m_pHelpController->Initialize(helpFilePath) )
    {
        wxLogDebug(wxT("Cannot initialize the help system. Online help will not be available."));
    }
	
	// Get the text extent of the longest sfm represented in the input data. We
	// only need to scan the UsfmAndPng map since it has all the markers in it.
	// For a pDC we've already created our canvas within the Main Frame above, so
	// we can use it, setting the font to the App's m_pNavTextFont 		
	wxClientDC aDC(m_pMainFrame->canvas);
	wxFont tempFont = *m_pNavTextFont;
	tempFont.SetPointSize(12);
	aDC.SetFont(tempFont);
	sizeLongestSfm = GetExtentOfLongestSfm(&aDC); // sizeLongestSFM does not include any unknown markers
	aDC.GetTextExtent(_T(' '),&sizeSpace.x,&sizeSpace.y);

	// Set up the rapid access data strings for wrap markers, sectionHead markers, 
	// inLine markers, and filter markers.
	SetupMarkerStrings();

	// gCurrentFilterMarkers is determined in SetupMarkerStrings above. We'll start with
	// gProjectFilterMarkersForConfig assigned gCurrentFilterMarkers's list of filter
	// markers; gProjectFilterMarkersForConfig may be changed once the config file is
	// read in
	gProjectFilterMarkersForConfig = gCurrentFilterMarkers; 

	// Retain the initial USFM filter marker string as "Factory default"
	gFactoryFilterMarkersStr = UsfmFilterMarkersStr;
	gFactorySfmSet = UsfmOnly;

	// set up data structures to be used for free translation support (ExitInstance will delete them)
	gpCurFreeTransSectionPileArray = new wxArrayPtrVoid;
	gpFreeTransArray = new wxArrayPtrVoid;

	// we are passed all the MFC initialization stuff, and about to enter the Start Working... wizard,
	// so indicate it is safe for OnNewDocument() to be able to write out the project config file once the
	// user's setting for book mode is in place
	gbPassedMFCinitialization = TRUE;

	// **** test code fragments here ****

/*
	// This test compared with the same code under MFC shows that
	// simply moving the x (or MFC left coordinate) has different
	// effects: In MFC the widthrecafter increases by 3, whereas in
	// wxWidgets the widthrecafter stays the same, indicating that
	// when the MFC version moves the upper left point of a rect,
	// we must also adjust the rect width/height in wxWidgets.
	wxRect testrec = wxRect(5,5,10,10);
	int widthrecbefore = testrec.GetWidth();
	testrec.x -= 3;
	int widthrecafter = testrec.GetWidth();
	int endoftest = 1;

	wxString test,test2,test3,test4;
	//test = _T("This is\r\n a test string.\r\n");
	//test2 = _T("This is another\n test string.\n");
	test = _T("This is a test string.");
	test2 = _T("This is another test string.");
	//test += test2;
	test3 = _T("Finished. ");
	//test3 = test3 + test + test2;
	//test4 = test3 + test + test2;
	//test4.Shrink();
	//test4.append(test3);
	//test4.append(test);
	//test4.append(test2);
	test4 += test3;
	test4 += test;
	test4 += test2;
	int len = test4.Len();
	wxChar* pBuf = test4.GetWriteBuf(len + 1);
	wxChar* pEnd = pBuf + len;
	*pEnd = (wxChar)0;
	//wxASSERT(*pEnd == '\0');
	test3.UngetWriteBuf();
	//test = test + test2;
	//test += test2;
	//wxASSERT(test == *pBuf);
	int junk = 1;
*/
	/*
	// test InsertInString()
	wxString str0 = _T(""); // empty string
	wxString str1 = _T("1");
	wxString str2 = _T("12");
	wxString str3 = _T("1234567890");
	int psn0 = 0; // at beginning of string
	int psn1 = 10; // outside string bounds
	int psn2 = 9; // at end of string
	int psn3 = 5; // in middle of string
	wxString resultStr;
	resultStr = InsertInString(str0,psn0,str0);
	resultStr = InsertInString(str1,psn0,str0);
	resultStr = InsertInString(str2,psn0,str0);
	resultStr = InsertInString(str3,psn0,str0);
	resultStr = InsertInString(str0,psn1,str0);
	resultStr = InsertInString(str1,psn1,str0);
	resultStr = InsertInString(str2,psn1,str0);
	resultStr = InsertInString(str3,psn1,str0);
	resultStr = InsertInString(str0,psn2,str0);
	resultStr = InsertInString(str1,psn2,str0);
	resultStr = InsertInString(str2,psn2,str0);
	resultStr = InsertInString(str3,psn2,str0);
	resultStr = InsertInString(str0,psn3,str0);
	resultStr = InsertInString(str1,psn3,str0);
	resultStr = InsertInString(str2,psn3,str0);
	resultStr = InsertInString(str3,psn3,str0);

	resultStr = InsertInString(str0,psn0,str1);
	resultStr = InsertInString(str1,psn0,str1);
	resultStr = InsertInString(str2,psn0,str1);
	resultStr = InsertInString(str3,psn0,str1);
	resultStr = InsertInString(str0,psn1,str1);
	resultStr = InsertInString(str1,psn1,str1);
	resultStr = InsertInString(str2,psn1,str1);
	resultStr = InsertInString(str3,psn1,str1);
	resultStr = InsertInString(str0,psn2,str1);
	resultStr = InsertInString(str1,psn2,str1);
	resultStr = InsertInString(str2,psn2,str1);
	resultStr = InsertInString(str3,psn2,str1);
	resultStr = InsertInString(str0,psn3,str1);
	resultStr = InsertInString(str1,psn3,str1);
	resultStr = InsertInString(str2,psn3,str1);
	resultStr = InsertInString(str3,psn3,str1);

	resultStr = InsertInString(str0,psn0,str2);
	resultStr = InsertInString(str1,psn0,str2);
	resultStr = InsertInString(str2,psn0,str2);
	resultStr = InsertInString(str3,psn0,str2);
	resultStr = InsertInString(str0,psn1,str2);
	resultStr = InsertInString(str1,psn1,str2);
	resultStr = InsertInString(str2,psn1,str2);
	resultStr = InsertInString(str3,psn1,str2);
	resultStr = InsertInString(str0,psn2,str2);
	resultStr = InsertInString(str1,psn2,str2);
	resultStr = InsertInString(str2,psn2,str2);
	resultStr = InsertInString(str3,psn2,str2);
	resultStr = InsertInString(str0,psn3,str2);
	resultStr = InsertInString(str1,psn3,str2);
	resultStr = InsertInString(str2,psn3,str2);
	resultStr = InsertInString(str3,psn3,str2);

	resultStr = InsertInString(str0,psn0,str3);
	resultStr = InsertInString(str1,psn0,str3);
	resultStr = InsertInString(str2,psn0,str3);
	resultStr = InsertInString(str3,psn0,str3);
	resultStr = InsertInString(str0,psn1,str3);
	resultStr = InsertInString(str1,psn1,str3);
	resultStr = InsertInString(str2,psn1,str3);
	resultStr = InsertInString(str3,psn1,str3);
	resultStr = InsertInString(str0,psn2,str3);
	resultStr = InsertInString(str1,psn2,str3);
	resultStr = InsertInString(str2,psn2,str3);
	resultStr = InsertInString(str3,psn2,str3);
	resultStr = InsertInString(str0,psn3,str3);
	resultStr = InsertInString(str1,psn3,str3);
	resultStr = InsertInString(str2,psn3,str3);
	resultStr = InsertInString(str3,psn3,str3);
*/
	/*
	// Testing wxCheckListBox class
	wxPanel *m_panel;
    m_panel = new wxPanel(m_pMainFrame, wxID_ANY, wxPoint(0, 0),
                          wxSize(400, 200), wxTAB_TRAVERSAL);
	wxCheckListBox *m_pListBox;
	long flags = 0;
	// check list box
    static const wxChar *aszChoices[] =
    {
        _T("Zeroth"),
        _T("First"), _T("Second"), _T("Third"),
        _T("Fourth"), _T("Fifth"), _T("Sixth"),
        _T("Seventh"), _T("Eighth"), _T("Nineth")
    };

    wxString *astrChoices = new wxString[WXSIZEOF(aszChoices)];
    unsigned int ui;
    for ( ui = 0; ui < WXSIZEOF(aszChoices); ui++ )
        astrChoices[ui] = aszChoices[ui];

    m_pListBox = new wxCheckListBox
        (
         m_panel,               // parent
         -1,       // control id
         wxPoint(10, 10),       // listbox poistion
         wxSize(400, 100),      // listbox size
         WXSIZEOF(aszChoices),  // number of strings
         astrChoices,           // array of strings
         flags
        );

    //m_pListBox->SetBackgroundColour(*wxGREEN);

    delete [] astrChoices;

    // set grey background for every second entry
    for ( ui = 0; ui < WXSIZEOF(aszChoices); ui += 2 ) 
	{
        //AdjustColour(ui);
     // even items have grey backround, odd ones - white
      unsigned char c = ui % 2 ? 255 : 200;
      m_pListBox->GetItem(ui)->SetBackgroundColour(wxColor(c, c, c));
   }

    m_pListBox->Check(2);
    m_pListBox->Select(3);
	*/

/*
	CString bookIDStr;
	CString docPath = 
	_T("C:\\Documents and Settings\\watersb\\My Documents\\Adapt It Unicode Work\\Takia to English adaptations\\Adaptations\\1Timothy.xml");
	bool isOK = FileContainsBookIndicator(docPath,bookIDStr);
	int yes = 1;
*/
	/*
	CString str = "something \\fr ";
	int len = str.GetLength();
	TCHAR* pBuff = str.GetBuffer(len+1);
	TCHAR* ptr = pBuff + len;
	int mkrlen;
	BOOL bWorked = GetView()->GetPrevMarker(pBuff,ptr,mkrlen);
	str.ReleaseBuffer();
	*/
/*
	CString sss = _T("\r\n ab\r\n ");
	CString xStr;
	xStr = Trim(sss,_T(" \r\n"));
*/
/*
#include "XML.h"
#include "BString.h"
CBString mybstr = "<<Tamol";
InsertEntities(mybstr);
ReplaceEntities(mybstr);
*/
/*
CSourcePhrase src;
src.m_bFootnote = TRUE;
src.m_bHasKBEntry = TRUE;
src.m_bHasFreeTrans = TRUE;
src.m_bStartFreeTrans = TRUE;
src.m_bEndFreeTrans = TRUE;
CBString bstr = MakeFlags(&src);
CString ss = (char*)bstr;
TRACE1("\n 22 digits = %s\n",ss);
bstr.Empty();
char numStr[24];
itoa(1524471,numStr,10);
bstr += numStr;
ss = (char*)bstr;
TRACE1("\n fewer digits (base 10) = %s\n",ss);
*/

/*
CString str( "%First Second#Third" );
CString resToken;
int curPos= 0;

resToken= str.Tokenize("% #",curPos);
while (resToken != "")
{
   //printf("Resulting token: %s\n", resToken);
   resToken= str.Tokenize("% #",curPos);
};
*/

/*	CString start;
	start.Format(IDS_MANUAL_FIXES_REFS);
	CString fixesStr = "23:1   ";
	fixesStr = start + fixesStr;
	for (int i = 2; i < 600; i++)
	{
		CString next = "23:";
		char pStr[10];
		_itoa(i,pStr,10);
		next += pStr;
		next += "   ";
		fixesStr += next;
	}
	int len = fixesStr.GetLength();
	CString path = gpApp->m_curProjectPath;
	path += _T('\\');
	path += _T("Rebuild Log.txt");
	CFile fout;
	CFileException error;
	BOOL bOK = fout.Open( path, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive,&error);
	fout.Write((LPCTSTR)fixesStr,len);
	fout.Close();
	if (len > 1200)
	{
		// trim the excess and add the string telling user to check visually
		fixesStr = fixesStr.Left(1200);
		fixesStr.MakeReverse();
		int nFound = fixesStr.Find(_T(' '));
		if (nFound != -1)
		{
			fixesStr = fixesStr.Mid(nFound);
		}
		fixesStr.MakeReverse();
		CString appendStr;
		appendStr.Format(IDS_APPEND_MSG);
		fixesStr += appendStr;
	}
	AfxMessageBox(fixesStr,MB_ICONINFORMATION);
	ASSERT(FALSE);
*/

#if wxMAC_USE_CORE_GRAPHICS
	wxLogDebug(_T("In OnInit() wxMAC_USE_CORE_GRAPHICS is defined!"));
	if (m_pMainFrame->canvas->IsDoubleBuffered())
		wxLogDebug(_T("This wxScrolledWindow is double-buffered by the system."));
#endif


	m_bJustLaunched = TRUE;// set false in OnIdle call
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return   nothing  
/// \remarks
/// Called from: the App's OnExit(). The function is a misnomer, beacuse all the function does
/// is call WriteConfigurationFile() to save the basic configuration file.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::Terminate()
{
	bool bOK;
	bOK = WriteConfigurationFile(szBasicConfiguration, m_workFolderPath,1 /* 1 = app level */);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     always zero (0)
/// \remarks
/// Called from: the wxApp object just before it exits and returns control back to the system.
/// All cleanup of allocated memory needs to be done here if not already done elsewhere in the
/// application. OnExit() is called after destroying all application windows and controls, but 
/// before wxWidgets cleanup. Note that it is not called at all if OnInit() failed.
////////////////////////////////////////////////////////////////////////////////////////////
int CAdapt_ItApp::OnExit(void)
{
	// OnExit() "is called when the application exits, but before wxWidgets
	// cleans up its internal structures. All wxWidgets' objects that the
	// program creates should be deleted by the time OnExit() finishes. In
	// particular, do NOT destroy them from the application class destructor!"
	m_pDocManager->FileHistorySave(* m_pConfig);

	// Code below transferred here from MFC View::PostNcDestroy() which is
	// called just before MFC's ExitInstance(). 
	// Transfer to the application all the view-related constants which are to be
	// saved in the basic config file (the basic config file is saved in the call 
	// to Terminate() below
	//maxToDisplay = m_nMaxToDisplay;
	//minPrecContext = m_nPrecedingContext;
	//minFollContext = m_nFollowingContext;
	//leading = m_curLeading;
	//gapWidth = m_curGapWidth;
	//leftMargin = m_curLMargin;
	//nBoxExpansionMultiplier = (int)gnExpandBox;
	//nTooNearEndMultiplier = (int)gnNearEndFactor; // can't be changed in the application,
									// but no harm done by resaving same value in the config file
	//if (m_bSuppressFirst)
	//	suppressFirst = 1;
	//else
	//	suppressFirst = 0;
	//if (m_bSuppressLast)
	//	suppressLast = 1;
	//else
	//	suppressLast = 0;

	// WriteConfigurationFile now uses m_specialTextColor, m_reTranslnTextColor and m_navTextColor
	//specialTextColor = m_specialTextColor;
	//reTranslnTextColor = m_reTranslnTextColor;
	//navTextColor = m_navTextColor;
	//m_lastSourceFileFolder = m_saveLastFolderPath; // so app can store it in config files

	bool bOK; // we won't care whether it succeeds or not, since the later Get... can use defaults
	// WriteConfigurationFile selector 2 forces write of project config info
	bOK = WriteConfigurationFile(szProjectConfiguration,m_curProjectPath,2);

    // clean up the help system
	delete m_pHelpController;
	m_pHelpController = (wxHtmlHelpController*)NULL;
	//m_pHelpController = (wxHelpController*)NULL;
	delete wxHelpProvider::Set(NULL);


	// delete the CSourceBundle instance
	if (m_pBundle != 0)
	{
		// old data still exists, so get rid of it
		m_pBundle->DestroyStrips( 0 ); // destroy all strips (start from 0th)
		delete m_pBundle;
		m_pBundle = (CSourceBundle*)NULL;
	}
	// Code above transferred here from MFC View::PostNcDestroy() which is
	// called just before MFC's ExitInstance(), which in turn calls Terminate(). 

	// call Terminate to get the app level 1 basic configuration file updated
	Terminate();

	// WX Note: Our TargetBox is now a child of the view's canvas (which 
	// itself is derived from wxScrolledWindow. As a child of the canvas 
	// window, m_pTargetBox will be automatically destroyed when pView->canvas 
	// is destroyed during doc/view's normal cleanup. That is, when our View is
	// destroyed, all child windows (including our target box) are automatically
	// destroyed too. Therefore, the target box must not be deleted again in 
	// the App's OnExit() method, when the App terminates.
	//delete m_pTargetBox;
	//m_pTargetBox = (CPhraseBox*)NULL;

    delete m_pDocManager; // deleting this 
	m_pDocManager = (wxDocManager*)NULL;

	delete m_pConfig;
	m_pConfig = (wxConfig*)NULL;
	delete m_pConfigSIL;
	m_pConfigSIL = (wxConfig*)NULL;
	if (m_pChecker)
	{
		delete m_pChecker;
		m_pChecker = (wxSingleInstanceChecker*)NULL;
	}

	delete m_pParser;
	m_pParser = (wxCmdLineParser*)NULL;

	delete m_pSourceFont;
	m_pSourceFont = (wxFont*)NULL;
	delete m_pTargetFont;
	m_pTargetFont = (wxFont*)NULL;
	delete m_pNavTextFont;
	m_pNavTextFont = (wxFont*)NULL;
	delete m_pDlgSrcFont;
	m_pDlgSrcFont = (wxFont*)NULL;
	delete m_pDlgTgtFont;
	m_pDlgTgtFont = (wxFont*)NULL;
	delete m_pComposeFont;
	m_pComposeFont = (wxFont*)NULL;
	delete m_pDlgGlossFont;
	m_pDlgGlossFont = (wxFont*)NULL;
	delete m_pRemovalsFont;
	m_pRemovalsFont = (wxFont*)NULL;
	delete m_pVertEditFont;
	m_pVertEditFont = (wxFont*)NULL;
	
	delete m_pSrcFontData;
	m_pSrcFontData = (wxFontData*)NULL;
	delete m_pTgtFontData;
	m_pTgtFontData = (wxFontData*)NULL;
	delete m_pNavFontData;
	m_pNavFontData = (wxFontData*)NULL;

	delete pPgSetupDlgData;
	pPgSetupDlgData = (wxPageSetupDialogData*)NULL;
	delete pPrintData;
	pPrintData = (wxPrintData*)NULL;

	// delete the stuff used for free translation support
	gpCurFreeTransSectionPileArray->Clear();
	delete gpCurFreeTransSectionPileArray;
	gpFreeTransArray->Clear();
	delete gpFreeTransArray;
	
	if (gpDocList != NULL)
	{
		// delete the object, it's contents should already have been removed
		delete gpDocList;
		gpDocList = (SPList*)NULL;
	}

	// In wxWidgets we've moved all the doc's data members to the App
	// The following is from the MFC's CAdapt_ItDoc::~CAdapt_ItDoc() destructor:
	if (m_pSourcePhrases != 0)
	{
		delete m_pSourcePhrases;
		m_pSourcePhrases = (SPList*)NULL;
	}

	if (m_pBuffer != 0)
	{
		delete m_pBuffer;
		m_pBuffer = (wxString*)NULL;
	}

	// wxWidgets docs say not to do cleanup in the App class destructor
	// The following from the MFC's CAdapt_ItApp::~CAdapt_ItApp() destructor:
	m_nTotalBooks = m_pBibleBooks->GetCount(); //m_nTotalBooks = m_pBibleBooks->GetSize();
	if (m_nTotalBooks == 0L)
	{
		delete m_pBibleBooks;
	}
	else
	{
		for (int i = 0; i < m_nTotalBooks; i++)
		{
			BookNamePair* pPair =  (BookNamePair*)(*m_pBibleBooks)[i];
			if (pPair)
				delete pPair;
		}
		delete m_pBibleBooks;
	}

	// whm added 19Jan05 AI_USFM.xml file processing and USFM Filtering
	wxString key;
	USFMAnalysis* pSfm;
	// destroy all USFMAnalysis objects and the CPtrArray pointing to them
	if (m_pMappedObjectPointers->GetCount() > 0)
	{
		// destroy the USFMAnalysis objects on the heap
		for (int upos = 0; upos < (int)m_pMappedObjectPointers->GetCount(); upos++)
		{
			pSfm = (USFMAnalysis*)m_pMappedObjectPointers->Item(upos);
			if (pSfm)
				delete pSfm;
		}
		// destroy all keys from CPtrArray
		m_pMappedObjectPointers->Clear();
		// destroy the CPtrArray itself
		delete m_pMappedObjectPointers;
	}

	// USFMAnalysis objects are destroyed, now destroy the map keys and 
	// maps themselves
	if (m_pUsfmStylesMap->size() > 0) //if (m_pUsfmStylesMap->GetCount() > 0)
	{
		// destroy all Usfm key/object associations
		m_pUsfmStylesMap->clear(); //m_pUsfmStylesMap->Clear();
	}
	// destroy the Usfm map itself
	delete m_pUsfmStylesMap;
	
	if (m_pPngStylesMap->size() > 0)
	{
		// destroy all Png key/object associations
		m_pPngStylesMap->clear();
	}
	// destroy the Png map itself
	delete m_pPngStylesMap;
	
	if (m_pUsfmAndPngStylesMap->size() > 0)
	{
		// destroy all UsfmAndPng key/object associations
		m_pUsfmAndPngStylesMap->clear();
	}
	// destroy the UsfmAndPng map itself
	delete m_pUsfmAndPngStylesMap;
	
	// whm: before deleting our CConsistentChanger objects, we need to
	// free up the memory used by the structures within CCCModule.
	

	int ct;
	for (ct = 0; ct < 4; ct++)
	{
		if (m_pConsistentChanger[ct] != NULL && m_bCCTableLoaded[ct])
		{
			delete m_pConsistentChanger[ct];
			m_pConsistentChanger[ct] = (CConsistentChanger*)NULL;
		}
	}

	if (m_pLocale)
	{
		delete m_pLocale;
		m_pLocale = (wxLocale*)NULL;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: the App's OnInit() and from the App's SetDefaults() if the user holds the
/// shift key down during startup, or if there is no existing configuration files.
/// InitializeFonts() handles the initial creation of all the fonts used in the application,
/// including m_pSourceFont, m_pTargetFont, m_pNavTextFont, m_pDlgSrcFont, m_pDlgTgtFont,
/// m_pDlgGlossFont, m_pComposeFont, m_pRemovalsFont, and m_pVertEditFont. It then sets a 
/// suitable default encoding if the ANSI build is running. It then creates corresponding 
/// wxFontData objects on the heap for the created fonts, and sets default point sizes, 
/// weights, and colors for the created fonts.
/// It then updates the fontInfo structs which function as an MFC compatibility structure for
/// saving the font data in the configuration files.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::InitializeFonts()
{
	// In the wx version, the original Initialize() function is reworked into individual files
	// more specific to the kind of initializations being done (all called from OnInit()).
	// Individual functions that do initialization tasks are now called InitializeFonts(); 
	// InitializePunctuation(); and GetConfigurationFileData().
	/*
	whm notes concerning 8-bit font encodings (non-Unicode):
	The MFC LOGFONT.lfCharset is defined as a byte value which has the following predefined values for
	the character set (encoding?) under Windows:

	MFC Symbol:		Byte Value
	==========================
	ANSI_CHARSET		0
	DEFAULT_CHARSET		1
	MFC docs say about DEFAULT_CHARSET on Windows 95/98/Me: You can use the DEFAULT_CHARSET value to allow 
	the name and size of a font to fully describe the logical font. If the specified font name does not 
	exist, a font from any character set can be substituted for the specified font, so you should use 
	DEFAULT_CHARSET sparingly to avoid unexpected results. 
	MFC docs say about DEFAULT_CHARSET on Windows NT/2000/XP: DEFAULT_CHARSET is set to a value based on the 
	current system locale. For example, when the system locale is English (United States), it is set as 
	ANSI_CHARSET 0. 

	SYMBOL_CHARSET		2  [N/A IN WX]
	MAC_CHARSET			77 
	SHIFTJIS_CHARSET	128
	HANGUL_CHARSET		129
	JOHAB_CHARSET		130 Korean language edition of Windows [N/A IN WX]
	GB2312_CHARSET		134
	CHINESEBIG5_CHARSET	136
	GREEK_CHARSET		161
	TURKISH_CHARSET		162 
	VIETNAMESE_CHARSET	163 [N/A IN WX]
	HEBREW_CHARSET		177 Middle East language edition of Windows
	ARABIC_CHARSET		178 Middle East language edition of Windows
	BALTIC_CHARSET		186
	RUSSIAN_CHARSET		204
	THAI_CHARSET		222 Thai language edition of Windows
	EASTEUROPE_CHARSET	238
	OEM_CHARSET			255 The OEM_CHARSET value specifies a character set that is operating-system 
							dependent
	Fonts with other character sets may exist in the operating system. If an application uses a font 
	with an unknown character set, it should not attempt to translate or interpret strings that are
	rendered with that font. 
	The lfCharset parameter of LOGFONT is important in the font mapping process. To ensure consistent 
	results, specify a specific character set. If you specify a typeface name in the lfFaceName member, 
	make sure that the lfCharSet value matches the character set of the typeface specified in lfFaceName. 
	
	Here is how the above charsets compare/map to the wxWidgets font encoding values for 8-bit fonts:
	(Note: "Value" is the value of the enum wxFontEncoding struct; there are also values in that
	struct which apply to Unicode encodings including: wxFONTENCODING_UTF7 40 for UTF-7 Unicode encoding;
    wxFONTENCODING_UTF8 41 for UTF-8 Unicode encoding; wxFONTENCODING_UNICODE 43 for Unicode - currently 
	used only by the wxEncodingConverter class.
	The following is from the wxWidgets header file fontenc.h
	
								 WX   MFC
	wxWidgets Symbol:			 Val  Val
	======================================
	wxFONTENCODING_SYSTEM		-1    255 The default encoding of the underlying operating system (notice that 
									      this might be a "foreign" encoding for foreign versions of Windows 9x/NT).  
	wxFONTENCODING_DEFAULT		0	  1	  The applications default encoding as returned by wxFont::GetDefaultEncoding.
									      On program startup, the applications default encoding is the same as 
									      wxFONTENCODING_SYSTEM, but may be changed to make all the fonts created 
									      later to use it (by default). 
									      The 1..15 ISO8859 family encodings are usually used by all non-Microsoft 
									      operating systems
	wxFONTENCODING_ISO8859_1	1         West European (Latin1)  
	wxFONTENCODING_ISO8859_2	2	      Central and East European (Latin2)
	wxFONTENCODING_ISO8859_3	3	      Esperanto (Latin3)
	wxFONTENCODING_ISO8859_4	4	      Baltic (old) (Latin4)
	wxFONTENCODING_ISO8859_5	5	      Cyrillic
	wxFONTENCODING_ISO8859_6	6	      Arabic
	wxFONTENCODING_ISO8859_7	7	      Greek
	wxFONTENCODING_ISO8859_8	8	      Hebrew
	wxFONTENCODING_ISO8859_9	9	      Turkish (Latin5)
	wxFONTENCODING_ISO8859_10	10	      Variation of Latin4 (Latin6)
	wxFONTENCODING_ISO8859_11	11	      Thai
	wxFONTENCODING_ISO8859_12	12	      doesn't exist currently, but put here to make all ISO8859 consecutive numbers
	wxFONTENCODING_ISO8859_13	13	      Baltic (Latin7)
	wxFONTENCODING_ISO8859_14	14	      Latin8
	wxFONTENCODING_ISO8859_15	15	      Latin9 (a.k.a. Latin0, includes euro)
    wxFONTENCODING_ISO8859_MAX	16       (max value for the ISO8859 encodings)
	wxFONTENCODING_KOI8			17        Standard Cyrillic encoding for the Internet (but see also 
									      wxFONTENCODING_ISO8859_5 and wxFONTENCODING_CP1251)  
									      Cyrillic charset soup (see http://czyborra.com/charsets/cyrillic.html)
									      we don't support any of KOI8 variants
    wxFONTENCODING_KOI8_U		18	      KOI8 Ukrainian
    wxFONTENCODING_ALTERNATIVE	19	      same as MS-DOS CP866
    wxFONTENCODING_BULGARIAN	20	      used under Linux in Bulgaria
    wxFONTENCODING_CP437		21	 254  original MS-DOS codepage (what would we do without Microsoft? They have 
									      their own encodings for DOS)
    wxFONTENCODING_CP850		22	      CP437 merged with Latin1
    wxFONTENCODING_CP852		23	      CP437 merged with Latin2
    wxFONTENCODING_CP855		24	      another cyrillic encoding
    wxFONTENCODING_CP866		25	      and another one  
	
	wxFONTENCODING_CP874		26	 222  WinThai
    wxFONTENCODING_CP932		27	 128  Japanese (shift-JIS)
    wxFONTENCODING_CP936		28	 134  Chinese simplified (GB)
    wxFONTENCODING_CP949		29	 129  Korean (Hangul charset)
    wxFONTENCODING_CP950		30	 136  Chinese (traditional - Big5)
	wxFONTENCODING_CP1250		31   238  Microsoft analogue of ISO8859-2 "WinLatin2"
	wxFONTENCODING_CP1251		32   204  Microsoft analogue of ISO8859-5 "WinCyrillic"
	wxFONTENCODING_CP1252		33   0    Microsoft analogue of ISO8859-1 "WinLatin1"
    wxFONTENCODING_CP1253		34	 161  WinGreek (8859-7)
    wxFONTENCODING_CP1254		35	 162  WinTurkish
    wxFONTENCODING_CP1255		36	 177  WinHebrew
    wxFONTENCODING_CP1256		37	 178  WinArabic
    wxFONTENCODING_CP1257		38	 186  WinBaltic (same as Latin 7)
    wxFONTENCODING_CP12_MAX		39	      (max cp12... encoding defined)
    
	wxFONTENCODING_UTF7			40		  UTF-7 Unicode encoding
    wxFONTENCODING_UTF8			41		  UTF-8 Unicode encoding
    wxFONTENCODING_EUC_JP		42		  Extended Unix Codepage for Japanese
    wxFONTENCODING_UTF16BE		43		  UTF-16 Big Endian Unicode encoding
    wxFONTENCODING_UTF16LE		44		  UTF-16 Little Endian Unicode encoding
    wxFONTENCODING_UTF32BE		45		  UTF-32 Big Endian Unicode encoding
    wxFONTENCODING_UTF32LE		46		  UTF-32 Little Endian Unicode encoding

    wxFONTENCODING_MACROMAN		47	  77  the standard mac encodings [47-86]
    wxFONTENCODING_MACJAPANESE	48
    wxFONTENCODING_MACCHINESETRAD 49
    wxFONTENCODING_MACKOREAN	50
    wxFONTENCODING_MACARABIC	51
    wxFONTENCODING_MACHEBREW	52
    wxFONTENCODING_MACGREEK		53
    wxFONTENCODING_MACCYRILLIC	54
    wxFONTENCODING_MACDEVANAGARI 55
    wxFONTENCODING_MACGURMUKHI	56
    wxFONTENCODING_MACGUJARATI	57
    wxFONTENCODING_MACORIYA		58
    wxFONTENCODING_MACBENGALI	59
    wxFONTENCODING_MACTAMIL		60
    wxFONTENCODING_MACTELUGU	61
    wxFONTENCODING_MACKANNADA	62
    wxFONTENCODING_MACMALAJALAM	63
    wxFONTENCODING_MACSINHALESE	64
    wxFONTENCODING_MACBURMESE	65
    wxFONTENCODING_MACKHMER		66
    wxFONTENCODING_MACTHAI		67
    wxFONTENCODING_MACLAOTIAN	68
    wxFONTENCODING_MACGEORGIAN	69
    wxFONTENCODING_MACARMENIAN	70
    wxFONTENCODING_MACCHINESESIMP 71
    wxFONTENCODING_MACTIBETAN	72
    wxFONTENCODING_MACMONGOLIAN	73
    wxFONTENCODING_MACETHIOPIC	74
    wxFONTENCODING_MACCENTRALEUR 75
    wxFONTENCODING_MACVIATNAMESE 76
    wxFONTENCODING_MACARABICEXT	77
    wxFONTENCODING_MACSYMBOL	78
    wxFONTENCODING_MACDINGBATS	79
    wxFONTENCODING_MACTURKISH	80
    wxFONTENCODING_MACCROATIAN	81
    wxFONTENCODING_MACICELANDIC	82
    wxFONTENCODING_MACROMANIAN	83
    wxFONTENCODING_MACCELTIC	84
    wxFONTENCODING_MACGAELIC	85
    wxFONTENCODING_MACKEYBOARD	86

    wxFONTENCODING_MAX			87	  highest enumerated encoding value

    wxFONTENCODING_MACMIN = wxFONTENCODING_MACROMAN	47
    wxFONTENCODING_MACMAX = wxFONTENCODING_MACKEYBOARD	86

    // aliases for endian-dependent UTF encodings
#ifdef WORDS_BIGENDIAN
    wxFONTENCODING_UTF16 = wxFONTENCODING_UTF16BE	43	// native UTF-16
    wxFONTENCODING_UTF32 = wxFONTENCODING_UTF32BE	45	// native UTF-32
#else // WORDS_BIGENDIAN
    wxFONTENCODING_UTF16 = wxFONTENCODING_UTF16LE	44	// native UTF-16
    wxFONTENCODING_UTF32 = wxFONTENCODING_UTF32LE	46	// native UTF-32
#endif // WORDS_BIGENDIAN

    // alias for the native Unicode encoding on this platform
    // (this is used by wxEncodingConverter and wxUTFFile only for now)
#if SIZEOF_WCHAR_T == 2
    wxFONTENCODING_UNICODE = wxFONTENCODING_UTF16	43
#else // SIZEOF_WCHAR_T == 4
    wxFONTENCODING_UNICODE = wxFONTENCODING_UTF32	45
#endif

    // alternative names for Far Eastern encodings
        // Chinese
    wxFONTENCODING_GB2312 = wxFONTENCODING_CP936	28	134	// Simplified Chinese
    wxFONTENCODING_BIG5 = wxFONTENCODING_CP950		30  136	// Traditional Chinese

        // Japanese (see http://zsigri.tripod.com/fontboard/cjk/jis.html)
    wxFONTENCODING_SHIFT_JIS = wxFONTENCODING_CP932	27	128	// Shift JIS

	The wxWidgets docs add the following comments (from: "Font encoding overview"):
	As you may see, Microsoft's encodings partly mirror the standard ISO8859 ones, but there are (minor) 
	differences even between ISO8859-1 (Latin1, ISO encoding for Western Europe) and CP1251 (WinLatin1, 
	standard code page for English versions of Windows) and there are more of them for other encodings.
	The situation is particularly complicated with Cyrillic encodings for which (more than) three 
	incompatible encodings exist: KOI8 (the old standard, widely used on the Internet), ISO8859-5 
	(ISO standard for Cyrillic) and CP1251 (WinCyrillic).
	This abundance of (incompatible) encodings should make it clear that using encodings is less easy 
	than it might seem. The problems arise both from the fact that the standard encodings for the given 
	language (say Russian, which is written in Cyrillic) are different on different platforms and because 
	the fonts in the given encoding might just not be installed (this is especially a problem with Unix, 
	or, in general, non-Win32 systems).
	To clarify, the wxFontEnumerator class may be used to enumerate all available encodings, and to 
	find the facename(s) in which the given encoding exists. If you can find the font in the correct 
	encoding with wxFontEnumerator then your troubles are over, but, unfortunately, sometimes this is 
	not enough. For example, there is no standard way (that I know of, please tell me if you do!) to 
	find a font on a Windows system for KOI8 encoding (only for WinCyrillic one which is quite different), 
	so wxFontEnumerator will never return one, even if the user has installed a KOI8 font on his system.
	To solve this problem, a wxFontMapper class is provided. This class stores the mapping between the 
	encodings and the font face names which support them in wxConfig object. Of course, it would be 
	fairly useless if it tried to determine these mappings by itself, so, instead, it (optionally) 
	asks the user and remembers his answers so that the next time the program will automatically 
	choose the correct font.
	All these topics are illustrated by the font sample; please refer to it and the documentation of 
	the classes mentioned here for further explanations.

	Design Considerations for Adapt It development:
	1.	On modern Windows machines wxLocale::GetSystemEncoding() returns wxFONTENCODING_CP1252,
		and wxFontMapper::Get()->GetAltForEncoding() returns wxFONTENCODING_ISO8859_1.
		To accommodate the possible/likely migration of data from Windows environment to an
		Ubuntu Linux environment, we can set the initial encoding for source, target, nav,
		and the dialog fonts to wxFONTENCODING_ISO8859_1, so that the text controls and
		display contexts can convert any upper ASCII character data to UTF-8 without generating
		the "Failed to set text in text control" error (displaying it as "boxed" hex values 
		inline in the displayed string)
	2.	We can give the user the option in the fontPage to change the encoding for a given
		language font which would then override the default setting when setting up an
		Adapt It project.
	3.	Presumably, the Charset value stored for each Unicode font in the config files (of
		the Unicode version) would be 0 (zero), as the charset value is not significant for
		Unicode operations.
	4.	We can determine from the project config file whether the config file was generated 
		by the legacy MFC version or the newer wxWidgets version, by whether the config file
		has the "SourceHasUpperCaseAndLowerCase n" field in it (which is unique to the 
		wxWidgets version). Then, the Charset fields for each of the fonts read at the 
		beginning of the config files can be interpreted accordingly. If we know that the
		config file was generated by the wxWidgets version, we can accurately know the
		font encoding for the particular font in the data. If that font encoding is available
		on the current system all is OK; if the font encoding is not available, we can ask
		the user to select a font encoding via a "Choose Font Encoding" dialog for each font
		necessary when opening the project. 
		If the config file was generated by the MFC version, we can assume that we should be
		able to map the Charset from the MFC byte Charset values to any corresponding wxWidgets
		encodings. If a reliable mapping is not available, we can ask the user to select a font 
		encoding via a "Choose Font Encoding" dialog for each font necessary when opening the 
		project.
	*/

	// First, in case this is very first time app is run, default to a platform appropriate 
	// font for source, target, and navtext languages and also appropriate fonts for the
	// dialog fonts.
	// Notes: 
	//    1. wxFont objects are created on the heap, and must be deleted in OnExit();
	//    2. Vadim Zeitlin's suggestion for ANSI builds was to create a wxNORMAL_FONT and then 
	//       set its encoding appropriately (and use SetFont) for converting the smart quotes to 
	//       UTF-8 used internally by the wxGTK library
	if (m_pSourceFont == NULL)
		m_pSourceFont = new wxFont(*wxNORMAL_FONT); // m_pSourceFont = new wxFont(wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT));
	wxASSERT(m_pSourceFont != NULL);
	if (m_pTargetFont == NULL)
		m_pTargetFont = new wxFont(*wxNORMAL_FONT); // m_pTargetFont = new wxFont(wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT));
	wxASSERT(m_pTargetFont != NULL);
	if (m_pNavTextFont == NULL)
		m_pNavTextFont = new wxFont(*wxNORMAL_FONT); // m_pNavTextFont = new wxFont(wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT));
	wxASSERT(m_pNavTextFont != NULL);

	// Create suitable dialog fonts
	if (m_pDlgSrcFont == NULL)
		m_pDlgSrcFont = new wxFont(*wxNORMAL_FONT); // m_pDlgSrcFont = new wxFont(wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT));
	wxASSERT(m_pDlgSrcFont != NULL);
	if (m_pDlgTgtFont == NULL)
		m_pDlgTgtFont = new wxFont(*wxNORMAL_FONT); // m_pDlgTgtFont = new wxFont(wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT));
	wxASSERT(m_pDlgTgtFont != NULL);
	if (m_pDlgGlossFont == NULL)
		m_pDlgGlossFont = new wxFont(*wxNORMAL_FONT); // m_pDlgGlossFont = new wxFont(wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT));
	wxASSERT(m_pDlgGlossFont != NULL);
	if (m_pComposeFont == NULL)
		m_pComposeFont = new wxFont(*wxNORMAL_FONT); // m_pComposeFont = new wxFont(wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT));
	wxASSERT(m_pComposeFont != NULL);
	if (m_pRemovalsFont == NULL)
		m_pRemovalsFont = new wxFont(*wxNORMAL_FONT);
	wxASSERT(m_pRemovalsFont != NULL);
	if (m_pVertEditFont == NULL)
		m_pVertEditFont = new wxFont(*wxNORMAL_FONT);
	wxASSERT(m_pVertEditFont != NULL);

	// Set a suitable default font encoding. 
	// Note: To accommodate Word's "smart quotes," we'll set the default font encoding as
	// follows:
	// For ANSI builds, we'll use what Windows calls the ANSI_CHARSET (value 0), which in the 
	// wxWidgets' wxFontEncoding equivalent is wxFONTENCODING_CP1252.
	// For Unicode builds, we'll use the system default wxFONTENCODING_DEFAULT (enum value of 0) which on 
	// program startup should be the same as wxFONTENCODING_SYSTEM (enum value of -1).
#ifndef _UNICODE
	// For ANSI use a default Windows encoding CP1252 which can represent the smart quotes

	// whm added 19Feb07 
	// The following static function call sets the default font encoding application wide to 
	// wxFONTENCODING_CP1252 so that any new font created with no parameter or with wxNORMAL_FONT
	// as parameter will use this encoding
	wxFont::SetDefaultEncoding(wxFONTENCODING_CP1252);

	// whm TODO: Some of the wxFontEncoding identifiers below duplicate the functions of others 
	// that are initialized in OnInit(), namely m_srcEncoding and m_tgtEncoding. Eliminate the
	// duplication if possible.
	m_srcEncoding = wxFONTENCODING_CP1252;
	m_tgtEncoding = wxFONTENCODING_CP1252;
	m_navtextFontEncoding = wxFONTENCODING_CP1252;
	
	m_dlgSrcFontEncoding = wxFONTENCODING_CP1252;
	m_dlgTgtFontEncoding = wxFONTENCODING_CP1252;
	m_dlgGlossFontEncoding = wxFONTENCODING_CP1252;
	m_composeFontEncoding = wxFONTENCODING_CP1252;
	m_removalsFontEncoding = wxFONTENCODING_CP1252;
	m_vertEditFontEncoding = wxFONTENCODING_CP1252;
#else
	// For Unicode use a default system encoding
	m_srcEncoding = wxFONTENCODING_DEFAULT;
	m_tgtEncoding = wxFONTENCODING_DEFAULT;
	m_navtextFontEncoding = wxFONTENCODING_DEFAULT;
	
	m_dlgSrcFontEncoding = wxFONTENCODING_DEFAULT;
	m_dlgTgtFontEncoding = wxFONTENCODING_DEFAULT;
	m_dlgGlossFontEncoding = wxFONTENCODING_DEFAULT;
	m_composeFontEncoding = wxFONTENCODING_DEFAULT;
	m_removalsFontEncoding = wxFONTENCODING_DEFAULT;
	m_vertEditFontEncoding = wxFONTENCODING_DEFAULT;
#endif

	// set the initial encoding of the major language fonts
	m_pSourceFont->SetEncoding(m_srcEncoding);
	m_pTargetFont->SetEncoding(m_tgtEncoding);
	m_pNavTextFont->SetEncoding(m_navtextFontEncoding);
	
	m_pDlgSrcFont->SetEncoding(m_dlgSrcFontEncoding);
	m_pDlgTgtFont->SetEncoding(m_dlgTgtFontEncoding);
	m_pDlgGlossFont->SetEncoding(m_dlgGlossFontEncoding);
	m_pComposeFont->SetEncoding(m_composeFontEncoding);
	m_pRemovalsFont->SetEncoding(m_removalsFontEncoding);
	m_pVertEditFont->SetEncoding(m_vertEditFontEncoding);
	

	// create the corresponding wxFontData ojbects on the heap; delete them in OnExit()
	// In wxWidgets these wxFontData objects are used mainly to get the user's 
	// chosen font from a font dialog, and to call Get/SetColour() methods to
	// get/set the font color.
	if (m_pSrcFontData == NULL)
		m_pSrcFontData = new wxFontData;	// must delete in OnExit()
	wxASSERT(m_pSrcFontData != NULL);
	if (m_pTgtFontData == NULL)
		m_pTgtFontData = new wxFontData;	// must delete in OnExit()
	wxASSERT(m_pTgtFontData != NULL);
	if (m_pNavFontData == NULL)
		m_pNavFontData = new wxFontData;	// must delete in OnExit()
	wxASSERT(m_pNavFontData != NULL);

	// For wxWidgets we have our own fontInfo structure to hold the legacy MFC 
	// LOGFONT struct-type data, mainly for use in maintaining backward compatibility 
	// in reading and writing config files. 
	// We'll initialize the fontInfo values and set the necessary font characteristics 
	// in those fontInfo structs to use the default system fonts as created above.

	// For our default system Source Font, we only set its size to 12 points and its
	// weight to bold and its color (via its wxFontData class) to bright blue.
	// We don't specify the face name nor the family here as we want to use whatever
	// the GetFont() call above gave us as a system font for the current platform.
	m_pSourceFont->SetPointSize(12); // 12 point
	m_pSourceFont->SetWeight(wxBOLD);
	m_sourceColor = wxColour(0,0,255); // bright blue "16711680"
	// It is necessary to call the font data's SetColour() function to effect color change
	m_pSrcFontData->SetColour(m_sourceColor);

	// For our default system Target Font, we only set its size to 12 points and its
	// weight to bold and its color (via its wxFontData class) to black.
	// We don't specify the face name nor the family here as we want to use whatever
	// the GetFont() call above gave us as a system font for the current platform.
	m_pTargetFont->SetPointSize(12); // 12 point
	m_pTargetFont->SetWeight(wxBOLD);
	m_targetColor = wxColour(0,0,0); // black 
	// It is necessary to call the font data's SetColour() function to effect color change
	m_pTgtFontData->SetColour(m_targetColor); // 

	// For our default system Nav Text Font, we only set its size to 12 points and its
	// weight to bold and its color (via its wxFontData class) to bright green.
	// We don't specify the face name nor the family here as we want to use whatever
	// the GetFont() call above gave us as a system font for the current platform.
	m_pNavTextFont->SetPointSize(11); // make it 11 pt - a little smaller
	m_pNavTextFont->SetWeight(wxBOLD);
	m_navTextColor = wxColour(0,255,0); //bright green "65280"
	// It is necessary to call the font data's SetColour() function to effect color change
	m_pNavFontData->SetColour(m_navTextColor); // 

	m_specialTextColor = wxColour(255,0,0); // red "255" - whm added
	m_reTranslnTextColor = wxColour(160,80,0); // mid brown "32896" - whm added
	m_AutoInsertionsHighlightColor = wxColour(203,151,255); // solid light purple "16750539"
	m_freeTransTextColor = wxColour(100,0,100); // dark purple - fixed not user selectable

	// color used for read-only text controls displaying static text info button face color
	sysColorBtnFace = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	// color used for read-only text controls displaying static text info window background color
	sysColorWindow = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);

	// The following values for fonInfo echo the defaults above using wxFont methods.
	// All the defaults set here in Initialize will be used on first run of program, or
	// any time the user holds the shift key down during program startup. These values
	// will normally change to the user's preferences when the config file is read.

	SrcFInfo.fLangType = _T("SourceFont");
	TgtFInfo.fLangType = _T("TargetFont");
	NavFInfo.fLangType = _T("NavTextFont");

	// The UpdateFontInfoStruct functions below assign the font characteristics of the
	// font in the first argument to the corresponding fontInfo struct passed in the
	// second argument.
	UpdateFontInfoStruct(m_pSourceFont,SrcFInfo);
	UpdateFontInfoStruct(m_pTargetFont,TgtFInfo);
	UpdateFontInfoStruct(m_pNavTextFont,NavFInfo);

	// the following are additional font characteristics not used by wxWidgets 
	// but are stored in the legacy config files. We set the default values to 
	// arbitrary defaults initially. If a legacy config file is read, the existing
	// values are stored in the appropriate fontInfo fields and used only for
	// writing the values back out to the config files.
	SrcFInfo.fWidth = 0;
	TgtFInfo.fWidth = 0;
	NavFInfo.fWidth = 0;

	SrcFInfo.fEscapement = 0;
	TgtFInfo.fEscapement = 0;
	NavFInfo.fEscapement = 0;

	SrcFInfo.fOrientation = 0;
	TgtFInfo.fOrientation = 0;
	NavFInfo.fOrientation = 0;

	SrcFInfo.fStrikeOut = FALSE;
	TgtFInfo.fStrikeOut = FALSE;
	NavFInfo.fStrikeOut = FALSE;

	SrcFInfo.fCharset = 0; // 0 is the value for ANSI_CHARSET, presumably same for Unicode encodings
	TgtFInfo.fCharset = 0; // 0 is the value for ANSI_CHARSET, presumably same for Unicode encodings
	NavFInfo.fCharset = 0; // 0 is the value for ANSI_CHARSET, presumably same for Unicode encodings

	SrcFInfo.fOutPrecision = 1;
	TgtFInfo.fOutPrecision = 1;
	NavFInfo.fOutPrecision = 1;

	SrcFInfo.fClipPrecision = 2;
	TgtFInfo.fClipPrecision = 2;
	NavFInfo.fClipPrecision = 2;

	SrcFInfo.fQuality = 2;
	TgtFInfo.fQuality = 2;
	NavFInfo.fQuality = 2;

}// end of InitializeFonts()

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: the App's OnInit() and from the App's SetDefaults() if the user holds the
/// shift key down during startup, or if there is no existing configuration files.
/// InitializePunctuation() handles the initial creation of the default punctuation characters
/// used in the application. Normally, the initial punctuation characters are replaced by those
/// stored in the basic and project configuration files when they are read.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::InitializePunctuation()
{
	// initialize the punctuation pairs array to all zeros (can be unicode) '\0' is okay for 
	// unicode or not
	for (int i=0; i<MAXPUNCTPAIRS; i++)
	{
		m_punctPairs[i].charSrc = '\0';
		m_punctPairs[i].charTgt = '\0';
	}

	// initialize the two-punctuation pairs array to all zeros
	for (int i=0; i<MAXTWOPUNCTPAIRS; i++)
	{
		m_twopunctPairs[i].twocharSrc[0] = '\0';
		m_twopunctPairs[i].twocharTgt[0] = '\0';
		m_twopunctPairs[i].twocharSrc[1] = '\0';
		m_twopunctPairs[i].twocharTgt[1] = '\0';
	}

	// whm moved following here from block after GetBasicConfigFileSettings() call in
	// OnInit() where bConfigFilesRead was false. This was done to simplify the handling
	// of default punctuation which should be consolidated in InitializePunctuation.
	//if (m_punctuation[0].IsEmpty())
	//{
		// leave this block here, but we no longer set up the defaults here, but rather in
		// the code following this block
		m_punctuation[0] = _T(" . , < > ; ? ! : ( ) \" { } [ ] "); // defaults for narrow or 
																// wide characters
		m_punctWordBuilding[0] = _T(""); // leave this in the code, just done use them --
										// since reading an old config file would require them
		if (m_punctuation[1].IsEmpty())
		{
			m_punctuation[1] = m_punctuation[0];
			m_punctWordBuilding[1] = m_punctWordBuilding[0]; // leave here, for above reason
		}

		// next, set up the correspondence strings...
		// BEW inserted in following section on March 17, 2005 
		// for support of curly quotes as punctuation for stripping out
		wxString additions;
		additions.Empty();
		wxString ch = _T("."); // we overwrite the full stop below (can't leave it empty)

		// next, set up the correspondence strings
#ifdef _UNICODE // Unicode version
		wxString strPunctPairsSrcSet;
		wxString strPunctPairsTgtSet;
		wxString strTwoPunctPairsSrcSet;
		wxString strTwoPunctPairsTgtSet;
		
		strPunctPairsSrcSet = _T("?.,;:\"!()<>{}[]");

		ch.SetChar(0,L'\x201C'); // hex for left double quotation mark
		additions += ch;
		ch.SetChar(0,L'\x201D'); // hex for right double quotation mark
		additions += ch;
		ch.SetChar(0,L'\x2018'); // hex for left single quotation mark
		additions += ch;
		ch.SetChar(0,L'\x2019'); // hex for right single quotation mark
		additions += ch;
		strPunctPairsSrcSet += additions;
		m_bSingleQuoteAsPunct = FALSE;

		strPunctPairsTgtSet = strPunctPairsSrcSet;
		strTwoPunctPairsSrcSet.Empty();
		strTwoPunctPairsTgtSet = strTwoPunctPairsSrcSet;
		TwoStringsToPunctPairs(m_punctPairs,strPunctPairsSrcSet,strPunctPairsTgtSet);
		TwoStringsToTwoPunctPairs(m_twopunctPairs,strTwoPunctPairsSrcSet,strTwoPunctPairsTgtSet);
#else // ANSI version
		wxString strValue = _T("??..,,;;::\"\"!!(())<<>>[[]]{{}}");

		// For Windows, we can support curly quotes like MFC does in the ANSI version.
		// When first converted to wxWidgets, GTK's text control was failing when use SetValue() with 
		// any of these upper ascii values in the punctuation string. A partial solution was to insure 
		// that SetEncoding(wxFONTENCODING_CP1252) was called on the fonts involved. Note: SetEncoding() 
		// is an "undocumented" method of wxFont. The documented SetDefaultEncoding() did not seem to
		// work. Also setting the encoding to wxFONTENCODING_ISO8859_1 did not work (it eliminated
		// the "Failed to set text in text control" message, but showed the smart quotes as boxes
		// with the hex values in them, instead of the actual quote characters).
		// Because it was only a partial solution and problems seemed to continually crop up, in the 
		// end I decided to include smart quotes only in the wxMSW ANSI version and not in wxGTK builds.
#ifdef __WXMSW__
		unsigned char thechar = 147; // left double quotation mark
		ch.SetChar(0,thechar);
		additions += ch;
		additions += ch;
		thechar = 148;// right double quotation mark
		ch.SetChar(0,thechar);
		additions += ch;
		additions += ch;
		thechar = 145; // left single quotation mark
		ch.SetChar(0,thechar);
		additions += ch;
		additions += ch;
		thechar = 146; // right single quotation mark
		ch.SetChar(0,thechar);
		additions += ch;
		additions += ch;
		strValue += additions;
#endif
		// wxGTK uses UTF-8 internally in its wxTextCtrl widgets, so let's try converting the punctuation
		// string to UTF-8 for wxGTK ANSI builds.
//#ifdef __GNUG__
//			// The following converts strValue to 
//			wxConvUTF8.cWC2MB(wxConvCurrent->cMB2WC(strValue));  
//#endif
		//int found = strValue.Find(_T('\''),0); // look for vertical ordinary quote (ie. apostrophe)
		//if (found >= 0)
		//	m_bSingleQuoteAsPunct = TRUE;
		//else
			m_bSingleQuoteAsPunct = FALSE; // default setup can only can be this

		StringToPunctPairs(m_punctPairs,strValue);
		strValue.Empty();
		StringToTwoPunctPairs(m_twopunctPairs,strValue);
#endif
		// end of BEW insertions for March 17, 2005

	//}

}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return    TRUE if the basic configuration file exists and was successfully read, FALSE
///             if the basic config file could not be read or if the user held the SHIFT key
///             down on application startup 
/// \remarks
/// Called from: the App's OnInit().
/// Reads the basic configuration file (AI-BasicConfiguration.aic) by calling the 
/// GetConfigurationFile() function with szBasicConfiguration passed as input parameter.
/// If the user holds down the SHIFT key during application startup, or if the configuration
/// file cannot be read, this function calls SetDefaults() instead.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::GetBasicConfigFileSettings() // whm 20Jan08 changed signature to return bool
{
	// Called from OnInit() at program startup.
	// If user starts up with the SHIFT key down, or if an existing basic config file cannot
	// be read successfully, SetupDefaults() is called and all initializations and settings 
	// are drawn from there. Otherwise, intializations and settings contained in the basic
	// config file are used
	// Now attempt to set source, target language fonts, and nav text font set up from the
	// data in the configuration file
	// ::wxGetKeyState() was not available as a global function in wxWidgets 
	// version 2.4.2, but ::wxGetKeyState() is available in wxWidgets version 2.5.3 
	// and later, so we use it here.
	bool bReturn = FALSE;
	if (!wxGetKeyState(WXK_SHIFT)) // if (keyState != WXK_SHIFT)
	{
		// Shift key is not down, so load the config file data for fonts & other settings.
		// This version of the function uses configuration file, not the registry.
		bReturn = GetConfigurationFile(szBasicConfiguration,m_workFolderPath,1 );// 1 is app level .aic
		if (!bReturn)
		{
			// we want defaults, if it fails - as it will on very first launch of the app
			SetDefaults();
		}
	}
	else
	{
		// SHIFT key is down at launch time, so user wants to bypass configuration file settings 
		// (which may be corrupt), so set up default values for the settings needed for a 
		// successful launch
		SetDefaults();
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return    TRUE if the directory structure and KBs were created successfully and/or exist, 
///             FALSE if an error occurred. 
/// \remarks
/// Called from: the Doc's OnOpenDocument(), DoUnpackDocument(), and from 
/// CLanguagesPage::OnWizardPageChanging() when the wizard page is moving forward.
/// SetupDirectories() insures that the appropriate directory/folder structure is created 
/// when a new project is started, and that the appropriate directory/folder structure exists
/// or can be created prior to opening or unpacking a document.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::SetupDirectories()
{

		// this is where we have to start setting up the directory structures
		bool bWorkExists = FALSE;
		if (::wxFileExists(m_workFolderPath) || ::wxDirExists(m_workFolderPath))
		// The Docs say ::wxFileExists() "returns TRUE if the file exists. It also returns TRUE
		// if the file is a directory", however, I've found that the second statement is false,
		// so, I'm ORing it with ::wxDirExists().
		{
			if (::wxDirExists(m_workFolderPath))
			{
				// work folder already exists, so don't create it
				bWorkExists = TRUE;
			}
			else
			{
				// Work folder does not yet exist, but a file of
				// that name does exist, so it must be renamed before
				// user can proceed. Abort after a message is easiest way.
				wxString str;
				// IDS_FILENAME_CLASH
				str = str.Format(_("Sorry, there is a file named 'Adapt It %sWork'. Please delete or rename this file because Adapt It needs to use this name instead for a folder."),m_strNR.c_str());
				wxMessageBox(str, _T(""), wxICON_ERROR);
				wxExit();
			}
		}
		else
		{
			// Work folder does not yet exist, so create it.
			bool bOK = ::wxMkdir(m_workFolderPath); //bool bOK = ::CreateDirectory(m_workFolderPath,NULL);
			// WX NOTE: On Unix/Linux wxMkdir has a second default param: int perm = 0777 which
			// makes a directory with full read, write, and execute permissions.
			if (!bOK)
			{
				wxString str;
				// IDS_CREATE_DIR_FAILED
				str = str.Format(_("Sorry, there was an error creating the \"Adapt It %sWork\" folder in your <home user>/My Documents folder.\nAdapt It %s is not set up correctly and so must close down."),m_strNR.c_str(),m_strNR.c_str());
				wxMessageBox(str, _T(""), wxICON_ERROR);
				wxASSERT(FALSE);
				return FALSE;
			}
			bWorkExists = TRUE; // it now exists
		}

		// at this point we have an "Adapt It Work" folder existing in My Documents
		// (If we loaded a document from the MRU list, m_sourceName and m_targetName are
		// serialized on the doc, and so we can determine the doc's project folder name from
		// those now too; and the wxString buffer has been pressed into service in the serialization
		// code to store and load book mode information on the document, so that is available now
		// to - all except m_bibleBooksFolderPath which we must recreate here below a bit)
		// whm: For localization purposes the " to " and " adaptations" strings should not be 
		// translated, otherwise other localizations would not be able to handle the unpacking 
		// of files created on different localizations.
		wxString workFolder = m_sourceName + _T(" to ") + m_targetName + _T(" adaptations");
		m_curProjectName = workFolder;
		wxASSERT(!m_curProjectName.IsEmpty());
		m_curProjectPath = m_workFolderPath + PathSeparator + m_curProjectName;

		// check to see if this folder already exists
		bool bLangWorkFolderExists = FALSE;
		if (::wxFileExists(m_curProjectPath) || ::wxDirExists(m_curProjectPath))
		{
			if (::wxDirExists(m_curProjectPath))
			{
				// language-specific work folder already exists, so don't create it
				bLangWorkFolderExists = TRUE;
				if (!gbViaMostRecentFileList && pStartWorkingWizard == NULL)
				{
					// if we are not in the startup wizard, then to set up same project
					// again would likely be an error made by a novice user, thinking he
					// was creating a new document, so he should be given this message. But
					// the message should be suppressed in the wizard, since the wizard allows
					// the user to navigate back to the start of the wizard, so having set up
					// a new project, he could navigate back, and then use Next> buttons again,
					// and so we would want the use of the existing folder to be done silently
					// BEW added 10Jan06; the Unpack Document... command calls SetupDirectories()
					// from its handler on the destination machine, and if there is a project of
					// the same name already on the same machine (eg. as would be the case if an
					// unpack was done of a second document file from the same source project)
					// we would not want this message to come up each time; nor even once, since
					// the destination machine's user expects the project to be automatically
					// set up and it would be a nuisance to mention it already exists. The
					// m_bUnpacking flag is cleared in the doc class's OnFileUnpackDoc() function.
					if (!m_bUnpacking)
					{
						wxString str;
						// IDS_DUP_PROJECT_NAME
						str = str.Format(_("Warning: a project with the name \"%s\" already exists. Adapt It will therefore use the existing project."),workFolder.c_str());
						// use wxMessageDialog rather than wxMessageBox below. For some reason the dialog
						// would have a null parent when loading a file from MRU, and wxMessageDialog
						// allows us to specify the main frame as parent.
						wxMessageBox(str, _T(""), wxICON_INFORMATION);
					}
				}
			}
			else
			{
				// language-specific Work folder does not yet exist, but a file of
				// that name does exist, so it must be renamed before
				// user can proceed. Abort after a message is easiest way.
				wxString text;
				// IDS_FILENAME2_CLASH
				text = text.Format(_("Sorry, there is a file named \"%s to %s adaptations\" in your Adapt It %sWork folder. You must rename or delete this file because Adapt It needs to use this name for a folder."),m_sourceName.c_str(),m_targetName.c_str(),m_strNR.c_str());
				wxMessageBox(text, _T(""), wxICON_ERROR);
				wxExit();
			}
		}
		else
		{
			// language-specific Work folder does not yet exist, so create it.
			bool bOK = ::wxMkdir(m_curProjectPath); //bool bOK = ::CreateDirectory(m_curProjectPath,NULL);
			// WX NOTE: On Unix/Linux wxMkdir has a second default param: int perm = 0777 which
			// makes a directory with full read, write, and execute permissions.
			if (!bOK)
			{
				wxString str;
				// IDS_CREATE_DIR2_FAILED
				str = str.Format(_("Sorry, there was an error creating the \"%s to %s adaptations\" folder in your Adapt It %sWork folder. Adapt It is not set up correctly and so must close down."),m_sourceName.c_str(),m_targetName.c_str(),m_strNR.c_str(),m_strNR.c_str());
				wxMessageBox(str, _T(""), wxICON_ERROR);
				wxASSERT(FALSE);
				return FALSE;
			}
			bLangWorkFolderExists = TRUE; // it now exists
		}

		// we now have a location in which we can store the adaption project's knowledge base,
		// and we have to finally create an Adaptations folder here too for the adapted output.
		// do the Adaptations folder first
		m_curAdaptionsPath = m_curProjectPath + PathSeparator + m_adaptionsFolder;

		// check to see if this folder already exists
		bool bAdaptionsFolderExists = FALSE;
		if (::wxFileExists(m_curAdaptionsPath) || ::wxDirExists(m_curAdaptionsPath))
		{
			if (::wxDirExists(m_curAdaptionsPath))
			{
				// Adaptions folder already exists, so don't create it
				bAdaptionsFolderExists = TRUE;
			}
			else
			{
				// adaptations folder does not yet exist, but a file of
				// that name does exist, so it must be renamed before
				// user can proceed. Abort after a message is easiest way.
				wxString text;
				// IDS_ADAPTATIONS_CLASH // MFC error message has "Adaptions" rather than "Adaptations"
				text = text.Format(_("Sorry, there is a file named \"Adaptations\" in your \"%s\" folder. Please delete or rename it because Adapt It needs to use that name for a directory instead."),m_curProjectName.c_str());
				wxMessageBox(text, _T(""), wxICON_ERROR);
				wxExit();
			}
		}
		else
		{
			// language-specific Work folder does not yet exist, so create it.
			bool bOK = ::wxMkdir(m_curAdaptionsPath); //bool bOK = ::CreateDirectory(m_curAdaptionsPath,NULL);
			// WX NOTE: On Unix/Linux wxMkdir has a second default param: int perm = 0777 which
			// makes a directory with full read, write, and execute permissions.
			if (!bOK)
			{
				// IDS_CREATE_DIR3_FAILED // MFC error message has "Adaptions" rather than "Adaptations"
				wxMessageBox(_("Sorry, there was an error creating the \"Adaptations\" folder in your project folder. Adapt It is not set up properly and so must close down."),_T(""), wxICON_ERROR);
				wxASSERT(FALSE);
				return FALSE;
			}
			bAdaptionsFolderExists = TRUE; // it now exists
		}

		// we have the desired directory structures. But in case the document was a MRU list
		// one saved when book mode was on, we must set up the appropriate book folder path
		// or not, depending on the serialized in information in the document (SetupDirectories()
		// is called immediately after a MRU document is serialized in, so the user can have done
		// nothing to muck up the restored book mode settings which we will use now
		
		// BEW added 09Jan06; we also need to do this in case SetupDirectories() was called on
		// someone else's computer as part of the Unpack Document... command, because it cannot
		// be certain that that person will already have turned on book mode, but the person
		// who packed the document may have had it on. In fact, Pack and Unpack forces us to add
		// code here to check the relevant folders have been created in the Adaptations folder,
		// and if not, we must first create them and set the m_pCurrBookNamePair app member.
		if (m_bBookMode && !m_bDisableBookMode)
		{
			// check the book folders are already present, and if not then create them
			// whm note: AreBookFoldersCreated() has the side effect of changing the current work directory
			// to the passed in m_curAdaptionsPath.
			bool bFoldersPresent = AreBookFoldersCreated(m_curAdaptionsPath);
			if (!bFoldersPresent)
			{
				CreateBookFolders(m_curAdaptionsPath,m_pBibleBooks);
			}

			// set the current folder and its path
			m_pCurrBookNamePair = ((BookNamePair*)(*m_pBibleBooks)[m_nBookIndex]);
			m_bibleBooksFolderPath = m_curAdaptionsPath + PathSeparator + m_pCurrBookNamePair->dirName;
		}
		else
		{
			m_bibleBooksFolderPath.Empty();
			m_pCurrBookNamePair = NULL;
			if (m_bDisableBookMode)
				gbAbortMRUOpen = TRUE;
		}
		
		//Now we need to get a KB initialized and stored in the languages-specific folder. 
		// Ditto for the glossing KB (version 2)
		// BEW changed 15Aug05
		SetupKBPathsEtc();
		if (::wxFileExists(m_curKBPath))
		{
			// there is an existing .KB file, so we need to create a CKB instance in
			// memory, open the .KB file on disk, and fill the memory instance's members
			wxASSERT(m_pKB == NULL);
			m_pKB = new CKB;
			wxASSERT(m_pKB != NULL);
			bool bOK = LoadKB();
			if (bOK)
			{
				m_bKBReady = TRUE;

				// now do it for the glossing KB
				wxASSERT(m_pGlossingKB == NULL);
				m_pGlossingKB = new CKB;
				wxASSERT(m_pGlossingKB != NULL);
				bOK = LoadGlossingKB();
				if (bOK)
				{
					m_bGlossingKBReady = TRUE;
				}
				else
				{
					// IDS_LOAD_GLOSSINGKB_FAILURE
					wxMessageBox(_("Error: loading the glossing knowledge base failed. The application will now close."),_T(""), wxICON_ERROR);
					wxASSERT(FALSE);
					wxExit();
				}
			}
			else
			{
				// IDS_LOAD_KB_FAILURE
				wxMessageBox(_("Error: loading a knowledge base failed. The application will now close."),_T(""), wxICON_ERROR);
				wxASSERT(FALSE);
				wxExit();
			}
		}
		else
		{
			// the KB file does not exist, so make sure there is an initialized CKB instance 
			// on the application ready to receive data, and save it to disk.
			// for version 2, do the same for the glossing KB
			wxASSERT(m_pKB == NULL);
			m_pKB = new CKB;
			wxASSERT(m_pKB != NULL);

			// store the language names in it
			m_pKB->m_sourceLanguageName = m_sourceName;
			m_pKB->m_targetLanguageName = m_targetName;

			bool bOK = StoreKB(FALSE); // first time, so we can't make a backup
			if (bOK)
			{
				m_bKBReady = TRUE;

				// now do the same for the glossing KB
				wxASSERT(m_pGlossingKB == NULL);
				m_pGlossingKB = new CKB;
				wxASSERT(m_pGlossingKB != NULL);

				bOK = StoreGlossingKB(FALSE); // first time, so we can't make a backup
				if (bOK)
				{
					m_bGlossingKBReady = TRUE;
				}
				else
				{
					// IDS_STORE_GLOSSINGKB_FAILURE
					wxMessageBox(_("Error: storing the glossing knowledge base to disk for the first time failed. The application will now close."),_T(""), wxICON_ERROR); // something went wrong
					wxASSERT(FALSE);
					wxExit();
				}
			}
			else
			{
				// IDS_STORE_KB_FAILURE
				wxMessageBox(_("Error: saving the knowledge base failed. The application will now close."),_T(""), wxICON_ERROR); // something went wrong
				wxASSERT(FALSE);
				wxExit();
			}
		}
		return m_bKBReady;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return    nothing 
/// \remarks
/// Called from: the App's SetupDirectories(), the Doc's DoUnpackDocument(), and from 
/// CProjectPage::OnWizardPageChanging() when the wizard page is moving forward.
/// SetupKBPathsEtc() composes the path and file names for the regular KB and its backup
/// and the path and file names for the glossing KB and its backup.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::SetupKBPathsEtc()
{
	wxString saveName;
	if (m_bSaveAsXML) // always true in wx version
	{
		// XML input (with binary as the alternative)
		m_curKBName = m_curProjectName + _T(".xml");
		m_altKBName = m_curProjectName + _T(".KB");
		m_curKBPath = m_curProjectPath + PathSeparator + m_curKBName;
		m_altKBPath = m_curProjectPath + PathSeparator + m_altKBName;
		m_curKBBackupPath = m_curProjectPath + PathSeparator + m_curProjectName + _T(".BAK") + _T(".xml");
		m_altKBBackupPath = m_curProjectPath + PathSeparator + m_curProjectName + _T(".BAK");

		// now the same stuff for the glossing KB
		saveName = m_curGlossingKBName;
		m_curGlossingKBPath = m_curProjectPath + PathSeparator + saveName + _T(".xml");
		m_altGlossingKBPath = m_curProjectPath + PathSeparator + saveName + _T(".KB");
		m_curGlossingKBBackupPath = m_curProjectPath + PathSeparator + saveName + _T(".BAK") + _T(".xml");
		m_altGlossingKBBackupPath = m_curProjectPath + PathSeparator + saveName + _T(".BAK");
	}
	//else
	//{
	//	// binary input (with .XML as the alternative)
	//	m_curKBName = m_curProjectName + _T(".KB");
	//	m_altKBName = m_curProjectName + _T(".xml");
	//	m_curKBPath = m_curProjectPath + PathSeparator + m_curKBName;
	//	m_altKBPath = m_curProjectPath + PathSeparator + m_altKBName;
	//	m_curKBBackupPath = m_curProjectPath + PathSeparator + m_curProjectName + _T(".BAK");
	//	m_altKBBackupPath = m_curProjectPath + PathSeparator + m_curProjectName + _T(".BAK") + _T(".xml");

	//	// now the same stuff for the glossing KB
	//	saveName = m_curGlossingKBName;
	//	m_curGlossingKBPath = m_curProjectPath + PathSeparator + saveName + _T(".KB");
	//	m_altGlossingKBPath = m_curProjectPath + PathSeparator + saveName + _T(".xml");
	//	m_curGlossingKBBackupPath = m_curProjectPath + PathSeparator + saveName + _T(".BAK");
	//	m_altGlossingKBBackupPath = m_curProjectPath + PathSeparator + saveName + _T(".BAK") + _T(".xml");
	//}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     always TRUE
/// \param      punctPgCommon   -> pointer to the actual punctuation page in use
/// \param      reparseDoc      -> enum value of DoReparse (NoReparse if available but not used)
/// \remarks
/// Called from: CPunctCorrespPagePrefs::OnOK().
/// DoPunctuationChanges() detects if the user has changed any punctuation correspondences. If
/// so, it calls the Doc's RetokenizeText() function to rebuild the document with the new
/// punctuation scheme. It also insures that safe indices are set up and that the phrasebox is 
/// appropriately located after the doc rebuild is complete. 
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::DoPunctuationChanges(CPunctCorrespPageCommon* punctPgCommon, enum Reparse reparseDoc)
{
	// whm added 26Apr05
	// BEW ammended 06May05 to handle the possibility of variation in the final box location and its
	// contents, and to get the indices set up safely before RedrawEverything() is called in the caller.
	// This function currently only has code to detect punctuation changes and
	// if reparse is called for executes the main block and calls RetokenizeText().
	if (reparseDoc == DoReparse)
	{
		CAdapt_ItDoc* pDoc = GetDocument();
		CAdapt_ItView* pView = gpApp->GetView();

		// if the source punctuation was empty and remains so, then don't retokenize
		if (!(punctPgCommon->m_punctuationBeforeEdit[0].IsEmpty() && m_punctuation[0].IsEmpty()))
		{
			if (punctPgCommon->m_punctuationBeforeEdit[0] != m_punctuation[0])
			{
				// the source punctuation list has changed, so do the retokenization
				int nOldCount = m_pSourcePhrases->GetCount();
				int difference = 0;

				// make sure we don't lose the active sourcephrase's contents from the KB
				// (if the sourcephrase's m_bHasKBEntry is TRUE, then no store will be done - which
				// is what we'd want because that means it has already been done; but if FALSE, then
				// the store gets done (which again is what we want, because the FALSE value means
				// that the active location's KB entry has already been removed (or its count
				// decremented) and so a re-store is appropriate now -- because the box location may
				// end up elsewhere depending on the results of the rebuild process.)
				pView->StoreBeforeProceeding(m_pActivePile->m_pSrcPhrase);// ignore returned BOOL

				wxString strSavePhraseBox = m_targetPhrase;
				// now do the reparse - the functions which effect the reconstitution of the doc when 
				// punctuation was changed, or when filtering settings were changed, or both, will 
				// progressively update the view's m_nActiveSequNum value as necessary so that the stored
				// phrase box contents and the final active location remain in synch.
				int nNewSrcPhraseCount = pDoc->RetokenizeText(TRUE,	// TRUE = punctuation only changing here
															FALSE,	// bFilterChange FALSE = no filter changes here
															FALSE);	// bSfmSetChange FALSE = no sfm set change here
				m_maxIndex = nNewSrcPhraseCount - 1; // update

				// set up some safe indices, since the counts could be quite different than before
				difference = nNewSrcPhraseCount - nOldCount; // could even be negative, but unlikely
				// try expand sufficiently to encompass the active location within the lengthened
				// bundle, assuming it actually is lengthened (it won't be if difference is zero)
				m_endIndex = wxMin(gpApp->m_maxIndex,m_endIndex + difference);
				// if difference was huge, we may need to further reduce the value
				m_endIndex = wxMin(m_endIndex,m_beginIndex + m_nMaxToDisplay);
				m_upperIndex -= m_nFollowingContext;
				if (m_upperIndex < 0)
					m_upperIndex = m_endIndex;

				// Unfortunately, we can't ensure the viability of the saved phrase box contents - there are
				// several scenarios that force us to abandon its contents - some are for filtering changes
				// and so will be dealt with in DoUsfmFilterChanges(), but the one which concerns us for 
				// punctuation changes is the following:
				//		So many extra source phrases were produced that the updated active location got
				//		pushed so far to the right that it is greater than m_beginIndex + m_nMaxToDisplay. 
				// If the latter obtains, then we'll have to find a new safe phrase box location within the
				// current bundle bounds - which also means we have to check for retranslations and avoid those;
				// and we should remove the KB entry for the active location and set the sourcephrase's
				// m_bHasKBEntry (or if glossing, m_bHasGlossingKBEntry) to FALSE so that the next move of the
				// phrase box will resave correctly.
				CSourcePhrase* pSrcPhrase = NULL;
				// remember, the layout is invalid and not yet recalculated, so we can't use pile pointers
				// in the adjustments being made below
				if (m_nActiveSequNum > m_endIndex)
				{
					// We must calculate a new save box location - try put it near the end of the bundle.
					// Clear the box if it currently exists.
					if (m_pTargetBox->GetHandle() != NULL)
					{
						m_pTargetBox->m_bAbandonable = TRUE;
						m_pTargetBox->SetValue(_T(""));
					}
					// Empty the view's m_targetPhrase member of any text currently in it
					m_targetPhrase.Empty();

					// Set an arbitrary active location -- there is no way to be smart about doing this
					// (just allow a little context to appear to its right)
					m_nActiveSequNum = m_endIndex - m_nFollowingContext - 1;
					if (m_nActiveSequNum < 0) 
						m_nActiveSequNum = 0;

					// GetSavePhraseBoxLocationUsingList calculates a safe location, sets the view's
					// m_nActiveSequNumber member to that value, and calculates and sets m_targetPhrase
					// to agree with what will be the new phrase box location; we could ignore the
					// return value, but we won't.
					int nNewSequNum;
					nNewSequNum = GetSafePhraseBoxLocationUsingList(pView);
				}
				else
				{
					// the updated active location is still within the adjusted bundle, so just use
					// that value
					m_targetPhrase = strSavePhraseBox; // restore the phrase box contents
					pSrcPhrase = pView->GetSrcPhrase(m_nActiveSequNum);
				}
				wxASSERT(pSrcPhrase);

				// remove the KB or GlossingKB entry for this location, depending on the mode
				// and make the box contents resaveable
				CRefString* pRefString;
				if (gbIsGlossing)
				{
					pRefString = pView->GetRefString(pView->GetKB(),1,pSrcPhrase->m_key,pSrcPhrase->m_gloss);
					pView->RemoveRefString(pRefString,pSrcPhrase,1); // pRefString is from glossing KB
				}
				else
				{
					pRefString = pView->GetRefString(pView->GetKB(),pSrcPhrase->m_nSrcWords,
												pSrcPhrase->m_key,pSrcPhrase->m_adaption);
					pView->RemoveRefString(pRefString,pSrcPhrase,pSrcPhrase->m_nSrcWords);
				}
				// Note: RedrawEverything() is called at end of OnEditPreferences() - the latter doesn't
				// change the indices, so we must exit here with the indices we want to have used when
				// the view is set up again.
			}
		}
	}
	// can use this return value to signal the caller if RetokenizeText() has a problem
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     an int value representing the active sequence number of a safe phrase box location
/// \param      pView   -> pointer to the View
/// \remarks
/// Called from: the App's DoPunctuationChanges() and the Doc's ReconstituteAfterFilteringChange().
/// Gets a safe location for placing the phrase box. It avoids placing the phrase box in a
/// retranslation (unless that is the only possible place), and prepares for a bundle change if
/// the safe location is outside the current bundle. 
////////////////////////////////////////////////////////////////////////////////////////////
int CAdapt_ItApp::GetSafePhraseBoxLocationUsingList(CAdapt_ItView* pView)
{
	// for GetSafePhraseBoxLocationUsingList() to work correctly, the view's m_nActiveSequNum value
	// must be set to a valid list location before the function is called; and likewise for the
	// view's m_endIndex value
	// 
	// get the sourcephrase which is at the potential active location
	CSourcePhrase*  pSrcPhrase = pView->GetSrcPhrase(m_nActiveSequNum);

	// this location could be in a retranslation, so adjust the sequence number
	// and its sourcephrase if that is the case
	CSourcePhrase* pSaveSrcPhrase = pSrcPhrase;
	if (pSrcPhrase->m_bRetranslation)
	{
		// its a retranslation location, so move active location to an earlier sequence
		// number until we find a sourcePhrase which is not in the retranslation
		int nPrevSequNum = m_nActiveSequNum - 1;
		CSourcePhrase* pNextSrcPhrase = NULL;
		if (nPrevSequNum < 0)
		{
			// we are at the start of the document and still within a
			// retranslation, so search forward instead
a:			pNextSrcPhrase = pSaveSrcPhrase;
			int nNextSequNum = m_nActiveSequNum;
			while (pNextSrcPhrase->m_bRetranslation) 
			{
				++nNextSequNum;
				if (nNextSequNum > m_maxIndex)
				{
					// there is nowhere where there is no retranslation
					// so go to the start & just too bad - put the box
					// in a retranslation!!!! It's naughty, but the app
					// will not die.
					{
						nNextSequNum = 0;
						pView->CalcInitialIndices();
						pNextSrcPhrase = pView->GetSrcPhrase(nNextSequNum);
						break;
					}
				}
				pNextSrcPhrase = pView->GetSrcPhrase(nNextSequNum);
			} // loop end
			pSrcPhrase = pNextSrcPhrase;
			m_nActiveSequNum = pNextSrcPhrase->m_nSequNumber;
		}
		else
		{
			// not yet at the doc start, so we can search back further
			CSourcePhrase* pPrevSrcPhrase = pView->GetSrcPhrase(nPrevSequNum);
			while (pPrevSrcPhrase->m_bRetranslation) 
			{
				--nPrevSequNum;
				if (nPrevSequNum < 0)
				{
					goto a;
				}
				pPrevSrcPhrase = pView->GetSrcPhrase(nPrevSequNum);
			} // loop end
			pSrcPhrase = pPrevSrcPhrase;
			m_nActiveSequNum = pPrevSrcPhrase->m_nSequNumber;
		}
	}

	// all of the above fiddling might have given us a final sourcephrase location
	// which has gone outside the bundle's current bounds. If this is the case,
	// then adjust the bundle to agree with the location we've chosen - that is,
	// adjust the indices so the active location falls within them; but don't do
	// so if it is already within them
	if (m_nActiveSequNum < m_beginIndex ||
		m_nActiveSequNum > m_endIndex)
	{
		// adjustment is needed
		pView->CalcIndicesForAdvance(m_nActiveSequNum);
	}

	// set m_targetPhrase
	if (gbIsGlossing)
		m_targetPhrase = pSrcPhrase->m_gloss;
	else
		m_targetPhrase = pSrcPhrase->m_adaption;

	return m_nActiveSequNum;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     always TRUE
/// \param      pfilterPageCommon   -> pointer to the appropriate filter page being displayed
/// \param      reparseDoc          -> enum value that can be either NoReparse or DoReparse
/// \remarks
/// Called from: the CFilterPageWiz::OnWizardPageChanging() when moving forward, and the 
/// CFilterPagePrefs::OnOK().
/// Updates the internal data structures for any changes made in the filter page (or in
/// the usfm page). It first checks for filtering changes made to the markers currently being 
/// used in the Doc. If filtering changes are made in the doc and/or the parameter 
/// reparseDoc == DoReparse, DoUSFMFilterChanges calls RetokenizeText() with the appropriate 
/// flags. Next, DoUSFMFilterChanges checks for filtering changes made to the list of markers 
/// stored in the Project (config file), and if changes were made, stores the updated 
/// filtering markers list in the App's gProjectFilterMarkersForConfig for later writing to 
/// the project config file.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::DoUsfmFilterChanges(CFilterPageCommon* pfilterPageCommon, enum Reparse reparseDoc)
{
	// whm added 26Apr05
	CAdapt_ItDoc* pDoc = GetDocument();
	CAdapt_ItView* pView = gpApp->GetView();

	bool bFilterChangeInDoc = FALSE;
	bool bFilterChangeInProj = FALSE;
	wxString tempStr;
	wxString filterMkr;
	wxString strMarkersToBeUnfiltered;
	wxString strMarkersToBeFiltered;
	strMarkersToBeUnfiltered.Empty();
	strMarkersToBeFiltered.Empty();

	// added for Bruce 1Apr05
	gpApp->m_FilterStatusMap.empty(); // start with empty map

	// since the CUIntArrays contain all the document's markers, and since the user cannot
	// add markers in the GUI which calls this function, the sizes must be identical
	int countBeforeEdit = pfilterPageCommon->m_filterFlagsDocBeforeEdit.GetCount();
	int countAfterEdit = pfilterPageCommon->m_filterFlagsDoc.GetCount();
	wxASSERT(countBeforeEdit == countAfterEdit);
	int numFlags = pfilterPageCommon->m_filterFlagsDoc.GetCount();
	// The filter page's m_SfmMarkerAndDescriptionsDoc wxStringArray and the parallel 
	// m_filterFlagsDoc CUIntArray contain data for both known and unknown markers.
	// The former contains the list box formatted whole marker and its description. The later
	// contains flags indicating the filter state of the associated/parallel markers.
	// Below we do the following filter marker housekeeping and data processes:
	// 1. Check for changes in the filter state of markers (both known and unknown markers)
	// 2. For markers whose filtering state has changed, we do the following:
	//    a. build the pDoc->m_FilterStatusMap which Bruce uses in the doc rebuild code.
	//    b. build the strMarkersToBeUnfiltered and strMarkersToBeFiltered strings for
	//       ResetUSFMFilterStructs() - DoUsfmFilterChanges now uses Bruce's version of
	//       ResetUSFMFilterStructs which handles the reset of USFM filter structs correctly 
	//       whereas the original version did not handle unfiltering correctly here in 
	//       DoUsfmFilterChanges.
	for (int index = 0; index < numFlags;  index++)
	{
		if (pfilterPageCommon->m_filterFlagsDoc[index] != pfilterPageCommon->m_filterFlagsDocBeforeEdit[index])
		{
			bFilterChangeInDoc = TRUE;
			// while we are at it, collect the markers that need changing into a string
			// that can be input into the Doc's ResetUSFMFilterStructs().

			tempStr = pfilterPageCommon->m_SfmMarkerAndDescriptionsDoc[index];
			int strLen = tempStr.Length();
			int spCt = 0;
			int nwspCt = 0;
			// parse white space at begtinning of string
			while(spCt < strLen && tempStr.GetChar(spCt) == _T(' '))
			{
				spCt++;
			}
			// remove prefixed white space
			tempStr = tempStr.Mid(spCt);
			filterMkr.Empty();
			// parse the whole marker
			while(nwspCt < strLen && tempStr.GetChar(nwspCt) != _T(' '))
			{
				filterMkr += tempStr.GetChar(nwspCt);
				nwspCt++;
			}

			filterMkr.Trim(TRUE); // trim right end
			filterMkr.Trim(FALSE); // trim left end
			
			wxString valStr;
			if (gpApp->m_FilterStatusMap.find(filterMkr) == gpApp->m_FilterStatusMap.end())
			{
				// marker does not already exist in m_FilterStatusMap so add it
				if (pfilterPageCommon->m_filterFlagsDocBeforeEdit[index] == 0	// previously unfiltered
					&& pfilterPageCommon->m_filterFlagsDoc[index] == 1)			// now filtered
				{
					gpApp->m_FilterStatusMap[filterMkr] = _T("1");
					strMarkersToBeFiltered += filterMkr + _T(' ');
				}
				else if (pfilterPageCommon->m_filterFlagsDocBeforeEdit[index] == 1	// previously filtered
					&& pfilterPageCommon->m_filterFlagsDoc[index] == 0)				// now unfiltered
				{
					gpApp->m_FilterStatusMap[filterMkr] = _T("0");
					strMarkersToBeUnfiltered += filterMkr + _T(' ');
				}
			}
		}
	}

	// whm added 12May06
	// Mark the doc as "dirty" so the save toolbar button will be active to allow the user to
	// save changes to the doc, and if he immediately exits the program it will prompt to save
	// those changes. This should go here rather than within the if (bFilterChangeInDoc) block
	// below, because under certain circumstances the doc can change without bFilterChangeInDoc
	// changing to TRUE.
	pDoc->Modify(TRUE);

	if (bFilterChangeInDoc)
	{
		// Any filter change in the Doc requires updating the current USFM and Filtering data
		// structures.

		// Update the active set of USFMAnalysis structs. 
		// ResetUSFMFilterStructs calls the App's SetupMarkerStrings which updates gCurrentFilterMarkers
		// Use Bruce's version of ResetUSFMFilterStructs
		pDoc->ResetUSFMFilterStructs(gpApp->gCurrentSfmSet, strMarkersToBeFiltered, strMarkersToBeUnfiltered);

		// When there is a set change, the Doc's m_filterMarkersAfterEdit and the App's 
		// gCurrentFilterMarkers should have been set to the same string value in DoUsfmSetChanges, 
		// however, when there was no set change, but was a filtering change the Doc's m_filterMarkersAfterEdit
		// needs to be updated
		gpApp->m_filterMarkersAfterEdit = gCurrentFilterMarkers;

		if (reparseDoc == DoReparse)
		{
			// Reparse the Document
			// code below copied and adapted from CAdapt_ItView::OnEditPunctCorresp()
			int activeSequNum;
			if (gpApp->m_nActiveSequNum < 0)
			{
				// must not have data yet, or we are at EOF and so no pile is currently active
				activeSequNum = -1;
			}
			else
			{
				// we are somewhere in the midst of the data, so a pile will be active
				activeSequNum = gpApp->m_pActivePile->m_pSrcPhrase->m_nSequNumber;
				gpApp->m_curIndex = activeSequNum;

				// remove any current selection, as we can't be sure of any pointers
				// depending on what user may choose to alter
				pView->RemoveSelection();
			}

			// as in DoPunctuationChanges, we need to be sure we can recreate the phrase box safely
			// so changes will be needed here; however, this is done at a lower level within
			// RetokenizeText(), and typically the box location is preserved, so it makes sense to retain
			// its contents for redisplay when unfiltering & filtering has completed -- however, if the
			// phrase box was located within a stretch which got filtered out, then it's location will be
			// changed to somewhere close by (the app tries to put it at the first safe location following
			// where its earlier location was destroyed by the filtering) - and restoring the old contents
			// will in that case typically be wrong. We'll do it nevertheless, since the user will immediately
			// see it and can manually edit the contents to what they should be before continuing to adapt.
			wxString strSavePhraseBox = gpApp->m_targetPhrase;

			// now do the reparse
			int nNewSrcPhraseCount = pDoc->RetokenizeText(FALSE, // FALSE = punctuation not changed here
															bFilterChangeInDoc, 
															FALSE); // FALSE = sfm set change not flagged here
			gpApp->m_maxIndex = nNewSrcPhraseCount - 1; // update (this is cosmetic, actually we have to have
														// updated this index progressively as we retokenized)
			gpApp->m_targetPhrase = strSavePhraseBox;

			// Note: RedrawEverything() is called at end of OnEditPreferences()
		}
		else
		{
			// No Reparse the Document called for
		}
	}

	// Now check for changes in the Project's list of filter markers
	wxString filterMkrStr;
	filterMkrStr.Empty(); // start with an empty string
	countBeforeEdit = pfilterPageCommon->m_filterFlagsProjBeforeEdit.GetCount();
	countAfterEdit = pfilterPageCommon->m_filterFlagsProj.GetCount();
	wxASSERT(countBeforeEdit == countAfterEdit);
	numFlags = pfilterPageCommon->m_filterFlagsProj.GetCount();
	for (int index = 0; index < numFlags;  index++)
	{
		// Collect the markers to store as new filter marker defaults for the Project
		// in case user made a change.
		int posn = pfilterPageCommon->m_SfmMarkerAndDescriptionsProj[index].Find(_T("  "));
		filterMkr = pfilterPageCommon->m_SfmMarkerAndDescriptionsProj[index].Mid(0,posn);
		filterMkr.Trim(TRUE); // trim right end
		filterMkr.Trim(FALSE); // trim left end
		if (pfilterPageCommon->m_filterFlagsProj[index] == TRUE)
		{
		filterMkrStr += filterMkr + _T(' ');
		}
		// check for changes
		if (pfilterPageCommon->m_filterFlagsProj[index] != pfilterPageCommon->m_filterFlagsProjBeforeEdit[index])
		{
			bFilterChangeInProj = TRUE;
		}
	}

	if (bFilterChangeInProj)
	{
		// Filtering changes in the Project do not require adjustments to any of the
		// document's current data structures. Project changes only require storing the 
		// new list of filter marker strings in the App's gProjectFilterMarkersForConfig
		// for later saving in the project config file.
		// Note: Here is the only place where gProjectFilterMarkersForConfig is set. 
		// The filter markers saved in the project config file are only set from the 
		// Filtering tab/wizard page.
		gpApp->gProjectFilterMarkersForConfig  = filterMkrStr;
	}

	// can use this return value to signal the caller if RetokenizeText() has a problem
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// \return     always TRUE
/// \param      pUsfmPageCommon   -> pointer to the appropriate usfm page
/// \param      pFilterPageCommon -> pointer to the appropriate filter page
/// \param      bSetChanged       <- TRUE by reference if the sfm set changed, FALSE otherwise
/// \param      reparseDoc        -> enum value that can be either NoReparse or DoReparse
/// \remarks
/// Called from: the CUSFMPageWiz::OnWizardPageChanging() when moving forward, and the 
/// CUSFMPagePrefs::OnOK().
/// It updates the internal data structures for any changes made to the sfm set in the 
/// CUSFMPage. It calls RetokenizeText with reparseDoc == DoReparse if the user changed 
/// the SfmSet in the document.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::DoUsfmSetChanges(CUSFMPageCommon* pUsfmPageCommon, CFilterPageCommon* pFilterPageCommon, bool& bSetChanged, enum Reparse reparseDoc)
{
	// whm added 23May05
	CAdapt_ItDoc* pDoc = GetDocument();
	CAdapt_ItView* pView = gpApp->GetView();

	wxString filterMkr;
	wxString filterMkrStr = _T("");

	// added for Bruce 1Apr05
	gpApp->m_FilterStatusMap.clear(); // start with empty map

	// whm Note 29Jun05: DoUsfmSetChanges() is always called when the user clicks on OK in Preferences,
	// and/or clicks FINISH in the wizard. As long as the local copies of the following variables are 
	// properly initialized in the USFM page and Filter page's constructors, we can unconditionally
	// copy those local copies back to their corresponding variables on the App and Doc when the user
	// clicks on OK in the Preferences, and/or clicks FINISH in the wizard. These variables are:
	// 1. gpApp->gCurrentSfmSet (enum SfmSet) [always represents the current sfm set of the active document]
	// 2. gpApp->gCurrentFilterMarkers (wxString) [always represents the filter markers of the active document]
	// 3. gpApp->gProjectSfmSetForConfig (enum SfmSet) [always has the current value for storing in proj config]
	// 4. gpApp->gProjectFilterMarkersForConfig (wxString) [always has the current value for storing in proj config]
	// 5. gpApp->m_sfmSetAfterEdit (enum SfmSet)
	// 6. gpApp->m_filterMarkersAfterEdit (wxString)
	// 7. gpApp->m_unknownMarkers (CStringArray)
	// 8. gpApp->m_filterFlagsUnkMkrs (CUIntArray)
	// 9. gpApp->m_currentUnknownMarkersStr (wxString)
	// When checking code integrity, the above should be used to initialize the corresponding temporary
	// local variables used in the USFM and Filter pages. 
	// Only the temporary local variables should be modified while the Preferences and/or Wizard are
	// active. Only when the user clicks OK in Preferences or FINISH in the wizard, should the temporary
	// local variables be used to update the above global variables on the App.

	// Update the sfm set stored in gCurrentSfmSet on the App (gCurrentSfmSet always reflects the 
	// state of the current Doc) with the USFM page's tempSfmSetAfterEditDoc
	gpApp->gCurrentSfmSet = pUsfmPageCommon->tempSfmSetAfterEditDoc;
	// Update the App's gCurrentFilterMarkers with the Filter page's tempFilterMarkersAfterEditDoc
	gpApp->gCurrentFilterMarkers = pFilterPageCommon->tempFilterMarkersAfterEditDoc;
	// Update the global for the project sfm set with the USFM page's tempSfmSetAfterEditProj
	gpApp->gProjectSfmSetForConfig = pUsfmPageCommon->tempSfmSetAfterEditProj;
	// Update the global for the project's filter markers with the Filter page's tempFilterMarkersAfterEditProj
	gpApp->gProjectFilterMarkersForConfig = pFilterPageCommon->tempFilterMarkersAfterEditProj;
	// Update the sfm set stored on the App with the USFM page's tempSfmSetAfterEditDoc
	gpApp->m_sfmSetAfterEdit = pUsfmPageCommon->tempSfmSetAfterEditDoc; // whm added 10Jun05 for Bruce
	// Update the list of filter markers on the App with the Filter page's tempFilterMarkersAfterEditDoc
	gpApp->m_filterMarkersAfterEdit = pFilterPageCommon->tempFilterMarkersAfterEditDoc;

#ifdef _Trace_UnknownMarkers
	TRACE0("In DoUsfmSetChanges BEFORE Doc's unknown markers copied from pFilterPageCommon\n");
	TRACE1("   Doc's unknown markers = %s\n", pDoc->GetUnknownMarkerStrFromArrays(&pDoc->m_unknownMarkers, &pDoc->m_filterFlagsUnkMkrs));
	TRACE1("    Filter pg's unk mkrs = %s\n", pDoc->GetUnknownMarkerStrFromArrays(&pFilterPageCommon->m_unknownMarkers, &pFilterPageCommon->m_filterFlagsUnkMkrs));
#endif

	// Update the unknown markers vars on the App with those in the Filter page
	gpApp->m_unknownMarkers.Clear();
	int ct;
	for (ct = 0; ct < (int)pFilterPageCommon->m_unknownMarkers.GetCount(); ct++)
		gpApp->m_unknownMarkers.Add(pFilterPageCommon->m_unknownMarkers.Item(ct));

	gpApp->m_filterFlagsUnkMkrs.Clear();
	for (ct = 0; ct < (int)pFilterPageCommon->m_filterFlagsUnkMkrs.GetCount(); ct++)
		gpApp->m_filterFlagsUnkMkrs.Add(pFilterPageCommon->m_filterFlagsUnkMkrs.Item(ct));
	gpApp->m_currentUnknownMarkersStr = pFilterPageCommon->m_currentUnknownMarkersStr;

	if (pUsfmPageCommon->tempSfmSetBeforeEditDoc != pUsfmPageCommon->tempSfmSetAfterEditDoc)
	{
		bSetChanged = TRUE; // return TRUE so that filtering call can be suppressed in caller
							// (caller must default this flag to FALSE before DoUsfmSetChanges is called)
	}

	if (pUsfmPageCommon->bChangeFixedSpaceToRegularSpace != pUsfmPageCommon->bChangeFixedSpaceToRegularBeforeEdit)
	{
		gpApp->m_bChangeFixedSpaceToRegularSpace = pUsfmPageCommon->bChangeFixedSpaceToRegularSpace;
	}

	// whm added 12May06
	// Mark the doc as "dirty" so the save toolbar button will be active to allow the user to
	// save changes to the doc, and if he immediately exits the program it will prompt to save
	// those changes. This should go here rather than within the if (bSetChanged) block
	// below, because under certain circumstances the doc can change without bSetChanged
	// changing to TRUE.
	pDoc->Modify(TRUE);

	if (bSetChanged)
	{

#ifdef _Trace_FilterMarkers
		TRACE0("In DoUsfmSetChanges BEFORE SetupMarkerStrings call:\n");
		TRACE1("   App's gCurrentSfmSet = %d\n",gpApp->gCurrentSfmSet);
		TRACE1("   App's gCurrentFilterMarkers = %s\n",gpApp->gCurrentFilterMarkers);
		TRACE1("   Doc's m_sfmSetBeforeEdit = %d\n",pDoc->m_sfmSetBeforeEdit);
		TRACE1("   Doc's m_filterMarkersBeforeEdit = %s\n",pDoc->m_filterMarkersBeforeEdit);
#endif

		// Sfm set change requires updating the rapid access data strings 
		gpApp->SetupMarkerStrings();

#ifdef _Trace_FilterMarkers
		TRACE0("In DoUsfmSetChanges AFTER SetupMarkerStrings call:\n");
		TRACE1("   App's gCurrentSfmSet = %d\n",gpApp->gCurrentSfmSet);
		TRACE1("   App's gCurrentFilterMarkers = %s\n",gpApp->gCurrentFilterMarkers);
		TRACE1("   Doc's m_sfmSetBeforeEdit = %d\n",pDoc->m_sfmSetBeforeEdit);
		TRACE1("   Doc's m_filterMarkersBeforeEdit = %s\n",pDoc->m_filterMarkersBeforeEdit);

#endif
		if (reparseDoc == DoReparse)
		{

#ifdef _Trace_FilterMarkers
			TRACE0("In DoUsfmSetChanges reparse == DoReparse so calling RetokenizeText next\n");
#endif

			// Reparse the Document
			// code below copied and adapted from CAdapt_ItView::OnEditPunctCorresp()
			int activeSequNum;
			if (gpApp->m_nActiveSequNum < 0)
			{
				// must not have data yet, or we are at EOF and so no pile is currently active
				activeSequNum = -1;
			}
			else
			{
				// we are somewhere in the midst of the data, so a pile will be active
				activeSequNum = gpApp->m_pActivePile->m_pSrcPhrase->m_nSequNumber;
				gpApp->m_curIndex = activeSequNum;

				// remove any current selection, as we can't be sure of any pointers
				// depending on what user may choose to alter
				pView->RemoveSelection();
			}
			wxString strSavePhraseBox = gpApp->m_targetPhrase;
			// now do the reparse
			int nNewSrcPhraseCount = pDoc->RetokenizeText(
										FALSE,	// FALSE = punctuation not changed here
										FALSE,  // because any filtering changes are done as a side-effect
										bSetChanged); 

#ifdef _Trace_FilterMarkers
			TRACE0("In DoUsfmSetChanges AFTER RetokenizeText call:\n");
			TRACE1("   App's gCurrentSfmSet = %d\n",gpApp->gCurrentSfmSet);
			TRACE1("   App's gCurrentFilterMarkers = %s\n",gpApp->gCurrentFilterMarkers);
			TRACE1("   Doc's m_sfmSetBeforeEdit = %d\n",pDoc->m_sfmSetBeforeEdit);
			TRACE1("   Doc's m_filterMarkersBeforeEdit = %s\n",pDoc->m_filterMarkersBeforeEdit);
#endif

			gpApp->m_maxIndex = nNewSrcPhraseCount - 1; // update

			// version 3 always rebuilds the doc, so we need to unconditionally restore the phrasebox contents...
			// (The box contents will generally be correct and the location unchanged, except if the SFM set
			// change involved a filtering of content in which the phrase box happened to be located - in which
			// case the phrase box will be relocated automatically at some nearby later safe location, but the
			// contents will no longer be appropriate when restored - but the user will see that and can edit it
			// immediately, so we won't do anything smarter here.
			gpApp->m_targetPhrase = strSavePhraseBox;
				
			// Note: RedrawEverything() is called at end of OnEditPreferences()
		}
		else
		{
			// No Reparse of the Document called for
		}
	}

	// can use this return value to signal the caller if RetokenizeText() has a problem
	return TRUE;
}

// whm Note: This macro is from Bob E
//	#define MAKE_DWORD(hw,lw) ((wxUint32) (((unsigned long)(hw)<<16) | ((unsigned long)(lw))) )

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pAdView   -> pointer to the View (unused)
/// \remarks
/// Called from: the Doc's OnNewDocument(), OnOpenDocument(), the View's OnInitialUpdate(), 
/// CFontPageWiz::OnWizardPageChanging() when moving forward, and from CFontPagePrefs::OnOK().
/// Gets the text heights for the source font (m_nSrcHeight), the target font (m_nTgtHeight)
/// and the navigation text font (m_nNavTextHeight). Each font is selected into the display
/// context and the GetCharHeight() method of the display context is called to determine the
/// height of each of the three fonts.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::UpdateTextHeights(CAdapt_ItView* WXUNUSED(pAdView))
{
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);
	// need a CDC
	wxClientDC dC(pApp->GetMainFrame()->canvas);
	wxASSERT(m_pSourceFont != NULL);
	wxFont SaveFont = dC.GetFont();
	dC.SetFont(*m_pSourceFont);

	m_nSrcHeight = dC.GetCharHeight();

	// whm note: The following is Bob Eaton's modification to detect if the font is a symbol
	// font, and if so, use a different "encoding" string in Adapt It's xml files.
	// Problem: m_nCodePage in the MFC version is a 32 bit int, whereas within the wxWidgets
	// font encoding representation they are represented as an enum, which has a maximum of
	// 255 different values. Bob's GetEncodingStringForXmlFiles() function has a 
	// switch(m_nCodePage) statement which has 141 cases in it, but the values in the
	// switch/case range from 42 to 65006. The values above 255 are not legal for an enum, so
	// we can't map directly from MFC's code page values to wxWidget's wxFontEncoding enum 
	// values. See the InitializeFonts() function for more info on wxWidgets and MFC font
	// encoding matters. See also the following functions: 
	//    GetEncodingStringForXmlFiles() [Bob Eaton's function for Windows]
	//    MapMFCCharsetToWXFontEncoding()
	//    MapWXFontEncodingToMFCCharset()
	//
	

	dC.SetFont(*m_pTargetFont);
	m_nTgtHeight = dC.GetCharHeight();

	// TODO: The wxDC class does not use a separate TEXTMETRIC struct. Within
	// the wxDC class there appears to be no way to access an internal leading
	// value. And, the external leading value seems to be only available when 
	// measuring an actual string of text using GetTextExtent(). 
#ifdef _RTL_FLAGS // still the Unicode version, but using CEdit for the base class for the phrase box
	// box height needs to include external and internal leading for CRichEditCtrl, else text is 
	// cut off at the bottom by the bottom of the box; see globals for explanation of 
	// gnVerticalBoxBloat
	gnVerticalBoxBloat = 0; //metrics.tmInternalLeading + metrics.tmExternalLeading;
#else
	gnVerticalBoxBloat = 0;
#endif // for _RTL_FLAGS

	// temporary - for use in fine tuning display code
	dC.SetFont(*m_pNavTextFont); // now handle the navigation text's font
	m_nNavTextHeight = dC.GetCharHeight();
	dC.SetFont(SaveFont); // restore original font
}

// MFC DoFontChanges signature
//bool CAdapt_ItApp::DoFontChanges(CAdapt_ItApp* pApp, CFontPage *pFPage, 
//						  wxFont *pSourceFont, wxFont *pTargetFont, wxFont* pNavTextFont)
// wx version note: DoFontChanges is no longer use in the wxWidgets version which
// does not delete and recreate the source, target and navigation fonts once they
// are initially created on the app.

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     FALSE if the store operation failed, TRUE if it succeeded
/// \remarks
/// Called from: the App's SetupDirectories(), SubstituteKBBackup(), the View's OnCreate(),
/// and CProjectPage::OnWizardPageChanging() when moving forward.
/// Reads the data from an existing xml glossing KB file if present. If the xml glossing KB
/// file was not found (or could not be read), it creates a new (empty) KB for glossing.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::LoadGlossingKB()
{
	bool bSaveIsGlossingFlag = gbIsGlossing;

	// set the gbIsGlossing flag to TRUE, as the XML needs to use it (it may have a different
	// value in the caller, so we restore it before returning)
	gbIsGlossing = TRUE;

	// first make sure the paths and filenames match are consistent with the m_gSaveAsXML
	// flag's value of TRUE (ie. set them to .xml extension, and the alternate filename and
	// path will have the .KB extension); or with FALSE (and the current and alternate are
	// the opposite)
	CAdapt_ItDoc* pDoc = GetDocument();
	pDoc->UpdateFilenamesAndPaths(FALSE,FALSE,FALSE,TRUE,TRUE);

	// some temporaries for the code below
	bool bReadOK = FALSE; // whm added initialization
	bool bSaveFlag = m_bSaveAsXML; // preserve it, we may have to temporarily change the value
	wxString path = m_curGlossingKBPath;

	if (m_bSaveAsXML) // always true in wx version
	{
		// version 3.0 and upwards, the default is to do i/o in XML
		if (wxFileExists(path))
		{
			// the expected *.xml glossing knowledge base file is in the project folder

			// attempt the load
			bReadOK = ReadKB_XML(path, m_pGlossingKB);
		}
		/* No binary serialization in the WX app
		else 
		{
			path = m_altGlossingKBPath;
			if (f.GetStatus(path,status) && 
				(status.m_attribute == CFile::normal || status.m_attribute == CFile::archive))
			{
				// the expected *.xml glossing knowledge base file is not in the project folder, but there
				// is the equivalent *.KB binary one there - so we must use that instead

				// we have to change the flag temporarily to FALSE so that the binary one
				// will be opened
				m_bSaveAsXML = FALSE;
				bOpened = f.Open(path, CFile::modeRead | CFile::shareExclusive,&e);
				if (bOpened)
				{
					goto g; // proceed with the serializing in of the binary file instead
				}
				else
				{
					// we need to make a stub KB, but we'll do the expected .xml one
					m_bSaveAsXML = TRUE; // restore it
					goto c;
				}
			}
			else
			{
				// neither is on disk, so we need an empty one created
				goto c;
			}
		}
		*/

		// if there was a bad load...
		if (!bReadOK)
		{
			// IDS_NO_GLOSSINGKB_WARNING
			wxMessageBox(_("Warning: a knowledge base for storing glosses was not found. An empty one has been created for your use instead."), _T(""), wxICON_INFORMATION);

			// make the substitute KB in memory
			if (m_pGlossingKB == NULL)
				m_pGlossingKB = new CKB;

			// store it on disk & close it
			bool bOK = StoreGlossingKB(FALSE); // first time, so we can't make a backup

			// restore the saved flag values to what they were in the caller
			gbIsGlossing = bSaveIsGlossingFlag;
			m_bSaveAsXML = bSaveFlag;

			if (!bOK)
			{
				return FALSE; // let the caller put up message and abort
			}

			// restore the gbIsGlossing flag value to what it was in the caller
			gbIsGlossing = bSaveIsGlossingFlag;
			return TRUE; // nothing much in it, so just return saying all is well
		}
	}

	// restore the gbIsGlossing flag value to what it was in the caller
	gbIsGlossing = bSaveIsGlossingFlag;
	m_bSaveAsXML = bSaveFlag;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     FALSE if the store operation failed, TRUE if it succeeded
/// \remarks
/// Called from: the App's SetupDirectories(), SubstituteKBBackup(), the View's OnCreate(),
/// and CProjectPage::OnWizardPageChanging() when moving forward.
/// Reads the data from an existing xml adapting KB file if present. If the xml KB
/// file was not found (or could not be read), it creates a new (empty) adapting KB.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::LoadKB()
{
	bool bSaveIsGlossingFlag = gbIsGlossing;

	// set the gbIsGlossing flag to FALSE, as the XML needs to use it (it may have a different
	// value in the caller, so we restore it before returning)
	gbIsGlossing = FALSE;

	// first make sure the paths and filenames match are consistent with the m_gSaveAsXML
	// flag's value of TRUE (ie. set them to .xml extension, and the alternate filename and
	// path will have the .KB extension); or with FALSE (and the current and alternate are
	// the opposite)
	CAdapt_ItDoc* pDoc = GetDocument();
	pDoc->UpdateFilenamesAndPaths(TRUE,TRUE,TRUE,FALSE,FALSE);

	// some temporaries for the code below
	bool bReadOK = FALSE; // whm added initialization
	//bool bOpened;
	bool bSaveFlag = m_bSaveAsXML; // preserve it, we may have to temporarily change the value
	wxString path = m_curKBPath;

	if (m_bSaveAsXML) // always true in the wx version
	{
		// version 3.0 and upwards, the default is to do i/o in XML

		if (wxFileExists(path))
		{
			// the expected *.xml knowledge base file is in the project folder
			// attempt the load
			bReadOK = ReadKB_XML(path, m_pKB);
		}
		/*
		// WX version cannot read the binary .KB file
		else
		{
			path = m_altKBPath;
			if (f.GetStatus(path,status) && 
				(status.m_attribute == CFile::normal || status.m_attribute == CFile::archive))
			{
				// the expected *.xml knowledge base file is not in the project folder, but there
				// is the equivalent *.KB binary one there - so we must use that instead

				// we have to change the flag temporarily to FALSE so that the binary one
				// will be opened
				m_bSaveAsXML = FALSE;
				bOpened = f.Open(path, CFile::modeRead | CFile::shareExclusive,&e);
				if (bOpened)
				{
					goto g; // proceed with the serializing in of the binary file instead
				}
				else
				{
					// we need to make a stub KB, but we'll do the expected .xml one
					m_bSaveAsXML = TRUE; // restore it
					goto c;
				}
			}
			else
			{
				// neither is on disk, so we need an empty one created
				goto c;
			}
		}
		*/

		// if there was a bad load...
		if (!bReadOK)
		{
			// IDS_NO_ADAPTINGKB_WARNING
			wxMessageBox(_("Warning: a knowledge base for storing adaptations was not found. An empty one has been created for your use instead. "), _T(""), wxICON_INFORMATION);

			// make the substitute KB in memory
			if (m_pKB == NULL)
				m_pKB = new CKB;

			// we'll have to make sure we get the right source and target language names, so we
			// must analyse the path name to extract them from there; the app's m_sourceName etc
			// can't be relied on here, because the user might have backtracked in the startup
			// wizard after having supplied different names.
			wxFileName pathName;
			wxString path, fname, ext;
			pathName.SplitPath(m_curProjectPath, &path, &fname, &ext);

			wxString name = fname; // this is the project folder name
			int index;
			// whm: For localization purposes the " to " and " adaptations" strings should not be 
			// translated, otherwise other localizations would not be able to handle the unpacking 
			// of files created on different localizations.
			index = name.Find(_T(" to ")); // find "to" between spaces
			m_sourceName = name.Left(index); // get the source name, can contain multiple words
			index += 4;
			name = name.Mid(index); // name has target name plus "adaptations"
			index = name.Find(_T(" adaptations"));
			m_targetName = name.Left(index);

			// store in the memory instance of the knowledge base
			m_pKB->m_sourceLanguageName = m_sourceName;
			m_pKB->m_targetLanguageName = m_targetName;

			// store it on disk & close it
			bool bOK = StoreKB(FALSE); // first time, so we can't make a backup

			// restore the saved flag values to what they were in the caller
			gbIsGlossing = bSaveIsGlossingFlag;
			m_bSaveAsXML = bSaveFlag;

			if (!bOK)
			{
				return FALSE; // let the caller put up message and abort
			}
			return TRUE; // nothing much in it, so just return saying all is well
		}
	}

	// set the language names from the KB's stored names, in case the user has been monkeying
	// with language names in the startup wizard and has backed up to open an existing project
	// without resetting the language names to what they should be
	m_sourceName = m_pKB->m_sourceLanguageName;
	m_targetName = m_pKB->m_targetLanguageName;

	// restore the gbIsGlossing flag value to what it was in the caller
	gbIsGlossing = bSaveIsGlossingFlag;
	m_bSaveAsXML = bSaveFlag;
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if the title is a project folder name (i.e., has " adaptations" and " to "
///             in it) and is is not less than 18 characters, otherwise FALSE
/// \param      title   -> the incoming title string
/// \remarks
/// Called from: the App's GetPossibleAdaptionProjects().
/// Checks to see if the title string has the signs of being a project folder name.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::IsAdaptitProjectDirectory(wxString title)
{
	// return FALSE if the title is not one of ours, ie, not a project folder name
	// whm: For localization purposes the " to " and " adaptations" strings should not be 
	// translated, otherwise other localizations would not be able to handle the unpacking 
	// of files created on different localizations.
	if (title.Find(_T(" adaptations")) == -1)
		return FALSE;
	if (title.Find(_T(" to ")) == -1)
		return FALSE;
	if (title.Length() < 18)
		return FALSE;
	else
		return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pList   <- list of possible adaptation documents found on dirPath
/// \param      dirPath -> the path to search for adaptation documents
/// \remarks
/// Called from: the App's EnumerateDocFiles(), CDocPage::OnSetActive(), 
/// CJoinDialog::InitialiseLists(), CListDocumentsInOtherFolderDialog::InitDialog(),
/// CMoveDialog::UpdateFileList(), and CSplitDialog::ListFiles().
/// Fills the array string pList with the file names having .xml extension in dirPath.
/// Note: The caller should insure that dirPath exists before calling this function
/// since it does not have a graceful way to return if dirPath is bad. 
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::GetPossibleAdaptionDocuments(wxArrayString *pList, wxString dirPath)
{
	// whm - The caller should insure that dirPath exists before calling this function
	// since it does not have a graceful way to return if dirPath is bad. See CDocPage.
	wxDir finder;//CFileFind finder;

	bool bOK = (::wxSetWorkingDirectory(dirPath) && finder.Open(dirPath)); // wxDir must call .Open() before enumerating files!
	if (!bOK)
	{
		// think again!
		wxMessageBox(_(
			"Failed to set the current directory when getting existing adaptation documents"), _T(""),
			wxICON_ERROR);
		wxASSERT(FALSE);
		wxExit();
	}
	else
	{
		CAdapt_ItDoc* pDoc = GetDocument();
		wxASSERT(pDoc != NULL);
		wxDocTemplate* pTemplate = pDoc->GetDocumentTemplate(); //CDocTemplate* pTemplate = pDoc->GetDocTemplate();
		wxASSERT(pTemplate != NULL);
		wxString strExt;
		strExt = pTemplate->GetFileFilter(); // GetFileFilter returns "*.xml" in wx version
		// Must call wxDir::Open() before calling GetFirst() - see above
		wxString str = _T("");
		bool bWorking = finder.GetFirst(&str,wxEmptyString,wxDIR_FILES); 
		// whm note: wxDIR_FILES finds only files; it ignores directories, and . and ..
		while (bWorking)
		{
			if (str.IsEmpty())
				continue;
			wxString strEnd = str.Right(4); // extract its extension
			wxString strEndLower = strEnd;
			strEndLower = strEndLower.MakeLower();
			int nFound = str.Find(_T(".BAK"));
			if ((strEnd == strExt || strEndLower == strExt) && nFound == -1) 
				pList->Add(str); // add filename to the list, if it's extension is ours, but exclude backup document files with ".BAK.xml"
			bWorking = finder.GetNext(&str);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      dirPath   -> must be the absolute path to the current project's Adaptations 
///                         folder (this is stored in the app's m_curAdaptionsPath member)
/// \param      pFolders  -> the app's m_pBibleBooks member
/// \remarks
/// Called from: the App's SetupDirectories(), AccessOtherAdaptionProject(), and
/// OnAdvancedBookMode().
/// Creates the set of book folders specified in pFolders in the directory specified by
/// dirPath (which should be the app's m_curAdaptionsPath).
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::CreateBookFolders(wxString dirPath, wxArrayPtrVoid* pFolders)
// the passed in dirPath string must be the absolute path to the current project's
// Adaptations folder (this is stored in the app's m_curAdaptionsPath member);
// and pFolders should be the app's m_pBibleBooks member.
{
	wxString path;
	wxString folder;
	bool bExists;
	int count = pFolders->GetCount();
	for (int i = 0; i < count; i++)
	{
		folder = ((BookNamePair*)(*pFolders)[i])->dirName;
		path = dirPath + PathSeparator + folder;
		bExists = ::wxDirExists(path);
		if (bExists)
		{
			// this folder exists already, so continue
			continue;
		}
		else
		{
			// this folder does not exist yet, so create it
			bool bOK = ::wxMkdir(path); // NULL for default security settings
			// WX NOTE: On Unix/Linux wxMkdir has a second default param: int perm = 0777 which
			// makes a directory with full read, write, and execute permissions.
			if (!bOK)
			{
				// error, we'll just warn user, skip this folder, and carry on; user could manually try create it later
				wxString str;
				str = str.Format(_("Creating folder: \"%s\" failed. This folder will be skipped, processing will continue."),path.c_str());
				wxMessageBox(str, _T(""), wxICON_WARNING);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if the Adaptations folder contains a subdirectory name which is contained
//                 within, and of equal length to, the seeName name in one of structs in
//                 the book folders array
/// \param      dirPath   -> must be the absolute path to the current project's Adaptations 
///                         folder (this is stored in the app's m_curAdaptionsPath member)
/// \remarks
/// Called from: the App's SetupDirectories(), OnFileRestoreKb(), AccessOtherAdaptionProject(), 
/// and OnAdvancedBookMode().
/// Determines if the book folders have already been created or not in the dirPath.
/// Note: AreBookFoldersCreated() has the side effect of changing the current work directory
/// to the passed in dirPath.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::AreBookFoldersCreated(wxString dirPath)
// the passed in dirPath string must be the absolute path to the current project's
// Adaptations folder (this is stored in the app's m_curAdaptionsPath member)
// Returns TRUE if the Adaptations folder contains a subdirectory name which is contained
// within, and of equal length to, the seeName name in one of structs in
// the book folders array
// whm note: AreBookFoldersCreated() has the side effect of changing the current work directory
// to the passed in dirPath.
{
	wxDir finder;
	bool bOK = (::wxSetWorkingDirectory(dirPath) && finder.Open(dirPath)); // wxDir must call .Open() before enumerating files!
	if (!bOK)
	{
		wxMessageBox(_T(
			"Failed to set the current directory in AreBookFoldersCreated function"),
			_T(""), wxICON_ERROR);
		wxASSERT(FALSE);
		wxExit();	// TODO: See if there is a more graceful way to recover from
					// not being able to set the working directory to dirPath
		return FALSE; // to get rid of compiler warning - this line never executes
	}
	else
	{
		wxString str;
		// whm note: in GetFirst below, wxDIR_FILES | wxDIR_DIRS flag finds files or directories, but not . or .. or hidden files
		bool bWorking = finder.GetFirst(&str,wxEmptyString,wxDIR_FILES | wxDIR_DIRS);
		while (bWorking)
		{
			bWorking = finder.GetNext(&str);
			if (str.IsEmpty())
				continue;
			if (finder.Exists(str)) // wxDir::Exists(str) returns true if str is a directory that exists
			{
				// BEW changed 25Aug05, so that other user-defined folders can be
				// in the Adaptations folder without making the app confused
				// as to whether or not Bible Book folders are present or not

				// we have found a folder, check if it matches one of those in
				// the array of BookNamePair structs (using the seeName member)
				if (IsDirectoryWithin(str,m_pBibleBooks))
					return TRUE;
				else
					continue;
			}
			else
			{
				// its a file, so ignore it
				continue;
			}
		}
	}
	return FALSE;

}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if the folder represented by dir is one of the recognized book folders
///                 listed in pBooks
/// \param      dir    -> the folder name being checked
/// \param      pBooks -> the array/list of recognized book names
/// \remarks
/// Called from: the App's AreBookFoldersCreated(), OnFileRestoreKb(), 
/// AccessOtherAdaptionProject(), the View's OnRetransReport().
/// Determines is dir is one of the recognized book folder names.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::IsDirectoryWithin(wxString& dir, wxArrayPtrVoid*& pBooks)
{
	int nCount = pBooks->GetCount();
	int index;
	BookNamePair* pNP;
	for (index = 0; index < nCount; index++)
	{
		pNP = (BookNamePair*)pBooks->Item(index);
		int curPos = pNP->seeName.Find(dir);
		if (curPos >= 0)
		{
			// the passed in directory name is equal to, or a substring of
			// one of the bible book names
			int lenDir = dir.Length();
			int lenBookFolderName = pNP->seeName.Length();
			if (lenDir == lenBookFolderName)
			{
				// equal lengths, so assume that all the folders are present already
				return TRUE;
			}
			else
				continue;
		}
		else
			continue;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pList   <- an array string list that gets filled with project names.
/// \remarks
/// Called from: COpenExistingProjectDlg::InitDialog(), and CProjectPage::InitDialog().
/// Gets a list of possible Adapt It projects located on the m_workFolderPath.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::GetPossibleAdaptionProjects(wxArrayString *pList)
{
	wxString dirPath = m_workFolderPath;
	wxDir finder;

	bool bOK = ::wxSetWorkingDirectory(dirPath) && finder.Open(dirPath); // wxDir must call .Open() before enumerating files!
	if (!bOK)
	{
		// oops, this is a fatal error, we can't go on
		wxMessageBox(_T("Failed setting current directory in GetPossibleAdaptationProjects()"),_T(""),
						wxICON_ERROR);
		wxASSERT(FALSE);
		wxExit(); // this calls OnExit() before exiting  //AfxAbort();
	}
	else
	{
		wxString str;
		// by default, finder enumerates everything except '.' and '..'
		// using wxDIR_FILES | wxDIR_DIRS as flag finds files or directories, but not . or .. or hidden files
		bool bWorking = finder.GetFirst(&str,wxEmptyString,wxDIR_FILES | wxDIR_DIRS);
		while (bWorking)
		{
			// whm Note: The Exists() method of wxDIR used below returns TRUE if the passed name IS a directory.
			if (finder.Exists(str))
			{
				// exclude any directory which does not have a name of the form
				// "X to Y adaptations"
				bool bOurs = IsAdaptitProjectDirectory(str);
				if (bOurs)
				{
					pList->Add(str);
				}
			}
			bWorking = finder.GetNext(&str);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE unless a serious enough error occurs that the app cannot continue in which
///                 case it returns FALSE
/// \param      bAutoBackup   -> if TRUE it also saves a backup copy of the glossing KB
/// \remarks
/// Called from: the App's SetupDirectories(), LoadGlossingKB(), SaveGlossingKB(),
/// SubstituteKBBackup(), AccessOtherAdaptionProject(), the Doc's DoFileSave(), the View's
/// OnCreate().
/// Stores the glossing KB data in the external xml file located in the path represented
/// by m_curGlossingKBPath.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::StoreGlossingKB(bool bAutoBackup)
{
	// whm Note: This StoreGlossingKB() could easily be combined with the StoreKB()
	// routine below into a single function
	wxFile f; // create a CFile instance, using default constructor
	wxString path;

	// ensure the extension is what it should be
	gpApp->GetDocument()->UpdateFilenamesAndPaths(FALSE,FALSE,FALSE,TRUE,TRUE);
	
	
	// open a CFile stream
	path = m_curGlossingKBPath;
	
	if (!f.Open(m_curGlossingKBPath, wxFile::write))
	{
		wxString message;
		message = _("Error opening glossing KB file for writing with path:\n") 
			+ m_curGlossingKBPath + _("\nThe glossing knowledge base was not saved.\nIs your drive's free space low, or is the file open in another application?");
		wxMessageBox(message, _T(""), wxICON_INFORMATION);
		return FALSE;
	}

	if (gpApp->m_bSaveAsXML) // always true in the wx version
	{
		DoKBSaveAsXML(f,TRUE); // TRUE means that we want a glossing KB file in XML format

		// close the file
		f.Close();

		// remove the .KB file, if there is one of same name, so that the *.xml file
		// is the only form of the glossing kb left on disk
		if (m_altGlossingKBPath != path)
		{
			if (wxFileExists(m_altGlossingKBPath))
			{
				// there is a *.KB glossing knowledge base file on the disk, so delete it
				if (!::wxRemoveFile(m_altGlossingKBPath))
				{
					wxMessageBox(_T("Could not remove the *.KB glossing knowledge base alternate file"));
					// do nothing else, let the app continue
				}
			}
		}
	}
	// WX version doesn't use binary serialization
	//else
	//{
	//	// the file is now open, truncated to zero if necessary, and ready for writing, so
	//	// we must now attach it to a CArchive initialized for storing
	//	CArchive ar( &f, CArchive::store, 8192); // buffer size of 2*default should be enough

	//	// serialize the KB
	//	m_pGlossingKB->Serialize(ar);

	//	// flush the archive's buffer and close the archive
	//	ar.Close();

	//	// close file object to complete file's data transfer
	//	f.Close();

	//	// remove the .xml file, if there is one of same name, so that the *.KB file
	//	// is the only form of the glossing kb left on disk
	//	if (m_altGlossingKBPath != path)
	//	{
	//		if (f.GetStatus(m_altGlossingKBPath,status) && 
	//			(status.m_attribute == CFile::normal || status.m_attribute == CFile::archive))
	//		{
	//			// there is a *.xml glossing knowledge base file on the disk, so delete it
	//			try {
	//				f.Remove(m_altGlossingKBPath);
	//			}
	//			catch (CFileException* pe)
	//			{
	//				AfxMessageBox(_T("Could not remove the *.xml glossing knowledge base alternate file"));
	//				pe->Delete();
	//				// do nothing else, let the app continue
	//			}
	//		}
	//	}
	//}


	// if backing up is desired, we rename the newly saved copy as a *.BAK file, or *.BAK.xml
	// if m_bSaveAsXML is TRUE, then resave the glossing knowledge base to the old name again
	if (bAutoBackup)
	{
		if (::wxFileExists(m_curGlossingKBBackupPath) && !::wxDirExists(m_curGlossingKBBackupPath))
		{
			// found a backup, so remove it to make way for new backup
			if (!::wxRemoveFile(m_curGlossingKBBackupPath))
			{
				// notify user of error (maybe backup file is protected or in use???)
				wxMessageBox(_("Removing backup glossing kb file failed."), _T(""), wxICON_ERROR);
				return TRUE; // allow the app to continue
			}
		}


		// attempt to rename the file to the name with .BAK extension (for the binary file), or
		// we are saving as XML, rename to the name with .BAK.xml dual extensions -- the extension
		// will have been made correct by code which is above
		if (!::wxRenameFile(m_curGlossingKBPath,m_curGlossingKBBackupPath))
		{
			wxString message;
			message = _("Error renaming backup glossing KB file with path:\n") 
				+ m_curGlossingKBPath + _("\nBackup copy of glossing knowledge base was not created.");
			wxMessageBox(message, _T(""), wxICON_INFORMATION);
			return TRUE;// allow app to continue
		}

		
		if (!f.Open(m_curGlossingKBPath, wxFile::write))
		{
			wxString message;
			message = _("Error opening glossing KB file for writing with path:\n") 
				+ m_curGlossingKBPath + _("\nThe glossing knowledge base was not saved.\nIs your drive's free space low, or is the file open in another application?");
			wxMessageBox(message, _T(""), wxICON_INFORMATION);
				return FALSE;
		}

		if (gpApp->m_bSaveAsXML) // always true in the wx version
		{
			DoKBSaveAsXML(f,TRUE); // TRUE means we are doing it for a Glossing KB saved in XML format

			// close the file
			f.Close();

			// remove the .BAK file, if there is one of same name, so that the .BAK.xml file
			// is the only form of the KB backup file left on disk
			if (m_altGlossingKBBackupPath != m_curGlossingKBBackupPath)
			{
				if (wxFileExists(m_altGlossingKBBackupPath))
				{
					// there is an *.BAK backup knowledge base file on the disk, so delete it
					if (!::wxRemoveFile(m_altGlossingKBBackupPath))
					{
						wxMessageBox(_("Could not remove the *.BAK backup glossing knowledge base alternate file"),_T(""),wxICON_INFORMATION);
						// do nothing else, let the app continue
					}
				}
			}
		}
		// WX version doesn't do binary serialization
		//else
		//{
		//	// the file is now open, truncated to zero, and ready for writing, so
		//	// we must now attach it to a CArchive initialized for storing
		//	CArchive ar( &f, CArchive::store, 8192); // buffer size of 2*default should be enough

		//	// serialize the KB
		//	m_pGlossingKB->Serialize(ar);

		//	// flush the archive's buffer and close the archive
		//	ar.Close();

		//	// close file object to complete file's data transfer
		//	f.Close();

		//	// remove the *.BAK.xml backup glossing kb file, if there is one of same name, so that
		//	//  the *.BAK file is the only form of the backup glossing kb left on disk
		//	if (m_altGlossingKBBackupPath != m_curGlossingKBBackupPath)
		//	{
		//		if (f.GetStatus(m_altGlossingKBBackupPath,status) && 
		//			(status.m_attribute == CFile::normal || status.m_attribute == CFile::archive))
		//		{
		//			// there is an *.BAK.xml backup glossing knowledge base file on the disk, so delete it
		//			try {
		//				f.Remove(m_altGlossingKBBackupPath);
		//			}
		//			catch (CFileException* pe)
		//			{
		//				AfxMessageBox(
		//					_T("Could not remove the *.BAK.xml backup glossing knowledge base alternate file"));
		//				pe->Delete();
		//				// do nothing else, let the app continue
		//			}
		//		}
		//	}
		//}

		// make the backup one have m_attribute set to normal, if we don't do this, it defaults to archive
		// which you'd not expect but then, that's MFC for you!
		// Not needed in WX
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE unless a serious enough error occurs that the app cannot continue in which
///                 case it returns FALSE
/// \param      bAutoBackup   -> if TRUE it also saves a backup copy of the KB
/// \remarks
/// Called from: the App's SetupDirectories(), LoadKB(), SaveKB(),
/// SubstituteKBBackup(), AccessOtherAdaptionProject(), the Doc's DoFileSave(), the View's
/// OnCreate().
/// Stores the KB data in the external xml file located in the path represented
/// by m_curKBPath.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::StoreKB(bool bAutoBackup)
{
	wxFile f; // create a CFile instance, using default constructor
	wxString path;

	// ensure the extension is what it should be
	gpApp->GetDocument()->UpdateFilenamesAndPaths(TRUE,TRUE,TRUE,FALSE,FALSE);

	if (!f.Open(m_curKBPath, wxFile::write))
	{
			wxString message;
			message = _("Error opening KB file for writing with path:\n") 
				+ m_curKBPath + _("\nThe knowledge base was not saved.\nIs your drive's free space low, or is the file open in another application?");
			wxMessageBox(message, _T(""), wxICON_INFORMATION);
			return FALSE;
	}

	if (m_bSaveAsXML) // always true in the wx version
	{
		DoKBSaveAsXML(f);

		// close the file
		f.Close();

		// remove the .KB file, if there is one of same name, so that the .xml file
		// is the only form of the document left on disk
		if (m_altKBPath != path)
		{
			if (wxFileExists(m_altKBPath))
			{
				// there is a *.KB knowledge base file on the disk, so delete it
				if (!::wxRemoveFile(m_altKBPath))
				{
					wxMessageBox(_("Could not remove the *.KB knowledge base alternate file"),_T(""),wxICON_INFORMATION);
					// do nothing else, let the app continue
				}
			}
		}
	}
	// WX version doesn't do binary serialization
	//else
	//{
	//	// the file is now open, truncated to zero if necessary, and ready for writing, so
	//	// we must now attach it to a CArchive initialized for storing
	//	CArchive ar( &f, CArchive::store, 8192); // buffer size of 2*default should be enough

	//	// serialize the KB
	//	m_pKB->Serialize(ar);

	//	// flush the archive's buffer and close the archive
	//	ar.Close();

	//	// close file object to complete file's data transfer
	//	f.Close();

	//	// remove the .xml file, if there is one of same name, so that the .KB file
	//	// is the only form of the document left on disk
	//	if (m_altKBPath != path)
	//	{
	//		if (f.GetStatus(m_altKBPath,status) && 
	//			(status.m_attribute == CFile::normal || status.m_attribute == CFile::archive))
	//		{
	//			// there is an *.xml knowledge base file on the disk, so delete it
	//			try {
	//				f.Remove(m_altKBPath);
	//			}
	//			catch (CFileException* pe)
	//			{
	//				AfxMessageBox(_T("Could not remove the *.xml knowledge base alternate file"));
	//				pe->Delete();
	//				// do nothing else, let the app continue
	//			}
	//		}
	//	}
	//}

	// if backing up is desired, we rename the newly saved copy as a *.BAK file, or *.BAK.xml
	// if m_bSaveAsXML is TRUE, then resave the knowledge base to the old name again
	if (bAutoBackup)
	{
		// first, remove the old backup file, if it still is on disk
		if (::wxFileExists(m_curKBBackupPath) && !::wxDirExists(m_curKBBackupPath))
		{
			// found a backup, so remove it to make way for new backup
			if (!::wxRemoveFile(m_curKBBackupPath))
			{
				// notify user of error (maybe backup file is protected or in use???)
				wxMessageBox(_("Removing backup kb file failed."), _T(""), wxICON_ERROR);
				return TRUE; // allow app to continue
			}
		}

		// attempt to rename the file to the name with .BAK extension (for the binary file), or
		// we are saving as XML, rename to the name with .BAK.xml dual extensions -- the extension
		// will have been made correct by code which is above
		if (!::wxRenameFile(m_curKBPath,m_curKBBackupPath))
		{
			wxString message;
			message = _("Error renaming backup KB file with path:\n") 
				+ m_curKBPath + _("\nBackup copy of knowledge base was not created.");
			wxMessageBox(message, _T(""), wxICON_INFORMATION);
			return TRUE; // allow app to continue
		}

		
		if (!f.Open(m_curKBPath, wxFile::write))
		{
			wxString message;
			message = _("Error opening KB file for writing with path:\n") 
				+ m_curKBPath + _("\nThe knowledge base was not saved.\nIs your drive's free space low, or is the file open in another application?");
			wxMessageBox(message, _T(""), wxICON_INFORMATION);
			return FALSE;
		}

		if (gpApp->m_bSaveAsXML) // always true in the wx version
		{
			DoKBSaveAsXML(f);

			// close the file
			f.Close();

			// remove the .BAK file, if there is one of same name, so that the .BAK.xml file is the
			// only form of the KB backup file left on disk
			if (m_altKBBackupPath != m_curKBBackupPath)
			{
				if (wxFileExists(m_altKBBackupPath))
				{
					if (!::wxRemoveFile(m_altKBBackupPath))
					{
							wxMessageBox(_("Could not remove the *.BAK backup knowledge base alternate file"),_T(""),wxICON_INFORMATION);
							// do nothing else, let the app continue
					}
				}
			}
		}
		// WX version doesn't do binary serialization
		//else
		//{
		//	// the file is now open, truncated to zero, and ready for writing, so
		//	// we must now attach it to a CArchive initialized for storing
		//	CArchive ar( &f, CArchive::store, 8192); // buffer size of 2*default should be enough

		//	// serialize the KB
		//	m_pKB->Serialize(ar);

		//	// flush the archive's buffer and close the archive
		//	ar.Close();

		//	// close file object to complete file's data transfer
		//	f.Close();

		//	// remove the *.BAK.xml backup file, if there is one of same name, so that
		//	//  the *.BAK file is the only form of the document left on disk
		//	if (m_altKBBackupPath != m_curKBBackupPath)
		//	{
		//		if (f.GetStatus(m_altKBBackupPath,status) && 
		//			(status.m_attribute == CFile::normal || status.m_attribute == CFile::archive))
		//		{
		//			// there is an *.BAK.xml backup knowledge base file on the disk, so delete it
		//			try {
		//				f.Remove(m_altKBBackupPath);
		//			}
		//			catch (CFileException* pe)
		//			{
		//				AfxMessageBox(
		//					_T("Could not remove the *.BAK.xml backup knowledge base alternate file"));
		//				pe->Delete();
		//				// do nothing else, let the app continue
		//			}
		//		}
		//	}
		//}

	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE unless an error occurred and the KB could not be stored in which case it
///                 returns FALSE
/// \param      bAutoBackup   -> if TRUE it also saves a backup copy of the KB
/// \remarks
/// Called from: the App's SetupDirectories(), LoadKB(), SaveKB(),
/// SubstituteKBBackup(), AccessOtherAdaptionProject(), the Doc's DoFileSave(), the View's
/// OnCreate().
/// Calls StoreKB() to store the KB data in the external xml file.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::SaveKB(bool bAutoBackup)
{
	if (bAutoBackup)
	{
		// Do any needed adjustment to m_curKBBackupPath. Note -- this is different than expected,
		// because we don't want KB files which could have been binary (extension .KB)
		// or XML to just have a .BAK extension for the backup one - since then we'd not know from the
		// extension whether the contents were binary or xml. So for the backups, we'll have .BAK for
		// a binary backup, and .BAK.xml for an XML backup (ie. just add the .xml extension to the
		// backup name as produced by the legacy code); we handle m_curKBFilename, m_curKBPath too,
		// because we keep all three in synch every time we change them
		gpApp->GetDocument()->UpdateFilenamesAndPaths(TRUE,TRUE,TRUE,FALSE,FALSE);

		// is there a backup file (ie. with .BAK extension)
		if(::wxFileExists(m_curKBBackupPath) && !::wxDirExists(m_curKBBackupPath))
		{
			// found a backup, so remove it to make way for new backup
			if (!::wxRemoveFile(m_curKBBackupPath))
			{
				// notify user of error (maybe backup file is protected or in use???)
				// The following message did not exist in the MFC version
				wxMessageBox(_("Removing backup kb file failed."), _T(""), wxICON_ERROR);
				return FALSE;
			}
		}
		// no removal needed if there is not a backup present
	}
	bool bOK = StoreKB(bAutoBackup);

	return bOK;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE unless an error occurred and the glossing KB could not be stored in which 
///                 case it returns FALSE
/// \param      bAutoBackup   -> if TRUE it also saves a backup copy of the glossing KB
/// \remarks
/// Called from: the App's DoGlossingKBBackup(), DoKBRestore(), 
/// SetupDirectories(), LoadKB(), SaveKB(), the View's OnFileSaveKB(), and
/// OnToolsKbEditor().
/// Calls StoreGlossingKB() to store the glossing KB data in the external xml file.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::SaveGlossingKB(bool bAutoBackup)
{
	if (bAutoBackup)
	{
		// Do any needed adjustment to m_curGlossingKBBackupPath. Note -- this is different than expected,
		// because we don't want KB files which could have been binary (extension .KB)
		// or XML to just have a .BAK extension for the backup one - since then we'd not know from the
		// extension whether the contents were binary or xml. So for the backups, we'll have .BAK for
		// a binary backup, and .BAK.xml for an XML backup (ie. just add the .xml extension to the
		// backup name as produced by the legacy code); we handle m_curKBFilename, m_curKBPath too,
		// because we keep all three in synch every time we change them
		gpApp->GetDocument()->UpdateFilenamesAndPaths(FALSE,FALSE,FALSE,TRUE,TRUE);

		// is there a backup file (ie. with .BAK extension)
		if(::wxFileExists(m_curGlossingKBBackupPath) && !::wxDirExists(m_curGlossingKBBackupPath))
		{
			// found a backup, so remove it to make way for new backup
			if (!::wxRemoveFile(m_curGlossingKBBackupPath))
			{
				// notify user of error (maybe backup file is protected or in use???)
				// The following message did not exist in the MFC version
				wxMessageBox(_("Removing backup glossing kb file failed."), _T(""), wxICON_ERROR);
				return FALSE;
			}
		}
		// no removal needed if there is not a backup present
	}
	bool bOK = StoreGlossingKB(bAutoBackup);

	return bOK;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxUpdateUIEvent that is generated when the File Menu is about
///                         to be displayed
/// \remarks
/// Called from: The wxUpdateUIEvent mechanism when the associated menu item is selected, and before
/// the menu is displayed. If the app is in Vertical Edit Mode the Start Working Wizard is disabled
/// and this handler returns immediately.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnUpdateFileStartupWizard(wxUpdateUIEvent& event) 
{
	if (gbVerticalEditInProgress)
	{
		event.Enable(FALSE);
		return;
	}
	event.Enable(TRUE); // we want it always available, with several entry points
}


////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if successful in getting all pointers, otherwise FALSE
/// \param      pDoc   <- pointer to the document
/// \param      pView  <- pointer to the view
/// \param      pBox   <- pointer to the phrasebox
/// \remarks
/// Called from: the Doc's OnCloseDocument(), the View's OnRetransReport(), OnFileExportKb(),
/// CMainFrame's SyncScrollReceive(), ComposeBarGuts(), OnIdle(), and 
/// CPhraseBox::ChooseTranslation().
/// Gets pointers for the Doc, View, and PhraseBox.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::GetBasePointers(CAdapt_ItDoc*& pDoc, CAdapt_ItView*& pView, 
									CPhraseBox*& pBox)
// return FALSE if not successful in getting all pointers, TRUE if all are okay
{
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);
	pDoc = (CAdapt_ItDoc*)NULL;
	pView = (CAdapt_ItView*)NULL;
	pBox = (CPhraseBox*)NULL;

	pDoc = GetDocument();
	if (pDoc == NULL) return FALSE;
	pView = (CAdapt_ItView*)pDoc->GetFirstView();
	if (pView == NULL) return FALSE;
	wxASSERT(pView->IsKindOf(CLASSINFO(CAdapt_ItView))); 
	pBox = pApp->m_pTargetBox;
	if (pBox == NULL) return FALSE; // MFC has if (pBox->m_hWnd == NULL) return FALSE;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     pointer to the MapSfmToUSFMAnalysisStruct map specified by sfmSet
/// \param      sfmSet   -> the specified sfm set (UsfmOnly, PngOnly or UsfmAndPng)
/// \remarks
/// Called from: the Doc's ReconstituteAfterFilteringChange(), ResetUSFMFilterStructs(),
/// GetUnknownMarkersFromDoc(), the View's GetMarkerInventoryFromCurrentDoc(), 
/// MarkerTakesAnEndMarker(), and (from ExportFunctions.cpp) DoExportInterlinearRTF(),
/// BuildRTFTagsMap(), BuildColorTableFromUSFMColorAttributes(), GetMaxMarkerLength(),
/// the CFilterPageCommon::LoadDocSFMListBox(), CFilterPageCommon::LoadProjSFMListBox(),
/// CUSFMPageCommon::DoInit(), CUSFMPageCommon::DoBnClickedRadioUseUbsSetOnlyDoc(),
/// CUSFMPageCommon::DoBnClickedRadioUseSilpngSetOnlyDoc(), 
/// CUSFMPageCommon::DoBnClickedRadioUseBothSetsDoc(), 
/// CUSFMPageCommon::DoBnClickedRadioUseUbsSetOnlyProj(), 
/// CUSFMPageCommon::DoBnClickedRadioUseSilpngSetOnlyProj(),
/// CUSFMPageCommon::DoBnClickedRadioUseBothSetsProj().
/// Gets a pointer to the current MapSfmToUSFMAnalysisStruct map specified by sfmSet.
////////////////////////////////////////////////////////////////////////////////////////////
MapSfmToUSFMAnalysisStruct* CAdapt_ItApp::GetCurSfmMap(enum SfmSet sfmSet)
{
	// returns the current USFMAnalysis map for the sfmSet
	switch(sfmSet)
	{
		case UsfmOnly: return gpApp->m_pUsfmStylesMap;
		case PngOnly: return gpApp->m_pPngStylesMap;
		case UsfmAndPng: return gpApp->m_pUsfmAndPngStylesMap;
		default: return gpApp->m_pUsfmStylesMap;// this should never happen
	}
}

/*
////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the filter markers being used by the sfmSet
/// \param      sfmSet   -> the specified sfm set (UsfmOnly, PngOnly or UsfmAndPng)
/// \remarks
/// Called from: currently unused.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetFilterMkrStrForSelectedSfmSet(enum SfmSet sfmSet)
{
	switch (sfmSet)
	{
	case UsfmOnly: return UsfmFilterMarkersStr;
	case PngOnly: return PngFilterMarkersStr;
	case UsfmAndPng: return UsfmAndPngFilterMarkersStr;
	default: return UsfmFilterMarkersStr;
	}
}
*/

// The OnIdle function moved from the App to CMainFrame
// bool CAdapt_ItApp::OnIdle(LONG lCount)
//{
//}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     always TRUE (but it aborts the program with a call to wxExit() if the path
///                 represented by m_curAdaptionsPath could not be found)
/// \param      event   -> (unused)
/// \remarks
/// Called from: DoStartWorkingWizard is called by App's DoFileOpen(), OnFileChangeFolder(), 
/// and OnAdvancedBookMode(), and also by the View's OnFileStartupWizard(), which in 
/// turn is called by DoStartupWizardOnLaunch() which gets called by the CMainFrame's OnIdle 
/// method, when the program starts (or by File | Start Working...).
/// Initiates the Start Working Wizard which has smarts to detect which page of the wizard
/// should be presented to the user for the current working situation.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::DoStartWorkingWizard(wxCommandEvent& WXUNUSED(event))
{
	// 
	// 
	// Note: This version of DoStartWorkingWizard() is 
	// structured quite differently from the MFC version.
	// WX Notes:
	// 1. The CPropertySheet and CPropertyPage classes in MFC are designed
	//    to double as tabbed property sheet pages in a single preferences dialog, 
	//    or, when CPropertySheet::SetWizardMode() is called, the property sheet
	//    turns into a wizard with more or less linear traversal of the pages. 
	//    In WX we don't have a single CPropertySheet class for both functions, 
	//    instead, we can define individual pages as in MFC, but we then use them 
	//    in two separate built-in WX classes: wxNotebook (for tabbed preferences 
	//    dialog), and wxWizard for the Start Working wizard.
	// 2. I originally tried to structure the operation of the wizard using all
	//    wxWizardSimple page classes. That design, however, resulted in problems
	//    because I destroyed the wizard and called RunWizard again each time
	//    I wanted the wizard to change the page ordering/sequencing. Eventually
	//    I decided to use the more advanced wxWizardPage class for all the wizard 
	//    pages.

	// Do we have a valid directory? Probably not if user copied
	// a project from another user or a different platform.
	// TODO: Check to see if following check is now redundant
	bool bDirPathOk = ::wxDirExists(m_curAdaptionsPath);
	if (m_curAdaptionsPath.IsEmpty())
	{
		// path is empty, possibly because user held SHIFT key down
		; // do nothing here
	}
	else if (!bDirPathOk)
	{
		wxString message1, message2;
		message1 = message1.Format(_("Failed to find the %s folder."),m_curAdaptionsPath.c_str());
		message2 = message1 + _("\nIf you want to continue, you must choose a different project or create a new project.\nDo you want to continue? ");
		int result = wxMessageBox(message2,_("Bad path in config file"), wxYES_NO | wxICON_ERROR);
		if (result == wxNO)
		{
			wxASSERT(FALSE);
			wxExit();
		}
	}

	CStartWorkingWizard startWorkingWizard(GetMainFrame());

	// Note:
	// wizard pages are created within the CStartWorkingWizard's constructor
	//
	// Since all the wizard pages have the startWorkingWizard as parent window
	// they will be destroyed automatically when startWorkingWizard goes out of scope below.
	//	
	// Each of the wizard page's InitDialog() handlers is NOT automatically 
	// called when the pages are created or shown in the Start Working Wizard. 
	// They must be called explicitly in program code to execute. We've made them
	// public functions in their class declarations so we can call them explicitly.
	// The InitDialog function for each page is called by the preceding page's 
	// OnWizardPageChanging() function (at the end of the bMovingForward block).
	//
	// The general ordering of the individual wizard pages is determined by the GetNext()
	// and GetPrev() calls within the page's methods. The changed order is determined and 
	// set from within the pProjectPage and the pDocPage's GetNext() and GetPrev() handlers.
	// Hence, the displaying of a 1-page, 2-page or 8-page wizard is established 
	// as the wizard runs depending on the following conditional states of the program:
	//
	//    a. If a project is currently open (user gets a 1-page wizard)
	//    b. If a project is not currently open (user gets a 2-page wizard)
	//    c. If the <New Project> item has been selected from the project page (user gets
	//       an 8-page wizard)
	// Details:
	// The 1-page wizard displays the docPage initially if a project is currently open and 
	// the user selects File | Start Working... This allows the user to select a different
	// document from the same project, or select <New Document> to create a new document in 
	// the same project.
	//
	// The 2-page wizard displays the projectPage initially (followed by the docPage) if
	// the user or program has selected an existing project on the projectPage, then continues
	// (via click on Next, or if the existing project selection was made with a double click).
	// In this case the docPage appears as the second wizard page, allowing the user to select
	// any existing document from the docPage's list, or <New Document> to create a new document
	// within the selected project. The reverse also happens if at the docPage, the user clicks
	// on the Prev button - the previous page is the projectPage.
	// 
	// The full 8-page wizard will appear if the user selects <New Project> on the projectPage 
	// and proceeds (via Next or double click). The 8 pages define the most important items of
	// information and parameters for any new project.
	// The 8 pages of the full wizard are presented in order, which are:
	//   (1) the projectPage, 
	//   (2) the langaugesPage,
	//   (3) the fontPage,
	//   (4) the punctMapPage,
	//   (5) the caseEquivPage,
	//   (6) the USFMpage,      [added in version 3]
	//   (7) the FilteringPage, [added in version 3]
	//   (8) the docPage. 
	// At any time within the 1-page or 2-page wizard the user can use the Back button to go
	// back to the project page and start creating a new project by selecting <New Project>
	// from the project page and continuing through the now active full 8-page wizard.

	// Call RunWizard to start the wizard
	pStartWorkingWizard->RunWizard(pStartWorkingWizard->GetFirstPage());

	gbWizardNewProject = FALSE; // reset wizard global back to FALSE
	pStartWorkingWizard = (CStartWorkingWizard*)NULL;

	// The code below was within the else (not ID_WIZFINISH) above
	// BEW added 31Oct05 to prevent the Project config file for the last-opened
	// project being clobbered by a Cancel of the wizard at the project page, or 
	// any other page other than the doc_page (since the latter will have resulted
	// in the project config file being loaded, if the project pre-existed).
	if (!gbReachedDocPage)
	{
		// user cancelled before getting to the document page, and so we cannot
		// know what project path is valid, so set m_curProjectPath to NULL
		// so that SaveModified() (which gets called, for example, from OnClose()'s
		// call of CanCloseFrame() when user clicks window's close box) does not
		// write out a default project config file to the last opened project - thereby
		// clobbering any case equivalences set up in it and its auto caps flag setting
		// if the latter was turned on in that project)
		m_curProjectPath.Empty();
	}

	// BEW added 28Nov05. If the user reached the Document page, in book mode, and then cancelled out
	// he likely did so because there were no files in the book folder yet and he cancelled out in
	// order to use the Move command to move one or more from the Adaptations folder into the book
	// folder. Without the following change, the status bar continues to say that the current folder is
	// the Adaptations one, when in fact it is not. So we have the status bar updated here too.
	if (gbReachedDocPage)
		RefreshStatusBarInfo();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     pointer to the document
/// \remarks
/// Called from: many places throughout the application where a pointer to the document is 
/// requited to call a function/method of the Doc.
/// Gets a pointer to the current document.
////////////////////////////////////////////////////////////////////////////////////////////
CAdapt_ItDoc* CAdapt_ItApp::GetDocument()
{
	// We need to get the first doc in doc manager's list
	CAdapt_ItDoc* pDoc = (CAdapt_ItDoc*)NULL;
	pDoc = (CAdapt_ItDoc*)m_pDocManager->GetCurrentDocument();
	if (pDoc == NULL)
	{
		return pDoc;
	}
	pDoc = (CAdapt_ItDoc*)m_pDocManager->GetDocuments().GetFirst()->GetData();
	if (pDoc == NULL) return NULL;
	wxASSERT(pDoc->IsKindOf(CLASSINFO(CAdapt_ItDoc)));
	return pDoc;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     pointer to the view
/// \remarks
/// Called from: many places throughout the application where a pointer to the view is 
/// requited to call a function/method of the View.
/// Gets a pointer to the current view.
////////////////////////////////////////////////////////////////////////////////////////////
CAdapt_ItView* CAdapt_ItApp::GetView()
{
	// We need to get the first view associated with the first doc
	CAdapt_ItDoc* pDoc;
	pDoc = GetDocument();
	if (pDoc == NULL)
		return NULL;
	CAdapt_ItView* pView = (CAdapt_ItView*)GetDocument()->GetFirstView();
	if (pView == NULL) return NULL;
	wxASSERT(pView->IsKindOf(CLASSINFO(CAdapt_ItView)));
	return pView;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: the Doc's OnFileOpen().
/// In recent version of Adapt It, this always calls up the Start Working Wizard by a call
/// to DoStartWorkingWizard().
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::DoFileOpen()
{
	// DoFileOpen() makes it possible to contextualize the Title of the file dialog.
	// MFC version note:
	// BEW changed 4May 05 so that a click on the Open command in the File menu (which can only
	// be done after the document has been closed - and so the window will be clear and the old
	// list of sourcephrase instances for the document will have been cleaned out already) will
	// open the Start Working... wizard at the document page. (It opens at the document page
	// because DoStartWorkingWizard() has a test near the start for m_pKB non-NULL and
	// the flag m_bKBReady being TRUE, and if so then it knows a project is open and so it
	// should open at the document pane.) 
	// The reason for changingi DoFileOpen() to avoid using the MFC standard file i/o dialog was
	// because of the possibility that Book Folder mode might have been used at some earlier time
	// and if so the Adaptations folder will have 67 folders at the top of the list of files and
	// folders. Not only would this be confusing to the user (he'd have to scroll down a long way
	// to see the available documents), but he could use the dialog to open any book folder or Adaptations
	// folder without Adapt It knowing, and so defeat the built in modality of legacy mode versus book
	// folder mode. This must be prevented at any cost, and so causing a click on the Open command
	// to instead open the wizard is both aesthetically pleasing and safe. This means it is 
	// impossible for the user to navigate up or down the folder hierarchy within Adapt It except
	// by the one way which Adapt It totally controls - to turn on or off book folders mode.
	wxCommandEvent dummyevent;
	DoStartWorkingWizard(dummyevent);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: the App's OnInit() and GetBasicSettingsConfiguration().
/// Makes use of the wxWidgets ::wxGetHomeDir() method to determine the best platform-specific
/// location for the user's work folder, creating it if necessary. The user's work folder is
/// stored in the m_workFolderPath global.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::EnsureWorkFolderPresent()
{
	// TODO: Modify this rouitine to implement the ability of the user to specify a non-default
	// location for a work folder. The non-default location then needs to be recorded in the
	// m_pConfig instance of the wxConfig class (stored in the Registry on a Windows system, but
	// in a hidden file on Linux and the Mac).
	// Note: This function is significantly changed from the MFC version because of
	// cross-platform differences in directory structures and file handling.
	// I haven't tried to represent what other changes Bruce made to the MFC version in his file 
	// sent 15Mar04. He basically initialized rStrLocalizedMyDocsName then assigned dirPath to it.
	wxString homeDir = _T("");
	wxString dirPath = _T("");
	wxString workFolderPath = _T("");
	wxString workFolder;

	workFolder = m_theWorkFolder;

	// The wxWidgets ::wxGetHomeDir() function returns different things depending 
	// on the system/platform it's called on:
	// On Windows systems it returns the path to the user's directory within 
	// the "Documents and Settings" directory, usually off the C: drive. 
	// We use this to test our assumption that the "My Documents" directory 
	// is simply a direct subdirectory of this "home" Directory. 
	// On Unix/Linux systems this should return the /usr/home directory.

	// Get the "home" directory for the current system/platform. 
	homeDir = ::wxGetHomeDir();
	// The PathSeparator becomes the appropriate symbol; \ on Windows, / on Linux
	// whm note: We should not localize the "My Documents" folder otherwise
	// we may not be able to find the folder
	if (::wxDirExists(homeDir + PathSeparator +_T("My Documents")))
	{
		// We've found a typical "My Documents" folder so we probably have a Windows system
		// Set the current directory to the "My Documents" folder
		::wxSetWorkingDirectory(homeDir + PathSeparator + _T("My Documents"));
		dirPath = ::wxGetCwd(); // on Win98 should be "C:\My Documents", but not on 
								// NT or 2000 or XP where it would be "C:\Documents and Settings\<UserName>\My Documents"
		m_localPathPrefix = dirPath; // m_localPathPrefix used in MakeForeignBasicConfigFileSafe below
		workFolderPath = dirPath + PathSeparator + workFolder;
	}
	else
	{
		// No "My Documents" folder exists off the HomeDir, so we either
		// have an old Windows system or a Unix/Linux type file system
		// so we'll just plan to use the detected HomeDir for the dirPath and
		// dirPath + PathSeparator + WorkFolder to put our AI stuff
		dirPath = homeDir;
		m_localPathPrefix = dirPath; // m_checkFolder used in MakeForeignBasicConfigFileSafe below
		workFolderPath = dirPath + PathSeparator + workFolder;
		::wxSetWorkingDirectory(dirPath); // set the current dir to the HomeDir
	}

	if (!::wxDirExists(workFolderPath))
	{
		// we did not find the required directory, so create it now - it has to be
		// present before the app calls GetPossibleAdaptionProjects function
		// WX NOTE: On Unix/Linux wxMkdir has a second default param: int perm = 0777 which
		// makes a directory with full read, write, and execute permissions.
		if (!::wxMkdir(workFolderPath))
		{
			// work folder creation failed for some reason, so abort
			wxString message;
			message = _("Adapt It cannot create its work folder: ") + workFolderPath;
			message += _("\nAdapt It cannot continue and will now abort.");
			wxMessageBox(message, _("Critical Error"), wxICON_ERROR | wxOK);
			abort();	// wxExit() calls OnExit() before aborting the program which would attempt
						// to delete some objects not yet created.
		}
	}
	// workFolderPath exists so set the current work directory to it 
	::wxSetWorkingDirectory(workFolderPath);

	// be sure to set the m_workFolderPath variable (path to the Adapt It (NR) Work folder) 
	// before exiting
	m_workFolderPath = workFolderPath;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     
/// \param      bSuppressOKMessage   -> if TRUE suppresses OK prompt from CheckKBIntegrity()
/// \remarks
/// Called from: the App's OnFileBackupKb() and OnFileRestoreKb().
/// Does an integrity check of the glossing KB. If that is OK, it calls SaveGlossingKB().
/// If the integrity check fails, it asks the user if the backup KB should be used for the
/// faulty one; if so, calls SubstituteKBBackup().
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::DoGlossingKBBackup(bool bSuppressOKMessage)
{
	if (CheckKBIntegrity(bSuppressOKMessage, TRUE)) // TRUE == do it on the glossing KB
	{
		SaveGlossingKB(TRUE); // resaves the KB and makes a backup which is a copy, 
							 // after deleting old one
	}
	else
	{
		// integrity check failed, so give user the option of substituting the backup KB
		// IDS_SUBSTITUTE_KB_BAK
		switch (wxMessageBox(_("Do you wish to substitute the backup knowledge base for the faulty one? (Entries made since you last saved will not be included.)"),_T(""), wxYES_NO))
		{
		case wxYES:
			SubstituteKBBackup(TRUE); // TRUE == do it on the glossing KB
			break;
		case wxNO:
			// don't make any substitution
			break;
		default:
			wxASSERT(FALSE);
			break;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////
/// \return     
/// \param      bSuppressOKMessage   -> if TRUE suppresses OK prompt from CheckKBIntegrity()
/// \remarks
/// Called from: the App's OnFileBackupKb() and OnFileRestoreKb().
/// Does an integrity check of the regular KB. If that is OK, it calls SaveKB().
/// If the integrity check fails, it asks the user if the backup KB should be used for the
/// faulty one; if so, calls SubstituteKBBackup().
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::DoKBBackup(bool bSuppressOKMessage)
{
	if (CheckKBIntegrity(bSuppressOKMessage, FALSE)) // FALSE == do it on the adaptation KB
	{
		// NOTE: wxWidgets version: SaveKB returns bool value which is ignored here.
		// I've added an error message in SaveKB() in case it fails. MFC code had no
		// error message anywhere.
		SaveKB(TRUE); // resaves the KB and makes a backup which is a copy, 
					  // after deleting old one
	}
	else
	{
		// integrity check failed, so give user the option of substituting the backup KB
		// IDS_SUBSTITUTE_KB_BAK
		switch (wxMessageBox(_("Do you wish to substitute the backup knowledge base for the faulty one? (Entries made since you last saved will not be included.)"), _T(""), wxYES_NO))
		{
		case wxYES:
			SubstituteKBBackup(FALSE); // FALSE == do it on the glossing KB
			break;
		case wxNO:
			// don't make any substitution
			break;
		default:
			wxASSERT(FALSE);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: OnFileCheckKb().
/// Calls CheckKBIntegrity() with parameters set to check the glossing KB and allow OK prompt.
/// If the integrity check fails it asks the user if the backup KB should be substituted; if so,
/// calls SubstituteKBBackup() to substitute the glossing backup KB.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::GlossingKBIntegrityCheck()
{
	bool bOK = CheckKBIntegrity(FALSE,TRUE); // don't suppress the OK message
											  // and do the check on the glossing KB
	if (!bOK)
	{
		// integrity check failed, so give user the option of substituting the backup KB
		// IDS_SUBSTITUTE_KB_BAK
		switch (wxMessageBox(_("Do you wish to substitute the backup knowledge base for the faulty one? (Entries made since you last saved will not be included.)"),_T(""),wxYES_NO))
		{
		case wxYES:
			SubstituteKBBackup(TRUE); // TRUE == do it on the glossing KB
			break;
		case wxNO:
			// don't make any substitution
			break;
		default:
			wxASSERT(FALSE);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: OnFileCheckKb().
/// Calls CheckKBIntegrity() with parameters set to check the regular KB and allow OK prompt.
/// If the integrity check fails it asks the user if the backup KB should be substituted; if so,
/// calls SubstituteKBBackup() to substitute the backup KB.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::KBIntegrityCheck()
{
	bool bOK = CheckKBIntegrity(FALSE,FALSE); // don't suppress the OK message
											  // and do the check on the adaptation KB
	if (!bOK)
	{
		// integrity check failed, so give user the option of substituting the backup KB
		// IDS_SUBSTITUTE_KB_BAK
		switch (wxMessageBox(_("Do you wish to substitute the backup knowledge base for the faulty one? (Entries made since you last saved will not be included.)"),_T(""), wxYES_NO))
		{
		case wxYES:
			SubstituteKBBackup(FALSE); // FALSE == do it on the adaptation KB
			break;
		case wxNO:
			// don't make any substitution
			break;
		default:
			wxASSERT(FALSE);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     FALSE if a failure occurred, otherwise TRUE
/// \param      bSuppressOKMessage   -> suppress the OK prompt
/// \param      bDoOnGlossingKB      -> check the glossing KB if TRUE, otherwise check the regular KB
/// \remarks
/// Called from: the App's DoGlossingKBBackup(), DoKBBackup(), GlossingKBIntegrityCheck(),
/// and KBIntegrityCheck().
/// Calls the DoKBIntegrityCheck() function on the appropriate KB. Writes any errors encountered
/// in a KBErrorLog.txt file in the project folder.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::CheckKBIntegrity(bool bSuppressOKMessage, bool bDoOnGlossingKB)
// this function will operate on either the normal adaptation KB or the glossing KB
// depending on the value of the bDoOnGlossingKB flag (default is FALSE)
{
	wxArrayString errors; // for use by KBErrorLog file

	wxString logName = _T("KBErrorLog.txt");

	bool bWasOK = DoKBIntegrityCheck(errors,bDoOnGlossingKB); 
	if (!bWasOK)
	{
		// warn the user, & inform about the KBErrorLog.txt file, & that KB recovery 
		// strategies exist
		// IDS_KB_ERRORS
		wxMessageBox(_("Sorry, integrity checking of the knowledge base revealed there were some errors. See the KBErrorLog.txt file in your project folder. Delete the bad knowledge base and build a new one."), _T(""), wxICON_INFORMATION);

		// save the error log to the KBErrorLog.txt file, overwriting any old instance, 
		// in project folder
		bool bOK = ::wxSetWorkingDirectory(m_curProjectPath);
		if (!bOK)
		{
			// keep it simple, we can continue working without doing the log file 
			// nor the KB backup
			wxMessageBox(_T(
	"Failed when setting the directory for writing the KBErrorLog.txt file, operation skipped."),_T(""),
				wxICON_INFORMATION);
			return FALSE;
		}

		// Note: Since we want a text file output, we'll use wxTextOutputStream which
		// writes text files as a stream on DOS, Macintosh and Unix in their native 
		// formats (concerning their line endings)
		wxFileOutputStream output(logName);
		wxTextOutputStream cout(output);

		wxString msgTitle;
		wxString errorMsg;
		if (wxTheApp->GetAppName() != wxT(""))
			msgTitle = wxTheApp->GetAppName();
		else
			msgTitle = wxString(_("File error"));
		if (output.GetLastError() != wxSTREAM_NO_ERROR)
		{
			if (bDoOnGlossingKB)
			{
				errorMsg = errorMsg.Format(_(
					"Error creating glossing KBErrorLog.txt file for writing with path:\n%s."),
															m_curGlossingKBPath.c_str());
			}
			else
			{
				errorMsg = errorMsg.Format(_(
					"Error creating KBErrorLog.txt file for writing with path:\n%s."),
															m_curKBPath.c_str());
			}
			(void)wxMessageBox(errorMsg, 
					msgTitle, wxOK | wxICON_EXCLAMATION);
			return FALSE;
		}

		// write the strings, one per line
		wxString str = _T("");
		size_t nErrors = errors.GetCount();
		for (size_t ct = 0; ct < nErrors; ct++)
		{
			str = errors[ct];
			str += _T("\n\n");
			cout << str;
		}
		if (output.GetLastError() != wxSTREAM_NO_ERROR)
		{
			errorMsg = _("Could not complete writing to the KBErrorLog.txt file.\n");
			errorMsg += _("The KBErrorLog.txt file should be correct for those entries listed within it.");
			(void)wxMessageBox(errorMsg, 
					msgTitle, wxOK | wxICON_EXCLAMATION);
			// Saving error
			return FALSE;
		}
		// Note: wxWidgets streams automatically close their file descriptors when
		// they go out of scope

		// remove the strings, otherwise memory will be leaked
		errors.Clear();

		return FALSE;
	}
	if (!bSuppressOKMessage)
	{
		if (bDoOnGlossingKB)
			// IDS_GLOSSINGKB_INTEGRITY_OK
			wxMessageBox(_("No errors were detected in the glossing knowledge base."), _T(""), wxICON_INFORMATION);
		else
			// IDS_KB_INTEGRITY_OK
			wxMessageBox(_("No errors were detected in the knowledge base."), _T(""), wxICON_INFORMATION);
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     FALSE if any errors were found, otherwise TRUE
/// \param      errors           <- an array string containing a list of errors encountered
/// \param      bDoOnGlossingKB  -> check the glossing KB if TRUE, otherwise check the regular KB
/// \remarks
/// Called from: the App's CheckKBIntegrity().
/// Does the actual integrity check on the appropriate KB. Stores any errors encountered
/// in the errors wxArrayString returned by reference to the caller.
/// Note: Only a few tests can be conducted in the wxWidgets version: a check that m_nMaxWords value 
/// is consistent with largest indexed map which is nonempty, and a test of integrity of the KB maps. 
/// No equivalent function to MFC's AfxIsValidAddress() has been found for the remaining tests.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::DoKBIntegrityCheck(wxArrayString& errors, bool bDoOnGlossingKB)
{
	CKB* pKB;
	if (bDoOnGlossingKB)
		pKB = m_pGlossingKB;
	else
		pKB = m_pKB;
	wxASSERT(pKB != NULL);
	wxString strError;
	bool bFoundErrors = FALSE;
	MapKeyStringToTgtUnit* pMap; 
	// MapKeyStringToTgtUnit is a wxWidgets hash map delcared by macro in KB.h
	// Note: Since the wxWidgets version has no AfxIsValidAddress() call the 
	// only test we can conduct is test 1. So I'm commenting out pMap above
	// since it is not used.

	// test 1: that m_nMaxWords value is consistent with largest indexed map 
	// which is nonempty
	int nMaxIndex = pKB->m_nMaxWords - 1;
	wxASSERT(nMaxIndex >= 0);
	bool bNoError = TRUE;

	int index;
	for (index = nMaxIndex + 1; index < MAX_WORDS; index++)
	{
		if (pKB->m_pMap[index]->size() > 0)
			bNoError = FALSE;
	}
	if (!bNoError)
	{
		//IDS_KB_LOW_INDEX_ERR // wxString::Format() returns a static wxString
		strError = strError.Format(_("The index to the storage containing the longest phrases is too low ( %d )."), nMaxIndex);
		errors.Add(strError);
		bFoundErrors = TRUE;
	}

	// MFC docs say about it:
	// AfxIsValidAddress(const void* lp, UINT nBytes, BOOL bReadWrite = TRUE);
	// AfxIsValidAddress "Tests any memory address to ensure that it is 
	// contained entirely within the program's memory space...The
	// address is not restricted to blocks allocated by new.
	// NOTE: I find no equivalent wxWidgets call to AfxIsValidAddress().
	// All such tests are highly platform/OS dependent. Hence, I'll ignore 
	// tests 2-9 in the wxWidgets version for now until I can find
	// a suitable set of validity tests for valid pointers on Linux and
	// the Mac. It will probably require conditional compiles for each
	// particular platform/OS.
	/*

	// test 2: valid addresses for each map
	bNoError = TRUE;
	for (index = 0; index <= nMaxIndex; index++)
	{
		if (!AfxIsValidAddress(pKB->m_pMap[index], sizeof(CMapStringToOb))) // bReadWrite = TRUE
		{
			strError = strError.Format(IDS_CORRUPT_MAP_PTR, index);
			errors.AddTail(strError);
			bFoundErrors = TRUE;
		}
	}

	// test 3: valid address for the target unit list
	bNoError = TRUE;
	if (!AfxIsValidAddress(pKB->m_pTargetUnits, sizeof(CObList)))
		bNoError = FALSE;
	if (!bNoError)
	{
		strError = strError.Format(IDS_CORRUPT_TULIST_PTR);
		errors.AddTail(strError);
		bFoundErrors = TRUE;
	}

	// the above errors are so destructive, further testing would crash Adapt It,
	// so if we already have an error, do no more testing
	if (bFoundErrors)
		goto a;

	// begin the nested tests, starting with the target unit list

	// test 4: valid addresses for CTargetUnit instances, in map with index = 0;
	// make a code block here, as the goto will cause a compile error otherwise
	{
		CObList* pList = pKB->m_pTargetUnits;
		wxASSERT(pList != NULL);
		wxASSERT(AfxIsValidAddress(pList,sizeof(CObList)));
		POSITION pos = pList->GetHeadPosition();
		int indx = -1;
		while (pos != NULL)
		{
			bNoError = TRUE;
			CTargetUnit* pTU = (CTargetUnit*)pList->GetNext(pos);
			indx++;
			if (!AfxIsValidAddress(pTU,sizeof(CTargetUnit)))
			{
				bFoundErrors = TRUE;
				strError = strError.Format(IDS_CORRUPT_TGTUNIT_PTR, indx + 1);
				errors.AddTail(strError);
				continue; // ignore the rest, but accumulate all these (gives an idea
						  // of just how bad things are)
			}

			// nested tests: check content of the CTargetUnit instance found
			CObList* pTranslations = &pTU->m_translations;
			// test 5: the ptr to the list object for the translations is valid
			if (!AfxIsValidAddress(pTranslations,sizeof(CObList)))
			{
				bFoundErrors = TRUE;
				strError = strError.Format(IDS_CORRUPT_TRANSLN_LIST_PTR, indx + 1);
				errors.AddTail(strError);
				continue; // ignore the rest of the tests for this target unit
			}

			// level 2 of nesting: check the CRefString instances in each target unit's list
			POSITION pos2 = pTranslations->GetHeadPosition();
			int indx2 = -1;
			while (pos2 != NULL)
			{
				// test 6: valid pointer to each CRefString instances stored in the target unit
				CRefString* pRefStr = (CRefString*)pTranslations->GetNext(pos2);
				indx2++;
				if (!AfxIsValidAddress(pRefStr,sizeof(CRefString)))
				{
					bFoundErrors = TRUE;
					strError = strError.Format(IDS_CORRUPT_REFSTR_PTR,indx + 1,indx2 + 1);
					errors.AddTail(strError);
					continue; // skip other possible errors for this CRefString
				}

				// test 7: check the string is stored in our address space
				wxString translation = pRefStr->m_translation;
				int len = translation.GetLength();
				wxASSERT(len >= 0);
				LPTSTR pBuff = translation.GetBuffer(len);
				bool bErr = !AfxIsValidAddress(pBuff,len+1);

				if (bErr)
				{
					bFoundErrors = TRUE;
					strError = strError.Format(IDS_CORRUPT_TRANSLATION,indx + 1,indx2 + 1);
					errors.AddTail(strError);
					continue; // skip any other errors for this CRefString
				}

				// test 8: check the pointer back to the target unit is correct
				if (pRefStr->m_pTgtUnit != pTU)
				{
					bFoundErrors = TRUE;
					strError = strError.Format(IDS_BAD_PTR_BACK,translation,indx + 1);
					errors.AddTail(strError);
					continue; // skip other errors
				}

				// test 9: invalid reference count
				if (pRefStr->m_refCount < 0 || pRefStr->m_refCount > 32000)
				{
					bFoundErrors = TRUE;
					strError = strError.Format(IDS_BAD_REF_COUNT,pRefStr->m_refCount, translation, indx + 1);
					errors.AddTail(strError); // this error is benign, so can continue looping
				}
			}
		}
		*/
		// checking the list of target units is finished, if we get here then any 
		// further errors could only be in the maps, so we do those now
		for (index = 0; index <= nMaxIndex; index++)
		{
			pMap = pKB->m_pMap[index];
			wxASSERT(pMap != NULL && (int)pMap->size() >= 0);
			if (pMap->size() == 0)
				continue; // could be some of the maps are empty
			else
			{
				MapKeyStringToTgtUnit::iterator iter;
				for (iter = pMap->begin(); iter != pMap->end(); ++iter) 
				{
					// test for a key of bad length (too small or too large)
					wxString key;
					key.Empty();
					CTargetUnit* pTgtUnit = (CTargetUnit*)NULL;
					key = iter->first; 
					pTgtUnit = iter->second;
					int len = key.Length();
					if (len < 1 || len > 512)
					{
						//IDS_BAD_KEY_LENGTH
						strError = strError.Format(_("Length of a source phrase has become bad.  (Length %d. In phrase \"%s\".  In list %d)"), len, key.c_str(), index + 1);
						errors.Add(strError);
						bFoundErrors = TRUE;
					}

					// test for valid association
					TUList* pList = pKB->m_pTargetUnits;
					TUList::Node* pos3 = pList->Find(pTgtUnit);
					if (pos3 == NULL)
					{
						// couldn't find the target unit in the CObList
						// IDS_BAD_ASSOCIATION
						strError = strError.Format(_("Bad association. The adaptation for the source phrase (\"%s\") was not found. (In list %d)"), key.c_str(), index + 1);
						errors.Add(strError);
						bFoundErrors = TRUE;
					}
				}
			}
		}
		/*
	}
	*/
//a:	// label not needed
	if (bFoundErrors)
		return FALSE;
	else
		return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxCommandEvent that is generated when the associated menu item is
///                         selected
/// \remarks
/// Called from: The Event Table when "Backup Knowledge Base" item is selected on the File Menu.
/// Backs up the regular or glossing KB. If gbIsGlossing is TRUE it calls DoGlossingKBBackup(), 
/// otherwise it calls DoKBBackup().
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnFileBackupKb(wxCommandEvent& WXUNUSED(event)) 
{
	if (gbIsGlossing)
		DoGlossingKBBackup(TRUE); // suppress the ok message
	else
		DoKBBackup(TRUE); // suppress the ok message
}


////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxUpdateUIEvent that is generated when the File Menu is about
///                         to be displayed
/// \remarks
/// Called from: The wxUpdateUIEvent mechanism when the associated menu item is selected, and before
/// the menu is displayed.
/// When gbIsGlossing is TRUE and m_bGlossingKBReady flag is TRUE the "Backup Knowledge Base" item on
/// the File menu is enabled, otherwise it is disabled. When gbIsGlossing is FALSE (using the normal KB), and
/// the m_bKBReady flag is TRUE the the "Backup Knowledge Base" item on the File menu is enabled,
/// otherwise it is disabled.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnUpdateFileBackupKb(wxUpdateUIEvent& event) 
{
	if (gbIsGlossing)
	{
		if (m_bGlossingKBReady)
			event.Enable(TRUE);
		else
			event.Enable(FALSE);
	}
	else
	{
		if (m_bKBReady)
			event.Enable(TRUE);
		else
			event.Enable(FALSE);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxCommandEvent that is generated when the associated menu item is
///                         selected
/// \remarks
/// Called from: The Event Table when "Check Knowledge Base" item is selected on the File Menu.
/// Checks the integrity of the active KB. If gbIsGlossing is TRUE it calls GlossingKBIntegrityCheck(), 
/// otherwise it calls KBIntegrityCheck().
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnFileCheckKb(wxCommandEvent& WXUNUSED(event)) 
{
	if (gbIsGlossing)
		GlossingKBIntegrityCheck();
	else
		KBIntegrityCheck();
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxUpdateUIEvent that is generated when the File Menu is about
///                         to be displayed
/// \remarks
/// Called from: The wxUpdateUIEvent mechanism when the associated menu item is selected, and before
/// the menu is displayed.
/// When gbIsGlossing is TRUE and m_bGlossingKBReady flag is TRUE the "Check Knowledge Base" item on
/// the File menu is enabled, otherwise it is disabled. When gbIsGlossing is FALSE (using the normal KB), and
/// the m_bKBReady flag is TRUE the the "Check Knowledge Base" item on the File menu is enabled,
/// otherwise it is disabled.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnUpdateFileCheckKb(wxUpdateUIEvent& event) 
{
	if (gbIsGlossing)
	{
		if (m_bGlossingKBReady)
			event.Enable(TRUE);
		else
			event.Enable(FALSE);
	}
	else
	{
		if (m_bKBReady)
			event.Enable(TRUE);
		else
			event.Enable(FALSE);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxUpdateUIEvent that is generated when the File Menu is about
///                         to be displayed
/// \remarks
/// Called from: The wxUpdateUIEvent mechanism when the associated menu item is selected, and before
/// the menu is displayed.
/// If the application is in Vertical Edit Mode, Free Translation Mode, or there is no valid document 
/// pointer (it is NULL), the "Restore Knowledge Base..." item on the File menu is always disabled 
/// and this event handler returns immediately.
/// As long as the active KB (glossing or regular) is in a ready state and there is at least one source 
/// phrase listed in m_pSourcePhrases, the "Restore Knowledge Base..." item on the File menu is enabled, 
/// otherwise it is disabled. 
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnUpdateFileRestoreKb(wxUpdateUIEvent& event) 
{
	if (gbVerticalEditInProgress)
	{
		event.Enable(FALSE);
		return;
	}
	if (m_bFreeTranslationMode)
	{
		event.Enable(FALSE);
		return;
	}
	CAdapt_ItDoc* pDoc = GetDocument();
	if (pDoc == NULL)
	{
		event.Enable(FALSE);
		return;
	}

	if ((!gbIsGlossing && m_bKBReady && (m_pSourcePhrases->GetCount() == 0)) ||
		(gbIsGlossing && m_bGlossingKBReady && (m_pSourcePhrases->GetCount() == 0)))
	{
		event.Enable(TRUE);
	}
	else
	{
		event.Enable(FALSE);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      bDoOnGlossingKB      -> check the glossing KB if TRUE, otherwise check the regular KB
/// \remarks
/// Called from: the App's DoGlossingKBBackup(), DoKBBackup(), GlossingKBIntegrityCheck(),
/// and KBIntegrityCheck().
/// Called only if a serious problem was detected in the active KB. After cleaning things up, it 
/// substitutes the backup copy of the active KB after detleting the currently (corrupted) KB.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::SubstituteKBBackup(bool bDoOnGlossingKB)
{
	CAdapt_ItDoc* pDoc = GetDocument();

	// clobber the bad knowledge base's structures in memory
	if (bDoOnGlossingKB)
	{
		pDoc->EraseKB(m_pGlossingKB);
		m_pGlossingKB = (CKB*)NULL;
	}
	else
	{
		pDoc->EraseKB(m_pKB);
		m_pKB = (CKB*)NULL;
	}

	// set up the m_curKBPath and m_altKBPath and backup ones, according
	// to the current value of the m_bSaveAsXML flag
	if (bDoOnGlossingKB)
	{
		pDoc->UpdateFilenamesAndPaths(FALSE,FALSE,FALSE,TRUE,TRUE);
	}
	else
	{
		pDoc->UpdateFilenamesAndPaths(TRUE,TRUE,TRUE,FALSE,FALSE);
	}

	// determine which path to use; since the KB or GlossingKB could be on disk in binary
	// or XML format from version 3 onwards
	wxString strUseThisPath;
	if (bDoOnGlossingKB)
	{
		if (::wxFileExists(m_curGlossingKBPath))
		{
			strUseThisPath = m_curGlossingKBPath;
		}
		else 
		{
			strUseThisPath = m_altGlossingKBPath;
		}
	}
	else
	{
		if (::wxFileExists(m_curKBPath))
		{
			strUseThisPath = m_curKBPath;
		}
		else 
		{
			strUseThisPath = m_altKBPath;
		}
	}

	// get rid of the knowledge base file
	wxString message;
	if (bDoOnGlossingKB)
		if (!::wxRemoveFile(m_curGlossingKBPath))
		{
			message = message.Format(_(
				"Warning: Did not remove bad knowledge base file %s."),
															m_curGlossingKBPath.c_str());
		}
	else
		if (!::wxRemoveFile(m_curKBPath))
		{
			message = message.Format(_(
				"Warning: Did not remove bad knowledge base file %s."),
															m_curKBPath.c_str());
		}
	wxMessageBox(message, _T(""), wxICON_ERROR);
	wxASSERT(FALSE);
	//AfxAbort();
	wxExit();

	// check to see if there is a backup KB available, & if so, rename if
	if (bDoOnGlossingKB)
	{
		if(::wxFileExists(m_curGlossingKBBackupPath) && !::wxDirExists(m_curGlossingKBBackupPath))
		{
			// attempt to rename the glossing backup file to the same name with .KB extension
			if (!::wxRenameFile(m_curGlossingKBBackupPath,m_curGlossingKBPath))
			{
				wxString message;
				message = message.Format(_(
"Error renaming glossing KB Backup file, path %s, to be the new glossing KB file."),
										m_curGlossingKBBackupPath.c_str());
				wxMessageBox(message, _T(""), wxICON_ERROR);
				wxASSERT(FALSE);
				wxExit();
			}
		}
		else
		{
			// the backup glossing KB does not exist, so all we can do is substitute an empty one
			// and save it to disk.
			wxASSERT(m_pGlossingKB == NULL);
			m_pGlossingKB = new CKB;
			wxASSERT(m_pGlossingKB != NULL);

			bool bOK = StoreGlossingKB(FALSE); // first time, so we can't make a backup
			if (bOK)
			{
				m_bGlossingKBReady = TRUE;
			}
			else
			{
				// IDS_STORE_KB_FAILURE
				wxMessageBox(_("Error: saving the knowledge base failed. The application will now close."), _T(""), wxICON_ERROR); // something went wrong
				wxASSERT(FALSE);
				wxExit();
			}
			return;
		}
	}
	else
	{
		if(::wxFileExists(m_curKBBackupPath) && !::wxDirExists(m_curKBBackupPath))
		{
			// attempt to rename the backup file to the same name with .KB extension
			if (!::wxRenameFile(m_curKBBackupPath,m_curKBPath))
			{
				wxString message;
				message = message.Format(_(
	"Error renaming KB Backup file with path %s to become the new KB file."),
										m_curKBBackupPath.c_str());
				wxMessageBox(message, _T(""), wxICON_ERROR);
				wxASSERT(FALSE);
				wxExit();
			}
		}
		else
		{
			// the backup KB does not exist, so all we can do is substitute an empty one
			// and save it to disk.
			wxASSERT(m_pKB == NULL);
			m_pKB = new CKB;
			wxASSERT(m_pKB != NULL);

			// store the language names in it
			m_pKB->m_sourceLanguageName = m_sourceName;
			m_pKB->m_targetLanguageName = m_targetName;

			bool bOK = StoreKB(FALSE); // first time, so we can't make a backup
			if (bOK)
			{
				m_bKBReady = TRUE;
			}
			else
			{
				// IDS_STORE_KB_FAILURE
				wxMessageBox(_("Error: saving the knowledge base failed. The application will now close."), _T(""), wxICON_ERROR); // something went wrong
				wxASSERT(FALSE);
				wxExit();
			}
			return;
		}
	}

	// now that we have a substitute KB made from the old backup, load it; but first
	// ensure it really can be found
	if (bDoOnGlossingKB)
	{
		if(::wxFileExists(m_curGlossingKBPath) && !::wxDirExists(m_curGlossingKBPath))
		{
			// the .KB file exists, so we need to create a CKB instance in
			// memory, open the .KB file on disk, and fill the memory instance's members
			wxASSERT(m_pGlossingKB == NULL);
			m_pGlossingKB = new CKB;
			wxASSERT(m_pGlossingKB != NULL);
			bool bOK = LoadGlossingKB();
			if (bOK)
				m_bGlossingKBReady = TRUE;
			else
			{
				// IDS_LOAD_GLOSSINGKB_FAILURE
				wxMessageBox(_("Error: loading the glossing knowledge base failed. The application will now close."), _T(""), wxICON_ERROR);
				wxASSERT(FALSE);
				wxExit();
			}
		}
		else
		{
			// it has to be there, so again something quick & diry since this should never happen
			wxMessageBox(
				_("Glossing KB file not found immediately after it was renamed!\n"), _T(""), wxICON_ERROR);
			wxASSERT(FALSE);
			wxExit();
		}
	}
	else
	{
		if(::wxFileExists(m_curKBPath) && !::wxDirExists(m_curKBPath))
		{
			// the .KB file exists, so we need to create a CKB instance in
			// memory, open the .KB file on disk, and fill the memory instance's members
			wxASSERT(m_pKB == NULL);
			m_pKB = new CKB;
			wxASSERT(m_pKB != NULL);
			bool bOK = LoadKB();
			if (bOK)
				m_bKBReady = TRUE;
			else
			{
				// IDS_LOAD_KB_FAILURE
				wxMessageBox(_("Error: loading a knowledge base failed. The application will now close."), _T(""), wxICON_ERROR);
				wxASSERT(FALSE);
				wxExit();
			}
		}
		else
		{
			// it has to be there, so again something quick & diry since this should never happen
			wxMessageBox(_("KB file not found immediately after it was renamed!\n"), _T(""), wxICON_ERROR);
			wxASSERT(FALSE);
			wxExit();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxUpdateUIEvent that is generated when the Tools Menu is about
///                         to be displayed
/// \remarks
/// Called from: The wxUpdateUIEvent mechanism when the associated menu item is selected, and before
/// the menu is displayed.
/// If the application is in Free Translation Mode, the "Load Consistent Changes..." item on the Tools 
/// menu is always disabled and this event handler returns immediately, otherwise the "Load Consistent 
/// Changes..." item on the Tools menu is enabled. 
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnUpdateLoadCcTables(wxUpdateUIEvent& event) 
{
	if (m_bFreeTranslationMode)
	{
		event.Enable(FALSE);
		return;
	}
	event.Enable(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxUpdateUIEvent that is generated when the Tools Menu is about
///                         to be displayed
/// \remarks
/// Called from: The wxUpdateUIEvent mechanism when the associated menu item is selected, and before
/// the menu is displayed.
/// If the application is in Free Translation Mode, the "Unload Consistent Changes..." item on the Tools 
/// menu is always disabled and this event handler returns immediately. If the m_bTablesLoaded 
/// flag is TRUE the "Unload Consistent Changes..." item on the Tools menu is enabled, otherwise it is
/// disabled. 
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnUpdateUnloadCcTables(wxUpdateUIEvent& event) 
{
	if (m_bFreeTranslationMode)
	{
		event.Enable(FALSE);
		return;
	}
	//Note: In the if (m_bCCTableLoaded) condition below m_bCCTableLoaded is an array, 
	// of booleans, and since the array name m_bCCTableLoaded is just a pointer the 
	// condition will be false if the array is NULL otherwise true, probably not what 
	// Bruce wanted. I'm changing this UpdateUI to use the m_bTablesLoaded boolean instead
	if (m_bTablesLoaded) 
		event.Enable(TRUE);
	else
		event.Enable(FALSE);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxCommandEvent that is generated when the associated menu item is
///                         selected
/// \remarks
/// Called from: the Tools menu when the "Load Consistent Changes..." menu item is selected.
/// This menu handler creates the four m_pConsistentChanger instances of the CConsistentChanger
/// class. It then creates a wxPropertySheet with 4 tabs, one for each potential changes
/// table, and shows it as a modal dialog to the user. If the user defines one or more .cct
/// tables for use by Adapt It, the table(s) is/are loaded by calling the appropriate 
/// m_pConsistentChanger's loadTableFromFile(path) method(s). Once one or more tables are
/// loaded the paths to the tables are stored/remembered and m_bTablesLoaded is set to TRUE. 
/// If an error is encountered in loading any one of the tables, the user is informed, and 
/// any subsequent tables are not loaded. Note: Simply loading consistent changes tables 
/// (.cct  files) does not turn on the consistent changes functionality. To turn on the consistent
/// changes functionality the user must also select the "Use Consistent Changes" toggle item in 
/// the Tools menu. Change tables remain loaded until the user either selects "Unload Consistent
/// Changes" from the Tools menu, or exits the application.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnToolsDefineCC(wxCommandEvent& WXUNUSED(event)) 
{
	// define and load one or more consistent change tables
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);
    // The wxWidgets version handles consistent changes directly via our CCModule which incapsulates the
    // CC functionality of the old cc32.dll dynamic library within the Adapt It executable, without
    // having to resort to calling any external dynamic library file. This approach has a number of
    // advantages including not having to install, locate and load a dynamic library file at run time.
    // Moreover, since the CCModule is a C++ class modified for wxWidgets, the CC functionality is
    // automatically available without need for further modifications for Windows, Linux and/or
    // Macintosh builds.
    // 
    // In the MFC version the Adapt It program itself cannot run at all unless the cc32.dll file is
    // present on the user's system. Although the wxWidgets framework has a wxDynamicLibrary class which
    // could be used to load external dynamic library files via its methods: Load(), IsLoaded(), and
    // Unload(), it makes more sense to do the extra effort of converting the CC functionality into the
    // CCModule class and thus avoid all the potential problems of dealing with external libraries. 
    // We can load, use, and unload .cct table files at will whenever the user selects the "Load 
    // Consistent Changes...", "Unload Consistent Changes", or the "Use Consistent Changes" menu
    // items from the Tools menu. 

    // Create the CConsistentChanger objects (a 4-element array of void* pointers was declared in the
    // App class. We create the objects here and store their pointers in the m_pConsistentChanger[]
    // array.
	// whm Note: Rather than creating all four consistent changer instances at this point, for the 
	// sake of better memory management and dealing with CCCModule's allocation of buffers and 
	// tables on the heap, we should only create an instance when the user actually loads one. 
	// See the "ShowModal() == wxID_OK" block below.
	// We can tell which instance has been created by examining the flags in the m_bCCTableLoaded[] 
	// array.
	
	CCCTabbedDialog ccPropertySheet(pApp->GetMainFrame());
	
	// make the font show only 12 point size in the dialog
	CopyFontBaseProperties(m_pSourceFont,m_pDlgSrcFont);
	m_pDlgSrcFont->SetPointSize(12);

	// show the property sheet
	if(ccPropertySheet.ShowModal() == wxID_OK)
	{
		for (int i=0; i<4; i++)
		{
			m_tableFolderPath[i] = ccPropertySheet.m_folderPath[i];
			m_tableName[i] = ccPropertySheet.m_tblName[i];
			// create consistent changer instances and consider loaded only those the user asked for
			if (m_tableName[i].IsEmpty())
				m_bCCTableLoaded[i] = FALSE;
			else
			{
				m_bCCTableLoaded[i] = TRUE;
				m_pConsistentChanger[i] = new CConsistentChanger;
			}
		}

		// load the tables
		wxString path = _T("");
		int whichOne = -1;

		wxString sError;
		sError.Empty();
		for (int j=0; j<4; j++)
		{
			if (m_bCCTableLoaded[j])
			{
				whichOne = j;
				path = m_tableFolderPath[j] + PathSeparator + m_tableName[j];
				sError = m_pConsistentChanger[j]->loadTableFromFile(path);
				if (!sError.IsEmpty())
					break; // don't load any more // whm added 19Jun07 - sError is checked below
			}
		}

		if (!sError.IsEmpty())
		{
			wxString addMore;
			// IDS_TABLES_LOADED_ERR
			addMore = addMore.Format(_("Warning: the consistent changes table number %d, and any further tables, were not loaded."),whichOne + 1);
			sError += addMore;
			wxMessageBox(sError, _T(""), wxICON_INFORMATION);
			// later ones won't be loaded once the error has occurred, so ensure that
			// the non-loaded ones are not tried
			switch (whichOne)
			{
			case 0:
				m_bCCTableLoaded[0] = m_bCCTableLoaded[1] = m_bCCTableLoaded[2] = 
															m_bCCTableLoaded[3] = FALSE;
				break;
			case 1:
				m_bCCTableLoaded[1] = m_bCCTableLoaded[2] = m_bCCTableLoaded[3] = FALSE;
				break;
			case 2:
				m_bCCTableLoaded[2] = m_bCCTableLoaded[3] = FALSE;
				break;
			case 3:
				m_bCCTableLoaded[3] = FALSE;
				break;
			default:
				wxASSERT(FALSE); // this would only be a programming error
				wxExit();
			}
		}

		// our algorithm above allows processing to proceed even if an exception was thrown,
		// the relevant table, and any subsequent ones, just won't be used; but the user will 
		// have been alerted
		if (m_bCCTableLoaded[0] || m_bCCTableLoaded[1] || m_bCCTableLoaded[2] 
																	|| m_bCCTableLoaded[3])
			m_bTablesLoaded = TRUE;
		else
			m_bTablesLoaded = FALSE;

		// force the Use ConsistentChanges checkbox to ON, if it is not ON already
		if (m_bTablesLoaded && !pApp->m_bUseConsistentChanges) // whm 23Jun07 added m_bTablesLoaded &&
		{
			wxCommandEvent event;
			GetView()->OnUseConsistentChanges(event);
		}
	}

	int len = pApp->m_targetPhrase.Length();
	gnStart = len;
	gnEnd = len;
	pApp->m_pTargetBox->SetSelection(len,len);
	pApp->m_pTargetBox->SetFocus();
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxCommandEvent that is generated when the associated menu item is
///                         selected
/// \remarks
/// Called from: the Tools menu when the "Unload Consistent Changes" menu item is selected.
/// This menu handler empties the m_tableName array, sets the m_bCCTableLoaded array flags
/// to FALSE, and changes the interface globals and menu items to their "off" state, 
/// effectively "unloading" the consistent changes functionality.
/// To turn on the consistent changes functionality again, the user must again select the 
/// "Load Consistent Changes..." menu item, load the .cct changes files, and then select the
/// "Use Consistent Changes" toggle item in the Tools menu.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnToolsUnloadCcTables(wxCommandEvent& WXUNUSED(event)) 
{

	m_tableName[0] = m_tableName[1] = m_tableName[2] = m_tableName[3] = _T("");
	m_bCCTableLoaded[0] = m_bCCTableLoaded[1] = m_bCCTableLoaded[2] = m_bCCTableLoaded[3]
																		= FALSE;
	m_bTablesLoaded = FALSE;
	// leave m_folderPath[] unchanged, so these can function as default folders for accessing
	// tables if the user invokes Define Consistent Changes... menu item again

	// turn off the use of cc tables
	if (m_bUseConsistentChanges)
	{
		wxCommandEvent cevent;
		GetView()->OnUseConsistentChanges(cevent);
	}

	// in case we are in auto insert mode, we should stop at this point so that user can take
	// stock of the situation and decide if he wants to change any other mode flags
	m_bAutoInsert = FALSE;
	// the above FALSE value is incompatible with m_bAcceptDefaults being TRUE, so ensure that
	// is turned off too
	m_bAcceptDefaults = FALSE;

	// turn the checkmarks off
	CMainFrame *pFrame = GetMainFrame();
	wxASSERT(pFrame != NULL);
	wxMenuBar* pMenuBar = pFrame->GetMenuBar(); 
	wxASSERT(pMenuBar != NULL);
	wxMenuItem * pToolsUseCC = pMenuBar->FindItem(ID_USE_CC);
	wxASSERT(pToolsUseCC != NULL);
	pToolsUseCC->Check(FALSE);
	wxMenuItem * pToolsAcceptChanges = pMenuBar->FindItem(ID_ACCEPT_CHANGES);
	wxASSERT(pToolsAcceptChanges != NULL);
	pToolsAcceptChanges->Check(FALSE);

	// ensure the "Accept changes without stopping" flag is FALSE
	m_bAcceptDefaults = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: the App's SaveDocChanges() and from CMainFrame's OnIdle.
/// Calls the Doc's DoFileSave() method, and updates the times for auto saving operations.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::DoAutoSaveDoc()
// with additions for handling glossing versus adapting (ie. to get the stuff in the
// phrase box into the appropriate KB before the save is done)
{
	bool bOkay;
	bOkay = GetDocument()->DoFileSave();

	// update the time it was last saved
	wxDateTime time = wxDateTime::Now();
	m_timeSettings.m_tLastDocSave = time;
	m_timeSettings.m_tLastKBSave = time;
	nSequNumForLastAutoSave = nCurrentSequNum;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: CMainFrame's OnIdle.
/// Calls the View's DoFileSaveKB() method, and updates the time last saved.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::DoAutoSaveKB()
{
	GetView()->DoFileSaveKB();

	// update the time last saved
	wxDateTime time = wxDateTime::Now();
	m_timeSettings.m_tLastKBSave = time;
}

/////////////////////////////////////////////////////////////////////////////
// MRU file list default implementation -- copied from MFC folder's appui.cpp
// in case I want to develop a smarter override based on it sometime
/*
BOOL CWinApp::OnOpenRecentFile(UINT nID)
{
	ASSERT(m_pRecentFileList != NULL);

	ASSERT(nID >= ID_FILE_MRU_FILE1);
	ASSERT(nID < ID_FILE_MRU_FILE1 + (UINT)m_pRecentFileList->GetSize());
	int nIndex = nID - ID_FILE_MRU_FILE1;
	ASSERT((*m_pRecentFileList)[nIndex].GetLength() != 0);

	TRACE(traceAppMsg, 0, _T("MRU: open file (%d) '%s'.\n"), (nIndex) + 1,
			(LPCTSTR)(*m_pRecentFileList)[nIndex]);

	if (OpenDocumentFile((*m_pRecentFileList)[nIndex]) == NULL)
		m_pRecentFileList->Remove(nIndex);

	return TRUE;
}
*/
/////////////////////////////////////////////////////////////////////////////

//BOOL CAdapt_ItApp::OnOpenRecentFile(UINT nID)
// wx version Note: The wx doc/view framework uses OnMRUFile() as a virtual method
// of wxDocParentFrame. It is now a handler in CMainFrame, renamed to OnMRUFile(), 
// and is a void function.
//bool CAdapt_ItApp::OnOpenRecentFile(wxCommandEvent& event)

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxCommandEvent that is generated when the associated menu item is
///                         selected
/// \remarks
/// Called from: The Event Table when "Restore Knowledge Base..." item is selected on the File Menu.
/// Restores the Knowledge Base by discarding the current knowledge base, and then using the user's
/// saved document files, it rebuilds a new KB (or glossing KB if that mode is current). The restore 
/// first processes files in the current project's Adaptations folder, after giving the user the 
/// option of choosing which files to process via the "Which Files" dialog. Then, if book folder mode 
/// was ever used, it processes all of files found in book folders automatically (without asking the 
/// user). At the end of the process, it reports how many words and phrases, and how many documents
/// it processed.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnFileRestoreKb(wxCommandEvent& WXUNUSED(event))
// Modified to work for either the glossing KB, or the adaptations KB
// BEW changed 05Sept05 to work on contents not just of the Adaptations folder
// but also the contents of the set of all Bible book folders in the Adaptations
// folder (if book mode has at some previous time been turned on)
{
	// BEW added 05Jan07 to enable work folder on input to be restored when done
	wxString strSaveCurrentDirectoryFullPath = GetDocument()->GetCurrentDirectory();

	// IDS_RESTORE_KB_MSG
	int value = wxMessageBox(_("Use this command if the knowledge base has become corrupted.\n(To restore a glossing knowledge base, turn glossing ON and then choose this command again.)\nIt will throw away your current knowledge base, and then using your saved document files it will build a new one.\n\nDo you wish to go ahead?"), _T(""), wxYES_NO);
	if (!(value == wxYES))
		return;

	bool bOK;
	wxArrayString* pList = &m_acceptedFilesList;
	int nCount;

	// the 'accepted' list holds the document filenames to be used
	pList->Clear(); // ensure it starts empty

	CAdapt_ItDoc* pDoc = GetDocument();
	wxASSERT(pDoc);

	// get whichever KB is to be worked on
	CKB* pKB;
	if (gbIsGlossing)
		pKB = m_pGlossingKB;
	else
		pKB = m_pKB;

	// recover as many m_bAlwaysAsk flag = TRUE instances as possible from the corrupted KB
	bool bRescueFlags = FALSE;
	value = wxMessageBox(_("Adapt It can also try to rescue your settings for the \"Force Choice For This Item\" checkbox,\nbut it might result in a harmless crash. (If so, just run Adapt It again and take the \"No\" option.)\n\nDo you wish to try this extra rescue?"), _T(""), wxYES_NO);
	if (value == wxYES)
		bRescueFlags = TRUE;
	KPlusCList keys;
	if (bRescueFlags)
	{
		GetForceAskList(pKB,&keys);
	}

	// clear out the KB, actually erase it & create a new empty one & store it ready 
	// for filling
	ClearKB(pDoc); // checks gbIsGlossing and clears whichever KB needs clearing

	// the pointer will be a new one because of ClearKB( ) call, so update pKB to point to it
	if (gbIsGlossing)
		pKB = m_pGlossingKB;
	else
		pKB = m_pKB;

	// get a pointer to the view (since it's SDI interface, our implementation
	// has the view always present, as the doc and its template get reused)
	CAdapt_ItView* pView = GetView();
	wxASSERT(pView);

	// save entry state (only necessary if entry state had book mode on)
	BookNamePair* pSave_BookNamePair = m_pCurrBookNamePair;
	int nSave_BookIndex = m_nBookIndex;
	wxString save_bibleBooksFolderPath = m_bibleBooksFolderPath;

	// determine whether or not there are Bible book folders in the Adaptations folder;
	// if there are book folders then we have to process all the documents in each
	// such folder after processing the ones in Adaptations
	// whm note: AreBookFoldersCreated() has the side effect of changing the current work directory
	// to the passed in m_curAdaptionsPath.
	gbHasBookFolders = AreBookFoldersCreated(m_curAdaptionsPath);

	// lock view window updates till done
	pView->canvas->Freeze();

	int nTotal = 0;
	int nCumulativeTotal = 0;
	int nDocCount = 0;

	// handle the Adaptations folder's files first - for these, we allow the IDD_WHICH_FILES
	// dialog to show and the user can remove some files from the processing using it if he
	// wishes, but for looping across all the book folders, we'll suppress the dialog so that
	// all doc files in the book folders get processed
	// whm note: EnumerateDocFiles() has the side effect of changing the current work directory
	// to the passed in m_curAdaptionsPath.
	bOK = EnumerateDocFiles(pDoc, m_curAdaptionsPath);
	nCount = pList->GetCount(); // the count of doc files (could be zero if all docs are in
								// Bible book folders only)
	nDocCount += nCount;
	wxASSERT(nCount >= 0);
	if (bOK)
	{
		// if there are no documents yet, a restore or other operation requiring one of more
		// document files is impossible, so then return to caller with a FALSE value
		if (m_acceptedFilesList.GetCount() == 0 && !gbHasBookFolders)
		{
			// IDS_NO_DOCUMENTS_YET
			wxMessageBox(_("Sorry, there are no saved document files yet for this project. At least one document file is required for the operation you chose to be successful. The command will be ignored."),_T(""), wxICON_EXCLAMATION);
			return;
		}
		// there is at least one document, so do the restore
		DoKBRestore(pKB, nCount, nTotal, nCumulativeTotal);
	}

	if (gbHasBookFolders)
	{
		// process this block only if the project's Adaptations folder contains the set of
		// Bible book folders - these could contain documents, and some or all could be empty; 
		// NOTE: the code below is smart enough to ignore any user-created folders which are sisters
		// of the Bible book folders for which the Adaptations folder is the common parent folder
		wxDir finder; //CFileFind finder;
		bool bOK = (::wxSetWorkingDirectory(m_curAdaptionsPath) && finder.Open(m_curAdaptionsPath)); // wxDir must call .Open() before enumerating files!
		if (!bOK)
		{
			wxString s1, s2, s3;
			s1 = _T(
			"Failed to set the current directory to the Adaptations folder in OnFileRestoreKb function, ");
			s2 = _T("processing book folders, so the book folder document files do not contribute to the rebuild.");
			s3 = s3.Format(_T("%s%s"),s1.c_str(),s2.c_str());
			wxMessageBox(s3,_T(""), wxICON_EXCLAMATION);
			return;
		}
		else
		{
			wxString str;
			bool bWorking = finder.GetFirst(&str,_T("*.*"),wxDIR_FILES | wxDIR_DIRS);
			while (bWorking)
			{
				bWorking = finder.GetNext(&str);
				if (str.IsEmpty())
					continue;

				// whm note: in the MFC version's "if (finder.IsDirectory())" test below, the finder continues
				// to use the directory path that was current when the inital finder.FindFile call was made
				// above, even though the EnumerateDocFiles() call below changes the current working dir for
				// each of the book folder directories it processes. In the wx version the finder.Exists(str)
				// call uses whatever the current working directory is and checks for a sub-directory "str"
				// below that - a difference we must account for here in the wx version.
				if (finder.Exists(m_curAdaptionsPath + PathSeparator + str)) //if (finder.IsDirectory()) // returns true if str is a dir
				{
					// User-defined folders can be in the Adaptations folder without making the
					// app confused as to whether or not Bible Book folders are present or not

					// we have found a folder, check if it matches one of those in
					// the array of BookNamePair structs (using the seeName member)
					if (IsDirectoryWithin(str,m_pBibleBooks))
					{
						// we have found a folder name which belongs to the set of
						// Bible book folders, so construct the required path to the
						// folder and enumerate is documents then call
						// DoTransformationsToGlosses() to process any documents within
						wxString folderPath = m_curAdaptionsPath;
						folderPath += PathSeparator + str;

						// clear the string list of directory names & then enumerate the
						// directory's file contents; the EnumerateDocFiles() call sets the
						// current directory to the one given by folderPath (ie. to a book folder)
						// so after the DoKBRestore() call, which relies on that directory being
						// current, we must call ::SetCurrentDirectory(m_curAdaptionsPath) again
						// so that this outer look which iterates over directories continues correctly
						pList->Clear();
						// whm note: EnumerateDocFiles() has the side effect of changing the current work directory
						// to the passed in folderPath.
						bOK = EnumerateDocFiles(pDoc, folderPath, TRUE); // TRUE == suppress dialog
						
						if (!bOK)
						{
							// don't process any directory which gives an error, but continue looping --
							// this is a highly unlikely error, so an English message will do
							wxString errStr;
							errStr = errStr.Format(_T("Error returned by EnumerateDocFiles in Book Folder loop, directory %s skipped."),
								folderPath.c_str());
							wxMessageBox(errStr,_T(""), wxICON_EXCLAMATION);
							continue;
						}
						nCount = pList->GetCount();
						nDocCount += nCount;
						if (pList->GetCount() == 0)
						{
							// no documents to work on in this folder, so iterate
							continue;
						}

						// There are files to be processed. TRUE parameter suppresses the statistics dialog.
						DoKBRestore(pKB, nCount, nTotal, nCumulativeTotal);
						// whm note: The following statement that sets the current working directory will
						// be skipped when there are no files within any folder with the result that the
						// OnFileRestoreKb will exit with the current working directory set to "...\Zechariah"
						// TODO: check if the following statement should be moved to near the end of OnFileRestoreKb.
						bOK = (::wxSetWorkingDirectory(m_curAdaptionsPath) && finder.Open(m_curAdaptionsPath)); // restore parent folder as current
						wxASSERT(bOK);
					}
					else
					{
						continue;
					}
				}
				else
				{
					// its a file, so ignore it
					continue;
				}
			} // end loop for FindFile() scanning all possible files in folder
		}  // end block for bOK == TRUE
	} // end block for test for gbHasBookFolders yielding TRUE
	
	if (bRescueFlags)
	{
		// do the rescue, for either glossing KB or the adaptation KB
		RestoreForceAskSettings(pKB,&keys); // also cleans up after itself, 
											  // deleting the list contents
	}

	// do the KB backup, if the relevant flag is set
	if(m_bAutoBackupKB)
	{
		if (gbIsGlossing)
			DoGlossingKBBackup(TRUE);
		else
			DoKBBackup(TRUE);
	}

	// inform user of success and some statistics
	wxString stats;
	// IDS_RESTORE_KB_STATS
	stats = stats.Format(_("Your rebuilt knowledge base is now in operation. It contains %d source words and phrases taken from %d  documents."),nCumulativeTotal,nDocCount);
	wxMessageBox(stats);

	// let the view respond again to updates
	pView->canvas->Thaw();

	// clean up the list
	m_acceptedFilesList.Clear();

	// restore the former book mode parameters (even if book mode was not on on entry)
	m_pCurrBookNamePair = pSave_BookNamePair;
	m_nBookIndex = nSave_BookIndex;
	m_bibleBooksFolderPath = save_bibleBooksFolderPath;
	// now, if the user opens the Document tab of the Start Working wizard, and book
	// mode is on, then at least the path and index and book name are all consistent

	// make sure that book mode is off if there is no valid folder path
	if (m_bBookMode)
	{
		if (m_bibleBooksFolderPath.IsEmpty())
		{
			// no path defined, so having the mode on would lead to a crash, 
			// so turn it off (a manual turn on subsequently will get all
			// set up correctly)
			m_bBookMode = FALSE;
			m_nBookIndex = -1;
			m_nDefaultBookIndex = 39;
			m_nLastBookIndex = 39;
		}
	}

	// BEW added 05Jan07 to restore the former current working directory
	// to what it was on entry
	bOK = ::wxSetWorkingDirectory(strSaveCurrentDirectoryFullPath);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if all was okay, FALSE if the path to the folder could not be made the current
///				directory, or if the user cancelled the Which Files dialog
/// \param      pDoc        -> pointer to the document class (unused)
/// \param      folderPath  -> a wxString representing the full path to the folder which is to have
///                             its document files enumerated
/// \param      bSuppressDialog -> default is FALSE (ie. the IDD_WHICH_FILES dialog is visible), but if
///						          set TRUE then the dialog is suppressed (and all document files in the
///						          passed in folderPath are left in m_acceptedFilesList for the caller to use)
/// \remarks
/// Called from: the App's OnFileRestoreKb(), AccessOtherAdaptionProject(), the View's 
/// OnEditConsistencyCheck(), OnRetransReport(), and SyncScrollReceive() in MainFrm.cpp.
/// We only want to suppress the IDD_WHICH_FILES dialog when iterating through the 67 folders
/// of the Bible books - when these are present in the Adaptations folder, since having to
/// manually interact with the dialog 67 times would be grossly tedious for the user, and the
/// need to exclude a document in one of the folders would be pretty well zero.
/// However, we will also suppress showing the dialog if there are no files in the string list,
/// because this could well be the situation when there are book folders in the Adaptations
/// folder and all the documents are in them and none are in Adaptations itself.
/// Another context for suppressing the dialog is when showing lists of files
/// in the split or join or move dialog; also when scanning xml documents after
/// having received a scripture reference message.
/// Note: EnumerateDocFiles() has the side effect of changing the current work directory
/// to the passed in folderPath.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::EnumerateDocFiles(CAdapt_ItDoc* WXUNUSED(pDoc), wxString folderPath, bool bSuppressDialog)
{
	// set a local var for the Adaptations folder's path, or book folder path - whichever
	// was passed in
	wxASSERT(!folderPath.IsEmpty());
	bool bOK = ::wxSetWorkingDirectory(folderPath);
	if (!bOK)
	{
		// something's real wrong!
		wxMessageBox(_T(
		"Failed to set the current directory to the passed in folder. Command aborted."), _T(""),
			wxICON_EXCLAMATION);
		return FALSE;
	}

	// enumerate the document files in the Adaptations folder or the current book folder; and
	// note that internally GetPossibleAdaptionDocuments excludes any files with names of the
	// form *.BAK.xml (these are backup XML document files, and for each there will be present
	// an *.xml file which has identical content -- it is the latter we enumerate) and also note
	// the result could be an empty m_acceptedFilesList, but have the caller of EnumerateDocFiles
	// check it for no entries in the list
	GetPossibleAdaptionDocuments(&m_acceptedFilesList, folderPath);

	if (m_acceptedFilesList.GetCount() > 0)
	{
		if (!bSuppressDialog)
		{
			CWhichFilesDlg dlg(GetMainFrame());
			dlg.Centre();
			if (dlg.ShowModal() == wxID_OK)
			{
				return TRUE;
			}
			else
			{
				return FALSE; // user cancelled the dialog, typically this constitutes a wish to
							// abort the operation - so return FALSE so that the caller can
							// use the FALSE value if it wants to effect the operation abort
			}
		}
	}
	return TRUE; // return TRUE even if the list is empty, because the caller may wish not to abort
				 // the operation, but instead attempt it on another folder of files, so we need
				 // to allow different functionalities to interpret an empty list in different ways
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pKB     -> pointer to the affected KB (can be glossing or the regular KB)
/// \param      pKeys   <- pointer to the list of keys that is produced
/// \remarks
/// Called from: the App's OnFileRestoreKb().
/// Gets a list of the keys which have a unique translation and for which the CTargetUnit instance's
/// m_bAlwaysAsk attribute is set to TRUE. This function is only called when a corrupted KB was
/// detected, so we cannot expect to be able to recover all such keys, but as many as we can, which
/// will reduce the amount of manual setting of the flags in the KB editor after a KB restore is done.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::GetForceAskList(CKB* pKB, KPlusCList* pKeys) // MFC has CPtrList
// get a list of the keys which have a unique translation and for which the
// CTargetUnit instance's m_bAlwaysAsk attribute is set TRUE. (Since we are using this
// with a probably corrupted KB, we cannot expect to be able to recover all such keys,
// and we will have to check carefully for corrupt pointers, etc. We will recover as
// many as we can, which will reduce the amount of the user's manual setting of the flag
// in the KB editor, after the KB restore is done.)
// For glossing being ON, this function should work correctly without any changes needed,
// provided the caller gives the pointer to the glossing KB as the first parameter.
{
	wxASSERT(pKeys->IsEmpty());
	for (int i=0; i < MAX_WORDS; i++)
	{
		MapKeyStringToTgtUnit* pMap = pKB->m_pMap[i];
		if (!pMap->empty())
		{
			MapKeyStringToTgtUnit::iterator iter;
			for (iter = pMap->begin(); iter != pMap->end(); ++iter)
			{
				wxString key;
				key.Empty();
				CTargetUnit* pTU = (CTargetUnit*)NULL;
				try
				{
					key = iter->first; 
					pTU = iter->second;
					if (key.Length() > 360)
						break; // almost certainly a corrupt key string, so exit the map

					// we have a valid pointer to a target unit, so see if the flag is set
					// and if the translation is unique
					if (pTU->m_pTranslations->GetCount() == 1)
					{
						if (pTU->m_bAlwaysAsk)
						{
							// we've found a setting which is to be preserved, so do so
							KeyPlusCount* pKPlusC = new KeyPlusCount;
							wxASSERT(pKPlusC != NULL);
							pKPlusC->count = i+1; // which map we are in
							pKPlusC->key = key;
							pKeys->Append(pKPlusC); // add it to the list
						}
					}	
				}
				catch (...)
				{
					break; // exit from this map, as we no longer can be sure of a valid pos
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pKB     -> pointer to the affected KB (can be glossing or the regular KB)
/// \param      pKeys   -> pointer to the list of keys that with m_bAlwaysAsk TRUE
/// \remarks
/// Called from: the App's OnFileRestoreKb().
/// Uses the list of the keys which have a unique translation and for which the CTargetUnit instance's
/// m_bAlwaysAsk attribute was set to TRUE (the list was previously generated by a call to 
/// the App's GetForceAskList() function), and insures that the appropriate target unit's m_bAlwaysAsk
/// member is set to TRUE.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::RestoreForceAskSettings(CKB* pKB, KPlusCList* pKeys)
// Modified for glossing KB use - no code change needed, just make sure the caller sends
// the pointer to the glossing KB as the first parameter. Modified (July 2003) for support
// of Auto-Capitalization.
{
	wxASSERT(pKeys);
	wxASSERT(pKB);
	if (pKeys->IsEmpty())
		return;
	KPlusCList::Node *node = pKeys->GetFirst();
	while (node)
	{
		KeyPlusCount* pK = (KeyPlusCount*)node->GetData();
		wxASSERT(pK != NULL);
		int index = pK->count - 1; // index to map
		MapKeyStringToTgtUnit* pMap = pKB->m_pMap[index];
		wxASSERT(pMap);
		CTargetUnit* pTU = (CTargetUnit*)NULL;
		// when the RestoreForceAskSettings function is called, the KB (glossing or adaptation)
		// will have just been recreated from the documents. This recreation will be done with
		// auto-capitalization either On or Off, with differing results, so the lookup done in
		// the next line has to also be conformant with the current value of the gbAutoCaps flag.
		bool bOK = GetView()->AutoCapsLookup(pMap,pTU,pK->key); //bool bOK = GetView()->AutoCapsLookup(pMap,(wxObject*&)pTU,pK->key);
		if (bOK)
		{
			wxASSERT(pTU);
			pTU->m_bAlwaysAsk = TRUE; // restore the flag setting
		}
		else
		{
			// this error is unlikely to occur, so we can leave it in English
			wxString str;
			str = str.Format(_T(
"Error (non fatal): did not find an entry for the key:  %s  in the map with index %d\n"),
				pK->key.c_str(), pK->count - 1);
			wxMessageBox(str, _T(""), wxICON_EXCLAMATION);
		}
		delete pK; // no longer needed, so ensure we don't leak memory
		pK = (KeyPlusCount*)NULL;

		node = node->GetNext();
	}
	pKeys->Clear(); // get rid of the now hanging pointers
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pKB               -> pointer to the affected KB
/// \param      nCount            <- the count of files in the folder being processed
/// \param      nTotal            <- the total source phrase instances in the doc being processed
/// \param      nCumulativeTotal  <- the accumulation of the nTotal values over all documents processed
///                                     so far
/// \remarks
/// Called from: the App's OnFileRestoreKb(). 
/// Iterates over the document files of a project, opens them and uses the data stored within
/// them to regererate new Knowledge Base file(s) from those documents.
/// This function assumes that the current directory will have already been set correctly
/// before being called. Modified to support restoration of either the glossing KB, or the
/// adaptation KB - which one gets the restoration depends on the gbIsGlossing flag's value.
/// Allows for the possibility that Bible book folders may be in the Adaptations folder.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::DoKBRestore(CKB* pKB, int& nCount, int& nTotal, int& nCumulativeTotal)
{
	wxArrayString* pList = &m_acceptedFilesList;
	CAdapt_ItDoc* pDoc = GetDocument();
	CAdapt_ItView* pView = GetView();
	wxASSERT(pDoc);
	wxASSERT(pView);
	
	// iterate over the document files
	int i;
	for (i=0; i < nCount; i++)
	{
		wxString newName = pList->Item(i);
		wxASSERT(!newName.IsEmpty());

		bool bOK;
		bOK = pDoc->OnOpenDocument(newName);
		// The docview sample has a routine called SetFileName() that it uses
		// to override the automatic associations of file name/path of a doc
		// with a view. The wxDocument member that holds the file name/path
		// is m_documentFile. I've put the SetFileName() routine in the Doc
		// so we can do this like MFC has it.
		pDoc->SetFilename(newName,TRUE); // here TRUE means "notify the views" whereas
		// in the MFC version TRUE meant "add to MRU list"

		nTotal = m_pSourcePhrases->GetCount();
		wxASSERT(nTotal > 0);
		nCumulativeTotal += nTotal;

		// put up a progress indicator
#ifdef __WXMSW__
		wxString progMsg = _("%s  - %d of %d Total words and phrases");
		wxString msgDisplayed = progMsg.Format(progMsg,newName.c_str(),1,nTotal);
		wxProgressDialog progDlg(_("Restoring the Knowledge Base"),
								msgDisplayed,
								nTotal,    // range
								GetMainFrame(),   // parent
								//wxPD_CAN_ABORT |
								//wxPD_CAN_SKIP |
								wxPD_APP_MODAL |
								// wxPD_AUTO_HIDE | -- try this as well
								wxPD_ELAPSED_TIME |
								wxPD_ESTIMATED_TIME |
								wxPD_REMAINING_TIME
								| wxPD_SMOOTH // - makes indeterminate mode bar on WinXP very small
								);
#else
		// wxProgressDialog tends to hang on wxGTK so I'll just use the simpler CWaitDlg
		// notification on wxGTK and wxMAC
		// put up a Wait dialog - otherwise nothing visible will happen until the operation is done
		CWaitDlg waitDlg(gpApp->GetMainFrame());
		// indicate we want the reading file wait message
		waitDlg.m_nWaitMsgNum = 5;	// 5 hides the static leaving only "Please wait..." in title bar
		waitDlg.Centre();
		waitDlg.Show(TRUE);
		waitDlg.Update();
		// the wait dialog is automatically destroyed when it goes out of scope below.
#endif
		SPList* pPhrases = m_pSourcePhrases;
		SPList::Node* pos1 = pPhrases->GetFirst();
		int counter = 0;
		while (pos1 != NULL)
		{
			CSourcePhrase* pSrcPhrase = (CSourcePhrase*)pos1->GetData();
			pos1 = pos1->GetNext();
			counter++;

			// update the glossing or adapting KB for this source phrase
			pView->RedoStorage(pKB,pSrcPhrase);

#ifdef __WXMSW__
			// update the progress bar every 20th iteration
			if (counter % 1000 == 0) //if (20 * (counter / 20) == counter)
			{
				msgDisplayed = progMsg.Format(progMsg,newName.c_str(),counter,nTotal);
				progDlg.Update(counter,msgDisplayed);
			}
#endif
		}

		GetView()->ClobberDocument();

		// delete the buffer containing the filed-in source text
		if (m_pBuffer != NULL)
		{
			delete m_pBuffer;
			m_pBuffer = (wxString*)NULL;
		}

		// wx version note: Since the progress dialog is modeless we do not need to destroy
		// or otherwise end its modeless state; it will be destroyed when DoKBRestore goes
		// out of scope

		// save the KB now that we have finished this document file (FALSE = no backup wanted)
		bool bSavedOK;
		if (gbIsGlossing)
			bSavedOK = SaveGlossingKB(FALSE);
		else
			bSavedOK = SaveKB(FALSE);
		if (!bSavedOK)
		{
			wxMessageBox(_("Warning: something went wrong doing a save of the KB"), _T(""),
									wxICON_INFORMATION);
		}
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pDoc    -> pointer to the document
/// \remarks
/// Called from: the App's OnFileRestoreKb().
/// Calls the Doc's EraseKB() on the appropriate KB, then creates a new, empty KB in its place.
/// It also stores the language names in the memory instance of the knowledge base, and checks
/// the KB is ready for use.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::ClearKB(CAdapt_ItDoc *pDoc)
{
	wxASSERT(pDoc != NULL);

	if (gbIsGlossing) // doing glossing
	{
		// erase the current GlossingKB in memory & delete the memory if occupies
		pDoc->EraseKB(m_pGlossingKB); // leaves m_bGlossingKBReady unchanged

		//create a new KB
		m_pGlossingKB = new CKB;
		wxASSERT(m_pGlossingKB != NULL);

		// check it is ready for use
		wxASSERT(m_bGlossingKBReady);
	}
	else // doing adaptation
	{
		// erase the current KB in memory & delete the memory if occupies
		pDoc->EraseKB(m_pKB); // leaves m_bKBReady unchanged

		//create a new KB
		m_pKB = new CKB;
		wxASSERT(m_pKB != NULL);

		// store the language names in the memory instance of the knowledge base
		wxASSERT(!m_sourceName.IsEmpty());
		wxASSERT(!m_targetName.IsEmpty());
		m_pKB->m_sourceLanguageName = m_sourceName;
		m_pKB->m_targetLanguageName = m_targetName;

		// check it is ready for use
		wxASSERT(m_bKBReady);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      fi   -> the fontInfo struct containing the font information for writing in the
///                         configuration files
/// \param      pf   <- pointer to the wxTextFile that is being written as the config file
/// \remarks
/// Called from: the App's WriteConfigurationFile(). 
/// Writes the font configuration part of a configuration file. WriteFontConfiguration() gets
/// called once for each of the three main fonts used: source language, target language, and
/// navigation text language. The text file is opened by the caller and remains open after each
/// call to WriteFontConfiguration(). Note: wxTextFile is processed entirely in memory, so it
/// is up to the caller to actually write the information to the file by calling pf->Write()
/// once the in-memory file has been fully composed/processed.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::WriteFontConfiguration(const fontInfo fi, wxTextFile* pf)
{
	// Note: wxTextFile is Unicode enabled when _UNICODE is defined
	wxString data = _T("");
	wxString tab = _T("\t");
	wxString number;

#ifndef _UNICODE // ANSI or _MBCS version

	data.Empty();
	data << szHeight << tab << fi.fHeight; //data = szHeight + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szWidth << tab << fi.fWidth; //data = szWidth + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szEscapement << tab << fi.fEscapement;//data = szEscapement + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szOrientation << tab << fi.fOrientation; //data = szOrientation + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szWeight << tab << fi.fWeightConfSave; //data = szWeight + tab + number + end;
	pf->AddLine(data);

	if (fi.fItalic)
		number = _T("1");
	else
		number = _T("0"); 
	data.Empty();
	data << szItalic << tab << number; //data = szItalic + tab + number + end;
	pf->AddLine(data);

	if (fi.fUnderline)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szUnderline << tab << number; //data = szUnderline + tab + number + end;
	pf->AddLine(data);

	if (fi.fStrikeOut)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szStrikeOut << tab << number; //data = szStrikeOut + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szFontEncoding << tab << fi.fEncoding; //data = szCharSet + tab + number + end;
	pf->AddLine(data); // whm added 14Feb07

	data.Empty();
	data << szCharSet << tab << fi.fCharset; //data = szCharSet + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szOutPrecision << tab << fi.fOutPrecision; //data = szOutPrecision + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szClipPrecision << tab << fi.fClipPrecision; //data = szClipPrecision + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szQuality << tab << fi.fQuality; //data = szQuality + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szPitchAndFamily << tab << fi.fPitchAndFamily; //data = szPitchAndFamily + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szFaceName << tab << fi.fFaceName; //data = szFaceName + tab + number + end;
	pf->AddLine(data);

	int fntColor = 0; // default black until changed below
	if (fi.fLangType.GetChar(0) == _T('S'))
		fntColor = WxColour2Int(m_sourceColor); // WxColour2Int() in helpers.h
	else if (fi.fLangType.GetChar(0) == _T('T'))
		fntColor = WxColour2Int(m_targetColor); // WxColour2Int() in helpers.h
	else if (fi.fLangType.GetChar(0) == _T('N'))
		fntColor = WxColour2Int(m_navTextColor); // WxColour2Int() in helpers.h
	data.Empty();
	data << szColor << tab << fntColor; //data = szColor + tab + number + end;
	pf->AddLine(data);

#else // _UNICODE version (ie. _UNICODE defined)

	// MFC: for the unicode version, from version 2.0.4 and onwards we may as well force output of the
	// configuration files to be in UTF-8, nothing is lost and the potential for spurious conversions
	// to a legacy encoding is avoided; hence the next six lines, and 3 lines at end of the function
	// to restore the old system encoding value
	data.Empty();
	data << szHeight << tab << fi.fHeight; //data = szHeight + tab + number + end;
	// Note: On 15Mar04 Bruce changed all of the m_systemEncoding params in ConvertAndWrite to eUTF8
	// but I don't expect I'll need to use ConvertAndWrite, so I'll keep his changes as commented
	// out lines below the earlier commented out lines.
	pf->AddLine(data);

	data.Empty();
	data << szWidth << tab << fi.fWidth; //data = szWidth + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szEscapement << tab << fi.fEscapement; //data = szEscapement + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szOrientation << tab << fi.fOrientation; //data = szOrientation + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szWeight << tab << fi.fWeightConfSave; //data = szWeight + tab + number + end;
	pf->AddLine(data);

	if (fi.fItalic)
		number = _T("1");
	else
		number = _T("0"); 
	data.Empty();
	data << szItalic << tab << number; //data = szItalic + tab + number + end;
	pf->AddLine(data);

	if (fi.fUnderline)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szUnderline << tab << number; //data = szUnderline + tab + number + end;
	pf->AddLine(data);

	if (fi.fStrikeOut)
		number = _T("1");
	else
		number = _T("0");
	data.Empty(); 
	data << szStrikeOut << tab << number; //data = szStrikeOut + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szFontEncoding << tab << fi.fEncoding; //data = szCharSet + tab + number + end;
	pf->AddLine(data); // whm added 14Feb07

	data.Empty();
	data << szCharSet << tab << fi.fCharset; //data = szCharSet + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szOutPrecision << tab << fi.fOutPrecision; //data = szOutPrecision + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szClipPrecision << tab << fi.fClipPrecision; //data = szClipPrecision + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szQuality << tab << fi.fQuality; //data = szQuality + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szPitchAndFamily << tab << fi.fPitchAndFamily; //data = szPitchAndFamily + tab + number + end;
	pf->AddLine(data);

	data.Empty();
	data << szFaceName << tab << fi.fFaceName; //data = szFaceName + tab + number + end;
	pf->AddLine(data);

	int fntColor = 0; // default black until changed below
	if (fi.fLangType.GetChar(0) == _T('S'))
		fntColor = WxColour2Int(m_sourceColor); // WxColour2Int() in helpers.h
	else if (fi.fLangType.GetChar(0) == _T('T'))
		fntColor = WxColour2Int(m_targetColor); // WxColour2Int() in helpers.h
	else if (fi.fLangType.GetChar(0) == _T('N'))
		fntColor = WxColour2Int(m_navTextColor); // WxColour2Int() in helpers.h
	data.Empty();
	data << szColor << tab << fntColor; //data = szColor + tab + number + end;
	pf->AddLine(data);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pf   <- pointer to the wxTextFile that is being written as the config file
/// \remarks
/// Called from: the App's WriteConfigurationFile(). 
/// Writes the basic settings part of a configuration file. The text file is opened by the 
/// caller and remains open after the call to WriteFontConfiguration(). 
/// Note: wxTextFile is processed entirely in memory, so it is up to the caller to actually 
/// write the information to the file by calling pf->Write() once the in-memory file has 
/// been fully composed/processed.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::WriteBasicSettingsConfiguration(wxTextFile* pf)
{
	// wx version combines ANSI and UNICODE parts in common to simplify this function
	wxString data = _T("");
	wxString tab = _T("\t");
	wxString number;

	data.Empty();
	data << szSourceLanguageName << tab << m_sourceName;
	pf->AddLine(data);

	data.Empty();
	data << szTargetLanguageName << tab << m_targetName;
	pf->AddLine(data);

	data.Empty();
	data << szAdaptitPath << tab << m_workFolderPath;
	pf->AddLine(data);

	data.Empty();
	data << szCurProjectName << tab << m_curProjectName;
	pf->AddLine(data);

	data.Empty();
	data << szCurLanguagesPath << tab << m_curProjectPath;
	pf->AddLine(data);

	data.Empty();
	data << szCurAdaptionsPath << tab << m_curAdaptionsPath;
	pf->AddLine(data);

	data.Empty();
	data << szCurKBName << tab << m_curKBName;
	pf->AddLine(data);

	data.Empty();
	data << szCurKBPath << tab << m_curKBPath;
	pf->AddLine(data);

	data.Empty();
	data << szCurKBBackupPath << tab << m_curKBBackupPath;
	pf->AddLine(data);

	data.Empty();
	data << szLastDocPath << tab << m_lastDocPath;
	pf->AddLine(data);

	data.Empty();
	data << szLastSourceFileFolder << tab << m_lastSourceFileFolder;
	pf->AddLine(data);

	data.Empty();
	data << szLastExportPath << tab << m_lastExportPath;
	pf->AddLine(data);

	data.Empty();
	data << szLastActiveSequNum << tab << nLastActiveSequNum;
	pf->AddLine(data);

	// now for the view stuff
	data.Empty();
	data << szDefaultTablePath << tab << m_defaultTablePath;
	pf->AddLine(data);

#ifndef _UNICODE
	// ANSI
	wxString s;
	PunctPairsToString(m_punctPairs,s);
	data.Empty();
	data << szPunctPairs << tab << s; //data = szPunctPairs + tab + s + end;
	pf->AddLine(data);

	TwoPunctPairsToString(m_twopunctPairs,s);
	data.Empty();
	data << szTwoPunctPairs << tab << s; //data = szTwoPunctPairs + tab + s + end;
	pf->AddLine(data);

#else
	// UNICODE
	// From version 2.3.0 the hard-coding of the encoding ID in punctuation fields 
	// has been removed, we use UTF-8 for them all now
	wxString src;
	wxString tgt;
	PunctPairsToTwoStrings(m_punctPairs,src,tgt);

	data.Empty();
	data << szPunctPairsSrc << tab << src;
	pf->AddLine(data);

	data.Empty();
	data << szPunctPairsTgt << tab << tgt;
	pf->AddLine(data);

	TwoPunctPairsToTwoStrings(m_twopunctPairs,src,tgt);
	
	data.Empty();
	data << szTwoPunctPairsSrc << tab << src;
	pf->AddLine(data);

	data.Empty();
	data << szTwoPunctPairsTgt << tab << tgt; 
	pf->AddLine(data);
#endif // _UNICODE

	if (m_bZoomed)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szZoomed << tab << number;
	pf->AddLine(data);

	data.Empty();
	data << szMaxToDisplay << tab << m_nMaxToDisplay;
	pf->AddLine(data);

	data.Empty();
	data << szMinPrecContext << tab << m_nPrecedingContext;
	pf->AddLine(data);

	data.Empty();
	data << szMinFollContext << tab << m_nFollowingContext;
	pf->AddLine(data);

	data.Empty();
	data << szLeading << tab << m_curLeading;
	pf->AddLine(data);

	data.Empty();
	data << szLeftMargin << tab << m_curLMargin;
	pf->AddLine(data);

	data.Empty();
	data << szGapWidth << tab << m_curGapWidth;
	pf->AddLine(data);

	if (m_bSuppressFirst)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szSuppressFirst << tab << number;
	pf->AddLine(data);

	if (m_bSuppressLast)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szSuppressLast << tab << number;
	pf->AddLine(data);

	if (m_bBackupDocument)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szBackupDocument << tab << number;
	pf->AddLine(data);

	if (m_bHidePunctuation)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szHidePunctuation << tab << number;
	pf->AddLine(data);

	data.Empty();
	data << szSpecialTextColor << tab << WxColour2Int(m_specialTextColor);
	pf->AddLine(data);

	data.Empty();
	data << szReTranslnTextColor << tab << WxColour2Int(m_reTranslnTextColor);
	pf->AddLine(data);

	if (m_bSuppressWelcome)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szSuppressWelcome << tab << number;
	pf->AddLine(data);

	// Version 3 always uses startup wizard
	number = _T("1");
	data.Empty();
	data << szUseStartupWizardOnLaunch << tab << number;
	pf->AddLine(data);

	if (m_bAutoBackupKB)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szBackupKBFlag << tab << number;
	pf->AddLine(data);

	if (m_bNoAutoSave)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szNoAutoSaveFlag << tab << number;
	pf->AddLine(data);

	data.Empty();
	data << szTS_DOC_MINS << tab << m_timeSettings.m_tsDoc.GetMinutes();
	pf->AddLine(data);

	// The wxTimeSpan::GetSeconds() works differently than CTimeSpan's method of the same name.
	// The wxWidgets GetSeconds() gets the total time span difference in seconds, whereas the MFC
	// method only returns the number of seconds in the current minute (-59 through 59).
	// Because of these differences and since Bruce decided to ignore seconds any way, we 
	// will just set the string value to zero _T("0");
	data.Empty();
	data << szTS_DOC_SECS << tab << _T("0");
	pf->AddLine(data);

	data.Empty();
	data << szTS_KB_MINS << tab << m_timeSettings.m_tsKB.GetMinutes();
	pf->AddLine(data);

	// force the seconds value to be string zero _T("0")
	data.Empty();
	data << szTS_KB_SECS << tab << _T("0");
	pf->AddLine(data);

	data.Empty();
	data << szPhraseBoxMoves << tab << m_nMoves;
	pf->AddLine(data);

	if (m_bIsDocTimeButton)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szIsDocTimeButtonFlag << tab << number;
	pf->AddLine(data);

	data.Empty();
	data << szNavTextColor << tab << WxColour2Int(m_navTextColor);
	pf->AddLine(data);

	data.Empty();
	data << szTopLeftX << tab << m_ptViewTopLeft.x;
	pf->AddLine(data);

	data.Empty();
	data << szTopLeftY << tab << m_ptViewTopLeft.y;
	pf->AddLine(data);

	data.Empty();
	data << szWSizeCX << tab << m_szView.x;
	pf->AddLine(data);

	data.Empty();
	data << szWSizeCY << tab << m_szView.y;
	pf->AddLine(data);

	if (m_bMarkerWrapsStrip)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szMarkerWrapsFlag << tab << number;
	pf->AddLine(data);

	if (m_bIsInches)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szLoEnglishFlag << tab << number;
	pf->AddLine(data);

	if (m_bIsPortraitOrientation)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szUsePortraitOrientation << tab << number;
	pf->AddLine(data);

	data.Empty();
	data << szLastPageWidth << tab << m_pageWidth;
	pf->AddLine(data);

	data.Empty();
	data << szLastPageLength << tab << m_pageLength;
	pf->AddLine(data);

	data.Empty();
	data << szPaperSizeCode << tab << m_paperSizeCode;
	pf->AddLine(data);

	data.Empty();
	data << szMarginTop << tab << m_marginTop;
	pf->AddLine(data);

	data.Empty();
	data << szMarginBottom << tab << m_marginBottom;
	pf->AddLine(data);

	data.Empty();
	data << szMarginLeft << tab << m_marginLeft;
	pf->AddLine(data);

	data.Empty();
	data << szMarginRight << tab << m_marginRight;
	pf->AddLine(data);

	data.Empty();
	data << szPhraseBoxExpansionMultiplier << tab << gnExpandBox;
	pf->AddLine(data);

	data.Empty();
	data << szTooNearEndMultiplier << tab << gnNearEndFactor;
	pf->AddLine(data);

	if (m_bRTL_Layout)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szRTL_Layout << tab << number;
	pf->AddLine(data);

// the following is only in the _UNICODE version
#ifdef _UNICODE
#ifdef _RTL_FLAGS

	if (m_bSrcRTL)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szRTLSource << tab << number;
	pf->AddLine(data);

	if (m_bTgtRTL)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szRTLTarget << tab << number;
	pf->AddLine(data);

	if (m_bNavTextRTL)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szRTLNavText << tab << number;
	pf->AddLine(data);

#endif // _RTL_FLAGS
#endif // _UNICODE
	
	data.Empty();
	data << szLastSourceExportPath << tab << m_lastSrcExportPath;
	pf->AddLine(data);

	data.Empty();
	data << szKBExportPath << tab << m_kbExportPath;
	pf->AddLine(data);

	data.Empty();
	data << szRetranslationReportPath << tab << m_retransReportPath;
	pf->AddLine(data);

	data.Empty();
	data << szRTFExportPath << tab << m_rtfExportPath;
	pf->AddLine(data);

	if (m_bSuppressTargetHighlighting)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szSuppressTargetHighlighting << tab << number;
	pf->AddLine(data);

	data.Empty();
	data << szAutoInsertionsHighlightColor << tab << WxColour2Int(m_AutoInsertionsHighlightColor);
	pf->AddLine(data);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      strReadIn   -> a text line read from the config file
/// \param      strValue    <- the "value" of the setting as a string
/// \param      name        <- the "label" or setting being defined on the left part of the config file line
/// \remarks
/// Called from: the App's GetFontConfiguration(), GetBasicSettingsConfiguration(), 
/// GetProjectSettingsConfiguration(), and GetConfigurationFile().
/// Parses the text line from the config file into a string label and a string value associated
/// with that label (the value can be an empty string). Assumes that the label and its associated 
/// value are separated by a tab ('\t') on the same text line (or two tabs for version 2.3.0 and 
/// earlier).
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::GetValue(const wxString strReadIn, wxString& strValue, wxString& name)
{
	wxChar delimiter = _T('\t');
	name.Empty();
	strValue.Empty();
	if (strReadIn.IsEmpty())
		return;
	name = strReadIn.BeforeFirst(delimiter);
	int offset = strReadIn.Find(delimiter);
	if (offset == -1)
	{
		// its a section heading only
		return;
	}
	// determine how many tabs. If there are two, we have an old config file with the encoding
	// (UTF-8) value stored between the two tabs, and we want to just jump it and extract the
	// data which follows the second tab; but if there is only one tab, then the data we want
	// follows immediately after it. Version 2.3.0 will no longer write out the encoding value.
	offset++; // offset now references the start of either the data or the encoding subfield
	int freq = strReadIn.Freq(delimiter); // returns the number of occurrences of delimiter
	int nFound;
	if (freq >= 2)
		nFound = strReadIn.Find(delimiter,TRUE); // TRUE search from end forward
	else
		nFound = strReadIn.Find(delimiter,FALSE); // search from front of strReadIn
	wxString strExtract;
	if (freq == 1)
	{
		// the line has only a singe tab, so offset references the data we want to extract
		strExtract = strReadIn.Mid(offset);
	}
	else
	{
		// the line has two tabs, so it is an older config file, and ++nFound will be
		// the reference to the data we want
		strExtract = strReadIn.Mid(++nFound);
	}
	// note: strValue can be empty (eg. the default path to the cc tables)
	strValue = strExtract;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxSize representing the extent of the longest sfm string in the styles map
/// \param      pDC   -> pointer to the display context
/// \remarks
/// Called from: the App's OnInit().
/// Scans through all entries in the m_pUsfmAndPngStylesMap styles map and determines the 
/// maximum (horizontal) text extent of the longest sfm read from the AI_USFM.xml file.
////////////////////////////////////////////////////////////////////////////////////////////
wxSize CAdapt_ItApp::GetExtentOfLongestSfm(wxDC* pDC)
{
	wxSize tempSize;
	wxSize maxSize(0,0);
	USFMAnalysis* pSfm;
	wxString key;
	MapSfmToUSFMAnalysisStruct::iterator iter;
	for( iter = m_pUsfmAndPngStylesMap->begin(); iter != m_pUsfmAndPngStylesMap->end(); ++iter )
	{
		pSfm = iter->second;
		pDC->GetTextExtent(pSfm->marker, &tempSize.x,&tempSize.y);
		if (tempSize.x > maxSize.x)
		{
			maxSize.x = tempSize.x;
			maxSize.y = tempSize.y;
		}
	}
	return maxSize;
}


#ifdef _UNICODE

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      s   <- a wxString on which any "NR Work" substring exists is converted to "Unicode Work"
/// \remarks
/// Called from: the App's GetBasicSettingsConfiguration(), GetProjectSettingsConfiguration().
/// Changes an instance of "NR Work" found within a config file (path) string s to "Unicode Work". 
/// This is done to automatically update any path names that were created with very old versions of Adapt 
/// It to agree with the current work folder name scheme for the Unicode application.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::ChangeNRtoUnicode(wxString& s)
{
	int nFound = s.Find(_T("NR Work")); // only will occur once per string
	if (nFound < 0)
		return;
	s.Remove(nFound,7);
	s = InsertInString(s,nFound,_T("Unicode Work"));
}
#endif

// **********************************************************************************************
// !!! possible useful functions below !!!
// whm added 29Dec06 (modified from functions in catalog.cpp of PoEdit sources by Vaclav Slavik)

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the input string converted to the local charset/encoding
/// \param      str   -> the string to be converted to the local charset/encoding
/// \remarks
/// Called from: (currently unused)
/// An inline function that onverts an input string to the local charset or encoding using the 
/// wxConvUTF8 and wxConvLocal predefined converters. Attempts conversion only in ANSI builds. 
/// In Unicode builds it simply returns the (Unicode) str unchanged.
////////////////////////////////////////////////////////////////////////////////////////////
inline wxString convertToLocalCharset(const wxString& str)
{
#if !wxUSE_UNICODE
	wxString s2(str.wc_str(wxConvUTF8), wxConvLocal);
	if (s2.empty() /*conversion failed*/)
		return str;
	else
		return s2;
#else
	return str;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the input string converted from the local charset/encoding
///                 to UTF-8 encoding
/// \param      str   -> the string to be converted from the local charset/encoding to UTF-8
/// \remarks
/// Called from: (currently unused)
/// An inline function that onverts an input string from the local charset or encoding to UTF-8 
/// using the wxConvLocal wxConvUTF8 predefined converters. Attempts conversion only in ANSI builds. 
/// In Unicode builds it simply returns the (Unicode) str unchanged.
////////////////////////////////////////////////////////////////////////////////////////////
inline wxString convertFromLocalCharset(const wxString& str)
{
#if !wxUSE_UNICODE
	return wxString(str.wc_str(wxConvLocal), wxConvUTF8);
#else
	return str;
#endif
}

// ReadTextLine is not used in the wx version which uses the wxTextFile class.

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE when font info section of config file was read up to a blank line. If there
///                 was a problem reading a font setting it returns FALSE to tell the caller to
///                 issue a warning message (but config file reading continues)
/// \param      fi   <- the fontInfo struct containing the font information as read in from the
///                         configuration files
/// \param      pf   -> pointer to the wxTextFile that is being read as the config file
/// \remarks
/// Called from: the App's GetConfigurationFile(). 
/// Reads the font configuration part of a configuration file. GetFontConfiguration() gets
/// called once for each of the three main fonts used: source language, target language, and
/// navigation text language. The text file is opened by the caller and remains open after each
/// call to GetFontConfiguration(). Note: wxTextFile is processed entirely in memory.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::GetFontConfiguration(fontInfo& fi, wxTextFile* pf)
// Note: wxTextFile is Unicode enabled when _UNICODE is defined
// This function accepts the config file entries in any order, as long as the format of
// each line's entry is correct: that is, a name, tab, string value, and \n at the end.
// For the unicode compilation, the config files are saved as UTF-8 with no initial BOM;
// but if the user opens the config file in WORD (as an encoded file), edits something perhaps,
// and then resaves it - WORD will write a UFT-8 BOM at the start of the saved file. This will
// clobber the S, T, or N selector syntax, so we must check for a BOM and remove it if present.

// NOTE: This wxWidgets version uses wxTextFile and we can assume that the whole
// config file was successfully read into memory. So, we will be "reading" successive
// lines from memory rather than from disk which eliminates the need for some of
// MFC's disk-related error trapping.
{
	wxChar chSelector;
	wxString data;
	data.Empty();
	wxString name;
	bool	bIsOK = TRUE;
	wxString strValue;
	int		num; // for the string converted to a number

	// The first call of GetFontConfiguration reads the Source Font data, and
	// exits at the first blank line at end of Source Font data; then the second 
	// call of the routine continues reading the Target Font data, exiting at 
	// the next blank line (at end of Target Font data), and the third
	// call of the routine continues reading the Nav Font data, exiting at the
	// blank line just before the "Settings" (or "ProjectSettings") heading.
	// Here we'll not use GetFirstLine() since this current function is designed to
	// be entered three times (once each for source, target and nav fonts) while the
	// config file is open with the pointer continuing through the file, rather than
	// being reset to the beginning like GetFirstLine would do. Hence, we'll check
	// to see if we are at line zero or not and only use GetNextLine when we're
	// not at line zero.
	size_t curLine = pf->GetCurrentLine();
	data = pf->GetLine(curLine);
	// don't use GetNextLine if we've just opened the file, otherwise we would skip
	// the first line of the file before getting a chance to process it below
	if (curLine != 0) 
	{
		data = pf->GetNextLine();
	}
	GetValue(data, strValue, name);

	// get rid of the BOM if present - we can assume it is there if the first character
	// of name is not S, T, or N
	chSelector = name.GetChar(0);
	if ((chSelector != _T('S')) && (chSelector != _T('T')) && (chSelector != _T('N')))
	{
		// BEW changed, 2July07, because the UTF8 name becomes UTF16, and is one UTF16 char by the
		// time the program counter gets here, so Mid(3) needs to become Mid(1) (from Bob Eaton)
#ifdef _UNICODE
		name = name.Mid(1); // in the Unicode version, the UTF-8 BOM (which was 3 bytes long) is turned
							// into a single UTF16 character
#else
		name = name.Mid(3);	// in the ansi version, the UTF-8 BOM is 3 characters
#endif
		chSelector = name.GetChar(0); // S for source, T for target, N for navText; 
									// use for setting color
		if ((chSelector != _T('S')) && (chSelector != _T('T')) && (chSelector != _T('N')))
		{
			wxMessageBox(_T(
	"Selector not set to one of S, T, or N, when getting the logfont info. Will try to continue."));
		}
	}

	// Because of a peculiarity of wxTextFile, its method Eof returns TRUE 
	// if the current line is the last one, so we need to use a do...while 
	// loop here rather than a while loop, otherwise we'll not process the 
	// last line. This behavior of wxTextFile may have changed with wxWidgets
	// library version 2.7.0 and later, but the method here still works.
	do
	{
		data = pf->GetNextLine();

		GetValue(data, strValue, name);

		if ( name == szHeight)
		{
			num = wxAtoi(strValue); // MFC has _ttoi((LPCTSTR)strValue);
									// Note: both _ttoi and wxAtoi return 0 if strValue is 
									// empty string ""
			if (num < -120 || num > 120)
				num = -16; // 12 point
			fi.fHeight = num; // this fi value is saved in config
			fi.fPointSize = NegHeightToPointSize(num);// this one for program use
			// set the fontsize for the actual font on the App
			if (fi.fLangType.GetChar(0)  == _T('S'))
				m_pSourceFont->SetPointSize(fi.fPointSize);
			else if (fi.fLangType.GetChar(0)  == _T('T'))
				m_pTargetFont->SetPointSize(fi.fPointSize);
			else if (fi.fLangType.GetChar(0)  == _T('N'))
				this->m_pNavTextFont->SetPointSize(fi.fPointSize);

			// testing of PointSize NegHeight conversion functions below !!!!
			//int lfh;
			//int ps;
			//lfh = PointSizeToNegHeight(9); // lfh = -12
			//ps = NegHeightToPointSize(lfh);// ps = 9
			//lfh = PointSizeToNegHeight(10);// lfh = -13
			//ps = NegHeightToPointSize(lfh);// ps = 10
			//lfh = PointSizeToNegHeight(11);// lfh = -14
			//ps = NegHeightToPointSize(lfh);// ps = 11
			//lfh = PointSizeToNegHeight(12);// lfh = -16
			//ps = NegHeightToPointSize(lfh);// ps = 12
			//lfh = PointSizeToNegHeight(13);// lfh = -17
			//ps = NegHeightToPointSize(lfh);// ps = 13
			//lfh = PointSizeToNegHeight(14);// lfh = -18
			//ps = NegHeightToPointSize(lfh);// ps = 14
			//lfh = PointSizeToNegHeight(15);// lfh = -20
			//ps = NegHeightToPointSize(lfh);// ps = 15
			//lfh = PointSizeToNegHeight(16);// lfh = -21
			//ps = NegHeightToPointSize(lfh);// ps = 16
			//lfh = PointSizeToNegHeight(17);// lfh = -22
			//ps = NegHeightToPointSize(lfh);// ps = 17
			//lfh = PointSizeToNegHeight(18);// lfh = -24
			//ps = NegHeightToPointSize(lfh);// ps = 18
			//lfh = PointSizeToNegHeight(19);// lfh = -25
			//ps = NegHeightToPointSize(lfh);// ps = 19
			//lfh = PointSizeToNegHeight(20);// lfh = -26
			//ps = NegHeightToPointSize(lfh);// ps = 20
			//int dummy = -1;
			// test results appear to be what they should be
			// end of testing!!!!
		}
		else if (name == szWeight)
		{
			num = wxAtoi(strValue);
			if (num < 400 || num > 1000) // just a guess for a valid range,
										 // its normally 400, or 700 for bold
				num = 700; // default bold

			// we'll retain the raw fWeight value read in, in order to be able to 
			// save it out to config file again
			fi.fWeightConfSave = num;
			// wxWidgets needs the predefined const value for Weight
			if (num >= 400 && num < 500)
				fi.fWeight = wxLIGHT;
			else if (num >= 500 && num < 700)
				fi.fWeight = wxNORMAL;
			else if (num >= 700)
				fi.fWeight = wxBOLD;
			// set the weight of the actual font on the App
			if (fi.fLangType.GetChar(0)  == _T('S'))
				m_pSourceFont->SetWeight(fi.fWeight);
			else if (fi.fLangType.GetChar(0)  == _T('T'))
				m_pTargetFont->SetWeight(fi.fWeight);
			else if (fi.fLangType.GetChar(0)  == _T('N'))
				this->m_pNavTextFont->SetWeight(fi.fWeight);
		}
		else if (name == szWidth)
		{
			num = wxAtoi(strValue); // allow nearly anything positive, 
									// but 0 for automatic calculation - 
			if (num < 0 || num > 1000)
				num = 0;
			fi.fWidth = num; // font width is ignored in wxWidgets
		}
		else if (name == szEscapement)
		{
			num = wxAtoi(strValue);
			if (num < -10 || num > 10) // only allow one degree, its normally 0
				num = 0; 
			fi.fEscapement = num; // font escapement is ignored in wxWidgets
		}
		else if (name == szOrientation)
		{
			num = wxAtoi(strValue); 
			if (num < -10 || num > 10) // only allow one degree, its normally 0
				num = 0; 
			fi.fOrientation = num; // font orientation is ignored in wxWidgets
		}
		else if (name == szItalic)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 255)) // TRUE if italic, regular is FALSE
				num = 0; // regular
			if (num == 0)
				fi.fStyle = wxNORMAL;
			else 
				fi.fStyle = wxITALIC;
			// set the style (normal or italic) of the actual font on the App
			if (fi.fLangType.GetChar(0)  == _T('S'))
				m_pSourceFont->SetStyle(fi.fStyle);
			else if (fi.fLangType.GetChar(0)  == _T('T'))
				m_pTargetFont->SetStyle(fi.fStyle);
			else if (fi.fLangType.GetChar(0)  == _T('N'))
				this->m_pNavTextFont->SetStyle(fi.fStyle);
		}
		else if (name == szStrikeOut)
		{
			num = wxAtoi(strValue);
			if (num != 0)
				num = 0; // don't want strikeout
			if (num == 0)
				fi.fStrikeOut = FALSE; // font strikeout is ignored in wxWidgets
			else
				fi.fStrikeOut = TRUE;
		}
		else if (name == szUnderline)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1)) // TRUE if underline, regular is FALSE
				num = 0; // regular
			if (num == 0) 
				fi.fUnderline = FALSE;
			else 
				fi.fUnderline = TRUE;
			// set the underline of the actual font on the App
			if (fi.fLangType.GetChar(0)  == _T('S'))
				m_pSourceFont->SetUnderlined(fi.fUnderline);
			else if (fi.fLangType.GetChar(0)  == _T('T'))
				m_pTargetFont->SetUnderlined(fi.fUnderline);
			else if (fi.fLangType.GetChar(0)  == _T('N'))
				this->m_pNavTextFont->SetUnderlined(fi.fUnderline);
		}
		else if (name == szOutPrecision)
		{
			num = wxAtoi(strValue);
			if (num < 0 || num > 9)
				num = 3; // OUT_DEFAULT_PRECIS is 0, others are +ve, 3 is usual one
			fi.fOutPrecision = num; // font outprecision is ignored in wxWidgets
		}
		else if (name == szQuality)
		{
			num = wxAtoi(strValue);
			if (num < 0 || num > 2)
				num = 0;// DRAFT_QUALITY; // DRAFT_QUALITY is 1, default 0,
									 // proof is 2; 1 is usual one
			fi.fQuality = num; // font quality is ignored in wxWidgets
		}
		else if (name == szClipPrecision)
		{
			num = wxAtoi(strValue);
			if (num < 0 || num > 7)
				num = 2; //CLIP_STROKE_PRECIS; // CLIP_STROKE_PRECIS is for TrueType fonts
			fi.fClipPrecision = num; // font clipprecision is ignored in wxWidgets
		}
		else if (name == szPitchAndFamily)
		{
			num = wxAtoi(strValue);
			if (num < 0 || num > 255)
				num = 2; // all bytes can be used, usual value is 2
			fi.fPitchAndFamily = num; // this one saved in conf file; fFamily used within program
			// wxWidgets is only interested in the family which is encoded in bits 4-7
			wxUint32 FF;
			FF = num & 0xF0; // mask out the 4 low order bits
			switch (FF)
			{
			case 1: fi.fFamily = wxROMAN;break;			//1 = FF_ROMAN
			case 2: fi.fFamily = wxSWISS;break;			//2 = FF_SWISS
			case 3: fi.fFamily = wxMODERN;break;		//3 = FF_MODERN
			case 4: fi.fFamily = wxSCRIPT;break;		//4 = FF_SCRIPT
			case 5: fi.fFamily = wxDECORATIVE;break;	//5 = FF_DECORATIVE
			case 0: fi.fFamily = wxDEFAULT;break;		//0 = FF_DONTCARE
			default: fi.fFamily = wxDEFAULT;
			}
			// set the family of the actual font on the App
			if (fi.fLangType.GetChar(0)  == _T('S'))
				m_pSourceFont->SetFamily(fi.fFamily);
			else if (fi.fLangType.GetChar(0)  == _T('T'))
				m_pTargetFont->SetFamily(fi.fFamily);
			else if (fi.fLangType.GetChar(0)  == _T('N'))
				this->m_pNavTextFont->SetFamily(fi.fFamily);
		}
		else if (name == szFontEncoding) // if present in config file, this should preceed szCharset
		{
			m_bConfigFileHasFontEncodingInfo = TRUE;
			num = wxAtoi(strValue);
			if (num < -1 || num > 255)
				num = 0; // allow only possible values, default to 0 (wxFONTENCODING_DEFAULT) if value corrupted out of range
			fi.fEncoding = (wxFontEncoding)num;
			// Note: setting the font encoding of the actual font is done after CharSet is read (below)
			// because if the config file was produced by the MFC app, it won't have this szFontEncoding
			// field and this else if block will not execute, but the CharSet block will execute
		}
		else if (name == szCharSet)
		{
			num = wxAtoi(strValue);
			if (num < 0 || num > 255)
				num = 0; // allow many values, usual value is 0 
						 // (probably DEFAULT_CHARSET)
			fi.fCharset = num;
			if (!m_bConfigFileHasFontEncodingInfo)
			{
				// config file was produced by the MFC version, so we need
				// to map the fCharset value to fEncoding value which
				// is used internally in the wx version (see fontInfo struct). 
				// The fCharset value is maintained primarily for backwards compatibility 
				// with the MFC version's LOGFONT apparatus.
				fi.fEncoding = MapMFCCharsetToWXFontEncoding(fi.fCharset);
			}
			// If this config file was produced by the MFC app, the "if (!m_bConfigFileHasFontEncodingInfo)"
			// block above will have mapped the MFC fi.fCharset to an equivalent wx font encoding (stored in
			// fi.fEncoding).
			// set the font encoding of the actual font on the App
			if (fi.fLangType.GetChar(0)  == _T('S'))
				m_pSourceFont->SetEncoding(fi.fEncoding);
			else if (fi.fLangType.GetChar(0)  == _T('T'))
				m_pTargetFont->SetEncoding(fi.fEncoding);
			else if (fi.fLangType.GetChar(0)  == _T('N'))
				this->m_pNavTextFont->SetEncoding(fi.fEncoding);
		}
		else if (name == szFaceName)
		{
			fi.fFaceName = strValue;
			// set the face name of the actual font on the App
			// wx version: We could use wxFontMapper to locate a font on the user's machine
			// corresponding to the font specified in the config file. If found we could assign 
			// its face name to the appropriate font using SetFaceName(). If the face name doesn't 
			// exist we won't try to set the face name, but allow the font family setting
			// to stand; and will notify the user that a substitute font is being used. This method,
			// however, is far to slow (see below).
			//wxFontEnumerator fe;
			//wxArrayString faceNames;
			//faceNames = fe.GetFacenames();
			//int ct,nFaceNames;
			//wxString fname;
			bool bFoundFaceName = FALSE;
			//nFaceNames = fe.GetFacenames().GetCount();
			// the following for loop is way too time consuming, sometimes taking 5-10 seconds
			// so we'll just try to assigne the facename. SetFaceName() returns a bool that is TRUE if
			// the face name exists; false otherwise. If it returns false we'll just get a font from
			// the general font family.
			//for (ct = 0; ct < nFaceNames; ct++)
			//{
			//	fname = fe.GetFacenames().Item(ct);
			//	if (fi.fFaceName == fname)
			//	{
			//		bFoundFaceName = TRUE;
			//		break;
			//	}
			//}
			// set the face name of the actual font on the App to the exact or compatible
			// value
			if (fi.fLangType.GetChar(0)  == _T('S'))
			{
				bFoundFaceName = m_pSourceFont->SetFaceName(fi.fFaceName);
				if (!bFoundFaceName)
				{
					// use whatever facename the system came up with
					fi.fFaceName = m_pSourceFont->GetFaceName();
				}
					
			}
			else if (fi.fLangType.GetChar(0)  == _T('T'))
			{
				bFoundFaceName = m_pTargetFont->SetFaceName(fi.fFaceName);
				if (!bFoundFaceName)
				{
					// use whatever facename the system came up with
					fi.fFaceName = m_pTargetFont->GetFaceName();
				}
			}
			else if (fi.fLangType.GetChar(0)  == _T('N'))
			{
				bFoundFaceName = m_pNavTextFont->SetFaceName(fi.fFaceName);
				if (!bFoundFaceName)
				{
					// use whatever facename the system came up with
					fi.fFaceName = m_pNavTextFont->GetFaceName();
				}
			}
		}
		else if (name == szColor)
		{
			num = wxAtoi(strValue); // allow anything 
			if (fi.fLangType.GetChar(0)  == _T('S'))
			{
				m_sourceColor = Int2wxColour(num);
				// It is necessary to call the font data's SetColour() function to effect color change
				m_pSrcFontData->SetColour(m_sourceColor);
			}
			else if (fi.fLangType.GetChar(0)  == _T('T'))
			{
				m_targetColor = Int2wxColour(num);
				// It is necessary to call the font data's SetColour() function to effect color change
				m_pTgtFontData->SetColour(m_targetColor); // 
			}
			else if (fi.fLangType.GetChar(0)  == _T('N'))
			{
				m_navTextColor = Int2wxColour(num);
				// In wx version text color is changed by modifying the font data object
				m_pNavFontData->SetColour(m_navTextColor);
			}
		}
		else if (name.IsEmpty()) // return when we come to the blank line
		{
			// The file pointer will be ready to read the next font's data
			return bIsOK;
		}
		else
		{
			bIsOK = FALSE;
			wxString error = _("Unrecognized font attribute field; unmatched name is: ") + name;
			wxMessageBox(error, _("Error parsing font information"), wxICON_ERROR);
		}
	} while(!pf->Eof());// should never get here just reading the font config data

	return bIsOK;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pf   -> pointer to the wxTextFile that is being read as the config file
/// \remarks
/// Called from: the App's GetConfigurationFile(). 
/// Reads the basic settings part of a basic configuration file. The text file is opened by 
/// the caller. Note: wxTextFile is processed entirely in memory. 
/// For most settings, this function accepts the config file entries in any order as long as
/// the format of each line's entry is correct: that is, a name, tab, and string value. Order
/// of entries is important in the config file only for unicode punctuation support i.e., the 
/// order of src & tgt punct lines.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::GetBasicSettingsConfiguration(wxTextFile* pf)
{
	//int nLineCount = 0;
	int		mins;
	int		secs;
	int		num; // for the string converted to a number
	bool	bAdjusted = FALSE;

	wxString data = _T("");
	wxString name;
	wxString strValue;

#ifdef _UNICODE
	bool bAddedLeftDoubleQuote = FALSE;
	bool bAddedRightDoubleQuote = FALSE;
	bool bAddedLeftSingleQuote = FALSE;
	bool bAddedRightSingleQuote = FALSE;

	wxString strPunctPairsSrcSet;
	wxString strPunctPairsTgtSet;
	wxString strTwoPunctPairsSrcSet;
	wxString strTwoPunctPairsTgtSet;
#endif
				 
	do
	{
		data = pf->GetNextLine();

#ifdef _UNICODE
		ChangeNRtoUnicode(data);
#endif
		GetValue(data, strValue, name);

		if (name == szSourceLanguageName)
		{
			m_sourceName = strValue;
		}
		else if (name == szTargetLanguageName)
		{
			m_targetName = strValue;
		}
		else if (name == szAdaptitPath)
		{
			m_workFolderPath = strValue;
			if (m_workFolderPath.IsEmpty())
			{
				EnsureWorkFolderPresent();
			}
		}
		else if (name == szCurProjectName)
		{
			m_curProjectName = strValue;
		}
		else if (name == szCurLanguagesPath)
		{
			m_curProjectPath = strValue;
		}
		else if (name == szCurAdaptionsPath)
		{
			m_curAdaptionsPath = strValue;
		}
		else if (name == szCurKBName)
		{
			m_curKBName = strValue;
		}
		else if (name == szCurKBPath)
		{
			m_curKBPath = strValue;
		}
		else if (name == szCurKBBackupPath)
		{
			m_curKBBackupPath = strValue;
		}
		else if (name == szLastDocPath)
		{
			m_lastDocPath = strValue;
		}
		else if (name == szLastSourceFileFolder)
		{
			m_lastSourceFileFolder = strValue;
		}
		else if (name == szLastExportPath)
		{
			m_lastExportPath = strValue;
		}
		else if (name == szLastActiveSequNum)
		{
			num = wxAtoi(strValue);
			if (num < 0 || num > 3000000)
				num = 0; // safe default is start of doc
			nLastActiveSequNum = num;
		}
		else if (name == szDefaultTablePath)
		{
			m_defaultTablePath = strValue; // can be empty
		}
		// From version 2.3.0, we don't write out m_punctuation[0] and m_punctuation[1]
		// and m_punctWordBuilding[0] and [1] are no longer used, so we retain the reads
		// here in case the user reads a legacy configuration file, but do nothing with
		// what is read for these fields; and in new config files, these fields are absent.
		else if (name == szSrcPunctuation) // new punctuation system (split)
		{
			; // m_punctuation[0] = strValue;
		}
		else if (name == szSrcPunctAsWordBuilding)
		{
			; // m_punctWordBuilding[0] = strValue;
		}
		else if (name == szTgtPunctuation)
		{
			; // m_punctuation[1] = strValue;
		}
		else if (name == szTgtPunctAsWordBuilding)
		{
			; // m_punctWordBuilding[1] = strValue;
		}
#ifndef _UNICODE
		else if (name == szPunctPairs)
		{
			// For Windows, ensure curly quotes are supported in ANSI version.
			// When first converted to wxWidgets, GTK's text control was failing when use SetValue() with 
			// any of these upper ascii values in the punctuation string. A partial solution was to insure 
			// that SetEncoding(wxFONTENCODING_CP1252) was called on the fonts involved. Note: SetEncoding() 
			// is an "undocumented" method of wxFont. The documented SetDefaultEncoding() did not seem to
			// work. Also setting the encoding to wxFONTENCODING_ISO8859_1 did not work (it eliminated
			// the "Failed to set text in text control" message, but showed the smart quotes as boxes
			// with the hex values in them, instead of the actual quote characters).
			// Because it was only a partial solution and problems seemed to continually crop up, in the 
			// end I decided to include smart quotes only in the wxMSW ANSI version and not in wxGTK builds.
#ifdef __WXMSW__
			wxString additions;
			additions.Empty();
			wxString ch = _T("."); // must not be empty, so put any char in it
			unsigned char thechar = 147; // left double quotation mark
			ch.SetChar(0,thechar);
			if (strValue.Find(ch) == -1) // not found
			{
				additions += ch;
				additions += ch;
			}
			thechar = 148; // right double quotation mark
			ch.SetChar(0,thechar);
			if (strValue.Find(ch) == -1) // not found
			{
				additions += ch;
				additions += ch;
			}
			thechar = 145; // left single quotation mark
			ch.SetChar(0,thechar);
			if (strValue.Find(ch) == -1) // not found
			{
				additions += ch;
				additions += ch;
			}
			thechar = 146; // right single quotation mark
			ch.SetChar(0,thechar);
			if (strValue.Find(ch) == -1) // not found
			{
				additions += ch;
				additions += ch;
			}
			strValue += additions;
#else
			// In non-windows ANSI versions, remove any smart quotes that exist in the
			// punctuation string. 
			// Possible Problem: We cannot assume that the user would always want smart 
			// quotes converted to regular quotes in the punctuation lists. Maybe they used
			// smart quotes for all their quotes requirements, and used a regular single quote 
			// (apostrophe) as a word building glottal character, for example.
			
			// whm note: For wx ANSI version, when compiled for wxGTK, we should
			// check the punct strings for illegal 8-bit chars, specifically for MS' smart
			// quotes which cause problems because the wxGTK library internally
			// keeps strings in UTF-8. We'll quietly convert any smart quotes we find
			// to normal quotes.
			int ct;
			bool hackedFontCharPresent = FALSE;
			int hackedCt = 0;
			wxString hackedStr;
			hackedStr.Empty();
			const wxChar lsqm(-111);
			const wxChar rsqm(-110);
			const wxChar ldqm(-109);
			const wxChar rdqm(-108);
			// replace any smart quote chars with a space - if both src and tgt punct have smart
			// quotes we end up with both being spaces, but I think StringToPunctPairs and its
			// complement PunctPairsToString can handle it.
			strValue.Replace(&lsqm,_T(" "));
			strValue.Replace(&rsqm,_T(" "));
			strValue.Replace(&ldqm,_T(" "));
			strValue.Replace(&rdqm,_T(" "));
			// check for any remaining hacked font chars
			for (ct = 0; ct < (int)strValue.Length(); ct++)
			{
				int charValue;
				charValue = (int)strValue.GetChar(ct);
				// whm note: The decimal int value of extended ASCII chars is represented
				// as its decimal extended value minus 256, making any extended characters
				// be represented as negative numbers
				if (charValue < 0)
				{
					// we have an extended ASCII character in the string, remove it
					//switch(charValue)
					//{
					//case -111:	strValue.Remove(ct,1); // left single quotation mark
					//			break;
					//case -110:	strValue.Remove(ct,1); // right single quotation mark
					//			break;
					//case -109:	strValue.Remove(ct,1); // left double quotation mark
					//			break;
					//case -108:	strValue.Remove(ct,1); // right double quotation mark
					//			break;
					//default:
					//	{
							// the default case indicates there is a non-smart quote extended
							// ASCII character present. In this case we should warn the user
							// that the data cannot be handled until it is converted to Unicode,
							// then delete the extended punct char from the list.
							//::wxBell();
							hackedFontCharPresent = TRUE;
							hackedCt++;
							if (hackedCt < 10)
							{
								hackedStr += _T("\n   character with ASCII value: ");
								hackedStr << (charValue+256);
							}
							else if (hackedCt == 10)
								hackedStr += _T("...\n");
					//	}
					//}
				}
			}

			// we assume that regular quote marks are already in the punctuation string if the user
			// desired them to be there

			if (hackedFontCharPresent && !gbHackedDataCharWarningGiven)
			{
				gbHackedDataCharWarningGiven = TRUE;
				wxString msg2 = _("\nYou should not use this non-Unicode version of Adapt It.\nYour data should first be converted to Unicode using TecKit\nand then you should use the Unicode version of Adapt It.");
				wxString msg1 = _("Extended 8-bit ASCII characters were detected in the\nBasic Configuration File\'s punctuation string (see below):");
				msg1 += hackedStr + msg2;
				wxMessageBox(msg1,_("Warning: Invalid Characters Detected"),wxICON_WARNING);
			}

#endif
			// wxGTK uses UTF-8 internally in its wxTextCtrl widgets, so let's try converting the punctuation
			// string to UTF-8 for wxGTK ANSI builds.
//#ifdef __GNUG__
//			// The following converts strValue to 
//			wxConvUTF8.cWC2MB(wxConvCurrent->cMB2WC(strValue));  
//#endif
			// set flag if apostrophe is designated as a punctuation character (this makes
			// ParseWord() do its job smarter within TokenizeText())
			int found = strValue.Find(_T('\'')); // look for vertical ordinary quote (ie. apostrophe)
			if (found >= 0)
				m_bSingleQuoteAsPunct = TRUE;
			else
				m_bSingleQuoteAsPunct = FALSE;

			StringToPunctPairs(m_punctPairs,strValue);
		}
		else if (name == szTwoPunctPairs)
		{
			StringToTwoPunctPairs(m_twopunctPairs,strValue);
		}
#else // _UNICODE version
		else if (name == szPunctPairsSrc)
		{
			// make sure curly quotes are supported as punctuation
			wxString additions;
			additions.Empty();
			wxString ch = _T("."); // must not be empty, or SetChar() fails
			ch.SetChar(0,L'\x201C'); // hex for left double quotation mark
			if (strValue.Find(ch) == -1) // not found
			{
				bAddedLeftDoubleQuote = TRUE;
				additions += ch;
			}
			ch.SetChar(0,L'\x201D'); // hex for right double quotation mark
			if (strValue.Find(ch) == -1) // not found
			{
				bAddedRightDoubleQuote = TRUE;
				additions += ch;
			}
			ch.SetChar(0,L'\x2018'); // hex for left single quotation mark
			if (strValue.Find(ch) == -1) // not found
			{
				bAddedLeftSingleQuote = TRUE;
				additions += ch;
			}
			ch.SetChar(0,L'\x2019'); // hex for right single quotation mark
			if (strValue.Find(ch) == -1) // not found
			{
				bAddedRightSingleQuote = TRUE;
				additions += ch;
			}
			int found = strValue.Find(_T('\'')); // look for vertical ordinary quote (ie. apostrophe)
			if (found >= 0)
				m_bSingleQuoteAsPunct = TRUE;
			else
				m_bSingleQuoteAsPunct = FALSE;
			strValue = additions + strValue;

			strPunctPairsSrcSet = strValue;
		}
		else if (name == szPunctPairsTgt)
		{
			// THIS BLOCK WORKS RIGHT ONLY WHEN THE
			// SOURCE SET WAS PROCESSED EARLIER - so
			// ORDER is important in the config file for unicode punctuation
			// support - so don't alter order of src & tgt punct lines

			// make sure curly quotes are supported as punctuation
			wxString additions;
			additions.Empty();
			wxString ch = _T("."); // set to anything, which will be overwritten below
			ch.SetChar(0,L'\x201C'); // hex for left double quotation mark
			if (bAddedLeftDoubleQuote) // source had this added
			{
				 // so target has to have it too
				additions += ch;
			}
			ch.SetChar(0,L'\x201D'); // hex for right double quotation mark
			if (bAddedRightDoubleQuote)
			{
				// add it if the source had it added
				additions += ch;
			}
			ch.SetChar(0,L'\x2018'); // hex for left single quotation mark
			if (bAddedLeftSingleQuote)
			{
				// add it if the source had it added
				additions += ch;
			}
			ch.SetChar(0,L'\x2019'); // hex for right single quotation mark
			if (bAddedRightSingleQuote)
			{
				// add it if the source had it added
				additions += ch;
			}
			int found = strValue.Find(_T('\'')); // look for vertical ordinary quote (ie. apostrophe)
			if (found >= 0)
				m_bSingleQuoteAsPunct = TRUE;
			else
				m_bSingleQuoteAsPunct = FALSE;
			strValue = additions + strValue;

			strPunctPairsTgtSet = strValue;
		}
		else if (name == szTwoPunctPairsSrc)
		{
			strTwoPunctPairsSrcSet = strValue;
		}
		else if (name == szTwoPunctPairsTgt)
		{
			strTwoPunctPairsTgtSet = strValue;
		}
#endif
		
		// now stuff for the view
		else if (name == szZoomed)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0; // window not zoomed
			if (num == 0)
				m_bZoomed = FALSE;
			else
				m_bZoomed = TRUE;
		}
		else if (name == szMaxToDisplay)
		{
			num = wxAtoi(strValue);
			if (num <= 80 || num > 4000)
				num = 300;
			m_nMaxToDisplay = num;
		}
		else if (name == szMinPrecContext)
		{
			num = wxAtoi(strValue);
			if (num <= 10 || num > 80)
				num = 50;
			m_nPrecedingContext = num;
		}
		else if (name == szMinFollContext)
		{
			num = wxAtoi(strValue);
			if (num <= 10 || num > 60)
				num = 30;
			m_nFollowingContext = num;
		}
		else if (name == szLeading)
		{
			num = wxAtoi(strValue);
			if (num <= 16 || num > 80)
				num = 32;
			m_curLeading = num; 
		}
		else if (name == szLeftMargin)
		{
			num = wxAtoi(strValue);
			if (num < 16 || num > 80) // whm 2Aug06 changed default to 16 for better visibility of the notes icon at left margin
				num = 16;
			m_curLMargin = num;
		}
		else if (name == szGapWidth)
		{
			num = wxAtoi(strValue);
			if (num < 6 || num > 80)
				num = 16;
			m_curGapWidth = num;
		}
		else if (name == szSuppressFirst)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1;
			if (num == 0)
				m_bSuppressFirst = FALSE;
			else
				m_bSuppressFirst = TRUE;
		}
		else if (name == szSuppressLast)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1;
			if (num == 0)
				m_bSuppressLast = FALSE;
			else
				m_bSuppressLast = TRUE;
		}
		else if (name == szBackupDocument)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1;
			if (num == 1)
				m_bBackupDocument = TRUE;
			else
				m_bBackupDocument = FALSE;
		}
		else if (name == szHidePunctuation)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0; // don't hide it
			if (num == 0)
				m_bHidePunctuation = FALSE;
			else
				m_bHidePunctuation = TRUE;
		}
		else if (name == szSuppressWelcome)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0; // don't suppress it
			if (num == 1)
				m_bSuppressWelcome = TRUE;
			else
				m_bSuppressWelcome = FALSE;
		}
		else if (name == szUseStartupWizardOnLaunch)
		{
			num = wxAtoi(strValue);
			// Ignore config value, Version 3 always uses startup wizard
			m_bUseStartupWizardOnLaunch = TRUE;
		}
		else if (name == szBackupKBFlag)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1; // back it up
			if (num == 1)
				m_bAutoBackupKB = TRUE;
			else
				m_bAutoBackupKB = FALSE;
		}
		else if (name == szNoAutoSaveFlag)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0; // auto save
			if (num == 1)
				m_bNoAutoSave = TRUE;
			else
				m_bNoAutoSave = FALSE;
		}
		else if (name == szSpecialTextColor)
		{
			num = wxAtoi(strValue); // allow anything
			m_specialTextColor = Int2wxColour(num); // Int2wxColour() in helpers.h
		}
		else if (name == szReTranslnTextColor)
		{
			num = wxAtoi(strValue); // allow anything 
			m_reTranslnTextColor = Int2wxColour(num);// Int2wxColour() in helpers.h
		}
		else if (name == szTS_DOC_MINS)
		{
			num = wxAtoi(strValue);
			if (num < 0 || num > 60)
				num = 10;
			mins = num;
			secs = 0; // we don't use seconds any more
			//CTimeSpan ts(0,0,mins,secs); // MFC version has (days,hours,minutes,seconds)
			wxTimeSpan ts(0,mins,secs,0); // wxTimeSpan has (hours,minutes,seconds,milliseconds)
			m_timeSettings.m_tsDoc = ts;
		}
		else if (name == szTS_DOC_SECS)
		{
			// ignore seconds now
			;
		}
		else if (name == szTS_KB_MINS)
		{
			num = wxAtoi(strValue);
			if (num < 0 || num > 60)
				num = 15;
			mins = num;
			secs = 0; // we don't use seconds any more
			//CTimeSpan tsKB(0,0,mins,secs);// MFC version has (days,hours,minutes,seconds)
			wxTimeSpan tsKB(0,mins,secs,0); // wxTimeSpan has (hours,minutes,seconds,milliseconds)

			m_timeSettings.m_tsKB = tsKB;
		}
		else if (name == szTS_KB_SECS)
		{
			// ignore seconds now
			;
		}
		else if (name == szPhraseBoxMoves)
		{
			num = wxAtoi(strValue);
			if (num < 0 || num > 1000)
				num = 100; // default bold
			m_nMoves = num;
		}
		else if (name == szIsDocTimeButtonFlag)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1; // save by time span elapsed
			if (num == 1)
				m_bIsDocTimeButton = TRUE;
			else
				m_bIsDocTimeButton = FALSE;
		}
		else if (name == szNavTextColor)
		{
			num = wxAtoi(strValue); // allow anything
			// BEW changed 28Sep05; the font info at the start of config is enough,
			// it only adds confusion to reset what has already been set, and is
			// a potential source of error; so we will continue to Write this
			// value in the settings (for backwards compatibity), but we will ignore
			// the value read in, and only rely on the one that GetFontConfiguration
			// returns
		}

		// Ursula Wiesemann (Brazil) was having bizarre values of 3000 for xTopLeft and yTopLeft,
		// and bad values (e.g 160 by 24) for cxWZize and cyWSize, respectively. I cannot find any
		// reason for these - unless rogue code is overwriting memory. So I'll check for rogue
		// values on read of the basic config file, and on writing the basic config file, and
		// substitute safe ones when values for the top left would place the main window to the right
		// or below the point near the bottom right of the screen, or too narrow or high a window size
		// etc. (Bill Martin got, one time, -32000 values for top left in Unicode app!)

		else if (name == szTopLeftX)
		{
			num = wxAtoi(strValue);
			// TODO: the test below needs to be checked in WX version
			if (num < -6 || num > wndBotRight.x - 5) // -4 is maximized window's x value
			{
				bAdjusted = TRUE; // it's too far left or right
				num = 5;
			}
			m_ptViewTopLeft.x = num;
		}
		else if (name == szTopLeftY)
		{
			num = wxAtoi(strValue);
			// TODO: the test below needs to be checked in WX version
			if (num < -6 || num > wndBotRight.y - 5) // -4 is maximized window's y value
			{
				bAdjusted = TRUE; // it's too far up or down
				num = 5;
			}
			m_ptViewTopLeft.y = num;
		}
		else if (name == szWSizeCX)
		{
			num = wxAtoi(strValue);
			// TODO: the test below needs to be checked in WX version
			if (num < 200 || num > wndBotRight.x - wndTopLeft.x)
			{
				bAdjusted = TRUE; // it's too narrow or too wide
				num = wndBotRight.x - wndTopLeft.x -100;
			}
			m_szView.x = num;
		}
		else if (name == szWSizeCY)
		{
			num = wxAtoi(strValue);
			// TODO: the test below needs to be checked in WX version
			if (num < 200 || num > wndBotRight.y - wndTopLeft.y)
			{
				bAdjusted = TRUE; // it's too short or too long
				num = wndBotRight.y - wndTopLeft.y - 100;
			}
			m_szView.y = num;
		}
		else if (name == szFitFlag)
		{
			; // unused as of version 2.4.2
		}
		else if (name == szMarkerWrapsFlag)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1;
			if (num == 0)
				m_bMarkerWrapsStrip = FALSE;
			else
				m_bMarkerWrapsStrip = TRUE;
		}
		else if (name == szLoEnglishFlag)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1; 
			if (num == 0)
				m_bIsInches = FALSE; // in wx version page setup only has mm for margins, so we only use Metric for now
			else
				m_bIsInches = TRUE;
		}
		else if (name == szUsePortraitOrientation)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1; 
			if (num == 0)
				m_bIsPortraitOrientation = FALSE;
			else
				m_bIsPortraitOrientation = TRUE;
		}
		else if (name == szLastPageWidth)
		{
			num = wxAtoi(strValue);
			if (num <= 0) // whm MFC used if (num < 0), wx versions uses if (num <= 0) to set default to A4 if config has 0
				num = 8267;
			m_pageWidth = num;
			m_pageWidthMM = wxRound(num * thousandthsInchToMillimetres); // use the more prcise conversion factor for page size
		}
		else if (name == szLastPageLength)
		{
			num = wxAtoi(strValue);
			if (num <= 0) // whm MFC used if (num < 0), wx versions uses if (num <= 0) to set default to A4 if config has 0
				num = 11692;
			m_pageLength = num;
			m_pageLengthMM = wxRound(num * thousandthsInchToMillimetres); // use the more prcise conversion factor for page size
		}
		else if (name == szPaperSizeCode)
		{
			num = wxAtoi(strValue);
			if (num < 0 || num > 300)
				num = 9; // A4 
			m_paperSizeCode = num;
		}
		else if (name == szMarginTop)
		{
			num = wxAtoi(strValue);
			if (num < 0)
				num = 1000;
			m_marginTop = num;
			m_marginTopMM = (int)(num * config_only_thousandthsInchToMillimetres); // use the less precise conversion factor for margins
		}
		else if (name == szMarginBottom)
		{
			num = wxAtoi(strValue);
			if (num < 0)
				num = 1000;
			m_marginBottom = num;
			m_marginBottomMM = (int)(num * config_only_thousandthsInchToMillimetres); // use the less precise conversion factor for margins
		}
		else if (name == szMarginLeft)
		{
			num = wxAtoi(strValue);
			if (num < 0)
				num = 1000;
			m_marginLeft = num;
			m_marginLeftMM = (int)(num * config_only_thousandthsInchToMillimetres); // use the less precise conversion factor for margins
		}
		else if (name == szMarginRight)
		{
			num = wxAtoi(strValue);
			if (num < 0)
				num = 1000;
			m_marginRight = num;
			m_marginRightMM = (int)(num * config_only_thousandthsInchToMillimetres); // use the less precise conversion factor for margins
		}
		else if (name == szPunctuation) // (now very) old punctuation system
		{
			; //m_punctuation[0] = strValue; // retain in case a pre-2001 config file needs to be read
		}
		else if (name == szPunctAsWordBuilding)
		{
			; //m_punctWordBuilding[0] = strValue; // ditto
		}
		else if (name == szPhraseBoxExpansionMultiplier)
		{
			num = wxAtoi(strValue);
			if (num < 5 || num > 30)
				num = 10;
			gnExpandBox = num;
		}
		else if (name == szTooNearEndMultiplier)
		{
			num = wxAtoi(strValue);
			if (num < 3 || num > 6)
				num = 3;
			gnNearEndFactor = num;
		}
		else if (name == szRTL_Layout)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0; // LTR layout is safe default
			if (num == 1)
				m_bRTL_Layout = TRUE;
			else
				m_bRTL_Layout = FALSE;
			gbRTL_Layout = m_bRTL_Layout; // set the global at same time
		}
		else if (name == szSuppressTargetHighlighting)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0; // don't suppress it
			if (num == 1)
				m_bSuppressTargetHighlighting = TRUE;
			else
				m_bSuppressTargetHighlighting = FALSE;
		}
		else if (name == szAutoInsertionsHighlightColor)
		{
			num = wxAtoi(strValue); // allow anything 
			m_AutoInsertionsHighlightColor = Int2wxColour(num); // Int2wxColour() in helpers.h
		}

#ifdef _RTL_FLAGS
		else if (name == szRTLSource)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0; // LTR reading text is safe default
			if (num == 1)
				m_bSrcRTL = TRUE;
			else
				m_bSrcRTL = FALSE;
		}
		else if (name == szRTLTarget)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0; // LTR reading text is safe default
			if (num == 1)
				m_bTgtRTL = TRUE;
			else
				m_bTgtRTL = FALSE;
		}
		else if (name == szRTLNavText)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0; // LTR reading text is safe default
			if (num == 1)
				m_bNavTextRTL = TRUE;
			else
				m_bNavTextRTL = FALSE;
		}
#endif
		// added for v2.0.1 and onwards
		else if (name == szLastSourceExportPath)
		{
			m_lastSrcExportPath = strValue;
		}
		else if (name == szKBExportPath)
		{
			m_kbExportPath = strValue;
		}
		else if (name == szRetranslationReportPath)
		{
			m_retransReportPath = strValue;
		}
		else if (name == szRTFExportPath)
		{
			m_rtfExportPath = strValue;
		}

		// added for v1.4.1 and onwards
		else if (name == szSFMescapechar)
		{
			;	// disregard the sfm esc char read in from config as of version 3
				// gSFescapechar is always backslash
		}

		else
		{
			if (!data.IsEmpty())
			{
				wxString error;
				error = error.Format(_(
				"Warning: Unrecognized Basic settings field; unmatched name is: %s\n"),name.c_str());
				wxMessageBox(error, _T(""), wxICON_INFORMATION);
			}
		}
	} while (!pf->Eof());
	// BEW changed 06Mar06, on Bill's recommendation - ie. don't tell the user when an adjustment
	// was required, just do it
	//if (bAdjusted)
	//{
	//	AfxMessageBox(IDS_WINDOW_LOC_ERR,MB_ICONINFORMATION);
	//}

#ifdef _UNICODE
	// do the to set the punct pairs arrays, the equivalent calls for the ANSI
	// version are done in the "else if" blocks above
	TwoStringsToPunctPairs(m_punctPairs,strPunctPairsSrcSet,strPunctPairsTgtSet);
	TwoStringsToTwoPunctPairs(m_twopunctPairs,strTwoPunctPairsSrcSet,strTwoPunctPairsTgtSet);
#endif
	//set the src and tgt punctuation sets to be used for parsing
	GetPunctuationSets(m_punctuation[0], m_punctuation[1]);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: the App's GetProjectConfiguration() if SHIFT key is being held down, 
/// CCaseEquivPageCommon::DoBnClickedSrcSetEnglish(), CCaseEquivPageCommon::DoBnClickedTgtSetEnglish(), 
/// CCaseEquivPageCommon::DoBnClickedGlossSetEnglish(), and CProjectPage::OnWizardPageChanging() 
/// moving forward and creating a <New Project>. Sets the default source, target and gloss 
/// language case equivalences to the English/Latin alphabetical set.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::SetDefaultCaseEquivalences()
// ASCII defaults, for when a project first is created
{
	m_srcLowerCaseChars = _T("abcdefghijklmnopqrstuvwxyz");
	m_srcUpperCaseChars = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	m_tgtLowerCaseChars = m_srcLowerCaseChars;
	m_tgtUpperCaseChars = m_srcUpperCaseChars;
	m_glossLowerCaseChars = m_srcLowerCaseChars;
	m_glossUpperCaseChars = m_srcUpperCaseChars;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: the App's GetBasicConfigFileSettings() if the SHIFT key is being held down, 
/// or if the basic configuration file could not be read.
/// SetDefaults() establishes reasonable defaults that can be used for fonts, punctuation and other 
/// settings until the user sets more suitable defaults using the Edit | Preferences dialog.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::SetDefaults()
{
	// SetDefaults() is only called if the user holds down SHIFT key as app starts up, or if the
	// basic config file cannot be read successfully, i.e., the call to
	// GetConfigurationFile(szBasicConfiguration,m_workFolderPath,1) in the App's Initialize()
	// function returns FALSE.
	// 
	// Although InitializeFonts() always gets called from OnInit(), SetDefaults() can also be
	// invoked if there was a bad read of the basic config file, so we call InitializeFonts()
	// again here to correct any garbage settings that may have been set during a bad read of
	// the config file.
	InitializeFonts();
	InitializePunctuation();

	// TODO: Should some/all of the following be put in Adapt_ItConstants.h ???
	m_nMaxToDisplay = 300;
	m_nPrecedingContext = 50;
	m_nFollowingContext = 30;
	m_curLeading = 32;
	m_curLMargin = 16;// whm changed default to 16 for better visibility of any notes icon at left margin
	m_curGapWidth = 16;
	m_bSuppressFirst = TRUE;
	m_bSuppressLast = TRUE;
	gnExpandBox = 8;
	gnNearEndFactor = 3;
	m_bHidePunctuation = FALSE;

	m_bUseStartupWizardOnLaunch = TRUE;
	m_bAutoBackupKB = TRUE;
	m_bNoAutoSave = TRUE; // whm changed to TRUE in wx (MFC has FALSE here but TRUE in App's constructor as default)
	
	m_ptViewTopLeft.x = 20;
	m_ptViewTopLeft.y = 20;
	m_szView.x = 640;
	m_szView.y = 580;
	m_bMarkerWrapsStrip = TRUE; 
	m_bZoomed = FALSE;

	m_bIsInches = FALSE; // in wx version's Windows page setup only has mm for margins, so we set default to Metric for now
	m_bIsPortraitOrientation = TRUE;
	m_paperSizeCode = 9; // always mapped to MFC's enum for paper size code = 9; // A4

	// MFC used default values in MM_LOENGLISH mapping mode (thousandths of an inch)
	// The wx version uses mainly metric millimeter values internally for printing and print preview
	m_pageWidth = 8267;		// 210mm / 0.0254mm/in (result truncated to whole int) in thousandths of an inch = 8267 A4
	m_pageWidthMM = wxRound(m_pageWidth * thousandthsInchToMillimetres); // 210mm	A4 page width in mm
	m_pageLength = 11692;	// 297mm / 0.0254mm/in (result truncated to whole int) in thousandths of an inch = 11692 A4
	m_pageLengthMM = wxRound(m_pageLength * thousandthsInchToMillimetres); // 297mm A4 page height in mm
	// the default margin values below use less precise conversion factor of 0.0250mm/in
	m_marginTop = 1000;		// 25mm / 0.0250mm/in (result truncated to whole int) in thousandths of an inch = 1000; // one inch
	m_marginTopMM = (int)(m_marginTop * config_only_thousandthsInchToMillimetres); // 25mm top margin in mm
	m_marginBottom = 1000;	// 25mm / 0.0250mm/in (result truncated to whole int) in thousandths of an inch = 1000; // one inch
	m_marginBottomMM = (int)(m_marginBottom * config_only_thousandthsInchToMillimetres); // 25mm bottom margin in mm
	m_marginLeft = 1000;	// 25mm / 0.0250mm/in (result truncated to whole int) in thousandths of an inch = 1000; // one inch
	m_marginLeftMM = (int)(m_marginLeft * config_only_thousandthsInchToMillimetres); // 25mm left margin in mm
	m_marginRight = 1000;	// 25mm / 0.0250mm/in (result truncated to whole int) in thousandths of an inch = 1000; // one inch
	m_marginRightMM = (int)(m_marginRight * config_only_thousandthsInchToMillimetres); // 25mm right margin in mm

	// standard format marker's escape char
	gSFescapechar = _T('\\');

	m_nCurDelay = 0; // default is no delay (zero ticks)
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pf   <- pointer to the wxTextFile that is being written as the config file
/// \remarks
/// Called from: the App's WriteConfigurationFile(). 
/// Writes the project settings part of a project configuration file. The text file is opened by 
/// the caller. 
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::WriteProjectSettingsConfiguration(wxTextFile* pf)
{
	// wx version combines ANSI and UNICODE parts in common to simplify this function
	wxString data = _T("");
	wxString tab = _T("\t");
	wxString number;

	data.Empty();
	data << szSourceLanguageName << tab << m_sourceName;
	pf->AddLine(data);

	data.Empty();
	data << szTargetLanguageName << tab << m_targetName;
	pf->AddLine(data);

	// now for the view stuff
	data.Empty();
	data << szDefaultTablePath << tab << m_defaultTablePath;
	pf->AddLine(data);

	data.Empty();
	data << szLastExportPath << tab << m_lastExportPath;
	pf->AddLine(data);

	data.Empty();
	data << szLastSourceFileFolder << tab << m_lastSourceFileFolder;
	pf->AddLine(data);

#ifndef _UNICODE
	// ANSI
	wxString s;
	PunctPairsToString(m_punctPairs,s);
	data.Empty();
	data << szPunctPairs << tab << s;
	pf->AddLine(data);

	TwoPunctPairsToString(m_twopunctPairs,s);
	data.Empty();
	data << szTwoPunctPairs << tab << s;
	pf->AddLine(data);
#else
	// UNICODE
	// From version 2.3.0 we no longer write out the encoding ID value for the punctuation fields
	wxString src;
	wxString tgt;
	PunctPairsToTwoStrings(m_punctPairs,src,tgt);

	data.Empty();
	data << szPunctPairsSrc << tab << src;
	pf->AddLine(data);

	data.Empty();
	data << szPunctPairsTgt << tab << tgt;
	pf->AddLine(data);

	TwoPunctPairsToTwoStrings(m_twopunctPairs,src,tgt);

	data.Empty();
	data << szTwoPunctPairsSrc << tab << src;
	pf->AddLine(data);

	data.Empty();
	data << szTwoPunctPairsTgt << tab << tgt;
	pf->AddLine(data);
#endif // _UNICODE

	data.Empty();
	data << szSpecialTextColor << tab <<  WxColour2Int(m_specialTextColor);
	pf->AddLine(data);

	data.Empty();
	data << szReTranslnTextColor << tab << WxColour2Int(m_reTranslnTextColor);
	pf->AddLine(data);

	data.Empty();
	data << szNavTextColor << tab <<  WxColour2Int(m_navTextColor);
	pf->AddLine(data);

	if (m_bBackupDocument)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szBackupDocument << tab << number;
	pf->AddLine(data);

	if (m_bRTL_Layout)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szRTL_Layout << tab << number;
	pf->AddLine(data);

	data.Empty();
	data <<	szSFMescapechar << tab;
	data << gSFescapechar;
	pf->AddLine(data);

	if (gbSfmOnlyAfterNewlines)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szSFMafterNewlines << tab << number;
	pf->AddLine(data);

#ifdef _UNICODE
// the following will only be in the _UNICODE version (_RTL_FLAGS is
// undefined whenever _UNICODE is undefined)
#ifdef _RTL_FLAGS

	if (m_bSrcRTL)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szRTLSource << tab << number;
	pf->AddLine(data);

	if (m_bTgtRTL)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szRTLTarget << tab << number;
	pf->AddLine(data);

	if (m_bNavTextRTL)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szRTLNavText << tab << number;
	pf->AddLine(data);

#endif // _RTL_FLAGS
#endif // _UNICODE
	
	// now for the auto-capitalization stuff
	data.Empty();
	data << szLowerCaseSourceChars << tab << m_srcLowerCaseChars;
	pf->AddLine(data);

	data.Empty();
	data << szUpperCaseSourceChars << tab << m_srcUpperCaseChars;
	pf->AddLine(data);

	data.Empty();
	data << szLowerCaseTargetChars << tab << m_tgtLowerCaseChars;
	pf->AddLine(data);

	data.Empty();
	data << szUpperCaseTargetChars << tab << m_tgtUpperCaseChars;
	pf->AddLine(data);

	data.Empty();
	data << szLowerCaseGlossChars << tab << m_glossLowerCaseChars;
	pf->AddLine(data);

	data.Empty();
	data << szUpperCaseGlossChars << tab << m_glossUpperCaseChars;
	pf->AddLine(data);

	if (gbAutoCaps)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szAutoCapitalization << tab << number;
	pf->AddLine(data);

	if (gbSrcHasUcAndLc)	// not in MFC version; whm added for wx version
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szSrcHasUcAndLc << tab << number;
	pf->AddLine(data);

	data.Empty();
	data << szFontSizeForDialogs << tab << m_dialogFontSize;
	pf->AddLine(data);

	switch (gProjectSfmSetForConfig) // whm ammended 6May05 to use gProjectSfmSetForConfig 
	{
		case UsfmOnly: number = szUsfmOnly;break;
		case PngOnly: number = szPngOnly;break;
		case UsfmAndPng: number = szUsfmAndPng;break;
		default: number = szUsfmOnly;
	}
	data.Empty();
	data << szUseSFMarkerSet << tab << number;
	pf->AddLine(data);

	data.Empty();
	data << szUseFilterMarkers << tab << gProjectFilterMarkersForConfig; // whm added 13May05
	pf->AddLine(data);

	if (m_bChangeFixedSpaceToRegularSpace)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szChangeFixedSpaceToRegSpace << tab << number;
	pf->AddLine(data);
	
	// order is important for the next two - m_bBookMode must be in the config file
	// earlier than m_nBookIndex, since the latter must be used to restore the pair pointer
	// when book mode is on
	if (m_bBookMode)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szBookMode << tab << number;
	pf->AddLine(data);

	data.Empty();
	data << szBookIndex << tab <<  m_nBookIndex;
	pf->AddLine(data);

	// wx version: m_bSaveAsXML must always be true
	number = _T("1");
	data.Empty();
	data << szSaveAsXML << tab << number;
	pf->AddLine(data);

	data.Empty();
	data << szSilConverterName << tab << m_strSilEncConverterName;
	pf->AddLine(data);

	if (m_bSilConverterDirForward)
		number = _T("1");
	else
		number = _T("0");
    data.Empty();
	data << szSilConverterDirForward << tab << number;
    pf->AddLine(data);

#ifndef USE_SIL_CONVERTERS
	number = _T("0");
#endif
#ifdef USE_SIL_CONVERTERS
	number.Empty();
	number <<  (int)m_eSilConverterNormalizeOutput; // use the int value of the m_eSilConverterNormalizeOutput enum 
#endif
	data.Empty();
	data << szSilConverterNormalize << tab << number;
	pf->AddLine(data);
	
	if (gbLegacySourceTextCopy)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szLegacyCopyForPhraseBox << tab << number;
	pf->AddLine(data);

	if (gbAdaptBeforeGloss)
		number = _T("1");
	else
		number = _T("0");
	data.Empty();
	data << szDoAdaptingBeforeGlossing_InVerticalEdit << tab << number;
	pf->AddLine(data);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pf   -> pointer to the wxTextFile that is being read as the config file
/// \remarks
/// Called from: the App's GetConfigurationFile(). 
/// Reads the project settings part of a project configuration file. 
/// For the most part, the project settings entries can be in any order, as long as the 
/// format of each line's entry is correct: that is, a name, tab, and string value. The 
/// exception is that the source language punctuation pairs data must be read before the
/// target punctuation pairs data.
/// NOTE: This function expects pf to already have been opened and the internal
/// pointer to be moved to the "ProjectSettings" line in the config file. The
/// wxWidgets version holds the entire config file in memory and reads individual
/// text lines from memory.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::GetProjectSettingsConfiguration(wxTextFile* pf)
{
	wxString data = _T("");
	wxString name;
	wxString strValue;
	int num; // for the string converted to a number
	bool bExistsUsfmSettingInConfig = FALSE; // whm added 2Feb05

#ifdef _UNICODE
	wxString strPunctPairsSrcSet;
	wxString strPunctPairsTgtSet;
	wxString strTwoPunctPairsSrcSet;
	wxString strTwoPunctPairsTgtSet;
	bool bAddedLeftDoubleQuote = FALSE;
	bool bAddedRightDoubleQuote = FALSE;
	bool bAddedLeftSingleQuote = FALSE;
	bool bAddedRightSingleQuote = FALSE;
#endif
				 
	do
	{
		data = pf->GetNextLine();
		
#ifdef _UNICODE
		ChangeNRtoUnicode(data);
#endif
		GetValue(data, strValue, name);
		if (name == szSourceLanguageName)
		{
			m_sourceName = strValue;
		}
		else if (name == szTargetLanguageName)
		{
			m_targetName = strValue;
		}
		else if (name == szDefaultTablePath)
		{
			m_defaultTablePath = strValue; // can be empty
		}
		else if (name == szLastExportPath)
		{
			// BEW added 16Oct05, so we can have a 'fixed' m_lastExportPath too...
			// compare the initial part of the path with what is in the initial
			// part of the path as a result of the MakeForeignBasicConfigFileSafe() call
			// being done before using the basic config file -- if a basic config file from
			// someone else was copied and used, the latter function will have made the
			// last export path safe, and a copied project file from another user does not
			// get made safe so we could be about to replace a good path with an invalid one
			// so we'll compare the part of the paths up to the "Adapt It" part of the work
			// folder name, and if different we'll use the one already in m_lastExportPath
			wxString workfolder = PathSeparator + _T("Adapt It");
			int nFound1 = strValue.Find(workfolder);
			wxString leftStrFromProjectConfigFile = strValue.Left(nFound1);
			int nFound2 = m_lastExportPath.Find(workfolder);
			wxString leftStrFromBasicConfigFile = m_lastExportPath.Left(nFound2);
			if (leftStrFromProjectConfigFile != leftStrFromBasicConfigFile)
			{
				// they are different substrings, so we can assume that the ...\<profile name>\...
				// part of the path differs between them (ie. different users), and so we'll go
				// with the existing one
				;
			}
			else
			{
				// identical, so we can safely use the project config file's strValue to overwrite
				// what is already in m_lastExportPath as a result of reading the basic config file
				// earlier
				m_lastExportPath = strValue;
			}
		}
		else if (name == szLastSourceFileFolder)
		{
			m_lastSourceFileFolder = strValue;
		}
		// the next four are redundant from 2.3.0 and onwards, but must be retained
		// in case the user uses a later version to read a config file produced by
		// a pre-2.3.0 version of the application
		else if (name == szSrcPunctuation) // new punctuation system (split)
		{
			; // m_punctuation[0] = strValue; // do nothing with it
		}
		else if (name == szSrcPunctAsWordBuilding)
		{
			; // m_punctWordBuilding[0] = strValue;
		}
		else if (name == szTgtPunctuation)
		{
			; // m_punctuation[1] = strValue;
		}
		else if (name == szTgtPunctAsWordBuilding)
		{
			; // m_punctWordBuilding[1] = strValue;
		}
#ifndef _UNICODE
		else if (name == szPunctPairs)
		{
			// For Windows, ensure curly quotes are supported in ANSI version.
			// When first converted to wxWidgets, GTK's text control was failing when use SetValue() with 
			// any of these upper ascii values in the punctuation string. A partial solution was to insure 
			// that SetEncoding(wxFONTENCODING_CP1252) was called on the fonts involved. Note: SetEncoding() 
			// is an "undocumented" method of wxFont. The documented SetDefaultEncoding() did not seem to
			// work. Also setting the encoding to wxFONTENCODING_ISO8859_1 did not work (it eliminated
			// the "Failed to set text in text control" message, but showed the smart quotes as boxes
			// with the hex values in them, instead of the actual quote characters).
			// Because it was only a partial solution and problems seemed to continually crop up, in the 
			// end I decided to include smart quotes only in the wxMSW ANSI version and not in wxGTK builds.
#ifdef __WXMSW__
			wxString additions;
			additions.Empty();
			wxString ch = _T("."); // must not be empty, so put any char in it
			unsigned char thechar = 147; // left double quotation mark
			ch.SetChar(0,thechar);
			if (strValue.Find(ch) == -1) // not found
			{
				additions += ch;
				additions += ch;
			}
			thechar = 148; // right double quotation mark
			ch.SetChar(0,thechar);
			if (strValue.Find(ch) == -1) // not found
			{
				additions += ch;
				additions += ch;
			}
			thechar = 145; // left single quotation mark
			ch.SetChar(0,thechar);
			if (strValue.Find(ch) == -1) // not found
			{
				additions += ch;
				additions += ch;
			}
			thechar = 146; // right single quotation mark
			ch.SetChar(0,thechar);
			if (strValue.Find(ch) == -1) // not found
			{
				additions += ch;
				additions += ch;
			}
			strValue += additions;
#else
			// In non-windows ANSI versions, remove any smart quotes that exist in the
			// punctuation string. 
			// Possible Problem: We cannot assume that the user would always want smart 
			// quotes converted to regular quotes in the punctuation lists. Maybe they used
			// smart quotes for all their quotes requirements, and used a regular single quote 
			// (apostrophe) as a word building glottal character, for example.
			
			// whm note: For wx ANSI version, when compiled for wxGTK, we should
			// check the punct strings for illegal 8-bit chars, specifically for MS' smart
			// quotes which cause problems because the wxGTK library internally
			// keeps strings in UTF-8. We'll quietly convert any smart quotes we find
			// to normal quotes.
			int ct;
			bool hackedFontCharPresent = FALSE;
			int hackedCt = 0;
			wxString hackedStr;
			hackedStr.Empty();
			const wxChar lsqm(-111);
			const wxChar rsqm(-110);
			const wxChar ldqm(-109);
			const wxChar rdqm(-108);
			// replace any smart quote chars with a space - if both src and tgt punct have smart
			// quotes we end up with both being spaces, but I think StringToPunctPairs and its
			// complement PunctPairsToString can handle it.
			strValue.Replace(&lsqm,_T(" "));
			strValue.Replace(&rsqm,_T(" "));
			strValue.Replace(&ldqm,_T(" "));
			strValue.Replace(&rdqm,_T(" "));
			for (ct = 0; ct < (int) strValue.Length(); ct++)
			{
				int charValue;
				charValue = (int)strValue.GetChar(ct);
				// whm note: The decimal int value of extended ASCII chars is represented
				// as its decimal extended value minus 256, making any extended characters
				// be represented as negative numbers
				if (charValue < 0)
				{
					//// we have an extended ASCII character in the string
					//switch(charValue)
					//{
					//case -111:	strValue.Remove(ct,1);
					//			break;
					//case -110:	strValue.Remove(ct,1);
					//			break;
					//case -109:	strValue.Remove(ct,1);
					//			break;
					//case -108:	strValue.Remove(ct,1);
					//			break;
					//default:
					//	{
							// the default case indicates there is a non-smart quote extended
							// ASCII character present. In this case we should warn the user
							// that the data cannot be handled until it is converted to Unicode,
							// then delete the extended punct char from the list.
							// TODO: implement warning message and delete routine instead of Beep
							//::wxBell();
							hackedFontCharPresent = TRUE;
							hackedCt++;
							if (hackedCt < 10)
							{
								hackedStr += _T("\n   character with ASCII value: ");
								hackedStr << (charValue+256);
							}
							else if (hackedCt == 10)
								hackedStr += _T("...\n");
					//	}
					//}
				}
			}

			// we assume that regular quote marks are already in the punctuation string if the user
			// desired them to be there

			if (hackedFontCharPresent && !gbHackedDataCharWarningGiven)
			{
				gbHackedDataCharWarningGiven = TRUE;
				wxString msg2 = _("\nYou should not use this non-Unicode version of Adapt It.\nYour data should first be converted to Unicode using TecKit\nand then you should use the Unicode version of Adapt It.");
				wxString msg1 = _("Extended 8-bit ASCII characters were detected in the\nProject Configuration File\'s punctuation string (see below):");
				msg1 += hackedStr + msg2;
				wxMessageBox(msg1,_("Warning: Invalid Characters Detected"),wxICON_WARNING);
			}
#endif
			// wxGTK uses UTF-8 internally in its wxTextCtrl widgets, so let's try converting the punctuation
			// string to UTF-8 for wxGTK ANSI builds.
//#ifdef __GNUG__
//			// The following converts strValue to 
//			wxConvUTF8.cWC2MB(wxConvCurrent->cMB2WC(strValue));  
//#endif
			// set flag if apostrophe is designated as a punctuation character (this makes
			// ParseWord() do its job smarter within TokenizeText())
			int found = strValue.Find(_T('\'')); // look for vertical ordinary quote (ie. apostrophe)
			if (found >= 0)
				m_bSingleQuoteAsPunct = TRUE;
			else
				m_bSingleQuoteAsPunct = FALSE;

			StringToPunctPairs(m_punctPairs,strValue);
		}
		else if (name == szTwoPunctPairs)
		{
			StringToTwoPunctPairs(m_twopunctPairs,strValue);
		}
#else // _UNICODE version
		else if (name == szPunctPairsSrc)
		{
			// make sure curly quotes are supported as punctuation
			wxString additions;
			additions.Empty();
			wxString ch = _T("."); // must be initialized to something, else SetChar() fails
			ch.SetChar(0,L'\x201C'); // hex for left double quotation mark
			if (strValue.Find(ch) == -1) // not found
			{
				bAddedLeftDoubleQuote = TRUE;
				additions += ch;
			}
			ch.SetChar(0,L'\x201D'); // hex for right double quotation mark
			if (strValue.Find(ch) == -1) // not found
			{
				bAddedRightDoubleQuote = TRUE;
				additions += ch;
			}
			ch.SetChar(0,L'\x2018'); // hex for left single quotation mark
			if (strValue.Find(ch) == -1) // not found
			{
				bAddedLeftSingleQuote = TRUE;
				additions += ch;
			}
			ch.SetChar(0,L'\x2019'); // hex for right single quotation mark
			if (strValue.Find(ch) == -1) // not found
			{
				bAddedRightSingleQuote = TRUE;
				additions += ch;
			}
			int found = strValue.Find(_T('\'')); // look for vertical ordinary quote (ie. apostrophe)
			if (found >= 0)
				m_bSingleQuoteAsPunct = TRUE;
			else
				m_bSingleQuoteAsPunct = FALSE;
			strValue = additions + strValue;

			strPunctPairsSrcSet = strValue;
		}
		else if (name == szPunctPairsTgt)
		{
			// THIS BLOCK WORKS RIGHT ONLY WHEN THE
			// SOURCE SET WAS PROCESSED EARLIER - so
			// ORDER is important in the config file for unicode punctuation
			// support - so don't alter order of src & tgt punct lines

			// make sure curly quotes are supported as punctuation
			wxString additions;
			additions.Empty();
			wxString ch = _T("."); // must be initialized to something, else SetAt() fails
			ch.SetChar(0,L'\x201C'); // hex for left double quotation mark
			if (bAddedLeftDoubleQuote) // source had this added
			{
				 // so target has to have it too
				additions += ch;
			}
			ch.SetChar(0,L'\x201D'); // hex for right double quotation mark
			if (bAddedRightDoubleQuote)
			{
				// add it if the source had it added
				additions += ch;
			}
			ch.SetChar(0,L'\x2018'); // hex for left single quotation mark
			if (bAddedLeftSingleQuote)
			{
				// add it if the source had it added
				additions += ch;
			}
			ch.SetChar(0,L'\x2019'); // hex for right single quotation mark
			if (bAddedRightSingleQuote)
			{
				// add it if the source had it added
				additions += ch;
			}
			int found = strValue.Find(_T('\'')); // look for vertical ordinary quote (ie. apostrophe)
			if (found >= 0)
				m_bSingleQuoteAsPunct = TRUE;
			else
				m_bSingleQuoteAsPunct = FALSE;
			strValue = additions + strValue;

			strPunctPairsTgtSet = strValue;
		}
		else if (name == szTwoPunctPairsSrc)
		{
			strTwoPunctPairsSrcSet = strValue;
		}
		else if (name == szTwoPunctPairsTgt)
		{
			strTwoPunctPairsTgtSet = strValue;
		}
#endif
		else if (name == szSpecialTextColor)
		{
			num = wxAtoi(strValue); // allow anything
			m_specialTextColor = Int2wxColour(num); // Int2wxColour() in helpers.h
		}
		else if (name == szReTranslnTextColor)
		{
			num = wxAtoi(strValue); // allow anything 
			m_reTranslnTextColor = Int2wxColour(num); // Int2wxColour() in helpers.h
		}
		else if (name == szNavTextColor)
		{
			num = wxAtoi(strValue); // allow anything
			// BEW changed 28Sep05; the font info at the start of config is enough,
			// it only adds confusion to reset what has already been set, and is
			// a potential source of error; so we will continue to Write this
			// value in the settings (for backwards compatibity), but we will ignore
			// the value read in, and only rely on the one that GetFontConfiguration
			// returns
		}
		else if (name == szPunctuation) // old punctuation system
		{
			; // do nothing, (retained for reading legacy config files safely)
		}
		else if (name == szPunctAsWordBuilding)
		{
			; // ditto
		}
		else if (name == szBackupDocument)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1;
			if (num == 1)
				m_bBackupDocument = TRUE;
			else
				m_bBackupDocument = FALSE;
		}
		else if (name == szRTL_Layout)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0; // LTR layout is safe default
			if (num == 1)
				m_bRTL_Layout = TRUE;
			else
				m_bRTL_Layout = FALSE;
			gbRTL_Layout = m_bRTL_Layout; // set the global at same time
		}
		else if (name == szSFMescapechar)
		{
			;	// disregard the sfm esc char read in from config as of version 3
				//gSFescapechar is always backslash
		}
		else if (name == szSFMafterNewlines)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0;
			if (num == 1)
				gbSfmOnlyAfterNewlines = TRUE;
			else
				gbSfmOnlyAfterNewlines = FALSE;
		}

#ifdef _RTL_FLAGS
		else if (name == szRTLSource)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0; // LTR reading text is safe default
			if (num == 1)
				m_bSrcRTL = TRUE;
			else
				m_bSrcRTL = FALSE;
		}
		else if (name == szRTLTarget)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0; // LTR reading text is safe default
			if (num == 1)
				m_bTgtRTL = TRUE;
			else
				m_bTgtRTL = FALSE;
		}
		else if (name == szRTLNavText)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0; // LTR reading text is safe default
			if (num == 1)
				m_bNavTextRTL = TRUE;
			else
				m_bNavTextRTL = FALSE;
		}
#endif
		// autocapitalization stuff
		else if (name == szLowerCaseSourceChars)
		{
			m_srcLowerCaseChars = strValue;
		}
		else if (name == szUpperCaseSourceChars)
		{
			m_srcUpperCaseChars = strValue;
		}
		else if (name == szLowerCaseTargetChars)
		{
			m_tgtLowerCaseChars = strValue;
		}
		else if (name == szUpperCaseTargetChars)
		{
			m_tgtUpperCaseChars = strValue;
		}
		else if (name == szLowerCaseGlossChars)
		{
			m_glossLowerCaseChars = strValue;
		}
		else if (name == szUpperCaseGlossChars)
		{
			m_glossUpperCaseChars = strValue;
		}
		else if (name == szAutoCapitalization)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1;
			if (num == 1)
				gbAutoCaps = TRUE;
			else
				gbAutoCaps = FALSE;
		}
		else if (name == szSrcHasUcAndLc) // whm added 12Aug04 - not in MFC version
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1;
			if (num == 1)
				gbSrcHasUcAndLc = TRUE;
			else
				gbSrcHasUcAndLc = FALSE;
		}
		else if (name == szFontSizeForDialogs)
		{
			num = wxAtoi(strValue);
			if (num < 10 || num > 24)
				num = 12;
			m_dialogFontSize = num;
		}
		else if (name == szUseSFMarkerSet) 
		{
			num = wxAtoi(strValue);
			if (strValue == szUsfmOnly)
				gProjectSfmSetForConfig = UsfmOnly;
			else if (strValue == szPngOnly)
				gProjectSfmSetForConfig = PngOnly;
			else if (strValue == szUsfmAndPng)
				gProjectSfmSetForConfig = UsfmAndPng;
			else
				gProjectSfmSetForConfig = UsfmOnly;
			bExistsUsfmSettingInConfig = TRUE;
		}
		else if (name == szUseFilterMarkers) // whm added 13May05
		{
			if (!strValue.IsEmpty())
			{
				gProjectFilterMarkersForConfig = strValue;
			}
			else 
			{
				// config has szUseFilterMarkers label but no filter markers following it
				// so use the gCurrentFilterMarkers value for gProjectFilterMarkersForConfig
				gProjectFilterMarkersForConfig = gCurrentFilterMarkers;
			}
		}
		else if (name == szChangeFixedSpaceToRegSpace) // whm added 3Apr05
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1;
			if (num == 1)
				m_bChangeFixedSpaceToRegularSpace = TRUE;
			else
				m_bChangeFixedSpaceToRegularSpace = FALSE;
		}
		else if (name == szBookMode)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 0; // off, if there was an error
			if (num == 1)
				m_bBookMode = TRUE;
			else
				m_bBookMode = FALSE;
		}
		else if (name == szBookIndex)
		{
			num = wxAtoi(strValue);
			if ((num < -1 || num > 66))
				num = -1; // -1, if there was an error
			m_nBookIndex = num;
			
			// we must now restore the pair pointer, if book mode is on
			// and the folder path
			if (m_bBookMode && !m_bDisableBookMode)
			{
				if (m_nBookIndex == -1)
					goto t;

				// whm added 30Nov07 check to insure the m_nBookIndex is not out of
				// range for the number of Bible books in m_pBibleBooks. If it is out
				// of range (i.e., user changed to a custom books.xml or edited the
				// project config file's BookIndexValue to an erroneous value), turn
				// off book mode and set flags to default values.
				if (m_nBookIndex < -1 || (m_nBookIndex + 1) > (int)(*m_pBibleBooks).GetCount())
				{
					goto t;
				}

				m_pCurrBookNamePair = ((BookNamePair*)(*m_pBibleBooks)[m_nBookIndex]);
				m_bibleBooksFolderPath = m_curAdaptionsPath + PathSeparator +
																m_pCurrBookNamePair->dirName;
			}
			else
			{
t:				m_pCurrBookNamePair = NULL;
				m_bibleBooksFolderPath.Empty();
				m_bBookMode = FALSE;
				m_nBookIndex = -1;
				m_nLastBookIndex = m_nDefaultBookIndex;
			}
		}
		else if (name == szSaveAsXML) // BEW added 04Aug05
		{
			num = wxAtoi(strValue);
			// wx version: we only allow a value of TRUE for wx version
			m_bSaveAsXML = TRUE;
		}
		else if (name == szSilConverterName)	// rde added 03 Apr 06
		{
			m_strSilEncConverterName = strValue;
		}
		else if (name == szSilConverterDirForward)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1;
			if (num == 0)
				m_bSilConverterDirForward = FALSE;
			else
				m_bSilConverterDirForward = TRUE;
		}
		else if (name == szSilConverterNormalize)
		{
			num = wxAtoi(strValue);
#ifdef USE_SIL_CONVERTERS
			m_eSilConverterNormalizeOutput = (NormalizeFlags)num;
#endif
		}
		else if (name == szLegacyCopyForPhraseBox)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1;
			if (num == 0)
				gbLegacySourceTextCopy = FALSE;
			else
				gbLegacySourceTextCopy = TRUE;
		}
		else if (name == szDoAdaptingBeforeGlossing_InVerticalEdit)
		{
			num = wxAtoi(strValue);
			if (!(num == 0 || num == 1))
				num = 1;
			if (num == 0)
				gbAdaptBeforeGloss = FALSE;
			else
				gbAdaptBeforeGloss = TRUE;
		}
		else
		{
			if (!data.IsEmpty())
			{
				wxString error = _(
				"Warning: Unrecognized project settings field; unmatched name is: ") + name;
				wxMessageBox(error, _T(""), wxICON_INFORMATION);
			}
		}
	} while (!pf->Eof());

	// whm ammended 6May05
	if (!bExistsUsfmSettingInConfig)
	{
		// If no UseSFMarkerSet is encoded in the project config file, we assume the user
		// is upgrading from a previous Adapt It version. In this case, all previous documents
		// created by previous version will have been created using a PngOnly inventory of
		// standard format markers, therefore we assign both gCurrentSfmSet and 
		// gProjectSfmSetForConfig to be PngOnly. This setting remains PngOnly until the user 
		// explicitly changes to a different SfmSet using Edit Preferences USFM tab, or the 
		// USFM page in the Start Working... wizard upon creation of a new project.
		gCurrentSfmSet = PngOnly;
		gProjectSfmSetForConfig = gCurrentSfmSet; // whm added 6May05
		gCurrentFilterMarkers = PngFilterMarkersStr; // whm added 13May05
		gProjectFilterMarkersForConfig = gCurrentFilterMarkers; // whm added 13May05
	}

#ifdef _UNICODE
	// the ANSI function calls are done above, we only need do the Unicode ones here
	TwoStringsToPunctPairs(m_punctPairs,strPunctPairsSrcSet,strPunctPairsTgtSet);
	TwoStringsToTwoPunctPairs(m_twopunctPairs,strTwoPunctPairsSrcSet,strTwoPunctPairsTgtSet);
#endif

	// compute the source and target punctuation sets to be used for parsing
	GetPunctuationSets(m_punctuation[0], m_punctuation[1]);

	// toggle the checkmark on if the flag value from the config file is TRUE
	// BEW added 31Oct05; else remove the checkmark if the value was FALSE
	// remove the checkmark
	gbSuppressAutoCapsAsk = TRUE;
	// toggle the flag value because the OnToolsAutoCapitalization call will retoggle it back again
	if (gbAutoCaps)
	{
		gbAutoCaps = FALSE;
	}
	else
	{
		gbAutoCaps = TRUE;
	}
	wxCommandEvent dummyEvent;
	OnToolsAutoCapitalization(dummyEvent); // this toggles the flag & sets the tick accordingly
	gbSuppressAutoCapsAsk = FALSE; // enable the ask message to be shown to the user

	// determine the flag values for when there may be empty correspondences list(s)
	gbNoSourceCaseEquivalents = FALSE; // default
	if (m_srcLowerCaseChars.IsEmpty() && m_srcUpperCaseChars.IsEmpty())
		gbNoSourceCaseEquivalents = TRUE; // restore this flag if user cleared source lists

	gbNoTargetCaseEquivalents = FALSE; // default
	if (m_tgtLowerCaseChars.IsEmpty() && m_tgtUpperCaseChars.IsEmpty())
		gbNoTargetCaseEquivalents = TRUE; // restore this flag if user cleared target lists

	gbNoGlossCaseEquivalents = FALSE; // default
	if (m_glossLowerCaseChars.IsEmpty() && m_glossUpperCaseChars.IsEmpty())
		gbNoGlossCaseEquivalents = TRUE; // restore this flag if user cleared source lists

	// make the command on the File menu echo the same setting
	// whm Note: in 12Sep08 refactoring bruce commented out the MFC version's code here
	// but I'm leaving it to force the value to TRUE since the wx app only uses a TRUE value
	m_bSaveAsXML = TRUE; // whm changed for wx version //m_bSaveAsXML == TRUE ? FALSE : TRUE; // toggle it
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxCommandEvent that is generated when the associated menu item is
///                         selected
/// \remarks
/// Called from: The Event Table of the CAdapt_ItDoc class where the Doc's OnFileNew()
/// handler gets triggered by a File | New command, and the Doc's OnFileNew() explicitly
/// calls this function. This OnFileNew() handler is also called explicitly from the App's 
/// OnInit() function with a wxID_NEW command event passed to it in order to create an
/// initial default document when the app is first run. 
/// This handler simply calls the OnFileNew() method of the m_pDocManager object.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnFileNew(wxCommandEvent& event)
{
	//This is needed in the wx version
	if (m_pDocManager != NULL)
		m_pDocManager->OnFileNew(event);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      srcPunctuation   <- a wxString that receives the source language punctuation
/// \param      tgtPunctuation   <- a wxString that receives the target language punctuation
/// \remarks
/// Called from: the App's GetBasicSettingsConfiguration(), GetProjectSettingsConfiguration(),
/// and from CPunctCorrespPageCommon::GetPunctuationSets().
/// Parses the source and target language punctuations characters from the PUNCTPAIR or
/// TWOPUNCTPAIR structs and stores them in separate strings. Also checks to insure that
/// '<' and '>' angle brackets are used as punctuation characters rather than word building
/// characters.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::GetPunctuationSets(wxString& srcPunctuation, wxString& tgtPunctuation)
{
	// next compute srcPunctuation and tgtPunctuation referenced variable contents (note,
	// CPunctCorrespPageWiz and CPunctCorrespPagePrefs classes also uses this function)
	srcPunctuation.Empty();
	tgtPunctuation.Empty();
	for (int index = 0; index < MAXPUNCTPAIRS; index++)
	{
		if (m_punctPairs[index].charSrc != _T('\0'))
		{
			srcPunctuation += _T(" ");
			srcPunctuation += m_punctPairs[index].charSrc;
		}
		if (m_punctPairs[index].charTgt != _T('\0'))
		{
			tgtPunctuation += _T(" ");
			tgtPunctuation += m_punctPairs[index].charTgt;
		}
	}

	// now scan the two punct pairs arrays for anything the user included there and forgot
	// to add to the single equivalences, or chose not to add to the single equivalences
	wxChar srcChar = _T('\0');
	wxChar tgtChar = _T('\0');
	for (int index = 0; index < MAXTWOPUNCTPAIRS; index++)
	{
		if (m_twopunctPairs[index].twocharSrc[0] != _T('\0'))
		{
			srcChar = m_twopunctPairs[index].twocharSrc[0];
			int nFound = srcPunctuation.Find(srcChar);
			if (nFound == -1)
			{
				// we've found one not included in the single set, so add it to the inventory
				srcPunctuation += _T(" ");
				srcPunctuation += srcChar;
			}
		}
		if (m_twopunctPairs[index].twocharSrc[1] != _T('\0'))
		{
			srcChar = m_twopunctPairs[index].twocharSrc[1];
			int nFound = srcPunctuation.Find(srcChar);
			if (nFound == -1)
			{
				// we've found one not included in the single set, so add it to the inventory
				srcPunctuation += _T(" ");
				srcPunctuation += srcChar;
			}
		}

		// now do the same for the target set
		if (m_twopunctPairs[index].twocharTgt[0] != _T('\0'))
		{
			tgtChar = m_twopunctPairs[index].twocharTgt[0];
			int nFound = tgtPunctuation.Find(tgtChar);
			if (nFound == -1)
			{
				// we've found one not included in the single set, so add it to the inventory
				tgtPunctuation += _T(" ");
				tgtPunctuation += tgtChar;
			}
		}
		if (m_twopunctPairs[index].twocharTgt[1] != _T('\0'))
		{
			tgtChar = m_twopunctPairs[index].twocharTgt[1];
			int nFound = tgtPunctuation.Find(tgtChar);
			if (nFound == -1)
			{
				// we've found one not included in the single set, so add it to the inventory
				tgtPunctuation += _T(" ");
				tgtPunctuation += tgtChar;
			}
		}
	}

	// check that < and > have not been omitted, if they have, warn user that they are being
	// automatically added (they are required so that the algorithm for parsing nested quotes
	// with space between them does not get defeated - the nested quotes belong on the same
	// CSourcePhrase instance, not on consecutive ones)
	int offset;
	if (srcPunctuation.IsEmpty())
		goto n; // don't bother with the checks if there is no content anyway
	offset = srcPunctuation.Find(_T("<"));
	if (offset == -1)
	{
		// no left wedge, so it would be treated as a word-building character, which is illegal
		// for standard format, so warn user and insert it; all we do now is make sure it gets 
		// put in m_punctuation[0] and [1]) 
		// BEW changed this, 27Apr05, to also construct a PUNCTPAIR for the < wedge, provided
		// there is space for it
		// IDS_LEFT_WEDGE_ERR
		wxMessageBox(_("Sorry, Adapt It will not permit the < character to be used as a word-building character. It must be included with the punctuation characters, and it will be placed there now."), _T(""), wxICON_INFORMATION);
		srcPunctuation += _T(" <"); // add it after a delimiting space
		AddWedgePunctPair(_T('<'));
	}
	offset = srcPunctuation.Find(_T(">"));
	if (offset == -1)
	{
		// no right wedge, so it would be treated as a word-building character, which is illegal
		// for standard format, so warn user and insert it (if the user wants to set up the
		// source to target correspondence, he'll have to explicitly return to the dialog manually;
		// all we do now is make sure it gets put in m_punctuation[0] and [1])
		// IDS_RIGHT_WEDGE_ERR
		wxMessageBox(_("Sorry, Adapt It will not permit the > character to be used as a word-building character. It must be included with the punctuation characters, and it will be placed there now."), _T(""), wxICON_INFORMATION);
		srcPunctuation += _T(" >"); // add it after a delimiting space
		AddWedgePunctPair(_T('>'));
	}

	// do the target ones now
	if (tgtPunctuation.IsEmpty())
		goto n; // don't bother with the checks if there is no content anyway
	offset = tgtPunctuation.Find(_T("<"));
	if (offset == -1)
	{
		// we'll do the fix silently for the target language
		tgtPunctuation += _T(" <"); // add it after a delimiting space
	}
	offset = tgtPunctuation.Find(_T(">"));
	if (offset == -1)
	{
		// we'll do the fix silently for the target language
		tgtPunctuation += _T(" >"); // add it after a delimiting space
	}
	
	// finally, add a delimiting space to the end, provided there is some content already
	if (!srcPunctuation.IsEmpty())
		srcPunctuation += _T(' ');
	if (!tgtPunctuation.IsEmpty())
		tgtPunctuation += _T(' ');
n:	;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      wedge   -> a wxChar representing either '<' or '>'
/// \remarks
/// Called from: the App's GetPunctuationSets().
/// This function is called only if the user tries to use the left wedge '<' and/or right
/// wedgt '>' as word building characters - which is illegal. After notifying the user of the
/// error, the function adds the wedges to the the PUNCTPAIR struct (or TWOPUNCTPAIR struct if
/// all PUNCTPAIR possibilities are already taken).
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::AddWedgePunctPair(wxChar wedge)
{
	// BEW added 27Apr05 to support adding of a left or right wedge PUNCTPAIR (or TWOPUNCTPAIR
	// if all PUNCTPAIR possibilities are already taken, or no effect if both sets of possibilities
	// are already taken) when the GetPunctuationSets() function detects the omission of < or >
	// in the source punctuation set - we assume that such an omission from the source punctuation
	// set (ie. the CString) indicates the lack of < to < or > to >, as the case may be,
	// equivalences and so AddWedgePunctPair builds what we need.
	int index;

	// a PunctPair instance for this wedge might previously have been constructed (such
	// as when dealing with Project configuration where it is lacking after having dealt with
	// its lack in the Basic configuration), so we check for this and do nothing if the
	// appropriate one is already in the m_punctPairs array.
	for (index = 0; index < MAXPUNCTPAIRS; index++)
	{
		if (gpApp->m_punctPairs[index].charSrc == wedge)
			return;
	}
	for (index = 0; index < MAXTWOPUNCTPAIRS; index++)
	{
		if (gpApp->m_twopunctPairs[index].twocharSrc[0] == wedge)
			return;
	}

	// it's not there yet, so create the PunctPair and add it to one of the arrays
	// now (try for the single-to-single one, but if not, the doubles one will do
	bool bAddedIt = FALSE;
	for (index = 0; index < MAXPUNCTPAIRS; index++)
	{
		if (gpApp->m_punctPairs[index].charSrc != _T('\0'))
			continue;
		else
		{
			// found an empty one, so see if its matching target character is also null
			// and if it is, construct the wedge pair here
			if (gpApp->m_punctPairs[index].charTgt != _T('\0'))
			{
				continue; // this one is already filled, so iterate
			}
			else
			{
				// eureka! we can build it now
				gpApp->m_punctPairs[index].charSrc = wedge;
				gpApp->m_punctPairs[index].charTgt = wedge;
				return;
			}
		}
	}
	if (!bAddedIt)
	{
		// If we did not add it in the singles correspondences because all 24 pairs had
		// already got correspondences in them, then we can add it to an empty 
		// TWOPUNCTPAIR in the first character of each double - since the algorithm
		// for using equivalences is general enough to cope with this. We exploit the
		// fact that if a double cell is empty, then the first character of the pair
		// will be a null - this is a sufficient condition for an empty cell in the dialog
		for (index = 0; index < MAXTWOPUNCTPAIRS; index++)
		{
			if (gpApp->m_twopunctPairs[index].twocharSrc[0] != _T('\0'))
				continue;
			else
			{
				// found an empty one, so see if its matching target characters are also nulls
				// and if so, construct the wedge pair here
				if (gpApp->m_twopunctPairs[index].twocharTgt[0] != _T('\0'))
				{
					continue; // this one is already filled, so iterate
				}
				else
				{
					// eureka! we can build it now
					gpApp->m_twopunctPairs[index].twocharSrc[0] = wedge;
					gpApp->m_twopunctPairs[index].twocharSrc[1] = _T('\0');
					gpApp->m_twopunctPairs[index].twocharTgt[0] = wedge;
					gpApp->m_twopunctPairs[index].twocharTgt[1] = _T('\0');
					return;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     FALSE if destinationFolder is empty, if no configuration file yet exists,
///             or could not set directory path, otherwise TRUE indicating write was successful
/// \param      configFilename     -> the name of the configuration file (without .aic extension)
/// \param      destinationFolder  -> the path location where the configuration file is to be written
/// \param      selector           -> 1 if writing the Basic config file; 2 if writing the Project
///                                     config file
/// \remarks
/// Called from: the App's Terminate() and OnExit(), the Doc's OnNewDocument(), OnSaveModified(),
/// OnOpenDocument(), and OnFilePackDoc().
/// Composes the whole configuration file in memory (as a wxTextFile) by calling the appropriate
/// helper functions (WriteFontConfiguration and either WriteBasicSettingsConfiguration or 
/// WriteProjectSettingsConfiguration), then writes the in-memory text file to its path/name 
/// destination.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::WriteConfigurationFile(wxString configFilename, 
										  wxString destinationFolder,int selector)
{
	// WriteConfigurationFile(,,1) to Basic config file: called only in App's Terminate()
	// WriteConfigurationFile(,,2) to Project config file: called in App's OnExit(), OnSaveModified() and OnFilePackDoc()
	// BEW on 4Jan07 added change to WriteConfiguration to save the external current work directory
	// and reestablish it at the end of the WriteConfiguration call, because the latter function
	// resets the current directory to the project folder before saving the project config file - and
	// this clobbered the restoration of a KB from the 2nd doc file accessed
	wxString strSaveCurrentDirectoryFullPath = GetDocument()->GetCurrentDirectory();

	wxString fName = configFilename + _T(".aic"); // extension for config files

	// exit if there is no path yet
	if (destinationFolder.IsEmpty())
	{
		// BEW removed message 31Oct05; it serves no useful purpose
		// because the user does not need to know this
		//IDS_NO_PROJECT_PATH
		return FALSE;
	}
	wxString path;
	path.Empty();
	path << destinationFolder << PathSeparator << fName;
	wxString data = _T("");
	wxString end = _T(""); // we're using wxStrings which already have EOL
	wxString tab = _T("\t");
	wxString number;
	//int color = 0; // default black - unused

	// open the config file for writing
	bool bIsOK = TRUE;
	wxTextFile f; 
	// make the working directory the "Adapt It Work" one
	// bool bOK = ::SetCurrentDirectory(destinationFolder); // ignore failures
	bool bOK;
	bOK = ::wxSetWorkingDirectory(destinationFolder);

	// open the config file for writing
	// wxWidgets version we use appropriate version of Open() for ANSI or Unicode build
	// Note: Need to check if file exists, otherwise if Open fails wxWidgets' wxTextFile
	// conjures up its own error message to that fact which it issues in Idle time, which
	// we don't want.
	bool bSuccessful = TRUE;
	if (!::wxFileExists(path))
	{
		// config file doesn't yet exist so create it
		bSuccessful = f.Create(path);
#ifndef _UNICODE
		// ANSI
		bSuccessful = f.Write(wxTextFileType_None); // read ANSI file into memory
#else
		// UNICODE
		bSuccessful = f.Write(wxTextFileType_None, wxConvUTF8); // read UNICODE file into memory
#endif
		bSuccessful = f.Close();
	}

	// open the config file in memory
	// wxWidgets version we use appropriate version of Open() for ANSI or Unicode build
#ifndef _UNICODE
	// ANSI
	bSuccessful = f.Open(path); // read ANSI file into memory
#else
	// UNICODE
	bSuccessful = f.Open(path, wxConvUTF8); // read UNICODE file into memory
#endif

	if (!bSuccessful)
	{
		// assume there was no configuration file in existence yet, so nothing needs to be fixed
		return FALSE;
	}

	// Since we are going to write a new config file we clear the current contents from memory
	while (f.GetLineCount())
		f.RemoveLine(0);

	// The fontInfo structs SrcFInfo, TgtFInfo, and NavFInfo should have current
	// information in them, so write out the information to the config file
	// first, do the source font
	data.Empty();
	data << szSourceFont; //data = szSourceFont;
	f.AddLine(data);
	// wx version: WriteFontConfiguration includes the color setting
	WriteFontConfiguration(SrcFInfo,&f);
	f.AddLine(end); // blank line as separator

	// next do the target font
	f.AddLine(szTargetFont);
	// wx version: WriteFontConfiguration includes the color setting
	WriteFontConfiguration(TgtFInfo,&f);
	f.AddLine(end); // blank line as separator

	// next do the navigation text's font
	f.AddLine(szNavTextFont);
	// wx version: WriteFontConfiguration includes the color setting
	WriteFontConfiguration(NavFInfo,&f);
	f.AddLine(end); // blank line as separator

	if (selector == 1)
	{
		f.AddLine(szBasicSettings);
		WriteBasicSettingsConfiguration(&f);
	}
	else
	{
		f.AddLine(szProjectSettings);
		WriteProjectSettingsConfiguration(&f);
	}
		
	if (!f.Write())
		bIsOK = FALSE;

	f.Close();

	// BEW added 04Jan07 to restore the former current working directory
	// to what it was for document accesses
	bOK = ::wxSetWorkingDirectory(strSaveCurrentDirectoryFullPath);

	return bIsOK;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     FALSE if if no configuration file yet exists, or could not could not read it, 
///             otherwise TRUE indicating read was successful
/// \param      configFilename     -> the name of the configuration file (without .aic extension)
/// \param      sourceFolder       -> the path location where the configuration file is to be read
/// \param      selector           -> 1 if reading the Basic config file; 2 if reading the Project
///                                     config file
/// \remarks
/// Called from: the App's GetBasicConfigFileSettings() and the Doc's GetProjectConfiguration().
/// Reads the whole config file into memory and from there it decomposes it line-by-line 
/// (as a wxTextFile) by calling the appropriate helper functions: GetFontConfiguration() and either 
/// GetBasicSettingsConfiguration() or GetProjectSettingsConfiguration().
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::GetConfigurationFile(wxString configFilename, wxString sourceFolder, int selector)
{
	wxString fname;
	fname.Empty();
	fname << configFilename << _T(".aic");
	wxString path;
	path.Empty();
	path << sourceFolder << PathSeparator << fname;
	wxString data = _T("");
	wxString name;
	bool	bIsOK = TRUE;
	bool	bSuccessful = FALSE;
	wxString  strValue;
	wxString dummy;

	// make the working directory the "Adapt It Work" one
	bool bOK = ::wxSetWorkingDirectory(sourceFolder);

	wxTextFile f;
	// Under wxWidgets, wxTextFile actually reads the entire file into memory at the Open()
	// call. It is set up so we can treat it as a line oriented text file while in memory,
	// modifying it, then if not just reading it, we can write it back out to persistent 
	// storage with a single call to Write().

	// open the config file for reading
	// wxWidgets version we use appropriate version of Open() for ANSI or Unicode build
	// Note: Need to check if file exists, otherwise if Open fails wxWidgets' wxTextFile
	// conjures up its own error message to that fact which it issues in Idle time just
	// after it's no longer busy.
	if (!::wxFileExists(path))
	{
		bSuccessful = FALSE;
	}
	else
	{
#ifndef _UNICODE
		// ANSI
		bSuccessful = f.Open(path); // read ANSI file into memory
#else
		// UNICODE
		bSuccessful = f.Open(path, wxConvUTF8); // read UNICODE file into memory
#endif
	}
	if (!bSuccessful)
	{
		wxString configType,msg;
		if (selector == 1)
			configType = _("basic");
		else if (selector == 2)
			configType = _("project");
		msg = msg.Format(_("Unable to open the %s configuration file for reading. Default values will be used instead. (Ignore this message if you have just launched Adapt It for the first time, or have just created a new project, because no configuration file exists yet.)"),configType.c_str());
		// assume there was no configuration file in existence yet, so nothing needs to be fixed
		wxMessageBox(msg,_T(""),wxICON_INFORMATION);
		bIsOK = FALSE;

		// This block added by JF Tue 5-Apr-2005.
		// The problem is that, when making a new project in book mode, no default book is specified, meaning that
		// m_pCurrBookNamePair remains NULL, causing this software to crash not far down the track, because elsewhere
		// in this program it is assumed that if book mode is on, m_pCurrBookNamePair is non-NULL.
		// We solve the problem by explicitly defaulting to the first book (book 0) when a new project is created.
		// Of course, all this is subject to us really defaulting to book mode being ON, and since I don't know the
		// conditions under which we default one way or the other for the setting in question, I explicitly test that
		// book mode is on before doing my magic.
		if (m_bBookMode && !m_bDisableBookMode)
		{
            m_nBookIndex = 39;
			m_pCurrBookNamePair = ((BookNamePair*)(*m_pBibleBooks)[m_nBookIndex]);
			m_bibleBooksFolderPath.Empty();
			m_bibleBooksFolderPath << m_curAdaptionsPath << PathSeparator <<
																m_pCurrBookNamePair->dirName;
		}
	}

	if (!bIsOK)
	{
		// whm note: When GetConfiguration is called to get the basic configuration and it fails,
		// and bIsOK is FALSE, in which case the caller calls SetDefaults which sets default color 
		// values for the fonts. When GetConfigurationFile is called to get the project 
		// configuration and it fails, the previously set values for basic configuration will apply, 
		// or if it also failed, those set in OnInit() and Initialize() will prevail. Therefore we, 
		// don't need to set the font color defaults here.
		return FALSE;
	}

	// We need to initialize the wxTextFile file pointer to the beginning of the file
	// otherwise the GetNextLine() will fail
	dummy = f.GetFirstLine();

	// The entire basic config file is now in memory and we can read the information
	// by scanning its virtual lines.

	// wx version: fonts are created only once in the App's OnInit(). The default font color
	// is also assigned there using wxFontData::SetColour(). If GetFontConfiguration fails
	// here the default values will be used
	wxString configFileType,errMsg,msg;
	if (selector == 1)
		configFileType = _("basic");
	else
		configFileType = _("project");
	msg = _("Unable to read %s font data from the %s configuration file. Default values will be used instead.");
	
	// get the source font's SrcFInfo data from the configFileType config file
	bOK = GetFontConfiguration(SrcFInfo,&f);
	if (!bOK)
	{
		wxString fontType = _("source");
		errMsg = errMsg.Format(msg,
			fontType.c_str(), configFileType.c_str());
		wxMessageBox(errMsg,_T(""),wxICON_WARNING);
	}

	// get the target font's TgtFInfo data from the configFileType config file
	bOK = GetFontConfiguration(TgtFInfo,&f);
	if (!bOK)
	{
		wxString fontType = _("target");
		errMsg = errMsg.Format(msg,
			fontType.c_str(), configFileType.c_str());
		wxMessageBox(errMsg,_T(""),wxICON_WARNING);
	}

	// get the nav font's NavFInfo data from the configFileType config file
	bOK = GetFontConfiguration(NavFInfo, &f);
	if (!bOK)
	{
		wxString fontType = _("navigation");
		errMsg = errMsg.Format(msg,
			fontType.c_str(), configFileType.c_str());
		wxMessageBox(errMsg,_T(""),wxICON_WARNING);
	}
	
	// get the section heading from our in-memory config file
	data = f.GetNextLine(); // data should be "Settings" for basic config file or "ProjectSettings" for proj config file

	GetValue(data, strValue, name);

	// now get the rest of the settings parameters from the config file
	// wx note: The wxTextFile remains open during the following calls which
	// simply continue reading the configuration data from memory
	if (selector == 1) // app level
		GetBasicSettingsConfiguration(&f);
	else // project level
		GetProjectSettingsConfiguration(&f);

	f.Close(); // closes the wxTextFile and frees memory used for it

	return bIsOK;
}

#ifdef _UNICODE

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pp[MAXPUNCTPAIRS]   -> the PUNCTPAIR array
/// \param      rSrcStr             <- a wxString that receives the source language punctuation characters
/// \param      rTgtStr             <- a wxString that receives the target language punctuation characters
/// \remarks
/// Called from: the App's WriteBasicSettingsConfiguration() and WriteProjectSettingsConfiguration().
/// Populates the strings with the current contents of the array of pairs; source puncts
/// in the rSrcStr, and target ones in rTgtStr. Separating them in such a way allows us to 
/// convert each with the encoding appropriate for each when we output the config files
/// to disk.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::PunctPairsToTwoStrings(PUNCTPAIR pp[MAXPUNCTPAIRS], 
											wxString& rSrcStr, wxString& rTgtStr)
{
	// clear strings
	rSrcStr.Empty();
	rTgtStr.Empty();

	// populate the strings with the current contents of the array of pairs; source puncts
	// in the rSrcStr, and target ones in rTgtStr - separating them like this allows us to 
	// convert each with the encoding appropriate for each when we output the config files
	// to disk
	// BEW changed 15May08 to remove the test for chSrc and chTgt both being null bytes, because
	// that happens when the user clears out a punctuation character from both src and tgt lists
	// in the GUI, and the result was that the wrong test here was causing premature list
	// termination. Our new algorithm should (1) use space as a placeholder for any null, and
	// process the whole set of cells, including empty pairs; (2) on loop termination, remove
	// any matched space pairs at the list ends, as there is no point in keeping them; (3) support
	// "closing of the gaps" when the user removes pairs of punctuation charcters in the GUI, so
	// that the gaps he thereby creates will, after an app exit and relaunch, be closed up (if he
	// wanted to put the removed punctuation characters back in the list, he'd then just add them
	// at the empty cells at the end; (4) implement the same general protocols as 1 -3 for the
	// two character pairs too, in the TwoPunctPairsToTwoStrings() function. 
	// The best way to support gap closure is to do it BEFORE the final strings are written out
	// in the configuration file, as that makes reading the punctuation from the config file on
	// app launch and project entry easy, as there would be no gaps to worry about. That means 
	// we must implement the "close the gaps" code in this present function, and in the 
	// TwoPunctPairsToTwoStrings() function.
	for (int i = 0; i < MAXPUNCTPAIRS; i++)
	{
		wxChar chSrc = pp[i].charSrc;
		wxChar chTgt = pp[i].charTgt;
		if (chSrc == _T('\0') && chTgt == _T('\0'))
		{
			continue; // eliminate this "gap", whether list-medial, or at list end.
			//return; BEW removed 15May08 // return if both source and target are null, this indicates end of list
		}
		if (chSrc == _T('\0'))
		{
			rSrcStr += _T(" ");
		}
		else
		{
			rSrcStr += chSrc;
		}
		if (chTgt == _T('\0'))
		{
			rTgtStr += _T(" ");
		}
		else
		{
			rTgtStr += chTgt;
		}
	}
}
#endif

#ifdef _UNICODE

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pp[MAXTWOPUNCTPAIRS]   -> the TWOPUNCTPAIR array
/// \param      rSrcStr                <- a wxString that receives the source language punctuation characters
/// \param      rTgtStr                <- a wxString that receives the target language punctuation characters
/// \remarks
/// Called from: the App's WriteBasicSettingsConfiguration() and WriteProjectSettingsConfiguration().
/// Populates the strings with the current contents of the array of twopunct pairs; source puncts
/// in the rSrcStr, and target ones in rTgtStr. Separating them in such a way allows us to 
/// convert each with the encoding appropriate for each when we output the config files
/// to disk.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::TwoPunctPairsToTwoStrings(TWOPUNCTPAIR pp[MAXTWOPUNCTPAIRS], wxString& rSrcStr,
												wxString& rTgtStr)
{
	// clear strings
	rSrcStr.Empty();
	rTgtStr.Empty();

	// populate the strings with the current contents of the array of twopunct pairs, 
	// one string for source text punctuation, the other for target text punctuation; 
	// so we can support separate encodings
	// BEW changed 15May08 to remove the test for twocharSrc[0] & [1] both being null bytes, as
	// the old code did not adequately support the user when he cleared out a cell or cell pair
	// medially within the GUI lists; the result was that the test here would cause premature list
	// termination. Our new algorithm should (1) use spaces as a placeholders for null character,
	// and process the whole set of cells, including empty double-pairs; (2) on loop termination,
	//  remove any matched space double-pairs at the list ends, as there is no point in keeping 
	// them; (3) support "closing of the gaps" when the user removes pairs of punctuation charcters in the GUI, so
	// that the gaps he thereby creates will, after an app exit and relaunch, be closed up (if he
	// wanted to put the removed punctuation characters back in the list, he'd then just add them
	// at the empty cells at the end; (4) implement the same general protocols as 1 -3 for the
	// one character pairs too, in the PunctPairsToTwoStrings() function. 
	// The best way to support gap closure is to do it BEFORE the final strings are written out
	// in the configuration file, as that makes reading the punctuation from the config file on
	// app launch and project entry easy, as there would be no gaps to worry about. That means 
	// we must implement the "close the gaps" code in this present function, and in the 
	// PunctPairsToTwoStrings() function. Only "gaps" where all four characters of paired double
	// characters are absent need be eliminated, these will be four null characters which we need
	// to test for; all other combinations must be retrained, using space as placeholder for each
	// null character.
	for (int i = 0; i < MAXTWOPUNCTPAIRS; i++)
	{
		if (pp[i].twocharSrc[0] == _T('\0') && pp[i].twocharSrc[1] == _T('\0' &&)
			pp[i].twocharTgt[0] == _T('\0') && pp[i].twocharTgt[1] == _T('\0'))
		{
			continue; // remove this gap, whether list-medial or at the end of the list
			//return; // BEW removed 15May05 // do no more once both source ones are empty
		}
		wxChar chSrc0 = pp[i].twocharSrc[0];
		wxChar chSrc1 = pp[i].twocharSrc[1];
		wxChar chTgt0 = pp[i].twocharTgt[0];
		wxChar chTgt1 = pp[i].twocharTgt[1];
		if (chSrc0 == _T('\0'))
		{
			// if first is null, put a placeholder space in the string, then check second
			rSrcStr += _T(" ");
			if (chSrc1 == _T('\0'))
				rSrcStr += _T(" ");
			else
				rSrcStr += chSrc1;
		}
		else
		{
			rSrcStr += chSrc0;
			if (chSrc1 == _T('\0'))
				rSrcStr += _T(" ");
			else
				rSrcStr += chSrc1;
		}
		if (chTgt0 == _T('\0'))
		{
			// if first is null, put a placeholder space there in the string , then check second
			rTgtStr += _T(" ");
			if (chTgt1 == _T('\0'))
				rTgtStr += _T(" ");
			else
				rTgtStr += chTgt1;
		}
		else
		{
			rTgtStr += chTgt0; // first is non empty, append it; then check second
			if (chTgt1 == _T('\0'))
				rTgtStr += _T(" ");
			else
				rTgtStr += chTgt1;
		}
	}
}
#endif

#ifdef _UNICODE

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pp[MAXPUNCTPAIRS]   <- the PUNCTPAIR array that gets populated with punct pairs
/// \param      rSrcStr             -> a wxString that contains the source language punctuation characters
/// \param      rTgtStr             -> a wxString that contains the target language punctuation characters
/// \remarks
/// Called from: the App's GetBasicSettingsConfiguration() and GetProjectSettingsConfiguration().
/// Populates the PUNCTPAIR array with the current contents of the punctuation characters stored in
/// the rSrcStr and rTgtStr strings. See PunctPairsToTwoStrings() for the function that does the
/// opposite action.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::TwoStringsToPunctPairs(PUNCTPAIR pp[MAXPUNCTPAIRS], wxString& rSrcStr, wxString& rTgtStr)
{
	// clear the array
	for (int j = 0; j < MAXPUNCTPAIRS; j++)
	{
		pp[j].charSrc = _T('\0');
		pp[j].charTgt = _T('\0');
	}
	// populate the contents of the array of pairs using the string contents
	int lenSrc = rSrcStr.Length();
	int lenTgt = rTgtStr.Length();
	if (rSrcStr.IsEmpty())
		return;
	if (rTgtStr.IsEmpty())
		return;
	// from version 2.2.1 I will make the config table less fragile - the two strings can be
	// different in length (we makes as many pairs as we can, and bale out when there are no
	// more to be made), don't have to be filled to 24 characters by trailing spaces, and we
	// no longer care if they are even or odd in length
	int nCount = -1; // count (index) of how many pairs have been formed
	for (int i = 0; i < MAXPUNCTPAIRS; i++)
	{
		nCount++;
		if (nCount >= lenSrc)
			return; // bale out when source string is traversed
		wxChar ch = rSrcStr.GetChar(i);
		if (ch == _T(' '))
		{
			pp[i].charSrc = _T('\0');
		}
		else
		{
			pp[i].charSrc = ch;
		}
		if (nCount >= lenTgt)
		{
			// end of the target string has been reached, so bale out
			return;
		}
		ch = rTgtStr.GetChar(i); // get its paired target punct char
		if (ch == _T(' '))
		{
			pp[i].charTgt = _T('\0'); // space is used for setting up a null correspondence
		}
		else
		{
			pp[i].charTgt = ch;
		}
	}
}
#endif

#ifdef _UNICODE

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pp[MAXTWOPUNCTPAIRS]   <- the TWOPUNCTPAIR array that gets populated with  two punct pairs
/// \param      rSrcStr                -> a wxString that contains the source language punctuation characters
/// \param      rTgtStr                -> a wxString that contains the target language punctuation characters
/// \remarks
/// Called from: the App's GetBasicSettingsConfiguration() and GetProjectSettingsConfiguration().
/// Populates the TWOPUNCTPAIR array with the current contents of the punctuation characters stored in
/// the rSrcStr and rTgtStr strings. See TwoPunctPairsToTwoStrings() for the function that does the
/// opposite action.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::TwoStringsToTwoPunctPairs(TWOPUNCTPAIR pp[MAXTWOPUNCTPAIRS], wxString& rSrcStr, wxString& rTgtStr)
{
	// if a single character, it can be only in the first position of a pair because in the
	// GUI there is no way to type a single character and make it occupy the second place
	// in the underlying 2-character array; and to get the pairs counting right, when single
	// character pairs are in these lists, the second null byte has to be preserved as a following
	// space. The only removals will have been all four characters null, for a given pair of
	// two-character punctuations; and those removals were done to "close the gaps" left by
	// the user removing the contents of a cell pair.
	// clear the array
	for (int j = 0; j < MAXTWOPUNCTPAIRS; j++)
	{
		pp[j].twocharSrc[0] = _T('\0');
		pp[j].twocharSrc[1] = _T('\0');
		pp[j].twocharTgt[0] = _T('\0');
		pp[j].twocharTgt[1] = _T('\0');
	}

	// populate the contents of the array of two-character pairs using the 
	// string contents
	int lenSrc = rSrcStr.Length();
	int lenTgt = rTgtStr.Length();
	int nCount = -1; // from version 2.2.1, we don't require spaces at the end, just build
					 // as many pairs as possible
	
	// BEW changed 4 May, since allowing singles to be placed in the doubles rows, we
	// can no longer assume pairs; but we can assert the strings must be equal length
	//wxASSERT(lenSrc % 2 == 0); // must be even
	//wxASSERT(lenTgt % 2 == 0); // must be even
	// BEW changed 15May08 to reinstate the earlier protocol, because we eliminated gaps
	// (which eliminated 4 characters at a time, two per string in matching locations,) and
	// so the modulo 2 test must be TRUE for each string
	wxASSERT(lenSrc % 2 == 0); // must be even
	wxASSERT(lenTgt % 2 == 0); // must be even
	wxASSERT(lenSrc == lenTgt); // must be same length
	
	for (int i = 0; i < MAXTWOPUNCTPAIRS; i++)
	{
		nCount++;
		int k[2] = {2*i,2*i+1};
		if (rSrcStr.IsEmpty())
		{
			return; // no correspondences at all
		}
		if (nCount >= lenSrc/2) // divide  by 2, since we deal with pairs of chars
		{
			return;
		}
		wxChar ch = rSrcStr.GetChar(k[0]);
		wxChar ch1 = rSrcStr.GetChar(k[1]);

		pp[i].twocharSrc[0] = ch; // left one of pair always has a character in it
		if (ch1 == _T(' '))
			{
				// if second was a space, user put a single character in the cell
				// so make this second character a null
				pp[i].twocharSrc[1] = _T('\0');
			}
			else
			{
				// second character of the pair exists, so put it in the array
				pp[i].twocharSrc[1] = ch1;
			}
		
		if (nCount >= lenTgt/2) // divide by 2, since dealing with pairs
		{
			return;
		}

		// now the target side...
		ch = rTgtStr.GetChar(k[0]); // get its paired target punct chars
		ch1 = rTgtStr.GetChar(k[1]); // assume there are at least two chars left, perhaps spaces

		pp[i].twocharTgt[0] = ch; // first of pair is never empty
		if (ch1 == _T(' '))
		{
			// if second was a space, second user put a single character in the cell, so
			// make this second character a null in the character array
			pp[i].twocharTgt[1] = _T('\0');
		}
		else
		{
			// second character of the pair exists, so put it in the array
			pp[i].twocharTgt[1] = ch1;
		}
	}
	/* old code - deprecated, 15May08 (we don't care to support very early config files)
	for (int i = 0; i < MAXTWOPUNCTPAIRS; i++)
	{
		nCount++;
		int k[2] = {2*i,2*i+1};
		if (rSrcStr.IsEmpty())
			return; // no correspondences at all
		if (nCount >= lenSrc/2) // divide  by 2, since we deal with pairs of chars
			return;
		wxChar ch = rSrcStr.GetChar(k[0]);
		wxChar ch1 = rSrcStr.GetChar(k[1]);
		if (ch == _T(' ') && ch1 == _T(' '))
		{
			// assume no more when both are spaces (this makes old config files get
			// processed safely)
			return;
		}
		else
		{
			if (ch == _T(' '))
			{
				// if first was a space, second will be non-zero
				pp[i].twocharSrc[0] = _T('\0');
				pp[i].twocharSrc[1] = ch1;
			}
			else
			{
				pp[i].twocharSrc[0] = ch; // left one of pair has a character in it
				if (ch1 == _T(' '))
					pp[i].twocharSrc[1] = _T('\0'); // right one of the pair is empty
				else
					pp[i].twocharSrc[1] = ch1; // right one of the pair has a character in it
			}
		}
		if (nCount >= lenTgt/2) // divide by 2, since dealing with pairs
		{
			// end of target string reached, so clear our the source pair
			// because we can't pair anything with it (a null correspondence would require
			// a pair of spaces be here, instead of the end of the target string)
			pp[i].twocharSrc[0] = _T('\0');
			pp[i].twocharSrc[1] = _T('\0');
			return;
		}
		ch = rTgtStr.GetChar(k[0]); // get its paired target punct chars
		ch1 = rTgtStr.GetChar(k[1]); // assume there are at least two chars left, perhaps spaces
		if (ch == _T(' '))
		{
			// if first was a space, second may not be
			pp[i].twocharTgt[0] = _T('\0');
			if (ch1 == _T(' '))
				pp[i].twocharTgt[1] = _T('\0');
			else
				pp[i].twocharTgt[1] = ch1;
		}
		else
		{
			pp[i].twocharTgt[0] = ch; // first of pair is not empty
			if (ch1 == _T(' '))
				pp[i].twocharTgt[1] = _T('\0');
			else
				pp[i].twocharTgt[1] = ch1;
		}
	}
	*/
}
#endif

//ANSI version routines below

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pp[MAXPUNCTPAIRS]   -> the PUNCTPAIR array
/// \param      rStr                <- a wxString that receives the punctuation characters
/// \remarks
/// Called from: the App's WriteBasicSettingsConfiguration() and WriteProjectSettingsConfiguration().
/// Populates the string with the current contents of the array of pairs.
/// See StringToPunctPairs() for a function that does the opposite action.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::PunctPairsToString(PUNCTPAIR pp[MAXPUNCTPAIRS], wxString& rStr)
{
	//void CAdapt_ItApp::PunctPairsToString(PUNCTPAIR pp[24], wxString& rStr)
	// BEW changed behaviour, 27Apr05, of PunctPairsToString so it would iterate, rather
	// than return, if both source and target cells in the punctuation map dialog had
	// been edited so that they contained nothing. If that was the case, we want other
	// cell entries to be utilized; the old algorithm would return and anything subsequent
	// to the empty pair of cells used to be ignored.
	// clear rStr
	rStr.Empty();

	// populate rStr with the current contents of the array of pairs
	for (int i = 0; i < MAXPUNCTPAIRS; i++)
	{
		wxChar chSrc = pp[i].charSrc;
		wxChar chTgt = pp[i].charTgt;
		if (chSrc == _T('\0') && chTgt == _T('\0'))
			continue; // iterate when there is no character in source & target of the pair
		if (chSrc == _T('\0'))
		{
			rStr += _T(" ");
		}
		else
		{
			rStr += chSrc;
		}
		if (chTgt == '\0')
		{
			rStr += _T(" "); // use space as a placeholder when user has specified no equivalence
		}
		else
		{
			rStr += chTgt;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pp[MAXTWOPUNCTPAIRS]   -> the TWOPUNCTPAIR array
/// \param      rStr                   <- a wxString that receives the punctuation characters
/// \remarks
/// Called from: the App's WriteBasicSettingsConfiguration() and WriteProjectSettingsConfiguration().
/// Populates the string with the current contents of the array of two-punct pairs.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::TwoPunctPairsToString(TWOPUNCTPAIR pp[MAXTWOPUNCTPAIRS], wxString& rStr)
{
	//void CAdapt_ItApp::TwoPunctPairsToString(TWOPUNCTPAIR pp[8], wxString& rStr)
	// BEW changed behaviour, 27Apr05, of TwoPunctPairsToString so it would iterate, rather
	// than return, if both a source cell in the punctuation map dialog had been edited
	// so it contained nothing. If that was the case, we want other cell entries to be utilized; the
	// old algorithm would return and anything subsequent to the empty source cell used to be ignored.
	// BEW modified 16May08, to work like the Unicode-supporting function does (ie. TwoPunctPairsToTwoStrings())
	// clear rStr
	rStr.Empty();

	// populate rStr with the current contents of the array of twopunct pairs
	for (int i = 0; i < MAXTWOPUNCTPAIRS; i++)
	{
		if (pp[i].twocharSrc[0] == _T('\0') && pp[i].twocharSrc[1] == _T('\0') &&
			pp[i].twocharTgt[0] == _T('\0') && pp[i].twocharTgt[1] == _T('\0'))
			continue; // skip when matched cell pairs are empty
		wxChar chSrc0 = pp[i].twocharSrc[0];
		wxChar chSrc1 = pp[i].twocharSrc[1];
		wxChar chTgt0 = pp[i].twocharTgt[0];
		wxChar chTgt1 = pp[i].twocharTgt[1];
		// in the following, the GUI character, if there is only one, will always be at the [0]
		// location; the code assumes this could be at [0] or [1], but that generality doesn't
		// have any effect as the actual things which can happen are all catered for anyway
		if (chSrc0 == _T('\0'))
		{
			// if first is null, put a placeholder space in the string, then check second
			rStr += _T(" ");
			if (chSrc1 == _T('\0'))
				rStr += _T(" ");
			else
				rStr += chSrc1;
		}
		else
		{
			rStr += chSrc0;
			if (chSrc1 == _T('\0'))
				rStr += _T(" ");
			else
				rStr += chSrc1;
		}
		if (chTgt0 == '\0')
		{
			// if first is null, put a placeholder space there in the string , then check second
			rStr += _T(" ");
			if (chTgt1 == _T('\0'))
				rStr += _T(" ");
			else
				rStr += chTgt1;
		}
		else
		{
			rStr += chTgt0;  // first is non empty, append it; then check second
			if (chTgt1 == _T('\0'))
				rStr += _T(" ");
			else
				rStr += chTgt1;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pp[MAXPUNCTPAIRS]   <- the PUNCTPAIR array that is populated from the characters in rStr
/// \param      rStr                -> a wxString that stores the punctuation characters
/// \remarks
/// Called from: the App's InitializePunctuation(), GetBasicSettingsConfiguration() and 
/// GetProjectSettingsConfiguration().
/// Populates the PUNCTPAIR array with the current contents of the rStr string.
/// See PunctPairsToString() for a function that does the opposite action.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::StringToPunctPairs(PUNCTPAIR pp[MAXPUNCTPAIRS], wxString& rStr)
{
	// clear the array
	for (int m = 0; m < MAXPUNCTPAIRS; m++)
	{
		pp[m].charSrc = _T('\0');
		pp[m].charTgt = _T('\0');
	}

	// populate the contents of the array of pairs using the string contents
	int len = rStr.Length();
	wxASSERT(len % 2 == 0); // len must be even
	if (rStr.IsEmpty())
		return;
	
	// from version 2.2.1 I won't pad the string with nulls, which will reduce the fragility of
	// the config file to user editing
	int nCount = -1; // count (index) of how man pairs have been formed
	for (int i = 0; i < MAXPUNCTPAIRS; i++)
	{
		nCount++;
		if (nCount >= len/2)
			return; // bale out when the last pair of characters has been traversed
		int k = 2*i;
		int j = 2*i + 1;
		wxChar ch = rStr.GetChar(k);
		if (ch == _T(' '))
		{
			pp[i].charSrc = _T('\0');
		}
		else
		{
			pp[i].charSrc = ch;
		}
		ch = rStr.GetChar(j); // get its paired target punct char (it could be a placeholding space)
		if (ch == _T(' '))
		{
			pp[i].charTgt = _T('\0');
		}
		else
		{
			pp[i].charTgt = ch;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pp[MAXTWOPUNCTPAIRS]   <- the TWOPUNCTPAIR array that is populated from the characters in rStr
/// \param      rStr                   -> a wxString that stores the punctuation characters
/// \remarks
/// Called from: the App's InitializePunctuation(), GetBasicSettingsConfiguration() and 
/// GetProjectSettingsConfiguration().
/// Populates the TWOPUNCTPAIR array with the current contents of the rStr string.
/// See TwoPunctPairsToString() for a function that does the opposite action.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::StringToTwoPunctPairs(TWOPUNCTPAIR pp[MAXTWOPUNCTPAIRS], wxString& rStr)
{
	// clear the array
	for (int m = 0; m < MAXTWOPUNCTPAIRS; m++)
	{
		pp[m].twocharSrc[0] = _T('\0');
		pp[m].twocharSrc[1] = _T('\0');
		pp[m].twocharTgt[0] = _T('\0');
		pp[m].twocharTgt[1] = _T('\0');
	}

	// populate the contents of the array of two-character pairs using the 
	// string contents
	int len = rStr.Length();
	int nCount = -1; // from version 2.2.1, we don't require spaces at the end, just build
					 // as many pairs as possible
	wxASSERT(len % 2 == 0); // len must be even - if not for allowing singles correspondence,
						  // we'd  be able to make it a modulo 4 test; but it will have to 
						  // be just mod 2
	
	for (int i = 0; i < MAXTWOPUNCTPAIRS; i++)
	{
		nCount++;
		int k[2] = {4*i,4*i+1};
		int j[2] = {4*i + 2, 4*i+3};
		if (rStr.IsEmpty())
			return; // no correspondences at all
		if (nCount >= len/4) // divide  by 4, since we deal with pairs of pairs of chars
			return; // we are at the end of the string, so bale out early (don't pad with spaces)
		wxChar ch = rStr.GetChar(k[0]);
		wxChar ch1 = rStr.GetChar(k[1]);
		if (ch == _T(' ') && ch1 == _T(' '))
		{
			// assume no more when both source characters are spaces (this makes old config files
			// get processed safely)
			return;
		}
		else
		{
			if (ch == _T(' '))
			{
				// if first was a space, second will be non-zero
				pp[i].twocharSrc[0] = _T('\0');
				pp[i].twocharSrc[1] = ch1;
			}
			else
			{
				pp[i].twocharSrc[0] = ch;
				if (ch1 == _T(' '))
					pp[i].twocharSrc[1] = _T('\0');
				else
					pp[i].twocharSrc[1] = ch1;
			}
		}
		ch = rStr.GetChar(j[0]); // get its paired target punct chars
		ch1 = rStr.GetChar(j[1]);
		if (ch == _T(' '))
		{
			// if first was a space, second may not be
			pp[i].twocharTgt[0] = _T('\0');
			if (ch1 == _T(' '))
				pp[i].twocharTgt[1] = _T('\0');
			else
				pp[i].twocharTgt[1] = ch1;
		}
		else
		{
			pp[i].twocharTgt[0] = ch;  // first of pair is not empty
			if (ch1 == _T(' '))
				pp[i].twocharTgt[1] = _T('\0');
			else
				pp[i].twocharTgt[1] = ch1;
		}
	}
}

// According to the wxWidgets developers, the "Print Setup..." menu selection is obsolete since
// Windows 95. Users are expecte to do any necessary print setup from the main print dialog.
//void CAdapt_ItApp::OnFilePrintSetup(wxCommandEvent& WXUNUSED(event))
//{
//	wxPrintDialogData printDialogData(*pPrintData);
//    wxPrinter printer(& printDialogData);
//	printer.Setup(GetMainFrame());
//}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxCommandEvent that is generated when the associated menu item is
///                         selected
/// \remarks
/// Called from: The Event Table of the CAdapt_ItApp class in response to a File | Page Setup
/// menu command. Currently, the Page Setup dialog provided with wxWidgets only takes margin
/// settings in millimetres. As a consequence of this the Units Of Measure dialog and 
/// Preference page only allow the selection of "Centimeters" with the "Inches" selection 
/// disabled.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnFilePageSetup(wxCommandEvent& WXUNUSED(event))
{
	// wxWidgets Note:
	// The page setup dialog as it is currently instituted in wxWidgets appears
	// to only take margin units in millimetres. There is no provision for changing the
	// Windows page setup dialog flags parameter. It might be possible to override the default
	// dialog in a Windows port, but not sure about Linux or Mac. Therefore, I'm going to leave 
	// it in millimeters here for now. TODO: I'll have to look at it again when instituting the
	// "Units of Measure..." dialog.

	// MFC Note: this code works fine for starting with Portrait orientation and changing to 
	// Landscape; but when opening the Page Setup dialog in Landscape orientation, the dialog
	// does not show the radio button for Landscape checked, instead Portrait is checked. 
	// Clicking OK will then restore Portrait, or Cancel will leave user in Landscape, so the
	// behaviour is reasonable, the only problem is that landscape is not reflected in the 
	// button setting. This can't be changed without my having to set up a subclass and access
	// the radio button from it - a lot of work for very little payoff, so I will see if I can 
	// get away with not doing it.

	// create the page setup class instance using defaults
	CAdapt_ItView* pView = GetView();
	if (pView == NULL)
	{
		wxMessageBox(_T("Unable to display page setup dialog because no View is currently valid."));
		return;
	}

	// The pPgSetupDlgData and pPrintData objects were created in OnInit() and exist
	// while the App itself exists, so we need not create temporary ones here as the
	// MFC version does.


	// Margins and Paper Size are kept in the App, so update the pPgSetupDlgData with them
	// The MFC version internally keeps these measurements in 1,000ths of an inch.
	// We'll maintain the MFC internal storage units, but we'll need to convert
	// them to millimetres for the dialog's text boxes.
	// SetMarginTopLeft, SetMarginBottomRight, and SetPaperSize expect values 
	// in millimetres (use thousandths of inches internally)

    // Set the page orientation according to the App's m_bIsPortraitOrientation. Note:
    // SetPageOrientation() sets the page orientation in our pPrintData global 
	SetPageOrientation(m_bIsPortraitOrientation);
	// the initializations of pPgSetupDlgData are done in OnInit()
	// Margin MM values determined using the less precise config_only_thousandthsInchToMillimetres conversion factor
	pPgSetupDlgData->SetMarginTopLeft(wxPoint(m_marginLeftMM,m_marginTopMM));
	pPgSetupDlgData->SetMarginBottomRight(wxPoint(m_marginRightMM,m_marginBottomMM));
	// Paper size MM values determined using the more precise thousandthsInchToMillimetres conversion factor
	pPgSetupDlgData->SetPaperSize(wxSize(m_pageWidthMM,m_pageLengthMM)); 
	// set the paper id, converting it from MFC's enum to wx's enum.
	// Note: SetPaperId() overrides the explicit paper dimensions passed in wxPageSetupDialogData::SetPaperSize()
	pPgSetupDlgData->SetPaperId(MapMFCtoWXPaperSizeCode(m_paperSizeCode)); 

	// put up page setup dialog for the canvas using default print data setup
    wxPageSetupDialog pageSetupDialog(GetMainFrame()->canvas, pPgSetupDlgData);
    if (pageSetupDialog.ShowModal() == wxID_CANCEL)
	{
		// user cancelled
		return;
	}
	else
	{
		// User pressed OK
		// 
		// Get any new values and store them in the global print data object.
		(*pPrintData) = pageSetupDialog.GetPageSetupData().GetPrintData();
		// Get updated values and store them in the global page setup dialog data object.
		(*pPgSetupDlgData) = pageSetupDialog.GetPageSetupData();

		// save new values for configuration file when next written out
		// MFC says, "we use SAE units internally, so that we can just use MM_LOENGLISH for 
		// both SAE and Metric
		wxPoint pt;
		pt = pPgSetupDlgData->GetMarginTopLeft();
		m_marginLeftMM = pt.x;
		m_marginLeft = (int)(pt.x*config_only_millimetresToThousandthsInch);
		m_marginTopMM = pt.y;
		m_marginTop = (int)(pt.y*config_only_millimetresToThousandthsInch);
		pt = pPgSetupDlgData->GetMarginBottomRight();
		m_marginRightMM = pt.x;
		m_marginRight = (int)(pt.x*config_only_millimetresToThousandthsInch);
		m_marginBottomMM = pt.y;
		m_marginBottom = (int)(pt.y*config_only_millimetresToThousandthsInch);

		wxPaperSize pSize,pSizepd;
		pSize = pPgSetupDlgData->GetPaperId();
		pSizepd = pPrintData->GetPaperId();
		m_paperSizeCode = MapWXtoMFCPaperSizeCode(pSizepd);

		wxSize size;
		size = pPgSetupDlgData->GetPaperSize();
		m_pageWidthMM = size.x;
		m_pageLengthMM = size.y;
		m_pageWidth = (int)(size.x*millimetresToThousandthsInch);
		m_pageLength = (int)(size.y*millimetresToThousandthsInch);

		m_bIsPortraitOrientation = (pPrintData->GetOrientation() == wxPORTRAIT);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxUpdateUIEvent that is generated when the File Menu is about
///                         to be displayed
/// \remarks
/// Called from: The wxUpdateUIEvent mechanism when the associated menu item is selected, and before
/// the menu is displayed.
/// If Vertical Editing is in progress the Page Setup menu item is always disabled and this 
/// handler returns immediately. Otherwise, if the application has a valid View pointer, the 
/// "Page Setup..." item on the File menu is enabled, otherwise it is disabled. 
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnUpdateFilePageSetup(wxUpdateUIEvent& event)
{
	if (gbVerticalEditInProgress)
	{
		event.Enable(FALSE);
		return;
	}
	// Shouldn't enable the File|Page Setup... interface unless there is a valid view
	CAdapt_ItView* pView = GetView();
	if (pView != NULL)
		event.Enable(TRUE);
	else
		event.Enable(FALSE);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      bIsPortrait   -> TRUE for portrait, FALSE for landscape
/// \remarks
/// Called from: the App's OnInit(), OnFilePageSetup(), and the View's OnInitialUpdate()
/// Calls the SetOrientation() method of the global wxPrintData object passing it either the
/// wxPORTRAIT or wxLANDSCAPE symbol as parameter, depending on the value of bIsPortrait passed
/// into the current function.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::SetPageOrientation(bool bIsPortrait)
{
	// pPrintData is the App's wxPrintData member
	if (pPrintData->IsOk())
	{
		if (bIsPortrait)
			pPrintData->SetOrientation(wxPORTRAIT);
		else
			pPrintData->SetOrientation(wxLANDSCAPE);
		// insure that the pPgSetupDlgData gets any pPrintData change
		//(*pPgSetupDlgData) = (*pPrintData); // NO!!, Doing this resets the default pPgSetupDlgData!
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     1 if portrait, 2 if landscape (for compatibility with MFC)
/// \remarks
/// Called from: AIPrintout::OnPreparePrinting(), DoExportSrcOrTgtRTF() of ExportFunctions.cpp,
/// and the CExportInterlinearDlg's constructor.
/// Calls the GetOrientation() method of the global wxPrintData object. If it returns wxPORTRAIT
/// this function returns 1; if it returns wxLANDSCAPE this function returns 2 (for compatibility
/// with MFC).
////////////////////////////////////////////////////////////////////////////////////////////
int CAdapt_ItApp::GetPageOrientation()
{
	// returns 1 if dmOrientation = 1 (portrait), returns 2 if dmOrientation = 2 (landscape)
	int nOrientation = 1; // default is portrait

	// pPrintData is the App's wxPrintData member
	if (pPrintData->IsOk())
	{
		if (pPrintData->GetOrientation() != wxPORTRAIT)
		{
			wxASSERT(pPrintData->GetOrientation() == wxLANDSCAPE);
			nOrientation = 2;
		}
	}
	return nOrientation;
}

#ifdef _UNICODE

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      bstr       -> a CBString containing the single-byte character string to be converted
/// \param      convStr    <- a wxString representing the UTF-16 converted string
/// \remarks
/// Called from: the App's DoInputConversion(), the Doc's DoUnpackDocument(), the View's
/// DoConsistentChanges(), FileContainsBookIndicator() in helpers.cpp, ParseXML() in XML.cpp
/// as well as AtBooksAttr() and AtSFMAttr(), and CXMLErrorDlg::InitDialog().
/// Converts a single-byte character string, which is ASCII or UTF-8 (as returned from parsing XML data)
/// to a Unicode (UTF-16) string storing it in the wxString convStr.
/// See also the overloaded version in which the function returns the converted wxString.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::Convert8to16(CBString& bstr,wxString& convStr)
{
	// Convert8to16 is used for converting a single-byte character string stored in CBString, which is ASCII or
	// UTF-8, (as returned from parsing XML data), and converting it to a UTF-16 string in a CString instance.
	// BEW added an overloaded version on 09Aug05, which returns a copy of the constructed CString - this 
	// overloaded version is more useful because it can be then used within the parameter list of function
	// signatures
	// BEW changed 8Apr06 to accomodate the buffer-safe new conversion macros in VS 2003, which
	// use malloc for buffer allocation of long string to be converted, etc.

	// See also the wxString CBString::Convert8To16() function in CBString class.

	//CA2WEX<5120> lpMacroConvertedText((LPCSTR)bstr,eUTF8); // this line commented out in MFC app
	// the following two lines and the last return convStr were the only two non-commented lines
	// in the MFC app
	//CA2W lpMacroConvertedText((LPCSTR)bstr,eUTF8);
	//convStr = lpMacroConvertedText;

	// This function is only called when _UNICODE is defined, so we'll use the appropriate
	// portion of the function of the same name defined in the CBString class.
#if !wxUSE_UNICODE
#error "This program can't be built without wxWidgets library built with wxUSE_UNICODE set to 1"
#endif
	wxWCharBuffer buf(wxConvUTF8.cMB2WC(bstr.GetBuffer()));
	if(!buf.data())
	{
		convStr.Empty();
		return;
	}
	convStr = wxString(buf);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the UTF-16 converted string
/// \param      bstr       -> a CBString containing the single-byte character string to be converted
/// \remarks
/// Called from: the App's DoInputConversion(), the Doc's DoUnpackDocument(), the View's
/// DoConsistentChanges(), FileContainsBookIndicator() in helpers.cpp, ParseXML() in XML.cpp
/// as well as AtBooksAttr() and AtSFMAttr(), and CXMLErrorDlg::InitDialog().
/// Converts a single-byte character string, which is ASCII or UTF-8 (as returned from parsing XML data)
/// to a Unicode (UTF-16) string which it returns.
/// See the overloaded void function which returns the converted string in a reference parameter.
/// See also the complementary Convert16to8() function which converts the other direction.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::Convert8to16(CBString& bstr)
// BEW changed 8Apr06 to accomodate the buffer-safe new conversion macros in VS 2003, which
// use malloc for buffer allocation of long string to be converted, etc.
{
	// See also the wxString CBString::Convert8To16() function in CBString class.

	// This function is only called when _UNICODE is defined, so we'll use the appropriate
	// portion of the function of the same name defined in the CBString class.
#if !wxUSE_UNICODE
#error "This program can't be built without wxWidgets library built with wxUSE_UNICODE set to 1"
#endif
	wxWCharBuffer buf(wxConvUTF8.cMB2WC(bstr.GetBuffer()));
	if(!buf.data())
		return buf;
	return wxString(buf);
}
#endif

#ifdef _UNICODE

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pBuf       <- the buffer which receives the converted text
/// \param      pbyteBuff  -> the byte buffer containing the text to be converted
/// \param      eEncoding  -> the encoding to be used for conversion
/// \param      bHasBOM    -> TRUE if a BOM is present, FALSE otherwise
/// \remarks
/// Called from: the Doc's GetNewFile() and CCCTabbedDialog::DoEditor().
/// Converts a UTF-8 or UTF-16 input text to the required UTF-16 for internal use in Adapt It,
/// removing any BOM that may be present before storing the text in pBuf.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::DoInputConversion(wxString& pBuf, const char* pbyteBuff, wxFontEncoding eEncoding,bool bHasBOM)
// for unicode conversions
// BEW changed 8Apr06 to accomodate the buffer-safe new conversion macros in VS 2003, which
// use malloc for buffer allocation of long string to be converted, etc.
{
	// pbyteBuff is null terminated
	wxChar* lpUnconvertedText;
	switch(eEncoding)
	{
	case wxFONTENCODING_UTF8: //eUTF8
		{
		// we expect this, so make it be first
		if (bHasBOM)
		{
			pbyteBuff += nBOMLen; // skip over BOM
		}
		CBString tempBuff(pbyteBuff);
		pBuf = gpApp->Convert8to16(tempBuff);
		break;
		}
	case wxFONTENCODING_UTF16: //eUTF16:
		{
		if (bHasBOM)
		{
			pbyteBuff += nU16BOMLen; // skip over BOM
		}
		lpUnconvertedText = (wxChar*)pbyteBuff;
		pBuf = lpUnconvertedText;
		break;
		}
	//case eAnsi:
	default: // same as wxFONTENCODING_UTF8 above
		{
		if (bHasBOM)
		{
			// we don't expect this, so just fail for a debug build, 
			// & for release build do nothing
			::wxBell(); 
			wxASSERT(FALSE);
		}
		CBString tempBuff(pbyteBuff);
		pBuf = gpApp->Convert8to16(tempBuff);
		break;
		}
	};
	
}
#endif

// whm Note: The ConvertAndWrite() function might not actually be needed within the WX version because
// the wxTextFile class in wxWidgets automatically takes care of UNICODE and non-Unicode conversions. 
// The places in the MFC app where ConvertAndWrite() is used is for the writing of:
//    1. The WriteFontConfiguration parts of the config files. [WX version uses wxTextFile]
//    2. The WriteBasicSettingsConfiguration config file. [WX version uses wxTextFile]
//    3. The WriteProjectSettingsConfiguration config file. [WX version uses wxTextFile]
//    4. The MakeForeignBasicConfigFileSafe (config file). [WX version uses wxTextFile]
//    5. The OnRetransReport function. [WX version uses ConvertAndWrite now, but could use wxTextFile]
//    6. The DoOneDocReport function (called from DoRetranslationReport). [WX version uses ConvertAndWrite now, but could use wxTextFile]
//    7. The DoKBExport function. [WX version uses ConvertAndWrite now, but could use wxTextFile]
//    8. The DoExportSrcOrTgt function. [WX version uses ConvertAndWrite and wxTextFile doesn't lend
//                                         itself to simple buffer conversions]
// For the above functions that can utilize the wxTextFile class for writing the output, the
// ConvertAndWrite() function would not be needed; but in DoExportSrcOrTgt() the wxTextFile class is
// not really suitable for use there, therefore we retain ConvertAndWrite() in the WX version.

// file in the _UNICODE version.
#ifdef _UNICODE

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      eEncoding           -> (unused in wx version)
/// \param      pFile               <- a pointer to the wxFile being written
/// \param      str                 -> the string being converted and written
/// \param      bNoTerminatingNull  -> if TRUE we don't allow for a terminating null (line-based output)
/// \remarks
/// Called from: the View's OnRetransReport(), DoOneDocReport(), DoKBExport(), and DoExportSrcOrTgt()
/// in ExportFunctions.cpp.
/// Converts the string str from its internal UTF-16 to UTF-8 and writes it to pFile.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::ConvertAndWrite(wxFontEncoding WXUNUSED(eEncoding), wxFile* pFile, wxString& str) //, bool bNoTerminatingNull)
{
	// MFC original Note: convert from internal UTF-16 to either the user's legacy encoding, or for all other input
	// encodings, (including UTF-16 or variants thereof) to UTF-8; and then write the bytes out
	// BEW changed 8Apr06, to use the new conversion macros in VS2003 which don't allocate a buffer on the stack
	
	wxCharBuffer tempBuf = str.mb_str(wxConvUTF8);
	size_t nLen = strlen(tempBuf); //strlen(lpMacroConvertedText);
	// don't allow for a terminating null when we are doing line-based output 
	// (ie. the flag is TRUE)
	// whm Note: In the wx version we handle the output of platform appropriate eol sequences as a
	// separate output process, so the bNoTerminatingNull is always TRUE.
	//if (!bNoTerminatingNull) // whm: defaults to TRUE and never explicitly called with FALSE in wx version, so not needed
	//	nLen++;
	
	// Note: Early on in Unicode builds, tempBuf ended up with only the first character
	// of the string str (a backslash). This resulted in the pFile->Write(tempBuf,nLen) 
	// and the alternate pFile->Write(str) function calls outputting only a backslash 
	// character. The failure turned out to happened in the str.mb_str(wxConvUTF8) conversion 
	// which is apparently also used implicitly in the pFile-Write(str) call. 
	// It turns out that the reason for the failure was that in the caller (the View), the
	// source.UngetWriteBuf() call was not made to get the buffer back into shape before 
	// passing the source string to ConvertAndWrite!!
	
	// write it out
	pFile->Write(tempBuf,nLen);
	
	// whm note: the following defaults to doing the same thing as the code above
	// using the str.mb_str(wxConvUTF8) call and the wxCharBuffer.
	//pFile->Write(str); // default for wxFile Write is wxConvUTF8
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a CBString representing the UTF-8 converted string
/// \param      str       -> a wxString representing a UTF-16 string to be converted to UTF-8
/// \remarks
/// Called from: the App's MakeKBElementXML(), DoKBSaveAsXML(), the Doc's ConstructSettingsInfoAsXML(),
/// from CSourcePhrase::MakeXML(), and MakeMSWORDWarning() in XML.cpp.
/// Converts a UTF-16 string to UTF-8.
/// See the complementary Convert8to16() function which converts the other direction.
////////////////////////////////////////////////////////////////////////////////////////////
CBString CAdapt_ItApp::Convert16to8(const wxString& str)
{
	// converts UTF-16 strings to UTF-8, doing it in a buffer on the stack
	// BEW changed 10Apr06, to use 'safe' conversion macro, rather than one which allocates
	// a buffer on the stack, as the latter could cause a crash for large files being processed
	
	// The wxString::mb_str() method returns a wxCharBuffer. The wxConvUTF8 is a predefined
	// instance of the wxMBConvUTF8 class which converts between Unicode (UTF-16) and UTF-8.
	// The CBString constructor is happy to accept the wxCharBuffer
	wxCharBuffer tempBuf = str.mb_str(wxConvUTF8);
	return CBString(tempBuf);
}

#endif // for _UNICODE


////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxUpdateUIEvent that is generated when the Advanced Menu is about
///                         to be displayed
/// \remarks
/// Called from: The wxUpdateUIEvent mechanism when the associated menu item is selected, and before
/// the menu is displayed.
/// If the application is in Free Translation Mode or the m_pSourcePhrases pointer is NULL, the 
/// "Transform Adaptations Into Glosses..." menu item on the Advanced menu is disabled and this 
/// handler returns immediately.
/// If m_pSourcePhrases has at least one item in its list and the appropriate KB is in a ready state,
/// "Transform Adaptations Into Glosses..." menu item on the Advanced menu is enabled, otherwise it 
/// is disabled. 
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnUpdateAdvancedTransformAdaptationsIntoGlosses(wxUpdateUIEvent& event)
// BEW ammended 31Aug05; enabled if a project is open but no document is open, and
// we no longer care whether book mode is on or not, or if it has been disabled - because
// what will happen depends on whether book folders are in the Adaptations folder of the
// chosen project for transformation -- if they are there, we must create them in the
// current project too (if not already there)
{
	// wxWidgets uses OnIdle time to poll the wxUpdateUIEvents. Hence, onUpdate
	// handlers get called earlier in the wxWidgets version than in the MFC version.
	// Hence this, and other OnUpdate handlers get called when the m_pSourcePhrases
	// pointer is still NULL, causing the following if() line to crash. 
	//if (GetDocument()->m_pSourcePhrases->GetCount() == 0 && m_bKBReady && m_bGlossingKBReady)
	// To prevent this I've added a conditional statement to prevent the test being made when
	// the Doc's m_pSourcePhrases member pointer is NULL. 
	// TODO: I think a better approach would be to have actual global boolean flags called 
	// bProjectOpen and bDocumentOpen that could be used for the many cases (especially 
	// for the OnUpdate... handlers) when certain actions need to be taken based on 
	// whether the project and/or document is open or not.

	if (m_bFreeTranslationMode)
	{
		event.Enable(FALSE);
		return;
	}

	// If the project and/or the doc are not yet set up don't enable the interface
	if (m_pSourcePhrases == NULL)
	{
		event.Enable(FALSE);
		return;
	}

	if (m_pSourcePhrases->GetCount() == 0 && m_bKBReady && m_bGlossingKBReady)
	{
		event.Enable(TRUE);
	}
	else
	{
		event.Enable(FALSE);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxCommandEvent that is generated when the associated menu item is
///                         selected
/// \remarks
/// Called from: The Event Table of the CAdapt_ItApp class in response to a "Transform Adaptations
/// Into Glosses..." command on the Advanced menu.
/// Shows the CTransformToGlossesDlg dialog which allows the advanced user to take adaptations
/// done in a different project and turn them into glosses for reference in the current project. 
/// Usually the adaptations taken are those which were done as a back translation of a different, 
/// but related language project - the back translations being done into a major language. If the 
/// source language for the back translation project was a language related to the current 
/// language and is being contemplated as the source language for adaptation work into the current 
/// language, this command can be used to pre-populate Adapt It ready documents for the current 
/// project that contain the major language adaptations (from the other back translation project) 
/// and transform them into glosses for reference while adapting in the current project. Such 
/// pre-populated gloss lines can help explain the meaning of the source text for MTTs working 
/// on adapting from that "other" language into the current language. See "Transform Adaptations
/// Into Glosses" in the Adapt It Basics.doc reference document in the Adapt It start menu group.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnAdvancedTransformAdaptationsIntoGlosses(wxCommandEvent& WXUNUSED(event))
{
	CTransformToGlossesDlg dlg(GetMainFrame());
	dlg.Centre();
	if (dlg.ShowModal() == wxID_OK)
	{
		// user is ready to proceed (i.e., pressed "Yes").
		gbExcludeCurrentProject = TRUE; // cause suppression of name of current project

		// save entry state (only necessary if entry state had book mode on)
		BookNamePair* pSave_BookNamePair = m_pCurrBookNamePair;
		int nSave_BookIndex = m_nBookIndex;
		wxString save_bibleBooksFolderPath = m_bibleBooksFolderPath;

		// The process will open documents in another project; and these have various settings
		// stored within them (such as whether or not book mode was turned on when the document
		// was created, and SF set and which markers are filtered, etc). These settings should
		// be preserved into the transformed document files, and we do so. The document settings
		// which are in effect when the last such document has been processed will therefore be
		// the settings which obtained for that document when last saved in the 'other' project.
		// In particular, book mode may have been on or off, and so - depending on what the setting
		// is in the last document processed - it may get turned on as a byproduct of the process,
		// or turned off. There is no reliable way to do otherwise, such as to have the process 
		// support document settings originally in the receptor project, since typically there will
		// be no documents in the receptor project when the process is initiated. The last secttion
		// of code in this block will therefore aim to get the application consistent with whatever
		// was the last document processed

		bool bOK;
		bOK = AccessOtherAdaptionProject(); // see the list and choose one

		// restore the flag to its default value
		gbExcludeCurrentProject = FALSE;
		// restore the former book mode parameters (even if book mode was not on on entry)
		m_pCurrBookNamePair = pSave_BookNamePair;
		m_nBookIndex = nSave_BookIndex;
		m_bibleBooksFolderPath = save_bibleBooksFolderPath;
		// now, if the user opens the Document tab of the Start Working wizard, and book
		// mode is on, then at least the path and index and book name are all consistent

		// determine what the last document's settings are, and make sure supporting
		// variables are consistent with them - what this boils down to is to determine if
		// book mode has been turned on, and if so, to make sure that the app member
		// m_bibleBooksFolderPath is not null; if it is null, then turn off book mode to
		// force the user to manually turn it back on if he wants it, which will set everything
		// up correctly. (Note: m_bibleBooksFolderPath is set to the folder path in the function
		// DoTransformationsToGlosses, just after the other project's document has been loaded)
		if (m_bBookMode)
		{
			if (m_bibleBooksFolderPath.IsEmpty())
			{
				// no path defined, so having the mode on would lead to a crash, so turn it off
				m_bBookMode = FALSE;
				m_nBookIndex = -1;
				m_nDefaultBookIndex = 39;
				m_nLastBookIndex = 39;
			}
		}
	}
	else
	{
		// user is not ready to proceed, so he clicked the No button
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if no error
/// \remarks
/// Called from: the App's OnAdvancedTransformAdaptationsIntoGlosses().
/// Presents the user with the COpenExistingProjectDlg dialog which lists possible projects. 
/// Based on the user's selection it computes the needed paths to the other project which 
/// is to supply the adaptations to be transformed into glosses for both (glossing) KB and 
/// documents.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::AccessOtherAdaptionProject()
{
	// BEW added 05Jan07 to enable work folder on input to be restored when done
	wxString strSaveCurrentDirectoryFullPath = GetDocument()->GetCurrentDirectory();

	bool bSuccess = FALSE;
	CAdapt_ItDoc* pDoc = GetDocument();
	wxASSERT(pDoc);
	COpenExistingProjectDlg dlg(GetMainFrame());
	dlg.Centre();

	// put up the dialog
	if(dlg.ShowModal() == wxID_OK)
	{
		// get the chosen "other" project's name
		wxString strOtherProjectName = dlg.m_projectName;

		// define paths to the other project's kb file and adaptations folder
		wxString strOtherProjectPath = m_workFolderPath + PathSeparator + strOtherProjectName;
		wxString strOtherAdaptationsPath = strOtherProjectPath + PathSeparator + m_adaptionsFolder;
		// m_adaptionsFolder is defined in the app creator as _T("Adaptations")
		
		// determine whether or not there are Bible book folders in the Adaptations folder
		// of the "other" project (the one to be transformed)
		// whm note: AreBookFoldersCreated() has the side effect of changing the current work directory
		// to the passed in strOtherAdaptationsPath.
		gbHasBookFolders = AreBookFoldersCreated(strOtherAdaptationsPath);
		if (gbHasBookFolders)
		{
			// there may not be any book folders yet in the Adaptations folder of the current 
			// project (the one which is going to accept the transformed KBs and documents), if
			// that is the case, then insert a set of folders there ready for receiving the
			// transformed documents
			// whm note: AreBookFoldersCreated() has the side effect of changing the current work directory
			// to the passed in m_curAdaptionsPath.
			bool bCurrentProjectHasBookFolders = AreBookFoldersCreated(m_curAdaptionsPath);
			if (!bCurrentProjectHasBookFolders)
			{
				// install them
				CreateBookFolders(m_curAdaptionsPath,m_pBibleBooks);
			}
		}

		// in version 3 we have the possibility of KB files being binary (*.KB) or XML (*.xml)
		// and either can be on disk when the m_gSaveAsXML flag has the 'opposite' value to
		// what would be expected from the kb file's extension, so we have to set up alternate
		// names and check for what file type is actually present, and use that
		wxString strOtherKBName = strOtherProjectName + _T(".KB");
		wxString strOtherKBNameXML = strOtherProjectName + _T(".xml");
		wxString strOtherKBPath = strOtherProjectPath + PathSeparator + strOtherKBName;
		wxString strOtherKBPathXML = strOtherProjectPath + PathSeparator + strOtherKBNameXML;

		// count the entries in the current project's KB, since we must warn the user that
		// it's contents are now about to be removed, so if the user is in a project in which
		// some work has been done, he better know that this operation (if he continues) is 
		// going to clobber the KB (because we assume that the adaptations KB should be empty
		// following this operation which will fill up the glossing KB only, so we will make
		// the adaptations KB empty in preparation for others to use the transformed documents.)
		wxASSERT(m_pKB != NULL);
		int nCount = 0;
		for (int i = 0; i < MAX_WORDS; i++)
		{
			nCount += m_pKB->m_pMap[i]->size();
		}
		// also count the glossing KB's entries, but we won't make any decisions based on the
		// result, though we will tell the user it will be cleared and how many entries are in it
		int nGlossingCount = m_pGlossingKB->m_pMap[0]->size();

		// the user will have to decide if he is willing to let the current kb's contents
		// be cleared, likewise the current glossing KB
		wxString message;
		// IDS_ABOUT_TO_CLOBBER_KB
		message = message.Format(_("Warning: the Transform Adaptations Into Glosses command will clear out the contents of the current project's knowledge base, which contains %d entries.\nLikewise for the glossing knowledge base, which contains %d entries.\nAre you willing for this to happen?"),nCount,nGlossingCount);
		int nGoAhead = wxYES; // default (proceed)

		nGoAhead = wxMessageBox(message,_T(""), wxYES_NO);
		if (nGoAhead == wxNO)
		{
			// whm added 05Jan07 for safety sake restore the former current working directory
			// to what it was on entry. The AreBookFoldersCreated(m_curAdaptionsPath) call above
			// changes the current working directory to m_curAdaptionsPath. (This change not in MFC version)
			bool bOK;
			bOK = ::wxSetWorkingDirectory(strSaveCurrentDirectoryFullPath);
			return FALSE; // user baled out by clicking NO button, so do nothing
		}

		// okay, clean out the glossing KB ready for the new content (we don't want to
		// just add the new content, since the user might invoke the Transform Adaptations
		// Into Glosses command more than once, so that an addition schema would result in
		// duplicates and hence a glossing KB with incorrect reference counts)
		if (nGlossingCount > 0)
		{
			pDoc->EraseKB(m_pGlossingKB);
			m_pGlossingKB = new CKB; // don't store yet, in case failure occurs we'd want old
									 // contents left undisturbed
			wxASSERT(m_pGlossingKB != NULL);
			m_bGlossingKBReady = TRUE;
			wxASSERT(m_pGlossingKB->m_pTargetUnits->GetCount() == 0);
		}

		// if we get to here, the user is committed, the current project's kb is cleared,
		// and so we go ahead with the transformations -- after we work out what kind of file
		// the one we want is - binary (ie. .KB) or XML (ie. .xml)
		bool bXMLforKB;
		if (wxFileExists(strOtherKBPath))
		{
			// there is a binary KB file present - so use it
			bXMLforKB = FALSE;
		}
		else if (wxFileExists(strOtherKBPathXML))
		{
			// there is an XML KB file present - so use that one
			bXMLforKB = TRUE;
		}
		else
		{
			// we could not detect any valid KB file, so abort the operation; don't expect this
			// to ever be the case, so it can be a hard coded English message for debug purposes only
			wxMessageBox(_T("Aborting the Transform operation, because no valid KB file was detected"),_T(""),wxICON_WARNING);
			// whm added 05Jan07 for safety sake restore the former current working directory
			// to what it was on entry. The AreBookFoldersCreated(m_curAdaptionsPath) call above
			// changes the current working directory to m_curAdaptionsPath. (This change not in MFC version)
			bool bOK;
			bOK = ::wxSetWorkingDirectory(strSaveCurrentDirectoryFullPath);
			return FALSE;
		}

		// "Load" the other project's adaptations KB. Code for this will be plagiarized from
		// the app class's LoadKB() function, but using a local CKB pointer to access the KB
		CKB* pOtherKB = new CKB;
		if (bXMLforKB)
		{
			bool bReadOK = ReadKB_XML(strOtherKBPathXML, pOtherKB);
			if (!bReadOK)
			{
				// a bad read or parsing - if so, there will have been an XML error report
				// already, so just abort the command
				// IDS_NO_OTHER_KB
				wxMessageBox(_("Error: the application could not find the other project's knowledge base, or failed to open and load it. The command has therefore been ignored."), _T(""), wxICON_INFORMATION);
				// whm added 05Jan07 for safety sake restore the former current working directory
				// to what it was on entry. The AreBookFoldersCreated(m_curAdaptionsPath) call above
				// changes the current working directory to m_curAdaptionsPath. (This change not in MFC version)
				bool bOK;
				bOK = ::wxSetWorkingDirectory(strSaveCurrentDirectoryFullPath);
				return FALSE; // abandon the command, the adaptations KB couldn't be opened
			}
		}
// WX version doesn't use binary KB serialization
//		else
//		{
//			// the kb file we found was a binary one
//			if (!f.Open(strOtherKBPath, CFile::modeRead | CFile::shareExclusive,&e))
//			{
//d:					AfxMessageBox(IDS_NO_OTHER_KB, MB_ICONINFORMATION);
//					return FALSE; // abandon the command, the adaptations KB couldn't be opened
//			}
//			CArchive ar( &f, CArchive::load,8192);
//			pOtherKB->Serialize(ar);
//			ar.Close();
//			f.Close();
//		}


		// the other KB is now in memory, so we can scan its contents and transfer them to
		// the current project's glossing KB; remember, null src phrases must not be transferred,
		// nor <Not In KB> entries, and all maps are squished into the one map in the glossing KB
		// (code pinched from DoKBIntegrityCheck() and then modified)
		int nMaxIndex = pOtherKB->m_nMaxWords - 1; // index of highest map having content
		MapKeyStringToTgtUnit* pMap = (MapKeyStringToTgtUnit*)NULL;
		CTargetUnit* pTgtUnit = (CTargetUnit*)NULL;
		CRefString* pRefString = (CRefString*)NULL;
		wxString key;
		wxString storedStr;
		MapKeyStringToTgtUnit::iterator iter;
		for (int index = 0; index <= nMaxIndex; index++)
		{
			pMap = pOtherKB->m_pMap[index];
			wxASSERT(pMap != NULL && pMap->size() >= 0);
			if (pMap->size() == 0)
				continue; // could be some of the maps are empty
			else // the map is not empty, so process its contents
			{
				iter = pMap->begin(); 
				while (iter != pMap->end()) //while (pos != 0)
				{
					// get the next association (ie. a CTargetUnit instance associated
					// with a key string
					key = iter->first;
					pTgtUnit = iter->second; 
					iter++;
	
					// we now have a key, and its associated CTargetUnit instance. These
					// will need to go in the glossing KB except as follows: we throw away
					// the CTargetUnit instance if it is for a null source phrase
					// (key = "..."), or if it contains "<Not In KB>"
					if (key == _T("..."))
						continue;
					TranslationsList::Node* tuPos = pTgtUnit->m_pTranslations->GetFirst();
					pRefString = (CRefString*)tuPos->GetData();
					storedStr = pRefString->m_translation;
					if (storedStr == _T("<Not In KB>"))
						continue;

					// if we get to here, then we have a target unit which has to go in the
					// glossing KB's list, and its association with the key has to go into 
					// the glossing KB's first map.
					CTargetUnit* pGlossingTgtUnit = new CTargetUnit; // create an empty one
					wxASSERT(pGlossingTgtUnit != NULL);
					pGlossingTgtUnit->Copy(*pTgtUnit); // copy it (a copy constructor does not
													  // work, hence the two step workaround)
					wxASSERT(pGlossingTgtUnit->m_pTranslations->GetCount() >= 1);
					m_pGlossingKB->m_pTargetUnits->Append(pGlossingTgtUnit); // in the list
					(*m_pGlossingKB->m_pMap[0])[key] = pGlossingTgtUnit; // in the map
					m_pGlossingKB->m_nMaxWords = 1; // always 1 for the glossing KB
				} // end block for scanning all associations stored in the current map
			} // end block for processing a map with contents
		} // end for loop for processing all maps

		// clean up the kbs
		if (nCount > 0)
		{
			// clean out the adaptations kb (actually delete it and make a new empty one)
			// (delayed to here, in case the transformation process failed, in which case
			// we'd prefer the current adaptations KB to be left untouched)
			pDoc->EraseKB(m_pKB); //pDoc->EraseKB(m_pKB);
			m_pKB = new CKB;
			m_bKBReady = TRUE;
			wxASSERT(m_pKB);
			wxASSERT(m_pKB->m_pTargetUnits->GetCount() == 0);
			bool bStoredOK = StoreKB(m_bAutoBackupKB);
			// unlikely to fail, so an English message hardcoded will do
			if (!bStoredOK)
				wxMessageBox(_T("The erased adaptations KB did not successfully store to disk"),
				_T(""), wxICON_INFORMATION);
		}

		// delete the other project's KB from the heap
		pDoc->EraseKB(pOtherKB); 

		// store the filled glossing KB on disk; leave it open since the user may want to
		// do some work or examine the glossing KB in the KB editor etc.
		bool bOK = StoreGlossingKB(m_bAutoBackupKB);
		// unlikely to fail, so an English message hardcoded will do
		if (!bOK)
		{
			wxMessageBox(_T(
			"Warning: the newly filled glossing KB did not successfully store to disk"),
				_T(""), wxICON_INFORMATION);
		}

		// now we must look at all the documents in the other project, making copies which
		// have the adaptations shifted to the m_gloss member, and saved to the current project's
		// Adaptations folder. Code for this kind of stuff is to be found in the handler
		// OnFileRestoreKB( ) and its auxiliary function, DoKBRestore( ), with a few modifications
		// needed for our present task

		// first, get a list of all the documents the user wants transformed (normally all of
		// them, but the dialog which comes up allows fewer than all to be worked on)
		m_acceptedFilesList.Clear(); // ensure we start with a clear list
		
		// BEW 31Aug05, for version 3 we must deal first with document files in the Adaptations folder,
		// as in the legacy app, but then if there are embedded book folders in the Adaptations folder,
		// we must loop through each such folder and enumerate the doc files in each, and call
		// DoTransformationsToGlosses() for each document in each of the book folders
		
		// Enumerate the document files (fills m_acceptedFilesList with the document filenames, which
		// potentially could be a mix of *.xml and *.adt names, and some book folders can be expected
		// to contain no documents) -- this first enumeration is for the Adaptations folder only
		bool bIsOK;
		// whm note: EnumerateDocFiles() has the side effect of changing the current work directory
		// to the passed in strOtherAdaptationsPath.
		bIsOK = EnumerateDocFiles(pDoc, strOtherAdaptationsPath);
		if (!bIsOK)
		{
			// if no English error message occurs, then the user cancelled & the process should be halted
			// -- either way, halt the process if FALSE is returned before looping through the Adaptations
			// folder's document file set
			// whm added 05Jan07 for safety sake restore the former current working directory
			// to what it was on entry. The EnumerateDocFiles(strOtherAdaptationsPath) call above
			// changes the current working directory to strOtherAdaptationsPath. (This change not in MFC version)
			bool bOK;
			bOK = ::wxSetWorkingDirectory(strSaveCurrentDirectoryFullPath);
			return FALSE;
		}
		if (m_acceptedFilesList.GetCount() == 0 && !gbHasBookFolders)
		{
			// no documents to work on, and no book folders either, so abort the operation
			// IDS_NO_DOCUMENTS_YET
			wxMessageBox(_("Sorry, there are no saved document files yet for this project. At least one document file is required for the operation you chose to be successful. The command will be ignored."),_T(""), wxICON_EXCLAMATION);
			// whm added 05Jan07 for safety sake restore the former current working directory
			// to what it was on entry. The EnumerateDocFiles(strOtherAdaptationsPath) call above
			// changes the current working directory to strOtherAdaptationsPath. (This change not in MFC version)
			bool bOK;
			bOK = ::wxSetWorkingDirectory(strSaveCurrentDirectoryFullPath);
			return FALSE;
		}

		// do the transformation of the other project's documents - changing adaptations into
		// glosses, and storing the transformed documents in the current project
		wxString bookFolderName;
		bookFolderName.Empty();
		bool bTransformedOK;
		bTransformedOK = DoTransformationsToGlosses(pDoc, strOtherAdaptationsPath, bookFolderName);

		if (gbHasBookFolders)
		{
			// process this block only if the 'other' project's Adaptations folder contains the set of
			// Bible book folders - these could contain documents, and some or all could be empty; 
			// NOTE: the code below is smart enough to ignore any user-created folders which are sisters
			// of the Bible book folders for which the Adaptations folder is the common parent folder
			wxDir finder;
			bool bOK = (::wxSetWorkingDirectory(strOtherAdaptationsPath) && finder.Open(strOtherAdaptationsPath)); // wxDir must call .Open() before enumerating files!
			if (!bOK)
			{
				wxString s1, s2, s3;
				s1 = _T(
				"Failed to set the current directory to the Adaptations folder in AccessOtherAdaptionProject function, ");
				s2 = _T("processing book folders, so the book folder document files are absent in the transformed project.");
				s3 = s1 + s2;
				wxMessageBox(s3,_T(""),wxICON_EXCLAMATION);
				// whm added 05Jan07 for safety sake restore the former current working directory
				// to what it was on entry. The wxSetWorkingDirectory(strOtherAdaptationsPath) call above
				// changes the current working directory to strOtherAdaptationsPath. (This change not in MFC version)
				bool bOK;
				bOK = ::wxSetWorkingDirectory(strSaveCurrentDirectoryFullPath);
				return FALSE;
			}
			else
			{
				// Must call wxDir::Open() before calling GetFirst() - see above
				wxString str = _T("");
				// whm note: in GetFirst below, wxDIR_FILES | wxDIR_DIRS flag finds files or directories, but not . or .. or hidden files
				bool bWorking = finder.GetFirst(&str,wxEmptyString,wxDIR_FILES | wxDIR_DIRS);
				while (bWorking)
				{
					// whm note: in the MFC version's "if (finder.IsDirectory())" test below, the finder continues
					// to use the directory path that was current when the inital finder.FindFile call was made
					// above, even though the EnumerateDocFiles() call below changes the current working dir for
					// each of the book folder directories it processes. In the wx version the finder.Exists(str)
					// call uses whatever the current working directory is and checks for a sub-directory "str"
					// below that - a difference we must account for here in the wx version.
					// whm Note: The Exists() method of wxDIR used below returns TRUE if the passed name IS a directory.
					// TODO: Test that this while loop succeeds in traversing all book folders
					if (finder.Exists(strOtherAdaptationsPath + PathSeparator + str))
					{
						// BEW changed 25Aug05, so that other user-defined folders can be
						// in the Adaptations folder without making the app confused
						// as to whether or not Bible Book folders are present or not

						// we have found a folder, check if it matches one of those in
						// the array of BookNamePair structs (using the seeName member)
						if (IsDirectoryWithin(str,m_pBibleBooks))
						{
							// we have found a folder name which belongs to the set of
							// Bible book folders, so construct the required path to the
							// folder and enumerate is documents then call
							// DoTransformationsToGlosses() to process any documents within
							wxString otherFolderPath = strOtherAdaptationsPath;
							otherFolderPath += PathSeparator + str;

							// clear the string list of directory names & then enumerate the
							// directory's file contents
							m_acceptedFilesList.Clear();
							// whm note: EnumerateDocFiles() has the side effect of changing the current work directory
							// to the passed in otherFolderPath.
							bIsOK = EnumerateDocFiles(pDoc, otherFolderPath, TRUE); // TRUE == suppress dialog
							if (m_acceptedFilesList.GetCount() == 0)
							{
								// no documents to work on in this folder, so iterate
								bWorking = finder.GetNext(&str);
								continue;
							}

							// there are files to be processed, so do the transformations (the function
							// internally does the required saving of the transformed document as part
							// of the current project) TRUE parameter suppresses the statistics dialog.
							bTransformedOK = DoTransformationsToGlosses(pDoc,otherFolderPath,str,TRUE);
						}
						else
						{
							bWorking = finder.GetNext(&str);
							continue;
						}
					}
					else
					{
						// its a file, so ignore it
						bWorking = finder.GetNext(&str);
						continue;
					}
					bWorking = finder.GetNext(&str);
				} // end of while (bWorking)
			}
		}

		// clean out the app's string list for the list of doc files
		m_acceptedFilesList.Clear(); //m_acceptedFilesList.RemoveAll();
	}

	// BEW added 05Jan07 to restore the former current working directory
	// to what it was on entry
	bool bOK;
	bOK = ::wxSetWorkingDirectory(strSaveCurrentDirectoryFullPath);

	return bSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     always TRUE
/// \param      pDoc                 -> a pointer to the document
/// \param      folderPath           <- the full path to whatever folder in the other project is
///                                     supplying the documents being currently transformed (it could
///                                     be the Adaptations folder, or any one of the embedded Bible
///                                     book folders if that mode has previously been turned on)
/// \param      bookFolderName       <- an empty string when the function is being called on the Adaptations
///                                     folder; otherwise pass the name of the currently being accessed
///                                     Bible book folder
/// \param      bSuppressStatistics  -> If FALSE (default) presents statistics for the operation,
///                                     otherwise does the transformations quietly
/// \remarks
/// Called from: the App's AccessOtherAdaptionProject().
/// Transforms another project's documents - changing adaptations into glosses, and stores the 
/// transformed documents in the current project.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::DoTransformationsToGlosses(CAdapt_ItDoc* pDoc, wxString& folderPath, 
											  wxString& bookFolderName, bool bSuppressStatistics)
// (Code for this function I will steal from DoKBRestore()
// BEW updated it on 31Aug05 to comply with version 3 - specifically, to handle book folders, 
// and the possibility of some or all documents being XML rather than binary).
// The bSuppressStatistics defaults to FALSE (in the prototype)
{
	wxArrayString List = m_acceptedFilesList;
	int nCount = List.GetCount();
	wxASSERT(nCount > 0);
	int nTotal = 0;
	int nCumulativeTotal = 0;

	// get a pointer to the view (since it's SDI interface, our implementation
	// has the view always present, as the doc and its template get reused)
	CAdapt_ItView* pView = GetView();
	wxASSERT(pView != NULL);

	// lock view window updates till done
	wxGetApp().GetMainFrame()->canvas->Freeze();

	// iterate over the document files
	for (int i=0; i < nCount; i++)
	{
		wxString newDocName = List[i];
		wxASSERT(!newDocName.IsEmpty());
		wxString ourProjectsDocFileName = newDocName;
		wxString curOutputPath; // for full path to the transformed document to be saved in current project
		wxString newPathName; // for the full path to whatever document is currently to be loaded
							 // and transformed
		// Work out whether we have an xml or adt document, set global gbProcessingXMLDoc 
		// accordingly and set up the needed paths (if document backups are or were turned
		// on, there may be *.BAK.xml files present - but these will automatically be ignored
		// because GetPossibleAdaptionDocuments() which EnumerateDocFiles() calls will not 
		// have included any such files in the m_acceptedFilesList)
		wxString extn;
		newDocName = MakeReverse(newDocName);
		extn = newDocName.Left(3);
		extn = MakeReverse(extn);
		newDocName = MakeReverse(newDocName);
		bool bProcessingXMLDoc;
		if (extn == _T("xml"))
		{
			// it is an XML document file we are about to process
			bProcessingXMLDoc = TRUE;
		}
		else
		{
			// its a binary document file
			bProcessingXMLDoc = FALSE;
		}

		// we'll save the transformed document file in whatever format is indicated by the
		// current setting of the app's m_bSaveAsXML flag value

		// create the path to the other project's document file using the passed in
		// folderPath and the bookFolderName; then create the needed output path to 
		// to either the Adaptations folder in current project, or to the same named Bible
		// book folder if processing doc files from a Bible book folder -- to determine if
		// the latter is the case, use the bookFolderName as a flag -- it will be empty when
		// we are processing documents in an Adaptations folder, and it will be non-empty
		// when processing documents from a Bible book folder
		newPathName = folderPath + PathSeparator + newDocName;
		wxString bookFolderPath;
		if (bookFolderName.IsEmpty())
		{
			// we are processing documents from an Adaptations folder, not an embedded
			// Bible book folder in the Adaptations folder
			curOutputPath = m_curAdaptionsPath + PathSeparator + ourProjectsDocFileName;
			bookFolderPath.Empty();
		}
		else
		{
			// we are processing documents from one of the Bible book folders
			curOutputPath = m_curAdaptionsPath + PathSeparator;
			curOutputPath += bookFolderName;
			bookFolderPath = curOutputPath; // extract the path to the book folder
			curOutputPath += PathSeparator + ourProjectsDocFileName; // the path to the file
		}

		// OpenDocumentInAnotherProject() handles opening xml or binary documents
		// without the caller needing to specify which type has been passed in
		
		bool bOK = pDoc->OpenDocumentInAnotherProject(newPathName);
		if (bOK)
		{
			// why do I do set the path name? Should not be necessary since the view is 
			// frozen while the documents are processed, but there is no harm in it
			pDoc->SetFilename(newPathName,TRUE);

			// the document may be the last to be processed, so we must ensure that if it
			// turned book mode on then we have a valid path for the app member 
			// m_bibleBooksFolderPath, and m_nLastBookIndex can be set to m_nBookIndex
			// as obtained from the document settings as loaded
			if (gpApp->m_bBookMode && !m_bDisableBookMode)
			{
				gpApp->m_bibleBooksFolderPath = bookFolderPath;
				gpApp->m_nLastBookIndex = gpApp->m_nBookIndex;
			}
			else if (m_bDisableBookMode)
			{
				// if the XML read did not succeed, then disable the mode and 
				// set parameters accordingly
				gpApp->m_bibleBooksFolderPath.Empty();
				gpApp->m_nBookIndex = -1;
				gpApp->m_nLastBookIndex = 39; // Matthew
				gpApp->m_nDefaultBookIndex = 39;
				gpApp->m_bBookMode = FALSE;
			}
			
			nTotal = m_pSourcePhrases->GetCount();
			wxASSERT(nTotal > 0);
			nCumulativeTotal += nTotal;
#ifdef __WXMSW__
			wxString progMsg = _("%s  - %d of %d Total words and phrases");
			wxString msgDisplayed = progMsg.Format(progMsg,ourProjectsDocFileName.c_str(),1,nTotal);
			wxProgressDialog progDlg(_("Transformations To Glosses"),
                            msgDisplayed,
                            nTotal,    // range
                            GetMainFrame(),   // parent
                            //wxPD_CAN_ABORT |
                            //wxPD_CAN_SKIP |
                            wxPD_APP_MODAL |
                            // wxPD_AUTO_HIDE | -- try this as well
                            wxPD_ELAPSED_TIME |
                            wxPD_ESTIMATED_TIME |
                            wxPD_REMAINING_TIME
                            | wxPD_SMOOTH // - makes indeterminate mode bar on WinXP very small
                            );
#else
			// wxProgressDialog tends to hang on wxGTK so I'll just use the simpler CWaitDlg
			// notification on wxGTK and wxMAC
			// put up a Wait dialog - otherwise nothing visible will happen until the operation is done
			CWaitDlg waitDlg(gpApp->GetMainFrame());
			// indicate we want the reading file wait message
			waitDlg.m_nWaitMsgNum = 5;	// 5 hides the static leaving only "Please wait..." in title bar
			waitDlg.Centre();
			waitDlg.Show(TRUE);
			waitDlg.Update();
			// the wait dialog is automatically destroyed when it goes out of scope below.
#endif

			SPList* pPhrases = m_pSourcePhrases;
			SPList::Node* pos1;
			pos1 = pPhrases->GetFirst();
			wxASSERT(pos1 != NULL);
			int counter = 0;
			while (pos1 != NULL)
			{
				SPList::Node* savePos = pos1;
				CSourcePhrase* pSrcPhrase = (CSourcePhrase*)pos1->GetData();
				pos1 = pos1->GetNext();
				counter++;

				// update the glossing or adapting KB for this source phrase
				bool bRemoveIt = FALSE;
				bRemoveIt = 
					pView->TransformSourcePhraseAdaptationsToGlosses(savePos,pos1,pSrcPhrase);

				// if it needs to be removed, do so; (any adjustments to flags which are needed will
				// already have been done in the TransformSourcePhraseAdaptationsToGlosses( ) function)
				if (bRemoveIt)
				{
					// removing the one at savePos, so pos1 will remain valid since it is next
					pDoc->DeleteSingleSrcPhrase(pSrcPhrase);
					pPhrases->DeleteNode(savePos);
					pDoc->UpdateSequNumbers(0); // update from the start to be safe
				}
#ifdef __WXMSW__
				// update the progress bar every 20th iteration
				if (counter % 1000 == 0)
				{
					msgDisplayed = progMsg.Format(progMsg,ourProjectsDocFileName.c_str(),counter,nTotal);
					progDlg.Update(counter,msgDisplayed);
				}
#endif
			}

			bool bSavedOK;
			bSavedOK = pDoc->DoTransformedDocFileSave(curOutputPath);

			pView->ClobberDocument();
		}
		else
		{
			// user needs to know a document could not be processed; the message can be in English
			// because it is unlikely to ever be seen
			wxString errStr;
			errStr = errStr.Format(_("The document with pathname: %s, was not opened and therefore not processed."),
				newPathName.c_str());
			wxMessageBox(errStr, _T(""), wxICON_EXCLAMATION);
			//goto a; // allow the loop to continue
		}

		if (m_pBuffer != NULL)
		{
			delete m_pBuffer;
			m_pBuffer = (wxString*)NULL;
		}
	}

	// inform user of success and some statistics
	if (!bSuppressStatistics)
	{
		// we suppress this information when processing through the documents in the
		// 67 Bible book folders - otherwise the user might have to manually dismiss
		//  an information dialog some hundreds of times!!!
		wxString stats;
		// IDS_TRANSFORMATION_DONE
		stats = stats.Format(_("The documents you chose from the other project have been transformed and copied to the current project. A total of %d source words and phrases were transformed, and these occur in %d  documents."),nCumulativeTotal,nCount);
		wxMessageBox(stats,_T(""),wxICON_INFORMATION);
	}

	// let the view respond again to updates
	wxGetApp().GetMainFrame()->canvas->Thaw();
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxUpdateUIEvent that is generated when the Tools Menu is about
///                         to be displayed
/// \remarks
/// Called from: The wxUpdateUIEvent mechanism when the associated menu item is selected, and before
/// the menu is displayed.
/// If the appropriate KB is in a ready state the "Use Automatic Capitalization" menu item on the 
/// Tools menu is enabled, otherwise it is disabled. 
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnUpdateToolsAutoCapitalization(wxUpdateUIEvent& event)
{
	if (gbIsGlossing)
	{
		if (m_bGlossingKBReady)
			event.Enable(TRUE);
		else
			event.Enable(FALSE);
	}
	else
	{
		if (m_bKBReady)
			event.Enable(TRUE);
		else
			event.Enable(FALSE);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxCommandEvent that is generated when the associated menu item is
///                         selected
/// \remarks
/// Called from: The Event Table of the CAdapt_ItApp class in response to a "Use Automatic 
/// Capitalization" command on the Tools menu.
/// Toggles the Automatic Capitalization feature of Adapt It on or off depending on the
/// initial value of the gbAutoCaps global flag. Warns the user if the project's settings
/// are not set up to utilize this feature.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnToolsAutoCapitalization(wxCommandEvent& WXUNUSED(event))
{
	// TODO: This handler won't be needed once we eliminate the Tools'
	// "Use Automatic Capitalization" menu item (possibly incorporated into an
	// upper/lower case definition pane in wizard and Edit Preferences). Some
	// of the code below should be transferred to the LowerToUpperCaseEquivalencesDlg
	// and/or LowerToUpperCaseEquivalencesPage routines.
	CMainFrame *pFrame = GetMainFrame();
	wxASSERT(pFrame != NULL);
	wxMenuBar* pMenuBar = pFrame->GetMenuBar(); 
	wxASSERT(pMenuBar != NULL);
	wxMenuItem * pToolsMenuAutoCap = pMenuBar->FindItem(ID_TOOLS_AUTO_CAPITALIZATION);
	wxASSERT(pToolsMenuAutoCap != NULL);

	// toggle the setting & update the display accordingly
	if (gbAutoCaps)
	{
		// toggle the checkmark to OFF & recalc the layout with strip-wrap off
		pToolsMenuAutoCap->Check(FALSE);
		gbAutoCaps = FALSE;
	}
	else
	{
		// don't permit auto caps to be turned on if the source language does not
		// have an upper/lower case distinction (before 2.0.6, gbSuppressAutoCapsAsk was only
		// ever true when reading a config file and the app was about to set up a tick or no
		// tick in the Use Automatic Capitalization menu item; once that was done, it was
		// cleared to false and remained that way until next launch of the app; but in 2..0.6
		// we test for non empty src and tgt upper and lower case strings being nonEmpty and
		// if so, we assume that user has things set up okay and is just wanting to turn
		// auto caps back on, so we should suppress the ask in that circumstance too) (the test
		// has to be sensitive to gbIsGlossing flag too)
		if (gbIsGlossing)
		{
			// in glossing mode
			if (!m_srcLowerCaseChars.IsEmpty() && !m_srcUpperCaseChars.IsEmpty()
				&& !m_glossLowerCaseChars.IsEmpty() && !m_glossUpperCaseChars.IsEmpty())
				gbSuppressAutoCapsAsk = TRUE;
		}
		else
		{
			// in adapting mode
			if (!m_srcLowerCaseChars.IsEmpty() && !m_srcUpperCaseChars.IsEmpty()
				&& !m_tgtLowerCaseChars.IsEmpty() && !m_tgtUpperCaseChars.IsEmpty())
				gbSuppressAutoCapsAsk = TRUE;
		}

		if (!gbSuppressAutoCapsAsk)
		{
			int intTrue = wxYES;
			if (!gbSrcHasUcAndLc)
			{
				intTrue = wxMessageBox(_("Automatic capitalization support will work only provided the source language distinguishes between upper case and lower case letters. A \"No\" response will prohibit auto-capitalization from being turned on. Does your source language have both upper and lower case letters?"),_T(""), wxYES_NO);
			}
			if (intTrue == wxNO)
			{
				gbNoSourceCaseEquivalents = TRUE;
				gbSuppressAutoCapsAsk = FALSE;
				return; // return without turning the flag on
			}
			else
			{
				gbNoSourceCaseEquivalents = FALSE;

				// if the user has not yet set up case equivalences, then open the dialog
				// for him now, so he can do it; what we do here will depend on the mode
				// - whether glossing or adapting
				if (gbIsGlossing)
				{
					if ((m_srcLowerCaseChars.IsEmpty() && m_srcUpperCaseChars.IsEmpty())
						|| (m_glossLowerCaseChars.IsEmpty() && m_glossUpperCaseChars.IsEmpty()))
					{
						// the source correspondences do not yet exist, or the gloss ones do not
						// yet exist (all must exist for auto caps to work), so until they do automatic
						// capitalization cannot be turned on. So now we give the user the chance
						// to set up what is needed

						// Remind user to set equivalences in the Case tab of Edit Preferences
						gbNoSourceCaseEquivalents = TRUE;
						gbSuppressAutoCapsAsk = FALSE;
						// IDS_NO_SRC_PUNCT_CORRESP_EXIST
						wxMessageBox(_("The source language upper and lower case correspondences do not yet exist. Until they do, automatic captialization cannot be turned on.\nYou can set them up in the Edit Preferences menu selection on the \"Case\" tab."), _T(""), wxICON_WARNING);
						return;
					}
				}
				else
				{
					if ((m_srcLowerCaseChars.IsEmpty() && m_srcUpperCaseChars.IsEmpty())
						|| (m_tgtLowerCaseChars.IsEmpty() && m_tgtUpperCaseChars.IsEmpty()))
					{
						// the source correspondences do not yet exist, or the target ones do not
						// yet exist (all must exist for auto caps to work), so until they do automatic
						// capitalization cannot be turned on. So now we give the user the chance
						// to set up what is needed

						// Remind user to set equivalences in the Case tab of Edit Preferences
						gbNoSourceCaseEquivalents = TRUE;
						gbSuppressAutoCapsAsk = FALSE;
						// IDS_NO_SRC_PUNCT_CORRESP_EXIST
						wxMessageBox(_("The source language upper and lower case correspondences do not yet exist. Until they do, automatic captialization cannot be turned on.\nYou can set them up in the Edit Preferences menu selection on the \"Case\" tab."), _T(""), wxICON_WARNING);
						return;
					}
				}
			}
		}

		// toggle the checkmark to ON, and recalc the layout with strip-wrap on
		pToolsMenuAutoCap->Check(TRUE);
		gbAutoCaps = TRUE;
		gbSuppressAutoCapsAsk = FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      configFName  -> the name of the basic config file
/// \param      folderPath   -> the work folder path m_workFolderPath
/// \remarks
/// Called from: the App's OnInit().
/// Compares the "home" directory part of the current user's path to the
/// "My Documents" folder (or the "Adapt It Work" folder for non-Win systems), with the paths
/// in the AdaptItPath, ProjectFolderPath, DocumentsFolderPath, KnowledgeBasePath, and 
/// KBBackupsPath pathname strings in the current AI-BasicConfiguration.aic file. If its a 
/// foreign config file, this first part of these paths will differ for any of the following 
/// reasons: 
/// 1. The foreign machine uses a different drive letter, 
/// 2. Or, because the user's name is different,
/// 3. Or, the foreign machine uses the Unix/Linux home directory scheme (typically /usr/home).
/// The function then fixes these paths so as to get safe defaults for the current machine, and the
/// subsequent read of the updated config file will not cause an app crash due to bad paths
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::MakeForeignBasicConfigFileSafe(wxString& configFName,wxString& folderPath)
{
	// This function is significantly different from the function of the same name in the MFC version.
	// 
	// Get the "home" directory for the current system/platform. This would typically be:
	// For Windows: C:\Documents and Settings\<UserName>
	// For Linux: /usr/home
	wxString homeDir = ::wxGetHomeDir();
	// The PathSeparator becomes the appropriate symbol; \ on Windows, / on Linux
	wxString configPath = folderPath + PathSeparator + configFName;
	wxString localPathPrefix = m_localPathPrefix; // m_localPathPrefix was set in EnsureWorkFolderPresent.
	// m_localPathPrefix represents the part of the path preceeding the m_theWorkFolder part
	// so that m_localPathPrefix + m_theWorkFolder together represents the desired folderPath
	// On any imported data, we expect the m_theWorkFolder part of config paths to be the same,
	// but the m_localPathPrefix part may vary. 

	// For small config type text files, wxTextFile is probably most convenient. A version
	// of the Open() function also understands Unicode, so we don't need to distinguish 
	// for example CStdioFile from CStdioFileEx. We just conditional compile with the
	// appropriate methods for Unicode.

	wxTextFile f;
	// Under wxWidgets wxTextFile actually reads the entire file into memory at the Open()
	// call. It is set up so we can treat it as a line oriented text file while in memory,
	// modifying it, then writing it back out to persistent storage with a single 
	// call to Write().

	// wx note: To avoid an annoying system message that pops up if Open is called on
	// a non-existing file, we'll check for its existence first, and return immediately
	// if it doesn't exist
	if (!::wxFileExists(configPath))
		return;
	// wxWidgets version we use appropriate version of Open() for ANSI or Unicode build
#ifndef _UNICODE
	// ANSI
	bool bSuccessful = f.Open(configPath); // read ANSI file into memory
#else
	// UNICODE
	bool bSuccessful = f.Open(configPath, wxConvUTF8); // read UNICODE file into memory
#endif
	if (!bSuccessful)
		// there was a problem opening the file, so we'll just return
		return;

	// The entire basic config file is now in memory and we can modify the lines we want 
	// to change. Basically we want the wxHomeDir part of the paths of concern to be the 
	// same as what wxHomeDir returns on the local machine. 

	// In the MFC version the first instance of the string "\My Documents" is located
	// and the left part of that path determined. In our wxWidgets context we want to
	// know the foreign localPathPrefix part that differs from the localPathPrefix of 
	// the local machine, which won't necessarily have the string "My Documents" in it. 

	// The five paths of concern are:
	// szAdaptitPath = _("AdaptItPath");
	// szCurLanguagesPath = _("ProjectFolderPath");
	// szCurAdaptionsPath = _("DocumentsFolderPath");
	// szCurKBPath = _("KnowledgeBasePath");
	// szCurKBBackupPath = _("KBBackupPath");
	
	wxString localPath = m_localPathPrefix + PathSeparator + m_theWorkFolder;
	wxString tab = _T('\t');
	wxString fileLine;
	// scan the in-memory file line-by-line and process those needing path updates
	// whm 29Dec06 TODO: determine if we should call convertToLocalCharset() here on fileLine
	for ( fileLine = f.GetFirstLine(); !f.Eof(); fileLine = f.GetNextLine() )
	{
		wxString foreignFilePath = _T("");	// the part of the path to the left of the m_theWorkFolder
											// excluding the sz... keyword part of fileLine
		wxString subFoldersPath = _T("");	// the part of the path to the right of m_theWorkFolder
		int nStartOfForeignFilePath;
		int nStartOfSubFoldersPath;
		int strLength;
		if(fileLine.First(szAdaptitPath) != -1)
		{
			// we're at a line with "AdaptItPath" in it that we want to check/adjust
			// Get the foreign path
			nStartOfForeignFilePath = fileLine.First(szAdaptitPath) + szAdaptitPath.Length();
			nStartOfSubFoldersPath = fileLine.Find(m_theWorkFolder) + m_theWorkFolder.Length();
			strLength = fileLine.Length();
			foreignFilePath = fileLine.Right(strLength - nStartOfForeignFilePath);
			foreignFilePath.Trim(TRUE); // Trim any white space from Right end
			foreignFilePath.Trim(FALSE); // Trim white space (expect tab) from Left end
			subFoldersPath = fileLine.Right(strLength - nStartOfSubFoldersPath);
			if (!subFoldersPath.IsEmpty() && subFoldersPath.First(PathSeparator) == -1)
			{
				// subFoldersPath doesn't have native PathSeparator, so convert it/them to native
				// We don't know whether the subFoldersPath has \ or / separators so
				// we'll take the brute force method and change both to the native one (PathSeparator)
				// The default behavior of the wxString::Replace method is to replace all.
				int NumReplacements = subFoldersPath.Replace(_T("/"),PathSeparator);
				NumReplacements = subFoldersPath.Replace(_T("\\"),PathSeparator);
			}
			// Compare the imported localPathPrefix with that of our local machine
			if (!foreignFilePath.StartsWith(m_localPathPrefix))// what about sensitivity case esp for Linux???
			{
				// foreignFilePath doesn't start with the localPathPrefix
				// so we need to modify it for the local machine's use
				fileLine = szAdaptitPath + tab + localPath + subFoldersPath; // subFoldersPath is null here
				size_t lineNum = f.GetCurrentLine();
				f.RemoveLine(lineNum);
				f.InsertLine(fileLine, lineNum);
			}
		}
		else if (fileLine.First(szCurLanguagesPath) != -1)
		{
			// we're at a line with "ProjectFolderPath" in it that we want to check/adjust
			// Compare the imported localPathPrefix with that of our local machine
			nStartOfForeignFilePath = fileLine.First(szCurLanguagesPath) + szCurLanguagesPath.Length();
			nStartOfSubFoldersPath = fileLine.Find(m_theWorkFolder) + m_theWorkFolder.Length();
			strLength = fileLine.Length();
			foreignFilePath = fileLine.Right(strLength - nStartOfForeignFilePath);
			foreignFilePath.Trim(TRUE); // Trim any white space from Right end
			foreignFilePath.Trim(FALSE); // Trim white space (expect tab) from Left end
			subFoldersPath = fileLine.Right(strLength - nStartOfSubFoldersPath);
			if (!subFoldersPath.IsEmpty() && subFoldersPath.First(PathSeparator) == -1)
			{
				// subFoldersPath doesn't have native PathSeparator, so convert it/them to native
				// We don't know whether the subFoldersPath has \ or / separators so
				// we'll take the brute force method and change both to the native one (PathSeparator)
				// The default behavior of the wxString::Replace method is to replace all.
				int NumReplacements = subFoldersPath.Replace(_T("/"),PathSeparator);
				NumReplacements = subFoldersPath.Replace(_T("\\"),PathSeparator);
			}
			// Compare the imported localPathPrefix with that of our local machine
			if (!foreignFilePath.StartsWith(m_localPathPrefix))// what about case sensitivity esp for Linux???
			{
				// foreignFilePath doesn't start with the localPathPrefix
				// so we need to modify it for the local machine's use
				fileLine = szCurLanguagesPath + tab + localPath + subFoldersPath;
				size_t lineNum = f.GetCurrentLine();
				f.RemoveLine(lineNum);
				f.InsertLine(fileLine, lineNum);
			}
		}
		else if (fileLine.First(szCurAdaptionsPath) != -1)
		{
			// we're at a line with "DocumentsFolderPath" in it that we want to check/adjust
			// Compare the imported localPathPrefix with that of our local machine
			nStartOfForeignFilePath = fileLine.First(szCurAdaptionsPath) + szCurAdaptionsPath.Length();
			nStartOfSubFoldersPath = fileLine.Find(m_theWorkFolder) + m_theWorkFolder.Length();
			strLength = fileLine.Length();
			foreignFilePath = fileLine.Right(strLength - nStartOfForeignFilePath);
			foreignFilePath.Trim(TRUE); // Trim any white space from Right end
			foreignFilePath.Trim(FALSE); // Trim white space (expect tab) from Left end
			subFoldersPath = fileLine.Right(strLength - nStartOfSubFoldersPath);
			if (!subFoldersPath.IsEmpty() && subFoldersPath.First(PathSeparator) == -1)
			{
				// subFoldersPath doesn't have native PathSeparator, so convert it/them to native
				// We don't know whether the subFoldersPath has \ or / separators so
				// we'll take the brute force method and change both to the native one (PathSeparator)
				// The default behavior of the wxString::Replace method is to replace all.
				int NumReplacements = subFoldersPath.Replace(_T("/"),PathSeparator);
				NumReplacements = subFoldersPath.Replace(_T("\\"),PathSeparator);
			}
			// Compare the imported localPathPrefix with that of our local machine
			if (!foreignFilePath.StartsWith(m_localPathPrefix))// what about case sensitivity esp for Linux???
			{
				// foreignFilePath doesn't start with the localPathPrefix
				// so we need to modify it for the local machine's use
				fileLine = szCurAdaptionsPath + tab + localPath + subFoldersPath;
				size_t lineNum = f.GetCurrentLine();
				f.RemoveLine(lineNum);
				f.InsertLine(fileLine, lineNum);
			}
		}
		else if (fileLine.First(szCurKBPath) != -1)
		{
			// we're at a line with "KnowledgeBasePath" in it that we want to check/adjust
			// Compare the imported localPathPrefix with that of our local machine
			nStartOfForeignFilePath = fileLine.First(szCurKBPath) + szCurKBPath.Length();
			nStartOfSubFoldersPath = fileLine.Find(m_theWorkFolder) + m_theWorkFolder.Length();
			strLength = fileLine.Length();
			foreignFilePath = fileLine.Right(strLength - nStartOfForeignFilePath);
			foreignFilePath.Trim(TRUE); // Trim any white space from Right end
			foreignFilePath.Trim(FALSE); // Trim white space (expect tab) from Left end
			subFoldersPath = fileLine.Right(strLength - nStartOfSubFoldersPath);
			if (!subFoldersPath.IsEmpty() && subFoldersPath.First(PathSeparator) == -1)
			{
				// subFoldersPath doesn't have native PathSeparator, so convert it/them to native
				// We don't know whether the subFoldersPath has \ or / separators so
				// we'll take the brute force method and change both to the native one (PathSeparator)
				// The default behavior of the wxString::Replace method is to replace all.
				int NumReplacements = subFoldersPath.Replace(_T("/"),PathSeparator);
				NumReplacements = subFoldersPath.Replace(_T("\\"),PathSeparator);
			}
			// Compare the imported localPathPrefix with that of our local machine
			if (!foreignFilePath.StartsWith(m_localPathPrefix))// what about case sensitivity esp for Linux???
			{
				// foreignFilePath doesn't start with the localPathPrefix
				// so we need to modify it for the local machine's use
				fileLine = szCurKBPath + tab + localPath + subFoldersPath; // add tab back
				size_t lineNum = f.GetCurrentLine();
				f.RemoveLine(lineNum);
				f.InsertLine(fileLine, lineNum);
			}
		}
		else if (fileLine.First(szCurKBBackupPath) != -1)
		{
			// we're at a line with "KBBackupPath" in it that we want to check/adjust
			// Compare the imported localPathPrefix with that of our local machine
			nStartOfForeignFilePath = fileLine.First(szCurKBBackupPath) + szCurKBBackupPath.Length();
			nStartOfSubFoldersPath = fileLine.Find(m_theWorkFolder) + m_theWorkFolder.Length();
			strLength = fileLine.Length();
			foreignFilePath = fileLine.Right(strLength - nStartOfForeignFilePath);
			foreignFilePath.Trim(TRUE); // Trim any white space from Right end
			foreignFilePath.Trim(FALSE); // Trim white space (expect tab) from Left end
			subFoldersPath = fileLine.Right(strLength - nStartOfSubFoldersPath);
			if (!subFoldersPath.IsEmpty() && subFoldersPath.First(PathSeparator) == -1)
			{
				// subFoldersPath doesn't have native PathSeparator, so convert it/them to native
				// We don't know whether the subFoldersPath has \ or / separators so
				// we'll take the brute force method and change both to the native one (PathSeparator)
				// The default behavior of the wxString::Replace method is to replace all.
				int NumReplacements = subFoldersPath.Replace(_T("/"),PathSeparator);
				NumReplacements = subFoldersPath.Replace(_T("\\"),PathSeparator);
			}
			// Compare the imported localPathPrefix with that of our local machine
			if (!foreignFilePath.StartsWith(m_localPathPrefix))// what about case sensitivity esp for Linux???
			{
				// foreignFilePath doesn't start with the localPathPrefix
				// so we need to modify it for the local machine's use
				fileLine = szCurKBBackupPath + tab + localPath + subFoldersPath; // add tab back
				size_t lineNum = f.GetCurrentLine();
				f.RemoveLine(lineNum);
				f.InsertLine(fileLine, lineNum);
			}
		}
	}

#ifndef _UNICODE
	// ANSI
	bSuccessful = f.Write(); // write ANSI file back to disk
#else
	// UNICODE
	bSuccessful = f.Write(); // write UNICODE file back to disk // whm note: default is to use wxConvUTF8 in Unicode build
#endif
	if (!bSuccessful)
	{
		// could not update the config file so inform user
		wxMessageBox(_("Unable to write updated config file, so just did nothing"));
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxCommandEvent that is generated when the associated menu item is
///                         selected
/// \remarks
/// Called from: The Event Table of the CAdapt_ItApp class in response to a "Change Folder..." 
/// command on the File menu.
/// Shows the CWhichBook dialog allowing the user to change the active book folder, but only
/// while the application is in book folder mode. Any currently opened document is first closed
/// and after changing the book folder, the Start Working Wizard is invoked automatically.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnFileChangeFolder(wxCommandEvent& event)
{
	//force closure of the document, before seeing the dialog
	CAdapt_ItDoc* pDoc = GetDocument();
	CAdapt_ItView* pView = GetView();
	CKB* pKB;
	if (gbIsGlossing)
		pKB = gpApp->m_pGlossingKB;
	else
		pKB = gpApp->m_pKB;
	if (pKB != NULL && gpApp->m_pBundle->m_nStripCount > 0)
	{
		// doc is open, so close it first
		pDoc->OnFileClose(event); // my version, which does not call OnCloseDocument
		pView->canvas->Refresh(); // force immediate repaint
	}

	// show the dialog
	CWhichBook whichBkDlg(gpApp->GetMainFrame());
	whichBkDlg.Centre();
	whichBkDlg.ShowModal();

	// force the Startup Wizard open, otherwise user might be confused about the doc remaining gone
	// and nothing else happening
	bool bSuccess = DoStartWorkingWizard(event);
	if (!bSuccess)
	{
		wxMessageBox(_("The Startup Wizard failed to open. Use the File menu's Open command to open a document."),
				_T(""), wxICON_EXCLAMATION);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxUpdateUIEvent that is generated when the File Menu is about
///                         to be displayed
/// \remarks
/// Called from: The wxUpdateUIEvent mechanism when the associated menu item is selected, and before
/// the menu is displayed.
/// If Vertical Editing is in progress the Change Folder menu item is always disabled and this
/// handler returns immediately. Otherwise, if the appropriate KB is in a ready state and 
/// Book Mode is enabled, the "Change Folder..." menu item on the File menu is enabled, 
/// otherwise it is disabled. 
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnUpdateFileChangeFolder(wxUpdateUIEvent& event)
{
	if (gbVerticalEditInProgress)
	{
		event.Enable(FALSE);
		return;
	}
	// enable if a project is active and book mode is ON
	if ((m_bKBReady || m_bGlossingKBReady) && m_bBookMode && !m_bDisableBookMode)
	{
		event.Enable(TRUE); 
	}
	else
	{
		event.Enable(FALSE);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxCommandEvent that is generated when the associated menu item is
///                         selected
/// \remarks
/// Called from: The Event Table of the CAdapt_ItApp class in response to a "Storing Documents in 
/// Book Folders" command on the Advanced menu. This handler first closes any document that is
/// currently open. The first time this command is chosen the application creates the book folders 
/// structure and notes their divisions as specified in books.xml. This handler then invokes the
/// Start Working Wizard which allows the user to select the division and specific folder to use
/// for opening and/or creating new documents.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnAdvancedBookMode(wxCommandEvent& event) 
{
	CAdapt_ItView* pView = GetView();
	CMainFrame *pFrame = wxGetApp().GetMainFrame();
	wxASSERT(pFrame != NULL);
	wxMenuBar* pMenuBar = pFrame->GetMenuBar(); 
	wxASSERT(pMenuBar != NULL);
	wxMenuItem * pAdvancedMenu = pMenuBar->FindItem(ID_ADVANCED_BOOKMODE);
	wxASSERT(pAdvancedMenu != NULL);

	// force closure of an open document, before the mode is changed - since the
	// document has to go to its current folder, and after the mode change that will
	// not be possible
	CAdapt_ItDoc* pDoc = GetDocument();
	CKB* pKB;
	if (gbIsGlossing)
		pKB = gpApp->m_pGlossingKB;
	else
		pKB = gpApp->m_pKB;
	if (pKB != NULL && gpApp->m_pBundle->m_nStripCount > 0)
	{
		// doc is open, so close it first
		pDoc->OnFileClose(event); // my version, which does not call OnCloseDocument
		pView->canvas->Refresh(); // force immediate repaint
	}

	// toggle the setting
	if (m_bBookMode)
	{
		// toggle the checkmark to OFF
		pAdvancedMenu->Check(FALSE);
		m_bBookMode = FALSE;
		m_nLastBookIndex = m_nBookIndex; // store last used index, in case user restarts book mode
		m_nBookIndex = -1;
		m_pCurrBookNamePair = NULL;
		m_bibleBooksFolderPath.Empty();
	}
	else
	{
		// toggle the checkmark to ON
		pAdvancedMenu->Check(TRUE);

		// restore the settings, set the name pair structure, and redefine the book folder's path
		m_bBookMode = TRUE;
		if (m_nLastBookIndex < 0)
		{
			// mode not set yet in this run of the app, so use the default
			m_nBookIndex = m_nDefaultBookIndex;
		}
		else
		{
			// the mode was in effect earlier in this run, so reset some book as was last used at that time
			m_nBookIndex = m_nLastBookIndex;
		}
		m_pCurrBookNamePair = ((BookNamePair*)(*m_pBibleBooks)[m_nBookIndex]);
		m_bibleBooksFolderPath = m_curAdaptionsPath + PathSeparator + m_pCurrBookNamePair->dirName;

		// check the book folders are already present, and if not then create them
		// whm note: AreBookFoldersCreated() has the side effect of changing the current work directory
		// to the passed in m_curAdaptionsPath.
		bool bFoldersPresent = AreBookFoldersCreated(m_curAdaptionsPath);
		if (!bFoldersPresent)
		{
			CreateBookFolders(m_curAdaptionsPath,m_pBibleBooks);
		}
	}

	// restore focus to the targetBox, if it is visible
	if (m_pTargetBox != NULL)
	{
		if (m_pTargetBox->IsShown())
			m_pTargetBox->SetFocus();
		// also get the status bar updated, if there is a document visible
		if (gpApp->m_nActiveSequNum != -1 && gpApp->m_pActivePile != NULL)
			gpApp->RefreshStatusBarInfo();
	}

	// force the Startup Wizard open, otherwise user might be confused about the doc remaining gone
	// and nothing else happening
	bool bSuccess = DoStartWorkingWizard(event);
	if (!bSuccess)
	{
		wxMessageBox(_("The Startup Wizard failed to open. Use the File menu's Open command to open a document."),
				_T(""), wxICON_EXCLAMATION);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      event   -> the wxUpdateUIEvent that is generated when the Advanced Menu is about
///                         to be displayed
/// \remarks
/// Called from: The wxUpdateUIEvent mechanism when the associated menu item is selected, and before
/// the menu is displayed.
/// If Vertical Editing is in progress the Book Mode menu item is disabled and this handler returns
/// immediately. Otherwise, if the appropriate KB is in a ready state and Book Mode is enabled, the 
/// "Storing Documents in Book Folders" menu item on the Advanced menu is enabled, otherwise it is 
/// disabled. This handler also insures that the toggle state is ticked if On and unticked if Off.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::OnUpdateAdvancedBookMode(wxUpdateUIEvent& event) 
{
	if (gbVerticalEditInProgress)
	{
		event.Enable(FALSE);
		return;
	}
	// BEW changed 27Nov05 to get the tick shown or not when the menu is opened
	CMainFrame *pFrame = wxGetApp().GetMainFrame();
	wxASSERT(pFrame != NULL);
	wxMenuBar* pMenuBar = pFrame->GetMenuBar(); 
	wxASSERT(pMenuBar != NULL);
	wxMenuItem * pAdvancedMenu = pMenuBar->FindItem(ID_ADVANCED_BOOKMODE);
	wxASSERT(pAdvancedMenu != NULL);

	if (m_bBookMode && !m_bDisableBookMode)
	{
		pAdvancedMenu->Check(TRUE);
	}
	else
	{
		pAdvancedMenu->Check(FALSE);
	}

	// enable if a project is active & the disable flag is not set
	if ((m_bKBReady || m_bGlossingKBReady) && !m_bDisableBookMode)
	{
		event.Enable(TRUE); 
	}
	else
	{
		event.Enable(FALSE);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if s contains an ordinary double quote " or a single quote ' mark, FALSE otherwise
/// \param      s   -> the wxString being examined
/// \param      ch  -> the quote character for comparison, either \" or \'
/// \remarks
/// Called from: CPunctCorrespPageCommon::UpdateAppValues().
/// Determines if the string s contains an ordinary double " or single ' quote.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::ContainsOrdinaryQuote(wxString s, wxChar ch)
{
	// BEW added 5May05 to handle determining whether or not and ordinary single quote
	// (ie. apostrophe  ASCII 39) or double-quote (ASCII 34) is in the passed in string s.
	// Can't use CString's .Find() function because it returns a positive result if the
	// string has curly quotes - and for punctuation in Adapt It that will always be the
	// case. So I do it by explicitly checking for value 39 or 34 as the case may be; it
	// will of course work for other passed in ch values, but I intend to use it for only
	// " or '
	int valueCh = (int)ch;
	int len = s.Length();
	// wx version note: Since we require a read-only buffer we use GetData which just returns
	// a const wxChar* to the data in the string.
	const wxChar* pBuff = s.GetData();
	wxChar* pEnd = (wxChar*)pBuff + len; // 
	wxASSERT(*pEnd == _T('\0')); // ensure there is a null there; this must be done for all wxStringBuffer calls!!!
	wxChar* ptr = (wxChar*)pBuff;
	while (ptr < pEnd)
	{
		int itsValue = (int)*ptr;
		if (itsValue == valueCh)
		{
			return TRUE;
		}
		else
			ptr++;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: the App's OnInit(), DoUsfmSetChanges(), the Doc's ResetUSFMFilterStructs()
/// and ResetUSFMFilterStructs().
/// Takes data from the sfm maps, and constructs some raipd access strings as a convenience for
/// string Find operations. Strings are set up for wrap markers, section head markers, inline
/// markers, filter markers and unknown markers.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::SetupMarkerStrings()
{
	// At this point all the SFM maps are ready, so we can now set up some rapid access 
	// data strings, for such things as wrap markers (and perhaps others as the need arises)
	// Note: We can easily set up other rapid access strings here if needed for any other
	// attributes in USFMAnalysis.
	// Ammended to also setup the m_currentUnknownMarkersStr for saving in the Doc's Buffer
	// member. Assumes the m_filterFlagsUnkMkrs is current reflecting the current filter
	// status of the unknown markers in the m_unknownMarkers CStringArray. This would normally
	// be the case when SetupMarkerStrings is called from within ResetUSFMFilterStructs.
	// When SetupMarkerStrings is called from elsewhere, it is up to the caller to ensure
	// that m_filterFlagsUnkMkrs is up to date.

	CAdapt_ItDoc* pDoc = GetDocument();
	// set up marker strings for each type of map.
	UsfmWrapMarkersStr.Empty();
	PngWrapMarkersStr.Empty();
	UsfmAndPngWrapMarkersStr.Empty();

	UsfmSectionHeadMarkersStr.Empty();
	PngSectionHeadMarkersStr.Empty();
	UsfmAndPngSectionHeadMarkersStr.Empty();

	UsfmInLineMarkersStr.Empty();
	PngInLineMarkersStr.Empty();
	UsfmAndPngInLineMarkersStr.Empty();

	UsfmFilterMarkersStr.Empty();
	PngFilterMarkersStr.Empty();
	UsfmAndPngFilterMarkersStr.Empty();

	gCurrentFilterMarkers.Empty();
	m_currentUnknownMarkersStr.Empty();

	USFMAnalysis* pSfm;
	wxString key;
	MapSfmToUSFMAnalysisStruct::iterator iter;

	for (iter = m_pUsfmStylesMap->begin(); iter != m_pUsfmStylesMap->end(); ++iter)
	{
		// Retrieve each USFMAnalysis struct from the map
		key = iter->first;
		pSfm = iter->second;
		if (pSfm->wrap)
		{
			UsfmWrapMarkersStr += gSFescapechar;
			UsfmWrapMarkersStr += key;
			UsfmWrapMarkersStr += _T(' ');
		}
		if (pSfm->textType == sectionHead)
		{
			UsfmSectionHeadMarkersStr += gSFescapechar;
			UsfmSectionHeadMarkersStr += key;
			UsfmSectionHeadMarkersStr += _T(' ');
		}
		if (pSfm->inLine)
		{
			UsfmInLineMarkersStr += gSFescapechar;
			UsfmInLineMarkersStr += key;
			UsfmInLineMarkersStr += _T(' ');
		}
		if (pSfm->filter)
		{
			UsfmFilterMarkersStr += gSFescapechar;
			UsfmFilterMarkersStr += key;
			UsfmFilterMarkersStr += _T(' ');
		}
	}
	for (iter = m_pPngStylesMap->begin(); iter != m_pPngStylesMap->end(); ++iter)
	{
		// Retrieve each USFMAnalysis struct from the map
		key = iter->first;
		pSfm = iter->second;
		if (pSfm->wrap)
		{
			PngWrapMarkersStr += gSFescapechar;
			PngWrapMarkersStr += key;
			PngWrapMarkersStr += _T(' ');
		}
		if (pSfm->textType == sectionHead)
		{
			PngSectionHeadMarkersStr += gSFescapechar;
			PngSectionHeadMarkersStr += key;
			PngSectionHeadMarkersStr += _T(' ');
		}
		if (pSfm->inLine)
		{
			PngInLineMarkersStr += gSFescapechar;
			PngInLineMarkersStr += key;
			PngInLineMarkersStr += _T(' ');
		}
		if (pSfm->filter)
		{
			PngFilterMarkersStr += gSFescapechar;
			PngFilterMarkersStr += key;
			PngFilterMarkersStr += _T(' ');
		}
	}
	for (iter = m_pUsfmAndPngStylesMap->begin(); iter != m_pUsfmAndPngStylesMap->end(); ++iter)
	{
		// Retrieve each USFMAnalysis struct from the map
		key = iter->first;
		pSfm = iter->second;
		if (pSfm->wrap)
		{
			UsfmAndPngWrapMarkersStr += gSFescapechar;
			UsfmAndPngWrapMarkersStr += key;
			UsfmAndPngWrapMarkersStr += _T(' ');
		}
		if (pSfm->textType == sectionHead)
		{
			UsfmAndPngSectionHeadMarkersStr += gSFescapechar;
			UsfmAndPngSectionHeadMarkersStr += key;
			UsfmAndPngSectionHeadMarkersStr += _T(' ');
		}
		if (pSfm->inLine)
		{
			UsfmAndPngInLineMarkersStr += gSFescapechar;
			UsfmAndPngInLineMarkersStr += key;
			UsfmAndPngInLineMarkersStr += _T(' ');
		}
		if (pSfm->filter)
		{
			UsfmAndPngFilterMarkersStr += gSFescapechar;
			UsfmAndPngFilterMarkersStr += key;
			UsfmAndPngFilterMarkersStr += _T(' ');
		}
	}

	switch(gCurrentSfmSet) // gCurrentSfmSet is UsfmOnly when SetupMarkerStrings is called from InitInstance
	{
	case UsfmOnly: gCurrentFilterMarkers = UsfmFilterMarkersStr; break;
	case PngOnly: gCurrentFilterMarkers = PngFilterMarkersStr; break;
	case UsfmAndPng: gCurrentFilterMarkers = UsfmAndPngFilterMarkersStr; break;
	default: gCurrentFilterMarkers = UsfmFilterMarkersStr;
	}

#ifdef _Trace_UnknownMarkers
	wxString filteredUnkMkrsAddedTogCurrentFilterMarkers;
	filteredUnkMkrsAddedTogCurrentFilterMarkers.Empty();
	TRACE0("In SetupMarkerStrings BEFORE Doc's unknown markers copied from pFilterPageCommon\n");
	TRACE1("   Doc's unknown markers = %s\n", pDoc->GetUnknownMarkerStrFromArrays(&m_unknownMarkers, &m_filterFlagsUnkMkrs));
#endif

	// Populate the Doc's unknown markers string pDoc->m_currentUnknownMarkersStr
	// from the doc's unknown marker data arrays. 
	// Add any filtered unknown markers to the App's filter markers string gCurrentFilterMarkers
	// if the unknown marker is not already present.
	wxASSERT(m_unknownMarkers.GetCount() == m_filterFlagsUnkMkrs.GetCount());
	int unkCount = m_unknownMarkers.GetCount();
	int index;
	int dummyPos; // unused here
	wxString bareMkr;
    for (index = 0; index < unkCount; index++)
    {
        bareMkr = m_unknownMarkers.Item(index);
        // if the unknown marker doesn't already exist in m_currentUnknownMarkersStr add it
        if (!pDoc->MarkerExistsInString(m_currentUnknownMarkersStr,bareMkr,dummyPos))
        {
            m_currentUnknownMarkersStr += bareMkr;
            int filterFlag = m_filterFlagsUnkMkrs.Item(index);
            if (filterFlag == 0)
            {
                // the unknown marker is not filtered
			// if it doesn't already exist in m_current
                m_currentUnknownMarkersStr += _T("=0 ");
            }
            else
            {
                // the unknown marker is filtered
                m_currentUnknownMarkersStr += _T("=1 ");
                // also add this marker to gCurrentFilterMarkers if it doesn't already exist
                if (!pDoc->MarkerExistsInString(gCurrentFilterMarkers,bareMkr,dummyPos))
                {
                    gCurrentFilterMarkers += bareMkr;
                    gCurrentFilterMarkers += _T(' '); // add the space for efficient parsing later
#ifdef _Trace_UnknownMarkers
					filteredUnkMkrsAddedTogCurrentFilterMarkers += bareMkr;
					filteredUnkMkrsAddedTogCurrentFilterMarkers += _T(' ');
#endif
                }
            }
        }
    }

#ifdef _Trace_UnknownMarkers
	TRACE0("In SetupMarkerStrings AFTER Doc's unknown markers copied from pFilterPageCommon\n");
	TRACE1("   Doc's unknown markers = %s\n", pDoc->GetUnknownMarkerStrFromArrays(&m_unknownMarkers, &m_filterFlagsUnkMkrs));
	TRACE1("   Unk mkrs added to glbl= %s\n", filteredUnkMkrsAddedTogCurrentFilterMarkers);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      markerStr     <- the string from which any instance of wholeMarker is removed
/// \param      wholeMarker   -> the whole marker of interest
/// \remarks
/// Called from: (currently unused)
/// Removes any instance of wholeMarker found in markerStr.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::RemoveMarkerFromString(wxString& markerStr, wxString wholeMarker)
{
	// If the wholeMarker exists in markerStr, RemoveMarkerFromString remove it from markerStr.
	// Assumes markerStr consists of a string of markers, each beginning with a backslash
	// and terminating with a delimiting space
	// Also assumes wholeMarker begins with backslash, and insures it ends with a delimiting 
	// space.
	wholeMarker.Trim(FALSE); // trim left end
	wholeMarker.Trim(TRUE); // trim right end
	wxASSERT(!wholeMarker.IsEmpty());
	// then add the necessary final space
	wholeMarker += _T(' ');
	int posn = markerStr.Find(wholeMarker);
	if (posn != -1)
	{
		// The wholeMarker does exist in the string, so remove it.
		markerStr.Remove(posn, wholeMarker.Length());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pDC                -> pointer to the display context
/// \param      MkrAndDescrArray   <- a wxStringArray containing strings of markers and their descriptions
/// \param      minNumSpaces       -> the minimum number of spaces required to be maintained 
///                                     between the whole marker and the following descriptive text
/// \remarks
/// Called from: the View's GetMarkerInventoryFromCurrentDoc(), 
/// from CFilterPageCommon::LoadDocSFMListBox(), CFilterPageCommon::LoadProjSFMListBox(),
/// CFilterPageCommon::LoadFactorySFMListBox(), from CUSFMPageCommon::DoInit(),
/// CUSFMPageCommon::DoBnClickedRadioUseUbsSetOnlyDoc(), 
/// CUSFMPageCommon::DoBnClickedRadioUseSilpngSetOnlyDoc(),
/// CUSFMPageCommon::DoBnClickedRadioUseBothSetsDoc(), 
/// CUSFMPageCommon::DoBnClickedRadioUseUbsSetOnlyProj(),
/// CUSFMPageCommon::DoBnClickedRadioUseSilpngSetOnlyProj(),
/// CUSFMPageCommon::DoBnClickedRadioUseBothSetsProj().
/// This function scans the array to find the whole marker that has the greatest text extent 
/// in the pDC. It then scans the array again and uses that text extent determined in the 
/// first scan to calculate and insert the amount of white space needed between the whole 
/// marker and the following descriptive text in order to make them line up relatively 
/// straight within the containing list box.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::FormatMarkerAndDescriptionsStringArray(wxClientDC* pDC, wxArrayString* MkrAndDescrArray, int minNumSpaces)
{
	// Takes an input CStringArray which is assumed to be composed of strings which are roughly 
	// formatted as white space, followed by a whole marker (no embedded spaces) followed by some 
	// white space, followed by some descriptive text. This function scans the array to find the 
	// whole marker that has the greatest text extent in the pDC. It then scans the array again 
	// and uses that text extent determined in the first scan to calculate and insert the amount 
	// of white space needed between the whole marker and the following descriptive text in order 
	// to make them line up relatively straight within the containing list box. They will not be 
	// perfectly lined up because we cannot add fractional spaces to the string in order to get 
	// a perfectly justified text line up. The minNumSpaces parameter indicates the minimum number
	// of spaces required to be maintained between the whole marker and the following 
	// descriptive text. The string array is repopulated with the same strings but with the 
	// amount of white space between whole marker and following text adjusted so that they
	// will line up.
	// Note: Most strings added to a CCheckListBox style list box look better if they have at
	// least one space prefixed to the string. In addition to this space the caller may have 
	// prefixed an extra space to force the list box to sort certain of its strings at the 
	// beginning of the list box. Because of this we preserve any initial white space found on
	// the array strings and only adjust the medial space in our formatting process.
	// wx version note: The wx version does not have a "Show All" button in the filter page
	// dialog, therefore I've modified the code within the filterpage to eliminate from the
	// wxArrayString items that won't be shown in the listbox before the wxArrayString is
	// input into this FormatMarkerAndDescriptionsStringArray function. Hence, here in this
	// function we only calculate the extents of markers that do not display within list 
	// boxes. 
	int arrayCt = MkrAndDescrArray->GetCount();
	if (arrayCt == 0)
	{
		// nothing to do so
		return;
	}

	wxString tempStr, checkStr;
	wxString initialWhiteSp;
	initialWhiteSp.Empty();
	wxString descrStr;
	descrStr.Empty();
	wxString wholeMkr;
	wholeMkr.Empty();
	wxString formattedStr;
	formattedStr.Empty();
	wxSize longestSfmExtent(0,0);
	wxSize spaceExtent(0,0);
	wxSize testExtent(0,0);
	if (pDC != NULL)
	{
		pDC->GetTextExtent(_T(' '),&spaceExtent.x, &spaceExtent.y);
	}
	else
	{
		// use the sizeSpace determined in InitInstance
		spaceExtent = sizeSpace;
	}

	// first scan the input wxStringArray strings and determine the extent of the marker having the 
	// greatest text extent
	for (int ct = 0; ct < arrayCt; ct++)
	{
		tempStr = MkrAndDescrArray->Item(ct);
		int spCt = 0;
		int nwspCt = 0;
		int strLen = tempStr.Length();
		// count white space
		while(spCt < strLen && tempStr.GetChar(spCt) == _T(' '))
		{
			spCt++;
		}
		// count non-white space (the whole marker)
		while(spCt + nwspCt < strLen && tempStr.GetChar(spCt + nwspCt) != _T(' '))
		{
			nwspCt++;
		}
		// checkStr is the array string up to but not including the first "medial space"
		checkStr = tempStr.Mid(0, spCt + nwspCt);

		if (pDC != NULL)
		{
			// measure the text extent of this marker (including preceeding white space)
			 pDC->GetTextExtent(checkStr,&testExtent.x, &testExtent.y);
			if (testExtent.x > longestSfmExtent.x)
			{
				longestSfmExtent = testExtent;
			}
		}
		else
		{
			// use the sizeSpace determined in InitInstance for longestSfmExtent
			longestSfmExtent = sizeSpace;
		}
	}

	// now scan the string array again, this time reformatting the strings to add the correct
	// amount of white space between the whole marker and following descriptive text so they
	// will line up
	for (int ct = 0; ct < arrayCt; ct++)
	{
		initialWhiteSp.Empty();
		descrStr.Empty();
		wholeMkr.Empty();
		formattedStr.Empty();

		tempStr = MkrAndDescrArray->Item(ct);
		int spCt = 0;
		int nwspCt = 0;
		int strLen = tempStr.Length();
		// count white space
		while(spCt < strLen && tempStr.GetChar(spCt) == _T(' '))
		{
			spCt++;
			initialWhiteSp += _T(' ');
		}
		// remove prefixed white space
		tempStr = tempStr.Mid(spCt);
		// count non-white space (the whole marker)
		while(nwspCt < strLen && tempStr.GetChar(nwspCt) != _T(' '))
		{
			wholeMkr += tempStr.GetChar(nwspCt);
			nwspCt++;
		}
		// checkStr is the array string up to but not including the first "medial space"
		checkStr = tempStr.Mid(0, nwspCt);


		if (pDC != NULL)
		{
			// measure the text extent of this marker (including preceeding white space)
			 pDC->GetTextExtent(checkStr, &testExtent.x, &testExtent.y);
			if (testExtent.x > longestSfmExtent.x)
			{
				longestSfmExtent = testExtent;
			}
		}
		else
		{
			// use the sizeSpace determined in InitInstance for longestSfmExtent
			longestSfmExtent = sizeSpace;
		}

		descrStr += tempStr.Mid(nwspCt); // get remainder of string for descrStr
		descrStr.Trim(TRUE); // trim right end
		descrStr.Trim(FALSE); // trim left end
		// build the formatted array string
		formattedStr = initialWhiteSp;
		formattedStr += wholeMkr;
		// add enough spaces to make the text extent of formattedStr equal to or greater 
		// than longestSfmExtent.cx

		if (pDC != NULL)
		{
			int extSpacex, extSpacey;
			int fStrExtx, fStrExty;
			pDC->GetTextExtent(_T(' '),&extSpacex,&extSpacey);
			pDC->GetTextExtent(formattedStr,&fStrExtx,&fStrExty);
			while (fStrExtx < longestSfmExtent.x)
			{
				formattedStr += _T(' ');
				fStrExtx += extSpacex;
			}
		}
		// now add the minimum number of spaces of separation - the array string gets
		// this much separation between whole marker and its description even when pDC == NULL
		for (int minSp = 0; minSp < minNumSpaces; minSp++)
		{
			formattedStr += _T(' ');
		}
		// add back the description
		formattedStr += descrStr;
		// set the newly formatted string back into the array
		(*MkrAndDescrArray)[ct] = formattedStr;
	}
}

// MFC: BEW removed 3Apr08 (moved to a checkbox in CAutoSavingPage of Preferences dialog)
// The MFC version moves the function to a checkbox in CAutoSavingPage of Preferences dialog.
// The wx version never used the function at all.
//void CAdapt_ItApp::OnFileSaveAsXml(wxCommandEvent& WXUNUSED(event))
//{
//	// wx version: In the wxWidgets version we only handle documents and kbs in xml form
//	// therefore, we do not allow the user to turn this setting off. Since some users may
//	// have migrated to the wx version thinking they could use it to read the legacy
//	// serialized format, we'll give them a message of explanation.
//	wxMessageBox(_("Sorry, this version of Adapt It only handles Adapt It documents\nand knowledge bases in xml format. If you have .adt or .KB\nfiles created by a previous version of Adapt It, you will need\nto convert those files to .xml format by saving them using\nVersion 3.x of Adapt It, or an Adapt It file conversion tool."),_T(""),wxICON_INFORMATION);
//
//	CAdapt_ItView* pView = GetView();
//	wxASSERT(pView);
//
//	CMainFrame* pFrame = (CMainFrame*)pView->GetFrame();
//	wxMenuBar* pMenuBar = pFrame->GetMenuBar();
//	wxASSERT(pMenuBar != NULL);
//	wxMenuItem * pFileSaveAsXml = pMenuBar->FindItem(ID_FILE_SAVE_AS_XML);
//	wxASSERT(pFileSaveAsXml != NULL);
//
//	pFileSaveAsXml->Check(TRUE);
//	m_bSaveAsXML = TRUE;
//
//	// restore focus to the targetBox, if it is visible and we are not in free translation mode,
//	// but if in free translation mode then restore it to the edit box in the Compose Bar
//	if (m_bFreeTranslationMode)
//	{
//		CMainFrame* pFrame = GetMainFrame();
//		if (pFrame->m_pComposeBar->GetHandle() != NULL)
//			if (pFrame->m_pComposeBar->IsShown())
//			{
//				pFrame->m_pComposeBarEditBox->SetFocus();
//			}
//	}
//	else
//	{
//		if (m_pTargetBox->GetHandle() != NULL)
//			if (m_pTargetBox->IsShown())
//				m_pTargetBox->SetFocus();
//	}
//}

//void CAdapt_ItApp::OnUpdateFileSaveAsXml(wxUpdateUIEvent& event)
//{
//	event.Enable(TRUE); // in wx version we always and only save docs and kbs as xml
//	
//	// whm added below to insure Save As XML menu item is always checked and m_bSaveAsXML is TRUE
//	CAdapt_ItView* pView = GetView();
//	CMainFrame* pFrame = (CMainFrame*)pView->GetFrame();
//	wxMenuBar* pMenuBar = pFrame->GetMenuBar();
//	wxASSERT(pMenuBar != NULL);
//	wxMenuItem * pFileSaveAsXml = pMenuBar->FindItem(ID_FILE_SAVE_AS_XML);
//	wxASSERT(pFileSaveAsXml != NULL);
//	pFileSaveAsXml->Check(TRUE);
//	m_bSaveAsXML = TRUE;
//
//}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a CBString (byte string) containing the composed KB element (each element handles one
///             CTargetUnit and its associated key string (the source text word or phrase, minus punctuation)
/// \param      src       -> reference to the source text word or phrase
/// \param      pTU       -> pointer to the associated CTargetUnit instance
/// \param      nTabLevel -> how many tabs are to be put at the start of each line for indenting
///                             purposes (2)
/// \remarks
/// Called from: the App's DoKBSaveAsXML().
/// Constructs a byte string containing a composed KB element in XML format in which one 
/// CTargetUnit and its associated key string is represented (the source text word or phrase, 
/// minus punctuation). This function is called once for each target unit in the KB's map.
////////////////////////////////////////////////////////////////////////////////////////////
CBString CAdapt_ItApp::MakeKBElementXML(wxString& src,CTargetUnit* pTU,int nTabLevel)
{
	// Constructs
	// <TU f="boolean" k="source text key (a word or phrase)">
	// 	<RS n="reference count" a="adaptation (or gloss) word or phrase"/>
	// 	... possibly more RS elements (for other adaptations of the same source)
	// </TU>
	// The RS element handles the contents of the CRefString instances stored on the pTU instance
	CBString aStr;
	CRefString* pRefStr;
	int i;
	wxString intStr;
	// wx note: the wx version in Unicode build refuses assign a CBString to char numStr[24]
	// so I'll declare numStr as a CBString also
	CBString numStr; //char numStr[24];

#ifndef _UNICODE  // ANSI (regular) version

	// start the targetUnit element
	aStr.Empty();
	for (i = 0; i < nTabLevel; i++)
	{
		aStr += "\t"; // tab the start of the line
	}
	aStr += "<TU f=\"";
	intStr.Empty(); // needs to start empty, otherwise << will append the string value of the int
	intStr << (int)pTU->m_bAlwaysAsk;
	numStr = intStr;
	aStr += numStr;
	aStr += "\" k=\"";
	CBString s(src);
	InsertEntities(s);
	aStr += s;
	aStr += "\">\r\n";

	TranslationsList::Node* pos = pTU->m_pTranslations->GetFirst();
	while (pos != NULL)
	{
		// there will always be at least one pRefStr in a pTU
		pRefStr = (CRefString*)pos->GetData();
		pos = pos->GetNext();
		CBString bstr(pRefStr->m_translation);
		InsertEntities(bstr);
		intStr.Empty(); // needs to start empty, otherwise << will append the string value of the int
		intStr << pRefStr->m_refCount;
		numStr = intStr;

		// construct the tabs
		int j;
		int last = nTabLevel + 1;
		for (j = 0; j < last ; j++)
		{
			aStr += "\t"; // tab the start of the line
		}
		// construct the element
		aStr += "<RS n=\"";
		aStr += numStr;
		aStr += "\" a=\"";
		aStr += bstr;
		aStr += "\"/>\r\n";
	}

	// construct the closing TU tab
	for (i = 0; i < nTabLevel; i++)
	{
		aStr += "\t"; // tab the start of the line
	}
	aStr += "</TU>\r\n";

#else  // Unicode version

	// start the targetUnit element
	aStr.Empty();
	for (i = 0; i < nTabLevel; i++)
	{
		aStr += "\t"; // tab the start of the line
	}
	aStr += "<TU f=\"";
	intStr.Empty(); // needs to start empty, otherwise << will append the string value of the int
	intStr << (int)pTU->m_bAlwaysAsk;
	numStr = gpApp->Convert16to8(intStr);
	aStr += numStr;
	aStr += "\" k=\"";
	CBString s = Convert16to8(src);
	InsertEntities(s);
	aStr += s;
	aStr += "\">\r\n";

	TranslationsList::Node* pos = pTU->m_pTranslations->GetFirst();
	while (pos != NULL)
	{
		// there will always be at least one pRefStr in a pTU
		pRefStr = (CRefString*)pos->GetData();
		pos = pos->GetNext();
		CBString bstr = Convert16to8(pRefStr->m_translation);
		InsertEntities(bstr);
		intStr.Empty(); // needs to start empty, otherwise << will append the string value of the int
		intStr << pRefStr->m_refCount;
		numStr = gpApp->Convert16to8(intStr);

		// construct the tabs
		int j;
		int last = nTabLevel + 1;
		for (j = 0; j < last ; j++)
		{
			aStr += "\t"; // tab the start of the line
		}
		// construct the element
		aStr += "<RS n=\"";
		aStr += numStr;
		aStr += "\" a=\"";
		aStr += bstr;
		aStr += "\"/>\r\n";
	}

	// construct the closing TU tab
	for (i = 0; i < nTabLevel; i++)
	{
		aStr += "\t"; // tab the start of the line
	}
	aStr += "</TU>\r\n";

#endif // end of Unicode version
	return aStr;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxFontEncoding value that is the closest equivalent to the MFC Charset
/// \param      Charset   -> the MFC Charset value
/// \remarks
/// Called from: the App's GetFontConfiguration().
/// Maps an MFC Charset value to its closest equivalent wxFontEncoding value in wxWidgets.
/// See the complementing function MapWXFontEncodingToMFCCharset().
////////////////////////////////////////////////////////////////////////////////////////////
wxFontEncoding CAdapt_ItApp::MapMFCCharsetToWXFontEncoding(const int Charset)
{
	switch(Charset)
	{
		// Note: MFC's SYMBOL_CHARSET, JOHAB_CHARSET, and VIETNAMESE_CHARSET do not seem to have a wx
		// equivalent to map to, so I'm mapping SYMBOL_CHARSET to wxFONTENCODING_DEFAULT; and the other
		// two to wxFONTENCODING_DEFAULT.
	case 0: return wxFONTENCODING_CP1252; // ANSI_CHARSET maps to wxFONTENCODING_CP1252 [Microsoft analogue of ISO8859-1 "WinLatin1"]
	//case 1: return wxFONTENCODING_DEFAULT; // DEFAULT_CHARSET maps to wxFONTENCODING_DEFAULT
	//case 2: return wxFONTENCODING_SYSTEM; // SYMBOL_CHARSET maps to [???] wxFONTENCODING_SYSTEM
	case 1: return wxFONTENCODING_ISO8859_1; // Western European (ISO) whm Note: Bob Eaton's GetEncodingStringForXmlFiles() function maps CP_SYMBOL to "iso-8859-1"
	case 2: return wxFONTENCODING_ISO8859_2; // Central European (ISO) TODO: verify
	case 3: return wxFONTENCODING_ISO8859_3; // Latin 3 (ISO) TODO: verify
	case 4: return wxFONTENCODING_ISO8859_4; // Baltic (ISO) TODO: verify
	case 5: return wxFONTENCODING_ISO8859_5; // Cyrillic (ISO) TODO: verify
	case 6: return wxFONTENCODING_ISO8859_6; // Arabic (ISO) TODO: verify
	case 7: return wxFONTENCODING_ISO8859_7; // Greek (ISO) TODO: verify
	case 8: return wxFONTENCODING_ISO8859_8; // Hebrew (ISO-Visual) TODO: verify
	case 9: return wxFONTENCODING_ISO8859_9; // Turkish (ISO) TODO: verify
	case 10: return wxFONTENCODING_ISO8859_10; // TODO: verify
	case 11: return wxFONTENCODING_ISO8859_11; // TODO: verify
	case 12: return wxFONTENCODING_ISO8859_12; // TODO: verify
	case 13: return wxFONTENCODING_ISO8859_13; // Estonian (ISO) TODO: verify
	case 14: return wxFONTENCODING_ISO8859_14; // TODO: verify
	case 15: return wxFONTENCODING_ISO8859_15; // Latin 9 (ISO) TODO: verify
	case 77: return wxFONTENCODING_MACROMAN; // MAC_CHARSET maps to wxFONTENCODING_MACROMAN [mac has encodings 47-86 under wx, default to Roman]
	case 128: return wxFONTENCODING_CP932; // SHIFTJIS_CHARSET maps to wxFONTENCODING_CP932 [shift-JIS]
	case 129: return wxFONTENCODING_CP949; // HANGUL_CHARSET maps to wxFONTENCODING_CP949 [Korean (Hangul charset)]
	case 130: return wxFONTENCODING_DEFAULT; // JOHAB_CHARSET maps to [???] wxFONTENCODING_DEFAULT
	case 134: return wxFONTENCODING_CP936; // GB2312_CHARSET maps to wxFONTENCODING_CP936 [Chinese simplified (GB)]
	case 136: return wxFONTENCODING_CP950; // CHINESEBIG5_CHARSET maps to wxFONTENCODING_CP950 [Chinese (traditional - Big5)]
	case 161: return wxFONTENCODING_CP1253; // GREEK_CHARSET maps to wxFONTENCODING_CP1253 [WinGreek (8859-7)]
	case 162: return wxFONTENCODING_CP1254; // TURKISH_CHARSET maps to wxFONTENCODING_CP1254 [WinTurkish]
	case 163: return wxFONTENCODING_DEFAULT; // VIETNAMESE_CHARSET maps to [???] wxFONTENCODING_DEFAULT
	case 177: return wxFONTENCODING_CP1255; // HEBREW_CHARSET maps to wxFONTENCODING_CP1255 [WinHebrew]
	case 178: return wxFONTENCODING_CP1256; // ARABIC_CHARSET maps to wxFONTENCODING_CP1256 [WinArabic]
	case 186: return wxFONTENCODING_CP1257; // BALTIC_CHARSET maps to wxFONTENCODING_CP1257 [WinBaltic (same as Latin 7)]
	case 204: return wxFONTENCODING_CP1251; // RUSSIAN_CHARSET maps to wxFONTENCODING_CP1251 [Microsoft analogue of ISO8859-5 "WinCyrillic"]
	case 222: return wxFONTENCODING_CP874; // THAI_CHARSET maps to wxFONTENCODING_CP874 [WinThai]
	case 238: return wxFONTENCODING_CP1250; // EASTEUROPE_CHARSET maps to wxFONTENCODING_CP1250 [Microsoft analogue of ISO8859-2 "WinLatin2"]
	case 255: return wxFONTENCODING_SYSTEM; // OEM_CHARSET maps to wxFONTENCODING_SYSTEM
	default: return wxFONTENCODING_SYSTEM; // Use the default encoding of the underlying operating system
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     an int representing the MFC Charset value that fontEnc maps to from wxWidgets
/// \param      fontEnc   -> the wxFontEncoding value to map from
/// \remarks
/// Called from: the App's UpdateFontInfoStruct().
/// Maps a wxWidgets wxFontEncoding value to its closest MFC Charset value.
/// See the complementing function MapMFCCharsetToWXFontEncoding().
////////////////////////////////////////////////////////////////////////////////////////////
int CAdapt_ItApp::MapWXFontEncodingToMFCCharset(const wxFontEncoding fontEnc)
{
	switch(fontEnc)
	{
		// Note: MFC's SYMBOL_CHARSET, JOHAB_CHARSET, and VIETNAMESE_CHARSET do not seem to have a wx
		// equivalent to map to, so I'm mapping SYMBOL_CHARSET to wxFONTENCODING_DEFAULT; and the other
		// two to wxFONTENCODING_DEFAULT.
	case wxFONTENCODING_CP1252: return 0; //  wxFONTENCODING_CP1252 [Microsoft analogue of ISO8859-1 "WinLatin1"] maps to (0) ANSI_CHARSET
	//case wxFONTENCODING_DEFAULT: return 1; // wxFONTENCODING_DEFAULT maps to (1) DEFAULT_CHARSET
	//case wxFONTENCODING_SYSTEM: return 2; //  wxFONTENCODING_SYSTEM maps to [???] SYMBOL_CHARSET
	case wxFONTENCODING_ISO8859_1: return 1; // Western European (ISO) whm Note: Bob Eaton's GetEncodingStringForXmlFiles() function maps CP_SYMBOL to "iso-8859-1"
	case wxFONTENCODING_ISO8859_2: return 2; // Central European (ISO) TODO: verify
	case wxFONTENCODING_ISO8859_3: return 3; // Latin 3 (ISO) TODO: verify
	case wxFONTENCODING_ISO8859_4: return 4; // Baltic (ISO) TODO: verify
	case wxFONTENCODING_ISO8859_5: return 5; // Cyrillic (ISO) TODO: verify
	case wxFONTENCODING_ISO8859_6: return 6; // Arabic (ISO) TODO: verify
	case wxFONTENCODING_ISO8859_7: return 7; // Greek (ISO) TODO: verify
	case wxFONTENCODING_ISO8859_8: return 8; // Hebrew (ISO-Visual) TODO: verify
	case wxFONTENCODING_ISO8859_9: return 9; // Turkish (ISO) TODO: verify
	case wxFONTENCODING_ISO8859_10: return 10; // TODO: verify
	case wxFONTENCODING_ISO8859_11: return 11; // TODO: verify
	case wxFONTENCODING_ISO8859_12: return 12; // TODO: verify
	case wxFONTENCODING_ISO8859_13: return 13; // Estonian (ISO) TODO: verify
	case wxFONTENCODING_ISO8859_14: return 14; // TODO: verify
	case wxFONTENCODING_ISO8859_15: return 15; // Latin 9 (ISO) TODO: verify
	case wxFONTENCODING_MACROMAN: return 77; // wxFONTENCODING_MACROMAN [mac has encodings 47-86 under wx, default to Roman] maps to MAC_CHARSET
	case wxFONTENCODING_CP932: return 128; // wxFONTENCODING_CP932 [shift-JIS] maps to SHIFTJIS_CHARSET 
	case wxFONTENCODING_CP949: return 129; // wxFONTENCODING_CP949 [Korean (Hangul charset)] maps to HANGUL_CHARSET
	//case wxFONTENCODING_DEFAULT: return 130; // wxFONTENCODING_DEFAULT maps to [???] JOHAB_CHARSET 
	case wxFONTENCODING_CP936: return 134; // wxFONTENCODING_CP936 [Chinese simplified (GB)] maps to GB2312_CHARSET 
	case wxFONTENCODING_CP950: return 136; // wxFONTENCODING_CP950 [Chinese (traditional - Big5)] maps to CHINESEBIG5_CHARSET 
	case wxFONTENCODING_CP1253: return 161; // wxFONTENCODING_CP1253 [WinGreek (8859-7)] maps to GREEK_CHARSET 
	case wxFONTENCODING_CP1254: return 162; // wxFONTENCODING_CP1254 [WinTurkish] maps to TURKISH_CHARSET 
	//case wxFONTENCODING_DEFAULT: return 163; // wxFONTENCODING_DEFAULT maps to [???] VIETNAMESE_CHARSET 
	case wxFONTENCODING_CP1255: return 177; // wxFONTENCODING_CP1255 [WinHebrew] maps to HEBREW_CHARSET 
	case wxFONTENCODING_CP1256: return 178; // wxFONTENCODING_CP1256 [WinArabic] maps to ARABIC_CHARSET 
	case wxFONTENCODING_CP1257: return 186; // wxFONTENCODING_CP1257 [WinBaltic (same as Latin 7)] maps to BALTIC_CHARSET 
	case wxFONTENCODING_CP1251: return 204; // wxFONTENCODING_CP1251 [Microsoft analogue of ISO8859-5 "WinCyrillic"] maps to RUSSIAN_CHARSET 
	case wxFONTENCODING_CP874: return 222; // wxFONTENCODING_CP874 [WinThai] maps to THAI_CHARSET 
	case wxFONTENCODING_CP1250: return 238; // wxFONTENCODING_CP1250 [Microsoft analogue of ISO8859-2 "WinLatin2"] maps to EASTEUROPE_CHARSET 
	//case wxFONTENCODING_SYSTEM: return 255; // wxFONTENCODING_SYSTEM maps to OEM_CHARSET 
	default: return 0; // Use the default encoding of the underlying operating system for all the others
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      aStr   <- a CBString (byte string) representing the prologue line for an XML file
///                         which contains the encoding
/// \remarks
/// Called from: the App's DoKBSaveAsXML(), the Doc's BackupDocument(), DoFileSave(), and
/// DoTransformedDocFileSave().
/// Generates a byte string formatted as an XML prologue that contains the encoding= element with
/// the standard encoding name. The wxWidgets version identifies only a subset of the exhaustive
/// list of (mostly unused) encoding names available in the MFC version.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::GetEncodingStringForXmlFiles(CBString& aStr)
{
#ifdef _UNICODE
	// rde's MFC note: if the user configures a legacy font in the AIUnicode product, this will not work! That is, 
	//	I'm assuming with this code that AIU always only deals with Unicode, for which UTF-8 is the correct
	//	encoding name/code page. If the user tried to configure, say, SIL Galatia in AIU, this will fail to work 
	//	(since in that case, the encoding name should be iso-8859-1)... however, there's no reason we need to 
	//	support legacy fonts in the Unicode product, so this is a fair assumption for now.
	//
	// whm notes: This GetEncodingStringForXmlFiles is Bob Eaton's large switch case structure
	// designed to return an encoding name string for use in the prologue of Adapt It's
	// xml files. The m_nCodePage is assigned its current value within the UpdateTextHeights() 
	// function which is called often - whenever a font change requires redrawing the screen.
	// The current value of m_nCodePage is determined by Microsoft specific calls which are
	// unique to the Windows platform. Bob uses the FONTSIGNATURE and CHARSETINFO structs 
	// along with the GetTextCharsetInfo() and TranslateCharsetInfo() functions. When AI starts 
	// up it initially sets the m_systemEncoding UINT using the Windows GetACP(void) function 
	// which "returns the current ANSI code-page identifier for the system, or a default 
	// identifier if no code page is current." The MSDN "Code Page Identifiers" table lists 
	// about 149 ANSI code page numbers from 037 through 65001. 
	// The FONTSIGNATURE struct contains two bitfields, the first is a 128-bit Unicode subset
	// bitfield, and the second is a 64-bit code-page bitfield that identifies a specific
	// character set of code page - code pages are in the lower 32 bits of this bitfield; the
	// high 32 bits are used for non-Windows code pages. The CHARSETINFO struct contains info
	// about a character set. This struct has a UINT ciCharset containing the "Character set 
	// value"; a UINT ciACP containing the "ANSI code-page identifier"; and a FONTSIGNATURE fs
	// member that "identifies the Unicode and code pagefont signature values." 
	// The GetTextCharsetInfo() function "retrieves information about the character set of 
	// the font that is currently selected into a specified device context." Its int return 
	// value can be one of the following defined identifiers (their #define values on Windows 
	// are also indicated):
	// ANSI_CHARSET			0
	// BALTIC_CHARSET		186
	// CHINESEBIG5_CHARSET	136
	// DEFAULT_CHARSET		1
	// EASTEUROPE_CHARSET	238
	// GB2312_CHARSET		134
	// GREEK_CHARSET		161
	// HANGUL_CHARSET		129
	// MAC_CHARSET			77
	// OEM_CHARSET			255
	// RUSSIAN_CHARSET		204
	// SHIFTJIS_CHARSET		128
	// SYMBOL_CHARSET		2
	// TURKISH_CHARSET		162
	// VIETNAMESE_CHARSET	163
	//
	// Korean language edition of Windows:
	// JOHAB_CHARSET		130
	//
	// Middle East language edition of Windows:
	// ARABIC_CHARSET		178
	// HEBREW_CHARSET		177
	//
	// Thai language edition of Windows:
	// THAI_CHARSET			222
	//
	// If the function fails, the return value is DEFAULT_CHARSET.
	// The TranslateCharsetInfo() function "translates based on the specified character set, 
	// code page, or font signature value, setting all members of the destination structure 
	// to appropriate values."
	// The TranslateCharsetInfo() function

	// Bob's code in UpdateTextHeights() is designed to detect if the font is a symbol
	// font, and if so, use a different "encoding" string in Adapt It's xml files.
	// Problem: m_nCodePage in the MFC version is a 32 bit int, whereas within the wxWidgets
	// font encoding representation they are represented as an enum, which has a maximum of
	// 255 different values. Bob's GetEncodingStringForXmlFiles() function has a 
	// switch(m_nCodePage) statement which has 141 cases in it, but the values in the
	// switch/case range from 42 to 65006. The values above 255 are not legal for an enum.
	// The appropriate thing to do within wxWidgets is to supply the encoding string
	// for the font encodings that wxWidgets knows about. Therefore, I've commented out
	// parts of Bob's large switch() statement and provided the encoding strings 
	// that match those Bob uses where the closest font encoding equivalents match,
	// and a few that don't match - based on the current encoding of Adapt It's source 
	// font.

	aStr = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n";
#else

	
	switch (m_srcEncoding) //switch(m_nCodePage)
	{
	//case 37:
	//	aStr = "IBM037";
	//	break;
	case wxFONTENCODING_CP437: //case 437:
		aStr = "IBM437";
		break;
	//case 500:
	//	aStr = "IBM500";
	//	break;
	//case 708:
	//	aStr = "ASMO-708";
	//	break;
	//case 720:
	//	aStr = "DOS-720";
	//	break;
	//case 737:
	//	aStr = "ibm737";
	//	break;
	//case 775:
	//	aStr = "ibm775";
	//	break;
	case wxFONTENCODING_CP850: //case 850:
		aStr = "ibm850";
		break;
	case wxFONTENCODING_CP852: //case 852:
		aStr = "ibm852";
		break;
	case wxFONTENCODING_CP855: //case 855:
		aStr = "IBM855";
		break;
	//case 857:
	//	aStr = "ibm857";
	//	break;
	//case 858:
	//	aStr = "IBM00858";
	//	break;
	//case 860:
	//	aStr = "IBM860";
	//	break;
	//case 861:
	//	aStr = "ibm861";
	//	break;
	//case 862:
	//	aStr = "DOS-862";
	//	break;
	//case 863:
	//	aStr = "IBM863";
	//	break;
	//case 864:
	//	aStr = "IBM864";
	//	break;
	//case 865:
	//	aStr = "IBM865";
	//	break;
	case wxFONTENCODING_CP866: //case 866:
		aStr = "cp866";
		break;
	//case 869:
	//	aStr = "ibm869";
	//	break;
	//case 870:
	//	aStr = "IBM870";
	//	break;
	case wxFONTENCODING_CP874: //case 874:
		aStr = "windows-874";
		break;
	//case 875:
	//	aStr = "cp875";
	//	break;
	case wxFONTENCODING_CP932: //case 932:
		aStr = "shift_jis";
		break;
	case wxFONTENCODING_CP936: //case 936:
		aStr = "gb2312";
		break;
	case wxFONTENCODING_CP949: //case 949:
		aStr = "ks_c_5601-1987";
		break;
	case wxFONTENCODING_CP950: //case 950:
		aStr = "big5";
		break;
	//case 1026:
	//	aStr = "IBM1026";
	//	break;
	//case 1047:
	//	aStr = "IBM01047";
	//	break;
	//case 1140:
	//	aStr = "IBM01140";
	//	break;
	//case 1141:
	//	aStr = "IBM01141";
	//	break;
	//case 1142:
	//	aStr = "IBM01142";
	//	break;
	//case 1143:
	//	aStr = "IBM01143";
	//	break;
	//case 1144:
	//	aStr = "IBM01144";
	//	break;
	//case 1145:
	//	aStr = "IBM01145";
	//	break;
	//case 1146:
	//	aStr = "IBM01146";
	//	break;
	//case 1147:
	//	aStr = "IBM01147";
	//	break;
	//case 1148:
	//	aStr = "IBM01148";
	//	break;
	//case 1149:
	//	aStr = "IBM01149";
	//	break;
	//case 1200:
	//	aStr = "utf-16";
	//	break;
	//case 1201:
	//	aStr = "unicodeFFFE";
	//	break;
	case wxFONTENCODING_CP1250: //case 1250:
		aStr = "windows-1250";
		break;
	case wxFONTENCODING_CP1251: //case 1251:
		aStr = "windows-1251";
		break;
	case wxFONTENCODING_CP1253: //case 1253:
		aStr = "windows-1253";
		break;
	case wxFONTENCODING_CP1254: //case 1254:
		aStr = "windows-1254";
		break;
	case wxFONTENCODING_CP1255: //case 1255:
		aStr = "windows-1255";
		break;
	case wxFONTENCODING_CP1256: //case 1256:
		aStr = "windows-1256";
		break;
	case wxFONTENCODING_CP1257: //case 1257:
		aStr = "windows-1257";
		break;
	//case 1258:
	//	aStr = "windows-1258";
	//	break;
	//case 1361:
	//	aStr = "Johab";
	//	break;
	case wxFONTENCODING_MACROMAN: //case 10000:
		aStr = "macintosh";
		break;
	case wxFONTENCODING_MACJAPANESE: //case 10001:
		aStr = "x-mac-japanese";
		break;
	case wxFONTENCODING_MACCHINESETRAD: //case 10002:
		aStr = "x-mac-chinesetrad";
		break;
	case wxFONTENCODING_MACKOREAN: //case 10003:
		aStr = "x-mac-korean";
		break;
	case wxFONTENCODING_MACARABIC: //case 10004:
		aStr = "x-mac-arabic";
		break;
	case wxFONTENCODING_MACHEBREW: //case 10005:
		aStr = "x-mac-hebrew";
		break;
	case wxFONTENCODING_MACGREEK: //case 10006:
		aStr = "x-mac-greek";
		break;
	case wxFONTENCODING_MACCYRILLIC: //case 10007:
		aStr = "x-mac-cyrillic";
		break;
	case wxFONTENCODING_MACCHINESESIMP: //case 10008:
		aStr = "x-mac-chinesesimp";
		break;
	case wxFONTENCODING_MACROMANIAN: //case 10010:
		aStr = "x-mac-romanian";
		break;
	//case 10017:
	//	aStr = "x-mac-ukrainian";
	//	break;
	case wxFONTENCODING_MACTHAI: //case 10021:
		aStr = "x-mac-thai";
		break;
	case wxFONTENCODING_MACCENTRALEUR: //case 10029:
		aStr = "x-mac-ce"; // "ce" stands for "central european"
		break;
	case wxFONTENCODING_MACICELANDIC: //case 10079:
		aStr = "x-mac-icelandic";
		break;
	case wxFONTENCODING_MACTURKISH: //case 10081:
		aStr = "x-mac-turkish";
		break;
	case wxFONTENCODING_MACCROATIAN: //case 10082:
		aStr = "x-mac-croatian";
		break;
	//case 20000:
	//	aStr = "x-Chinese-CNS"; // same as wxFONTENCODING_CP950 Chinese (traditional - Big5)?
	//	break;
	//case 20001:
	//	aStr = "x-cp20001";
	//	break;
	//case 20002:
	//	aStr = "x-Chinese-Eten";
	//	break;
	//case 20003:
	//	aStr = "x-cp20003";
	//	break;
	//case 20004:
	//	aStr = "x-cp20004";
	//	break;
	//case 20005:
	//	aStr = "x-cp20005";
	//	break;
	//case 20105:
	//	aStr = "x-IA5";
	//	break;
	//case 20106:
	//	aStr = "x-IA5-German";
	//	break;
	//case 20107:
	//	aStr = "x-IA5-Swedish";
	//	break;
	//case 20108:
	//	aStr = "x-IA5-Norwegian";
	//	break;
	//case 20127:
	//	aStr = "us-ascii";
	//	break;
	//case 20261:
	//	aStr = "x-cp20261";
	//	break;
	//case 20269:
	//	aStr = "x-cp20269";
	//	break;
	//case 20273:
	//	aStr = "IBM273";
	//	break;
	//case 20277:
	//	aStr = "IBM277";
	//	break;
	//case 20278:
	//	aStr = "IBM278";
	//	break;
	//case 20280:
	//	aStr = "IBM280";
	//	break;
	//case 20284:
	//	aStr = "IBM284";
	//	break;
	//case 20285:
	//	aStr = "IBM285";
	//	break;
	//case 20290:
	//	aStr = "IBM290";
	//	break;
	//case 20297:
	//	aStr = "IBM297";
	//	break;
	//case 20420:
	//	aStr = "IBM420";
	//	break;
	//case 20423:
	//	aStr = "IBM423";
	//	break;
	//case 20424:
	//	aStr = "IBM424";
	//	break;
	//case 20833:
	//	aStr = "x-EBCDIC-KoreanExtended";
	//	break;
	//case 20838:
	//	aStr = "IBM-Thai";
	//	break;
	//case 20866:
	//	aStr = "koi8-r";
	//	break;
	//case 20871:
	//	aStr = "IBM871";
	//	break;
	//case 20880:
	//	aStr = "IBM880";
	//	break;
	//case 20905:
	//	aStr = "IBM905";
	//	break;
	//case 20924:
	//	aStr = "IBM00924";
	//	break;
	case wxFONTENCODING_EUC_JP: //case 20932:
		aStr = "EUC-JP";
		break;
	//case 20936:
	//	aStr = "x-cp20936";
	//	break;
	//case 20949:
	//	aStr = "x-cp20949";
	//	break;
	//case 21025:
	//	aStr = "cp1025";
	//	break;
	case wxFONTENCODING_KOI8_U: //case 21866:
		aStr = "koi8-u";
		break;
	//case CP_SYMBOL: // ??? The enum value for this in Windows is 42 which is wxFONTENCODING_EUC_JP
	case wxFONTENCODING_ISO8859_1: //case 28591:
		aStr = "iso-8859-1";
		break;
	case wxFONTENCODING_ISO8859_2: //case 28592:
		aStr = "iso-8859-2";
		break;
	case wxFONTENCODING_ISO8859_3: //case 28593:
		aStr = "iso-8859-3";
		break;
	case wxFONTENCODING_ISO8859_4: //case 28594:
		aStr = "iso-8859-4";
		break;
	case wxFONTENCODING_ISO8859_5: //case 28595:
		aStr = "iso-8859-5";
		break;
	case wxFONTENCODING_ISO8859_6: //case 28596:
		aStr = "iso-8859-6";
		break;
	case wxFONTENCODING_ISO8859_7: //case 28597:
		aStr = "iso-8859-7";
		break;
	case wxFONTENCODING_ISO8859_8: //case 28598:
		aStr = "iso-8859-8";
		break;
	case wxFONTENCODING_ISO8859_9: //case 28599:
		aStr = "iso-8859-9";
		break;
	// Note: wxFONTENCODING_CP1252 is just above the default: case below
	case wxFONTENCODING_ISO8859_13: //case 28603:
		aStr = "iso-8859-13";
		break;
	case wxFONTENCODING_ISO8859_15: //case 28605:
		aStr = "iso-8859-15";
		break;
	//case 29001:
	//	aStr = "x-Europa";
	//	break;
	//case 38598:
	//	aStr = "iso-8859-8-i";
	//	break;
	//case 50220:
	//	aStr = "iso-2022-jp";
	//	break;
	//case 50221:
	//	aStr = "csISO2022JP";
	//	break;
	//case 50222:
	//	aStr = "iso-2022-jp";
	//	break;
	//case 50225:
	//	aStr = "iso-2022-kr";
	//	break;
	//case 50227:
	//	aStr = "x-cp50227";
	//	break;
	//case 51932:
	//	aStr = "euc-jp";
	//	break;
	//case 51936:
	//	aStr = "EUC-CN";
	//	break;
	//case 51949:
	//	aStr = "euc-kr";
	//	break;
	//case 52936:
	//	aStr = "hz-gb-2312";
	//	break;
	//case 54936:
	//	aStr = "GB18030";
	//	break;
	//case 57002:
	//	aStr = "x-iscii-de";
	//	break;
	//case 57003:
	//	aStr = "x-iscii-be";
	//	break;
	//case 57004:
	//	aStr = "x-iscii-ta";
	//	break;
	//case 57005:
	//	aStr = "x-iscii-te";
	//	break;
	//case 57006:
	//	aStr = "x-iscii-as";
	//	break;
	//case 57007:
	//	aStr = "x-iscii-or";
	//	break;
	//case 57008:
	//	aStr = "x-iscii-ka";
	//	break;
	//case 57009:
	//	aStr = "x-iscii-ma";
	//	break;
	//case 57010:
	//	aStr = "x-iscii-gu";
	//	break;
	//case 57011:
	//	aStr = "x-iscii-pa";
	//	break;
	case wxFONTENCODING_UTF7: //case 65000:
		aStr = "utf-7";
		break;
	case wxFONTENCODING_UTF8: //case 65001:
		aStr = "utf-8";
		break;
	case wxFONTENCODING_UTF32LE: // case 65005: // I assume utf-32 is Little Endian LE
		aStr = "utf-32";
		break;
	case wxFONTENCODING_UTF32BE: //case 65006:
		aStr = "utf-32BE";
		break;

	case wxFONTENCODING_CP1252: //case 1252: // falls through to "Windows-1252"
	default:
		aStr = "Windows-1252";
		break;
	}

	aStr = "<?xml version=\"1.0\" encoding=\"" + aStr + "\" standalone=\"yes\"?>\r\n";
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      f               -> the wxFile instance used to save the KB file
/// \param      bIsGlossingKB   -> TRUE if saving a glossing KB, otherwise FALSE (the default) for a
///                                 regular KB
/// \remarks
/// Called from: the App's StoreGlossingKB() and StoreKB().
/// Structures the KB data in XML form, calling DoWrite() to save the various XML parts to the 
/// external file.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::DoKBSaveAsXML(wxFile& f, bool bIsGlossingKB)
{
	// In DoKBSaveAsXML, the bIsGlossingKB parameter has a default FALSE value (ie. normal KB), and
	// must be explicitly included as TRUE for the glossing KB
	CBString aStr;

	// prologue (Changed by BEW, 18june07, at request of Bob Eaton so as to support
	// legacy KBs using his SILConverters software, UTF-8 becomes Windows-1252 for the Regular app)
	GetEncodingStringForXmlFiles(aStr);
	DoWrite(f,aStr);

	//	BEW added AdaptItKnowledgeBase element, 3Apr06, for Bob Eaton's SILConverters support, 
	aStr = "<AdaptItKnowledgeBase xmlns=\"http://www.sil.org/computing/schemas/AdaptIt KB.xsd\">\r\n";
	DoWrite(f,aStr);

	// add the comment with the warning about not opening the XML file in MS WORD 
	// 'coz is corrupts it - presumably because there is no XSLT file defined for it
	// as well. When the file is then (if saved in WORD) loaded back into Adapt It,
	// the latter goes into an infinite loop when the file is being parsed in.
	// (MakeMSWORDWarning is defined in XML.cpp)
	aStr = MakeMSWORDWarning(TRUE); // the warning ends with \r\n so we don't need
									// to add them here; TRUE gives us a note included as well
	DoWrite(f,aStr);


	// construct the opening tag and add the list of targetUnits with their associated
	// key strings (source text)
	CTargetUnit* pTU;
	CKB* pKB;
	if (bIsGlossingKB)
		pKB = m_pGlossingKB;
	else
		pKB = m_pKB;
	wxString srcStr;
	CBString tempStr;
	int i;
	int maxWords;
	wxString intStr;
	// wx note: the wx version in Unicode build refuses assign a CBString to char numStr[24]
	// so I'll declare numStr as a CBString also
	CBString numStr; //char numStr[24];

	// the KB opening tag
	intStr.Empty();
	// wx note: The itoa() operator is Microsoft specific and not standard; unknown to g++ on Linux/Mac.
	// The wxSprintf() statement below in Unicode build won't accept CBString or char numStr[24] 
	// for first parameter, therefore, I'll simply do the int to string conversion in UTF-16 with 
	// wxString's overloaded insertion operatior << then convert to UTF-8 with Bruce's Convert16to8() 
	// method. [We could also do it here directly with wxWidgets' conversion macros rather than calling
	// Convert16to8() - see the Convert16to8() function in the App.]
	//wxSprintf(numStr, 24,_T("%d"),(int)VERSION_NUMBER); 
	intStr << (int)VERSION_NUMBER; // versionable schema number (see AdaptitConstants.h)
#ifdef _UNICODE
	numStr = gpApp->Convert16to8(intStr);
#else
	numStr = intStr;
#endif
	
	aStr = "<";
	if (bIsGlossingKB)
	{
		// aStr += gkb; // BEW changed to KB, for SILConverter support, so that KB element used for both KBs
		aStr += xml_kb;
		aStr += " docVersion=\"";
		aStr += numStr;
		aStr += "\" max=\"1";
	}
	else
	{
		aStr += xml_kb;
		aStr += " docVersion=\"";
		aStr += numStr;
		aStr += "\" srcName=\"";
#ifdef _UNICODE
		tempStr = Convert16to8(pKB->m_sourceLanguageName);
		InsertEntities(tempStr);
		aStr += tempStr;
		aStr += "\" tgtName=\"";
		tempStr = Convert16to8(pKB->m_targetLanguageName);
#else
		tempStr = pKB->m_sourceLanguageName;
		InsertEntities(tempStr);
		aStr += tempStr;
		aStr += "\" tgtName=\"";
		tempStr = pKB->m_targetLanguageName;
#endif
		InsertEntities(tempStr);
		aStr += tempStr;
		aStr += "\" max=\"";
		intStr.Empty(); // needs to start empty, otherwise << will append the string value of the int
		intStr << pKB->m_nMaxWords;
#ifdef _UNICODE
		numStr = gpApp->Convert16to8(intStr);
#else
		numStr = intStr;
#endif
		aStr += numStr;
	}
	aStr += "\">\r\n";
	DoWrite(f,aStr);

	// now the maps -- note, we have to include the map number (and for the user's benefit
	// it will be a 1-based value), we can't rely on counting maps on input of the xml file
	// later on, because some maps may be empty and so not be included in the xml output
	if (bIsGlossingKB)
	{
		maxWords = 1; // GlossingKB has only one map
	}
	else
	{
		maxWords = (int)MAX_WORDS;
	}
	// loop through all the maps
	for (i = 0; i < maxWords; i++)
	{
		if (!pKB->m_pMap[i]->empty())
		{
			aStr = "\t<MAP mn=\"";
			intStr.Empty(); // needs to start empty, otherwise << will append the string value of the int
			intStr << i + 1;
#ifdef _UNICODE
			numStr = gpApp->Convert16to8(intStr);
#else
			numStr = intStr;
#endif
			aStr += numStr;
			aStr += "\">\r\n";
			DoWrite(f,aStr);

			MapKeyStringToTgtUnit::iterator iter;
			for( iter = pKB->m_pMap[i]->begin(); iter != pKB->m_pMap[i]->end(); ++iter )
			{
				srcStr = iter->first;
				pTU = iter->second;
				aStr = MakeKBElementXML(srcStr,pTU,2); // 2 = two left-margin tabs
				DoWrite(f,aStr);
			}

			// close off this map
			aStr = "\t</MAP>\r\n";
			DoWrite(f,aStr);
		}
	}

	// KB closing tag
	aStr = "</";
	if (bIsGlossingKB)
	{
		// aStr += gkb; // BEW changed 19Apr06, so both kbs use KB element name, not GKB for glossing one
		aStr += xml_kb;
	}
	else
	{
		aStr += xml_kb;
	}
	aStr += ">\r\n";
	DoWrite(f,aStr);

	// BEW added 0Apr06 for .NET parsing support for xml
	aStr = "</AdaptItKnowledgeBase>\r\n";
	DoWrite(f,aStr);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: the App's DoStartWorkingWizard(), OnAdvancedBookMode(), the canvas' OnLButtonDown(),
/// the Doc's OnOpenDocument(), the View's Jump(), OnAdvancedUseTransliterationMode(), 
/// CDocPage's InitDialog(), OnWizardFinish(), CJoinDialog's InitDialog(), OnBnClickedJoinNow(), 
/// CMoveDialog's InitDialog(), OnBnClickedMoveNow(), OnBnClickedButtonRenameDoc(), 
/// OnBnClickedRadioToBookFolder(), OnBnClickedRadioFromBookFolder(), CPhraseBox's OnPhraseBoxChanged(), 
/// MoveToNextPile(), MoveToPrevPile(), OnKeyDown(), ChooseTranslation(), CSplitDialog's 
/// OnBnClickedButtonSplitNow(), InitDialog(), CWhichBook's InitDialog(), OnSelchangeChooseBook(),
/// OnCancel(), and OnOK().
/// Updates the status bar message to reflect the current activity.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::RefreshStatusBarInfo()
{
	CAdapt_ItView* pView = gpApp->GetView();
	CMainFrame* pFrame = gpApp->GetMainFrame();
	wxString rscStr; // MFC Note: to get past a bug 
	rscStr = _("Current Project: %s");
	wxString message;
	message = message.Format(rscStr,gpApp->m_curProjectName.c_str());

	// do nothing if there is no active pile or a bad sequence number of no target box
	bool bDocIsThere = TRUE;
	if (gpApp->m_pTargetBox != NULL)
	{
		// whm added test for m_pActivePile == NULL
		if (gpApp->m_pActivePile == NULL || gpApp->m_pActivePile->m_pSrcPhrase == NULL 
			|| gpApp->m_nActiveSequNum < 0
			|| gpApp->m_pTargetBox->IsShown())
			bDocIsThere = FALSE;
	}
	else
		bDocIsThere = FALSE;
	
	if (bDocIsThere)
	{
		gpApp->m_pActivePile = pView->GetPile(gpApp->m_nActiveSequNum); // ensure a valid pointer (BEW added 28Jun05)
		wxString chVerse = pView->GetChapterAndVerse(gpApp->m_pActivePile->m_pSrcPhrase);
		if (!chVerse.IsEmpty())
		{
			message += _T("  ") + chVerse;
		}
	}
	wxString prefix;
	if (gbIsGlossing)
	{
		// IDS_GLOSSING
		prefix = _("Glossing");
	}
	else
	{
		// IDS_ADAPTING
		prefix = _("Adapting");
	}
	message = prefix + _T("  ") + message;
	wxString mssg;
	if (gpApp->m_bBookMode && !gpApp->m_bDisableBookMode)
	{
		if (gpApp->m_pCurrBookNamePair != NULL && gpApp->m_nBookIndex != -1)
		{
			// IDS_CURFOLDER
			mssg = mssg.Format(_("  Current Folder: %s"),gpApp->m_pCurrBookNamePair->seeName.c_str());
			message += mssg;
		}
		else
		{
			wxString undef;
			// IDS_UNDEFINED
			undef = _(" Undefined");
			// IDS_CURFOLDER
			mssg = mssg.Format(_("  Current Folder: %s"),undef.c_str());
			message += mssg;
		}
	}
	else
	{
		// IDS_CURFOLDER
		mssg = mssg.Format(_("  Current Folder: %s"),_("Adaptations"));
		message += mssg;
	}
	if (gpApp->m_bTransliterationMode)
	{
		// add the extra !! Using Transliteration Mode !! to the end, when it is turned on
		//IDS_USING_TRANSLIT_STATUS_MSG
		mssg = _("!! TRANSLITERATING !!"); 
		message += _T("    ") + mssg;
	}
	pView->StatusBarMessage(message);
	pFrame->m_pStatusBar->Update();
}

// Getters, setters, and shorthands.  Added by Jonathn Field 2005

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the current book display name
/// \remarks
/// Called from: The App's GetCurrentDocFolderDisplayName(), CMoveDialog's OnBnClickedViewOther(),
/// and MoveDirectionChanged().
/// Gets the seeName for the current book.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetCurrentBookDisplayName()
{
	return m_pCurrBookNamePair->seeName;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the current book folder path
/// \remarks
/// Called from: The App's GetCurrentBookFolderName(), GetCurrentDocFolderPath(),
/// CMoveDialog's OnBnClickedButtonRenameDoc(), OnBnClickedViewOther(), UpdateFileList(),
/// and Mover::BeginMove().
/// Gets the value stored in m_bibleBooksFolderPath.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetCurrentBookFolderPath() {
	return m_bibleBooksFolderPath; 
	// OR perhaps ConcatenatePathBits(GetAdaptationsFolderPath(), m_pCurrBookNamePair->dirName);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the current book folder name
/// \remarks
/// Called from: (currently unused).
/// Gets the current book folder name by stripping the path from the current book folder path.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetCurrentBookFolderName()
{
	return StripPath(GetCurrentBookFolderPath());
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the adaptations folder display name
/// \remarks
/// Called from: the App's GetCurrentDocFolderDisplayName(), CMoveDialog's OnBnClickedViewOther(), 
/// and MoveDirectionChanged().
/// Gets the adaptations folder display name which is "Adaptations"
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetAdaptationsFolderDisplayName()
{
	wxString rv; // Return Value
	rv = _("Adaptations");
	return rv;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the adaptations folder path
/// \remarks
/// Called from: the App's GetAdaptationsFolderName(), GetCurrentDocFolderPath(), 
/// CMoveDialog's OnBnClickedButtonRenameDoc(), OnBnClickedViewOther(), UpdateFileList(),
/// and Mover::BeginMove(). 
/// Gets the adaptations folder path stored in m_curAdaptionsPath.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetAdaptationsFolderPath()
{
	return m_curAdaptionsPath;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the adaptations folder name
/// \remarks
/// Called from: (currently unused). 
/// Gets the adaptations folder name by stripping the path from the adaptatioins folder path.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetAdaptationsFolderName()
{
	return StripPath(GetAdaptationsFolderPath());
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the current document's name
/// \remarks
/// Called from: the App's GetCurrentDocPath(), CJoinDialog's OnBnClickedJoinNow(),  
/// InitialiseLists(), CSplitDialog's OnBnClickedButtonSplitNow(), 
/// SplitAtPhraseBoxLocation_Interactive(), and InitDialog().
/// Gets the current document's name as stored in m_curOutputFilename.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetCurrentDocFileName()
{
	return m_curOutputFilename;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the current document's path
/// \remarks
/// Called from: CSplitDialog::SplitAtPhraseBoxLocation_Interactive(), DoSplitIntoChapters(),
/// and SplitIntoChapters_Interactive().
/// Gets the current document's path by concatenating its path and name.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetCurrentDocPath()
{
	return ConcatenatePathBits(GetCurrentDocFolderPath(), GetCurrentDocFileName());
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the current document's display name
/// \remarks
/// Called from: CSplitDialog::ListFiles().
/// Gets the current document's display name depending on the book mode.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetCurrentDocFolderDisplayName()
{
	return GetBookMode() ? GetCurrentBookDisplayName() : GetAdaptationsFolderDisplayName();
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the current document folder's path
/// \remarks
/// Called from: the App's GetCurrentDocPath(), ChangeDocUnderlyingFileDetailsInPlace(),
/// CJoinDialog's OnBnClickedJoinNow(), InitialiseLists(), CMoveDialog::OnBnClickedButtonRenameDoc(),
/// CSplitDialog's SplitAtPhraseBoxLocation_Interactive(), SplitIntoChapters_Interactive(), 
/// and ListFiles().
/// Gets the current document folder's path depending on the book mode.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetCurrentDocFolderPath()
{
	return GetBookMode() ? GetCurrentBookFolderPath() : GetAdaptationsFolderPath();
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      NewFileName -> the new filename
/// \remarks
/// Called from: CSplitDialog's SplitIntoChapters_Interactive().
/// Calls ChangeDocUnderlyingFileDetailsInPlace().
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::ChangeDocUnderlyingFileNameInPlace(wxString NewFileName)
{
	ChangeDocUnderlyingFileDetailsInPlace(NewFileName);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      NewFolderPath -> the new folder path
/// \param      NewFileName   -> the new filename
/// \remarks
/// Called from: the App's ChangeDocUnderlyingFileDetailsInPlace() [one paramater override],
/// CJoinDialog's OnBnClickedJoinNow(), CMoveDialog's OnBnClickedButtonRenameDoc(),
/// Changes the App's m_curOutputFilename to NewFileName, and the App's m_curOutputPath to
/// NewFolderPath + NewFileName. Calls the Doc's SetFilename() with m_curOutputPath.
/// See override that takes only NewFileName parameter.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::ChangeDocUnderlyingFileDetailsInPlace(wxString NewFolderPath, wxString NewFileName)
{
	CAdapt_ItDoc *d = GetDocument();
	m_curOutputFilename = NewFileName;
	m_curOutputPath = ConcatenatePathBits(NewFolderPath, NewFileName);
	d->SetFilename(m_curOutputPath, true);

	// get OnIdle() to update the document name
	gbUpdateDocTitleNeeded = TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      NewFileName   -> the new filename
/// \remarks
/// Called from: the App's ChangeDocUnderlyingFileNameInPlace(),
/// CSplitDialog's SplitAtPhraseBoxLocation_Interactive().
/// Calls ChangeDocUnderlyingFileDetailsInPlace() [two parameter override].
/// See override that takes both NewFolderPath and NewFileName.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::ChangeDocUnderlyingFileDetailsInPlace(wxString NewFileName)
{
	ChangeDocUnderlyingFileDetailsInPlace(this->GetCurrentDocFolderPath(), NewFileName);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if doc is open, FALSE otherwise
/// \param      FolderPath   -> the folder path
/// \param      FileName     -> the filename
/// \remarks
/// Called from: CMoveDialog::OnBnClickedButtonRenameDoc() and Mover::FinishMove().
/// Determines if the FolderPath+FileName doc is open (does not use file operations).
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::IsOpenDoc(wxString FolderPath, wxString FileName)
{
	return m_curOutputPath.CmpNoCase(ConcatenatePathBits(FolderPath, FileName)) == 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if doc has unsaved changes, FALSE otherwise
/// \remarks
/// Called from: CMoveDialog::OnBnClickedButtonRenameDoc(), Mover::FinishMove(),
/// CSplitDialog's SplitAtPhraseBoxLocation_Interactive() and DoSplitIntoChapters().
/// Determines if the doc is dirty (has unsaved changes) by calling IsModified().
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::GetDocHasUnsavedChanges()
{
	return (gpApp->GetDocument()->IsModified() != FALSE);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the book indicator string for the current book folder
///                 if in book mode, otherwise a null string if book mode is not activated
/// \remarks
/// Called from: the App's GetBookID(), and Mover::FinishMove().
/// Gets the book indicator string for the current book folder, or a null string if not in
/// book folder mode.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetBookIndicatorStringForCurrentBookFolder()
{
	wxString rv;
	rv.Empty();
	wxString s;
	
	// BEW modified 03Nov05: we rely on the fact that when book mode is on, the m_nBookIndex
	// tells us which BookNamePair pointer to index in m_pBibleBooks in order to get the
	// current directory name, and the book ID code (from the same BookNamePair instance)
	// and we return an empty string if book mode is not turned on
	if (GetBookMode() && m_nBookIndex >= 0 && m_nBookIndex <= 66)
	{
		BookNamePair* pPair = (BookNamePair*)m_pBibleBooks->Item(m_nBookIndex);
		return pPair->bookCode;
	}
	return rv;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if in book mode, otherwise FALSE
/// \remarks
/// Called from: the App's GetCurrentDocFolderDisplayName(), GetCurrentDocFolderPath(), 
/// GetBookIndicatorStringForCurrentBookFolder(), and GetBookID().
/// Determines if the application is in book mode or not. Simply returns the value in m_bBookMode.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::GetBookMode()
{
	return m_bBookMode != 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: CJoinDialog::OnBnClickedJoinNow(), CMoveDialog::OnBnClickedButtonRenameDoc(),
/// Mover::FinishMove(), CSplitDialog's SplitAtPhraseBoxLocation_Interactive(), and
/// SplitIntoChapters_Interactive().
/// Calls the App's DoAutoSaveDoc().
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::SaveDocChanges()
{
	DoAutoSaveDoc();
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: CMoveDialog::OnBnClickedButtonRenameDoc() and Mover::FinishMove().
/// Calls the View's ClobberDocument() to destroy the current in-memory document structures, then
/// calls the Doc's OnOpenDocument() to open the same document afresh.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::DiscardDocChanges()
{
	CAdapt_ItDoc* pDoc = gpApp->GetDocument();
	CAdapt_ItView* pView = gpApp->GetView();
	pView->ClobberDocument();
	wxString reversionPath = m_curOutputPath;
	wxASSERT(!reversionPath.IsEmpty());
	bool bOK = pDoc->OnOpenDocument(reversionPath);
	// we should get it open safely every time, but just in case...
	if (!bOK)
	{
		// a hard coded message will do, we never expect to see it
		wxMessageBox(_T("DiscardDocChanges() failed on the OnOpenDocument() call after passing in the reversion document's path."),_T(""), wxICON_WARNING);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \remarks
/// Called from: Mover::FinishMove() and CSplitDialog::SplitIntoChapters_Interactive().
/// Calls the View's ClobberDocument().
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::CloseDocDiscardingAnyUnsavedChanges()
{
	GetView()->ClobberDocument();
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a pointer to a SPList of source phrases loaded from the document at FilePath
/// \remarks
/// Called from: CJoinDialog::OnBnClickedJoinNow().
/// Calls the Doc's method of the same name: CAdapt_ItDoc::LoadSourcePhraseListFromFile().
////////////////////////////////////////////////////////////////////////////////////////////
SPList *CAdapt_ItApp::LoadSourcePhraseListFromFile(wxString FilePath)
{
	return CAdapt_ItDoc::LoadSourcePhraseListFromFile(FilePath);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE unless there was a mismatch book ID or an invalid book ID in which case returns FALSE
/// \param      ol                                   <- pointer to the list of source phrases to be appended
/// \param      curBookID                            -> the current book ID
/// \param      IsLastAppendUsingThisMethodRightNow  -> TRUE when being called on the last of the
///                                                     listed files to be appended
/// \remarks
/// Called from: CJoinDialog::OnBnClickedJoinNow().
/// Appends a list of source phrases (from ol) to the current Doc's list of source phrases, effectively
/// joining two documents. Tests to make sure book IDs are valid and that they match, and does other 
/// housekeeping to make sure end markers and sequence numbers are handled properly.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::AppendSourcePhrasesToCurrentDoc(SPList *ol, wxString& curBookID, bool IsLastAppendUsingThisMethodRightNow)
{
	// In AppendSourcePhrasesToCurrentDoc the bool input parameter will be true when this method is being
	// called on the last of the listed files to be appended.
	// BEW modified 15Aug07  (version 3.5.0 is officially dated 7Aaug07, but I did not release it until end of the month
	// and so these 15Aug bug fix changes are in 3.5.0) to move final endmarkers on a null CSourcePhrase to the first
	// CSourcePhrase instance (after removal of the book ID one) in the doc to be joined, to its m_markers member's start
	CAdapt_ItDoc *d = GetDocument();
	CAdapt_ItView *v = GetView();

	// get the current document's count of CSourcePhrase instances, so we can later locate the
	// active location to the first instance at the join location
	int nOldCount = m_pSourcePhrases->GetCount();

	// test for match of book ID codes here, return if they don't match
	wxString appendingDoc_BookID;
	SPList::Node* pos = ol->GetFirst();
	CSourcePhrase* pFirstSrcPhrase = (CSourcePhrase*)pos->GetData();
	wxASSERT(pFirstSrcPhrase);
	if (pFirstSrcPhrase->m_markers.Find(_T("\\id ")) == 0)
	{
		// there is an \id defined, and so we can get it and check for a match
		appendingDoc_BookID = pFirstSrcPhrase->m_srcPhrase; // if it had punctuation it would be invalid
		if (IsValidBookID(appendingDoc_BookID))
		{
			// check if we have matching book IDs
			if (curBookID != appendingDoc_BookID)
			{
				// mismatched
				return FALSE;
			}
			else
			{
				// matched book IDs, so delete the first source phrase in the doc to be appended's list
				ol->DeleteNode(ol->GetFirst()); //ol->RemoveHead();
			}
		}
		else
		{
			// an invalid ID cannot possibly match the passed in bookID, so we have a mismatch
			return FALSE;
		}
	}
	else
	{
		// here is no \id, (the document might not have USFM or SFM markup), so don't make any book ID check
		;
	}

	// BEW added 15Aug07: check for final endmarkers that were moved to the doc end, and if so, remove them
	// and put them back in the m_markers member first (after book ID element is removed) in the joined part
	
	SPList::Node* lastPos = m_pSourcePhrases->GetLast();
	CSourcePhrase* pTailSPhr = lastPos->GetData();
	int backslash_offset = pTailSPhr->m_markers.Find(_T('\\'));
	int asterisk_offset = pTailSPhr->m_markers.Find(_T('*'));
	if (pTailSPhr->m_key.IsEmpty() && pTailSPhr->m_srcPhrase.IsEmpty() && backslash_offset != -1
		&& asterisk_offset != -1)
	{
		// while the test was not perfect, it is almost 100% certain that only an appended CSourcePhrase
		// having a moved endmarker(s) string in its m_markers member would satisfy the test, so assume
		// we have found such a one
		wxString markers = pTailSPhr->m_markers; // don't bother to check its contents
		// WX Note: wxList does not have RemoveTail(). We can do it in a two stage process
		// by using GetLast(), then DeleteNode() [ which is equivalent to MFC RemoveAt()].
		SPList::Node *pLast = m_pSourcePhrases->GetLast();
		m_pSourcePhrases->DeleteNode(pLast);
		delete pTailSPhr;

		// get the CSourcePhrase pointer first in the ol list and insert the markers string at the start
		// of its m_markers member
		SPList::Node* hpos = ol->GetFirst();
		CSourcePhrase* pHeadSPhr = (CSourcePhrase*)hpos->GetData();
		if (pHeadSPhr)
		{
			pHeadSPhr->m_markers = markers + pHeadSPhr->m_markers;
		}
	}
	// Jonathan's code continues here...
	//m_pSourcePhrases->Append(ol);
	// wx doesn't have a wxList method for appending one list onto another list, so we'll do it
	// manually
	SPList::Node* node = ol->GetFirst();
	while (node)
	{
		m_pSourcePhrases->Append(node->GetData());
		node = node->GetNext();
	}
	if (IsLastAppendUsingThisMethodRightNow) 
	{
		// get all the sequence numbers into correct sequence
		d->UpdateSequNumbers(0);

		// BEW modified 07Nov05
		// the document may now be longer than a bundle can display, in fact it may have been that way
		// before the append operation, so we have to ensure we get the indices correct for the currently
		// allowed bundle size, and that we locate the bundle somewhere safely - until we decide otherwise
		// we will locate it at the last join point -- that is, the active location (ie phrase box) will
		// be at the first sourcephrase instance of the just appended document, so that the its preceding
		// context will show the end of the document to which we just appended; the end of the bundle
		// may or may not reach to the end of the combined document (typically it won't) depending on how big
		// these document files happened to be in relation to the bundle size currently in effect
		int nFirstIndex = nOldCount - m_nPrecedingContext;
		if (nFirstIndex < 0) nFirstIndex = 0; // ensure a valid index
		m_nActiveSequNum = SetBundleIndices(m_pSourcePhrases, nFirstIndex);
		CSourcePhrase* pSrcPhrase = v->GetSrcPhrase(m_nActiveSequNum);
		v->Jump(this, pSrcPhrase); // Jump to the last join point, if possible; else to a safe location
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a pointer to the SPList of the current document's source phrases
/// \remarks
/// Called from: CSplitDialog::GoToNextChapter_Interactive().
/// Simply returns the pointer to the App's m_pSourcePhrases member.
////////////////////////////////////////////////////////////////////////////////////////////
SPList *CAdapt_ItApp::GetSourcePhraseList()
{
	return m_pSourcePhrases;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a pointer to the current source phrase (at m_cruIndex)
/// \remarks
/// Called from: the App's CascadeSourcePhraseListChange(), and 
/// CSplitDialog::GoToNextChapter_Interactive().
/// Simply calls GetSourcePhraseByIndex() for the m_curIndex.
////////////////////////////////////////////////////////////////////////////////////////////
CSourcePhrase *CAdapt_ItApp::GetCurrentSourcePhrase()
{
	return GetSourcePhraseByIndex(m_curIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      sp   -> a pointer to a source phrase to jump to
/// \remarks
/// Called from: CSplitDialog::GoToNextChapter_Interactive().
/// Insures that gbIsDocumentSplittingDialogActive is TRUE, then calls the View's Jump()
/// method to jump to the source phrase at sp. Then sets the global back to FALSE. The purpose
/// of the global is so that "the Jump() call will not copy the source text into the phrasebox 
/// if the box lands at a hitherto unadapted (or unglossed) location -- so if the document split 
/// is then made at that location, it won't save into the KB a spurious copy of the source text 
/// as the 'adaptation' at whatever location the box happened to land at."
/// whm comment: A cleaner design would add a bool parameter to the Jump() method to specify
/// whether the source text should be copied, something that would eliminate the use of the
/// global and which might simplify the code at the other locations where Jump() is called.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::SetCurrentSourcePhrase(CSourcePhrase *sp)
{
	// BEW modified 02Nov05, to prevent a copy of the source text word at the new
	// box location being copied to m_targetPhrase when there is no adaptation (or
	// gloss if gloss mode is currently on) already available for that location
	gbIsDocumentSplittingDialogActive = TRUE;
	GetView()->Jump(this, sp);
	gbIsDocumentSplittingDialogActive = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a pointer to a source phrase that starts the next chapter of text or NULL if
///             none found
/// \param      ol   -> the SPList of the document
/// \param      sp   <- the source phrase at the initial and subsequent active search locations
/// \remarks
/// Called from: CSplitDialog::GoToNextChapter_Interactive().
/// Determines which source phrase in the SPList of the current document marks the beginning
/// of the next chapter, i.e., the potential point where that next chapter could be split off.
////////////////////////////////////////////////////////////////////////////////////////////
CSourcePhrase *CAdapt_ItApp::FindNextChapter(SPList *ol, CSourcePhrase *sp)
{
	// Note 1: FindNextChapter includes any section head which precedes the chapter because
	// of the rather fortunate (or deliberate) fact that correct SF markup places the \c
	// marker preceding the \s or \s1 marker of the section head; and even if the section head
	// is specified as to be filtered, it will be stored on the pSrcPhrase instance which
	// has the m_bChapter boolean set TRUE, so correct behaviour is assured provided the
	// next chapter's start is determined by the SplitDialog's button for that purpose.
	// Note 2: In the case where the phrase box is located anywhere in an \id marker's content,
	// we don't want the next chapter to be wrongly set as the chapter immediately following
	// the identification information, but rather at the end of chapter one - so we explicitly
	// check for a TextType different than verse, and advance over it any other text types
	// until we come to a verse TextType and use that as the kick-off point
	SPList::Node* p = ol->Find(sp);
	sp = (CSourcePhrase*)p->GetData();
	p = p->GetNext();	// TODO: check this - doesn't MFC version skip a source phrase before 
						// the while loop below???

	// get over any \id content, or other TextType(s)
	while (sp->m_curTextType != verse)
	{
		sp = (CSourcePhrase*)p->GetData();
		p = p->GetNext();
	}

	// get past any sourcephrase which may start the current rather than next chapter
	//if (p) sp = (CSourcePhrase*)ol->GetNext(p); 
	if (p)
	{
		sp = (CSourcePhrase*)p->GetData();
		p = p->GetNext();
	}
												
	// scan until the next start of chapter location is found
	while (p != NULL && sp->m_bChapter == false) 
	{
		sp = (CSourcePhrase*)p->GetData();
		p = p->GetNext();
	}
	return p ? sp : NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing a filename guaranteed to end with the .xml extension
/// \param      s   -> a wxString representing a file name
/// \remarks
/// Called from: CJoinDialog::OnBnClickedJoinNow(), 
/// CSplitDialog's SplitAtPhraseBoxLocation_Interactive() and DoSplitIntoChapters().
/// Makes sure the file name s has an .xml extension and returns it to the caller.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::ApplyDefaultDocFileExtension(wxString s)
{
	// BEW modified 02Nov05, to include the .xml option, and to make the
	// testing be smarter - in particular, to allow the user to type names like
	// 1Timothy.1-2 without the 1-2 being interpretted as a filename extension;
	// the modifications ensure the returned filename always ends in either
	// .xml or .adt, and hence DoFileSave() will behave robustly
	// whm modified to only use the .xml extension for the wx version.
	wxString defExtn;
	if (gpApp->m_bSaveAsXML)
	{
		defExtn = _T(".xml");
	}
	// wx version doesn't do .adt serialization
	//else
	//{
	//	defExtn = _T(".adt");
	//}
	if (s.Find(_T(".")) == -1) 
	{
		// if there is no point, then we can unlaterally
		// add the default extension
a:		return s + defExtn;
	} 
	else 
	{
		// there is a point character, but it might not be .xml or .adt,
		// and so we must check out if the point is a chapter and/or verse
		// delimiter in the filename - our criterion will be that this
		// is so whenever the filename does not end in .xml or .adt
		wxString xmlext = _T(".xml");
		//wxString adtext = _T(".adt");
		s = MakeReverse(s);
		xmlext = MakeReverse(xmlext);
		//adtext = MakeReverse(adtext);
		if (s.Find(xmlext) == 0) // || s.Find(adtext) == 0)
		{
			// we have a valid extension, so return without doing anything
			s = MakeReverse(s);
			return s;
		}
		else
		{
			// we need to add the current default extension
			s = MakeReverse(s);
			goto a;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      l   -> pointer to a list of source phrases
/// \remarks
/// Called from: CSplitDialog's SplitAtPhraseBoxLocation_Interactive() and 
/// SplitIntoChapters_Interactive().
/// Deletes the source phrases from the list pointed to by l. Does nothing if there
/// are no source phrases in l.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::DeleteSourcePhraseListContents(SPList *l)
{
	// BEW modified 02Nov05, because earlier version leaked memory (so I plagiarized
	// the DeleteSourcePhrases() function in the doc class for safe deletion code)
	CAdapt_ItDoc* pDoc = GetDocument();
	if (!l->IsEmpty())
	{
		// delete all the tokenizations of the source text
		SPList::Node* pos = l->GetFirst();
		while (pos != NULL)
		{
			CSourcePhrase* pSrcPhrase = (CSourcePhrase*)pos->GetData();
			pos = pos->GetNext();
			pDoc->DeleteSingleSrcPhrase(pSrcPhrase);
		}
		l->Clear();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      DoFullScreenUpdate   -> fully updates the screen if TRUE
/// \remarks
/// Called from: CJoinDialog::OnBnClickedJoinNow(), 
/// CSplitDialog's SplitAtPhraseBoxLocation_Interactive() and 
/// SplitIntoChapters_Interactive().
/// Updates sequence numbers and the various indices including: m_curIndex, m_beginIndex,
/// m_endIndex, m_lowerIndex, and m_upperIndex to insure they are within proper ranges.
/// Calls the View's Jump() on the current source phrase to effect any desired screen update.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::CascadeSourcePhraseListChange(bool DoFullScreenUpdate)
{
	CAdapt_ItDoc *d = GetDocument();
	CAdapt_ItView *v = GetView();

	d->UpdateSequNumbers(0);
	m_maxIndex = m_pSourcePhrases->GetCount() - 1;
	if (m_curIndex >= m_maxIndex) {
		m_curIndex = 0;
	}
	if (m_beginIndex >= m_maxIndex) {
		m_beginIndex = 0;
	}
	if (m_endIndex >= m_maxIndex) {
		m_endIndex = m_maxIndex - 1;
	}
	// BEW added 02Nov05, to ensure m_lowerIndex and m_upperIndex are
	// within safe range, given any change to the other indices
	if (m_lowerIndex >= m_curIndex)
		m_lowerIndex = m_beginIndex;
	if (m_upperIndex >= m_endIndex)
		m_upperIndex = m_endIndex - wxMin(m_nFollowingContext,(m_endIndex - m_beginIndex)/10);

	if (DoFullScreenUpdate) {
		v->Jump(this, GetCurrentSourcePhrase());

		// TODO Or is this better than Jump?  v->RedrawEverything(0);
		// BEW - I think Jump is fine, at least until we find a problem due to it
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a pointer to the source phrase found by index value
/// \param      Index   -> an int representing the index of the source phrase in m_pSourcePhrases
/// \remarks
/// Called from: the App's GetCurrentSourcePhrase() and SetCurrentSourcePhraseByIndex().
/// Gets a pointer to the source phrase instance at Index in m_pSourcePhrases.
/// whm: Does not do any error checking so it assumes that Index is a valid index into the list,
/// i.e., within the list.
////////////////////////////////////////////////////////////////////////////////////////////
CSourcePhrase *CAdapt_ItApp::GetSourcePhraseByIndex(int Index)
{
	SPList *ol = m_pSourcePhrases;
	SPList::Node* node = ol->Item(Index);
	return (CSourcePhrase*)node->GetData();
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      Index   -> an int representing the index of the source phrase to Jump to
/// \remarks
/// Called from: CSplitDialog::SplitAtPhraseBoxLocation_Interactive().
/// Calls the View's Jump() method to jump to the source phrase instance at Index in m_pSourcePhrases.
/// whm: Does not do any error checking so it assumes that Index is a valid index into the list,
/// i.e., within the list.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::SetCurrentSourcePhraseByIndex(int Index)
{
	gbIsDocumentSplittingDialogActive = TRUE; // suppress source text copy if the box location is empty
	GetView()->Jump(this, GetSourcePhraseByIndex(Index));
	gbIsDocumentSplittingDialogActive = FALSE;
}

// additional getters, setters or helpers added by BEW on 03Nov05 or later

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     TRUE if id is a valid book ID, otherwise FALSE
/// \param      id   -> the book ID being tested for validity
/// \remarks
/// Called from: the App's AppendSourcePhrasesToCurrentDoc() and GetBookIDFromDoc().
/// Determines if the string represented by id is a valid book ID contained in the
/// m_pBibleBooks list of pointers to BookNamePair structs. It is valid if it is the
/// same as a bookCode member in the list.
////////////////////////////////////////////////////////////////////////////////////////////
bool CAdapt_ItApp::IsValidBookID(wxString& id)
{
	wxArrayPtrVoid* pBooks = this->m_pBibleBooks;
	int nArraySize = pBooks->GetCount();
	int index;
	bool bFoundIt = FALSE;
	for (index = 0; index < nArraySize; index++)
	{
		if (id == ((BookNamePair*)pBooks->Item(index))->bookCode)
		{
			bFoundIt = TRUE;
			break;
		}
	}
	return bFoundIt != FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the document's book ID if present and if valid, otherwise
///             a null string
/// \remarks
/// Called from: the App's GetBookID() if the GetBookMode() call returns FALSE.
/// Looks as the first source phrase instance in the document (which should be the one to have
/// any "\id" marker in its m_markers member). If the \id marker exists, it examines the next word
/// of the \id line checking there for a valid 3-letter book ID name. If a valid book ID is found
/// it returns it, otherwise it returns a null string.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetBookIDFromDoc()
{
	// get BookIDFromDoc looks at the first CSourcePhrase instance in the document, which should be
	// one with a "\id " m_markers content, and its m_key member should store the book ID we want.
	// It is possible the source text file from which the doc was created lacked an initial \id field,
	// or if it had one, the book ID might contain a typo, so we check for validity of the ID code and
	// that it has a length of 3 (characters) returning an empty string if invalid, no matter what the
	// string may have been; or returning the code if it is a valid one
	wxString id;
	id.Empty();
	SPList::Node* pos = m_pSourcePhrases->GetFirst();
	CSourcePhrase* pSrcPhrase = (CSourcePhrase*)pos->GetData();
	wxASSERT(pSrcPhrase);
	if (pSrcPhrase->m_markers.Find(_T("\\id ")) == -1)
	{
		// there is no \id field
		return id;
	}
	id = pSrcPhrase->m_key;
	if (id.Length() != 3)
	{
		// string does not qualify for a valid ID because its length is not 3
		id.Empty();
		return id;
	}
	bool bIsValid = this->IsValidBookID(id);
	if (!bIsValid)
	{
		// it's not one of the IDs defined in the books.xml file
		id.Empty();
		return id;
	}
	return id;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the document's book ID if present and if valid, otherwise
///             a null string
/// \remarks
/// Called from: CJoinDialog::InitDialog(), CSplitDialog's SplitAtPhraseBoxLocation_Interactive()
/// and DoSplitIntoChapters().
/// If GetBookMode() returns TRUE, this function calls GetBookIndicatorStringForCurrentBookFolder(),
/// otherwise it calls GetBookIdFromDoc().
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::GetBookID()
{
	wxString id;
	id = GetBookMode() ? this->GetBookIndicatorStringForCurrentBookFolder() : this->GetBookIDFromDoc();
	return id;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pSrcPhrasesList   <- pointer to the SPList of a document
/// \param      id                -> a wxString representing the book ID to be added
/// \remarks
/// Called from: CSplitDialog's SplitAtPhraseBoxLocation_Interactive() and DoSplitIntoChapters().
/// Makes sure that the document has the correct \id entry at its beginning after a document split 
/// operation. To effect this this function adds a pointer to a new source phrase instance at the 
/// head of the pSrcPhrasesList, giving it an "\id " string in its m_markers member. The id 
/// is in m_key and m_srcPhrase, and an appropriate navigation text string is placed in m_inform
/// by calling RedoNavigationText(). 
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::AddBookIDToDoc(SPList* pSrcPhrasesList, wxString id)
{
	// AddBookIDToDoc addes a pointer to a new pSrcPhrase instance at the head of the pSrcPhrasesList
	// which is passed in, giving it an "\id " string in m_markers, the id in m_key and m_srcPhrase,
	// and an appropriate navigation text string in m_inform -- this function is used within the Split
	// functionality to ensure that what remains when the doc is split still has the correct \id entry
	// at its beginning
	CAdapt_ItDoc* pDoc = this->GetDocument();
	wxASSERT(!id.IsEmpty());
	CSourcePhrase* pSrcPhrase = new CSourcePhrase;
	pSrcPhrase->m_markers = _T("\\id ");
	pSrcPhrase->m_key = id;
	pSrcPhrase->m_srcPhrase = id;
	pSrcPhrase->m_curTextType = identification;

	// update the navigation text & relevant flags
	pSrcPhrase->m_inform = pDoc->RedoNavigationText(pSrcPhrase);
	pSrcPhrase->m_bSpecialText = TRUE;
	pSrcPhrase->m_bFirstOfType = TRUE;

	// insert this instance at the start of the document
	SPList::Node* pos = pSrcPhrasesList->GetFirst(); //POSITION pos = pSrcPhrasesList->GetHeadPosition();
	pos = pSrcPhrasesList->Insert(pos,pSrcPhrase);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     the index for the active location
/// \param      pList                   -> the SPList for the document
/// \param      nFirstSequNumInBundle   -> an int representing the first sequence number of the needed 
///                                         bundle
/// \remarks
/// Called from: the App's AppendSourcePhrasesToCurrentDoc().
/// SetBundleIndices takes a pointer to the m_pSourcePhrases list in the document, and an index
/// to the CSourcePhrase instance in that list which is to be the first one of the bundle needed
/// to be computed after a Join operation (since the join may have created a document bigger than
/// a single bundle, even if not so previously), and returns the index for the active location
/// (which typically is m_nPrecedingContext sourcephase instances further along than the 
/// nFirstSequNumInBundle value). The caller can then do a GetSrcPhrase() call on the list using the
/// returned index to get the active CSourcePhrase instance there, and then Jump() can be used
/// to set up a correctly laid out bundle with the phrase box at the desired location. For the
/// phrase box to be at the last join location, the caller should do a little simple arithmetic to
/// work out the correct nFirstSequNumInBundle value to produce the wanted result.
////////////////////////////////////////////////////////////////////////////////////////////
int CAdapt_ItApp::SetBundleIndices(SPList* pList, int nFirstSequNumInBundle)
{
	CAdapt_ItView *v = GetView();

	int nBundleSize = m_nMaxToDisplay; // current limit
	int nCurMaxIndex = pList->GetCount() - 1; // index to last CSourcePhrase in list
	int nActiveLoc = nFirstSequNumInBundle + m_nPrecedingContext;

	// do some reality checking before we commit to the active location index value
	if (nCurMaxIndex < nBundleSize)
	{
		v->CalcInitialIndices();
		if (m_nPrecedingContext < nCurMaxIndex)
			return m_nPrecedingContext + 1;
		else
			return 0;
	}
	else
	{
		// the document is longer than a single bundle
		if (nActiveLoc > nCurMaxIndex)
		{
			// the join location is too close to the end of the document to allow the
			// normal preceding context, so put the box at the join location instead
			nActiveLoc = nFirstSequNumInBundle; // we know this index is okay, as we must have appended something!
		}
		// calculate a valid bundle
		v->CalcIndicesForAdvance(nActiveLoc);
	}
	return nActiveLoc;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      project   -> the project name in the form of "srcName to tgtName Adaptations"
/// \param      srcName   <- a wxString that receives the source name of the project name
/// \param      tgtName   <- a wxString that receives the target name of the project name
/// \remarks
/// Called from: the Doc's RestoreDocParamsOnInput() and OnFilePackDoc().
/// GetSrcAndTgtLanguageNamesFromProjectName takes the current project name as stored in the app's
/// m_curProjectName member, and extracts from it the source and target language names (these may not actaully
/// be language names, they are potentially arbitrary text strings). The project name is in the form
/// "srcName to tgtName Adaptations" where srcName and tgtName represent the strings that we want to get.
/// We use this in the document's RestoreDocParamsOnInput() function, so we can check whether we are restoring
/// a document from the currently open project or not; it will be so if these extracted names match the
/// m_sourceName and m_targetName values on the app class as determined by RestoreDocParamsOnInput() from the
/// information stored within the document file itself (whether XML or binary). This is necessary in order for
/// the latter function to restore the correct settings for the book mode (whether on or off) and book index 
/// whether (a positive value or -1), since we can have opened the document either via the Start Working
/// wizard, or the MRU list, and these two options give different results - the former opens only within a
/// known project, the latter opens any document of any project. For additional extended comments, see the
/// comments within RestoreDocParamsOnInput() itself.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::GetSrcAndTgtLanguageNamesFromProjectName(wxString& project, wxString& srcName, wxString& tgtName)
{
	// BEW modified 27Nov05 because the old algorithm failed for project names which include multi-space names
	if (project.IsEmpty())
	{
		srcName.Empty();
		tgtName.Empty();
		return;
	}
	// whm: For localization purposes the " to " and " adaptations" strings should not be 
	// translated, otherwise other localizations would not be able to handle the unpacking 
	// of files created on different localizations.
	int curPos = project.Find(_T(" to "));
	srcName = project.Left(curPos);
	curPos += 4; // where we think the target name starts
	int curPos2 = FindFromPos(project,_T(" adaptations"),curPos); // find the end of the target name
	int count = curPos2 - curPos;
	wxASSERT(count > 0);
	tgtName = project.Mid(curPos,count);
}

// Helper function
////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      font   -> pointer to the font from which the attributes are taken
/// \param      fInfo  <- the fontInfo struct whose members get updated
/// \remarks
/// Called from: the App's InitializeFonts(), CFontPageWiz's OnWizardPageChanging() when
/// moving forward, and CFontPagePrefs::OnOK().
/// Queries the font for its characteristics and stores the appropriate values in the fontInfo
/// struct. The attributes that are stored are the font's point size (fHeight), weight (fWeight),
/// style (fItalic), underlined (fUnderline), encoding (fEncoding), charset (mapped from wx values),
/// family (bit mapped to fPitchAndFamily) and face name (fFaceName).
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::UpdateFontInfoStruct(wxFont* font, fontInfo& fInfo)
{
	// This helper function is used to set the fontInfo struct
	// members (fPointSize, fWeight, etc). When the user changes
	// the font or font characteristic from within the program
	// this function will get called to update the fontInfo structs
	// which are used to update the font info values in the config
	// files. This function will become deprecated if/when the 
	// wxWidgets version does away entirely with fontInfo structs.
	fInfo.fHeight = PointSizeToNegHeight(font->GetPointSize());
	//fInfo.fWidth = 0; // font width ignored in wxWidgets, so leave current value
	//fInfo.fEscapement = 0; // escapement ignored in wxWidgets, so leave current value
	//fInfo.fOrientation = 0; // orientation ignored in wxWidgets, so leave current value
	switch(font->GetWeight())
	{
		case wxLIGHT: 
			fInfo.fWeight = 400; 
			fInfo.fWeightConfSave = 400; 
			break; // same as normal
		case wxNORMAL: 
			fInfo.fWeight = 400; 
			fInfo.fWeightConfSave = 400;  
			break;
		case wxBOLD: 
			fInfo.fWeight = 700; 
			fInfo.fWeightConfSave = 700;  
			break;
		default: 
			fInfo.fWeight = 700;  
			fInfo.fWeightConfSave = 700;
	}
	if (font->GetStyle() == wxNORMAL)
        fInfo.fItalic = 0;
	else if (font->GetStyle() == wxITALIC || font->GetStyle() == wxSLANT)
        fInfo.fItalic = 1;
	if (font->GetUnderlined())
        fInfo.fUnderline = 1;
	else
        fInfo.fUnderline = 0;
	//fInfo.fStrikeOut = 0; // strikeout ignored in wxWidgets, so leave current value
	// Note: wxWidgets does use wxFontEncoding which can be retrieved with GetEncoding()
	fInfo.fEncoding = font->GetEncoding();
	fInfo.fCharset = MapWXFontEncodingToMFCCharset(fInfo.fEncoding); // the wx encodings that MFC doesn't know about map to 0 (zero)
	//fInfo.fOutPrecision = 1; // outprecision ignored in wxWidgets, so leave current value
	//fInfo.fClipPrecision = 2; // clipprecision ignored in wxWidgets, so leave current value
	//fInfo.fQuality = 2; // quality ignored in wxWidgets, so leave current value
	switch(font->GetFamily())
	{
		case wxROMAN: fInfo.fFamily = 1; break;     //1 = FF_ROMAN (MFC)
		case wxSWISS: fInfo.fFamily = 2; break;     //2 = FF_SWISS (MFC)
		case wxMODERN: fInfo.fFamily = 3; break;    //3 = FF_MODERN (MFC)
		case wxSCRIPT: fInfo.fFamily = 4; break;    //4 = FF_SCRIPT (MFC)
		case wxDECORATIVE: fInfo.fFamily = 5; break;//5 = FF_DECORATIVE (MFC)
		case wxDEFAULT: fInfo.fFamily = 0; break;   //0 = FF_DONTCAR (MFC)
		default: fInfo.fFamily = 0;
	}
	// wxWidgets is only interested in the family which is encoded in bits 4-7
	// of the fPitchAndFamily value. The Pitch is encoded in the two lower order
	// bits of fPitchAndFamily (bits 1 and 2), and can be (MFC) DEFAULT_PITCH, 
	// FIXED_PITCH, or VARIABLE_PITCH. MFC defines determine these to be:
	// #define DEFAULT_PITCH 0
	// #define FIXED_PITCH 1
	// #define VARIABLE_PITCH 2
	// According to the MFC docs "The proper value can be obtained by using the 
	// Boolean OR operator to join one pitch constant with one family constant."
	// wxFont does not track font pitch, so we'll assume a default pitch (0) 
	// then we should be able to get the fPitchAndFamily by shifting the fFamily 
	// value 3 bits to the left.
	fInfo.fPitchAndFamily = fInfo.fFamily << 4;
	fInfo.fFaceName = font->GetFaceName();
}

// helper function
////////////////////////////////////////////////////////////////////////////////////////////
/// \return     an int representing the calculated optimal height for a dialog's text control
/// \param      pointSize  -> an int representing the font point size
/// \remarks
/// Called from: (currently unused)
/// Calculates an estimated optimal value for the height of a text crontrol by applying a
/// ratio to the dialog font's point size.
////////////////////////////////////////////////////////////////////////////////////////////
int	 CAdapt_ItApp::GetOptimumDlgEditBoxHeight(int pointSize)
{
	// For 12pt font the default height of a text ctrl is about 20 pixels. 
	// With the following ratio a 24pt font would need a text ctrl with 
	// height of about 40 pixels.
	int intTemp = (int)(22.0 + ((float)pointSize - MIN_DLG_FONT_SIZE)*1.667);
	return intTemp;
}

// helper function
////////////////////////////////////////////////////////////////////////////////////////////
/// \return     an int representing the font size constrained to the defined limits
/// \param      pointSize  -> an int representing the font point size
/// \remarks
/// Called from: (currently unused)
/// Limits a text control's font size to the range specified by MIN_DLG_FONT_SIZE and
/// MAX_DLG_FONT_SIZE.
////////////////////////////////////////////////////////////////////////////////////////////
int CAdapt_ItApp::GetOptimumDlgFontSize(int pointSize)
{
	// Limit Optimum dialog text edit box font size to 
	// between MIN_DLG_FONT_SIZE (11 points) and
	// MAX_DLG_FONT_SIZE (24 points)
	int ps = pointSize;
	if (ps < MIN_DLG_FONT_SIZE)
		ps = MIN_DLG_FONT_SIZE;
	if (ps > MAX_DLG_FONT_SIZE)
		ps = MAX_DLG_FONT_SIZE;
	return ps;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the path of the currently executing program
///                 or an empty/null string if the path could not be determined
/// \param      argv0            -> a wxString representing the system's argv[0] parameter (to Main)
/// \param      cwd              -> a wxString representing the current working directory (value 
///                                     returned by wxGetCwd)
/// \param      appVariableName  -> an (optional) application environment variable name
/// \remarks
/// Called from: the App's OnInit().
/// Gets an absolute path for the currently executing program, or a null string if the path could
/// not be determined. This function also stores the argv0 parameter in the App's 
/// m_executingAppPathName member.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::FindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName) 
{
	// whm added 5Jan04 FindAppPath() from suggestion by Julian Smart in wxWidgets 
	// docs re "Writing installers for wxWidgets applications"
    wxString str;

    // Try appVariableName
    if (!appVariableName.IsEmpty())
    {
        str = wxGetenv(appVariableName);
        if (!str.IsEmpty())
            return str;
    }

#if defined(__WXMAC__) && !defined(__DARWIN__)
    // On Mac, the current directory is the relevant one when
    // the application starts.
    return cwd;
#endif

	// whm added 31July06 to store the path and file name of the executing app
	m_executingAppPathName = argv0;

    if (wxIsAbsolutePath(argv0))
        return wxPathOnly(argv0);
    else
    {
        // Is it a relative path?
        wxString currentDir(cwd);
        if (currentDir.Last() != wxFILE_SEP_PATH)
            currentDir += wxFILE_SEP_PATH;

        str = currentDir + argv0;
        if (wxFileExists(str))
            return wxPathOnly(str);
    }

    // OK, it's neither an absolute path nor a relative path.
    // Search PATH.

    wxPathList pathList;
    pathList.AddEnvList(wxT("PATH"));
    str = pathList.FindAbsoluteValidPath(argv0);
    if (!str.IsEmpty())
        return wxPathOnly(str);

    // Failed
    return wxEmptyString;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxString representing the file name with any .xml extension removed
/// \param      anyName   -> a wxString representing a file name
/// \remarks
/// Called from: the Doc's SetDocumentWindowTitle(), and CMoveDialog::OnBnClickedButtonRenameDoc().
/// Removes any .xml extension occurring at the end of anyName and returns the result.
////////////////////////////////////////////////////////////////////////////////////////////
wxString CAdapt_ItApp::MakeExtensionlessName(wxString anyName)
{
	// whm Note: this could be done more efficiently using wxFileName, but I'll
	// follow the MFC coding here
	wxString name = anyName;
	wxString extn;
	name = MakeReverse(name);
	// beware, there could be . in the name which is not for an extension
	int nFound = name.Find(_T('.'));
	if (nFound >= 0)
	{
		// there may be an extension to be removed
		extn = name.Left(nFound); // a potential extension
		int len = extn.Length();
		if (len == 3)
		{
			// it could be an extension, so check further
			extn = MakeReverse(extn);
			extn.MakeLower();
			if (extn == _T("xml")) // || extn == _T("adt"))
			{
				// it's one of ours, so remove it and the .
				name = name.Mid(nFound + 1);
			}
		}
	}
	name = MakeReverse(name);
	return name;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     nothing
/// \param      pFont      -> a pointer to the font from which its base properties are copied in setting the pDlgFont
/// \param      pEdit1     <- a pointer to the first text control whose font that can be set
/// \param      pEdit2     <- a pointer to the second text control whose font that can be set
/// \param      pListBox1  <- a pointer to the first list box control whose font that can be set
/// \param      pListBox2  <- a pointer to the second list box control whose font that can be set
/// \param      pDlgFont   <- the dialog font being adjusted for use in the above dialog controls
/// \param      bIsRTL     -> TRUE if the control is Right-to-Left (RTL), FALSE if the control is Left-to-Right (LTR)
/// \remarks
/// Called from: The InitDialog() methods of most of the application's dialogs that use source 
/// or target language text within text controls or list boxes.
/// SetFontAndDirectionalityForDialogControl sets the dialog's font and script directionality for up
/// two two edit boxes and/or two list boxes at a time -- the pFont pointer passed in is used for each.
/// To set a mixture of such controls to different fonts and directionalities, according to the app's
/// settings for source, target and navigation text fonts, repeat the calls with varying font pointers.
/// The desired directionality is specified by the bIsRTL, which should be TRUE when right to left
/// reading text is wanted (and right justification), default is FALSE for left to right.
/// Use NULL for any control which is to be ignored, but the font pointer must never be NULL.
////////////////////////////////////////////////////////////////////////////////////////////
void CAdapt_ItApp::SetFontAndDirectionalityForDialogControl(wxFont* pFont, wxTextCtrl* pEdit1, 
					wxTextCtrl* pEdit2, wxListBox* pListBox1, wxListBox* pListBox2, 
					wxFont*& pDlgFont, bool bIsRTL)
{
	//
	// whm Note: The MFC function deletes any existing dialog font that is passed in; then creates the 
	// dialog font anew by first getting the pFont's LOGFONT data (stored in navLF), and using that log
	// font data calls CreateFontIndirect(&navLF) to create a new pDlgFont. If the attempt to get the
	// log font data from pFont fails, it creates a new pApp->m_pNavTextFont using a Windows SYSTEM_FONT, 
	// then uses that font at 12 point size as input to SetFont for the edit and/or listbox controls' font.
	// The WX version does not need to delete the existing dialog font and recreate it for the dialog.
	// Instead, the wx version simply assigns the pFont characteristics to the dialog font pDlgFont, but
	// sets its size to pApp->m_dialogFontSize. 
	wxASSERT(pFont != NULL);
	wxASSERT(pDlgFont != NULL);
	CopyFontBaseProperties(pFont,pDlgFont);
	// The CopyFontBaseProperties function above doesn't copy the point size, so 
	// make the dialog font show in the proper dialog font size.
	pDlgFont->SetPointSize(m_dialogFontSize);
	if (pEdit1 != NULL)
		pEdit1->SetFont(*pDlgFont);
	if (pEdit2 != NULL)
		pEdit2->SetFont(*pDlgFont);
	if (pListBox1 != NULL)
		pListBox1->SetFont(*pDlgFont);
	if (pListBox2 != NULL)
		pListBox2->SetFont(*pDlgFont);

	bIsRTL = bIsRTL; // suppresses "unreferenced formal parameter" warning in ANSI version
	// add RTL support for Unicode version
#ifdef _RTL_FLAGS
	if (pEdit1 != NULL)
	{
		if (bIsRTL)
		{
			pEdit1->SetLayoutDirection(wxLayout_RightToLeft);
		}
		else
		{
			pEdit1->SetLayoutDirection(wxLayout_LeftToRight);
		}
	}
	if (pEdit2 != NULL)
	{
		if (bIsRTL)
		{
			pEdit2->SetLayoutDirection(wxLayout_RightToLeft);
		}
		else
		{
			pEdit2->SetLayoutDirection(wxLayout_LeftToRight);
		}
	}
	if (pListBox1 != NULL)
	{
		if (bIsRTL)
		{
			pListBox1->SetLayoutDirection(wxLayout_RightToLeft);
		}
		else
		{
			pListBox1->SetLayoutDirection(wxLayout_LeftToRight);
		}
	}
	if (pListBox2 != NULL)
	{
		if (bIsRTL)
		{
			pListBox2->SetLayoutDirection(wxLayout_RightToLeft);
		}
		else
		{
			pListBox2->SetLayoutDirection(wxLayout_LeftToRight);
		}
	}
	#endif
}

// wx version note: This function is not needed because the wx version of SetFontAndDirectionalityForDialogControl
// does not clobber anything as does the MFC version. 
//void CAdapt_ItApp::setFontAndDirectionalityForDialogControl_DlgFontPredefined(wxTextCtrl* pEdit1, wxTextCtrl* pEdit2,
//				 wxListBox* pListBox1,wxListBox* pListBox2, wxFont*& pDlgFont, bool bIsRTL)

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     an int representing the index into the array where findStr was found, otherwise
///                 -1 if the string was not found.
/// \param      findStr   -> a wxString representing the (exact) string being searched for
/// \param      strArray  <- a pointer to the wxArrayString being searched
/// \remarks
/// Called from: CExportOptionsDlg's OnBnClickedRadioExportSelectedMarkers(), OnLbnSelchangeListSfms(),
/// OnCheckListBoxToggle(), OnBnClickedButtonFilterOutFromExport(), OnBnClickedButtonIncludeInExport(),
/// OnBnClickedButtonUndo(), CFilterPageCommon's CheckListBoxToggleFactory(), 
/// DoBoxClickedIncludeOrFilterOutDoc() and DoBoxClickedIncludeOrFilterOutProj().
/// This function does a brute force linear search through strArray looking for findStr. If 
/// found (array element == findStr exactly) it returns the found element's index in the array, 
/// otherwise it returns -1.
////////////////////////////////////////////////////////////////////////////////////////////
int CAdapt_ItApp::FindArrayString(const wxString& findStr, wxArrayString* strArray)
{
	int ct;
	for (ct = 0; ct < (int)strArray->GetCount(); ct++)
	{
		if (strArray->Item(ct) == findStr)
		{
			return ct;
		}
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     an int representing the index of the list box item if found, otherwise -1
/// \param      pListBox         -> a pointer to the list box being searched
/// \param      searchStr        <- a wxString representing the string being searched for
/// \param      searchCaseType   -> a SearchCaseType which can be caseSensitive or caseInsensitive
/// \param      searchStrLenType <- a SearchStrLengthType which can be subString or exactString
/// \remarks
/// Called from: CChooseTranslation's OnButtonMoveUp(), OnButtonMoveDown(), 
/// OnSelchangeListboxTranslations(), OnDblclkListboxTranslations(), OnButtonRemove(),
/// InitDialog(), CConsistencyCheckDlg::InitDialog(), 
/// CFilterPageCommon::LoadListBoxFromArrays(), CKBEditor's OnSelchangeListSrcKeys(),
/// OnSelchangeListExistingTranslations(), OnUpdateEditSrcKey(), OnButtonUpdate(),
/// OnAddNoAdaptation(), OnButtonAdd(), OnButtonRemove(), OnButtonMoveUp(),OnButtonMoveDown(),
/// and LoadDataForPage().
/// Finds the index of an item in pListBox that matches the searchStr and the case and and/or
/// substring type. Returns the index of the item if found, or -1 if not found. This function
/// adds more search options and search criteria than what is available in the standard wxListBox
/// which can do case sensitive or insensitive searches but cannot do substring searches with its 
/// FindString() method.
////////////////////////////////////////////////////////////////////////////////////////////
int CAdapt_ItApp::FindListBoxItem(wxListBox* pListBox, wxString searchStr, 
		enum SearchCaseType searchCaseType, enum SearchStrLengthType searchStrLenType)
{
	// FindListBoxItem searches for searchStr in pListBox and returns the zero-based
	// index of a found item, or -1 if not found. If searchType is caseSensitive, it will
	// return the index of the first exact (case sensitive) match found; if searchCaseType is
	// caseInsensitive, it will return the index of the first match found that may differ
	// in case; if searchStrLenType is subString, it will return the index of the first item
	// with a matching substring.
	int ct;
	bool bFound = FALSE;
	wxString caseKeyStr = searchStr;
	if (searchCaseType == caseInsensitive)
		caseKeyStr.UpperCase();
	wxString srcStr;
	for (ct = 0; ct < (int)pListBox->GetCount(); ct++)
	{
		srcStr = pListBox->GetString(ct);
		if (searchCaseType == caseInsensitive)
			srcStr.UpperCase();
		if (searchStrLenType == subString)
		{
			// If needed we could here further modify FindListBoxItem to allow
			// for a substring at any location in the string, not just initially
			// It would require an additional enum parameter SubStrType {initialOnly, anywhere}
			if (srcStr.Find(caseKeyStr) == 0)
			{
				// we found an item whose beginning chars match 
				bFound = TRUE;
				return ct;
			}
		}
		else
		{
			// searchStrLenType == exactString
			if (srcStr == caseKeyStr)
			{
				// we found an item whose chars match exactly
				bFound = TRUE;
				return ct;
			}
		}
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     an int representing an MFC paper size code value equivalent to the wxPaperSize id
/// \param      id   -> the wxPaperSize id to be mapped to an MFC value
/// \remarks
/// Called from: the App's OnFilePageSetup().
/// Maps a wxPaperSize value to the equivalent MFC paper size value.
////////////////////////////////////////////////////////////////////////////////////////////
int CAdapt_ItApp::MapWXtoMFCPaperSizeCode(wxPaperSize id)
{
	switch(id)
	{
	case wxPAPER_NONE: return 9;	//	0       // Use specific dimensions
	case wxPAPER_LETTER: return 1;  // Letter, 8 1/2 by 11 inches
	case wxPAPER_LEGAL: return 5;  // Legal, 8 1/2 by 14 inches
    case wxPAPER_A4: return 9;  // A4 Sheet, 210 by 297 millimeters
    case wxPAPER_CSHEET: return 24; // C Sheet, 17 by 22 inches
    case wxPAPER_DSHEET: return 25; // D Sheet, 22 by 34 inches
    case wxPAPER_ESHEET: return 26; // E Sheet, 34 by 44 inches
    case wxPAPER_LETTERSMALL: return 2;  // Letter Small, 8 1/2 by 11 inches
    case wxPAPER_TABLOID: return 3;  // Tabloid, 11 by 17 inches
    case wxPAPER_LEDGER: return 4;  // Ledger, 17 by 11 inches
    case wxPAPER_STATEMENT: return 6;  // Statement, 5 1/2 by 8 1/2 inches
    case wxPAPER_EXECUTIVE: return 7;  // Executive, 7 1/4 by 10 1/2 inches
    case wxPAPER_A3: return 8;  // A3 sheet, 297 by 420 millimeters
    case wxPAPER_A4SMALL: return 9; // A4 small sheet, 210 by 297 millimeters // whm changed from return 10 to return 9
    case wxPAPER_A5: return 11; // A5 sheet, 148 by 210 millimeters
    case wxPAPER_B4: return 12; // B4 sheet, 250 by 354 millimeters
    case wxPAPER_B5: return 13; // B5 sheet, 182-by-257-millimeter paper
    case wxPAPER_FOLIO: return 14; // Folio, 8-1/2-by-13-inch paper
    case wxPAPER_QUARTO: return 15; // Quarto, 215-by-275-millimeter paper
    case wxPAPER_10X14: return 16; // 10-by-14-inch sheet
    case wxPAPER_11X17: return 17; // 11-by-17-inch sheet
	// For some unknown reason, selecting "LETTER" in a Windows Page Setup dialog, the return code is
	// 18 wxPAPER_NOTE, which is the same size as wxPAPER_LETTER. I've changed the return value for
	// wxPAPER_NOTE to 1 (wxPAPER_LETTER) which should make things work better.
    case wxPAPER_NOTE: return 1; // Note, 8 1/2 by 11 inches
    case wxPAPER_ENV_9: return 19; // #9 Envelope, 3 7/8 by 8 7/8 inches
    case wxPAPER_ENV_10: return 20; // #10 Envelope, 4 1/8 by 9 1/2 inches
    case wxPAPER_ENV_11: return 21; // #11 Envelope, 4 1/2 by 10 3/8 inches
    case wxPAPER_ENV_12: return 22; // #12 Envelope, 4 3/4 by 11 inches
    case wxPAPER_ENV_14: return 23; // #14 Envelope, 5 by 11 1/2 inches
    case wxPAPER_ENV_DL: return 27; // DL Envelope, 110 by 220 millimeters
    case wxPAPER_ENV_C5: return 28; // C5 Envelope, 162 by 229 millimeters
    case wxPAPER_ENV_C3: return 29; // C3 Envelope, 324 by 458 millimeters
    case wxPAPER_ENV_C4: return 30; // C4 Envelope, 229 by 324 millimeters
    case wxPAPER_ENV_C6: return 31; // C6 Envelope, 114 by 162 millimeters
    case wxPAPER_ENV_C65: return 32; // C65 Envelope, 114 by 229 millimeters
    case wxPAPER_ENV_B4: return 33; // B4 Envelope, 250 by 353 millimeters
    case wxPAPER_ENV_B5: return 34; // B5 Envelope, 176 by 250 millimeters
    case wxPAPER_ENV_B6: return 35; // B6 Envelope, 176 by 125 millimeters
    case wxPAPER_ENV_ITALY: return 36; // Italy Envelope, 110 by 230 millimeters
    case wxPAPER_ENV_MONARCH: return 37; // Monarch Envelope, 3 7/8 by 7 1/2 inches
    case wxPAPER_ENV_PERSONAL: return 38; // 6 3/4 Envelope, 3 5/8 by 6 1/2 inches
    case wxPAPER_FANFOLD_US: return 39; // US Std Fanfold, 14 7/8 by 11 inches
    case wxPAPER_FANFOLD_STD_GERMAN: return 40; // German Std Fanfold, 8 1/2 by 12 inches
    case wxPAPER_FANFOLD_LGL_GERMAN: return 41; // German Legal Fanfold, 8 1/2 by 13 inches
// Windows 95 only:
// whm: For the values below I've only estimated what seems
    case wxPAPER_ISO_B4: return 12; // B4 (ISO) 250 x 353 mm // whm: same as wxPAPER_B4
    case wxPAPER_JAPANESE_POSTCARD: return 11; // Japanese Postcard 100 x 148 mm // no equiv just map to wxPAPER_A5
    case wxPAPER_9X11: return 16; // 9 x 11 in // no equiv just map to wxPAPER_10X14
    case wxPAPER_10X11: return 16; // 10 x 11 in // no equiv just map to wxPAPER_10X14
    case wxPAPER_15X11: return 4;  // 15 x 11 in // no equiv just map to wxPAPER_LEDGER
    case wxPAPER_ENV_INVITE: return 1;  // Envelope Invite 220 x 220 mm // no equiv just map to wxPAPER_LETTER
    case wxPAPER_LETTER_EXTRA: return 1;  // Letter Extra 9 \275 x 12 in // no equiv just map to wxPAPER_LETTER
    case wxPAPER_LEGAL_EXTRA: return 5;  // Legal Extra 9 \275 x 15 in // no equiv just map to wxPAPER_LEGAL
    case wxPAPER_TABLOID_EXTRA: return 3;  // Tabloid Extra 11.69 x 18 in // no equiv just map to wxPAPER_TABLOID
    case wxPAPER_A4_EXTRA: return 9;  // A4 Extra 9.27 x 12.69 in // no equiv just map to wxPAPER_A4
    case wxPAPER_LETTER_TRANSVERSE: return 1;  // Letter Transverse 8 \275 x 11 in // no equiv just map to wxPAPER_LETTER
    case wxPAPER_A4_TRANSVERSE: return 9;  // A4 Transverse 210 x 297 mm // no equiv just map to wxPAPER_A4
    case wxPAPER_LETTER_EXTRA_TRANSVERSE: return 1;  // Letter Extra Transverse 9\275 x 12 in // no equiv just map to wxPAPER_LETTER
    case wxPAPER_A_PLUS: return 9;  // SuperA/SuperA/A4 227 x 356 mm // no equiv just map to wxPAPER_A4
    case wxPAPER_B_PLUS: return 8;  // SuperB/SuperB/A3 305 x 487 mm // no equiv just map to wxPAPER_A3
    case wxPAPER_LETTER_PLUS: return 1;  // Letter Plus 8.5 x 12.69 in // no equiv just map to wxPAPER_LETTER
    case wxPAPER_A4_PLUS: return 9;  // A4 Plus 210 x 330 mm // no equiv just map to wxPAPER_A4
    case wxPAPER_A5_TRANSVERSE: return 11; // A5 Transverse 148 x 210 mm // same size as wxPAPER_A5
    case wxPAPER_B5_TRANSVERSE: return 13; // B5 (JIS) Transverse 182 x 257 mm // no equiv just map to wxPAPER_B5
    case wxPAPER_A3_EXTRA: return 8;  // A3 Extra 322 x 445 mm  // no equiv just map to wxPAPER_A3
    case wxPAPER_A5_EXTRA: return 11; // A5 Extra 174 x 235 mm  // same size as wxPAPER_A5
    case wxPAPER_B5_EXTRA: return 13; // B5 (ISO) Extra 201 x 276 mm // no equiv just map to wxPAPER_B5
    case wxPAPER_A2: return 24; // A2 420 x 594 mm  // no equiv just map to wxPAPER_CSHEET
    case wxPAPER_A3_TRANSVERSE: return 8;  // A3 Transverse 297 x 420 mm // same size as wxPAPER_A3
    case wxPAPER_A3_EXTRA_TRANSVERSE: return 8;  // A3 Extra Transverse 322 x 445 mm  // no equiv just map to wxPAPER_A3
	default: return 9;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \return     a wxPaperSize value equivalent to the MFC paper size code
/// \param      id   -> an int representing the MFC paper size code to be mapped
/// \remarks
/// Called from: the App's OnInit() and OnFilePageSetup().
/// Maps an MFC paper size value to the equivalent wxWidgets' wxPaperSize value.
////////////////////////////////////////////////////////////////////////////////////////////
wxPaperSize CAdapt_ItApp::MapMFCtoWXPaperSizeCode(int id)
{
	switch(id)
	{
	case 1: return wxPAPER_LETTER; //				  1 Letter, 8 1/2 by 11 inches
    case 2: return wxPAPER_LETTERSMALL; //	          2 Letter Small, 8 1/2 by 11 inches
    case 3: return wxPAPER_TABLOID; //		          3 Tabloid, 11 by 17 inches
    case 4: return wxPAPER_LEDGER; //				  4  Ledger, 17 by 11 inches
    case 5: return wxPAPER_LEGAL; //				  5 Legal, 8 1/2 by 14 inches
    case 6: return wxPAPER_STATEMENT; //			  6  Statement, 5 1/2 by 8 1/2 inches
    case 7: return wxPAPER_EXECUTIVE; //			  7  Executive, 7 1/4 by 10 1/2 inches
    case 8: return wxPAPER_A3; //					  8  A3 sheet, 297 by 420 millimeters
    case 9: return wxPAPER_A4; //					  9 A4 Sheet, 210 by 297 millimeters
    case 10: return wxPAPER_A4SMALL; //			     10 A4 small sheet, 210 by 297 millimeters
    case 11: return wxPAPER_A5; //					 11 A5 sheet, 148 by 210 millimeters
    case 12: return wxPAPER_B4; //					 12 B4 sheet, 250 by 354 millimeters
    case 13: return wxPAPER_B5; //					 13 B5 sheet, 182-by-257-millimeter paper
    case 14: return wxPAPER_FOLIO; //				 14 Folio, 8-1/2-by-13-inch paper
    case 15: return wxPAPER_QUARTO; //				 15 Quarto, 215-by-275-millimeter paper
    case 16: return wxPAPER_10X14; //				 16 10-by-14-inch sheet
    case 17: return wxPAPER_11X17; //				 17 11-by-17-inch sheet
    case 18: return wxPAPER_NOTE; //				 18 Note, 8 1/2 by 11 inches
    case 19: return wxPAPER_ENV_9; //				 19 #9 Envelope, 3 7/8 by 8 7/8 inches
    case 20: return wxPAPER_ENV_10; //				 20 #10 Envelope, 4 1/8 by 9 1/2 inches
    case 21: return wxPAPER_ENV_11; //				 21 #11 Envelope, 4 1/2 by 10 3/8 inches
    case 22: return wxPAPER_ENV_12; //				 22 #12 Envelope, 4 3/4 by 11 inches
    case 23: return wxPAPER_ENV_14; //				 23 #14 Envelope, 5 by 11 1/2 inches
    case 24: return wxPAPER_CSHEET; //				 24 C Sheet, 17 by 22 inches
    case 25: return wxPAPER_DSHEET; //				 25 D Sheet, 22 by 34 inches
    case 26: return wxPAPER_ESHEET; //				 26 E Sheet, 34 by 44 inches
    case 27: return wxPAPER_ENV_DL; //				 27 DL Envelope, 110 by 220 millimeters
    case 28: return wxPAPER_ENV_C5; //				 28 C5 Envelope, 162 by 229 millimeters
    case 29: return wxPAPER_ENV_C3; //				 29 C3 Envelope, 324 by 458 millimeters
    case 30: return wxPAPER_ENV_C4; //				 30 C4 Envelope, 229 by 324 millimeters
    case 31: return wxPAPER_ENV_C6; //				 31 C6 Envelope, 114 by 162 millimeters
    case 32: return wxPAPER_ENV_C65; //			     32 C65 Envelope, 114 by 229 millimeters
    case 33: return wxPAPER_ENV_B4; //				 33 B4 Envelope, 250 by 353 millimeters
    case 34: return wxPAPER_ENV_B5; //				 34 B5 Envelope, 176 by 250 millimeters
    case 35: return wxPAPER_ENV_B6; //				 35 B6 Envelope, 176 by 125 millimeters
    case 36: return wxPAPER_ENV_ITALY; //			 36 Italy Envelope, 110 by 230 millimeters
    case 37: return wxPAPER_ENV_MONARCH; //		     37 Monarch Envelope, 3 7/8 by 7 1/2 inches
    case 38: return wxPAPER_ENV_PERSONAL; //		 38 6 3/4 Envelope, 3 5/8 by 6 1/2 inches
    case 39: return wxPAPER_FANFOLD_US; //			 39 US Std Fanfold, 14 7/8 by 11 inches
    case 40: return wxPAPER_FANFOLD_STD_GERMAN; // 	 40 German Std Fanfold, 8 1/2 by 12 inches
    case 41: return wxPAPER_FANFOLD_LGL_GERMAN; // 	 41 German Legal Fanfold, 8 1/2 by 13 inches
// The Windows 95 only values don't map to any unique wxPaperSize
	default: return wxPAPER_A4;
	}
}
