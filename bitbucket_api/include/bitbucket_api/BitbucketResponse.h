#pragma once
#include <expected>
#include <string>

struct BitbucketError
{
    std::string message;
};

template<typename TResult>
using BitbucketResponse = std::expected<TResult, BitbucketError>;
