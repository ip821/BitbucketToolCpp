#include "WorkspacesRequest.h"

#include <nlohmann/json.hpp>
#include <ranges>

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
                 [](const Success& success) -> WorkspacesResult
                 {
                     const auto jObject = nlohmann::json::parse(success.body.ToStdString());
                     const auto response = jObject.get<WorkspacesResponse>();

                     const auto workspaces = response.values
                         | std::views::transform(&WorkspaceAccess::workspace)
                         | std::views::filter([](const Workspace& it) { return !it.slug.IsEmpty(); })
                         | std::ranges::to<std::vector>();

                     return WorkspacesSuccess{workspaces};
                 },
                 [](const Error& error) -> WorkspacesResult
                 {
                     return error;
                 }
        );
}
