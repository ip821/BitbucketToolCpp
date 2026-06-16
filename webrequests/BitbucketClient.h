#pragma once
#include <expected>
#include <cpp_utils/macros_expected.h>
#include <cpp_utils/wx_json.h>
#include <nlohmann/json.hpp>

#include "../http/HttpConnection.h"

template<typename TResult>
class BitbucketClient
{
protected:
    using TResponse = std::expected<TResult, Error>;

    std::expected<TResult, Error> PerformRequest(const wxString& requestUrl) const
    {
        HttpConnection connection;
        UNWRAP_OR_RETURN_ERROR(success, connection.HttpGet(requestUrl));

        const wxString bodyUtf8 = success.body.ToUTF8();
        const auto body = bodyUtf8.ToStdString();
        const auto jObject = nlohmann::json::parse(body);
        const auto& response = jObject.get<TResult>();
        return response;
    }
};
