#pragma once

#include "bitbucket_api/BitbucketApiGlobalInit.h"

class StatusItem;

class App : public wxApp
{
public:
    explicit App();
    bool OnInit() override;

private:
    BitbucketApiGlobalInit m_bitbucketApiGlobalInit;
    std::unique_ptr<StatusItem> m_pStatusItem;
};
