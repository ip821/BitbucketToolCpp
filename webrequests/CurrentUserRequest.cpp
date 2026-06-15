#include "CurrentUserRequest.h"

#include <cpp_utils/macros_expected.h>

#include "../Constants.h"

CurrentUserRequest::CurrentUserRequest(const HttpConnection& connection) :
    m_connection(connection)
{
}

CurrentUserResult CurrentUserRequest::GetCurrentUser() const
{
    const auto url = BitBucketBaseUrl + wxS("/user/");

    UNWRAP_OR_RETURN_ERROR(success, m_connection.HttpGet(url));

    const auto jObject = nlohmann::json::parse(success.body.ToStdString());
    const auto& response = jObject.get<User>();
    return CurrentUserResult{response};
}
