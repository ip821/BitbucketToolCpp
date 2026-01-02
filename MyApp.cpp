#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "MyApp.h"

#include "wx/fs_mem.h"
#include "wx/xrc/xmlres.h"

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
  InitXmlResource();

  m_statusItem = std::make_shared<StatusItem>();
  return true;
}
