#ifndef __AML_PSDK_SAAEPEDLESSSPEECHAUDIOENTITY_H
#define __AML_PSDK_SAAEPEDLESSSPEECHAUDIOENTITY_H

#include "AEAudioEntity.h"

struct CEntity;

DECL_CLASS(CAEPedlessSpeechAudioEntity)
    CAEPedSpeechAudioEntity pedSpeech;
    CAESound *SoundPtrs[5];
    CEntity *m_pEntityAttachedTo;
DECL_CLASS_END()

#endif // __AML_PSDK_SAAEPEDLESSSPEECHAUDIOENTITY_H