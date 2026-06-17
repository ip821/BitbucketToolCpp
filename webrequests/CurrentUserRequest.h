#pragma once

#include <expected>

#include "BitbucketRequest.h"
#include "BitbucketUrlBuilder.h"
#include "User.h"
#include "../http/HttpConnection.h"

using CurrentUserResult = std::expected<User, Error>;

class CurrentUserRequest : public BitbucketRequest<User>
{
public:
    TResponse GetCurrentUser() const
    {
        return PerformRequest(BitbucketUrlBuilder::GetCurrentUserUrl());
    }
};
