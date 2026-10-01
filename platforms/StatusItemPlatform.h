#pragma once

#include <cstddef>
#include <functional>
#include <memory>

class wxTaskBarIcon;
class wxMenu;

class StatusItemPlatform
{
    struct Impl;
    std::unique_ptr<Impl> m_impl;

public:
    using ActivationHandler = std::function<void()>;

    StatusItemPlatform();
    ~StatusItemPlatform();

    StatusItemPlatform(const StatusItemPlatform&) = delete;
    StatusItemPlatform& operator=(const StatusItemPlatform&) = delete;

    void Initialize(wxTaskBarIcon& statusItem, wxMenu& menu, ActivationHandler onActivate);
    void UpdateTitle(
        wxTaskBarIcon& statusItem,
        size_t waitingCount,
        size_t myCount,
        bool hasAlert);
    void OnEventQueued();
};
