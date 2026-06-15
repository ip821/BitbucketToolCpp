#pragma once
#include <expected>

#include "BitbucketClient.h"
#include "User.h"
#include "../Constants.h"
#include "../http/HttpConnection.h"

using CurrentUserResult = std::expected<User, Error>;

class CurrentUserRequest : public BitbucketClient<User>
{
public:
    TResponse GetCurrentUser() const
    {
        const auto url = BitBucketBaseUrl + wxS("/user/");
        return PerformRequest(url);
    }
};
