#pragma once

#include "../../pch.h"

#include "CalloutBase.h"

class CalloutRegistry {
public:
    static inline std::vector<CalloutBase*> m_callouts;

    static void Register(CalloutBase* callout) { m_callouts.emplace_back(callout); }

    static CalloutBase* GetRandom() {
        if (m_callouts.empty()) return nullptr;
        int idx = rand() % m_callouts.size();
        return m_callouts[idx];
    }

    static const std::vector<CalloutBase*>& GetAll() { return m_callouts; }
};