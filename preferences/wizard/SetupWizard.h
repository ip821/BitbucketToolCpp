//
// Created by Igor Palkin on 27.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_MYWIZARD_H
#define WXWIDGETSPLAYGROUND_MYWIZARD_H

#include "../../stdafx.h"

class SetupWizard : public wxWizard
{
public:
    explicit SetupWizard(wxWindow* pWindow);
    void Run();

private:
    wxWizardPageSimple* m_pLoginPage;
};


#endif //WXWIDGETSPLAYGROUND_MYWIZARD_H
