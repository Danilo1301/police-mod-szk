#pragma once

#include "simpleGta.h"
#include <string>
#include <vector>

struct IDebugLine
{
    std::string text;
    CRGBA color;
};

class IDebug {
public:
    std::vector<IDebugLine> lines;
    bool enabled = true;
    
    virtual void AddLine(std::string text) = 0;
    virtual void AddLine(std::string text, CRGBA color) = 0;
    virtual void Clear() = 0;
};