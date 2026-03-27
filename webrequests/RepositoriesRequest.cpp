#include "RepositoriesRequest.h"

#include <nlohmann/json.hpp>

#include "../Constants.h"
#include "../Switch.h"

RepositoriesRequest::RepositoriesRequest(const CurlConnection& connection) :
    m_connection(connection)
{
}

RepositoriesResult RepositoriesRequest::GetRepositories(const wxString& workspaceSlug) const
{
    const auto response = m_connection.HttpGet(BitBucketBaseUrl + "/repositories/" + workspaceSlug);
    return Match(response,
                 [](const Success& success)
                 {
                     const auto jObject = nlohmann::json::parse(success.body.ToStdString());

                     wxString repositoryNames;

                     std::vector<Repository> repositories;
                     const auto& jRepositories = jObject["values"];
                     for (const auto& jRepository : jRepositories)
                     {
                         const Repository& repository = {
                             jRepository["full_name"].get<std::string>(),
                             jRepository["slug"].get<std::string>()
                         };
                         repositories.push_back(repository);
                     }

                     return static_cast<RepositoriesResult>(RepositoriesSuccess{repositories});
                 },
                 [](const Error& error)
                 {
                     return static_cast<RepositoriesResult>(error);
                 }
        );
}
