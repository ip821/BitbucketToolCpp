#pragma once

#include <expected>

#include "../curl/CurlConnection.h"
#include "../preferences/wizard/SetupWizardContext.h"

struct RepositoriesSuccess
{
    std::vector<Repository> repositories;
};

using RepositoriesResult = std::expected<RepositoriesSuccess, Error>;

class RepositoriesRequest
{
    const CurlConnection& m_connection;

public:
    explicit RepositoriesRequest(const CurlConnection& connection);

    [[nodiscard]] RepositoriesResult GetRepositories(const Workspace& workspace) const;
};
