#include "stdafx.h"
#include "MyApp.h"
#include "StatusItem.h"

extern void InitXmlResource();

wxIMPLEMENT_APP(MyApp);

MyApp::MyApp()
{
  // Enable legacy systray icons support
#ifdef __WXGTK__
  wxSetEnv("GDK_BACKEND", "x11");
#endif
}

bool MyApp::OnInit()
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
