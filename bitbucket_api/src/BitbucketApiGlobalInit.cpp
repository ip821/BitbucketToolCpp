#include "bitbucket_api/BitbucketApiGlobalInit.h"

#include <cpp_curl/CurlGlobalInit.h>

class BitbucketApiGlobalInit::Impl
{
    CurlGlobalInit m_curlGlobalInit;
};

BitbucketApiGlobalInit::BitbucketApiGlobalInit()
    : m_pImpl(std::make_unique<Impl>())
{
}

BitbucketApiGlobalInit::~BitbucketApiGlobalInit() = default;
