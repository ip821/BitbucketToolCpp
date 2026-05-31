#include "RepositoriesRequest.h"

#include <cpp_utils/macros_expected.h>
#include <nlohmann/json.hpp>
#include <cpp_utils/match_expected.h>

#include "../Constants.h"

RepositoriesRequest::RepositoriesRequest(const HttpConnection& connection) :
    m_connection(connection)
{
}

RepositoriesResult RepositoriesRequest::GetRepositories(const Workspace& workspace) const
{
    UNWRAP_OR_RETURN_ERROR(success, m_connection.HttpGet(BitBucketBaseUrl + "/repositories/" + workspace.slug));

    const auto jObject = nlohmann::json::parse(success.body.ToStdString());
    const auto& repositories = jObject.get<RepositoriesResponse>();
    return RepositoriesSuccess{repositories.values};
}
