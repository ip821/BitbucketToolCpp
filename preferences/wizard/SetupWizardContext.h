//
// Created by Igor Palkin on 22.02.2026.
//

#ifndef PRTOOLFORBITBUCKET2_SETUPWIZARDCONTEXT_H
#define PRTOOLFORBITBUCKET2_SETUPWIZARDCONTEXT_H
#include <vector>

class wxString;

struct Workspace
{
    wxString m_slug;
};

struct Repository
{
    wxString m_name;
    wxString m_slug;
};

struct SetupWizardContext
{
    std::vector<Workspace> m_workspaces;
    std::vector<Workspace> m_selectedWorkspaces;
    std::vector<Repository> m_repositories;
};


#endif //PRTOOLFORBITBUCKET2_SETUPWIZARDCONTEXT_H
