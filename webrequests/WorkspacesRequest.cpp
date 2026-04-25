#include "WorkspacesRequest.h"

#include <nlohmann/json.hpp>
#include <ranges>
#include <cpp_utils/match_expected.h>
#include <cpp_utils/ranges.h>

#include "../curl/CurlConnection.h"
#include "../Constants.h"

WorkspacesRequest::WorkspacesRequest(const CurlConnection& connection) :
    m_connection(connection)
{
}

WorkspacesResult WorkspacesRequest::GetWorkspaces() const
{
    return ip::match_expected(m_connection.HttpGet(BitBucketBaseUrl + "/user/workspaces"),
                              [](const Success& success)
                              {
                                  const auto jObject = nlohmann::json::parse(success.body.ToStdString());
                                  const auto [repositories] = jObject.get<WorkspacesResponse>();

                                  const auto workspaces = repositories
                                          | std::views::transform([](const WorkspaceAccess& it) { return it.workspace; })
                                          | std::views::filter([](const Workspace& it) { return !it.slug.IsEmpty(); })
                                          | std::ranges::to<std::vector>();

                                  return WorkspacesSuccess{workspaces};
                              },
                              [](const Error& error) { return error; });
}
