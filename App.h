#pragma once

#include <wx/wx.h>

#include "bitbucket_api/BitbucketApiGlobalInit.h"

class StatusItem;

class App : public wxApp
{
public:
    explicit App();
    bool OnInit() override;
    int OnRun() override;

private:
    BitbucketApiGlobalInit m_bitbucketApiGlobalInit;
    std::unique_ptr<StatusItem> m_pStatusItem;
};
