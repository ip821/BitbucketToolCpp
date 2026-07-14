#pragma once

#include <vector>

#include "bitbucket_api/Structs.h"

class wxString;

struct SetupWizardContext
{
    std::vector<Workspace> m_workspaces;
    std::vector<Workspace> m_selectedWorkspaces;
    std::vector<Repository> m_repositories;
};
