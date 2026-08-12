#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <wx/frame.h>

#include "PullRequestsInfo.h"
#include "../bitbucket_api/include/bitbucket_api/Structs.h"

class wxBoxSizer;
class wxButton;
class wxCheckBox;
class wxGauge;
class wxScrolledWindow;
class wxStaticText;
class wxTimer;

struct PullRequestsWindowCallbacks
{
    std::function<void()> createPullRequest;
    std::function<void()> update;
    std::function<void()> showPreferences;
    std::function<void(bool)> showAllPullRequests;
};

class PullRequestsWindow final : public wxFrame
{
public:
    explicit PullRequestsWindow(PullRequestsWindowCallbacks callbacks);

    void SetPullRequests(
        const PullRequestsInfo& pullRequestsInfo,
        bool showAllPullRequests,
        bool hideChangesRequestedPullRequests,
        bool useTwoColumnLayout);
    void SetRepositories(const std::vector<Repository>& repositories);
    void SetUpdating(bool updating);
    void SetUpdateProgress(bool isFetchingDetails, size_t completed, size_t total);
    void SetUpdateStatusText(const wxString& status);
    void ToggleNear(const wxPoint& anchor);

private:
    struct Section
    {
        wxStaticText* title{};
        wxScrolledWindow* scrolledWindow{};
        wxBoxSizer* itemsSizer{};
    };

    PullRequestsWindowCallbacks m_callbacks;
    PullRequestsInfo m_pullRequestsInfo;
    bool m_hasPullRequests{};
    bool m_showAllPullRequests{};
    bool m_hideChangesRequestedPullRequests{};
    bool m_useTwoColumnLayout{true};
    bool m_updating{};
    size_t m_repositoryCount{};

    wxButton* m_pCreatePullRequestButton{};
    wxButton* m_pUpdateButton{};
    wxCheckBox* m_pShowAllCheckBox{};
    wxGauge* m_pProgressGauge{};
    std::unique_ptr<wxTimer> m_pProgressPulseTimer;
    wxStaticText* m_pStatusText{};
    wxBoxSizer* m_pSectionsSizer{};
    Section m_reviewSection;
    Section m_mySection;

    Section CreateSection(wxWindow* parent);
    void RefreshContent();
    void RefreshSection(
        const Section& section,
        const wxString& title,
        const std::vector<const PullRequestInfo*>& pullRequests,
        bool isMyPullRequest) const;
    wxWindow* CreatePullRequestCard(
        wxWindow* parent,
        const PullRequestInfo& pullRequest,
        bool isMyPullRequest) const;
    void ApplyLayout();
    void PositionNear(const wxPoint& anchor);
};
