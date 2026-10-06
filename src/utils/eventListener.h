#pragma once

#include <algorithm>
#include <functional>
#include <vector>

template <typename... Args> class E_EventListener
{
public:
    using E_Callback = std::function<void(Args...)>;
    using E_ConditionCallback = std::function<bool(Args...)>;

    struct Listener
    {
        void* ref;
        E_Callback callback;
    };

    void Add(const E_Callback& cb)
    {
        if (!cb) { return; }

        callbacks.push_back({ nullptr, cb });
    }

    void AddOnce(const E_Callback& cb)
    {
        if (!cb) { return; }

        onceCallbacks.push_back({ nullptr, cb });
    }

    void AddUntil(const E_ConditionCallback& cb)
    {
        if (!cb) { return; }

        conditions.push_back(cb);
    }

    void AddRef(void* ptr, const E_Callback& cb)
    {
        if (!cb) { return; }

        callbacks.push_back({ ptr, cb });
    }

    void Remove(void* ref)
    {
        callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(), [ref](const Listener& listener) { return listener.ref == ref; }),
            callbacks.end());
    }

    void Emit(Args... args)
    {
        auto callbacksCopy = callbacks;

        for (const auto& listener : callbacksCopy)
        {
            if (listener.callback) { listener.callback(args...); }
        }

        auto onceCallbacksCopy = std::move(onceCallbacks);
        onceCallbacks.clear();

        for (const auto& listener : onceCallbacksCopy)
        {
            if (listener.callback) { listener.callback(args...); }
        }

        auto conditionsCopy = std::move(conditions);
        conditions.clear();

        for (auto& condition : conditionsCopy)
        {
            if (!condition) { continue; }

            if (!condition(args...)) { conditions.push_back(std::move(condition)); }
        }
    }

    size_t GetListenersCount() const
    {
        return callbacks.size() + onceCallbacks.size() + conditions.size();
    }

    void Clear()
    {
        callbacks.clear();
        onceCallbacks.clear();
        conditions.clear();
    }

private:
    std::vector<Listener> callbacks;
    std::vector<Listener> onceCallbacks;
    std::vector<E_ConditionCallback> conditions;
};