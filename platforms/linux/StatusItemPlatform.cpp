#include "../StatusItemPlatform.h"

#include <format>
#include <memory>
#include <wx/taskbar.h>
#include <wx/wx.h>

#include "AppIndicatorStatusIndicator.h"

struct StatusItemPlatform::Impl
{
    std::unique_ptr<AppIndicatorStatusIndicator> indicator;
};

StatusItemPlatform::StatusItemPlatform() :
    m_impl(std::make_unique<Impl>())
{
}

StatusItemPlatform::~StatusItemPlatform() = default;

void StatusItemPlatform::Initialize(
    wxTaskBarIcon& statusItem,
    wxMenu& menu,
    [[maybe_unused]] ActivationHandler onActivate)
{
    m_impl->indicator = std::make_unique<AppIndicatorStatusIndicator>(menu);
    if (!m_impl->indicator->IsAvailable())
        wxMessageBox("Could not create the application indicator.");

    UpdateTitle(statusItem, 0, 0, false);
}

void StatusItemPlatform::UpdateTitle(
    [[maybe_unused]] wxTaskBarIcon& statusItem,
    const size_t waitingCount,
    [[maybe_unused]] const size_t myCount,
    const bool hasAlert)
{
    const auto title = std::format(wxS("{}"), waitingCount);
    m_impl->indicator->SetTitle(title, hasAlert);
}

void StatusItemPlatform::OnEventQueued()
{
}
