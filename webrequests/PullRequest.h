#pragma once

#include "wx/string.h"
#include <cpp_utils/wx_json.h>
#include <cpp_utils/wx_string_format.h>

#include "Link.h"
#include "Participant.h"
#include "User.h"
#include "Values.h"

enum PullRequestState
{
    Uninitialized,
    Merged,
    Superseded,
    Open,
    Declined,
};
NLOHMANN_JSON_SERIALIZE_ENUM(
    PullRequestState,
    {
    { PullRequestState::Merged, "MERGED" },
    { PullRequestState::Superseded, "SUPERSEDED" },
    { PullRequestState::Open, "OPEN" },
    { PullRequestState::Declined, "DECLINED" },
    }
);

struct PullRequestLinks
{
    Link self{};
    Link html{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PullRequestLinks, self, html);

struct Branch
{
    wxString name;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Branch, name);

struct DestinationRepository
{
    wxString name{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DestinationRepository, name);

struct Destination
{
    Branch branch{};
    DestinationRepository repository{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Destination, branch, repository);

struct Source
{
    Branch branch{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Source, branch);

struct PullRequestItem
{
    int id{};
    wxString title{};
    User author{};
    PullRequestLinks links{};
    wxString created_on{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PullRequestItem, id, title, author, links, created_on);

struct PullRequest
{
    wxString title{};
    User author{};
    wxString created_on{};
    int comment_count{};
    PullRequestState state{};
    std::vector<struct Participant> participants{};
    PullRequestLinks links{};
    Destination destination{};
    Source source{};
    bool draft{};

    std::optional<struct Participant> GetParticipantForUser(const User& user) const
    {
        const auto filtered = participants
                | std::views::filter([&user](const auto& it) { return it.user.uuid == user.uuid; })
                | std::ranges::to<std::vector>();

        if (filtered.empty())
            return std::nullopt;

        return *filtered.cbegin();
    }

    wxString GetTitle() const
    {
        return std::format(wxS("[{}] - {}"), destination.repository.name, title);
    }
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PullRequest,
                                   title,
                                   author,
                                   created_on,
                                   comment_count,
                                   state,
                                   participants,
                                   links,
                                   destination,
                                   source,
                                   draft);
