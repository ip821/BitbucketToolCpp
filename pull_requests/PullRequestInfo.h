#pragma once

#include "../webrequests/DiffStat.h"
#include "../webrequests/PullRequest.h"
#include "../webrequests/Status.h"

struct PullRequestInfo
{
    PullRequest pullRequest;
    std::vector<Status> statuses;
    DiffStat diffStat;

    wxString GetMainMenuItemTitle() const
    {
        const auto draftPrefix = pullRequest.draft ? wxS("[DRAFT] - ") : wxS("");
        return std::format(wxS("{}{}"), draftPrefix, pullRequest.title);
    }

    wxString GetAuthorAndBranchMenuItemTitle() const
    {
        return std::format(
            wxS("[{}] [{}] [{}] → [{}]"),
            pullRequest.destination.repository.name,
            pullRequest.author.display_name,
            pullRequest.source.branch.name,
            pullRequest.destination.branch.name);
    }

    wxString GetPullRequestDetailsMenuItemTitle() const
    {
        const auto approved = pullRequest.participants
                | std::views::filter([](const auto& it) { return it.approved; })
                | std::ranges::to<std::vector>();

        const auto buildStatus =
                HasBuildsWithStatus(StatusState::Failed)
                    ? wxS("Failed")
                    : HasBuildsWithStatus(StatusState::Stopped)
                          ? wxS("Cancelled")
                          : HasBuildsWithStatus(StatusState::InProgress)
                                ? wxS("InProgress")
                                : std::format(
                                    wxS("Success ({})"),
                                    std::ranges::count_if(statuses, [](const auto& it) { return it.state == StatusState::Successful; })
                                );

        return std::format(
            wxS("[Files: {}] [Comments: {}] [Approvals: {}] [Builds: {}]"),
            diffStat.size,
            pullRequest.comment_count,
            std::format(wxS("{}/{}"), approved.size(), pullRequest.participants.size()),
            buildStatus
        );
    }

    bool HasBuildsWithStatus(StatusState state) const
    {
        return std::ranges::any_of(statuses, [&state](const auto& it) { return it.state == state; });
    }

    wxString GetStatus() const
    {
        auto status = wxS("Waiting for approve");

        const auto approved = pullRequest.participants
                | std::views::filter([](const auto& it) { return it.approved; })
                | std::ranges::to<std::vector>();

        if (approved.size() == pullRequest.participants.size())
            status = wxS("Ready for merge");

        if (HasBuildsWithStatus(StatusState::Failed))
            status = wxS("Build failed");
        else if (HasBuildsWithStatus(StatusState::InProgress))
            status = wxS("Building");

        return status;
    }

    wxString GetMyPullRequestBranchMenuItemTitle() const
    {
        return std::format(
            wxS("[{}] [{}] → [{}]"),
            GetStatus(),
            pullRequest.source.branch.name,
            pullRequest.destination.branch.name
        );
    }
};

struct PullRequestsInfo
{
    const User currentUser{};
    const std::vector<PullRequestInfo> waitingForMyApprovalPullRequests{};
    const std::vector<PullRequestInfo> myPullRequests{};
};
