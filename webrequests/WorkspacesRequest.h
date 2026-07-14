#pragma once

#include <cpp_utils/ranges.h>
#include <cpp_utils/macros_expected.h>

#include "BitbucketRequest.h"
#include "BitbucketUrlBuilder.h"
#include "Values.h"
#include "../preferences/wizard/SetupWizardContext.h"

class WorkspacesRequest : public BitbucketRequest<Values<WorkspaceAccess> >
{
public:
    [[nodiscard]] std::expected<std::vector<Workspace>, BitbucketError> GetWorkspaces() const
    {
        const auto result = PerformRequest(BitbucketUrlBuilder::GetWorkspacesUrl());
        UNWRAP_OR_RETURN_ERROR(repositories, result);

        const auto workspaces = repositories.values
                | std::views::transform([](const auto& it) { return it.workspace; })
                | std::views::filter([](const auto& it) { return !it.slug.IsEmpty(); })
                | std::ranges::to<std::vector>();

        return workspaces;
    }
};
