#pragma once

#include "BitbucketClient.h"
#include "Values.h"
#include "../Constants.h"
#include "../http/HttpConnection.h"
#include "../preferences/wizard/SetupWizardContext.h"

class WorkspacesRequest : public BitbucketClient<Values<WorkspaceAccess> >
{
public:
    [[nodiscard]] std::expected<std::vector<Workspace>, Error> GetWorkspaces() const
    {
        const auto url = BitBucketBaseUrl + wxS("/user/workspaces");
        const auto result = PerformRequest(url);
        UNWRAP_OR_RETURN_ERROR(repositories, result);

        const auto workspaces = repositories.values
                | std::views::transform([](const auto& it) { return it.workspace; })
                | std::views::filter([](const auto& it) { return !it.slug.IsEmpty(); })
                | std::ranges::to<std::vector>();

        return workspaces;
    }
};
