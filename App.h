#pragma once

class StatusItem;

class App : public wxApp
{
public:
    explicit App();
    bool OnInit() override;

private:
    StatusItem* m_pStatusItem{};
};
