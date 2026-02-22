//
// Created by Igor Palkin on 27.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_WORKSPACEPAGE_H
#define WXWIDGETSPLAYGROUND_WORKSPACEPAGE_H

#include "SetupWizard.h"
#include "../../stdafx.h"

class WorkspacePage : public wxWizardPageSimple
{
    SetupWizardContext& m_context;

    wxStaticBox& m_staticBox;

public:
    explicit WorkspacePage(SetupWizard* pWizard, SetupWizardContext& context);
};


#endif //WXWIDGETSPLAYGROUND_WORKSPACEPAGE_H
