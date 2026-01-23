#pragma once
#include "WeaponCustomPistol.h"
class CUIWeaponGauss;
enum EGaussState
{
    eGaussWait = 0,
    eGaussReload,
    eGaussRevivel,
    eGaussFireStart,
    eGaussCompOpen,
    eGaussCompClose
};
class CWeaponGauss : public CWeaponCustomPistol
{
    typedef CWeaponCustomPistol inherited;


public:
    CWeaponGauss();
    virtual ~CWeaponGauss();
     virtual void Load(LPCSTR section);

    //virtual void net_Export(NET_Packet& P);
    //virtual void net_Import(NET_Packet& P);

    virtual void OnH_B_Independent(bool just_before_destroy);
    virtual void save(NET_Packet& output_packet);
    virtual void load(IReader& input_packet);

    virtual void shedule_Update(u32 dt);

    virtual bool Action(u16 cmd, u32 flags);

    virtual void UpdateSounds();
    
    virtual bool SwitchMode();
    bool CanSwitchMode();
    virtual void OnAnimationEnd(u32 state);
    virtual void OnStateSwitch(u32 S, u32 oldState);

    void PlayAnimModeSwitch();
    virtual void PlayAnimShow();
    virtual void PlayAnimHide();
    virtual void PlayAnimReload();
    virtual void PlayAnimIdle();
    virtual void PlayAnimShoot();
    virtual void PlayAnimBore();
    virtual void ReloadMagazine();
    void ReloadBattery();

    CUIWeaponGauss& ui();
    virtual void render_item_3d_ui();
    virtual bool render_item_3d_ui_query();

protected:
    bool m_bUseSwitchMode{true};
    bool m_bCompMode{false};
    bool m_bCompOpen{false};
    u32 m_shoot_cur_time{0};
    CUIWeaponGauss* m_ui;
    EGaussState eGaussState{eGaussWait};
};
