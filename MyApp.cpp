#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "MyApp.h"

#include "wx/fs_mem.h"
#include "wx/xrc/xmlres.h"

extern void InitXmlResource();

wxIMPLEMENT_APP(MyApp);

MyApp::MyApp() {}

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
