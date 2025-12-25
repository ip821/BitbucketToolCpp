//
// Created by Igor Palkin on 19.12.2025.
//

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/taskbar.h"

#include "MyApp.h"

#include "StatusItem.h"
#include "swift/file.h"

wxIMPLEMENT_APP(MyApp);

MyApp::MyApp()
{
    swift_create_status_item();
}

bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    if (!wxTaskBarIcon::IsAvailable())
    {
        wxMessageBox
        (
            "There appears to be no system tray support in your current environment. This sample may not behave as expected.",
            "Warning",
            wxOK | wxICON_EXCLAMATION
        );
    }

    m_statusItem = std::make_shared<StatusItem>();
    return true;
}
