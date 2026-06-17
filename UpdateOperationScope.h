#pragma once

#include <wx/app.h>
#include <wx/weakref.h>

#include "StatusItem.h"

class UpdateOperationScope
{
    StatusItem *m_pStatusItem;

public:
    explicit UpdateOperationScope(StatusItem *pStatusItem) :
        m_pStatusItem(pStatusItem)
    {
        CallAfter([this] { m_pStatusItem->BeginUpdate(); });
    }

    ~UpdateOperationScope()
    {
        CallAfter([this] { m_pStatusItem->EndUpdate(); });
    }

    template<typename TAction>
    static void CallAfter(TAction action)
    {
        wxWeakRef<StatusItem> isWindowValid;
        wxTheApp->CallAfter([isWindowValid, action]
        {
            if (!isWindowValid)
                return;

            action();
        });
    }
};
