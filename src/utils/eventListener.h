#pragma once

#include "include/menu.h"

template <typename... Args> class EventListener : public IEventListener<Args...>
{
public:
    using Callback = typename IEventListener<Args...>::Callback;
    using ConditionCallback = typename IEventListener<Args...>::ConditionCallback;

    struct Listener
    {
        void* ref;
        Callback callback;
    };

    void Add(const Callback& cb) override
    {
        callbacks.push_back({ nullptr, cb });
    }

    void AddOnce(const Callback& cb) override
    {
        onceCallbacks.push_back({ nullptr, cb });
    }

    void AddUntil(const ConditionCallback& cb) override
    {
        conditions.push_back(cb);
    }

    void AddRef(void* ptr, const Callback& cb) override
    {
        callbacks.push_back({ ptr, cb });
    }

    void Remove(void* ref) override
    {
        callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(), [ref](const Listener& listener) { return listener.ref == ref; }),
            callbacks.end());
    }

    void Emit(Args... args) override
    {
        auto callbacksCopy = callbacks;

        for (auto& listener : callbacksCopy) listener.callback(args...);

        auto onceCallbacksCopy = std::move(onceCallbacks);
        onceCallbacks.clear();

        for (auto& listener : onceCallbacksCopy) listener.callback(args...);

        auto conditionsCopy = std::move(conditions);
        conditions.clear();

        for (auto& condition : conditionsCopy)
        {
            if (condition(args...)) conditions.push_back(std::move(condition));
        }
    }

    int GetListenersCount() override
    {
        return callbacks.size() + onceCallbacks.size() + conditions.size();
    }

    void Clear() override
    {
        callbacks.clear();
        onceCallbacks.clear();
        conditions.clear();
    }

private:
    std::vector<Listener> callbacks;
    std::vector<Listener> onceCallbacks;
    std::vector<ConditionCallback> conditions;
};