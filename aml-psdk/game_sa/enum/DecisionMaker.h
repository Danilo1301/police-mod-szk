#pragma once

enum eDecisionMakerType : int
{
    UNKNOWN = -1,

    PED_GROUPMEMBER,
    PED_COP,
    PED_RANDOM1,
    PED_RANDOM2,
    PED_RANDOM3,
    PED_FIREMAN,
    PED_EMPTY,
    PED_INDOORS,

    GROUP_RANDOM_AGGRESSIVE,
    GROUP_RANDOM_PASSIVE,

    MISSION0,
    MISSION1,
    MISSION2,
    MISSION3,
    MISSION4,
    MISSION5,
    MISSION6,
    MISSION7,
    MISSION8,
    MISSION9,

    COUNT_TOTAL,
    COUNT_GAME_DM = MISSION0, // Number of built-in decision makers
};