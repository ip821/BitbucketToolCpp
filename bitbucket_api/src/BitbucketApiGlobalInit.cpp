#include "bitbucket_api/BitbucketApiGlobalInit.h"

#include <cpp_curl/CurlGlobalInit.h>

BitbucketApiGlobalInit::BitbucketApiGlobalInit() : m_pCurlGlobalInit(std::make_shared<CurlGlobalInit>())
{
}
