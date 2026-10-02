#ifndef __AML_PSDK_SATASKTIMER_H
#define __AML_PSDK_SATASKTIMER_H

struct CTaskTimer
{
    int m_nStartTime;
    int m_nInterval;
    bool m_bStarted;
    bool m_bStopped;

    inline CTaskTimer()
    {
        m_nStartTime = m_nInterval = 0;
        m_bStarted = m_bStopped = false;
    }   
    inline CTaskTimer(int start, int interval)
    {
        m_nStartTime = start;
        m_nInterval = interval;
        m_bStarted = true;
    }
};

#endif // __AML_PSDK_SATASKTIMER_H