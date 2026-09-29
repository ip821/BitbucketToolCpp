#include "../StatusItemPlatform.h"

#include <format>
#include <wx/taskbar.h>
#include <wx/wx.h>

struct StatusItemPlatform::Impl
{
};

StatusItemPlatform::StatusItemPlatform() :
    m_impl(std::make_unique<Impl>())
{
}

StatusItemPlatform::~StatusItemPlatform() = default;

void StatusItemPlatform::Initialize(
    wxTaskBarIcon& statusItem,
    [[maybe_unused]] ActivationHandler onActivate)
{
    statusItem.SetIcon("status32@2x");
}

void StatusItemPlatform::UpdateTitle(
    wxTaskBarIcon& statusItem,
    const size_t waitingCount,
    const size_t myCount,
    const bool hasAlert)
{
    wxString title;
    if (waitingCount || myCount)
    {
        if (myCount)
        {
            title = std::format(wxS("{}/{}"), waitingCount, myCount);
            if (hasAlert)
                title += wxS(" (!)");
        } else
        {
            title = std::format(wxS("{}"), waitingCount);
        }
    }

    statusItem.SetTitle(title);
}

void StatusItemPlatform::OnEventQueued()
{
}
