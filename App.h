#pragma once

#include "http/CurlGlobalInit.h"

class StatusItem;

class App : public wxApp
{
public:
    explicit App();
    bool OnInit() override;

private:
    CurlGlobalInit m_curlGlobalInit;
    std::unique_ptr<StatusItem> m_pStatusItem;
};
