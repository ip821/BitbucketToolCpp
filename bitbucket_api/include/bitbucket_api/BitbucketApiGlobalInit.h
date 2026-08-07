#pragma once

#include <memory>

class BitbucketApiGlobalInit
{
    class Impl;
    std::unique_ptr<Impl> m_pImpl;

public:
    BitbucketApiGlobalInit();
    ~BitbucketApiGlobalInit();

    BitbucketApiGlobalInit(const BitbucketApiGlobalInit&) = delete;
    BitbucketApiGlobalInit& operator=(const BitbucketApiGlobalInit&) = delete;
};
