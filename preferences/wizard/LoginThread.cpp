//
// Created by Igor Palkin on 19.02.2026.
//

#include "LoginThread.h"

#include "SetupWizard.h"
#include "../../stdafx.h"
#include "wx/event.h"
#include "wx/utils.h"

LoginThread::LoginThread(wxWindow* pWindow) :
    wxThread(wxTHREAD_DETACHED),
    m_window(*pWindow)
{
}

wxThread::ExitCode LoginThread::Entry()
{
    // Simulate long work (replace with real API call)
    wxMilliSleep(1500);

    wxThreadEvent event(wxEVT_THREAD, wxID_LOGIN_THREAD_COMPLETED);
    wxQueueEvent(&m_window, event.Clone());

    return nullptr;
}
