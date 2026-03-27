#pragma once
#include "../curl/CurlConnection.h"
#include "../preferences/wizard/SetupWizardContext.h"
#include "wx/arrstr.h"

struct RepositoriesSuccess
{
    std::vector<Repository> repositories;
};
using RepositoriesResult = std::variant<RepositoriesSuccess, Error>;

class RepositoriesRequest {
    const CurlConnection& m_connection;

public:
    explicit RepositoriesRequest(const CurlConnection& connection);

     RepositoriesResult GetRepositories(const wxString& workspaceSlug) const;
};
