#ifndef __AML_PSDK_SAOPCODECALLER_H
#define __AML_PSDK_SAOPCODECALLER_H

#include <cstdint>
#include <cstring>

// #include "aml-psdk/game_sa/utils/OpcodeCallerIDs.h"
#include "mod/logger.h"
#include <aml-psdk/game_sa/engine/RunningScript.h>

inline bool g_debugOpcodes = false;

struct ZeroArg
{
};

struct OpcodeCaller
{
    OpcodeCaller()
    {
        memset(&g_Script, 0, sizeof(g_Script));

        strcpy(g_Script.m_szName, "amlpsdk");

        g_Script.m_nScriptBrainType = -1;
        g_Script.m_bWastedBustedCheck = true;
    }

    static inline void Reset()
    {
        g_nBufferPos = 0;
        g_nVarsCount = 0;

        for (int i = 0; i < 18; ++i) { g_Script.m_aLocalVars[i].uParam = 0; }
    }

    static inline int ExecuteScriptBuf()
    {
        g_Script.m_pCurrentIP = g_ScriptBuffer;

        int result = g_Script.ProcessOneCommand();

        return result;
    }

    static inline void PushVarToBuffer(ZeroArg)
    {
        g_ScriptBuffer[g_nBufferPos++] = 0x00;
    }

    static inline void PushVarToBuffer(int v)
    {
        g_ScriptBuffer[g_nBufferPos] = 0x01;

        memcpy(&g_ScriptBuffer[++g_nBufferPos], &v, sizeof(v));

        g_nBufferPos += sizeof(v);
    }

    static inline void PushVarToBuffer(unsigned int v)
    {
        g_ScriptBuffer[g_nBufferPos] = 0x01;

        memcpy(&g_ScriptBuffer[++g_nBufferPos], &v, sizeof(v));

        g_nBufferPos += sizeof(v);
    }

    static inline void PushVarToBuffer(bool v)
    {
        int value = v ? 1 : 0;

        PushVarToBuffer(value);
    }

    static inline void PushVarToBuffer(float v)
    {
        g_ScriptBuffer[g_nBufferPos] = 0x06;

        memcpy(&g_ScriptBuffer[++g_nBufferPos], &v, sizeof(v));

        g_nBufferPos += sizeof(v);
    }

    static inline void PushVarToBuffer(double v)
    {
        PushVarToBuffer((float)v);
    }

    static inline void PushVarToBuffer(const char* s)
    {
        unsigned char length = (unsigned char)strlen(s);

        g_ScriptBuffer[g_nBufferPos] = 0x0E;

        g_ScriptBuffer[++g_nBufferPos] = length;

        memcpy(&g_ScriptBuffer[++g_nBufferPos], s, length);

        g_nBufferPos += length;
    }

    static inline void PushVarToBuffer(int* v)
    {
        g_ScriptBuffer[g_nBufferPos] = 0x03;

        g_ScriptVarPointers[g_nVarsCount] = (uint32_t*)v;

        g_Script.m_aLocalVars[g_nVarsCount].uParam = *(uint32_t*)v;

        uint16_t varIndex = g_nVarsCount;

        memcpy(&g_ScriptBuffer[++g_nBufferPos], &varIndex, sizeof(varIndex));

        g_nBufferPos += sizeof(varIndex);

        ++g_nVarsCount;
    }

    static inline void PushVarToBuffer(unsigned int* v)
    {
        g_ScriptBuffer[g_nBufferPos] = 0x03;

        g_ScriptVarPointers[g_nVarsCount] = (uint32_t*)v;

        g_Script.m_aLocalVars[g_nVarsCount].uParam = *(uint32_t*)v;

        uint16_t varIndex = g_nVarsCount;

        memcpy(&g_ScriptBuffer[++g_nBufferPos], &varIndex, sizeof(varIndex));

        g_nBufferPos += sizeof(varIndex);

        ++g_nVarsCount;
    }

    static inline void PushVarToBuffer(float* v)
    {
        g_ScriptBuffer[g_nBufferPos] = 0x03;

        g_ScriptVarPointers[g_nVarsCount] = (uint32_t*)v;

        memcpy(&g_Script.m_aLocalVars[g_nVarsCount].uParam, v, sizeof(float));

        uint16_t varIndex = g_nVarsCount;

        memcpy(&g_ScriptBuffer[++g_nBufferPos], &varIndex, sizeof(varIndex));

        g_nBufferPos += sizeof(varIndex);

        ++g_nVarsCount;
    }

    static inline CRunningScript g_Script{ 0 };

    static inline uint8_t g_ScriptBuffer[512]{ 0 };

    // Script values are 32-bit even on ARM64.
    static inline uint32_t* g_ScriptVarPointers[18]{ nullptr };

    static inline uint16_t g_nBufferPos = 0;
    static inline uint16_t g_nVarsCount = 0;
};

inline OpcodeCaller g_OpcodeCallerStub;

template <typename T> inline void PushVars(T arg)
{
    OpcodeCaller::PushVarToBuffer(arg);
}

template <typename A1, typename... Args> inline void PushVars(A1 arg, Args... args)
{
    OpcodeCaller::PushVarToBuffer(arg);
    PushVars(args...);
}

template <typename... ArgTypes> inline bool CommandByID(uint16_t opcode, ArgTypes... args)
{
    OpcodeCaller::Reset();

    memcpy(OpcodeCaller::g_ScriptBuffer, &opcode, sizeof(opcode));

    OpcodeCaller::g_nBufferPos = 2;

    PushVars(args...);

    int processResult = OpcodeCaller::ExecuteScriptBuf();

    if (g_debugOpcodes)
    {
        logger->Info("opcode=0x%04X process=%d cond=%d varCount=%d local0=%u",
            opcode,
            processResult,
            (int)OpcodeCaller::g_Script.m_bCondResult,
            (int)OpcodeCaller::g_nVarsCount,
            OpcodeCaller::g_Script.m_aLocalVars[0].uParam);
    }

    for (int i = 0; i < OpcodeCaller::g_nVarsCount; ++i)
    {
        *OpcodeCaller::g_ScriptVarPointers[i] = OpcodeCaller::g_Script.m_aLocalVars[i].uParam;
    }

    return OpcodeCaller::g_Script.m_bCondResult;
}

template <uint16_t CommandID, typename... ArgTypes> inline bool Command(ArgTypes... arguments)
{
    return CommandByID(CommandID, arguments...);
}

template <uint16_t CommandID> inline bool Command()
{
    OpcodeCaller::Reset();

    const uint16_t opcode = CommandID;

    memcpy(OpcodeCaller::g_ScriptBuffer, &opcode, sizeof(opcode));

    OpcodeCaller::g_nBufferPos = 2;

    OpcodeCaller::ExecuteScriptBuf();

    return OpcodeCaller::g_Script.m_bCondResult;
}

#endif // __AML_PSDK_SAOPCODECALLER_H