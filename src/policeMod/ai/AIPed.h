#pragma once

struct AIPed {
public:
    int pedRef = -1;

    virtual ~AIPed();

    virtual void Start();
    virtual void Update();
};