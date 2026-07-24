#pragma once

#include <ranges>
#include <ranges>
#include <ranges>
#include <wx/wx.h>
#include <cpp_utils/wx_string_format.h>

#include "../bitbucket_api/include/bitbucket_api/PullRequest.h"
#include "../bitbucket_api/include/bitbucket_api/Status.h"

struct PullRequestInfo
{
    PullRequest pullRequest;
    std::vector<Status> statuses;
    DiffStat diffStat;

    wxString GetMainMenuItemTitle() const
    {
        const auto draftPrefix = pullRequest.draft ? wxS("[DRAFT] - ") : wxS("");
        return std::format(wxS("{}{}"), draftPrefix, wxString::FromUTF8(pullRequest.title));
    }

    wxString GetAuthorAndBranchMenuItemTitle() const
    {
        return std::format(
            wxS("[{}] [{}] [{}] → [{}]"),
            wxString::FromUTF8(pullRequest.destination.repository.name),
            wxString::FromUTF8(pullRequest.author.display_name),
            wxString::FromUTF8(pullRequest.source.branch.name),
            wxString::FromUTF8(pullRequest.destination.branch.name));
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
            wxString::FromUTF8(pullRequest.source.branch.name),
            wxString::FromUTF8(pullRequest.destination.branch.name)
        );
    }

    wxString GetParticipantMenuItemTitle(const ParticipantUser& participant) const
    {
        const auto& symbol = participant.approved
                                 ? wxS("✔")
                                 : participant.AreChangesRequested()
                                       ? wxS("✘")
                                       : wxS("...");
        wxString participantMenuItemTitle = std::format(wxS("{} {}"), symbol, wxString::FromUTF8(participant.user.display_name));
        return participantMenuItemTitle;
    }

    std::vector<ParticipantUser> GetParticipantsRequestedChanges() const
    {
        return pullRequest.participants
            | std::views::filter([](const auto& it) { return it.AreChangesRequested(); })
            | std::ranges::to<std::vector>();
    }

    std::vector<ParticipantUser> GetParticipantsRequestedChangesWithout(const User& user) const
    {
        return GetParticipantsRequestedChanges()
            | std::views::filter([&user](const auto& it) { return it.user.uuid != user.uuid; })
            | std::ranges::to<std::vector>();
    }
};
