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
        return std::format(wxS("{}"), pullRequest.title);
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
                HasBuildsWithStatus(Failed)
                    ? wxS("Failed")
                    : HasBuildsWithStatus(Stopped)
                          ? wxS("Cancelled")
                          : HasBuildsWithStatus(InProgress)
                                ? wxS("InProgress")
                                : std::format(wxS("Success ({})"), std::ranges::count_if(statuses, [](const auto& it) { return it.state == Successful; }));

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
};

struct PullRequestsInfo
{
    const User currentUser;
    const std::vector<PullRequestInfo> waitingForMyApprovalPullRequests;
    const std::vector<PullRequestInfo> myPullRequests;
};
