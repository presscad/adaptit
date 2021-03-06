/////////////////////////////////////////////////////////////////////////////
/// \project		adaptit
/// \file			EditSourceTextDlg.cpp
/// \author			Bill Martin
/// \date_created	13 July 2006
/// \rcs_id $Id$
/// \copyright		2008 Bruce Waters, Bill Martin, SIL International
/// \license		The Common Public License or The GNU Lesser General Public License (see license directory)
/// \description	This is the implementation file for the CEditSourceTextDlg class. 
/// The CEditSourceTextDlg class provides a dialog in which the user can edit the
/// source text. Restrictions are imposed to prevent such editing while glossing, or 
/// if the source text has disparate text types, is a retranslation or has a free
/// translation or filtered information contained within it.
/// \derivation		The CEditSourceTextDlg class is derived from AIModalDialog.
/////////////////////////////////////////////////////////////////////////////
// Pending Implementation Items in EditSourceTextDlg.cpp (in order of importance): (search for "TODO")
// 1. 
//
// Unanswered questions: (search for "???")
// 1. 
// 
/////////////////////////////////////////////////////////////////////////////

// the following improves GCC compilation performance
#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation "EditSourceTextDlg.h"
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

// other includes
#include <wx/docview.h> // needed for classes that reference wxView or wxDocument
#include <wx/valgen.h> // for wxGenericValidator

#include "Adapt_It.h"
#include "EditSourceTextDlg.h"
#include "helpers.h"

// next two are for version 2.0 which includes the option of a 3rd line for glossing

// This global is defined in Adapt_ItView.cpp.
//extern bool	gbIsGlossing; // when TRUE, the phrase box and its line have glossing text

// the following globals make it easy to access the sublists and their counts; for use in 
// CEditSourceTextDlg
//extern SPList* gpOldSrcPhraseList;
//extern SPList* gpNewSrcPhraseList;
extern int gnCount;    // count of old srcphrases (user selected these) after unmerges, etc
extern int gnNewCount; // count of new srcphrases (after user finished editing the source text)

// BEW additions 07May08, for the refactored code
extern EditRecord gEditRecord; // store info pertinent to generalized editing in this global structure
// end BEW additions 07 May08


/// This global is defined in Adapt_It.cpp.
extern CAdapt_ItApp* gpApp; // if we want to access it fast

// BEW 10Jan12, added handlers for OK and Cancel buttons - the legacy code was relying
// on validators having been set, in order to get the user's edits transferred from the
// controls to the storage variables, but Bill's recent removal of the validators killed
// that mechanism, and doing it manually didn't work either because the OnOK() and
// OnCancel() members didn't get called as they weren't attached to the event mechanism - so I'm
// doing that now
//
// event handler table
BEGIN_EVENT_TABLE(CEditSourceTextDlg, AIModalDialog)
	EVT_INIT_DIALOG(CEditSourceTextDlg::InitDialog)
	EVT_TEXT_ENTER(IDC_EDIT_NEW_SOURCE,CEditSourceTextDlg::ReinterpretEnterKeyPress)
    EVT_BUTTON(ID_BUTTON_HELP_INFO, CEditSourceTextDlg::OnHelpOnEditingSourceText)
	EVT_BUTTON(wxID_OK, CEditSourceTextDlg::OnOK)
	EVT_BUTTON(wxID_CANCEL, CEditSourceTextDlg::OnCancel)
END_EVENT_TABLE()

CEditSourceTextDlg::CEditSourceTextDlg(wxWindow* parent) // dialog constructor
	: AIModalDialog(parent, -1, _("Edit Source Text"),
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	m_strNewSourceText = _T("");
	m_strOldSourceText = _T("");
	//m_bEditMarkersWanted = FALSE;
	
	// This dialog function below is generated in wxDesigner, and defines the controls and sizers
	// for the dialog. The first parameter is the parent which should normally be "this".
	// The second and third parameters should both be TRUE to utilize the sizers and create the right
	// size dialog.
	EditSourceTextDlgFunc(this, TRUE, TRUE);
	// The declaration is: EditSourceTextDlgFunc( wxWindow *parent, bool call_fit, bool set_sizer );

    // whm 5Mar2019 Note: The EditSourceTextDlgFunc() is cramped for vertical space, so it puts
    // the OK and Cancel buttons to the right of a static text that says: "Do your editing, including 
    // punctuation, on the text in the box below." The OK and Cancel buttons also have tooltip text
    // that should be preserved, so we should call our own ReverseOkCancelButtonsForMac() function
    // below to make platform button adjustments.
	bool bOK;
	bOK = gpApp->ReverseOkCancelButtonsForMac(this);
	bOK = bOK; // avoid warning
	// Need pointers for the following:
	// ID_TEXTCTRL_EDIT_SOURCE_AS_STATIC1 
	// ID_TEXTCTRL_EDIT_SOURCE_AS_STATIC2
	// IDC_EDIT_OLD_SOURCE_TEXT
	// IDC_EDIT_NEW_SOURCE // used in TransferMarkersDlg (which can be removed from project)
	// IDC_EDIT_FOLLCONTEXT // used in RetranslationDlg but both are modal dialogs so shouldn't
	// interfere with each other.
	// 
	pSrcTextEdit = (wxTextCtrl*)FindWindowById(IDC_EDIT_NEW_SOURCE);
	wxASSERT(pSrcTextEdit != NULL);
	//pSrcTextEdit->SetValidator(wxGenericValidator(&m_strNewSourceText)); // needed; OnEditSourceText() initializes this

	pPreContextEdit = (wxTextCtrl*)FindWindowById(IDC_EDIT_PRECONTEXT); // read only edit control
	wxASSERT(pPreContextEdit != NULL);
	//pPreContextEdit->SetValidator(wxGenericValidator(&m_preContext)); // needed; OnEditSourceText() initializes this
	pPreContextEdit->SetBackgroundColour(gpApp->sysColorBtnFace); //(wxSYS_COLOUR_WINDOW);

	pFollContextEdit = (wxTextCtrl*)FindWindowById(IDC_EDIT_FOLLCONTEXT); // read only edit control
	wxASSERT(pFollContextEdit != NULL);
	//pFollContextEdit->SetValidator(wxGenericValidator(&m_follContext)); // needed; OnEditSourceText() initializes this
	pFollContextEdit->SetBackgroundColour(gpApp->sysColorBtnFace); //(wxSYS_COLOUR_WINDOW);

	pOldSrcTextEdit = (wxTextCtrl*)FindWindowById(IDC_EDIT_OLD_SOURCE_TEXT); // read only edit control
	wxASSERT(pOldSrcTextEdit != NULL);
	//pOldSrcTextEdit->SetValidator(wxGenericValidator(&m_strOldSourceText)); // needed; OnEditSourceText() initializes this
	pOldSrcTextEdit->SetBackgroundColour(gpApp->sysColorBtnFace); //(wxSYS_COLOUR_WINDOW);

    pBtnOK = (wxButton*)FindWindowById(wxID_OK); // access for tooltip change
    wxASSERT(pBtnOK != NULL);

    pBtnCancel = (wxButton*)FindWindowById(wxID_CANCEL); // access for tooltip change
    wxASSERT(pBtnCancel != NULL);

	// The following two are for static text within read-only multi-line wxEditCtrls on the dialog
	
    // whm 1Apr2019 removed following two info text ctrls, putting the text into message box summoned by Help button.
	//pTextCtrlEditAsStatic1 = (wxTextCtrl*)FindWindowById(ID_TEXTCTRL_EDIT_SOURCE_AS_STATIC1); // read only edit control
	//wxASSERT(pTextCtrlEditAsStatic1 != NULL);
	//pTextCtrlEditAsStatic1->SetBackgroundColour(gpApp->sysColorBtnFace); //(wxSYS_COLOUR_WINDOW);
    // "You can edit the text, or the markers, or both. Do not edit the markers unless you know what you are doing. 
    // A marker must have a space following it; end markers (these end with *, for example \f*)  can optionally have a following space omitted. 
    // If your selection included one or more notes or free translations, they were removed and stored in lists so that you will not have to deal with them here. 
    // Collected back translations in this section of the document were deleted."
	
	//pTextCtrlEditAsStatic2 = (wxTextCtrl*)FindWindowById(ID_TEXTCTRL_EDIT_SOURCE_AS_STATIC2); // read only edit control
	//wxASSERT(pTextCtrlEditAsStatic2 != NULL);
	//pTextCtrlEditAsStatic2->SetBackgroundColour(gpApp->sysColorBtnFace); //(wxSYS_COLOUR_WINDOW);
    // "After this dialog closes, Adapt It helps you. 
    // (1) It helps you do new adaptations; and glosses too when appropriate, for the new source text. 
    // (2) It  automatically restores removed notes ( their locations may differ a little bit). 
    // (3) It helps you to produce new free translations, or edit the old ones. 
    // (4) Any deleted back translations will be automatically collected again."
}

CEditSourceTextDlg::~CEditSourceTextDlg() // destructor
{
	
}

void CEditSourceTextDlg::ReinterpretEnterKeyPress(wxCommandEvent& WXUNUSED(event))
{
    // now update the data and close off the dialog.
    // 
    // A nice thing wxWidgets does is if the wxTE_PROCESS_ENTER style is used for the wxTextCtrl, then
    // wxWidgets itself blocks any newline or carriage return from being entered into the data string,
    // and so no manual intervention is needed here in order to remove such characters. We just need the
    // calls below.
	// Call TransferDataFromWindow() which updates the m_stringNewSourceText associated with this
	// window by the SetValidator(wxGenericValidator(&m_strOldSourceText)) call in the constructor above.
	//TransferDataFromWindow(); whm removed 21Nov11
	m_strNewSourceText = pSrcTextEdit->GetValue(); // whm added 21Nov11
	m_preContext = pPreContextEdit->GetValue(); // whm added 21Nov11
	m_follContext = pFollContextEdit->GetValue(); // whm added 21Nov11
	m_strOldSourceText = pOldSrcTextEdit->GetValue(); // whm added 21Nov11

	// Call the EndModal(event) handler which automatically closes the dialog
	EndModal(wxID_OK); // we'll unilaterally end (gets the dialog dismissed)

	// uncomment out the following line if confirmation in a debugger is required that
	// m_stringNewSourceText has no \r nor \n in it
	//wxString str = m_strNewSourceText;
}

void CEditSourceTextDlg::OnHelpOnEditingSourceText(wxCommandEvent& WXUNUSED(event))
{
    // "You can edit the text, or the markers, or both. Do not edit the markers unless you know what you are doing. 
    // A marker must have a space following it; end markers (these end with *, for example \f*)  can optionally have a following space omitted. 
    // If your selection included one or more notes or free translations, they were removed and stored in lists so that you will not have to deal with them here. 
    // Collected back translations in this section of the document were deleted."

    // "After this dialog closes, Adapt It helps you. 
    // (1) It helps you do new adaptations; and glosses too when appropriate, for the new source text. 
    // (2) It  automatically restores removed notes ( their locations may differ a little bit). 
    // (3) It helps you to produce new free translations, or edit the old ones. 
    // (4) Any deleted back translations will be automatically collected again."
    wxString msg;
    msg = _("You can edit the text, or the markers, or both. Do not edit the markers unless you know what you are doing. " \
        "A marker must have a space following it; end markers(these end with *, for example \\f*)  can optionally have a following space omitted.\n\n" \
        "If your selection included one or more notes or free translations, they were removed and stored in lists so that you will not have to deal with them here.\n\n" \
        "Collected back translations in this section of the document were deleted.\n\n" \
        "After this dialog closes, Adapt It helps you.\n" \
        "  (1) It helps you do new adaptations; and glosses too when appropriate, for the new source text.\n" \
        "  (2) It  automatically restores removed notes (their locations may differ a little bit).\n" \
        "  (3) It helps you to produce new free translations, or edit the old ones.\n" \
        "  (4) Any deleted back translations will be automatically collected again.");
    // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
    gpApp->m_bUserDlgOrMessageRequested = TRUE;
    wxMessageBox(msg, _("Help on Editing Source Text"), wxICON_INFORMATION | wxOK);
}

void CEditSourceTextDlg::InitDialog(wxInitDialogEvent& WXUNUSED(event)) // InitDialog is method of wxWindow
{
	//InitDialog() is not virtual, no call needed to a base class

    // Add tooltips to the OK and Cancel buttons within the wxStdDialogButtonSizer
    pBtnOK->SetToolTip(_T("Click OK when you are finished editing the source text"));
    pBtnCancel->SetToolTip(_T("Click Cancel to discard any edits and close the dialog"));

	// next stuff copied from CRetranslationDlg's OnInitDialog() and then modified

	// make the fonts show user's desired point size in the dialog
	#ifdef _RTL_FLAGS
	gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pSourceFont, pSrcTextEdit, pPreContextEdit,
								NULL, NULL, gpApp->m_pDlgSrcFont, gpApp->m_bSrcRTL);
	#else // Regular version, only LTR scripts supported, so use default FALSE for last parameter
	gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pSourceFont, pSrcTextEdit, pPreContextEdit, 
								NULL, NULL, gpApp->m_pDlgSrcFont);
	#endif

	#ifdef _RTL_FLAGS
	gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pSourceFont, pFollContextEdit, pOldSrcTextEdit,
								NULL, NULL, gpApp->m_pDlgSrcFont, gpApp->m_bSrcRTL);
	#else // Regular version, only LTR scripts supported, so use default FALSE for last parameter
	gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pSourceFont, pFollContextEdit, pOldSrcTextEdit, 
								NULL, NULL, gpApp->m_pDlgSrcFont);
	#endif

	//#ifdef _RTL_FLAGS
	//gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pTargetFont, pTgtEdit, NULL,
	//							NULL, NULL, gpApp->m_pDlgTgtFont, gpApp->m_bTgtRTL);
	//#else // Regular version, only LTR scripts supported, so use default FALSE for last parameter
	//gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pTargetFont, pTgtEdit, NULL, 
	//							NULL, NULL, gpApp->m_pDlgTgtFont);
	//#endif
	//TransferDataToWindow(); // whm removed 21Nov11
	pSrcTextEdit->ChangeValue(m_strNewSourceText);
	pPreContextEdit->ChangeValue(m_preContext);
	pFollContextEdit->ChangeValue(m_follContext);
	pOldSrcTextEdit->ChangeValue(m_strOldSourceText);


	// make sure the end of the text is scolled into view
	pPreContextEdit->SetFocus();
	pPreContextEdit->ShowPosition(pPreContextEdit->GetLastPosition());

	// set the focus to the source text edit box
	pSrcTextEdit->SetFocus();
    // whm 3Aug2018 Note: TODO: Determine if the select all command below
    // should be suppressed for this edit box during Edit Source Text???
	pSrcTextEdit->SetSelection(-1,-1); // -1,-1 selects all

	//pEditSourceTextDlgSizer->Layout();
}

// OnOK() calls wxWindow::Validate, then wxWindow::TransferDataFromWindow.
// If this returns TRUE, the function either calls EndModal(wxID_OK) if the
// dialog is modal, or sets the return value to wxID_OK and calls Show(FALSE)
// if the dialog is modeless.
// BEW 10Jan12, Bug Fix: recently Bill removed dependence on validators - and so when
// OnOK() returns, there is no validator set and so the user's change to a new source text
// is not transferred from pSrcTextEdit, resulting in the dialog returning the old source
// text value. The fix is to put an explicit GetValue() call in the OnOK() handler
void CEditSourceTextDlg::OnOK(wxCommandEvent& event) 
{
	m_strNewSourceText = pSrcTextEdit->GetValue();
	event.Skip(); //EndModal(wxID_OK); //AIModalDialog::OnOK(event); // not virtual in wxDialog
}

void CEditSourceTextDlg::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	EndModal(wxID_CANCEL); //wxDialog::OnCancel(event);
}

// other class methods

