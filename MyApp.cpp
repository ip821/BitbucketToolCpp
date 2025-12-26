#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "MyApp.h"

wxIMPLEMENT_APP(MyApp);

MyApp::MyApp()
{
}

bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    m_statusItem = std::make_shared<StatusItem>();
    return true;
}
