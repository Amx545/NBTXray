#include "StdAfx.h"
#include "WeaponPM.h"

CWeaponPM::CWeaponPM() : CWeaponPistol() {}
CWeaponPM::~CWeaponPM() {}

void CWeaponPM::FireEnd() 
{
    if (fShotTimeCounter <= 0)
    {
        SetPending(false);
        inherited::FireEnd();
    }
}

void CWeaponPM::switch2_Fire() 
{
    m_bFireSingleShot = true;
    bWorking = false;
    m_iShotNum = 0;
    m_bStopedAfterQueueFired = false;
}
