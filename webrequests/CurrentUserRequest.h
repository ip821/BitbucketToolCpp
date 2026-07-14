#pragma once

#include <expected>

#include "BitbucketRequest.h"
#include "BitbucketUrlBuilder.h"
#include "User.h"

class CurrentUserRequest : public BitbucketRequest<User>
{
public:
    TResponse GetCurrentUser() const
    {
        return PerformRequest(BitbucketUrlBuilder::GetCurrentUserUrl());
    }
};
