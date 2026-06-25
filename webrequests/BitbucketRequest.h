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
    static std::string GetResponseBodyAsStdString(const wxString& responseBody)
    {
        const wxString bodyUtf8 = responseBody.ToUTF8();
        const auto body = bodyUtf8.ToStdString();
        return body;
    }

protected:
    using TResponse = std::expected<TResult, Error>;

    std::expected<TResult, Error> PerformRequest(const wxString& requestUrl) const
    {
        const HttpConnection connection;

        try
        {
            return ip::map_expected(
                connection.HttpGet(requestUrl),
                [](const auto& success)
                {
                    const auto body = GetResponseBodyAsStdString(success.responseBody);
                    const auto jObject = nlohmann::json::parse(body);
                    const auto& response = jObject.template get<TResult>();
                    return response;
                },
                [](const auto& error)
                {
                    const auto body = GetResponseBodyAsStdString(error.responseBody);
                    try
                    {
                        const auto jObject = nlohmann::json::parse(body);

                        if (const auto& serverResponse = jObject.template get<ServerResponse>();
                            serverResponse.error.has_value())
                        {
                            const auto errorMessage = std::format(wxS("{}: {}"), error.message, serverResponse.error.value().message);
                            return Error{errorMessage, error.responseBody};
                        }
                    }
                    catch (const nlohmann::json::exception&)
                    {
                    }
                    return error;
                }
            );
        }
        catch (const nlohmann::json::exception& e)
        {
            const auto exceptionMessage = wxString::FromUTF8(e.what());
            const auto errorMessage = std::format(wxS("Deserialization error: {}"), exceptionMessage);
            return std::unexpected(Error{errorMessage, wxS("")});
        }
    }
};
