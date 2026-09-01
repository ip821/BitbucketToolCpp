#pragma once

#include <expected>
#include <vector>

#include <cpp_curl/CurlConnection.h>
#include <cpp_utils/match_expected.h>
#include <nlohmann/json.hpp>

#include "ServerResponse.h"
#include "bitbucket_api/BitbucketResponse.h"

template<typename TResult>
class BitbucketRequest
{
    std::string m_requestUrl;

public:
    explicit BitbucketRequest(const std::string& requestUrl) : m_requestUrl(requestUrl)
    {
    }

    BitbucketResponse<TResult> Perform(const std::string& authToken) const
    {
        const cpp_curl::CurlConnection connection;
        const std::vector<std::string> headers{
            "Accept: application/json",
            "Authorization: Basic " + authToken
        };

        try
        {
            return ip::map_expected(
                connection.HttpGet(m_requestUrl, headers),
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
                            const auto errorMessage = std::format("{}: {}", error.message, serverResponse.error.value().message);
                            return BitbucketError{errorMessage};
                        }
                    } catch (const nlohmann::json::exception&)
                    {
                    }
                    return BitbucketError{error.message};
                }
            );
        } catch (const nlohmann::json::exception& e)
        {
            const auto exceptionMessage = e.what();
            const auto errorMessage = std::format("Deserialization error: {}", exceptionMessage);
            return std::unexpected(BitbucketError{errorMessage});
        }
    }
};
