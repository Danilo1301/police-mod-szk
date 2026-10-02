#ifndef __AML_PSDK_SARSGLOBAL_H
#define __AML_PSDK_SARSGLOBAL_H

#include <aml-psdk/game_sa/plugin.h>

struct RsGlobalType
{
    const char *appName;
    i32 maximumWidth;
    i32 maximumHeight;
    i32 maxFPS;
    i32 quit;

    void *ps; /* platform specific data */

    //RsInputDevice keyboard;
    //RsInputDevice mouse;
    //RsInputDevice pad;
};

DECL_OBJECT_PLT_GLOBAL(RsGlobalType, RsGlobal, BYBIT(0x67910C, 0x850240) );
#define RsGlobal RsGlobal()

#endif // __AML_PSDK_SARSGLOBAL_H
