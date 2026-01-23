#include "StdAfx.h"
#include "weaponBM16.h"
#include "ActorCondition.h"

CWeaponBM16::~CWeaponBM16() {}
void CWeaponBM16::Load(LPCSTR section)
{
    inherited::Load(section);
    m_sounds.LoadSound(section, "snd_reload_1", "sndReload1", true, m_eSoundShot);
}

void CWeaponBM16::PlayReloadSound()
{
    if (m_magazine.size() == 1)
        PlaySound("sndReload1", get_LastFP());
    else
        PlaySound("sndReload", get_LastFP());
}

void CWeaponBM16::PlayAnimShoot()
{
    if (bDoubleShotMode)
    {
        PlayHUDMotion("anm_shot_1", "anim_shoot_1", FALSE, this, GetState());
        return;
    }
    switch (m_magazine.size())
    {
    case 1: PlayHUDMotion("anm_shot_1", "anim_shoot_1", FALSE, this, GetState()); break;
    case 2: PlayHUDMotion("anm_shot_2", "anim_shoot", FALSE, this, GetState()); break;
    }
}

void CWeaponBM16::PlayAnimShow()
{
    switch (m_magazine.size())
    {
    case 0: PlayHUDMotion("anm_show_0", "anim_draw", TRUE, this, GetState()); break;
    case 1: PlayHUDMotion("anm_show_1", "anim_draw", TRUE, this, GetState()); break;
    case 2: PlayHUDMotion("anm_show_2", "anim_draw", TRUE, this, GetState()); break;
    }
}

void CWeaponBM16::PlayAnimHide()
{
    switch (m_magazine.size())
    {
    case 0: PlayHUDMotion("anm_hide_0", "anim_holster", TRUE, this, GetState()); break;
    case 1: PlayHUDMotion("anm_hide_1", "anim_holster", TRUE, this, GetState()); break;
    case 2: PlayHUDMotion("anm_hide_2", "anim_holster", TRUE, this, GetState()); break;
    }
}

void CWeaponBM16::PlayAnimBore()
{
    switch (m_magazine.size())
    {
    case 0: PlayHUDMotion("anm_bore_0", "anim_idle", TRUE, this, GetState()); break;
    case 1: PlayHUDMotion("anm_bore_1", "anim_idle_1", TRUE, this, GetState()); break;
    case 2: PlayHUDMotion("anm_bore_2", "anim_idle_2", TRUE, this, GetState()); break;
    }
}

void CWeaponBM16::PlayAnimReload()
{
    bool b_both = HaveCartridgeInInventory(2);

    VERIFY(GetState() == eReload);
    float speed = 1.f;
    CActor* pActor = smart_cast<CActor*>(this->H_Parent());
    if (pActor)
    {
        speed *= pActor->conditions().GetSpeedReloadPerk();
    }
    if (bMisfire && iAmmoElapsed && isHUDAnimationExist("anm_reload_misfire_2") && isHUDAnimationExist("anm_reload_misfire_1"))
    {
        if (m_magazine.size() == 1 || !b_both)
            PlayHUDMotion("anm_reload_misfire_1", TRUE, this, GetState(), speed);
        else
            PlayHUDMotion("anm_reload_misfire_2", TRUE, this, GetState(), speed);
    }
    else
    {
        if ((m_magazine.size() == 1 || !b_both) &&
            (m_set_next_ammoType_on_reload == undefined_ammo_type || m_ammoType == m_set_next_ammoType_on_reload))
            PlayHUDMotion("anm_reload_1", "anim_reload_1", TRUE, this, GetState(), speed);
        else
            PlayHUDMotion("anm_reload_2", "anim_reload", TRUE, this, GetState(), speed);
    }
}

void CWeaponBM16::PlayAnimIdleMoving()
{
    switch (m_magazine.size())
    {
    case 0: PlayHUDMotion("anm_idle_moving_0", TRUE, this, GetState()); break;
    case 1: PlayHUDMotion("anm_idle_moving_1", TRUE, this, GetState()); break;
    case 2: PlayHUDMotion("anm_idle_moving_2", TRUE, this, GetState()); break;
    }
}

void CWeaponBM16::PlayAnimIdleMovingCrouch()
{
    switch (m_magazine.size())
    {
    case 0: PlayHUDMotion("anm_idle_moving_crouch_0", TRUE, this, GetState()); break;
    case 1: PlayHUDMotion("anm_idle_moving_crouch_1", TRUE, this, GetState()); break;
    case 2: PlayHUDMotion("anm_idle_moving_crouch_2", TRUE, this, GetState()); break;
    }
}

void CWeaponBM16::PlayAnimIdleSprint()
{
    switch (m_magazine.size())
    {
    case 0: PlayHUDMotion("anm_idle_sprint_0", TRUE, this, GetState()); break;
    case 1: PlayHUDMotion("anm_idle_sprint_1", TRUE, this, GetState()); break;
    case 2: PlayHUDMotion("anm_idle_sprint_2", TRUE, this, GetState()); break;
    }
}

void CWeaponBM16::PlayAnimIdle()
{
    if (TryPlayAnimIdle())
        return;

    if (IsZoomed())
    {
        switch (m_magazine.size())
        {
        case 0: { PlayHUDMotion("anm_idle_aim_0", TRUE, NULL, GetState());
        }
        break;
        case 1: { PlayHUDMotion("anm_idle_aim_1", TRUE, NULL, GetState());
        }
        break;
        case 2: { PlayHUDMotion("anm_idle_aim_2", TRUE, NULL, GetState());
        }
        break;
        };
    }
    else
    {
        switch (m_magazine.size())
        {
        case 0: { PlayHUDMotion("anm_idle_0", TRUE, NULL, GetState());
        }
        break;
        case 1: { PlayHUDMotion("anm_idle_1", TRUE, NULL, GetState());
        }
        break;
        case 2: { PlayHUDMotion("anm_idle_2", TRUE, NULL, GetState());
        }
        break;
        };
    }
}
