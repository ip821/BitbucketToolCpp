//
// Created by Igor Palkin on 19.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_MYTASKBARICON_H
#define WXWIDGETSPLAYGROUND_MYTASKBARICON_H
#include "MyDialog.h"
#include "wx/taskbar.h"
#include "wx/bmpbndl.h"


class StatusItem : public wxTaskBarIcon
{
public:
    explicit StatusItem();

    void OnLeftButtonDClick(wxTaskBarIconEvent&);
    void OnMenuPreferences(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
    void OnMenuUpdate(wxCommandEvent&);
    void OnMenuCreatePr(wxCommandEvent&);
    virtual wxMenu* CreatePopupMenu() wxOVERRIDE;

    wxDECLARE_EVENT_TABLE();

private:
#ifndef __WXMSW__
    wxBitmapBundle m_bitmapBundle = wxBitmapBundle::FromResources("status32@2x");
#endif
    MyDialog* m_pDialog;
};


#endif //WXWIDGETSPLAYGROUND_MYTASKBARICON_H
