#include "WorkspacesRequest.h"

#include <nlohmann/json.hpp>
#include <ranges>

#include "../curl/CurlConnection.h"
#include "../Switch.h"
#include "../Constants.h"
#include "../Ranges.h"

WorkspacesRequest::WorkspacesRequest(const CurlConnection& connection) :
    m_connection(connection)
{
}

WorkspacesResult WorkspacesRequest::GetWorkspaces() const
{
    return m_connection
            .HttpGet(BitBucketBaseUrl + "/user/workspaces")
            .transform([](const Success& success)
            {
                const auto jObject = nlohmann::json::parse(success.body.ToStdString());
                const auto [repositories] = jObject.get<WorkspacesResponse>();

                const auto workspaces = repositories
                        | std::views::transform([](const WorkspaceAccess& it) { return it.workspace; })
                        | std::views::filter([](const Workspace& it) { return !it.slug.IsEmpty(); })
                        | std::ranges::to<std::vector>();

                return WorkspacesSuccess{workspaces};
            });
}
