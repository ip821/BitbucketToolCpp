#pragma once
#include <expected>

#include "User.h"
#include "../http/HttpConnection.h"

using CurrentUserResult = std::expected<User, Error>;

class CurrentUserRequest
{
    const HttpConnection& m_connection;

public:
    explicit CurrentUserRequest(const HttpConnection& connection);
    [[nodiscard]] CurrentUserResult GetCurrentUser() const;
};
