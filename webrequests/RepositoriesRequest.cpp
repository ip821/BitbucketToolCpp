#include "RepositoriesRequest.h"

#include <nlohmann/json.hpp>

#include "../Constants.h"
#include "../Switch.h"

RepositoriesRequest::RepositoriesRequest(const CurlConnection& connection) :
    m_connection(connection)
{
}

RepositoriesResult RepositoriesRequest::GetRepositories(const Workspace& workspace) const
{
    const auto response = m_connection.HttpGet(BitBucketBaseUrl + "/repositories/" + workspace.slug);
    return Match(response,
                 [](const Success& success) -> RepositoriesResult
                 {
                     const auto jObject = nlohmann::json::parse(success.body.ToStdString());
                     const auto& repositories = jObject.get<RepositoriesResponse>();
                     return RepositoriesSuccess{repositories.values};
                 },
                 [](const Error& error)-> RepositoriesResult
                 {
                     return error;
                 }
        );
}
