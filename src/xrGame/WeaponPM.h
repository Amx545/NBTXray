#ifndef __XR_WEAPON_PM_H__
#define __XR_WEAPON_PM_H__

#pragma once

#include "WeaponPistol.h"

class CWeaponPM : public CWeaponPistol
{
private:
    using inherited = CWeaponPistol;

protected:
public:
    CWeaponPM();
    virtual ~CWeaponPM();
    int GetCurrentFireMode() override { return 1; }

protected:
    void FireEnd() override;
    void switch2_Fire() override;
};

#endif //__XR_WEAPON_PM_H__
