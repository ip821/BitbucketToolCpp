//
// Created by Igor Palkin on 19.12.2025.
//

#ifndef WXWIDGETSPLAYGROUND_MYAPP_H
#define WXWIDGETSPLAYGROUND_MYAPP_H

class StatusItem;

class App : public wxApp
{
public:
    explicit App();
    bool OnInit() override;

private:
    std::shared_ptr<StatusItem> m_statusItem;
};

#endif //WXWIDGETSPLAYGROUND_MYAPP_H
