#pragma once

#include "../pch.h"

struct SkinInfo {
    int id;
    std::string name;
};

inline std::vector<SkinInfo> g_policeSkins = { { 71, "Security Guard" },
    { 73, "Hippy" },
    { 280, "LS Officer" },
    { 281, "SF Officer" },
    { 282, "LV Officer" },
    { 283, "County Sheriff" },
    { 284, "Motorbike Cop" },
    { 285, "SWAT" },
    { 286, "FBI" },
    { 287, "Army" } };

inline bool IsPoliceSkin(int modelId) {
    for (const auto& s : g_policeSkins)
        if (s.id == modelId) return true;

    return false;
}