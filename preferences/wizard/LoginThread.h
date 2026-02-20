//
// Created by Igor Palkin on 19.02.2026.
//

#ifndef PRTOOLFORBITBUCKET2_LOGINTHREAD_H
#define PRTOOLFORBITBUCKET2_LOGINTHREAD_H

#include "wx/thread.h"

class wxWindow;
class wxWizard;
class SetupWizard;
class LoginPage;

class LoginThread : public wxThread
{
    ExitCode Entry() override;
    wxWindow& m_window;

public:
    explicit LoginThread(wxWindow* pSetupWizard);
};


#endif //PRTOOLFORBITBUCKET2_LOGINTHREAD_H
