#pragma once
#include <expected>

#include "CurlHandle.h"

struct Success
{
    std::string responseBody;
};

struct Error
{
    std::string message;
    std::string responseBody;
};

using HttpResult = std::expected<Success, Error>;

class CurlConnection
{
public:
    CurlConnection() = default;
    ~CurlConnection() = default;

    [[nodiscard]] HttpResult HttpGet(const std::string& url, const std::optional<std::string>& authorizationHeaderValue) const;
    static HttpResult CreateHttpResultFromCurlErrorCode(CURLcode rc, const std::string& responseBody);
    static std::string GetHttpStatusMessage(long code);
};
