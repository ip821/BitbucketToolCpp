//
// Created by Igor Palkin on 19.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_MYTASKBARICON_H
#define WXWIDGETSPLAYGROUND_MYTASKBARICON_H

#include "stdafx.h"

class PreferencesWindow;

class StatusItem : public wxTaskBarIcon
{
public:
  explicit StatusItem();

  void OnLeftButtonDClick(wxTaskBarIconEvent &);
  void OnMenuPreferences(wxCommandEvent &);
  void OnMenuExit(wxCommandEvent &);
  void OnMenuUpdate(wxCommandEvent &);
  void OnMenuCreatePr(wxCommandEvent &);
  virtual wxMenu *CreatePopupMenu() wxOVERRIDE;

  wxDECLARE_EVENT_TABLE();

private:
  PreferencesWindow *m_pDialog;
  void ShowPreferencesDialog();

#ifdef __WXOSX__
  wxBitmapBundle m_bitmapBundle = wxBitmapBundle::FromResources("status32@2x");
#endif

#ifndef __WXOSX__
  wxBitmap m_statusBitmap;
  wxBitmapBundle m_bitmapBundle;
#endif
};

#endif // WXWIDGETSPLAYGROUND_MYTASKBARICON_H
