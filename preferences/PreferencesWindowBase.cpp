///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "PreferencesWindowBase.h"

///////////////////////////////////////////////////////////////////////////

PreferencesWindowBase::PreferencesWindowBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );


	bSizer4->Add( 0, 10, 0, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Workspace:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer3->Add( m_staticText4, 0, wxALL|wxEXPAND, 5 );

	m_workspaceText = new wxStaticText( this, wxID_ANY, _("test-workspace"), wxDefaultPosition, wxDefaultSize, 0 );
	m_workspaceText->Wrap( -1 );
	fgSizer3->Add( m_workspaceText, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Repository:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer3->Add( m_staticText6, 0, wxALL, 5 );

	m_repositoryText = new wxStaticText( this, wxID_ANY, _("test-repository"), wxDefaultPosition, wxDefaultSize, 0 );
	m_repositoryText->Wrap( -1 );
	fgSizer3->Add( m_repositoryText, 0, wxALIGN_RIGHT|wxALL, 5 );


	bSizer4->Add( fgSizer3, 1, wxEXPAND, 5 );

	m_pHelpText = new wxStaticText( this, wxID_ANY, _("Use Setup button to enter credentials and to\nchoose Workspace and Repository to follow"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	m_pHelpText->Wrap( -1 );
	bSizer4->Add( m_pHelpText, 0, wxALL|wxEXPAND, 5 );


	bSizer4->Add( 0, 10, 0, wxEXPAND, 5 );

	m_button1 = new wxButton( this, wxID_ANY, _("Setup..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	bSizer4->Add( 0, 10, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer4 );
	this->Layout();
	bSizer4->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( PreferencesWindowBase::OnInitDialog ) );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreferencesWindowBase::OnSetupClicked ), NULL, this );
}

PreferencesWindowBase::~PreferencesWindowBase()
{
}
