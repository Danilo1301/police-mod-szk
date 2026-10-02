#include "CleoFunctions.h"

std::vector<WaitFunction*> waitFunctions;

void CleoFunctions::Update(int dt)
{
    for(auto waitFunction : waitFunctions)
    {
        //fileLog->Debug("cleofunction : " + waitFunction->name);

        if(waitFunction->isConditionFunction)
        {
            waitFunction->conditionFn([waitFunction] () {
                waitFunction->state = WAIT_FN_STATE::WAIT_FN_COMPLETED;
            }, [waitFunction] () {
                waitFunction->state = WAIT_FN_STATE::WAIT_FN_CANCELLED;
            });
            continue;
        }

        if(waitFunction->isTestFunction)
        {
            auto result = waitFunction->testFn();
            if(result) waitFunction->state = WAIT_FN_STATE::WAIT_FN_COMPLETED;
            continue;
        }

        waitFunction->timePassed += dt;
        if(waitFunction->timePassed >= waitFunction->time)
        {
            waitFunction->state = WAIT_FN_STATE::WAIT_FN_COMPLETED;
        }
    }

    std::vector<WaitFunction*> toRemove;

    for(auto waitFunction : waitFunctions)
    {
        if(waitFunction->state == WAIT_FN_STATE::WAIT_FN_COMPLETED)
        {
            waitFunction->onComplete();
            toRemove.push_back(waitFunction);
        }

        if(waitFunction->state == WAIT_FN_STATE::WAIT_FN_CANCELLED)
        {
            waitFunction->onCancel();
            toRemove.push_back(waitFunction);
        }
    }

    for(auto waitFunction : toRemove)
    {
        RemoveWaitFunction(waitFunction);
    }
}

WaitFunction* CleoFunctions::AddWaitFunction(int time, std::function<void()> callback)
{
    WaitFunction* waitFunction = new WaitFunction();
    waitFunction->time = time;
    waitFunction->onComplete = callback;
    waitFunctions.push_back(waitFunction);
    return waitFunction;
}

void CleoFunctions::RemoveWaitFunction(WaitFunction* waitFunction)
{
    auto it = std::find(waitFunctions.begin(), waitFunctions.end(), waitFunction);
    if (it == waitFunctions.end()) return;
    waitFunctions.erase(it);
    delete waitFunction;
}

WaitFunction* CleoFunctions::AddWaitForFunction(std::string name, std::function<bool()> testFn, std::function<void()> callback)
{
    WaitFunction* waitFunction = new WaitFunction();
    waitFunction->time = 0;
    waitFunction->name = name;
    waitFunction->onComplete = callback;
    waitFunction->isTestFunction = true;
    waitFunction->testFn = testFn;
    waitFunctions.push_back(waitFunction);

    return waitFunction;
}

WaitFunction* CleoFunctions::AddCondition(std::function<void(std::function<void()>, std::function<void()>)> fn, std::function<void()> onComplete, std::function<void()> onCancel)
{
    WaitFunction* waitFunction = new WaitFunction();
    waitFunction->time = 0;
    waitFunction->onComplete = onComplete;
    waitFunction->onCancel = onCancel;
    waitFunction->isConditionFunction = true;
    waitFunction->conditionFn = fn;
    waitFunctions.push_back(waitFunction);

    return waitFunction;
}