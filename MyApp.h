//
// Created by Igor Palkin on 19.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_MYAPP_H
#define WXWIDGETSPLAYGROUND_MYAPP_H
#include "MyDialog.h"
#include "StatusItem.h"
#include "wx/app.h"

class MyApp : public wxApp
{
public:
    explicit MyApp();
    virtual bool OnInit() wxOVERRIDE;

private:
    std::shared_ptr<StatusItem> m_statusItem;
};

#endif //WXWIDGETSPLAYGROUND_MYAPP_H