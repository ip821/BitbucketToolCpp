#pragma once

#include <expected>
#include <cpp_utils/match_expected.h>
#include <cpp_utils/wx_json.h>
#include <nlohmann/json.hpp>

#include "ServerResponse.h"
#include "../http/HttpConnection.h"

template<typename TResult>
class BitbucketRequest
{
protected:
    using TResponse = std::expected<TResult, Error>;

    std::expected<TResult, Error> PerformRequest(const wxString& requestUrl) const
    {
        HttpConnection connection;

        return ip::map_expected(
            connection.HttpGet(requestUrl),
            [](const auto& success)
            {
                const wxString bodyUtf8 = success.responseBody.ToUTF8();
                const auto body = bodyUtf8.ToStdString();
                const auto jObject = nlohmann::json::parse(body);
                const auto& response = jObject.get<TResult>();
                return response;
            },
            [](const auto& error)
            {
                const wxString bodyUtf8 = error.responseBody.ToUTF8();
                const auto body = bodyUtf8.ToStdString();
                try
                {
                    const auto jObject = nlohmann::json::parse(body);
                    const auto& serverResponse = jObject.get<ServerResponse>();

                    if (serverResponse.error.has_value())
                    {
                        const auto errorMessage = std::format(wxS("{}: {}"), error.message, serverResponse.error.value().message);
                        return Error{errorMessage, error.responseBody};
                    }
                } catch (const nlohmann::json::exception& e)
                {
                }

                return error;
            }
        );
    }
};
