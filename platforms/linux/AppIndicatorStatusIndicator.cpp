#include "AppIndicatorStatusIndicator.h"

#include <array>

#include <libappindicator/app-indicator.h>

#include <wx/filename.h>
#include <wx/menu.h>
#include <wx/string.h>
#include <wx/utils.h>

#include "../CustomIcon.h"

struct AppIndicatorStatusIndicator::Impl
{
    AppIndicator* indicator{};
    std::array<wxString, 2> iconPaths;
    size_t nextIcon{};

    ~Impl()
    {
        if (indicator)
            g_object_unref(indicator);
        for (const auto& path: iconPaths)
        {
            if (!path.IsEmpty())
                wxRemoveFile(path);
        }
    }
};

AppIndicatorStatusIndicator::AppIndicatorStatusIndicator(wxMenu& menu) :
    m_impl(std::make_unique<Impl>())
{
    m_impl->indicator = app_indicator_new(
        "pr-tool-for-bitbucket",
        "pr-tool-for-bitbucket",
        APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
    if (!m_impl->indicator)
        return;

    // wxGTK's native menu is a GtkMenu. AppIndicator does not take ownership.
    app_indicator_set_menu(m_impl->indicator, GTK_MENU(menu.m_menu));
    app_indicator_set_status(m_impl->indicator, APP_INDICATOR_STATUS_ACTIVE);
}

AppIndicatorStatusIndicator::~AppIndicatorStatusIndicator() = default;

bool AppIndicatorStatusIndicator::IsAvailable() const
{
    return m_impl->indicator != nullptr;
}

void AppIndicatorStatusIndicator::SetTitle(const wxString& title, const bool hasAlert)
{
    if (!IsAvailable())
        return;

    auto bitmap = CustomIcon::CreateReviewCountBitmap(title, 32, hasAlert, true);
    if (!bitmap.IsOk())
        return;

    auto& path = m_impl->iconPaths[m_impl->nextIcon];
    if (path.IsEmpty())
    {
        path = wxFileName::CreateTempFileName("pr-tool-for-bitbucket-");
    }
    if (!bitmap.SaveFile(path, wxBITMAP_TYPE_PNG))
        return;

    const auto tooltipTitle = title.IsEmpty() ? wxS("none") : title;
    const auto description = wxS("Pull requests to review: ") + tooltipTitle;
    app_indicator_set_icon_full(
        m_impl->indicator,
        path.utf8_str(),
        description.utf8_str());
    m_impl->nextIcon = (m_impl->nextIcon + 1) % m_impl->iconPaths.size();
}
