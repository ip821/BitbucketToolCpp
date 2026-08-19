#pragma once

#include <vector>

#include "../pull_requests/PullRequestsInfo.h"

struct PullRequestMenuEntry
{
    const PullRequestInfo* pullRequest{};
    bool includesTitle{true};
    std::vector<wxString> secondaryTitles;
};

struct PullRequestMenuEntryResult
{
    int hiddenPullRequestsCount{};
    std::vector<PullRequestMenuEntry> entries;
};

struct GetWaitingMyApprovalMenuEntriesArgs
{
    const bool hideChangesRequestedPullRequests{};
    const bool displayRepositoryNameLowercase{};
};

class PullRequestMenuEntryFactory
{
    const PullRequestsInfo& m_pullRequests;

public:
    explicit PullRequestMenuEntryFactory(const PullRequestsInfo& pullRequests);
    PullRequestMenuEntryResult GetWaitingMyApprovalMenuEntries(GetWaitingMyApprovalMenuEntriesArgs args) const;
    PullRequestMenuEntryResult GetMyMenuEntries(bool displayRepositoryNameLowercase) const;
};
