//
// Created by Igor Palkin on 19.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_MYAPP_H
#define WXWIDGETSPLAYGROUND_MYAPP_H

#include "stdafx.h"

class StatusItem;

class MyApp : public wxApp
{
public:
    explicit MyApp();
    bool OnInit() override;

private:
    std::shared_ptr<StatusItem> m_statusItem;
};

#endif //WXWIDGETSPLAYGROUND_MYAPP_H
