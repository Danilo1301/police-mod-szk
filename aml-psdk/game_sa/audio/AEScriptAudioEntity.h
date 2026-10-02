#ifndef __AML_PSDK_SAAESCRIPTAUDIOENTITY_H
#define __AML_PSDK_SAAESCRIPTAUDIOENTITY_H

#include "AEDoorAudioEntity.h"

struct CEntity;
struct CPhysical;

struct CAudioLink
{
    CVector m_vPosition;
    CEntity* m_pEntity;
    CAESound *pSound;
    int m_nAudioEvent;
    int m_nBankId;
    int m_nBankSlotId;
};

DECL_CLASS_BASED(CAEExplosionAudioEntity, CAEAudioEntity)
    char m_nLastExplosionFrequencyIndex;
DECL_CLASS_END()

DECL_CLASS(CAEScriptAudioEntity)
    CAEAudioEntity audio;
    bool m_bMiniGameOneShotTriggered;
    bool m_bInACrane;
    char m_nMeshGateTriggerCount;
    int m_dwLastTimeHornPlayed;
    float m_fCraneVolume;
    float m_fCraneFrequencyScaling;
    float m_fMissionSoundEnvelopePhase;
    CAudioLink wavLinks[4];
    CPhysical *m_pCrane;
    CAEDoorAudioEntity m_doorAudio;
    CAEExplosionAudioEntity m_explosionAudio;
DECL_CLASS_END()
CHECKSIZE(CAEScriptAudioEntity, 0x21C, 0x2A8);

#endif // __AML_PSDK_SAAESCRIPTAUDIOENTITY_H