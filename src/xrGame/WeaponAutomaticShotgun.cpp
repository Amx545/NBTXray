#include "StdAfx.h"
#include "WeaponAutomaticShotgun.h"
#include "Entity.h"
#include "ParticlesObject.h"
#include "xrEngine/xr_level_controller.h"
#include "Inventory.h"
#include "player_hud.h"
#include "Level.h"
#include "Actor.h"
#include "ActorCondition.h"

CWeaponAutomaticShotgun::CWeaponAutomaticShotgun()
{
    m_eSoundOpen = ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING);
    m_eSoundOpenEmpty = ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING);
    m_eSoundClose = ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING);
    m_eSoundAddCartridge = ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING);
    m_eSoundAddCartridgeEmpty = ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING);
}

CWeaponAutomaticShotgun::~CWeaponAutomaticShotgun() {}
void CWeaponAutomaticShotgun::Load(LPCSTR section)
{
    inherited::Load(section);

    if (pSettings->line_exist(section, "tri_state_reload"))
    {
        m_bTriStateReload = !!pSettings->r_bool(section, "tri_state_reload");
    };
    if (m_bTriStateReload)
    {
        m_sounds.LoadSound(section, "snd_open_weapon", "sndOpen", false, m_eSoundOpen);
        m_sounds.LoadSound(section, "snd_open_weapon_empty", "sndOpenEmpty", false, m_eSoundOpenEmpty);

        m_sounds.LoadSound(section, "snd_add_cartridge", "sndAddCartridge", false, m_eSoundAddCartridge);
        m_sounds.LoadSound(section, "snd_add_cartridge_empty", "sndAddCartridgeEmpty", false, 
            m_eSoundAddCartridgeEmpty);

        m_sounds.LoadSound(section, "snd_close_weapon", "sndClose", false, m_eSoundClose);
    };
}

void CWeaponAutomaticShotgun::shedule_Update(u32 dt) 
{
    inherited::shedule_Update(dt); 
}

bool CWeaponAutomaticShotgun::Action(u16 cmd, u32 flags)
{
    if (inherited::Action(cmd, flags))
        return true;

    if (m_bTriStateReload && GetState() == eReload && cmd == kWPN_FIRE && flags & CMD_START &&
        m_sub_state == eSubstateReloadInProcess) //остановить перезагрузку
    {
        AddCartridge(1);
        m_sub_state = eSubstateReloadEnd;
        return true;
    }
    return false;
}

void CWeaponAutomaticShotgun::OnAnimationEnd(u32 state)
{
    if (!m_bTriStateReload || state != eReload)
        return inherited::OnAnimationEnd(state);
    if (IsMisfire() && iAmmoElapsed)
    {
        bMisfire = false;
        m_sub_state = eSubstateReloadBegin;
        SwitchState(eIdle);
        return;
    }
    switch (m_sub_state)
    {
    case eSubstateReloadBegin:
    {
        m_sub_state = eSubstateReloadInProcess;
        SwitchState(eReload);
    }
    break;

    case eSubstateReloadInProcess:
    {
        if (0 != AddCartridge(1))
        {
            m_sub_state = eSubstateReloadEnd;
        }
        SwitchState(eReload);
    }
    break;

    case eSubstateReloadEnd:
    {
        m_sub_state = eSubstateReloadBegin;
        SwitchState(eIdle);
    }
    break;
    };
}

void CWeaponAutomaticShotgun::Reload()
{
    if (m_bTriStateReload)
    {
        TriStateReload();
    }
    else
        inherited::Reload();
}

void CWeaponAutomaticShotgun::TriStateReload()
{
    if (IsMisfire() && iAmmoElapsed)
    {
        CWeapon::Reload();
        SwitchState(eReload);
        return;
    }
    if (m_magazine.size() == (u32)iMagazineSize || !HaveCartridgeInInventory(1))
        return;
    CWeapon::Reload();
    m_sub_state = eSubstateReloadBegin;
    SwitchState(eReload);
}

void CWeaponAutomaticShotgun::OnStateSwitch(u32 S, u32 oldState)
{
    if (!m_bTriStateReload || S != eReload)
    {
        inherited::OnStateSwitch(S, oldState);
        return;
    }
    if (IsMisfire() && iAmmoElapsed)
    {
        switch2_Misfire();
        return;
    }

    CWeapon::OnStateSwitch(S, oldState);

    if (m_magazine.size() == (u32)iMagazineSize || !HaveCartridgeInInventory(1))
    {
        switch2_EndReload();
        m_sub_state = eSubstateReloadEnd;
        return;
    };

    switch (m_sub_state)
    {
    case eSubstateReloadBegin:
        if (HaveCartridgeInInventory(1))
            switch2_StartReload();
        break;
    case eSubstateReloadInProcess:
        if (HaveCartridgeInInventory(1))
            switch2_AddCartgidge();
        break;
    case eSubstateReloadEnd: switch2_EndReload(); break;
    };
}

void CWeaponAutomaticShotgun::switch2_Misfire()
{
    PlaySound("sndReloadMisfire", get_LastFP());
    VERIFY(GetState() == eReload);
    PlayHUDMotion("anm_reload_misfire", false, this, eReload);
    SetPending(true);
}

void CWeaponAutomaticShotgun::switch2_StartReload()
{
    if (iAmmoElapsed == 0 && isHUDAnimationExist("anm_open_empty"))
    {
        PlaySound("sndOpenEmpty", get_LastFP());
    }
    else
    {
        PlaySound("sndOpen", get_LastFP());
    }
    PlayAnimOpenWeapon();
    SetPending(TRUE);
}

void CWeaponAutomaticShotgun::switch2_AddCartgidge()
{
    if (iAmmoElapsed == 0 && isHUDAnimationExist("anm_add_empty"))
    {
        PlaySound("sndAddCartridgeEmpty", get_LastFP());
    }
    else
    {
        PlaySound("sndAddCartridge", get_LastFP());
    }
    PlayAnimAddOneCartridgeWeapon();
    SetPending(TRUE);
}

void CWeaponAutomaticShotgun::switch2_EndReload()
{
    SetPending(FALSE);
    PlaySound("sndClose", get_LastFP());
    PlayAnimCloseWeapon();
}

void CWeaponAutomaticShotgun::PlayAnimOpenWeapon()
{
    VERIFY(GetState() == eReload);
    if (iAmmoElapsed == 0 && isHUDAnimationExist("anm_open_empty"))
        PlayHUDMotion("anm_open_empty", "anim_open_empty", FALSE, this, GetState());
    else
        PlayHUDMotion("anm_open", "anim_open", FALSE, this, GetState());
}
void CWeaponAutomaticShotgun::PlayAnimAddOneCartridgeWeapon()
{
    VERIFY(GetState() == eReload);
    float speed = 1.f;
    CActor* pActor = smart_cast<CActor*>(this->H_Parent());
    if (pActor)
    {
        speed *= pActor->conditions().GetSpeedReloadPerk();
    }
    if (iAmmoElapsed == 0 && isHUDAnimationExist("anm_add_empty"))
        PlayHUDMotion("anm_add_empty", "anim_add_cartridge_empty", FALSE, this, GetState(), speed);
    else
        PlayHUDMotion("anm_add_cartridge", "anim_add_cartridge", FALSE, this, GetState(), speed);
}
void CWeaponAutomaticShotgun::PlayAnimCloseWeapon()
{
    VERIFY(GetState() == eReload);

    if (iAmmoElapsed == 0 && isHUDAnimationExist("anm_close_empty"))
        PlayHUDMotion("anm_close_empty", "anim_close_empty", FALSE, this, GetState());
    else
        PlayHUDMotion("anm_close", "anim_close", FALSE, this, GetState());
}

void CWeaponAutomaticShotgun::PlayAnimShow() 
{
    VERIFY(GetState() == eShowing);

    if (iAmmoElapsed == 0 && isHUDAnimationExist("anm_show_empty"))
        PlayHUDMotion("anm_show_empty", "anim_draw_empty", FALSE, this, GetState());
    else
        inherited::PlayAnimShow();
}

void CWeaponAutomaticShotgun::PlayAnimIdle()
{
    if (TryPlayAnimIdle())
        return;

    if (iAmmoElapsed == 0 && isHUDAnimationExist("anm_idle_empty"))
    {
        if (IsZoomed())
        {
            PlayAnimAim();
        }
        else
        {
            PlayHUDMotion("anm_idle_empty", "anim_empty", TRUE, NULL, GetState());
        }
    }
    else
    {
        inherited::PlayAnimIdle();
    }
}

void CWeaponAutomaticShotgun::PlayAnimIdleMoving()
{
    if (iAmmoElapsed == 0 && isHUDAnimationExist("anm_idle_moving_empty"))
    {
        PlayHUDMotion("anm_idle_moving_empty", "anim_empty", TRUE, NULL, GetState());
    }
    else
    {
        inherited::PlayAnimIdleMoving();
    }
}

void CWeaponAutomaticShotgun::PlayAnimIdleMovingCrouch()
{
    if (iAmmoElapsed == 0 && isHUDAnimationExist("anm_idle_moving_crouch_empty"))
    {
        PlayHUDMotion("anm_idle_moving_crouch_empty", "anim_empty", TRUE, NULL, GetState());
    }
    else
    {
        inherited::PlayAnimIdleMovingCrouch();
    }
}

void CWeaponAutomaticShotgun::PlayAnimIdleSprint()
{
    if (iAmmoElapsed == 0 && isHUDAnimationExist("anm_idle_sprint_empty"))
    {
        PlayHUDMotion("anm_idle_sprint_empty", "anim_empty", TRUE, NULL, GetState());
    }
    else
    {
        inherited::PlayAnimIdleSprint();
    }
}

void CWeaponAutomaticShotgun::PlayAnimHide()
{
    VERIFY(GetState() == eHiding);
    if (iAmmoElapsed == 0 && isHUDAnimationExist("anm_hide_empty"))
    {
        PlaySound("sndClose", get_LastFP());
        PlayHUDMotion("anm_hide_empty", "anim_close", TRUE, this, GetState());
    }
    else
        inherited::PlayAnimHide();
}

void CWeaponAutomaticShotgun::PlayAnimShoot()
{
    VERIFY(GetState() == eFire);
    if (IsZoomed() && isHUDAnimationExist("anm_shots_aim") && isHUDAnimationExist("anm_shot_last_aim"))
    {
        if (iAmmoElapsed > 1)
        {
            PlayHUDMotion("anm_shots_aim", "anim_shoot", FALSE, this, GetState());
        }
        else
        {
            PlayHUDMotion("anm_shot_last_aim", "anim_shot_last", FALSE, this, GetState());
        }
    }
    else
    {
        if (iAmmoElapsed <= 1 && isHUDAnimationExist("anm_shot_last"))
        {
            PlayHUDMotion("anm_shot_last", "anim_shot_last", FALSE, this, GetState());
        }
        else
        {
            PlayHUDMotion("anm_shots", "anim_shoot", FALSE, this, GetState());
        }
    }
}

void CWeaponAutomaticShotgun::PlayAnimBore()
{
    if (iAmmoElapsed == 0 && isHUDAnimationExist("anm_bore_empty"))
        PlayHUDMotion("anm_bore_empty", "anim_empty", TRUE, this, GetState());
    else
        inherited::PlayAnimBore();
}

void CWeaponAutomaticShotgun::PlayAnimAim()
{
    if (iAmmoElapsed == 0 && isHUDAnimationExist("anm_idle_aim_empty"))
    {
        PlayHUDMotion("anm_idle_aim_empty", true, nullptr, GetState());
    }
    else
    {
        inherited::PlayAnimAim();
    }
}

bool CWeaponAutomaticShotgun::HaveCartridgeInInventory(u8 cnt)
{
    if (unlimited_ammo())
        return true;
    if (!m_pInventory)
        return false;

    u32 ac = GetAmmoCount(m_ammoType);
    if (ac < cnt)
    {
        for (u8 i = 0; i < u8(m_ammoTypes.size()); ++i)
        {
            if (m_ammoType == i)
                continue;
            ac += GetAmmoCount(i);
            if (ac >= cnt)
            {
                m_ammoType = i;
                break;
            }
        }
    }
    return ac >= cnt;
}

u8 CWeaponAutomaticShotgun::AddCartridge(u8 cnt)
{
    if (IsMisfire())
        bMisfire = false;

    if (m_set_next_ammoType_on_reload != undefined_ammo_type)
    {
        m_ammoType = m_set_next_ammoType_on_reload;
        m_set_next_ammoType_on_reload = undefined_ammo_type;
    }

    if (!HaveCartridgeInInventory(1))
        return 0;

    m_pCurrentAmmo = smart_cast<CWeaponAmmo*>(m_pInventory->GetAny(m_ammoTypes[m_ammoType].c_str()));
    VERIFY((u32)iAmmoElapsed == m_magazine.size());

    if (m_DefaultCartridge.m_LocalAmmoType != m_ammoType)
        m_DefaultCartridge.Load(m_ammoTypes[m_ammoType].c_str(), m_ammoType);
    CCartridge l_cartridge = m_DefaultCartridge;
    while (cnt)
    {
        if (!unlimited_ammo())
        {
            if (!m_pCurrentAmmo->Get(l_cartridge))
                break;
        }
        --cnt;
        AddCartridgeUpdate();
        l_cartridge.m_LocalAmmoType = m_ammoType;
        m_magazine.push_back(l_cartridge);
        //		m_fCurrentCartirdgeDisp = l_cartridge.m_kDisp;
    }

    VERIFY((u32)iAmmoElapsed == m_magazine.size());

    //выкинуть коробку патронов, если она пустая
    if (m_pCurrentAmmo && !m_pCurrentAmmo->m_boxCurr && OnServer())
        m_pCurrentAmmo->SetDropManual(TRUE);

    return cnt;
}

void CWeaponAutomaticShotgun::AddCartridgeUpdate()
{
    ++iAmmoElapsed;
}

void CWeaponAutomaticShotgun::net_Export(NET_Packet& P)
{
    inherited::net_Export(P);
    P.w_u8(u8(m_magazine.size()));
    for (u32 i = 0; i < m_magazine.size(); i++)
    {
        CCartridge& l_cartridge = *(m_magazine.begin() + i);
        P.w_u8(l_cartridge.m_LocalAmmoType);
    }
}

void CWeaponAutomaticShotgun::net_Import(NET_Packet& P)
{
    inherited::net_Import(P);
    u8 AmmoCount = P.r_u8();
    for (u32 i = 0; i < AmmoCount; i++)
    {
        u8 LocalAmmoType = P.r_u8();
        if (i >= m_magazine.size())
            continue;
        CCartridge& l_cartridge = *(m_magazine.begin() + i);
        if (LocalAmmoType == l_cartridge.m_LocalAmmoType)
            continue;
#ifdef DEBUG
        Msg("! %s reload to %s", *l_cartridge.m_ammoSect, m_ammoTypes[LocalAmmoType].c_str());
#endif
        l_cartridge.Load(m_ammoTypes[LocalAmmoType].c_str(), LocalAmmoType);
    }
}
