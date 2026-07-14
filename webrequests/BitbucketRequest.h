#pragma once

#include <expected>
#include <cpp_utils/match_expected.h>
#include <cpp_utils/wx_json.h>
#include <nlohmann/json.hpp>

#include "ServerResponse.h"
#include "../http/CurlConnection.h"
#include "../preferences/Credentials.h"

struct BitbucketError
{
    wxString message;
};

template<typename TResult>
class BitbucketRequest
{
protected:
    using TResponse = std::expected<TResult, BitbucketError>;

    TResponse PerformRequest(const wxString& requestUrl) const
    {
        const CurlConnection connection;
        const auto credentialsBase64 = Credentials::GetCredentialsBase64();

        try
        {
            return ip::map_expected(
                connection.HttpGet(requestUrl.ToUTF8().data(), credentialsBase64.ToUTF8().data()),
                [](const auto& success)
                {
                    const auto body = success.responseBody;
                    const auto jObject = nlohmann::json::parse(body);
                    const auto& response = jObject.template get<TResult>();
                    return response;
                },
                [](const auto& error)
                {
                    const auto body = error.responseBody;
                    try
                    {
                        const auto jObject = nlohmann::json::parse(body);

                        if (const auto& serverResponse = jObject.template get<ServerResponse>();
                            serverResponse.error.has_value())
                        {
                            const auto errorMessage = std::format(wxS("{}: {}"), wxString(error.message), serverResponse.error.value().message);
                            return BitbucketError{errorMessage};
                        }
                    }
                    catch (const nlohmann::json::exception&)
                    {
                    }
                    return BitbucketError{wxString(error.message)};
                }
            );
        }
        catch (const nlohmann::json::exception& e)
        {
            const auto exceptionMessage = e.what();
            const auto errorMessage = std::format(wxS("Deserialization error: {}"), wxString(exceptionMessage));
            return std::unexpected(BitbucketError{errorMessage});
        }
    }
};
