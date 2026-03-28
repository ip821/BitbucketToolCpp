//
// Created by Igor Palkin on 22.02.2026.
//

#ifndef PRTOOLFORBITBUCKET2_SETUPWIZARDCONTEXT_H
#define PRTOOLFORBITBUCKET2_SETUPWIZARDCONTEXT_H
#include <vector>

#include "../../webrequests/Repository.h"

class wxString;

struct SetupWizardContext
{
    std::vector<Workspace> m_workspaces;
    std::vector<Workspace> m_selectedWorkspaces;
    std::vector<Repository> m_repositories;
};


#endif //PRTOOLFORBITBUCKET2_SETUPWIZARDCONTEXT_H
