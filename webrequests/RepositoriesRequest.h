#pragma once

#include <expected>

#include "../http/HttpConnection.h"
#include "../preferences/wizard/SetupWizardContext.h"

struct RepositoriesSuccess
{
    std::vector<Repository> repositories;
};

using RepositoriesResult = std::expected<RepositoriesSuccess, Error>;

class RepositoriesRequest
{
    const HttpConnection& m_connection;

public:
    explicit RepositoriesRequest(const HttpConnection& connection);

    [[nodiscard]] RepositoriesResult GetRepositories(const Workspace& workspace) const;
};
