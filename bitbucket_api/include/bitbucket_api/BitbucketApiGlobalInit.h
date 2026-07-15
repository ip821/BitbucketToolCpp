#pragma once

#include <memory>

class CurlGlobalInit;

class BitbucketApiGlobalInit
{
    std::shared_ptr<CurlGlobalInit> m_pCurlGlobalInit;
public:
    explicit BitbucketApiGlobalInit();
};
