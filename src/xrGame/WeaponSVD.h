#pragma once

#include "WeaponPistol.h"

class CWeaponSVD : public CWeaponPistol
{
    typedef CWeaponPistol inherited;

protected:
    virtual void switch2_Fire();
    virtual void OnAnimationEnd(u32 state);

public:
    CWeaponSVD(void);
    virtual ~CWeaponSVD(void);
};
