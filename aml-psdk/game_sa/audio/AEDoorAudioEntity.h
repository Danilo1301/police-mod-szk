#ifndef __AML_PSDK_SAAEDOORAUDIOENTITY_H
#define __AML_PSDK_SAAEDOORAUDIOENTITY_H

#include "AEAudioEntity.h"

DECL_CLASS(CAEDoorAudioEntity)
    CAEAudioEntity audio;
    unsigned int m_nTime;
    unsigned int m_nLastEntryExitDoorEventTimeMs;
    float m_fDoorTurnSpeedLastFrame;
DECL_CLASS_END()

#endif // __AML_PSDK_SAAEDOORAUDIOENTITY_H