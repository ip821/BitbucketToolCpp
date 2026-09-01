///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "WizardPages.h"

///////////////////////////////////////////////////////////////////////////

LoginViewBase::LoginViewBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Login:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_pLoginTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_pLoginTextCtrl->SetValidator( wxTextValidator( wxFILTER_NONE, &m_email ) );

	fgSizer1->Add( m_pLoginTextCtrl, 0, wxALL|wxEXPAND, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, _("API token:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_pPasswordTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	m_pPasswordTextCtrl->SetValidator( wxTextValidator( wxFILTER_NONE, &m_password ) );

	fgSizer1->Add( m_pPasswordTextCtrl, 0, wxALL|wxEXPAND, 5 );


	bSizer4->Add( fgSizer1, 1, wxEXPAND, 5 );


	bSizer1->Add( bSizer4, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	m_pManageApiTokensButton = new wxButton( this, wxID_ANY, _("Manage API tokens"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_pManageApiTokensButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );


	bSizer1->Add( bSizer5, 1, wxEXPAND, 5 );

	m_pErrorStaticText = new wxStaticText( this, wxID_ANY, _("Error text"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pErrorStaticText->Wrap( -1 );
	m_pErrorStaticText->SetFont( wxFont( 9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Arial") ) );
	m_pErrorStaticText->SetForegroundColour( wxColour( 255, 0, 0 ) );
	m_pErrorStaticText->Hide();

	bSizer1->Add( m_pErrorStaticText, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_pActivityIndicator = new wxActivityIndicator( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_pActivityIndicator->Hide();

	bSizer1->Add( m_pActivityIndicator, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );

	// Connect Events
	m_pManageApiTokensButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LoginViewBase::OnManageApiTokensClicked ), NULL, this );
}

LoginViewBase::~LoginViewBase()
{
}

WorkspaceViewBase::WorkspaceViewBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	wxArrayString m_pListBoxChoices;
	m_pListBox = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_pListBoxChoices, 0 );
	bSizer2->Add( m_pListBox, 1, wxALL|wxEXPAND, 5 );

	m_pActivityIndicator = new wxActivityIndicator( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_pActivityIndicator->Hide();

	bSizer2->Add( m_pActivityIndicator, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	this->SetSizer( bSizer2 );
	this->Layout();
	bSizer2->Fit( this );
}

WorkspaceViewBase::~WorkspaceViewBase()
{
}

RepositoryViewBase::RepositoryViewBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	wxArrayString m_pListBoxChoices;
	m_pListBox = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_pListBoxChoices, 0 );
	bSizer3->Add( m_pListBox, 1, wxALL|wxEXPAND, 5 );


	this->SetSizer( bSizer3 );
	this->Layout();
	bSizer3->Fit( this );
}

RepositoryViewBase::~RepositoryViewBase()
{
}
