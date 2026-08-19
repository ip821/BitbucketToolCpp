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

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );


	bSizer2->Add( 0, 10, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Bitbucket") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText4 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("Workspace:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer3->Add( m_staticText4, 0, wxALL|wxEXPAND, 5 );

	m_workspaceText = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("test-workspace"), wxDefaultPosition, wxDefaultSize, 0 );
	m_workspaceText->Wrap( -1 );
	fgSizer3->Add( m_workspaceText, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_staticText6 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("Repository:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer3->Add( m_staticText6, 0, wxALL, 5 );

	m_repositoryText = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("test-repository"), wxDefaultPosition, wxDefaultSize, 0 );
	m_repositoryText->Wrap( -1 );
	fgSizer3->Add( m_repositoryText, 0, wxALIGN_RIGHT|wxALL, 5 );


	sbSizer1->Add( fgSizer3, 1, wxEXPAND, 5 );


	sbSizer1->Add( 0, 20, 0, wxEXPAND, 5 );

	m_pHelpText = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("Use Setup button to enter credentials and to\nchoose Workspace and Repository to follow"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	m_pHelpText->Wrap( -1 );
	sbSizer1->Add( m_pHelpText, 0, wxALL|wxEXPAND, 5 );

	m_pButtonSetup = new wxButton( sbSizer1->GetStaticBox(), wxID_ANY, _("Setup..."), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_pButtonSetup, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	bSizer2->Add( sbSizer1, 0, wxEXPAND, 5 );


	bSizer2->Add( 0, 10, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Options") ), wxVERTICAL );

	m_pCheckBoxHideChangesRequested = new wxCheckBox( sbSizer2->GetStaticBox(), wxID_ANY, _("Hide PR with \"Changes requested\""), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_pCheckBoxHideChangesRequested, 0, wxALL, 5 );

	m_pCheckBoxUseSubmenusOnMenuOverflow = new wxCheckBox( sbSizer2->GetStaticBox(), wxID_ANY, _("Use submenus on menu overflow"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_pCheckBoxUseSubmenusOnMenuOverflow, 0, wxALL, 5 );

	m_pCheckBoxDisplayRepositoryNameLowercase = new wxCheckBox( sbSizer2->GetStaticBox(), wxID_ANY, _("Display repository name in lower case"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_pCheckBoxDisplayRepositoryNameLowercase, 0, wxALL, 5 );


	bSizer2->Add( sbSizer2, 1, wxEXPAND, 5 );


	bSizer2->Add( 0, 10, 0, wxEXPAND, 5 );

	m_pVersionText = new wxStaticText( this, wxID_ANY, _("Version X.XX"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pVersionText->Wrap( -1 );
	bSizer2->Add( m_pVersionText, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer2->Add( 0, 20, 0, wxEXPAND, 5 );


	bSizer4->Add( bSizer2, 1, wxEXPAND|wxLEFT|wxRIGHT, 15 );


	this->SetSizer( bSizer4 );
	this->Layout();
	bSizer4->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( PreferencesWindowBase::OnInitDialog ) );
	m_pButtonSetup->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreferencesWindowBase::OnSetupClicked ), NULL, this );
	m_pCheckBoxHideChangesRequested->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PreferencesWindowBase::OnHideChangesRequestedChanged ), NULL, this );
	m_pCheckBoxUseSubmenusOnMenuOverflow->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PreferencesWindowBase::OnUseSubmenusOnMenuOverflowChanged ), NULL, this );
	m_pCheckBoxDisplayRepositoryNameLowercase->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PreferencesWindowBase::OnDisplayRepositoryNameLowercaseChanged ), NULL, this );
}

PreferencesWindowBase::~PreferencesWindowBase()
{
}
