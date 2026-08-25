///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>
#include <wx/sizer.h>
#include <wx/activityindicator.h>
#include <wx/panel.h>
#include <wx/checklst.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class LoginViewBase
///////////////////////////////////////////////////////////////////////////////
class LoginViewBase : public wxPanel
{
	private:

	protected:
		wxStaticText* m_staticText1;
		wxTextCtrl* m_pLoginTextCtrl;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_pPasswordTextCtrl;
		wxStaticText* m_pErrorStaticText;
		wxActivityIndicator* m_pActivityIndicator;

	public:
		wxString m_email;
		wxString m_password;

		LoginViewBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~LoginViewBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class WorkspaceViewBase
///////////////////////////////////////////////////////////////////////////////
class WorkspaceViewBase : public wxPanel
{
	private:

	protected:
		wxCheckListBox* m_pListBox;
		wxActivityIndicator* m_pActivityIndicator;

	public:

		WorkspaceViewBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~WorkspaceViewBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class RepositoryViewBase
///////////////////////////////////////////////////////////////////////////////
class RepositoryViewBase : public wxPanel
{
	private:

	protected:
		wxCheckListBox* m_pListBox;

	public:

		RepositoryViewBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~RepositoryViewBase();

};
