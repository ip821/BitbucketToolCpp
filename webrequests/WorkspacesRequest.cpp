#include "WorkspacesRequest.h"

#include <nlohmann/json.hpp>

#include "../curl/CurlConnection.h"
#include "../Switch.h"
#include "../Constants.h"

WorkspacesRequest::WorkspacesRequest(const CurlConnection& connection) :
    m_connection(connection)
{
}

WorkspacesResult WorkspacesRequest::GetWorkspaces() const
{
    const auto response = m_connection.HttpGet(BitBucketBaseUrl + "/user/workspaces");
    return Match(response,
                 [](const Success& success)
                 {
                     const auto result = nlohmann::json::parse(success.body.ToStdString());

                     std::vector<Workspace> workspaces;
                     for (const auto& jWorkspaceAccess : result["values"])
                     {
                         const auto& jWorkspace = jWorkspaceAccess["workspace"];
                         const Workspace& workspace = {
                             jWorkspace["slug"].get<std::string>()
                         };
                         workspaces.push_back(workspace);
                     }
                     return static_cast<WorkspacesResult>(WorkspacesSuccess{workspaces});
                 },
                 [](const Error& error)
                 {
                     return static_cast<WorkspacesResult>(error);
                 }
        );
}
