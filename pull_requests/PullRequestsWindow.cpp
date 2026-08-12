#include "PullRequestsWindow.h"

#include <algorithm>
#include <ranges>
#include <utility>

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/clipbrd.h>
#include <wx/display.h>
#include <wx/gauge.h>
#include <wx/scrolwin.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/timer.h>
#include <wx/wx.h>

namespace
{
    constexpr auto columnWidth = 500;
    constexpr auto windowMargin = 16;
    constexpr auto cardMargin = 8;
    constexpr auto minimumClientHeight = 400;
    constexpr auto preferredClientHeight = 700;

    constexpr auto twoColumnClientWidth = 2 * columnWidth + 3 * windowMargin;
    constexpr auto singleColumnClientWidth = columnWidth + 2 * windowMargin;

    wxString JoinParticipantTitles(const std::vector<ParticipantUser>& participants, const PullRequestInfo& pullRequest)
    {
        wxString result;
        for (const auto& participant: participants)
        {
            if (!result.IsEmpty())
                result += wxS("   ");

            result += pullRequest.GetParticipantMenuItemTitle(participant);
        }
        return result;
    }

    void OpenPullRequest(const wxString& href)
    {
        if (wxGetKeyState(WXK_ALT))
        {
            if (wxTheClipboard->Open())
            {
                wxTheClipboard->SetData(new wxTextDataObject(href));
                wxTheClipboard->Close();
            }
            return;
        }

        wxLaunchDefaultBrowser(href);
    }

    void SetEllipsizedLabel(wxStaticText* label, const wxString& text)
    {
        label->SetLabelText(text);
        label->SetToolTip(text);
    }

    long GetPullRequestsWindowStyle()
    {
        auto style = wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER | wxFRAME_NO_TASKBAR;
#if !defined(__WXOSX__)
        style |= wxFRAME_TOOL_WINDOW;
#endif
        return style;
    }
}

PullRequestsWindow::PullRequestsWindow(PullRequestsWindowCallbacks callbacks) :
    wxFrame(
        nullptr,
        wxID_ANY,
        "Pull requests",
        wxDefaultPosition,
        wxDefaultSize,
        GetPullRequestsWindowStyle()),
    m_callbacks(std::move(callbacks))
{
    auto* rootSizer = new wxBoxSizer(wxVERTICAL);
    auto* toolbarSizer = new wxBoxSizer(wxHORIZONTAL);

    m_pCreatePullRequestButton = new wxButton(this, wxID_ANY, "Create pull request...");
    m_pCreatePullRequestButton->Enable(false);
    m_pCreatePullRequestButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent&)
    {
        m_callbacks.createPullRequest();
    });
    toolbarSizer->Add(m_pCreatePullRequestButton, 0, wxRIGHT, FromDIP(6));

    m_pUpdateButton = new wxButton(this, wxID_ANY, "Update");
    m_pUpdateButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent&)
    {
        m_callbacks.update();
    });
    toolbarSizer->Add(m_pUpdateButton, 0, wxRIGHT, FromDIP(12));

    m_pShowAllCheckBox = new wxCheckBox(this, wxID_ANY, "Show hidden pull requests");
    m_pShowAllCheckBox->Hide();
    m_pShowAllCheckBox->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent& event)
    {
        m_callbacks.showAllPullRequests(event.IsChecked());
    });
    toolbarSizer->Add(m_pShowAllCheckBox, 0, wxALIGN_CENTER_VERTICAL);
    toolbarSizer->AddStretchSpacer();

    auto* preferencesButton = new wxButton(this, wxID_ANY, "Preferences...");
    preferencesButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent&)
    {
        m_callbacks.showPreferences();
    });
    toolbarSizer->Add(preferencesButton);

    rootSizer->Add(toolbarSizer, 0, wxEXPAND | wxALL, FromDIP(windowMargin));
    rootSizer->Add(new wxStaticLine(this), 0, wxEXPAND);

    m_pSectionsSizer = new wxBoxSizer(wxHORIZONTAL);
    m_reviewSection = CreateSection(this);
    m_mySection = CreateSection(this);
    m_reviewSection.title->GetParent()->SetMinSize(wxSize(FromDIP(columnWidth), wxDefaultCoord));
    m_mySection.title->GetParent()->SetMinSize(wxSize(FromDIP(columnWidth), wxDefaultCoord));
    m_pSectionsSizer->Add(m_reviewSection.title->GetParent(), 1, wxEXPAND);
    m_pSectionsSizer->AddSpacer(FromDIP(windowMargin));
    m_pSectionsSizer->Add(m_mySection.title->GetParent(), 1, wxEXPAND);
    rootSizer->Add(m_pSectionsSizer, 1, wxEXPAND | wxALL, FromDIP(windowMargin));

    rootSizer->Add(new wxStaticLine(this), 0, wxEXPAND);
    auto* footerSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pStatusText = new wxStaticText(this, wxID_ANY, "Pull requests have not been loaded yet.", wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
    footerSizer->Add(m_pStatusText, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(windowMargin));
    m_pProgressGauge = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, FromDIP(wxSize(180, 16)), wxGA_HORIZONTAL | wxGA_SMOOTH);
    m_pProgressGauge->Hide();
    footerSizer->Add(m_pProgressGauge, 0, wxALIGN_CENTER_VERTICAL);
    m_pProgressPulseTimer = std::make_unique<wxTimer>(this);
    Bind(wxEVT_TIMER, [this](wxTimerEvent&)
    {
        m_pProgressGauge->Pulse();
    }, m_pProgressPulseTimer->GetId());
    rootSizer->Add(footerSizer, 0, wxEXPAND | wxALL, FromDIP(windowMargin));

    SetSizer(rootSizer);
    RefreshContent();
    SetClientSize(FromDIP(wxSize(twoColumnClientWidth, preferredClientHeight)));

    Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& event)
    {
        if (event.CanVeto())
        {
            Hide();
            event.Veto();
        }
        else
        {
            event.Skip();
        }
    });
}

PullRequestsWindow::Section PullRequestsWindow::CreateSection(wxWindow* parent)
{
    auto* sectionPanel = new wxPanel(parent);
    auto* sectionSizer = new wxBoxSizer(wxVERTICAL);

    auto* title = new wxStaticText(sectionPanel, wxID_ANY, wxEmptyString);
    auto titleFont = title->GetFont();
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);
    sectionSizer->Add(title, 0, wxEXPAND | wxBOTTOM, FromDIP(8));

    auto* scrolledWindow = new wxScrolledWindow(
        sectionPanel,
        wxID_ANY,
        wxDefaultPosition,
        wxDefaultSize,
        wxVSCROLL | wxBORDER_THEME);
    scrolledWindow->SetScrollRate(0, FromDIP(12));
    scrolledWindow->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    auto* itemsSizer = new wxBoxSizer(wxVERTICAL);
    scrolledWindow->SetSizer(itemsSizer);
    sectionSizer->Add(scrolledWindow, 1, wxEXPAND);
    sectionPanel->SetSizer(sectionSizer);

    return {.title = title, .scrolledWindow = scrolledWindow, .itemsSizer = itemsSizer};
}

void PullRequestsWindow::SetPullRequests(
    const PullRequestsInfo& pullRequestsInfo,
    const bool showAllPullRequests,
    const bool hideChangesRequestedPullRequests,
    const bool useTwoColumnLayout)
{
    m_pullRequestsInfo = pullRequestsInfo;
    m_hasPullRequests = true;
    m_showAllPullRequests = showAllPullRequests;
    m_hideChangesRequestedPullRequests = hideChangesRequestedPullRequests;
    m_useTwoColumnLayout = useTwoColumnLayout;
    RefreshContent();
}

void PullRequestsWindow::SetRepositories(const std::vector<Repository>& repositories)
{
    m_repositoryCount = repositories.size();
    m_pCreatePullRequestButton->Enable(m_repositoryCount != 0);
}

void PullRequestsWindow::SetUpdating(const bool updating)
{
    m_updating = updating;
    m_pUpdateButton->Enable(!updating);
    m_pUpdateButton->SetLabel(updating ? "Updating..." : "Update");
    m_pProgressGauge->Show(updating);
    if (updating)
    {
        SetUpdateProgress(false, 0, 0);
    }
    else
    {
        m_pProgressPulseTimer->Stop();
    }
    Layout();
    if (!m_hasPullRequests)
        RefreshContent();
}

void PullRequestsWindow::SetUpdateProgress(
    const bool isFetchingDetails,
    const size_t completed,
    const size_t total)
{
    if (!m_updating)
        return;

    if (!isFetchingDetails || total == 0)
    {
        m_pProgressGauge->Pulse();
        if (!m_pProgressPulseTimer->IsRunning())
            m_pProgressPulseTimer->Start(75);
        m_pProgressGauge->SetToolTip("Updating pull requests...");
        return;
    }

    m_pProgressPulseTimer->Stop();
    constexpr auto gaugeRange = 1000;
    const auto value = static_cast<int>(std::min(total, completed) * gaugeRange / total);
    m_pProgressGauge->SetRange(gaugeRange);
    m_pProgressGauge->SetValue(value);
    m_pProgressGauge->SetToolTip(
        std::format(wxS("Fetching pull request details: {}/{}"), completed, total));
}

void PullRequestsWindow::SetUpdateStatusText(const wxString& status)
{
    SetEllipsizedLabel(m_pStatusText, status);
}

void PullRequestsWindow::ToggleNear(const wxPoint& anchor)
{
    if (IsShown())
    {
        Hide();
        return;
    }

    PositionNear(anchor);
    Show();
    Raise();
}

void PullRequestsWindow::RefreshContent()
{
    ApplyLayout();

    std::vector<const PullRequestInfo*> pullRequestsToReview;
    size_t hiddenPullRequestsCount{};
    for (const auto& pullRequest: m_pullRequestsInfo.waitingForMyApprovalPullRequests)
    {
        const bool hasChangesRequestedBySomeoneElse =
            !pullRequest.GetParticipantsRequestedChangesWithout(m_pullRequestsInfo.currentUser).empty();
        const bool hidden = m_hideChangesRequestedPullRequests && hasChangesRequestedBySomeoneElse;
        if (hidden)
            ++hiddenPullRequestsCount;

        if (!hidden || m_showAllPullRequests)
            pullRequestsToReview.push_back(&pullRequest);
    }

    std::vector<const PullRequestInfo*> myPullRequests;
    myPullRequests.reserve(m_pullRequestsInfo.myPullRequests.size());
    for (const auto& pullRequest: m_pullRequestsInfo.myPullRequests)
        myPullRequests.push_back(&pullRequest);

    m_pShowAllCheckBox->SetLabel(
        std::format(wxS("Show {} hidden pull request{}"), hiddenPullRequestsCount, hiddenPullRequestsCount == 1 ? wxS("") : wxS("s")));
    m_pShowAllCheckBox->SetValue(m_showAllPullRequests);
    m_pShowAllCheckBox->Show(m_hasPullRequests && hiddenPullRequestsCount != 0);

    const auto reviewTitle = std::format(wxS("Pull requests to review ({})"), pullRequestsToReview.size());
    const auto myTitle = std::format(wxS("Your pull requests ({})"), myPullRequests.size());
    RefreshSection(m_reviewSection, reviewTitle, pullRequestsToReview, false);
    RefreshSection(m_mySection, myTitle, myPullRequests, true);

    Layout();
}

void PullRequestsWindow::RefreshSection(
    const Section& section,
    const wxString& title,
    const std::vector<const PullRequestInfo*>& pullRequests,
    const bool isMyPullRequest) const
{
    section.title->SetLabelText(title);
    section.itemsSizer->Clear(true);

    if (pullRequests.empty())
    {
        const wxString message = !m_hasPullRequests && m_updating
                                     ? wxS("Loading pull requests...")
                                     : !m_hasPullRequests
                                           ? wxS("Pull requests have not been loaded yet.")
                                           : wxS("No pull requests.");
        auto* emptyState = new wxStaticText(section.scrolledWindow, wxID_ANY, message);
        emptyState->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
        section.itemsSizer->Add(emptyState, 0, wxALL, FromDIP(cardMargin));
    }
    else
    {
        for (const auto* pullRequest: pullRequests)
        {
            section.itemsSizer->Add(
                CreatePullRequestCard(section.scrolledWindow, *pullRequest, isMyPullRequest),
                0,
                wxEXPAND | wxLEFT | wxRIGHT | wxTOP,
                FromDIP(cardMargin));
        }
        section.itemsSizer->AddSpacer(FromDIP(cardMargin));
    }

    section.scrolledWindow->FitInside();
    section.scrolledWindow->Scroll(0, 0);
    section.scrolledWindow->Layout();
}

wxWindow* PullRequestsWindow::CreatePullRequestCard(
    wxWindow* parent,
    const PullRequestInfo& pullRequest,
    const bool isMyPullRequest) const
{
    auto* card = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME);
    card->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));
    auto* cardSizer = new wxBoxSizer(wxVERTICAL);

    auto* title = new wxStaticText(
        card,
        wxID_ANY,
        pullRequest.GetMainMenuItemTitle(),
        wxDefaultPosition,
        wxDefaultSize,
        wxST_ELLIPSIZE_END);
    auto titleFont = title->GetFont();
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleFont.SetUnderlined(true);
    title->SetFont(titleFont);
    title->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT));
    title->SetCursor(wxCursor(wxCURSOR_HAND));
    title->SetToolTip(pullRequest.GetMainMenuItemTitle());

    const auto href = wxString::FromUTF8(pullRequest.pullRequest.links.html.href);
    title->Bind(wxEVT_LEFT_UP, [href](wxMouseEvent&)
    {
        OpenPullRequest(href);
    });
    cardSizer->Add(title, 0, wxEXPAND | wxALL, FromDIP(cardMargin));

    const auto firstDetails = isMyPullRequest
                                  ? pullRequest.GetMyPullRequestBranchMenuItemTitle()
                                  : pullRequest.GetAuthorAndBranchMenuItemTitle();
    auto* firstDetailsLabel = new wxStaticText(card, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
    SetEllipsizedLabel(firstDetailsLabel, firstDetails);
    cardSizer->Add(firstDetailsLabel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(cardMargin));

    auto* detailsLabel = new wxStaticText(card, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
    SetEllipsizedLabel(detailsLabel, pullRequest.GetPullRequestDetailsMenuItemTitle());
    cardSizer->Add(detailsLabel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(cardMargin));

    const auto participants = isMyPullRequest
                                  ? pullRequest.pullRequest.participants
                                        | std::views::filter([](const auto& participant)
                                          {
                                              return participant.role == ParticipantRole::Reviewer || participant.approved;
                                          })
                                        | std::ranges::to<std::vector>()
                                  : pullRequest.GetParticipantsRequestedChanges();
    const auto participantTitles = JoinParticipantTitles(participants, pullRequest);
    if (!participantTitles.IsEmpty())
    {
        auto* participantsLabel = new wxStaticText(card, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
        SetEllipsizedLabel(participantsLabel, participantTitles);
        cardSizer->Add(participantsLabel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(cardMargin));
    }

    card->SetSizer(cardSizer);
    return card;
}

void PullRequestsWindow::ApplyLayout()
{
    const auto orientation = m_useTwoColumnLayout ? wxHORIZONTAL : wxVERTICAL;
    if (m_pSectionsSizer->GetOrientation() != orientation)
        m_pSectionsSizer->SetOrientation(orientation);

    const auto minimumClientWidth = m_useTwoColumnLayout ? twoColumnClientWidth : singleColumnClientWidth;
    const auto minimumClientSize = FromDIP(wxSize(minimumClientWidth, minimumClientHeight));
    SetMinClientSize(minimumClientSize);

    const auto currentClientSize = GetClientSize();
    if (currentClientSize.GetWidth() < minimumClientSize.GetWidth() ||
        currentClientSize.GetHeight() < minimumClientSize.GetHeight())
    {
        SetClientSize(wxSize(
            std::max(currentClientSize.GetWidth(), minimumClientSize.GetWidth()),
            std::max(currentClientSize.GetHeight(), minimumClientSize.GetHeight())));
    }
}

void PullRequestsWindow::PositionNear(const wxPoint& anchor)
{
    const auto displayIndex = wxDisplay::GetFromPoint(anchor);
    const wxDisplay display(displayIndex == wxNOT_FOUND ? 0 : static_cast<unsigned>(displayIndex));
    const auto workArea = display.GetClientArea();

    const auto minimumSize = GetMinSize();
    const auto maximumSize = wxSize(
        std::max(1, workArea.GetWidth() - FromDIP(2 * windowMargin)),
        std::max(1, workArea.GetHeight() - FromDIP(2 * windowMargin)));
    const auto currentSize = GetSize();
    const auto size = wxSize(
        std::max(minimumSize.GetWidth(), std::min(currentSize.GetWidth(), maximumSize.GetWidth())),
        std::max(minimumSize.GetHeight(), std::min(currentSize.GetHeight(), maximumSize.GetHeight())));
    SetSize(size);

    const auto offset = FromDIP(8);
    auto x = anchor.x - size.GetWidth() / 2;
    auto y = anchor.y - size.GetHeight() - offset;
    if (y < workArea.GetTop())
        y = anchor.y + offset;

    const auto maximumX = workArea.GetRight() - size.GetWidth() + 1;
    const auto maximumY = workArea.GetBottom() - size.GetHeight() + 1;
    x = maximumX < workArea.GetLeft() ? workArea.GetLeft() : std::clamp(x, workArea.GetLeft(), maximumX);
    y = maximumY < workArea.GetTop() ? workArea.GetTop() : std::clamp(y, workArea.GetTop(), maximumY);
    Move(x, y);
}
