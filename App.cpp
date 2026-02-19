#include "stdafx.h"
#include "App.h"
#include "StatusItem.h"

extern void InitXmlResource();

wxIMPLEMENT_APP(App);

App::App()
{
  // Enable legacy systray icons support
#ifdef __WXGTK__
  wxSetEnv("GDK_BACKEND", "x11");
#endif
}

bool App::OnInit()
{
  if (!wxApp::OnInit())
    return false;

  wxFileSystem::AddHandler(new wxMemoryFSHandler);
  wxInitAllImageHandlers();
  wxXmlResource::Get()->InitAllHandlers();

#ifndef __WXOSX__
  InitXmlResource();
#endif

  m_statusItem = std::make_shared<StatusItem>();
  return true;
}
