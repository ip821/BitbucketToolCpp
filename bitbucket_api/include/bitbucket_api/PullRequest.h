#pragma once

#include "bitbucket_api/ParticipantUser.h"
#include "bitbucket_api/Structs.h"
#include "bitbucket_api/Values.h"

enum class PullRequestState
{
    Merged = 1,
    Superseded,
    Open,
    Declined,
};

struct PullRequestLinks
{
    Link self{};
    Link html{};
};

struct Branch
{
    std::string name;
};

struct DestinationRepository
{
    std::string name{};
};

struct Destination
{
    Branch branch{};
    DestinationRepository repository{};
};

struct Source
{
    Branch branch{};
};

struct PullRequest
{
    int id{};
    std::string title{};
    User author{};
    std::string created_on{};
    std::string updated_on{};
    int comment_count{};
    PullRequestState state{};
    std::vector<ParticipantUser> participants{};
    PullRequestLinks links{};
    Destination destination{};
    Source source{};
    bool draft{};

    [[nodiscard]] bool IsWaitingForUserApproval(const User& user) const;
    [[nodiscard]] bool IsUserPullRequest(const User& user) const;
};
