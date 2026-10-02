#ifndef __AML_PSDK_SAAEFRONTENDAUDIOENTITY_H
#define __AML_PSDK_SAAEFRONTENDAUDIOENTITY_H

#include "AEAudioEntity.h"

DECL_CLASS(CAEFrontendAudioEntity)
    CAEAudioEntity audio;
    unsigned char m_bWakeupAmplifierEventTriggered;
    unsigned char m_nLoadingTrackIndex;
    unsigned short m_nLastRetuneSoundID;
    unsigned short m_nNumBulletSounds;
    unsigned short bankId;
    unsigned int m_LastPickupEventFrame;
    unsigned int m_LastPartMissionCompleteEventFrame;
    unsigned int m_LastBulletPassEventFrame;
    unsigned int m_nResprayStartTimeMs;
    unsigned int m_nLastFrontendTimerCountTimeMs;
    CAESound *m_pWakeupAmplifierSound;
    CAETwinLoopSoundEntity m_RadioTuneSound;
    CAETwinLoopSoundEntity m_RadioTuneSoundInPause;
DECL_CLASS_END()
CHECKSIZE(CAEFrontendAudioEntity, 0x1EC, 0x248);

#endif // __AML_PSDK_SAAEFRONTENDAUDIOENTITY_H