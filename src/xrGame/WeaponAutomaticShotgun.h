#pragma once
#include "WeaponMagazined.h"
#include "WeaponShotgun.h"

class CWeaponAutomaticShotgun : public CWeaponMagazined
{
    typedef CWeaponMagazined inherited;

public:
    CWeaponAutomaticShotgun();
    virtual ~CWeaponAutomaticShotgun();

    virtual void Load(LPCSTR section);
    virtual void shedule_Update(u32 dt);

    virtual void net_Export(NET_Packet& P);
    virtual void net_Import(NET_Packet& P);

    virtual void Reload();

    void switch2_Misfire();
    void switch2_StartReload();
    void switch2_AddCartgidge();
    void switch2_EndReload();

    virtual void PlayAnimOpenWeapon();
    virtual void PlayAnimAddOneCartridgeWeapon();
    void PlayAnimCloseWeapon();
    virtual void PlayAnimShow();
    virtual void PlayAnimIdle();
    virtual void PlayAnimIdleMoving();
    virtual void PlayAnimIdleMovingCrouch();
    virtual void PlayAnimIdleSprint();
    virtual void PlayAnimHide();
    virtual void PlayAnimShoot();
    virtual void PlayAnimBore();
    virtual void PlayAnimAim();

    virtual bool Action(u16 cmd, u32 flags);
    //virtual int GetCurrentFireMode() { return m_aFireModes[m_iCurFireMode]; } //AVO: this is already implemented in parent class (CWeaponMagazined)
protected:
    virtual void OnAnimationEnd(u32 state);
    void TriStateReload();
    virtual void OnStateSwitch(u32 S, u32 oldState);

    bool HaveCartridgeInInventory(u8 cnt);
    virtual u8 AddCartridge(u8 cnt);
    virtual void AddCartridgeUpdate();

    ESoundTypes m_eSoundOpen;
    ESoundTypes m_eSoundOpenEmpty;
    ESoundTypes m_eSoundAddCartridge;
    ESoundTypes m_eSoundAddCartridgeEmpty;
    ESoundTypes m_eSoundClose;
};
