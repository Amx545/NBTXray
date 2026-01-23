#pragma once
#include "WeaponAutomaticShotgun.h"

class CWeaponProtecta : public CWeaponAutomaticShotgun
{
    typedef CWeaponAutomaticShotgun inherited;

    
public:
    CWeaponProtecta();
    virtual ~CWeaponProtecta();

    virtual void Load(LPCSTR section);
    virtual void shedule_Update(u32 dt);

    virtual void PlayAnimAddOneCartridgeWeapon();
    virtual void PlayAnimHide();
    virtual void PlayAnimShow();

protected:
    virtual void AddCartridgeUpdate();

protected:
    pcstr sProtectaBulletName{"bullet"};
    u16 iProtectaBulletDelay{50};
    u8 iProtectaBulletCount{12};
    u8 iAmmoCurrent{0};
    bool bProtectaBulletScheme{true};
    bool bAmmoSkip{false};

private:
    void ProtectaBoneUpdate();
    void ProtectaBoneVisibility(u8 index, bool add);
};
