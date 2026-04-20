#include "RepositoriesRequest.h"

#include <nlohmann/json.hpp>

#include "../Constants.h"

RepositoriesRequest::RepositoriesRequest(const CurlConnection& connection) :
    m_connection(connection)
{
}

RepositoriesResult RepositoriesRequest::GetRepositories(const Workspace& workspace) const
{
    return m_connection
            .HttpGet(BitBucketBaseUrl + "/repositories/" + workspace.slug)
            .transform([](const Success& success)
            {
                const auto jObject = nlohmann::json::parse(success.body.ToStdString());
                const auto& repositories = jObject.get<RepositoriesResponse>();
                return RepositoriesSuccess{repositories.values};
            });
}
