#pragma once

#include "../pch.h"

struct VehicleDocInfo
{
    std::string plate;
    std::string chassis;
    std::string renavam;

    bool isStolen;
    bool isDocumentExpired;

    VehicleDocInfo();
};