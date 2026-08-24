#include "App.h"

#include <wx/app.h>
#include <wx/filesys.h>
#include <wx/fs_mem.h>
#include <wx/xrc/xmlres.h>

#include "StatusItem.h"

extern void InitXmlResource();

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

  InitXmlResource();

  m_pStatusItem = std::make_unique<StatusItem>();
  return true;
}

int App::OnRun()
{
  const auto exitCode = wxApp::OnRun();

  // Destroy the taskbar icon before wxWidgets deletes all top-level windows.
  m_pStatusItem.reset();
  return exitCode;
}
