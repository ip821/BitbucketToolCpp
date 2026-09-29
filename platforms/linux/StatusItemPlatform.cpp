#include "../StatusItemPlatform.h"

#include <format>
#include <utility>
#include <wx/bmpbndl.h>
#include <wx/taskbar.h>
#include <wx/wx.h>

#include "../CustomIcon.h"

struct StatusItemPlatform::Impl
{
    wxBitmapBundle bitmapBundle;
};

namespace
{
    wxBitmapBundle CreateReviewCountIcon(const wxString& text, const bool hasAlert)
    {
        return wxBitmapBundle::FromBitmaps(
            CustomIcon::CreateReviewCountBitmap(text, 16, hasAlert),
            CustomIcon::CreateReviewCountBitmap(text, 32, hasAlert));
    }
}

StatusItemPlatform::StatusItemPlatform() :
    m_impl(std::make_unique<Impl>())
{
}

StatusItemPlatform::~StatusItemPlatform() = default;

void StatusItemPlatform::Initialize(wxTaskBarIcon& statusItem, ActivationHandler onActivate)
{
    if (!statusItem.IsAvailable())
        wxMessageBox("System icon is not available");

    UpdateTitle(statusItem, 0, 0, false);

    // GTK reports tray icon activation as LEFT_DOWN and doesn't emit LEFT_UP.
    statusItem.Bind(
        wxEVT_TASKBAR_LEFT_DOWN,
        [onActivate = std::move(onActivate)](wxTaskBarIconEvent&)
        {
            onActivate();
        });
}

void StatusItemPlatform::UpdateTitle(
    wxTaskBarIcon& statusItem,
    const size_t waitingCount,
    [[maybe_unused]] const size_t myCount,
    const bool hasAlert)
{
    const auto title = std::format(wxS("{}"), waitingCount);
    m_impl->bitmapBundle = CreateReviewCountIcon(title, hasAlert);
    if (!m_impl->bitmapBundle.IsOk())
        return;

    const auto tooltipTitle = title.IsEmpty() ? wxS("none") : title;
    statusItem.SetIcon(m_impl->bitmapBundle, std::format(wxS("Pull requests: {}"), tooltipTitle));
}

void StatusItemPlatform::OnEventQueued()
{
}
