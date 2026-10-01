#pragma once

#include <memory>

class wxMenu;
class wxString;

// Small wrapper around libappindicator3, used only on Linux.
class AppIndicatorStatusIndicator
{
public:
    explicit AppIndicatorStatusIndicator(wxMenu& menu);
    ~AppIndicatorStatusIndicator();

    AppIndicatorStatusIndicator(const AppIndicatorStatusIndicator&) = delete;
    AppIndicatorStatusIndicator& operator=(const AppIndicatorStatusIndicator&) = delete;

    [[nodiscard]] bool IsAvailable() const;
    void SetTitle(const wxString& title, bool hasAlert);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
