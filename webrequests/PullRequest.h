#pragma once

#include "wx/string.h"
#include <cpp_utils/wx_json.h>

#include "Link.h"
#include "Participant.h"
#include "User.h"

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

struct Destination
{
    Branch branch{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Destination, branch);

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
    wxString name{};
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
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PullRequest,
                                   name,
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

struct PullRequestsResponse
{
    std::vector<PullRequestItem> values{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PullRequestsResponse, values);
