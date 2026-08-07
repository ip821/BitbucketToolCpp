#pragma once

#include <nlohmann/json.hpp>

#include "ParticipantUser.h"
#include "PullRequest.h"
#include "Status.h"
#include "Structs.h"
#include "Values.h"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Link, href);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DiffStat, size);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Workspace, slug);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WorkspaceAccess, workspace);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(User, display_name, uuid);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Repository, full_name, slug, workspace);
NLOHMANN_JSON_SERIALIZE_ENUM(
    StatusState,
    {
    {StatusState::Successful, "SUCCESSFUL"},
    {StatusState::Failed, "FAILED"},
    {StatusState::InProgress, "INPROGRESS"},
    {StatusState::Stopped, "STOPPED"},
    }
);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Status, state);
NLOHMANN_JSON_SERIALIZE_ENUM(
    PullRequestState,
    {
    {PullRequestState::Merged, "MERGED"},
    {PullRequestState::Superseded, "SUPERSEDED"},
    {PullRequestState::Open, "OPEN"},
    {PullRequestState::Declined, "DECLINED"},
    }
);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PullRequestLinks, self, html);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Branch, name);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DestinationRepository, name);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Destination, branch, repository);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Source, branch);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PullRequestItem,
                                   id,
                                   title,
                                   author,
                                   links,
                                   created_on,
                                   updated_on,
                                   state,
                                   participants,
                                   draft);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PullRequest,
                                   id,
                                   title,
                                   author,
                                   created_on,
                                   updated_on,
                                   comment_count,
                                   state,
                                   participants,
                                   links,
                                   destination,
                                   source,
                                   draft);
NLOHMANN_JSON_SERIALIZE_ENUM(
    ParticipantRole,
    {
    {ParticipantRole::Reviewer, "REVIEWER"},
    {ParticipantRole::Participant, "PARTICIPANT"},
    {ParticipantRole::Author, "AUTHOR"},
    }
);
NLOHMANN_JSON_SERIALIZE_ENUM(
    ParticipantState,
    {
    {ParticipantState::None, nullptr},
    {ParticipantState::ChangesRequested, "changes_requested"},
    {ParticipantState::Approved, "approved"}
    }
);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ParticipantUser, state, role, approved, user);

// Values<T>
template<typename T>
void to_json(nlohmann::json& j, const Values<T>& obj)
{
    j = nlohmann::json{
        {"values", obj.values},
        {"next", obj.next},
    };
}

template<typename T>
void from_json(const nlohmann::json& j, Values<T>& obj)
{
    j.at("values").get_to(obj.values);
    if (j.contains("next") && !j.at("next").is_null())
        obj.next = j.at("next").get<std::string>();
    else
        obj.next = std::nullopt;
}
