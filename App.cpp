#include "App.h"

#include <wx/app.h>

#if !defined(__WXOSX__) && !defined(__WXMSW__)
#include <wx/image.h>
#endif

#include "StatusItem.h"

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

#if !defined(__WXOSX__) && !defined(__WXMSW__)
  // The embedded status icon is PNG, so avoid pulling every image decoder into
  // the statically linked executable.
  wxImage::AddHandler(new wxPNGHandler);
#endif

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
