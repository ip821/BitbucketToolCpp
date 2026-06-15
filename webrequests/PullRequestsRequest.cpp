#include "PullRequestsRequest.h"

#include <cpp_utils/macros_expected.h>
#include <cpp_utils/wx_string_format.h>

#include "Repository.h"
#include "../Constants.h"

PullRequestsRequest::PullRequestsRequest(const HttpConnection& connection) :
    m_connection(connection)
{
}

PullRequestsResult PullRequestsRequest::GetPullRequests(const Repository& repository, const wxString& userUuid) const
{
    const auto repoKey = std::format(wxS("{0}/{1}"), repository.workspace.slug, repository.slug);

    auto userId = userUuid;
    userId.Replace(wxS("{"), wxS(""));
    userId.Replace(wxS("}"), wxS(""));

    const auto url = BitBucketBaseUrl
            + wxS("/repositories/")
            + repoKey
            + wxS("/pullrequests/")
            + std::format(wxS("?pagelen=50&q=state%3D%22open%22%20AND%20(reviewers.uuid%3D%22{0}%22%20OR%20author.uuid%3D%22{0}%22)"), userId);

    UNWRAP_OR_RETURN_ERROR(success, m_connection.HttpGet(url));

    const auto body = success.body.ToStdString();
    const auto jObject = nlohmann::json::parse(body);
    const auto& response = jObject.get<Values<PullRequestItem>>();
    return PullRequestsSuccess{response.values};
}
