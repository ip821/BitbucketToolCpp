///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class PreferencesWindowBase
///////////////////////////////////////////////////////////////////////////////
class PreferencesWindowBase : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText4;
		wxStaticText* m_workspaceText;
		wxStaticText* m_staticText6;
		wxStaticText* m_repositoryText;
		wxStaticText* m_pHelpText;
		wxButton* m_pButtonSetup;
		wxCheckBox* m_pCheckBoxHideChangesRequested;
		wxCheckBox* m_pCheckBoxUseSubmenusOnMenuOverflow;
		wxCheckBox* m_pCheckBoxDisplayRepositoryNameLowercase;
		wxStaticText* m_pVersionText;

		// Virtual event handlers, override them in your derived class
		virtual void OnInitDialog( wxInitDialogEvent& event ) = 0;
		virtual void OnSetupClicked( wxCommandEvent& event ) = 0;
		virtual void OnHideChangesRequestedChanged( wxCommandEvent& event ) = 0;
		virtual void OnUseSubmenusOnMenuOverflowChanged( wxCommandEvent& event ) = 0;
		virtual void OnDisplayRepositoryNameLowercaseChanged( wxCommandEvent& event ) = 0;


	public:

		PreferencesWindowBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );

		~PreferencesWindowBase();

};

